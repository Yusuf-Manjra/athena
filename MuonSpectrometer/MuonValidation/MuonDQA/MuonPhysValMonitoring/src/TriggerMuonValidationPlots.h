/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_TRIGGERMUONVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_TRIGGERMUONVALIDATIONPLOTS_H

#include "RecoRelatedTriggerMuonPlots.h"
#include "TriggerEfficiencyPlots.h"
#include "MuonHistUtils/L1TriggerMuonPlots.h"
#include "MuonHistUtils/HLTriggerMuonPlots.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoI.h"
#include "xAODMuon/MuonAuxContainer.h"

class TriggerMuonValidationPlots:public PlotBase {
  public:
  TriggerMuonValidationPlots(PlotBase* pParent, std::string sDir, std::vector<unsigned int> authors, bool isData, bool doBinnedResolutionPlots, bool doTrigMuonL1Validation, bool doTrigMuonL2Validation, bool doTrigMuonEFValidation, std::vector<std::string> chains, std::vector<std::string> seeds);
    float PlateauTreshold;

    virtual ~TriggerMuonValidationPlots();
    void fill(const xAOD::MuonRoI &TrigL1mu);
    void fill(const xAOD::L2StandAloneMuon &L2SAmu);
    void fill(const xAOD::L2CombinedMuon &L2CBmu);
    void fill(const xAOD::L2IsoMuon &L2Isomu);
    void fill(const xAOD::Muon& Trigmu);
    void fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu);
    void fillNumEff(const xAOD::Muon& Trigmu, std::string selectedChain);
    void fillFeatPlots(const xAOD::Muon& Trigmu, std::string selectedChain);
    void fillFeatPlots(const xAOD::MuonRoI& TrigL1mu, std::string selectedChain);
    void fillDenEff(const xAOD::Muon& Trigmu, std::string selectedChain);
    void fillDenRELEff(const xAOD::Muon& Trigmu, std::string selectedChain);
    void fillNumRELEff(const xAOD::Muon& Trigmu, std::string selectedChain);

    std::vector<unsigned int> m_selectedAuthors;
    std::vector<std::string> m_chains;
    std::vector<std::string> m_seeds;
    L1TriggerMuonPlots* m_oL1TriggerMuonPlots;    
    std::vector<HLTriggerMuonPlots*> m_oEFTriggerMuonPlots;
    std::vector<HLTriggerMuonPlots*> m_oL2TriggerMuonPlots;
    RecoRelatedTriggerMuonPlots* m_oL1TriggerMuonResolutionPlots;
    std::vector<RecoRelatedTriggerMuonPlots*> m_oL2TriggerMuonResolutionPlots;
    std::vector<RecoRelatedTriggerMuonPlots*> m_oEFTriggerMuonResolutionPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainBarrelEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainEndcapsEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainRELEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainBarrelRELEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainEndcapsRELEfficiencyPlots;

 private:
    void fillTriggerMuonPlots(const xAOD::Muon& Trigmu);
    void fillTriggerMuonPlots(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu);
    void fillTriggerMuonPlots(const xAOD::MuonRoI& TrigL1mu);
    void fillTriggerMuonPlots(const xAOD::L2StandAloneMuon& L2SAmu);
    void fillTriggerMuonPlots(const xAOD::L2CombinedMuon& L2CBmu);
    void fillTriggerMuonPlots(const xAOD::L2IsoMuon& L2Isomu);
    float findTrigTreshold(TString chain);

    bool m_isData;
    bool m_doTrigMuonL1Validation;
    bool m_doTrigMuonL2Validation;
    bool m_doTrigMuonEFValidation;


};

#endif
