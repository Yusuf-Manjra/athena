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

// ROOT includes
#include <TError.h>

namespace TrigCostRootAnalysis {

  UInt_t RatesChainItem::s_chainCount = 0; // Each ChainItem gets a sequential random seed

  /**
   * Construct new RatesChainItem with a given prescale.
   */
  RatesChainItem::RatesChainItem(std::string _name, Int_t _level, Double_t _PS) :
    m_name(_name),
    m_level(_level),
    m_PS(_PS), // Integer prescale
    m_PSWeight(1./m_PS), // Reciprocal of the prescale - this is the basic weight quantity for this ChainItem
    m_extraEfficiency(1.),
    m_R( ++s_chainCount ),
    m_ID( s_chainCount ),
    m_bunchGroupType(kBG_UNSET),
    m_passRaw(kFALSE),
    m_passPS(kFALSE),
    m_inEvent(kFALSE),
    m_doDirectPS(kFALSE),
    m_matchRandomToOnline(kFALSE),
    m_iAmRandom(kFALSE)
  {
    if (Config::config().debug()) {
      Info("RatesChainItem::RatesChainItem","New ChainItem:%s, Level:%i PS:%f", m_name.c_str(), m_level, m_PS);
    }

    m_doEBWeighting = Config::config().getInt(kDoEBWeighting); // Cache for speed
    m_doDirectPS = Config::config().getInt(kDirectlyApplyPrescales); // Cache for speed

    // If L1: then classify my bunchgroup
    if (m_level == 1) {
      classifyBunchGroup();
    }
    // Classify if I am random, or seed from random
    classifyRandom();

    if (m_PS <= 0.) m_PSWeight = 0.;
    m_forcePassRaw = (Bool_t) Config::config().getInt(kRatesForcePass);
    m_matchRandomToOnline = (Bool_t) Config::config().getInt(kMatchL1RandomToOnline);
  }

  /**
   * Look at myself and classify from their name what type of BG I trigger on
   */
  void RatesChainItem::classifyBunchGroup() {

    if (getName().find("_BPTX") != std::string::npos || getName().find("_BGRP") != std::string::npos) { // Ignore the beam pickup triggers
      m_bunchGroupType = kBG_NONE;
    } else if (getName().find("_FIRSTEMPTY") != std::string::npos) {
      m_bunchGroupType = kBG_FIRSTEMPTY;
    } else if (getName().find("_EMPTY") != std::string::npos) {
      m_bunchGroupType = kBG_EMPTY;
    } else if (getName().find("_UNPAIRED_ISO") != std::string::npos) {
      m_bunchGroupType = kBG_UNPAIRED_ISO;
    } else if (getName().find("_UNPAIRED_NONISO") != std::string::npos) {
      m_bunchGroupType = kBG_UNPAIRED_NONISO;
    } else {
      m_bunchGroupType = kBG_FILLED;
    }

    if (Config::config().debug()) {
      Info("RatesChainItem::classifyBunchGroup","Item %s classified as %s.",
        getName().c_str(), BunchGroupNameStr[m_bunchGroupType].c_str() );
    }

  }

  /**
   * Look at myself and classify if I am a random seeded L1
   */
  void RatesChainItem::classifyRandom() {

    if ( getName().find("_RD0") != std::string::npos ||
         getName().find("_RD1") != std::string::npos ||
         getName().find("_RD2") != std::string::npos ||
         getName().find("_RD3") != std::string::npos ||
         getName().find("_L1RD0") != std::string::npos ||
         getName().find("_L1RD1") != std::string::npos ||
         getName().find("_L1RD2") != std::string::npos ||
         getName().find("_L1RD3") != std::string::npos ) {
      m_iAmRandom = kTRUE;
      if (Config::config().debug()) Info("RatesChainItem::classifyRandom","Item %s classified as random", getName().c_str());
    } else {
      m_iAmRandom = kFALSE;
    }

  }

