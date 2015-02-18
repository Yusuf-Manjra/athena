/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// /***************************************************************************
//                           EmTauTriggerCaloTrigger.cxx  -  description
//                              -------------------
//     begin                : Wed Dec 13 2000
//     copyright            : (C) 2000 by moyse
//     email                : moyse@heppch.ph.qmw.ac.uk
//  ***************************************************************************/
//
//    updated: June 20, 2001 M. Wielers
//             move to Storegate
//
//================================================
// EmTauTrigger class Implementation
// ================================================
//
//
//

// Utilities
//#include <cmath>

// This algorithm includes
#include "TrigT1CaloSim/EmTauTrigger.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/CPMTowerCollection.h"
#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/CPMHits_ClassDEF.h"

//#include "StoreGate/DataHandle.h"

namespace LVL1{

using namespace TrigConf;

//--------------------------------
// Constructors and destructors
//--------------------------------

EmTauTrigger::EmTauTrigger
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ), 
      m_vectorOfEmTauROIs(0),
      m_intROIContainer(0),
      m_cpmHitsContainer(0),
      m_storeGate("StoreGateSvc", name), 
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
      m_EmTauTool("LVL1::L1EmTauTools/L1EmTauTools")
{
    m_emTauOutputLocation        = TrigT1CaloDefs::EmTauROILocation;
    m_cpmHitsLocation            = TrigT1CaloDefs::CPMHitsLocation ;
    m_CPMTowerLocation           = TrigT1CaloDefs::CPMTowerLocation;

    // This is how you declare the paramembers to Gaudi so that
    // they can be over-written via the job options file
    
    declareProperty("EventStore",m_storeGate,"StoreGate Service");
    declareProperty(  "EmTauROILocation",       m_emTauOutputLocation );
    declareProperty( "CPMHitsLocation",         m_cpmHitsLocation );
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    declareProperty( "CPMTowerLocation", m_CPMTowerLocation ) ;

}

// Destructor
EmTauTrigger::~EmTauTrigger() {
  ATH_MSG_INFO( "Destructor called" );
}


//---------------------------------
// initialise()
//---------------------------------

StatusCode EmTauTrigger::initialize()
{

  // We must here instantiate items which can only be made after
  // any job options have been set
  int outputLevel = msgSvc()->outputLevel( name() );

     //
    // Connect to the LVL1ConfigSvc for the trigger configuration:
    //
  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't connect to " << m_configSvc.typeAndName() );
    return sc;
  } else if (outputLevel <= MSG::DEBUG) {
    ATH_MSG_DEBUG( "Connected to " << m_configSvc.typeAndName() );
  }

  // Now connect to the StoreGateSvc

  sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't connect to " << m_storeGate.typeAndName() 
        );
    return sc;
  } else if (outputLevel <= MSG::DEBUG) {
    ATH_MSG_DEBUG( "Connected to " << m_storeGate.typeAndName() 
        );
  }

  // Retrieve L1EmTauTool
  sc = m_EmTauTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Problem retrieving EmTauTool. Abort execution" );
    return StatusCode::SUCCESS;
  }
    
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Optional debug of menu at start of run
//-------------------------------------------------

StatusCode EmTauTrigger::beginRun()
{

  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) setupTriggerMenuFromCTP();

  return StatusCode::SUCCESS ;
}



//---------------------------------
// finalise()
//---------------------------------

