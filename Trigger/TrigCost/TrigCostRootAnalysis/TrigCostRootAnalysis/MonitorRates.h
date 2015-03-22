// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorRates_H
#define TrigCostRootAnalysis_MonitorRates_H

// STL include(s):
#include <map>
#include <string>
#include <vector>
#include <iostream>

// Local include(s):
#include "MonitorBase.h"
#include "WriteXML_JSON.h"

// ROOT include(s):
#include <TH1.h>
#include <TCanvas.h>

namespace TrigCostRootAnalysis {

  class CounterBaseRates;
  class CounterRatesUnion;

  /**
   * @class MonitorRates
   */
  class MonitorRates : public MonitorBase {
  
   public:
   
    MonitorRates(const TrigCostData* _costData);
    ~MonitorRates();
    void newEvent(Float_t _weight = 1.);
    CounterBase* newCounter( const std::string &_name, Int_t _ID );
    Bool_t getIfActive(ConfKey_t _mode);
    void saveOutput();

   private:

    void populateCounterMap(CounterMap_t* _counterMap);
    void populateChainItemMaps();
    void saveRateGraphs();
    void saveRuleBookXML();
    void saveXMLElement(std::ofstream& _fout, XMLExport& _xml, CounterBaseRates* _counter);

    void createGlobalCounters(CounterMap_t* _counterMap);
    void createL1Counters(CounterMap_t* _counterMap);
    void createHLTCounters(CounterMap_t* _counterMap);
    void createOverlapCounters(CounterMap_t* _counterMap);

    CounterRatesUnion* m_globalRateHLTCounter;
    CounterRatesUnion* m_globalRateL1Counter;

    ChainItemMap_t m_chainItemsL1;
    ChainItemMap_t m_chainItemsHLT;

    
  }; //class MonitorRates
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorRates_H
