// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <assert.h>

// ROOT include(s):
#include <TError.h>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterRatesChain.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/RatesChainItem.h"

namespace TrigCostRootAnalysis {

  /**
   * Counter to monitor the rates of a single chain.
   * @param _costData Const pointer to the data store, not used by this counter at the moment.
   * @param _name Const ref to chain's name
   * @param _ID Chain's ID number.
   */
  CounterRatesChain::CounterRatesChain( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel, MonitorBase* _parent ) :
    CounterBaseRates(_costData, _name, _ID, _detailLevel, _parent) {
  }

  /**
   * Counter destructor. Nothing currently to delete.
   */
  CounterRatesChain::~CounterRatesChain() {
  }

  /**
   * Return if this chain passes the HLT and at least one L1 by checking both pass-raw and pass-prescale bits.
   * This is the naive method.
   * @param _usePrescale - if set to kTRUE (default) then the prescale will be simulated, otherwise the prescale is taken to be 1.
   * @return 1 if the chain passes, 0 if not.
   */
  Float_t CounterRatesChain::runDirect(Bool_t _usePrescale) {
    assert( m_L2s.size() == 1 || m_L1s.size() == 1); // We should only be one chain
    if (m_L2s.size() == 1) { // A HLT Chain

      RatesChainItem* _L2 = (*m_L2s.begin());
      // L2 must pass raw and pass PS
      if (_L2->getPassRaw() == kFALSE) return 0.;
      if (_usePrescale == kTRUE && _L2->getPassPS() == kFALSE) return 0.;

      // At least one L1 seed must pass Raw and pass PS
      for (ChainItemSetIt_t _lowerIt = _L2->getLowerStart(); _lowerIt != _L2->getLowerEnd(); ++_lowerIt) {
        RatesChainItem* _L1 = (*_lowerIt);

        if ( _L1->getPassRaw() == kFALSE ) continue;
        if ( _usePrescale == kTRUE && _L1->getPassPS() == kFALSE) continue;
        return 1; // This chain and lower chain have both passed raw and passed PS

      }
      return 0.;

    } else { // m_L1s.size() == 1

      RatesChainItem* _L1 = (*m_L1s.begin());
      // L1 must pass raw and pass PS
      if (_L1->getPassRaw() == kFALSE) return 0.;
      if (_usePrescale == kTRUE && _L1->getPassPS() == kFALSE) return 0.;
      return 1;

    }
  }

  /**
   * If this chain passes-raw at both the HLT and L1 levels, then calculate the effective weight from all prescale factors.
   * @return Event prescale weight for this chain 0 < PS weight < 1
   */
  Double_t CounterRatesChain::runWeight(Bool_t _includeExpress) {
    // This is a sub-case of the ChainOR, however here we know we only have one chain at L2.
    // However, this chain may have been seeded by many L1's.

    // See Eq 33 from http://arxiv.org/abs/0901.4118

    assert( m_L2s.size() == 1 || m_L1s.size() == 1); // We should only be one chain
    m_cachedWeight = 0.;

    if (m_L2s.size() == 1) { // A HLT Chain

      RatesChainItem* _L2 = (*m_L2s.begin());

      // First we check that the one L2 passed
      if (_L2->getPassRaw() == kFALSE) return 0.;

      Double_t _L1Weight = 1.;
      for (ChainItemSetIt_t _lowerIt = _L2->getLowerStart(); _lowerIt != _L2->getLowerEnd(); ++_lowerIt) {
        RatesChainItem* _L1 = (*_lowerIt);
        _L1Weight *= ( 1. - _L1->getPassRawOverPS() );
      }

      m_eventLumiExtrapolation = _L2->getLumiExtrapolationFactor(m_costData->getLumi(), m_disableEventLumiExtrapolation);
      //if (getName() == "HLT_cscmon_L1All") Info("DEBUG", "WL1:%f, NL1:%s, 1-L1: %f, HLT:%f, total: %f, lumi%f", m_lowerRates->getLastWeight(), m_lowerRates->getName().c_str(),  1. - _L1Weight, _L2->getPSWeight(_includeExpress), _L2->getPSWeight(_includeExpress) * ( 1. - _L1Weight ),  _L2->getLumiExtrapolationFactor()  );
      m_cachedWeight = _L2->getPSWeight(_includeExpress) * ( 1. - _L1Weight );

    } else { // A L1Chain

      RatesChainItem* _L1 = (*m_L1s.begin());
      m_eventLumiExtrapolation = _L1->getLumiExtrapolationFactor(m_costData->getLumi(), m_disableEventLumiExtrapolation);
      m_cachedWeight = _L1->getPassRawOverPS();

    }
    return m_cachedWeight;
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterRatesChain::debug(UInt_t _e) {
    UNUSED(_e);
  }

} // namespace TrigCostRootAnalysis