  /**
   * User can supply additional scaling factors which will alter the effective efficiency of this chain and hence the rate
   * @param _extraEfficiency Additional scaling factor which will scale the rate when the item fires
   */
  void RatesChainItem::setExtraEfficiency(Double_t _extraEfficiency) {
    m_extraEfficiency = _extraEfficiency;
  }

  /**
   * Equiv to reciprocal of @see RatesChainItem::setExtraEfficiency
   * @param _reductionFactor Scale rate down by this factor
   */
  void RatesChainItem::setRateReductionFactor(Double_t _reductionFactor) {
    m_extraEfficiency = 1. / _reductionFactor;
  }


  /**
   * For HLT items, each seeding L1 item should be linked here by passing its pointer.
   * Note we do not own the lower chainItem
   * @param _lower The pointer to another RatesChainItem which seeds this instance.
   */
  void RatesChainItem::addLower(RatesChainItem* _lower) {
    m_lower.insert(_lower);
  }

  /**
   * For L1 items a link to any HLT chanins seeded should be added here
   * @param _lower The pointer to another RatesChainItem which is seeded by this instance.
   */
  void RatesChainItem::addUpper(RatesChainItem* _upper) {
    m_upper.insert(_upper);
  }

  /**
   * Registers that a rates counter makes use of this ChainItem. We can use this info to speed up
   * execution by only processing the counters which we need to.
   * Note we do not own the CounterRates object
   * @param _clinet The pointer to a CounterRates object which makes use of this ChainItem to calculate the event weight.
   */
  void RatesChainItem::addCounter(CounterBaseRates* _client) {
    m_clients.insert( _client );
  }

  /**
   * @return A const iterator to the start of this counter's set of seeding counters
   */
  ChainItemSetIt_t RatesChainItem::getLowerStart() {
    return m_lower.begin();
  }

  /**
   * @return A const iterator to the end of this counter's set of seeding counters
   */
  ChainItemSetIt_t RatesChainItem::getLowerEnd() {
    return m_lower.end();
  }

  /**
   * @return A reference to the set of lower, seeding, items of this item
   */
  ChainItemSet_t& RatesChainItem::getLower() {
    return m_lower;
  }

  /**
   * @return A const iterator to the start of this counter's set of seeded counters
   */
  ChainItemSetIt_t RatesChainItem::getUpperStart() {
    return m_upper.begin();
  }

  /**
   * @return A const iterator to the end of this counter's set of seeded counters
   */
  ChainItemSetIt_t RatesChainItem::getUpperEnd() {
    return m_upper.end();
  }

  /**
   * @return A reference to the set of seeded items of this item
   */
  ChainItemSet_t& RatesChainItem::getUpper() {
    return m_upper;
  }

  /**
   * @param _find A chain item pointer to find in this chain item's set of seeding triggers.
   * @return kTRUE if this ChainItem has the supplied ChainItem listed as one of its lower, seeding items.
   */
  Bool_t RatesChainItem::getLowerContains(RatesChainItem* _find) {
    return static_cast<Bool_t>( m_lower.count( _find ) );
  }

  /**
   * @param _set Reference to a set of chain item pointers to test against.
   * @return kTRUE if *all* of the ChainItems supplied in _set are also listed as lower items of this ChainItem
   */
  Bool_t RatesChainItem::getLowerContainsAll( std::set<RatesChainItem*>& _set ) {
    for (ChainItemSetIt_t _it = _set.begin(); _it != _set.end(); ++_it) { // Check we contain all these
      if (getLowerContains( (*_it) ) == kFALSE) return kFALSE;
    }
    return kTRUE;
  }

  /**
   * @param _find A chain item pointer to find in this chain item's set of seeded triggers.
   * @return kTRUE if this ChainItem has the supplied ChainItem listed as one of its upper, seeded items.
   */
  Bool_t RatesChainItem::getUpperContains(RatesChainItem* _find) {
    return static_cast<Bool_t>( m_upper.count( _find ) );
  }

