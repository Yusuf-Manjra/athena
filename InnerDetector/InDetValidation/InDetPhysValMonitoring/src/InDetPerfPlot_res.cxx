/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_res.cxx
 * @author Max Baugh
 **/

#include "InDetPerfPlot_res.h"
#include "xAODTracking/TrackingPrimitives.h"
#include <vector>
#include <utility>
#include "TH1D.h"
#include "TH2D.h"

using namespace TMath;

InDetPerfPlot_res::InDetPerfPlot_res(InDetPlotBase *pParent, const std::string &sDir)  : InDetPlotBase(pParent, sDir),
  m_meanbasePlots(NPARAMS, nullptr),
  m_meanbase_vs_eta_d0{},
  m_meanbase_vs_eta_z0{},
  m_meanbase_vs_eta_phi{},
  m_meanbase_vs_eta_theta{},
  m_meanbase_vs_eta_z0_sin_theta{},
  m_meanbase_vs_eta_qopt{},
  m_meanPlots(NPARAMS, nullptr),
  m_resmean_vs_eta_d0{},
  m_resmean_vs_eta_z0{},
  m_resmean_vs_eta_phi{},
  m_resmean_vs_eta_theta{},
  m_resmean_vs_eta_z0_sin_theta{},
  m_resmean_vs_eta_qopt{},
  m_resoPlots(NPARAMS, nullptr),
  m_reswidth_vs_eta_d0{},
  m_reswidth_vs_eta_z0{},
  m_reswidth_vs_eta_phi{},
  m_reswidth_vs_eta_theta{},
  m_reswidth_vs_eta_z0_sin_theta{},
  m_reswidth_vs_eta_qopt{},										      
  m_mean_vs_ptbasePlots(NPARAMS, nullptr),
  m_base_vs_pt_d0{},
  m_base_vs_pt_z0{},
  m_base_vs_pt_phi{},
  m_base_vs_pt_theta{},
  m_base_vs_pt_z0_sin_theta{},
  m_base_vs_pt_qopt{},
  m_mean_vs_ptPlots(NPARAMS, nullptr),
  m_resmean_vs_pt_d0{},
  m_resmean_vs_pt_z0{},
  m_resmean_vs_pt_phi{},
  m_resmean_vs_pt_theta{},
  m_resmean_vs_pt_z0_sin_theta{},
  m_resmean_vs_pt_qopt{},
  m_resptPlots(NPARAMS, nullptr),
  m_reswidth_vs_pt_d0{},
  m_reswidth_vs_pt_z0{},
  m_reswidth_vs_pt_phi{},
  m_reswidth_vs_pt_theta{},
  m_reswidth_vs_pt_z0_sin_theta{},
  m_reswidth_vs_pt_qopt{},
  m_pullPlots(NPARAMS, nullptr),
  m_pull_d0{},
  m_pull_z0{},
  m_pull_phi{},
  m_pull_theta{},
  m_pull_z0_sin_theta{},
  m_pull_qopt{},
  m_pullbasePlots(NPARAMS, nullptr),
  m_pullbase_d0{},
  m_pullbase_z0{},
  m_pullbase_phi{},
  m_pullbase_theta{},
  m_pullbase_z0_sin_theta{},
  m_pullbase_qopt{},
  m_pullmeanPlots(NPARAMS, nullptr),
  m_pullmean_d0{},
  m_pullmean_z0{},
  m_pullmean_phi{},
  m_pullmean_theta{},
  m_pullmean_z0_sin_theta{},
  m_pullmean_qopt{},
  m_pullwidthPlots(NPARAMS, nullptr),
  m_pullwidth_d0{},
  m_pullwidth_z0{},
  m_pullwidth_phi{},
  m_pullwidth_theta{},
  m_pullwidth_z0_sin_theta{},
  m_pullwidth_qopt{},
  m_paramNames{"d0", "z0", "phi","theta","z0*sin(theta)","qopt"}
{
//
}


