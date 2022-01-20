/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPPARTONS_TOPPARTONUTILS_H
#define ANALYSISTOP_TOPPARTONS_TOPPARTONUTILS_H

#include "xAODTruth/TruthParticleContainer.h"

#include "TLorentzVector.h"

namespace top {
namespace PartonHistoryUtils {

  struct HiggsDecay {
    // DecayChannel
    char Higgs_channel;

    /// direct higgs decay
    TLorentzVector decay1_vector;
    int decay1_pdgId;
    TLorentzVector decay2_vector;
    int decay2_pdgId;
    int tau_decay1_isHadronic;
    int tau_decay2_isHadronic;
    
    TLorentzVector tau_decay1_vector;
    TLorentzVector tau_decay2_vector;

    TLorentzVector tauvis_decay1_vector;
    TLorentzVector tauvis_decay2_vector;

    /// subsequent decays of W/Z
    TLorentzVector decay1_from_decay1_vector;
    int decay1_from_decay1_pdgId;
    TLorentzVector decay2_from_decay1_vector;
    int decay2_from_decay1_pdgId;
    TLorentzVector decay1_from_decay2_vector;
    int decay1_from_decay2_pdgId;
    TLorentzVector decay2_from_decay2_vector;
    int decay2_from_decay2_pdgId;
    int tau_decay1_from_decay1_isHadronic;
    int tau_decay2_from_decay1_isHadronic;
    int tau_decay1_from_decay2_isHadronic;
    int tau_decay2_from_decay2_isHadronic;
    
    TLorentzVector tau_decay1_from_decay1_vector;
    TLorentzVector tau_decay2_from_decay1_vector;
    TLorentzVector tau_decay1_from_decay2_vector;
    TLorentzVector tau_decay2_from_decay2_vector;

    TLorentzVector tauvis_decay1_from_decay1_vector;
    TLorentzVector tauvis_decay2_from_decay1_vector;
    TLorentzVector tauvis_decay1_from_decay2_vector;
    TLorentzVector tauvis_decay2_from_decay2_vector;

    
  };

  ///Return particle after FSR (before the decay vertex)
  const xAOD::TruthParticle* findAfterFSR(const xAOD::TruthParticle* particle);
  
  ///Return true when particle is a top before FSR
  bool hasParticleIdenticalParent(const xAOD::TruthParticle* particle);

  ///handle gamma radiation of taus
  const xAOD::TruthParticle* findAfterGamma(const xAOD::TruthParticle* particle);
  
  /// Store higgs decay information
  HiggsDecay AnalyzeHiggsDecay(const xAOD::TruthParticle* higgs);
  
  /// Check if tau decays hadronically or not
  int TauIsHadronic(const xAOD::TruthParticle* tau, TLorentzVector& tau_p4);
}
}

#endif
