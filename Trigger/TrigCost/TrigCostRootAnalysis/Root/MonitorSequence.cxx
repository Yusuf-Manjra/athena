// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>
#include <utility> //std::pair
#include <iostream>

//Local include(s):
#include "../TrigCostRootAnalysis/MonitorSequence.h"
#include "../TrigCostRootAnalysis/CounterSequence.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"

namespace TrigCostRootAnalysis {

  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorSequence::MonitorSequence(const TrigCostData* _costData) : MonitorBase(_costData, "Sequence") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
  }
  
  /**
   * Process new event for this monitor.
   * Each sequence in the event is looped over and recorded in its counter.
   * @param _weight The event weight.
   */
  void MonitorSequence::newEvent(Float_t _weight) {
    if ( Config::config().debug() ) Info("MonitorSequence::newEvent", "*** Processing Sequences ***");
    
    // Populate the counter collections we'll be using for this event.
    collateCounterCollectionsForEvent( m_costData->getLumi(), TrigConfInterface::getCurrentDBKey() );
    
    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;
      
      // Reset global static counters, this only needs to be called on one event
      startEvent();
      
      // Loop over all sequences.
      for (UInt_t _s = 0; _s < m_costData->getNSequences(); ++_s) {
      
        // Get the name of the chain this sequence instance is in (Supplying L2 or EF helps, but is not needed)
        Int_t _chainID = m_costData->getSequenceChannelCounter(_s);
        const std::string _chainName = TrigConfInterface::getHLTNameFromChainID( _chainID, m_costData->getSequenceLevel(_s) );
        // Are we running over this chain?
        if ( checkPatternNameMonitor( _chainName ) == kFALSE ) continue;
        
        // Get the name of the sequence
        Int_t _seqIndex = m_costData->getSequenceIndex(_s);
        const std::string _seqName = TrigConfInterface::getHLTSeqNameFromIndex( _seqIndex );
        
        // Did we fail to fetch the config?
        if ( _chainName == Config::config().getStr(kBlankString) || _seqName == Config::config().getStr(kBlankString)) {
          Warning("MonitorSequence::newEvent", "Skipping Chain ID %i (%s). Seq ID %i (%s)."
                  " Cannot get Chain or Seq name from current configuration.", _chainID, _chainName.c_str(), _seqIndex, _seqName.c_str());
          continue;
        }
        
        CounterBase* _counter = getCounter( _counterMap, _seqName, _seqIndex );
        _counter->processEventCounter( _s, 0, _weight );
      }
      
      endEvent(_weight);
    }
  }
  
  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorSequence::getIfActive(ConfKey_t _mode) {
    switch(_mode) {
      case kDoAllSummary:       return kTRUE;
      case kDoKeySummary:       return kTRUE;
      case kDoLumiBlockSummary: return kTRUE;
      default: Error("MonitorSequence::getIfActive", "An invalid summary mode was provided (key %s)", Config::config().getName(_mode).c_str() );
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorSequence::saveOutput() {
  
    VariableOptionVector_t _toSavePlots;
    m_filterOutput = kTRUE; // Apply user name filter to output
    //TODO filter on chain name, not seq name here
    sharedHistogramOutputRoutine( _toSavePlots );

    std::vector<TableColumnFormatter> _toSaveTable;
    const std::string _slowText = "Calls > " + intToString( Config::config().getInt(kSlowEventThreshold) ) + " ms";

    _toSaveTable.push_back( TableColumnFormatter("Raw Active Events", 
      "Raw underlying statistics on how many events in which this sequence was executed.",
      kVarCalls, kSavePerEvent, 0, kFormatOptionUseEntries) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Active Events", 
      "How many events in which this sequence was executed.",
      kVarEventsActive, kSavePerEvent, 0) );

    _toSaveTable.push_back( TableColumnFormatter("Calls/Event", 
      "Total number of calls made to this sequence.",
      kVarCalls, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2) );

    _toSaveTable.push_back( TableColumnFormatter(_slowText, 
      "Number of sequence executions which were particularly slow.",
      kVarCallsSlow, kSavePerEvent, 0 ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Event Rate [Hz]", 
      "Rate in this run range of events with at least one execution of this algorithm.",
      kVarEventsActive, kSavePerEvent, 2, kFormatOptionNormaliseWallTime) );

    _toSaveTable.push_back( TableColumnFormatter("Call Rate [Hz]", 
      "Rate in this run range of calls to this sequence.",
      kVarCalls, kSavePerEvent, 2, kFormatOptionNormaliseWallTime) );

    _toSaveTable.push_back( TableColumnFormatter("Seq. Total Time [s]", 
      "Total time for this sequence in this range - CPU + ROS.",
      kVarTime, kSavePerEvent, 2, kFormatOptionMiliSecToSec) );

    _toSaveTable.push_back( TableColumnFormatter("Seq. Total Time [%]", 
      "Total time for this sequence (CPU+ROS) as a percentage of all sequence executions in this run range.",
      &tableFnChainGetTotalFracTime, 2 ) ); // We can re-use the Chain fn here as vars in both called "time"

    _toSaveTable.push_back( TableColumnFormatter("Seq. Total Time/Call [ms]", 
      "Average execution time per sequence call in this run range.",
      kVarTime, kSavePerCall, kVarCalls, kSavePerEvent, 2) ); 

    _toSaveTable.push_back( TableColumnFormatter("Seq. Total Time/Event [ms]", 
      "Average execution time (CPU+ROS) per event for events with at least one execution in this run range.",
      kVarTime, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2) );

    _toSaveTable.push_back( TableColumnFormatter("Run Agls/Event", 
      "Total number of algorithms executed by this sequence.",
      kVarAlgCalls, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("Cached Algs/Event", 
      "Total number of algorithms which supplied a cached result to this sequence.",
      kVarAlgCaches, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("ROS Time/Event [ms]", 
      "Average time waiting for ROS data per event for  events with at least one execution in this run range.",
      kVarROSTime, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("Data Request Rate [Hz]", 
      "Rate of calls to the ROS from this algorithm in this run range.",
      kVarROSCalls, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Cached ROB Rate [Hz]", 
      "Rate of cached ROB fetches from this algorithm in this run range.",
      kVarROBReqs, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Cached ROB Rate [kB/s]", 
      "Average size of cached ROB data fetches for this algorithm in this run range.",
      kVarROBReqSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Retrieved ROB Rate [Hz]", 
      "Rate of ROB retrievals from this algorithm in this run range.",
      kVarROBRets, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Retrieved ROB Rate [kB/s]", 
      "Average size of retrieved ROB data fetches for this algorithm in this run range.",
      kVarROBRetSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    sharedTableOutputRoutine( _toSaveTable );

  }
  
  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorSequence::newCounter( const std::string &_name, Int_t _ID  ) {
    return new CounterSequence( m_costData, _name,  _ID, m_detailLevel );
  }
  
} // namespace TrigCostRootAnalysis