void
InDetPerfPlot_res::initializePlots() {
  book(m_meanbase_vs_eta_d0,            "res_d0_vs_eta");
  book(m_meanbase_vs_eta_z0,            "res_z0_vs_eta");
  book(m_meanbase_vs_eta_phi,           "res_phi_vs_eta");
  book(m_meanbase_vs_eta_theta,         "res_theta_vs_eta");
  book(m_meanbase_vs_eta_z0_sin_theta,  "res_z0*sin(theta)_vs_eta");
  book(m_meanbase_vs_eta_qopt,          "res_qopt_vs_eta");

  m_meanbasePlots[0] = m_meanbase_vs_eta_d0;
  m_meanbasePlots[1] = m_meanbase_vs_eta_z0;
  m_meanbasePlots[2] = m_meanbase_vs_eta_phi;
  m_meanbasePlots[3] = m_meanbase_vs_eta_theta;
  m_meanbasePlots[4] = m_meanbase_vs_eta_z0_sin_theta;
  m_meanbasePlots[5] = m_meanbase_vs_eta_qopt;

  book(m_resmean_vs_eta_d0,           "resmean_d0_vs_eta");
  book(m_resmean_vs_eta_z0,           "resmean_z0_vs_eta");
  book(m_resmean_vs_eta_phi,          "resmean_phi_vs_eta");
  book(m_resmean_vs_eta_theta,        "resmean_theta_vs_eta");
  book(m_resmean_vs_eta_z0_sin_theta, "resmean_z0*sin(theta)_vs_eta");
  book(m_resmean_vs_eta_qopt,         "resmean_qopt_vs_eta");

  m_meanPlots[0] = m_resmean_vs_eta_d0;
  m_meanPlots[1] = m_resmean_vs_eta_z0;
  m_meanPlots[2] = m_resmean_vs_eta_phi;
  m_meanPlots[3] = m_resmean_vs_eta_theta;
  m_meanPlots[4] = m_resmean_vs_eta_z0_sin_theta;
  m_meanPlots[5] = m_resmean_vs_eta_qopt;

  book(m_reswidth_vs_eta_d0,           "reswidth_d0_vs_eta");
  book(m_reswidth_vs_eta_z0,           "reswidth_z0_vs_eta");
  book(m_reswidth_vs_eta_phi,          "reswidth_phi_vs_eta");
  book(m_reswidth_vs_eta_theta,        "reswidth_theta_vs_eta");
  book(m_reswidth_vs_eta_z0_sin_theta, "reswidth_z0*sin(theta)_vs_eta");
  book(m_reswidth_vs_eta_qopt,         "reswidth_qopt_vs_eta");

  m_resoPlots[0] = m_reswidth_vs_eta_d0;
  m_resoPlots[1] = m_reswidth_vs_eta_z0;
  m_resoPlots[2] = m_reswidth_vs_eta_phi;
  m_resoPlots[3] = m_reswidth_vs_eta_theta;
  m_resoPlots[4] = m_reswidth_vs_eta_z0_sin_theta;
  m_resoPlots[5] = m_reswidth_vs_eta_qopt;

  book(m_base_vs_pt_d0,           "res_d0_vs_pt");
  book(m_base_vs_pt_z0,           "res_z0_vs_pt");
  book(m_base_vs_pt_phi,          "res_phi_vs_pt");
  book(m_base_vs_pt_theta,        "res_theta_vs_pt");
  book(m_base_vs_pt_z0_sin_theta, "res_z0*sin(theta)_vs_pt");
  book(m_base_vs_pt_qopt,         "res_qopt_vs_pt");

  m_mean_vs_ptbasePlots[0] = m_base_vs_pt_d0;
  m_mean_vs_ptbasePlots[1] = m_base_vs_pt_z0;
  m_mean_vs_ptbasePlots[2] = m_base_vs_pt_phi;
  m_mean_vs_ptbasePlots[3] = m_base_vs_pt_theta;
  m_mean_vs_ptbasePlots[4] = m_base_vs_pt_z0_sin_theta;
  m_mean_vs_ptbasePlots[5] = m_base_vs_pt_qopt;

  book(m_resmean_vs_pt_d0,           "resmean_d0_vs_pt");
  book(m_resmean_vs_pt_z0,           "resmean_z0_vs_pt");
  book(m_resmean_vs_pt_phi,          "resmean_phi_vs_pt");
  book(m_resmean_vs_pt_theta,        "resmean_theta_vs_pt");
  book(m_resmean_vs_pt_z0_sin_theta, "resmean_z0*sin(theta)_vs_pt");
  book(m_resmean_vs_pt_qopt,         "resmean_qopt_vs_pt");

  m_mean_vs_ptPlots[0] = m_resmean_vs_pt_d0;
  m_mean_vs_ptPlots[1] = m_resmean_vs_pt_z0;
  m_mean_vs_ptPlots[2] = m_resmean_vs_pt_phi;
  m_mean_vs_ptPlots[3] = m_resmean_vs_pt_theta;
  m_mean_vs_ptPlots[4] = m_resmean_vs_pt_z0_sin_theta;
  m_mean_vs_ptPlots[5] = m_resmean_vs_pt_qopt;

  book(m_reswidth_vs_pt_d0,           "reswidth_d0_vs_pt");
  book(m_reswidth_vs_pt_z0,           "reswidth_z0_vs_pt");
  book(m_reswidth_vs_pt_phi,          "reswidth_phi_vs_pt");
  book(m_reswidth_vs_pt_theta,        "reswidth_theta_vs_pt");
  book(m_reswidth_vs_pt_z0_sin_theta, "reswidth_z0*sin(theta)_vs_pt");
  book(m_reswidth_vs_pt_qopt,         "reswidth_qopt_vs_pt");

  m_resptPlots[0] = m_reswidth_vs_pt_d0;
  m_resptPlots[1] = m_reswidth_vs_pt_z0;
  m_resptPlots[2] = m_reswidth_vs_pt_phi;
  m_resptPlots[3] = m_reswidth_vs_pt_theta;
  m_resptPlots[4] = m_reswidth_vs_pt_z0_sin_theta;
  m_resptPlots[5] = m_reswidth_vs_pt_qopt;

  book(m_pullbase_d0,           "pull_d0_vs_eta");
  book(m_pullbase_z0,           "pull_z0_vs_eta");
  book(m_pullbase_phi,          "pull_phi_vs_eta");
  book(m_pullbase_theta,        "pull_theta_vs_eta");
  book(m_pullbase_z0_sin_theta, "pull_z0*sin(theta)_vs_eta");
  book(m_pullbase_qopt,         "pull_qopt_vs_eta");

  m_pullbasePlots[0] = m_pullbase_d0;
  m_pullbasePlots[1] = m_pullbase_z0;
  m_pullbasePlots[2] = m_pullbase_phi;
  m_pullbasePlots[3] = m_pullbase_theta;
  m_pullbasePlots[4] = m_pullbase_z0_sin_theta;
  m_pullbasePlots[5] = m_pullbase_qopt;

  book(m_pullmean_d0,           "pullmean_d0_vs_eta");
  book(m_pullmean_z0,           "pullmean_z0_vs_eta");
  book(m_pullmean_phi,          "pullmean_phi_vs_eta");
  book(m_pullmean_theta,        "pullmean_theta_vs_eta");
  book(m_pullmean_z0_sin_theta, "pullmean_z0*sin(theta)_vs_eta");
  book(m_pullmean_qopt,         "pullmean_qopt_vs_eta");

  m_pullmeanPlots[0] = m_pullmean_d0;
  m_pullmeanPlots[1] = m_pullmean_z0;
  m_pullmeanPlots[2] = m_pullmean_phi;
  m_pullmeanPlots[3] = m_pullmean_theta;
  m_pullmeanPlots[4] = m_pullmean_z0_sin_theta;
  m_pullmeanPlots[5] = m_pullmean_qopt;

  book(m_pullwidth_d0,           "pullwidth_d0_vs_eta");
  book(m_pullwidth_z0,           "pullwidth_z0_vs_eta");
  book(m_pullwidth_phi,          "pullwidth_phi_vs_eta");
  book(m_pullwidth_theta,        "pullwidth_theta_vs_eta");
  book(m_pullwidth_z0_sin_theta, "pullwidth_z0*sin(theta)_vs_eta");
  book(m_pullwidth_qopt,         "pullwidth_qopt_vs_eta");

  m_pullwidthPlots[0] = m_pullwidth_d0;
  m_pullwidthPlots[1] = m_pullwidth_z0;
  m_pullwidthPlots[2] = m_pullwidth_phi;
  m_pullwidthPlots[3] = m_pullwidth_theta;
  m_pullwidthPlots[4] = m_pullwidth_z0_sin_theta;
  m_pullwidthPlots[5] = m_pullwidth_qopt;

  book(m_pull_d0,           "pull_d0");
  book(m_pull_z0,           "pull_z0");
  book(m_pull_phi,          "pull_phi");
  book(m_pull_theta,        "pull_theta");
  book(m_pull_z0_sin_theta, "pull_z0_sin_theta");
  book(m_pull_qopt,         "pull_qopt");
  
  m_pullPlots[0] = m_pull_d0;
  m_pullPlots[1] = m_pull_z0;
  m_pullPlots[2] = m_pull_phi;
  m_pullPlots[3] = m_pull_theta;
  m_pullPlots[4] = m_pull_z0_sin_theta;
  m_pullPlots[5] = m_pull_qopt;  
}

