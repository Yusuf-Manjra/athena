/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/RecoMuonPlotOrganizer.h"

namespace Muon{
  
RecoMuonPlotOrganizer::RecoMuonPlotOrganizer(PlotBase* pParent, std::string sDir, std::vector<int> *selPlots):
  PlotBase(pParent, sDir)
  // Reco plots
  , m_oTrkParamPlots(NULL)
  , m_oImpactPlots(NULL)
  , m_oMuonParamPlots(NULL)
  , m_oMuRecoInfoPlots(NULL)
  , m_oMomentumPullPlots(NULL)
  , m_oMuonHitSummaryPlots(NULL)
  , m_oMuonIsolationPlots(NULL)
{
  
  if (!selPlots) {
    m_selPlots.clear();
    for (unsigned int i=0; i<MAX_RECOPLOTCLASS; i++) m_selPlots.push_back(i);
  }
  else m_selPlots = *selPlots; 
  
  for (auto p: m_selPlots) {
    switch (p) {
    case MUON_TRKPARAM:
      m_oTrkParamPlots = new Trk::ParamPlots(this, "/kinematics/", "Reco Muon");
      m_allPlots.push_back(m_oTrkParamPlots);
      break;
    case MUON_PARAM:
      m_oMuonParamPlots = new Muon::MuonParamPlots(this,"/parameters/");
      m_allPlots.push_back(m_oMuonParamPlots);
      break;
    case MUON_RECOINFO:
      m_oMuRecoInfoPlots = new Muon::RecoInfoPlots(this, "/parameters/");
      m_allPlots.push_back(m_oMuRecoInfoPlots);
      break;
    case MUON_TRKIMPACT:
      m_oImpactPlots = new Trk::ImpactPlots(this, "/parameters/");
      m_allPlots.push_back(m_oImpactPlots);
      break;
    case MUON_MOMPULLS:
      m_oMomentumPullPlots = new Muon::MomentumPullPlots(this, "/momentumPulls/");
      m_allPlots.push_back(m_oMomentumPullPlots);
      break;
    case MUON_HITS:
      m_oMuonHitSummaryPlots = new Muon::MuonHitSummaryPlots(this,"/hits/");
      m_allPlots.push_back(m_oMuonHitSummaryPlots);
      break;
    case MUON_ISO:	
      m_oMuonIsolationPlots = new Muon::MuonIsolationPlots(this,"/isolation/");
      m_allPlots.push_back(m_oMuonIsolationPlots);
      break;
    } 
  }
}
  
RecoMuonPlotOrganizer::~RecoMuonPlotOrganizer()
{
  for (auto plots: m_allPlots)  delete plots;
  m_allPlots.clear();
}
  
void RecoMuonPlotOrganizer::fill(const xAOD::Muon& mu) {
  if (m_oTrkParamPlots) m_oTrkParamPlots->fill(mu);
  if (m_oMuonParamPlots) m_oMuonParamPlots->fill(mu);
  if (m_oMuRecoInfoPlots) m_oMuRecoInfoPlots->fill(mu);
  if (m_oMomentumPullPlots) m_oMomentumPullPlots->fill(mu);
  if (m_oMuonHitSummaryPlots) m_oMuonHitSummaryPlots->fill(mu);
  if (m_oMuonIsolationPlots) m_oMuonIsolationPlots->fill(mu);

  // tracking related plots
  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  if (!primaryTrk) return;
  if (m_oImpactPlots) m_oImpactPlots->fill(*primaryTrk);
}

}
