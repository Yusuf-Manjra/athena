/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauJetRNNUtils.h"
#include "tauRecTools/HelperFunctions.h"

#define GeV 1000

namespace TauJetRNNUtils {

VarCalc::VarCalc() : asg::AsgMessaging("TauJetRNNUtils::VarCalc") {
}

bool VarCalc::compute(const std::string &name, const xAOD::TauJet &tau,
                      double &out) const {
    // Retrieve calculator function
    ScalarCalc func = nullptr;
    try {
        func = m_scalar_map.at(name);
    } catch (const std::out_of_range &e) {
        ATH_MSG_ERROR("Variable '" << name << "' not defined");
        throw;
    }

    // Calculate variable
    return func(tau, out);
}

bool VarCalc::compute(const std::string &name, const xAOD::TauJet &tau,
                      const std::vector<const xAOD::TauTrack *> &tracks,
                      std::vector<double> &out) const {
    out.clear();

    // Retrieve calculator function
    TrackCalc func = nullptr;
    try {
        func = m_track_map.at(name);
    } catch (const std::out_of_range &e) {
        ATH_MSG_ERROR("Variable '" << name << "' not defined");
        throw;
    }

    // Calculate variables for selected tracks
    bool success = true;
    double value;
    for (const auto trk : tracks) {
        success = success && func(tau, *trk, value);
        out.push_back(value);
    }

    return success;
}

bool VarCalc::compute(const std::string &name, const xAOD::TauJet &tau,
                      const std::vector<xAOD::CaloVertexedTopoCluster> &clusters,
                      std::vector<double> &out) const {
    out.clear();

    // Retrieve calculator function
    ClusterCalc func = nullptr;
    try {
        func = m_cluster_map.at(name);
    } catch (const std::out_of_range &e) {
        ATH_MSG_ERROR("Variable '" << name << "' not defined");
        throw;
    }

    // Calculate variables for selected clusters
    bool success = true;
    double value;
    for (const auto cluster : clusters) {
        success = success && func(tau, cluster, value);
        out.push_back(value);
    }

    return success;
}

void VarCalc::insert(const std::string &name, ScalarCalc func) {
    if (!func) {
        throw std::invalid_argument("Nullptr passed to VarCalc::insert");
    }
    m_scalar_map[name] = func;
}

void VarCalc::insert(const std::string &name, TrackCalc func) {
    if (!func) {
        throw std::invalid_argument("Nullptr passed to VarCalc::insert");
    }
    m_track_map[name] = func;
}

void VarCalc::insert(const std::string &name, ClusterCalc func) {
    if (!func) {
        throw std::invalid_argument("Nullptr passed to VarCalc::insert");
    }
    m_cluster_map[name] = func;
}

std::unique_ptr<VarCalc> get_default_calculator() {
    auto calc = std::make_unique<VarCalc>();

    // Scalar variable calculator functions
    calc->insert("centFrac", Variables::centFrac);
    calc->insert("etOverPtLeadTrk", Variables::etOverPtLeadTrk);
    calc->insert("innerTrkAvgDist", Variables::innerTrkAvgDist);
    calc->insert("absipSigLeadTrk", Variables::absipSigLeadTrk);
    calc->insert("SumPtTrkFrac", Variables::SumPtTrkFrac);
    calc->insert("EMPOverTrkSysP", Variables::EMPOverTrkSysP);
    calc->insert("ptRatioEflowApprox", Variables::ptRatioEflowApprox);
    calc->insert("mEflowApprox", Variables::mEflowApprox);
    calc->insert("dRmax", Variables::dRmax);
    calc->insert("trFlightPathSig", Variables::trFlightPathSig);
    calc->insert("massTrkSys", Variables::massTrkSys);
    calc->insert("pt", Variables::pt);
    calc->insert("ptDetectorAxis", Variables::ptDetectorAxis);
    calc->insert("ptIntermediateAxis", Variables::ptIntermediateAxis);
    //---added for the eVeto
    calc->insert("ptJetSeed_log",              Variables::ptJetSeed_log  );
    calc->insert("absleadTrackEta",            Variables::absleadTrackEta  );
    calc->insert("leadTrackDeltaEta",          Variables::leadTrackDeltaEta);
    calc->insert("leadTrackDeltaPhi",          Variables::leadTrackDeltaPhi);
    calc->insert("EMFracFixed",                Variables::EMFracFixed      );
    calc->insert("etHotShotWinOverPtLeadTrk",  Variables::etHotShotWinOverPtLeadTrk);
    calc->insert("hadLeakFracFixed",           Variables::hadLeakFracFixed);
    calc->insert("PSFrac",                     Variables::PSFrac);
    calc->insert("ClustersMeanCenterLambda",   Variables::ClustersMeanCenterLambda  );
    calc->insert("ClustersMeanFirstEngDens",   Variables::ClustersMeanFirstEngDens  );
    calc->insert("ClustersMeanPresamplerFrac", Variables::ClustersMeanPresamplerFrac);

    // Track variable calculator functions
    calc->insert("pt_log", Variables::Track::pt_log);
    calc->insert("pt_jetseed_log", Variables::Track::pt_jetseed_log);
    calc->insert("d0_abs_log", Variables::Track::d0_abs_log);
    calc->insert("z0sinThetaTJVA_abs_log", Variables::Track::z0sinThetaTJVA_abs_log);
    calc->insert("dEta", Variables::Track::dEta);
    calc->insert("dPhi", Variables::Track::dPhi);
    calc->insert("nInnermostPixelHits", Variables::Track::nInnermostPixelHits);
    calc->insert("nPixelHits", Variables::Track::nPixelHits);
    calc->insert("nSCTHits", Variables::Track::nSCTHits);
    calc->insert("nIBLHitsAndExp", Variables::Track::nIBLHitsAndExp);
    calc->insert("nPixelHitsPlusDeadSensors", Variables::Track::nPixelHitsPlusDeadSensors);
    calc->insert("nSCTHitsPlusDeadSensors", Variables::Track::nSCTHitsPlusDeadSensors);
    calc->insert("eProbabilityHT", Variables::Track::eProbabilityHT);

    // Cluster variable calculator functions
    calc->insert("et_log", Variables::Cluster::et_log);
    calc->insert("pt_jetseed_log", Variables::Cluster::pt_jetseed_log);
    calc->insert("dEta", Variables::Cluster::dEta);
    calc->insert("dPhi", Variables::Cluster::dPhi);
    calc->insert("SECOND_R", Variables::Cluster::SECOND_R);
    calc->insert("SECOND_LAMBDA", Variables::Cluster::SECOND_LAMBDA);
    calc->insert("CENTER_LAMBDA", Variables::Cluster::CENTER_LAMBDA);
    //---added for the eVeto
    calc->insert("SECOND_LAMBDAOverClustersMeanSecondLambda", Variables::Cluster::SECOND_LAMBDAOverClustersMeanSecondLambda);
    calc->insert("CENTER_LAMBDAOverClustersMeanCenterLambda", Variables::Cluster::CENTER_LAMBDAOverClustersMeanCenterLambda);
    calc->insert("FirstEngDensOverClustersMeanFirstEngDens" , Variables::Cluster::FirstEngDensOverClustersMeanFirstEngDens);
    return calc;
}


namespace Variables {
using TauDetail = xAOD::TauJetParameters::Detail;

bool centFrac(const xAOD::TauJet &tau, double &out) {
    float centFrac;
    const auto success = tau.detail(TauDetail::centFrac, centFrac);
    out = std::min(centFrac, 1.0f);
    return success;
}

bool etOverPtLeadTrk(const xAOD::TauJet &tau, double &out) {
    float etOverPtLeadTrk;
    const auto success = tau.detail(TauDetail::etOverPtLeadTrk,
                                    etOverPtLeadTrk);
    out = std::log10(std::max(etOverPtLeadTrk, 0.1f));
    return success;
}

bool innerTrkAvgDist(const xAOD::TauJet &tau, double &out) {
    float innerTrkAvgDist;
    const auto success = tau.detail(TauDetail::innerTrkAvgDist,
                                    innerTrkAvgDist);
    out = innerTrkAvgDist;
    return success;
}

bool absipSigLeadTrk(const xAOD::TauJet &tau, double &out) {
    float ipSigLeadTrk = (tau.nTracks()>0) ? tau.track(0)->d0SigTJVA() : 0.;
    out = std::min(std::abs(ipSigLeadTrk), 30.0f);
    return true;
}

bool SumPtTrkFrac(const xAOD::TauJet &tau, double &out) {
    float SumPtTrkFrac;
    const auto success = tau.detail(TauDetail::SumPtTrkFrac, SumPtTrkFrac);
    out = SumPtTrkFrac;
    return success;
}

bool EMPOverTrkSysP(const xAOD::TauJet &tau, double &out) {
    float EMPOverTrkSysP;
    const auto success = tau.detail(TauDetail::EMPOverTrkSysP, EMPOverTrkSysP);
    out = std::log10(std::max(EMPOverTrkSysP, 1e-3f));
    return success;
}

bool ptRatioEflowApprox(const xAOD::TauJet &tau, double &out) {
    float ptRatioEflowApprox;
    const auto success = tau.detail(TauDetail::ptRatioEflowApprox,
                                    ptRatioEflowApprox);
    out = std::min(ptRatioEflowApprox, 4.0f);
    return success;
}

bool mEflowApprox(const xAOD::TauJet &tau, double &out) {
    float mEflowApprox;
    const auto success = tau.detail(TauDetail::mEflowApprox, mEflowApprox);
    out = std::log10(std::max(mEflowApprox, 140.0f));
    return success;
}

bool dRmax(const xAOD::TauJet &tau, double &out) {
    float dRmax;
    const auto success = tau.detail(TauDetail::dRmax, dRmax);
    out = dRmax;
    return success;
}

bool trFlightPathSig(const xAOD::TauJet &tau, double &out) {
    float trFlightPathSig;
    const auto success = tau.detail(TauDetail::trFlightPathSig,
                                    trFlightPathSig);
    out = std::log10(std::max(trFlightPathSig, 0.01f));
    return success;
}

bool massTrkSys(const xAOD::TauJet &tau, double &out) {
    float massTrkSys;
    const auto success = tau.detail(TauDetail::massTrkSys, massTrkSys);
    out = std::log10(std::max(massTrkSys, 140.0f));
    return success;
}

bool pt(const xAOD::TauJet &tau, double &out) {
    out = std::log10(std::min(tau.pt() / GeV, 100.0));
    return true;
}

bool ptDetectorAxis(const xAOD::TauJet &tau, double &out) {
    out = std::log10(std::min(tau.ptDetectorAxis() / GeV, 100.0));
    return true;
}

bool ptIntermediateAxis(const xAOD::TauJet &tau, double &out) {
    out = std::log10(std::min(tau.ptIntermediateAxis() /GeV, 100.0));
    return true;
}

bool ptJetSeed_log(const xAOD::TauJet &tau, double &out) {
  out = std::log10(std::max(tau.ptJetSeed(), 1e-3));
  return true;
}

bool absleadTrackEta(const xAOD::TauJet &tau, double &out){
  out = std::max(0.f, tau.auxdata<float>("ABS_ETA_LEAD_TRACK"));
  return true;
}

bool leadTrackDeltaEta(const xAOD::TauJet &tau, double &out){
  out = std::max(0.f, tau.auxdata<float>("TAU_ABSDELTAETA"));
  return true;
}

bool leadTrackDeltaPhi(const xAOD::TauJet &tau, double &out){
  out = std::max(0.f, tau.auxdata<float>("TAU_ABSDELTAPHI"));
  return true;
}

bool EMFracFixed(const xAOD::TauJet &tau, double &out){
  float emFracFized = tau.auxdata<float>("EMFracFixed");
  out = std::max(emFracFized, 0.0f);
  return true;
}

bool etHotShotWinOverPtLeadTrk(const xAOD::TauJet &tau, double &out){
  float etHotShotWinOverPtLeadTrk = tau.auxdata<float>("etHotShotWinOverPtLeadTrk");
  out = std::max(etHotShotWinOverPtLeadTrk, 1e-6f);
  out = std::log10(out);
  return true;
}

bool hadLeakFracFixed(const xAOD::TauJet &tau, double &out){
  float hadLeakFracFixed = tau.auxdata<float>("hadLeakFracFixed");
  out = std::max(0.f, hadLeakFracFixed);
  return true;
}

bool PSFrac(const xAOD::TauJet &tau, double &out){
  float PSFrac;
  const auto success = tau.detail(TauDetail::PSSFraction, PSFrac);
  out = std::max(0.f,PSFrac);
  
  return success;
}

bool ClustersMeanCenterLambda(const xAOD::TauJet &tau, double &out){
  float ClustersMeanCenterLambda;
  const auto success = tau.detail(TauDetail::ClustersMeanCenterLambda, ClustersMeanCenterLambda);
  out = std::max(0.f, ClustersMeanCenterLambda);

  return success;
}

bool ClustersMeanEMProbability(const xAOD::TauJet &tau, double &out){
  float ClustersMeanEMProbability;
  const auto success = tau.detail(TauDetail::ClustersMeanEMProbability, ClustersMeanEMProbability);
  out = std::max(0.f, ClustersMeanEMProbability);
  return success;
}

bool ClustersMeanFirstEngDens(const xAOD::TauJet &tau, double &out){
  float ClustersMeanFirstEngDens;
  const auto success = tau.detail(TauDetail::ClustersMeanFirstEngDens, ClustersMeanFirstEngDens);
  out =  std::max(-10.f, ClustersMeanFirstEngDens);
  return success;
}

bool ClustersMeanPresamplerFrac(const xAOD::TauJet &tau, double &out){
  float ClustersMeanPresamplerFrac;
  const auto success = tau.detail(TauDetail::ClustersMeanPresamplerFrac, ClustersMeanPresamplerFrac);
  out = std::max(0.f, ClustersMeanPresamplerFrac);

  return success;
}

bool ClustersMeanSecondLambda(const xAOD::TauJet &tau, double &out){
  float ClustersMeanSecondLambda;
  const auto success = tau.detail(TauDetail::ClustersMeanSecondLambda, ClustersMeanSecondLambda);
  out = std::max(0.f, ClustersMeanSecondLambda);

  return success;
}

namespace Track {

bool pt_log(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
            double &out) {
    out = std::log10(track.pt());
    return true;
}

bool pt_jetseed_log(const xAOD::TauJet &tau, const xAOD::TauTrack& /*track*/,
                    double &out) {
    out = std::log10(tau.ptJetSeed());
    return true;
}

bool d0_abs_log(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
                double &out) {
    out = std::log10(TMath::Abs(track.d0TJVA()) + 1e-6);
    return true;
}

bool z0sinThetaTJVA_abs_log(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
                            double &out) {
    out = std::log10(TMath::Abs(track.z0sinthetaTJVA()) + 1e-6);
    return true;
}

bool dEta(const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out) {
    out = track.eta() - tau.eta();
    return true;
}

bool dPhi(const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out) {
    out = track.p4().DeltaPhi(tau.p4());
    return true;
}

bool nInnermostPixelHits(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
                         double &out) {
    uint8_t inner_pixel_hits;
    const auto success = track.track()->summaryValue(
        inner_pixel_hits, xAOD::numberOfInnermostPixelLayerHits);
    out = inner_pixel_hits;
    return success;
}

bool nPixelHits(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
                double &out) {
    uint8_t pixel_hits;
    const auto success = track.track()->summaryValue(
        pixel_hits, xAOD::numberOfPixelHits);
    out = pixel_hits;
    return success;
}

bool nSCTHits(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
              double &out) {
    uint8_t sct_hits;
    const auto success = track.track()->summaryValue(
        sct_hits, xAOD::numberOfSCTHits);
    out = sct_hits;
    return success;
}

// same as in tau track classification for trigger
bool nIBLHitsAndExp(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
		    double &out) {
    uint8_t inner_pixel_hits, inner_pixel_exp;
    const auto success1 = track.track()->summaryValue(inner_pixel_hits, xAOD::numberOfInnermostPixelLayerHits);
    const auto success2 = track.track()->summaryValue(inner_pixel_exp, xAOD::expectInnermostPixelLayerHit);
    out =  inner_pixel_exp ? inner_pixel_hits : 1.;
    return success1 && success2;
}

bool nPixelHitsPlusDeadSensors(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
			       double &out) {
    uint8_t pixel_hits, pixel_dead;
    const auto success1 = track.track()->summaryValue(pixel_hits, xAOD::numberOfPixelHits);
    const auto success2 = track.track()->summaryValue(pixel_dead, xAOD::numberOfPixelDeadSensors);
    out = pixel_hits + pixel_dead;
    return success1 && success2;
}

bool nSCTHitsPlusDeadSensors(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
			     double &out) {
    uint8_t sct_hits, sct_dead;
    const auto success1 = track.track()->summaryValue(sct_hits, xAOD::numberOfSCTHits);
    const auto success2 = track.track()->summaryValue(sct_dead, xAOD::numberOfSCTDeadSensors);
    out = sct_hits + sct_dead;
    return success1 && success2;
}

bool eProbabilityHT(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
		    double &out) {

    float tracksEProbabilityHT;
    const auto success =  track.track()->summaryValue( tracksEProbabilityHT, 
						       xAOD::eProbabilityHT);
    out = tracksEProbabilityHT;
    return success;
}

} // namespace Track


namespace Cluster {
using MomentType = xAOD::CaloCluster::MomentType;

bool et_log(const xAOD::TauJet& /*tau*/, const xAOD::CaloVertexedTopoCluster &cluster,
            double &out) {
    out = std::log10(cluster.p4().Et());
    return true;
}

bool pt_jetseed_log(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster& /*cluster*/,
                    double &out) {
    out = std::log10(tau.ptJetSeed());
    return true;
}

bool dEta(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster &cluster,
          double &out) {
    out = cluster.eta() - tau.eta();
    return true;
}

bool dPhi(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster &cluster,
          double &out) {
    out = cluster.p4().DeltaPhi(tau.p4());
    return true;
}

bool SECOND_R(const xAOD::TauJet& /*tau*/, const xAOD::CaloVertexedTopoCluster &cluster,
              double &out) {
    const auto success = cluster.clust().retrieveMoment(MomentType::SECOND_R, out);
    out = std::log10(out + 0.1);
    return success;
}

bool SECOND_LAMBDA(const xAOD::TauJet& /*tau*/, const xAOD::CaloVertexedTopoCluster &cluster,
                   double &out) {
    const auto success = cluster.clust().retrieveMoment(MomentType::SECOND_LAMBDA, out);
    out = std::log10(out + 0.1);
    return success;
}

bool CENTER_LAMBDA(const xAOD::TauJet& /*tau*/, const xAOD::CaloVertexedTopoCluster &cluster,
                   double &out) {
    const auto success = cluster.clust().retrieveMoment(MomentType::CENTER_LAMBDA, out);
    out = std::log10(out + 1e-6);
    return success;
}

bool SECOND_LAMBDAOverClustersMeanSecondLambda(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
  float ClustersMeanSecondLambda = tau.auxdata<float>("ClustersMeanSecondLambda");

  double secondLambda(0);
  const auto success = cluster.clust().retrieveMoment(MomentType::SECOND_LAMBDA, secondLambda);

  out = secondLambda/ClustersMeanSecondLambda;

  return success;
}

bool CENTER_LAMBDAOverClustersMeanCenterLambda(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
  float ClustersMeanCenterLambda = tau.auxdata<float>("ClustersMeanCenterLambda");

  double centerLambda(0);
  const auto success = cluster.clust().retrieveMoment(MomentType::CENTER_LAMBDA, centerLambda);
  if (ClustersMeanCenterLambda == 0.){
    out = 250.;
  }else {
    out = centerLambda/ClustersMeanCenterLambda;
  }

  out = std::min(out, 250.);

  return success;
}


bool FirstEngDensOverClustersMeanFirstEngDens(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
  // the ClustersMeanFirstEngDens is the log10 of the energy weighted average of the First_ENG_DENS 
  // divided by ETot to make it dimension-less, 
  // so we need to evaluate the differance of log10(clusterFirstEngDens/clusterTotalEnergy) and the ClustersMeanFirstEngDens
  double clusterFirstEngDens = 0.0;
  bool status = cluster.clust().retrieveMoment(MomentType::FIRST_ENG_DENS, clusterFirstEngDens);
  if (clusterFirstEngDens < 1e-6) clusterFirstEngDens = 1e-6;

  float clusterTotalEnergy = tau.auxdata<float>("ClusterTotalEnergy");
  if (clusterTotalEnergy < 1e-6) clusterTotalEnergy = 1e-6;

  out = std::log10(clusterFirstEngDens/clusterTotalEnergy) - tau.auxdata<float>("ClustersMeanFirstEngDens");
  
  return status;
}

} // namespace Cluster
} // namespace Variables
} // namespace TauJetRNNUtils