void
InDetPerfPlot_res::fill(const xAOD::TrackParticle &trkprt, const xAOD::TruthParticle &truthprt) {
  double truth_eta = truthprt.eta();  // eta of the truthParticle
  double truth_pt = truthprt.pt();    // pt of the truthParticle
  double log_trupt = Log10(truth_pt) - 3.0;

  float truth_charge = 1;

  double trkParticleParams[NPARAMS];

  trkParticleParams[D0] = trkprt.d0();
  trkParticleParams[Z0] = trkprt.z0();
  trkParticleParams[PHI] = trkprt.phi0();
  trkParticleParams[THETA] = trkprt.theta();
  trkParticleParams[Z0SIN_THETA] = trkprt.z0() * std::sin(trkprt.theta());
  // trkParticleParams[QOVERP] = trkprt.qOverP();
  trkParticleParams[QOPT] = (trkprt.qOverP()) * 1000. / sin(trkprt.theta()); // This switches it to the "qOverPt" PRTT
                                                                             // uses

  if (trkParticleParams[PHI] < 0) {
    trkParticleParams[PHI] += 2 * CLHEP::pi;  // Add in the 2*pi correction for negative phi, as in PRTT
  }
  for (unsigned int var(0); var != NPARAMS; ++var) {
    const std::string varName = m_paramNames.at(var);
    const std::string errName = varName + std::string("err");
    float trackParameter = trkParticleParams[var];                      // needed for all of them
    bool truthIsAvailable = truthprt.isAvailable<float>(varName);
    if (varName == "qopt") {
      truthIsAvailable = truthprt.isAvailable<float>("qOverP");                      // need to get q/pt to actually
                                                                                     // fill
    }
    bool sigmaIsAvailable = trkprt.isAvailable<float>(errName);
    if (varName == "z0*sin(theta)") {
      truthIsAvailable = (truthprt.isAvailable<float>("z0") and truthprt.isAvailable<float>("theta"));
      sigmaIsAvailable = (trkprt.isAvailable<float>("z0err") and trkprt.isAvailable<float>("thetaerr"));
      if (truthIsAvailable and sigmaIsAvailable){
      //ATH_MSG_INFO("yeay! truth and sigma are available");
      }
    }
    if (truthIsAvailable) {  // get the corresponding truth variable, only Fill if it exists
      float truthParameter = 0;
      float deviation(0);
      if (m_meanbasePlots[var]) {
        if (var == QOPT) {
          truthParameter = (truthprt.auxdata< float >("qOverP")) * 1000. / sin(truthprt.auxdata< float >("theta"));
          if (truthParameter < 0) {
            truth_charge = -1;
          }
          deviation = (trackParameter - truthParameter) * fabs(truth_pt / 1000.) * truth_charge;  // Used to match PRTT
                                                                                                  // version
        }else if (var == Z0SIN_THETA) {
          truthParameter = truthprt.auxdata< float >("z0") * std::sin(truthprt.auxdata< float >("theta"));
          deviation = trackParameter - truthParameter;
        }else {
          truthParameter = (truthprt.auxdata< float >(varName));
          if ((var == PHI)and(truthParameter < 0)) {
            truthParameter += 2 * CLHEP::pi; // add in 2*pi correction for negative phi, as in PRTT
          }
          deviation = trackParameter - truthParameter;
        }
        fillHisto(m_meanbasePlots[var],truth_eta, deviation);
	      fillHisto(m_mean_vs_ptbasePlots[var],log_trupt, deviation);
	      //ATH_MSG_INFO("Filling "<<varName<<" "<<truth_eta<<" "<<deviation);
      }
      if (sigmaIsAvailable) {
        //ATH_MSG_INFO("sigma exists for "<<varName);
        float sigma(0);
        if (var == Z0SIN_THETA) {
          float z0_sigma = (trkprt.auxdata< float >("z0err"));
          float theta_sigma = (trkprt.auxdata< float >("thetaerr"));
          const float theta = trkprt.theta();
          const float cosTheta = std::cos(theta);
          const float sinTheta = std::sin(theta);
          const float z0 = trkprt.z0();
          const float sigmaSq = ((z0_sigma * sinTheta) * (z0_sigma * sinTheta)) +
                                ((z0 * theta_sigma * cosTheta) * (z0 * theta_sigma * cosTheta));
          sigma = std::sqrt(sigmaSq);
        } else {
          sigma = (trkprt.auxdata< float >(errName));
        }
        if (m_pullPlots[var]) {
          float pull(0);
          if (var == QOPT) {
            pull = ((trackParameter - truthParameter) * truth_charge / sigma); // used to match PRTT version
          }else {
            pull = ((trackParameter - truthParameter) / sigma);
          }
          fillHisto(m_pullPlots[var],pull);
          fillHisto(m_pullbasePlots[var],truth_eta, pull);
          //ATH_MSG_INFO("Filling "<<varName<<" "<<truth_eta<<" "<<pull);
        }
      }
    }// REAL END OF IF(TRUTHISAVAILABLE) STATEMENT
  }
}