StatusCode EmTauTrigger::finalize()
{
   ATH_MSG_INFO( "Finalizing" );
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode EmTauTrigger::execute( )
{

  //make a message logging stream

  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "starting emTauTrigger" ); 

  // Create containers for this event
  m_intROIContainer   =new DataVector<CPAlgorithm>;       //Container of ROIs
  m_vectorOfEmTauROIs =new DataVector<EmTauROI>;          //Container to hold EmTauRois in TES.
  m_cpmHitsContainer = new std::map<int, CPMHits *>;      // Map to hold CPM Hits

  // Retrieve the CPMTowerContainer
  if (m_storeGate->contains<CPMTowerCollection>(m_CPMTowerLocation)) {
    const DataVector<CPMTower>* storedTTs;
    StatusCode sc = m_storeGate->retrieve(storedTTs, m_CPMTowerLocation);  
    if ( sc==StatusCode::SUCCESS ) {
       // Check size of CPMTowerCollection - zero would indicate a problem
      if (storedTTs->size() == 0)
         ATH_MSG_WARNING( "Empty CPMTowerContainer - looks like a problem" );
  
      // Use L1EmTauTools to produce DataVector of RoIs passing trigger conditions
      m_EmTauTool->findRoIs(storedTTs, m_intROIContainer);

      // Form external RoIs
      DataVector<CPAlgorithm>::iterator intROI_it = m_intROIContainer->begin();
      for ( ; intROI_it!=m_intROIContainer->end() ; intROI_it++){
        EmTauROI* test = (*intROI_it)->produceExternal();
        if (outputLevel <= MSG::DEBUG)
            ATH_MSG_DEBUG( "*****InternalROI passes trigger : created an external ROI with ROI word "
            << std::hex << test->roiWord() << std::dec);
        m_vectorOfEmTauROIs->push_back( test );
      }//end for loop
    }
    else ATH_MSG_WARNING( "Error retrieving CPMTowers" );
  }
  else ATH_MSG_WARNING( "No CPMTowerContainer at " << m_CPMTowerLocation );
  
  // Save external RoIs in TES
  saveExternalROIs();

  // Form CPMHits for ByteStream simulation
  formCPMHits();
  
  // Clean up at end of event
  delete m_intROIContainer;
  delete m_cpmHitsContainer;
  m_intROIContainer=0;
  m_cpmHitsContainer=0;
  m_vectorOfEmTauROIs=0;

  return StatusCode::SUCCESS ;
}


/** place final ROI objects in the TES. */
void LVL1::EmTauTrigger::saveExternalROIs() {

  int outputLevel = msgSvc()->outputLevel( name() );
    
  StatusCode sc = m_storeGate->overwrite(m_vectorOfEmTauROIs, m_emTauOutputLocation,true,false,false);

  if (sc.isSuccess()) {
     if (outputLevel <= MSG::VERBOSE)
         ATH_MSG_VERBOSE("Stored " <<m_vectorOfEmTauROIs->size()
             << " EmTauROls at " << m_emTauOutputLocation );
  }
  else {
     ATH_MSG_ERROR( "failed to write EmTauROIs to  "
         << m_emTauOutputLocation );
  } 

  return;

} //end saveExternalROIs


/** Store CPM energy sums for bytestream simulation */
void LVL1::EmTauTrigger::formCPMHits() {
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "formCPMHits executing" );
  
  ClusterProcessorModuleKey testKey;
  CoordToHardware conv;
  DataVector<LVL1::EmTauROI>::const_iterator it;
  
  for (it=m_vectorOfEmTauROIs->begin(); it!=m_vectorOfEmTauROIs->end(); ++it){
    /** RoI coordinates are offset from TT by 0.5*dimension. Correct for this to
        avoid rounding errors in CoordToHardware */
    double eta = (*it)->eta() - 0.05;
    double phi = (*it)->phi() - 0.05;
    Coordinate coord = Coordinate(phi, eta);
    unsigned int crate = conv.cpCrate(coord);
    unsigned int module = conv.cpModule(coord);
    unsigned int key = testKey.cpmKey(crate, module);
    if (outputLevel <= MSG::DEBUG) {
      ATH_MSG_DEBUG( "Found RoI with (eta, phi) = ("
      << (*it)->eta() << ", " << (*it)->phi() << ") " << ", RoIWord = "
      << std::hex << (*it)->roiWord() << std::dec );
      ATH_MSG_DEBUG( "Crate = " << crate << ", Module = " << module << 
      ", CPM key = " << key );
    }
    CPMHits* cpmHits=0;
   // find whether corresponding CPMHits already exists
    std::map<int, CPMHits*>::iterator test=m_cpmHitsContainer->find(key);
    // if not, create it
    if ( test==m_cpmHitsContainer->end()){
      if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "New key. CPM has crate = " 
                                                << crate << ", Module = " 
						<< module );
      if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "Create new CPMHits" ); 
      cpmHits = new CPMHits(crate, module);
      
      if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "and insert into map" ); 
      m_cpmHitsContainer->insert(std::map<int,CPMHits*>::value_type(key,cpmHits));
    }
    else {
      if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "Existing CPMHits" ); 
      cpmHits = test->second; // Already exists, so set pointer
    }
    // increment hit multiplicity. Word format different for FCAL CPMs
    if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "Update CPM hits" ); 
    unsigned int hits0 = cpmHits->HitWord0();
    unsigned int hits1 = cpmHits->HitWord1();
    hits0 = addHits(hits0,(*it)->roiWord()&0xFF);
    hits1 = addHits(hits1,((*it)->roiWord()&0xFF00)>>8);
    std::vector<unsigned int> hitvec0; // CPMHits stored as vectors, to allow for multi-slice readout
    std::vector<unsigned int> hitvec1; 
    hitvec0.push_back(hits0);
    hitvec1.push_back(hits1);
    cpmHits->addHits(hitvec0,hitvec1);
    if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( "All done for this one" ); 
  }
  
  // Now add them to the TES
  saveCPMHits();
  
  return;
}

