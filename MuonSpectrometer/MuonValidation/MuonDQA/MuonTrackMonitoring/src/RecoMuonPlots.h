/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RECOMUONPLOTS_H
#define MUONTRACKMONITORING_RECOMUONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"
#include "MuonHistUtils/RecoInfoPlots.h"
#include "MuonHistUtils/MomentumPullPlots.h"
#include "MuonHistUtils/MuonHitSummaryPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

class RecoMuonPlots:public PlotBase {
    public:
      RecoMuonPlots(PlotBase* pParent, std::string sDir);
      void fill(const xAOD::Muon& mu);
      
      // Reco only information
      Trk::ParamPlots                 m_oAllPlots;
      Muon::RecoInfoPlots             m_oMuRecoInfoPlots;
      Trk::ImpactPlots                m_oImpactPlots;
      Muon::MomentumPullPlots         m_oMomentumPullPlots;
      Trk::MSHitPlots                 m_oMSHitPlots; 
      Muon::MuonHitSummaryPlots       m_oMuonHitSummaryPlots;
};

#endif