void
InDetPerfPlot_res::Refinement(TH1D *temp, std::string width, int var, int j, const std::vector<TH1 *> &tvec,
                              const std::vector<TH1 *> &rvec) {
  if (temp->GetXaxis()->TestBit(TAxis::kAxisRange)) {
    // remove range if set previously
    temp->GetXaxis()->SetRange();
    temp->ResetStats();
  }
  double mean(0.0), mean_error(0.0), prim_RMS(0.0), sec_RMS(0.0), RMS_error(0.0);
  if (temp->GetEntries() != 0.0) {
    mean = temp->GetMean();
    prim_RMS = temp->GetRMS();
    mean_error = temp->GetMeanError();
    RMS_error = temp->GetRMSError();
    if (width == "iterative_convergence") {
      sec_RMS = prim_RMS + 1.0;
      while (fabs(sec_RMS - prim_RMS) > 0.001) {
        prim_RMS = temp->GetRMS();
        double aymin = -3.0 * prim_RMS;
        double aymax = 3.0 * prim_RMS;
        if (aymin < temp->GetBinLowEdge(1)) {
          aymin = temp->GetBinLowEdge(1);
        }
        if (aymax > temp->GetBinCenter(temp->GetNbinsX())) {
          aymax = temp->GetBinCenter(temp->GetNbinsX());
        }
        temp->SetAxisRange(aymin, aymax);
        mean = temp->GetMean();
        sec_RMS = temp->GetRMS();
      }
      mean_error = temp->GetMeanError();
      RMS_error = temp->GetRMSError();
    }else if (width == "gaussian") {
      TFitResultPtr r = temp->Fit("gaus", "QS0");
      if (r.Get() and r->Status() % 1000 == 0) {
        mean = r->Parameter(1);
        mean_error = r->ParError(1);
        sec_RMS = r->Parameter(2);
        RMS_error = r->ParError(2);
      }
    }else if (width == "fusion") {
      sec_RMS = prim_RMS + 1.0;
      double aymin = temp->GetBinLowEdge(1);
      double aymax = temp->GetBinCenter(temp->GetNbinsX());
      while ((fabs(sec_RMS - prim_RMS) > 0.001)) {
        prim_RMS = temp->GetRMS();
        aymin = -3.0 * prim_RMS;
        aymax = 3.0 * prim_RMS;
        if (aymin < temp->GetBinLowEdge(1)) {
          aymin = temp->GetBinLowEdge(1);
        }
        if (aymax > temp->GetBinCenter(temp->GetNbinsX())) {
          aymax = temp->GetBinCenter(temp->GetNbinsX());
        }
        temp->SetAxisRange(aymin, aymax);
        mean = temp->GetMean();
        sec_RMS = temp->GetRMS();
      }
      TFitResultPtr r = temp->Fit("gaus", "QS0", "", aymin, aymax);
      if (r.Get() and r->Status() % 1000 == 0) {
        mean = r->Parameter(1);
        mean_error = r->ParError(1);
        sec_RMS = r->Parameter(2);
        RMS_error = r->ParError(2);
      }
    }
  }
  (tvec[var])->SetBinContent(j, mean);
  (tvec[var])->SetBinError(j, mean_error);
  (rvec[var])->SetBinContent(j, sec_RMS);
  (rvec[var])->SetBinError(j, RMS_error);
}

