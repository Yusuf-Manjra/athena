// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <assert.h>
#include <bitset>

// ROOT include(s):
#include <TMath.h>
#include <TError.h>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterRatesUnion.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/RatesChainItem.h"

namespace TrigCostRootAnalysis {

  /**
   * Counter to monitor the rates of the union of a group of chains
   * @param _costData Const pointer to the data store, not used by this counter at the moment.
   * @param _name Const ref to chain's name
   * @param _ID Chain's ID number.
   */
  CounterRatesUnion::CounterRatesUnion( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel, MonitorBase* _parent ) :
    CounterBaseRates(_costData, _name, _ID, _detailLevel, _parent), m_combinationClassification(kUnset) {
    //Info("CounterRatesUnion::CounterRatesUnion","New CounterRatesUnion, %s", _name.c_str());
  }

  /**
   * Counter destructor. Nothing currently to delete.
   */
  CounterRatesUnion::~CounterRatesUnion() {
  }

  void CounterRatesUnion::finalise() {

    // This step is only for unique counters
    if (getStrDecoration(kDecType) == "UniqueHLT" || getStrDecoration(kDecType) == "UniqueL1") {
      assert (m_globalRates != 0);

      // Get the global rate
      Float_t _globPasses       = m_globalRates->getValue(kVarEventsPassed,    kSavePerCall);
      Float_t _globPassesDP     = m_globalRates->getValue(kVarEventsPassedDP,  kSavePerCall);
      Float_t _globPassesNoPS   = m_globalRates->getValue(kVarEventsPassedNoPS,kSavePerCall);
      // Square these to keep them as sumw2
      Float_t _globPassesErr    = TMath::Power( m_globalRates->getValueError(kVarEventsPassed,    kSavePerCall), 2);
      Float_t _globPassesDPErr  = TMath::Power( m_globalRates->getValueError(kVarEventsPassedDP,  kSavePerCall), 2);
      Float_t _globPassesNoPSErr= TMath::Power( m_globalRates->getValueError(kVarEventsPassedNoPS,kSavePerCall), 2);

      // Get my rate
      Float_t _chainPasses       = getValue(kVarEventsPassed,   kSavePerCall);
      Float_t _chainPassesDP     = getValue(kVarEventsPassedDP, kSavePerCall);
      Float_t _chainPassesNoPS   = getValue(kVarEventsPassedNoPS, kSavePerCall);
      // Square these to keep them as sumw2
      Float_t _chainPassesErr    = TMath::Power( getValueError(kVarEventsPassed,    kSavePerCall), 2);
      Float_t _chainPassesDPErr  = TMath::Power( getValueError(kVarEventsPassedDP,  kSavePerCall), 2);
      Float_t _chainPassesNoPSErr= TMath::Power( getValueError(kVarEventsPassedNoPS,kSavePerCall), 2);

      //Set me to the difference
      setValue(kVarEventsPassed,    kSavePerCall, _globPasses       - _chainPasses       );
      setValue(kVarEventsPassedDP,  kSavePerCall, _globPassesDP     - _chainPassesDP     );
      setValue(kVarEventsPassedNoPS,kSavePerCall, _globPassesNoPS   - _chainPassesNoPS   );
      setErrorSquared(kVarEventsPassed,    kSavePerCall, _globPassesErr    - _chainPassesErr    ); // Note - directly changing m_sumw2
      setErrorSquared(kVarEventsPassedDP,  kSavePerCall, _globPassesDPErr  - _chainPassesDPErr  ); // Note - directly changing m_sumw2
      setErrorSquared(kVarEventsPassedNoPS,kSavePerCall, _globPassesNoPSErr- _chainPassesNoPSErr); // Note - directly changing m_sumw2

    }

    CounterBaseRates::finalise();
  }