  /**
   * @param _set Reference to a set of chain item pointers to test against.
   * @return kTRUE if *all* of the ChainItems supplied in _set are also listed as upper items of this ChainItem
   */
  Bool_t RatesChainItem::getUpperContainsAll( std::set<RatesChainItem*>& _set ) {
    for (ChainItemSetIt_t _it = _set.begin(); _it != _set.end(); ++_it) { // Check we contain all these
      if (getUpperContains( (*_it) ) == kFALSE) return kFALSE;
    }
    return kTRUE;
  }

  /**
   * @return The configured prescale value
   */
  Double_t RatesChainItem::getPS() {
    return m_PS;
  }

  /**
   * @return The chain item's name
   */
  const std::string& RatesChainItem::getName() {
    return m_name;
  }

  /**
   * @return The chain sequential internal ID
   */
  UInt_t RatesChainItem::getID() {
    return m_ID;
  }

  /**
   * @param _passRaw If this ChainItem passed raw in this event.
   * @param _counterSet Set of counters we will process, add to it counters that I influence. This is pass-by-reference and is modified.
   */
  void RatesChainItem::beginEvent(Bool_t _passRaw,  CounterBaseRatesSet_t& _counterSet) {
    m_passRaw = _passRaw;
    m_inEvent = kTRUE;
    _counterSet.insert( m_clients.begin(), m_clients.end() );

    if (m_doDirectPS) newRandomPS(); //TODO - check this is only used for DirectPS application. Saves many calls to TRandom3

    // For random seeded triggers where the HLT was re-run, we need to check that we only run over unbiased events in the sample
    if (m_matchRandomToOnline == kTRUE && m_iAmRandom == kTRUE) {
      if ( Config::config().getInt(kCurrentEventWasRandomOnline) == kFALSE ) {
        m_passRaw = kFALSE;
        m_inEvent = kFALSE;
      }
    }

  }

  /**
   * Reset all flags to zero
   */
  void RatesChainItem::endEvent() {
    m_passRaw = kFALSE;
    m_passPS = kFALSE;
    m_inEvent = kFALSE;
  }

  /**
   * Update the random prescale to a new value
   */
  void RatesChainItem::newRandomPS() {
    if (m_PS <= 0.) {
      m_passPS = kFALSE;
    } else {
      m_passPS = (m_R.Rndm() < m_PSWeight);
    }
  }

  /**
   * @return If this chain item was executed this event, regardless of whether or not it passed PS or passed Raw.
   * For L1 items, this means we need to check the bunch group
   */
  Bool_t RatesChainItem::getInEvent() {
    if (m_level == 1) {
      // If this L1 item passed then it must have been in the event, we only store L1s which pass so return this.
      // Also, if doEBWeighting is false we do not know what the bunch group was online
      if (m_inEvent == kTRUE || m_doEBWeighting == kFALSE) return m_inEvent;
      // Otherwise we check the bunchgroup
      return (Config::config().getInt(kCurrentEventBunchGroupID) == m_bunchGroupType);
    } else { // HLT
      return m_inEvent;
    }
  }

  /**
   * @return If this chain item passed (raw) in this event.
   */
  Bool_t RatesChainItem::getPassRaw() {
    if (m_forcePassRaw == kTRUE) return kTRUE;
    return m_passRaw;
  }

  /**
   * @return If this chain item passed its prescale in this event.
   */
  Bool_t RatesChainItem::getPassPS() {
    return m_passPS;
  }

  /**
   * @return 1/Prescale weighting factor for this event. This is scaled by an optional user supplied extra efficiency factor which can modulate the rate
   */
  Double_t RatesChainItem::getPSWeight() {
    return m_PSWeight * m_extraEfficiency;
  }

  /**
   * @return Zero if this chain did not pass raw, else returns 1/Prescale
   */
  Double_t RatesChainItem::getPassRawOverPS() {
    if (getPassRaw() == kFALSE) return 0.;
    return getPSWeight();
  }

  /**
   * @return If this chain passed both Raw and PS
   */
  Bool_t RatesChainItem::getPassRawAndPS() {
    return (getPassRaw() && getPassPS());
  }

} // namespace TrigCostRootAnalysis
