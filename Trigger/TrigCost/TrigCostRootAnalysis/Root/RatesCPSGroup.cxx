  // Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// Local include(s):
#include "../TrigCostRootAnalysis/RatesChainItem.h"
#include "../TrigCostRootAnalysis/CounterBaseRates.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/RatesCPSGroup.h"

// ROOT includes
#include <TError.h>

namespace TrigCostRootAnalysis {

  /**
   * Construct new RatesCPSGroup to keep these chains together.
   */
  RatesCPSGroup::RatesCPSGroup(std::string _name) :
    m_name(_name),
    m_commonPS(1.),
    m_commonPSWeight(1.),
    m_l1(nullptr)
  {}
    
  /**
   * Add a item to this CPS group. All items added will be weighted in a coherent way
   * @param _item A HLT chain item which is part of this CPS group
   */
  void RatesCPSGroup::add(RatesChainItem* _item) {
    if (m_items.count(_item) == 1) return;
    m_items.insert( _item );
  }

  /**
   * For combinations which are under a coherent prescale. A common prescale factor needs to be factored out.
   * This is applied coherently to the set of chains.
   */
  void RatesCPSGroup::calculateCPSFactor() {
        // We expect more than one L2
    if (m_items.size() < 2) {
      Error("RatesCPSGroup::calculateCPSFactor", "Expect two or more HLT chains to do CPS for %s", getName().c_str());
      return;
    }
    // We can only do this if all of our chains have the same L1 seed
    for (const auto _item : m_items) {
      if (_item->getLower().size() != 1) {
        Error("RatesCPSGroup::calculateCPSFactor", "Cannot factor out CPS for %s, there is not exactly one L1 seed", getName().c_str());
        return;
      }
      ChainItemSetIt_t _it = _item->getLowerStart(); // We know there is only one
      if (m_l1 == nullptr) m_l1 = (*_it);
      else if ( (*_it) != m_l1) {
        Error("RatesCPSGroup::calculateCPSFactor", "Cannot factor out CPS for %s, HLT chains have different seeds", getName().c_str());
        return;
      }
    }

    Double_t _lowestPS = 1e10;
    // Find lowest PS
    for (const auto _item : m_items) if (_item->getPS() < _lowestPS) _lowestPS = _item->getPS();

    if (_lowestPS <= 0.)  { // Disabled
      m_commonPSWeight = 0.;
      m_commonPS = -1;
      for (const auto _item : m_items) _item->setPSReduced( -1 );
    } else {
    // Set reduced PS
      m_commonPS = _lowestPS;
      m_commonPSWeight = 1. / m_commonPS; // Extra weight to apply coherently
      for (const auto _item : m_items) _item->setPSReduced( _item->getPS() / _lowestPS );
    }
    //Info("RatesCPSGroup::calculateCPSFactor", "Debug, group %s has CPS weight %f", getName().c_str(), (Float_t) m_commonPSWeight);
  }

  Double_t RatesCPSGroup::getCommonWeight() {
    return m_commonPSWeight;
  }

  RatesChainItem* RatesCPSGroup::getL1() {
    return m_l1;
  }

  ChainItemSetIt_t RatesCPSGroup::getChainStart() {
    return m_items.begin();
  }

  ChainItemSetIt_t RatesCPSGroup::getChainEnd() {
    return m_items.end();
  }

  /**
   * @return The CPS groups name
   */
  const std::string& RatesCPSGroup::getName() {
    return m_name;
  }

}