/** put CPMHits into SG */
void LVL1::EmTauTrigger::saveCPMHits(){
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG("saveCPMHits running");
  
  CPMHitsCollection* CPMHvector = new  CPMHitsCollection;

  std::map<int, CPMHits*>::iterator it;
  for( it=m_cpmHitsContainer->begin(); it!=m_cpmHitsContainer->end(); ++it ){
     CPMHvector->push_back(it->second);
  }

  if (outputLevel <= MSG::DEBUG) ATH_MSG_DEBUG( m_cpmHitsContainer->size()<<" CPMHits have been saved");
  StatusCode sc = m_storeGate->overwrite(CPMHvector, m_cpmHitsLocation,true,false,false);
  if (sc != StatusCode::SUCCESS) {
    ATH_MSG_ERROR("Error registering CPMHits collection in TES ");
  }
  
  return;
}

/** Increment CPM hit word */
unsigned int LVL1::EmTauTrigger::addHits(unsigned int hitMult, unsigned int hitVec) {

  int outputLevel = msgSvc()->outputLevel( name() );
  
  if (outputLevel <= MSG::DEBUG) 
     ATH_MSG_DEBUG("addHits: Original hitMult = " << std::hex << hitMult
         << ". Add hitWord = " << hitVec << std::dec);
	 
// Results transmitted in 2 words, each reporting half of the CP thresholds
  int nthresh = TrigT1CaloDefs::numOfCPThresholds/2;
  
  int nbits = 24/nthresh;  // 24 bits of results per word
  
  if (outputLevel <= MSG::DEBUG) 
     ATH_MSG_DEBUG(" Bits per threshold = " << nbits );
  
  int max = (1<<nbits) - 1;
  unsigned int multMask = max;
  unsigned int hitMask = 1;
  unsigned int shift = 0;
  
  unsigned int hits = 0;
    
  for (int i = 0; i < nthresh; i++) {
    int mult = (hitMult&multMask) + (hitVec&hitMask);
    mult = ( (mult<=max) ? mult : max);
    hits += (mult<<shift);
    
    hitMult >>= nbits;
    hitVec  >>= 1;
    shift += nbits;
  }
  
  if (outputLevel <= MSG::DEBUG) 
     ATH_MSG_DEBUG("addHits returning hitMult = " << std::hex << hits << std::dec );
  
  return hits;
}

} // end of LVL1 namespace bracket


/** retrieves the Calo config put into detectorstore by TrigT1CTP and set up trigger menu */
void LVL1::EmTauTrigger::setupTriggerMenuFromCTP(){
  
  L1DataDef def;

  std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.emType() || (*it)->type() == def.tauType() ) {
      ATH_MSG_DEBUG( "TriggerThreshold " << (*it)->id() << " has name " << (*it)->name() << endreq
          << "  threshold number " << (*it)->thresholdNumber() << endreq
          << "  number of values = " << (*it)->numberofValues() );
      for (std::vector<TriggerThresholdValue*>::const_iterator tv = (*it)->thresholdValueVector().begin();
           tv != (*it)->thresholdValueVector().end(); ++tv) {
        ClusterThresholdValue* ctv;
        ctv = dynamic_cast<ClusterThresholdValue*> (*tv);
	if (!ctv) {
          ATH_MSG_ERROR( "Threshold type name is EM/Tau, but is not a ClusterThreshold object!" );
          continue;
        }
        ATH_MSG_DEBUG( "ClusterThresholdValue: " << endreq
            << "  Threshold value = " << ctv->thresholdValueCount() << endreq
            << "  EM isolation = " << ctv->emIsolationCount() << endreq
            << "  Had isolation = " << ctv->hadIsolationCount() << endreq
            << "  Had veto = " << ctv->hadVetoCount() << endreq
            << "  EtaMin = " << ctv->etamin() << ", EtaMax = " << ctv->etamax() );
        
      } // end of loop over threshold values
    } //  is type == em or tau?
  } // end of loop over thresholds
    
}


