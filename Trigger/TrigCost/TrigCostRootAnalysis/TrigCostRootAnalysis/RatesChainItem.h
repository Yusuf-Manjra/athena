/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Dear emacs, this is -*- c++ -*-
// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_RatesChainItem_H
#define TrigCostRootAnalysis_RatesChainItem_H

// STL include(s):
#include <set>
#include <string>

// Local include(s):
#include "Utility.h"

// ROOT includes
#include <TRandom3.h>

namespace TrigCostRootAnalysis {

  //Forward declaration
  class CounterBaseRates;
  
  /**
   * @class RatesChainItem
   * One instance is needed for each L1 and each HLT item in the menu.
   * HLT items are connected to the L1 items which seed them.
   * The object is loaded with its prescale and (per event) if it passed raw.
   * They is used by single-chain and combination-chain rates counters to calculate the correct weight per event. 
   */
  class RatesChainItem {
   public:

    RatesChainItem(std::string _name, Int_t _level, Double_t _PS);

    void addLower(RatesChainItem* _lower);
    void addUpper(RatesChainItem* _upper);
    void addCounter(CounterBaseRates* _client);
    ChainItemSetIt_t getLowerStart();
    ChainItemSetIt_t getLowerEnd();
    ChainItemSet_t& getLower();
    ChainItemSetIt_t getUpperStart();
    ChainItemSetIt_t getUpperEnd();
    ChainItemSet_t& getUpper();
    Bool_t getLowerContains(RatesChainItem* _find);
    Bool_t getLowerContainsAll( std::set<RatesChainItem*>& _set );
    Bool_t getUpperContains(RatesChainItem* _find);
    Bool_t getUpperContainsAll( std::set<RatesChainItem*>& _set );
    void setExtraEfficiency(Double_t _extraEfficiency);
    void setRateReductionFactor(Double_t _reductionFactor);

    void beginEvent(Bool_t _passRaw, CounterBaseRatesSet_t& _counterSet);
    void endEvent();
    void newRandomPS();

    void classifyBunchGroup();
    void classifyRandom();

    Bool_t getInEvent();
    Bool_t getPassRaw();
    Bool_t getPassPS();
    // Bool_t getIsNotPhysics();
    Double_t getPSWeight();
    Double_t getPS();
    Double_t getPassRawOverPS();
    Bool_t getPassRawAndPS();
    const std::string& getName();
    UInt_t getID();

   private:
   
    std::string        m_name; //!< This chain item's name, more for debug
    Int_t              m_level; //!> Which level this item's at
    Double_t           m_PS; //!< The prescale to be applied to this item
    Double_t           m_PSWeight; //!< The equivalent PS weight, = 1/PS
    Double_t           m_extraEfficiency; //<! Extra factor which can be supplied to scale the rate of this triggeer
    TRandom3           m_R; //!< Random number generator for when applying the PS directly.
    UInt_t             m_ID; //!< ID number, sequential, used for random seed
    static UInt_t      s_chainCount; //!< Static counter of how many RatesChainItems have been made, used as random seed.
    EBBunchGroupType_t m_bunchGroupType; //!< If L1 chain, what bunch group I have been identified to. Used with EB weighting.

    Bool_t        m_passRaw; //!< If this chain item passed in this event
    Bool_t        m_passPS; //!< If this chain item passed just the PS check in this event
    Bool_t        m_inEvent; //!< If this chain item was at least run in this event.
    Bool_t        m_forcePassRaw; //!< A debug setting, when true all chains will always accept. Allow investigation of PS weighting.
    Bool_t        m_doEBWeighting; //!< If enhanced bias weights are to be applied
    Bool_t        m_doDirectPS; //<! True if calculating the direct application of prescales
    Bool_t        m_matchRandomToOnline; //!< Butter setting, if true and I am a L1_RDX trigger then I should online fire when online unbiased fired
    Bool_t        m_iAmRandom; //!< True if I am a L1 item of type L1_RDX

    ChainItemSet_t m_lower; //!< Pointers to seeding chains from lower levels
    ChainItemSet_t m_upper; //!< Pointers to seeding chains from lower levels

    CounterBaseRatesSet_t m_clients; //!< Set of pointers to client rates counters which use this chain item.


  }; // Class RatesChainItem

  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_RatesChainItem_H