void
InDetPerfPlot_res::finalizePlots() {
  // switch for changing between iterative convergence & gaussian fit methods
  // iterative convergence with 3*RMS works better than 5*RMS
  std::string width_method = "iterative_convergence";
  std::string pull_width_method = "gaussian";
  unsigned int ptBins(0);
  if (m_mean_vs_ptbasePlots[0]) {
    ptBins = m_mean_vs_ptPlots[0]->GetNbinsX();
  } else {
    ATH_MSG_WARNING("InDetPerfPlot_res::finalizePlots(): null pointer for histogram, likely it wasn't booked, possibly due to missing histogram definition");
    return;
  }
  for (unsigned int var(0); var != NPARAMS; ++var) {
    if (m_meanPlots[var]){
      unsigned int etaBins = m_meanPlots[var]->GetNbinsX();
      auto & meanbasePlot = m_meanbasePlots[var];
      auto & pullbasePlot = m_pullbasePlots[var];
      for (unsigned int j = 1; j <= etaBins; j++) {
        // Create dummy histo w/ content from TH2 in relevant eta bin
        TH1D *temp = meanbasePlot->ProjectionY(Form("%s_projy_bin%d", "Big_Histo", j), j, j);
        TH1D *temp_pull = pullbasePlot->ProjectionY(Form("%s_projy_bin%d", "Pull_Histo", j), j, j);
        Refinement(temp, width_method, var, j, m_meanPlots, m_resoPlots);
        Refinement(temp_pull, pull_width_method, var, j, m_pullmeanPlots, m_pullwidthPlots);
      }
      auto & mean_vs_ptbasePlot = m_mean_vs_ptbasePlots[var];
      for (unsigned int i = 1; i <= ptBins; i++) {
        TH1D *temp = mean_vs_ptbasePlot->ProjectionY(Form("%s_projy_bin%d", "Big_Histo", i), i, i);
        Refinement(temp, width_method, var, i, m_mean_vs_ptPlots, m_resptPlots);
      }
    } 
  }
}