  /**
   * Return if this chain passes the HLT and at least one L1 by checking both pass-raw and pass-prescale bits.
   * For the union of triggers, at least one L2 chains must pass raw and pass PS, plus one of their L1 chains must too.
   * This is the naive method.
   * @param _usePrescale - if set to kTRUE (default) then the prescale will be simulated, otherwise the prescale is taken to be 1.
   * @return 1 if the chain passes, 0 if not.
   */
  Float_t CounterRatesUnion::runDirect(Bool_t _usePrescale) {
    if (m_L2s.size() > 0) { // Am a combination of HLTs

      for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
        RatesChainItem* _L2 = (*_L2It);

        if (_L2->getPassRaw() == kFALSE) continue; // L2 did not pass, try next
        if (_usePrescale == kTRUE && _L2->getPassPS() == kFALSE) continue;

        for (ChainItemSetIt_t _lowerIt = _L2->getLowerStart(); _lowerIt != _L2->getLowerEnd(); ++_lowerIt) {
          RatesChainItem* _L1 = (*_lowerIt);

          if (_L1->getPassRaw() == kFALSE) continue; // L1 did not pass, try next
          if (_usePrescale == kTRUE && _L1->getPassPS() == kFALSE) continue;
          return 1.; // At least one L2 passed with an L1 which passed
        }
      }

    } else { // Am a combination of L1s

      for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
        RatesChainItem* _L1 = (*_L1It);

        if (_L1->getPassRaw() == kFALSE) continue; // L1 did not pass, try next
        if (_usePrescale == kTRUE && _L1->getPassPS() == kFALSE) continue; //PS did not pass, try next

        return 1;
      }

    }
    return 0.;
  }

  /**
   * If at least one of the items in this chain passes-raw at both the HLT and L1 levels, then calculate the effective weight from all prescale factors.
   * There are a few algorithms for the union of chains, with simpler algorithms for simpler topologies
   * @return Event prescale weight for this chain 0 < PS weight < 1
   */
  Float_t CounterRatesUnion::runWeight() {
    if (m_combinationClassification == kUnset) classify();
    if (m_cannotCompute == kTRUE) return 0.;

    if      (m_combinationClassification == kAllOneToMany) return runWeight_AllOneToMany();
    else if (m_combinationClassification == kOnlyL1) return runWeight_OnlyL1();
    else if (m_combinationClassification == kAllToAll) return runWeight_AllToAll();
    else if (m_combinationClassification == kAllOneToOne) return runWeight_AllOneToOne();
    else if (m_combinationClassification == kManyToMany) return runWeight_ManyToMany();
    return 0.;
  }

  /**
   * Classify this set of chains, see if we are able to use faster equations on simpler topologies
   */
  void CounterRatesUnion::classify() {

    // See if L1 only, no HLT chains
    if (m_L2s.size() == 0) {
      if (Config::config().debug()) {
        Info("CounterRatesUnion::classify","Chain %s topology classified as OnlyL1.",
          getName().c_str());
      }
      m_combinationClassification = kOnlyL1;
      return;
    }

    // See if all-to-all, each HLT chain must share identical L1 chains
    Bool_t _allToAll = kTRUE;
    for (ChainItemSetIt_t _L2TestIt = m_L2s.begin(); _L2TestIt != m_L2s.end(); ++_L2TestIt ) {
      RatesChainItem* _L2Test = (*_L2TestIt);
      // See if I share the exact same set of L1's as the others
      for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It ) {
        RatesChainItem* _L2 = (*_L2It);
        if (_L2 == _L2Test) continue; // Don't check against myself
        if (_L2->getLowerContainsAll( _L2Test->getLower() ) == kFALSE) {
          _allToAll = kFALSE;
          break;
        }
      }
      if (_allToAll == kFALSE) break;
    }
    if (_allToAll == kTRUE) {
      if (Config::config().debug()) {
        Info("CounterRatesUnion::classify","Chain %s topology classified as All-To-All.",
          getName().c_str());
      }
      m_combinationClassification = kAllToAll;
      return;
    }

    // See if all-one-to-one, each HLT chain must have a single unique L1 seed
    Bool_t _allOneToOne = kTRUE;
    for (ChainItemSetIt_t _L2TestIt = m_L2s.begin(); _L2TestIt != m_L2s.end(); ++_L2TestIt ) {
      RatesChainItem* _L2Test = (*_L2TestIt);
      // Check that I have exactly one seed
      if (_L2Test->getLower().size() != 1) {
        _allOneToOne = kFALSE;
        break;
      }
      // Check that no one else has the same L1 seed
      for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It ) {
        RatesChainItem* _L2 = (*_L2It);
        if (_L2 == _L2Test) continue; // Don't check against myself

        if (_L2->getLowerContains( (*_L2Test->getLowerStart()) ) == kTRUE) { // Remember that we know L2Test only has one lower
          _allOneToOne = kFALSE;
          break;
        }

      }
      if (_allOneToOne == kFALSE) break;
    }
    if (_allOneToOne == kTRUE) {
      if (Config::config().debug()) {
        Info("CounterRatesUnion::classify","Chain %s topology classified as All-One-To-One.",
          getName().c_str());
      }
      m_combinationClassification = kAllOneToOne;
      return;
    }

    // see if all-one-to-many, each L2 must have eactly one seed
    Bool_t _allOneToMany = kTRUE;
    for (ChainItemSetIt_t _L2TestIt = m_L2s.begin(); _L2TestIt != m_L2s.end(); ++_L2TestIt ) {
      RatesChainItem* _L2Test = (*_L2TestIt);
      // Check that I have exactly one seed
      if (_L2Test->getLower().size() != 1) {
        _allOneToMany = kFALSE;
        break;
      }
    }
    if (_allOneToMany == kTRUE) {
      if (Config::config().debug()) {
        Info("CounterRatesUnion::classify","Chain %s topology classified as All-One-To-Many.",
          m_name.c_str());
      }
      m_combinationClassification = kAllOneToMany;
      return;
    }

    // Otherwise we have to use the general form
    if (Config::config().debug()) {
      Info("CounterRatesUnion::classify","Chain %s topology classified as Many-To-Many. NL1:%i. Computational complexity (2^NL1-1)=%i.",
        getName().c_str(), (Int_t)m_L1s.size(), (UInt_t)(TMath::Power(2., (Double_t)m_L1s.size())-1) );
    }
    if (m_L1s.size() > 20) { // 32 is the technical maximim - but the this is already impractical
      Error("CounterRatesUnion::classify","Cannot calculate rates union for this complexity, %s. Use fewer L1 seeds!!! Disabling this combination.", getName().c_str());
      m_cannotCompute = kTRUE;
    } else if (m_L1s.size() > 10) {
      Warning("CounterRatesUnion::classify","Many L1s in this combination, %s. Calculatuon will be *SLOW.*", getName().c_str());
    }
    m_combinationClassification = kManyToMany;
  }

  /**
   * This is the trivial case where we only have items at L1
   */
  Float_t CounterRatesUnion::runWeight_OnlyL1() {
    Float_t _w = 1.;
    for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
      RatesChainItem* _L1 = (*_L1It);
      _w *= (1. - _L1->getPassRawOverPS());
    }
    return (1. - _w);
  }

  /**
   * This is the trivial parallel case where all L2s have one unique L1 chain
   * See Eq 37 of http://arxiv.org/abs/0901.4118
   */
  Float_t CounterRatesUnion::runWeight_AllOneToOne() {
    Float_t _w = 1.;

    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      RatesChainItem* _L2 = (*_L2It);
      RatesChainItem* _L1 = (*_L2->getLowerStart());
      assert( _L2->getLower().size() == 1);

      _w *= (1. - (_L2->getPassRawOverPS() * _L1->getPassRawOverPS()) );
    }

    return (1. - _w);
  }

  /**
   * This is the factorised case where all L2's share an identical set of L1 seeds.
   * L1 and L2 probabilities factorise.
   * See Eq 36 of http://arxiv.org/abs/0901.4118
   */
  Float_t CounterRatesUnion::runWeight_AllToAll() {
    Float_t _weightL2 = 1., _weightL1 = 1.;

    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      RatesChainItem* _L2 = (*_L2It);
      _weightL2 *= (1. - _L2->getPassRawOverPS());
    }

    for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
      RatesChainItem* _L1 = (*_L1It);
      _weightL1 *= (1. - _L1->getPassRawOverPS());
    }

    return (1. - _weightL1) * (1. - _weightL2);
  }

  /**
   * This is an extension of runWeight_AllToAll, here though we have the common occurance in ATLAS
   * of each L1 item seeding one-or-more HLT items, but each HLT item is seeded by exactly one L1 item.
   * Therefore we can treat the situation as multiple AllToAll sub-cases and combine them together.
   *
   * Any optimisation of this function can save a whole load of time
   */
  Float_t CounterRatesUnion::runWeight_AllOneToMany() {

    Float_t _weightAllChains = 1.;
    _weightAllChains = 1.;
    for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
      RatesChainItem* _L1 = (*_L1It);
      Float_t _weightL1 = (1. - _L1->getPassRawOverPS());
       //     Info("CounterRatesUnion::runWeight_AllOneToMany","L1 item  %s has weight %f", _L1->getName().c_str(),  _L1->getPassRawOverPS());
      if ( isEqual(_weightL1, 1.) ) continue; // If L1 failed, no point looking at HLT
      Float_t _weightL2 = 1.;
      for (ChainItemSetIt_t _L2It = _L1->getUpperStart(); _L2It != _L1->getUpperEnd(); ++_L2It) {
        if ( m_L2s.count( (*_L2It) ) == 0 ) continue;
        _weightL2 *= (1. - (*_L2It)->getPassRawOverPS());
        // Info("CounterRatesUnion::runWeight_AllOneToMany","L2 item  %s has weight %f", (*_L2It)->getName().c_str(),  (*_L2It)->getPassRawOverPS());
      }
      Float_t _weightChain = (1. - _weightL1) * (1. - _weightL2);
      _weightAllChains *= (1. - _weightChain);
    }
   return (1. - _weightAllChains);
  }


  /**
   * This is the general formula for the nastiest case which can handle:
   * -> Multiple L1 items seeding multiple L2 items
   * -> Where any L2 item may be seeded by many L1 items
   * -> And any L1 item may seed many L2 items.
   * The probabilities do not factorise and must be evaluated in turn for each possible combinatoric
   * of the L1 items. See Eq 35 of http://arxiv.org/abs/0901.4118
   */
  Float_t CounterRatesUnion::runWeight_ManyToMany() {
    assert( m_L1s.size() && m_L2s.size() );

    // We can speed up by pre-checking if any L2's passed, and return if not.
    Bool_t _shouldRun = kFALSE;
    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      if ( (*_L2It)->getPassRaw() == kTRUE ) {
        _shouldRun = kTRUE;
        break;
      }
    }
    if (_shouldRun == kFALSE) return 0.;

    Double_t _w = 0.; // Event weight, we will add to this for each L1 pattern as we evaluate them

    // Now we need to iterate over all possible L1 patterns (excluding all fail)
    // We will increment a UInt_t and use it as a bit-map. Therefore cannot have more than 32 L1 items (in reality, don't want more than ~5).
    assert( m_L1s.size() <= 32);
    // NPatterns = (2^L1Size - 1) E.G. For 3x L1 items, there are 7 options to consider [100, 010, 110, 001, 101, 011, 111]
    UInt_t _nPatterns = static_cast<UInt_t>( TMath::Power( 2., static_cast<Int_t>(m_L1s.size()) ) ) - 1;
    UInt_t _N = 0; // Used to remember which bit of the bit-map we are investigating
    for (UInt_t _pattern = 1; _pattern <= _nPatterns; ++_pattern) {
      // Get the pattern bit-map in a usable form
      std::bitset<32> _patternBits( _pattern );

      // Step 1
      // Get the L1 probability for this pattern to pass
      Double_t _pOfBitPattern = 1.;
      // Loop over all L1 chains
      _N = 0;
      for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
        RatesChainItem* _L1 = (*_L1It);

        // Is this L1 chain part of the bit-patter we are exploring?
        if ( _patternBits.test( _N++ ) == 1 ) {// It is
          _pOfBitPattern *= _L1->getPassRawOverPS();
        } else { // It is not
          _pOfBitPattern *= ( 1. - _L1->getPassRawOverPS() );
        }
      }

      //TODO add and check if (isZero(_pOfBitPattern)) continue;

      // Step 2
      // Get the probability that this L1 bit-pattern is kept by L2
      Double_t _pOfKeepingBitPattern = 1.;
      // Loop over all L2 chains
      for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
        RatesChainItem* _L2 = (*_L2It);

        Double_t _L2Weight = 0.;
        // Loop over all the L1 items
        _N = 0;
        for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
          RatesChainItem* _L1 = (*_L1It);

          // If this L1 item is not in the bit pattern then continue
          if (_patternBits.test( _N++ ) == 0) continue;

          // If this L2 chain does not seed this L1 item then continue
          if (_L2->getLowerContains(_L1) == kFALSE) continue;

          _L2Weight = 1.; // This L2 chain is seeded by a L1 item in the current bit-map pattern
          break;
        }

        // Now we see if this chain passed, and multiply the product by the PASS_RAW (0 or 1) and 1/PS
        _L2Weight *= _L2->getPassRawOverPS();

        // We add the contribution from this L2 chain to _pOfKeepingBitPattern
        _pOfKeepingBitPattern *= (1. - _L2Weight);
      }

      // Step 3
      // Combine L1 with L2 and add to the weight for the event.
      // Each "fail" chain will have multiplied _pOfKeepingBitPattern by 1, each pass chain by a factor (1 - 1/PS)
      // So now we do ( 1 - _probOfKeepingBitPattern ) to get the final p to keep the pattern at L2
      // Finally the weight for this bit pattern is the p of getting it * the p of keeping it
      _w += ( _pOfBitPattern * (1. - _pOfKeepingBitPattern) );
    }

    return _w;
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterRatesUnion::debug(UInt_t _e) {
    UNUSED(_e);
  }

} // namespace TrigCostRootAnalysis
