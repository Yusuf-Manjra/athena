/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RecoMuonPlots.h"

typedef ElementLink< xAOD::TrackParticleContainer > TrackLink;
typedef ElementLink< xAOD::MuonContainer > MuonLink;

RecoMuonPlots::RecoMuonPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),

m_oAllPlots(this, "/", "Reco Muon"),
m_oMuRecoInfoPlots(this, "/"),
m_oImpactPlots(this, "/"),
m_oMomentumPullPlots(this, "/"),
m_oMSHitPlots(this,"/"),
m_oMuonHitSummaryPlots(this,"/")
{}

void RecoMuonPlots::fill(const xAOD::Muon& mu) {
  //General Plots
  m_oAllPlots.fill(mu);
  m_oMuRecoInfoPlots.fill(mu);
  m_oMomentumPullPlots.fill(mu);
  m_oMuonHitSummaryPlots.fill(mu);

  // tracking related plots
  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  if (!primaryTrk) return;
  m_oImpactPlots.fill(*primaryTrk);
  m_oMSHitPlots.fill(*primaryTrk);
}
