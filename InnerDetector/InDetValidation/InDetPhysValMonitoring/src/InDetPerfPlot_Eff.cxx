/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_Eff.h"
#include "TrkValHistUtils/EfficiencyPurityCalculator.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"

using namespace TMath;

InDetPerfPlot_Eff::InDetPerfPlot_Eff(InDetPlotBase *pParent, const std::string &sDir) :
  InDetPlotBase(pParent, sDir),
  m_trackeff_vs_eta{},
  m_trackeff_vs_pt{},
  m_trackeff_vs_phi{},
  m_trackeff_vs_d0{},
  m_trackeff_vs_z0{},
  m_trackeff_vs_R{},
  m_trackeff_vs_Z{},
  m_trackeff_vs_prodR{},
  m_trackeff_vs_prodZ{},
  m_eff_vs_eta_of_daughters{},
  m_eff_vs_theta_of_daughters{},
  m_eff_vs_theta_tan_of_daughters{},
  m_eff_vs_theta_cotan_of_daughters{},
  m_eff_vs_phi_of_daughters{},
  m_eff_vs_phi_sin_of_daughters{},
  m_eff_vs_phi_cos_of_daughters{},
  m_trackinjeteff_vs_eta{},
  m_trackinjeteff_vs_phi{},
  m_trackinjeteff_vs_pt{},
  m_trackinjeteff_vs_dr{},
  m_trackinjeteff_vs_dr_lt_j50{},
  m_trackinjeteff_vs_dr_gr_j100{},
  m_trackinjeteff_vs_jetet{} {
  // nop
}

void
InDetPerfPlot_Eff::initializePlots() {
  book(m_trackeff_vs_eta, "trackeff_vs_eta");
  book(m_trackeff_vs_pt, "trackeff_vs_pt");
  book(m_trackeff_vs_phi, "trackeff_vs_phi");
  book(m_trackeff_vs_d0, "trackeff_vs_d0");
  book(m_trackeff_vs_z0, "trackeff_vs_z0");
  book(m_trackeff_vs_R, "trackeff_vs_R");
  book(m_trackeff_vs_Z, "trackeff_vs_Z");

  book(m_trackeff_vs_prodR, "trackeff_vs_prodR");
  book(m_trackeff_vs_prodZ, "trackeff_vs_prodZ");

  book(m_eff_vs_eta_of_daughters, "eff_vs_eta_of_daughters");
  book(m_eff_vs_theta_of_daughters, "eff_vs_theta_of_daughters");
  book(m_eff_vs_theta_tan_of_daughters, "eff_vs_theta_tan_of_daughters");
  book(m_eff_vs_theta_cotan_of_daughters, "eff_vs_theta_cotan_of_daughters");
  book(m_eff_vs_phi_of_daughters, "eff_vs_phi_of_daughters");
  book(m_eff_vs_phi_sin_of_daughters, "eff_vs_phi_sin_of_daughters");
  book(m_eff_vs_phi_cos_of_daughters, "eff_vs_phi_cos_of_daughters");

  book(m_trackinjeteff_vs_eta, "trackinjeteff_vs_eta");
  book(m_trackinjeteff_vs_phi, "trackinjeteff_vs_phi");
  book(m_trackinjeteff_vs_pt, "trackinjeteff_vs_pt");
  book(m_trackinjeteff_vs_dr, "trackinjeteff_vs_dr");
  book(m_trackinjeteff_vs_dr_lt_j50, "trackinjeteff_vs_dr_lt_j50");
  book(m_trackinjeteff_vs_dr_gr_j100, "trackinjeteff_vs_dr_gr_j100");
  book(m_trackinjeteff_vs_jetet, "trackinjeteff_vs_jetet");
}

void
InDetPerfPlot_Eff::pro_fill(const xAOD::TruthParticle &truth, float weight) {
  double eta = truth.eta();
  double pt = truth.pt() *0.001; // convert MeV to GeV
  double phi = truth.phi();
  double d0 = truth.auxdata<float>("d0");
  double z0 = truth.auxdata<float>("z0");
  double R = truth.auxdata<float>("prodR");
  double Z = truth.auxdata<float>("prodZ");

  fillHisto(m_trackeff_vs_eta,eta, weight);
  fillHisto(m_trackeff_vs_pt,pt, weight);
  fillHisto(m_trackeff_vs_phi,phi, weight);
  fillHisto(m_trackeff_vs_d0,d0, weight);
  fillHisto(m_trackeff_vs_z0,z0, weight);
  fillHisto(m_trackeff_vs_R,R, weight);
  fillHisto(m_trackeff_vs_Z,Z, weight);

  if (truth.hasProdVtx()) {
    const xAOD::TruthVertex *vtx = truth.prodVtx();
    double prod_rad = vtx->perp();
    double prod_z = vtx->z();
    fillHisto(m_trackeff_vs_prodR,prod_rad, weight);
    fillHisto(m_trackeff_vs_prodZ,prod_z, weight);
  }
}

void
InDetPerfPlot_Eff::BT_fill(const xAOD::TruthParticle &truth, float weight) {
  double eta = truth.eta();
  double theta = truth.auxdata< float >("theta");
  double phi = truth.phi();

  double tan_theta = std::tan(theta);
  double cot_theta = (std::cos(theta) / std::sin(theta));
  double sin_phi = std::sin(phi);
  double cos_phi = std::cos(phi);

  fillHisto(m_eff_vs_eta_of_daughters,eta, weight);
  fillHisto(m_eff_vs_theta_of_daughters,theta, weight);
  fillHisto(m_eff_vs_theta_tan_of_daughters,tan_theta, weight);
  fillHisto(m_eff_vs_theta_cotan_of_daughters,cot_theta, weight);
  fillHisto(m_eff_vs_phi_of_daughters,phi, weight);
  fillHisto(m_eff_vs_phi_sin_of_daughters,sin_phi, weight);
  fillHisto(m_eff_vs_phi_cos_of_daughters,cos_phi, weight);
}

void
InDetPerfPlot_Eff::jet_fill(const xAOD::TrackParticle &track, const xAOD::Jet &jet, float weight) {
  double trketa = track.eta();
  double trkphi = track.phi();
  double trkpt = track.pt() / 1000.;
  double dR = jet.p4().DeltaR(track.p4());
  double jetet = jet.pt() / 1000.;

  fillHisto(m_trackinjeteff_vs_eta,trketa, weight);
  fillHisto(m_trackinjeteff_vs_phi,trkphi, weight);
  fillHisto(m_trackinjeteff_vs_pt,trkpt, weight);
  fillHisto(m_trackinjeteff_vs_dr,dR, weight);

  if (jetet < 50) {
    fillHisto(m_trackinjeteff_vs_dr_lt_j50,dR, weight);
  }else if (jetet > 100) {
    fillHisto(m_trackinjeteff_vs_dr_gr_j100,dR, weight);
  }

  fillHisto(m_trackinjeteff_vs_jetet,jetet, weight);
}

void
InDetPerfPlot_Eff::finalizePlots() {
}
