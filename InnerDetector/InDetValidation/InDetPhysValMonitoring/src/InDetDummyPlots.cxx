/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetDummyPlots.h"
#include "xAODTruth/TruthVertex.h"
#include "InDetPhysValMonitoringUtilities.h"
#include <cmath>
using namespace IDPVM;

InDetDummyPlots::InDetDummyPlots(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_lepton_disappearance_radius{},
  m_low_Pt_lepton_frac{},
  m_nOut_of_lepdeath{},
  m_brem_spectrum{},
  m_energy_remaining{},
  m_energy_remaining_vs_eta{},
  m_energy_remaining_vs_prodR_TRT_barrel{},
  m_energy_remaining_vs_prodR_type_A_endcap{},
  m_energy_remaining_vs_prodR_type_A_and_B{},
  m_energy_remaining_vs_prodR_type_B_endcap{},
  m_primary_photon_eta_vs_conversion_radius{},
  m_primary_photon_pt_vs_decay_radius{},
  m_primary_photon_pt{},
  m_brem_photon_eta_vs_radius_of_conversion{},
  m_brem_photon_pt_vs_radius_of_origin_of_conversion{},
  m_truthMatchProbability_vs_delta_R{},
  m_minimum_delta_R{},
  m_minimum_delta_R_2{},
  m_minimum_delta_R_3{},
  m_minimum_delta_R_sub300{},
  m_minimum_delta_R_300_600{},
  m_minimum_delta_R_overall{},
  m_minimum_delta_R_not_found{},
  m_minimum_delta_R_2_not_found{},
  m_minimum_delta_R_3_not_found{},
  m_minimum_delta_R_sub300_not_found{},
  m_minimum_delta_R_300_600_not_found{},
  m_minimum_delta_R_not_found_overall{},
  m_delta_inverse_pt{},
  m_delta_inverse_pt_not_found{},
  m_charge_vs_truth_match_rate{},
  m_min_dR_vs_delta_inverse_pt{},
  m_charge_truth_agreement{},
  m_truth_vs_charge_product{} {
  // nop
}

void
InDetDummyPlots::initializePlots() {
  book(m_lepton_disappearance_radius, "lepton_disappearance_radius");
  book(m_low_Pt_lepton_frac, "low_Pt_lepton_frac");
  book(m_nOut_of_lepdeath, "nOut_of_lepdeath");
  book(m_brem_spectrum, "brem_spectrum");
  book(m_energy_remaining, "energy_remaining");
  book(m_energy_remaining_vs_eta, "energy_remaining_vs_eta");
  book(m_energy_remaining_vs_prodR_TRT_barrel, "energy_remaining_vs_prodR_TRT_barrel");
  book(m_energy_remaining_vs_prodR_type_A_endcap, "energy_remaining_vs_prodR_type_A_endcap");
  book(m_energy_remaining_vs_prodR_type_A_and_B, "energy_remaining_vs_prodR_type_A_and_B");
  book(m_energy_remaining_vs_prodR_type_B_endcap, "energy_remaining_vs_prodR_type_B_endcap");
  book(m_primary_photon_eta_vs_conversion_radius, "primary_photon_eta_vs_conversion_radius");
  book(m_primary_photon_pt_vs_decay_radius, "primary_photon_pt_vs_decay_radius");
  book(m_primary_photon_pt, "primary_photon_pt");
  book(m_brem_photon_eta_vs_radius_of_conversion, "brem_photon_eta_vs_radius_of_conversion");
  book(m_brem_photon_pt_vs_radius_of_origin_of_conversion, "brem_photon_pt_vs_radius_of_origin_of_conversion");
  book(m_truthMatchProbability_vs_delta_R, "truthMatchProbability_vs_delta_R");
  book(m_minimum_delta_R, "minimum_delta_R");
  book(m_minimum_delta_R_2, "minimum_delta_R_2");
  book(m_minimum_delta_R_3, "minimum_delta_R_3");
  book(m_minimum_delta_R_sub300, "minimum_delta_R_sub300");
  book(m_minimum_delta_R_300_600, "minimum_delta_R_300_600");
  book(m_minimum_delta_R_overall, "minimum_delta_R_overall");
  book(m_minimum_delta_R_not_found, "minimum_delta_R_not_found");
  book(m_minimum_delta_R_2_not_found, "minimum_delta_R_2_not_found");
  book(m_minimum_delta_R_3_not_found, "minimum_delta_R_3_not_found");
  book(m_minimum_delta_R_sub300_not_found, "minimum_delta_R_sub300_not_found");
  book(m_minimum_delta_R_300_600_not_found, "minimum_delta_R_300_600_not_found");
  book(m_minimum_delta_R_not_found_overall, "minimum_delta_R_not_found_overall");
  book(m_delta_inverse_pt, "delta_inverse_pt");
  book(m_delta_inverse_pt_not_found, "delta_inverse_pt_not_found");
  book(m_charge_vs_truth_match_rate, "charge_vs_truth_match_rate");
  book(m_min_dR_vs_delta_inverse_pt, "min_dR_vs_delta_inverse_pt");
  book(m_charge_truth_agreement, "charge_truth_agreement");
  book(m_truth_vs_charge_product, "truth_vs_charge_product");
}

void
InDetDummyPlots::lepton_fill(const xAOD::TruthParticle& truth, float weight) {
  double R = truth.auxdata<float>("prodR");
  double eta = truth.eta();
  double abseta = fabs(eta);

  fillHisto(m_low_Pt_lepton_frac, R, weight);
  if (truth.hasProdVtx()) {
    const xAOD::TruthVertex* vtx = truth.prodVtx();
    double prod_rad = vtx->perp();
    // double prod_z = vtx->z(); unused
    if (truth.hasDecayVtx()) {
      const xAOD::TruthVertex* decay_vtx = truth.decayVtx();
      double decay_rad = decay_vtx->perp();
      int nOut = decay_vtx->nOutgoingParticles();
      if (prod_rad < 150) {
        fillHisto(m_lepton_disappearance_radius, decay_rad);
      }
      fillHisto(m_nOut_of_lepdeath, nOut);
      for (int i = 0; i < nOut; i++) {
        const xAOD::TruthParticle* out = decay_vtx->outgoingParticle(i);
        double brem_e = out->e() * 0.001; // convert MeV to GeV
        double log_e = std::log10(brem_e);
        if (out->pdgId() == 22) {
          fillHisto(m_brem_spectrum, decay_rad, log_e);
        }
      }
    }
    double ECal(1000);            //Inner radius of the ECal, not sure this is correct
    double e_init = truth.e();    //initial electron/positron total energy
    double e_lost(0);             //energy the electron/positron has lost so far
    
    std::vector<const xAOD::TruthParticle*> pers_truth; //Not sure if this is right
    pers_truth.push_back(&truth);
   
    std::vector<double> decays;

    std::cout<<"Tarkin: You may fire when ready \n";
    if(prod_rad < ECal){
      do {
	if(pers_truth.back()->hasDecayVtx()){
	  const xAOD::TruthVertex * decay = pers_truth.back()->decayVtx();
	  double dec_rad = decay->perp();
	  decays.push_back(dec_rad);
	  int temp_nOut = decay->nOutgoingParticles();
	  std::vector<const xAOD::TruthParticle*> outs;
	  for(int i=0; i<temp_nOut; i++){
	    const xAOD::TruthParticle * temp_out = decay->outgoingParticle(i);
	    outs.push_back(temp_out);
	  }
	  if(temp_nOut == 2){
	    if(outs[0]->pdgId() == 22){
	      e_lost += outs[0]->e();
	      pers_truth.push_back(outs[1]);  //Adding electron/positron to pers_truth vector
	    }else if(outs[1]->pdgId() == 22){
	      e_lost += outs[1]->e();
	      pers_truth.push_back(outs[0]);
	    }else{
	      break;
	    }
	  }else{
	    break;
	  }
	  outs.clear();
	}else{
	  break;
	}
      }while(decays.back() < ECal);  
    }
    double e_left = ((e_init - e_lost)/e_init);  //fraction of origina electron/positron energy left when it reaches the ECal
    std::cout<<"Fraction of electron/positron energy remaining: "<<e_left<<"\n";
    fillHisto(m_energy_remaining, prod_rad, e_left);
    fillHisto(m_energy_remaining_vs_eta, eta, e_left);
    if(abseta < 0.625){
      fillHisto(m_energy_remaining_vs_prodR_TRT_barrel, prod_rad, e_left);
    }else if((1.070 < abseta) and (abseta < 1.304)){
      fillHisto(m_energy_remaining_vs_prodR_type_A_endcap, prod_rad, e_left);
    }else if((1.304 < abseta) and (abseta < 1.752)){
      fillHisto(m_energy_remaining_vs_prodR_type_A_and_B, prod_rad, e_left);
    }else if((1.752 < abseta) and (abseta < 2.0)){
      fillHisto(m_energy_remaining_vs_prodR_type_B_endcap, prod_rad, e_left);
    }
  }
}

void
InDetDummyPlots::prim_photon_fill(const xAOD::TruthParticle& truth) {
  double eta = truth.eta();
  double pt = truth.pt() * 0.001; // convert MeV to GeV
  double log_pt = std::log10(pt);

  fillHisto(m_primary_photon_pt, pt);
  if (truth.hasDecayVtx()) {
    const xAOD::TruthVertex* vtx = truth.decayVtx();
    double decay_rad = vtx->perp();
    fillHisto(m_primary_photon_eta_vs_conversion_radius, decay_rad, eta);
    fillHisto(m_primary_photon_pt_vs_decay_radius, decay_rad, log_pt);
  }
}

void
InDetDummyPlots::brem_photon_fill(const xAOD::TruthParticle& truth) {
  double eta = truth.eta();
  double pt = truth.pt() * 0.001; // convert MeV to GeV
  double log_pt = std::log10(pt);
  const xAOD::TruthVertex* vtx = truth.prodVtx();

  if (truth.hasDecayVtx()) {
    double decay_rad = vtx->perp();
    fillHisto(m_brem_photon_eta_vs_radius_of_conversion, decay_rad, eta);
    fillHisto(m_brem_photon_pt_vs_radius_of_origin_of_conversion, decay_rad, log_pt);
  }
}

void
InDetDummyPlots::track_vs_truth(const xAOD::TrackParticle& track, const xAOD::TruthParticle& truth, float tmp){
  double track_theta = track.theta();
  double truth_theta = truth.auxdata< float >("theta");
  double truth_eta = truth.eta();
  double track_eta = -std::log(std::tan(track_theta*0.5));

  double delta_eta = track_eta - truth_eta;
  double delta_theta = track_theta - truth_theta;

  double delta_R = sqrt(delta_eta * delta_eta + delta_theta * delta_theta);

  fillHisto(m_truthMatchProbability_vs_delta_R, delta_R, tmp);
}

void
InDetDummyPlots::minDR(float min_dR, float prod_rad, float bestmatch, double BIDPt, double truth_pt){

  if(bestmatch > 0.50){
    fillHisto(m_delta_inverse_pt, BIDPt);
    if(prod_rad < 100){
      fillHisto(m_minimum_delta_R, min_dR);
    }else if(prod_rad < 200){
      fillHisto(m_minimum_delta_R_2, min_dR);
    }else{
      fillHisto(m_minimum_delta_R_3, min_dR);
    }
    if(prod_rad < 300){
      fillHisto(m_minimum_delta_R_sub300, min_dR);
    }else{
      fillHisto(m_minimum_delta_R_300_600, min_dR);
    }
    fillHisto(m_minimum_delta_R_overall, min_dR);
  }else{
    fillHisto(m_delta_inverse_pt_not_found, BIDPt);
    std::cout<<"Han: the bestmatch probability is "<<bestmatch<<"\n";
    if(prod_rad < 100){
      fillHisto(m_minimum_delta_R_not_found, min_dR);
    }else if(prod_rad < 200){
      fillHisto(m_minimum_delta_R_2_not_found, min_dR);
    }else{
      fillHisto(m_minimum_delta_R_3_not_found, min_dR);
    }
    if(prod_rad < 300){
      fillHisto(m_minimum_delta_R_sub300_not_found, min_dR);
    }else{
      fillHisto(m_minimum_delta_R_300_600_not_found, min_dR);
    }
    fillHisto(m_minimum_delta_R_not_found_overall, min_dR);
  }

  //fillHisto(m_charge_vs_truth_match_rate, truth_pt, cvst);
  fillHisto(m_min_dR_vs_delta_inverse_pt, BIDPt, min_dR);
  //fillHisto(m_charge_truth_agreement, prod_rad, cvst);
  //fillHisto(m_truth_vs_charge_product, charge_product, bestmatch);
}

void
InDetDummyPlots::finalizePlots() {
}
