/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/HelperFunctions.h"

#include <vector>

//_____________________________________________________________________________
MvaTESEvaluator::MvaTESEvaluator(const std::string& name)
  : TauRecToolBase(name)
{
}

//_____________________________________________________________________________
MvaTESEvaluator::~MvaTESEvaluator()
{
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::initialize(){
  
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::initReader(std::unique_ptr<MVAUtils::BDT>& reader,
                                       std::map<TString, float*>& availableVars,
                                       MvaInputVariables& vars) const {

  // Declare input variables to the reader
  if(!m_in_trigger) {
    availableVars.insert( std::make_pair("TauJetsAuxDyn.mu", &vars.mu) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.nVtxPU", &vars.nVtxPU) );
    
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanCenterLambda", &vars.center_lambda) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanFirstEngDens", &vars.first_eng_dens) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanSecondLambda", &vars.second_lambda) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanPresamplerFrac", &vars.presampler_frac) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanEMProbability", &vars.eprobability) );
    
    availableVars.insert( std::make_pair("TauJetsAuxDyn.pt_combined", &vars.ptCombined) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ptDetectorAxis/TauJetsAuxDyn.pt_combined", &vars.ptLC_D_ptCombined) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ptPanTauCellBased/TauJetsAuxDyn.pt_combined", &vars.ptConstituent_D_ptCombined) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.etaPanTauCellBased", &vars.etaConstituent) );
    
    availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_BDTValue_1p0n_vs_1p1n", &vars.PanTauBDT_1p0n_vs_1p1n) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_BDTValue_1p1n_vs_1pXn", &vars.PanTauBDT_1p1n_vs_1pXn) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_BDTValue_3p0n_vs_3pXn", &vars.PanTauBDT_3p0n_vs_3pXn) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.nTracks", &vars.nTracks) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.PFOEngRelDiff", &vars.PFOEngRelDiff) );
    
    // Spectator variables declared in the training have to be added to the reader, too
    availableVars.insert( std::make_pair("TauJetsAuxDyn.truthPtVis", &vars.truthPtVis) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.pt", &vars.pt) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ptPanTauCellBased", &vars.ptPanTauCellBased) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ptDetectorAxis", &vars.ptDetectorAxis) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.truthDecayMode", &vars.truthDecayMode) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_DecayMode", &vars.PanTau_DecayMode) );
  }
  else {
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.mu", &vars.mu) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanCenterLambda", &vars.center_lambda) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanFirstEngDens", &vars.first_eng_dens) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanSecondLambda", &vars.second_lambda) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanPresamplerFrac", &vars.presampler_frac) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanEMProbability", &vars.eprobability) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.LeadClusterFrac", &vars.lead_cluster_frac) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.UpsilonCluster", &vars.upsilon_cluster) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ptDetectorAxis", &vars.ptDetectorAxis) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.etaDetectorAxis", &vars.etaDetectorAxis) );
  }

  std::string weightFile = find_file(m_sWeightFileName);

  reader = tauRecTools::configureMVABDT( availableVars, weightFile.c_str() );
  if(reader==nullptr) {
    ATH_MSG_FATAL("Couldn't configure MVA");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::execute(xAOD::TauJet& xTau) const {

  std::unique_ptr<MVAUtils::BDT> reader{nullptr};
  std::map<TString, float*> availableVars;
  MvaInputVariables vars;
  if (initReader(reader, availableVars, vars) == StatusCode::FAILURE)
   return StatusCode::FAILURE;

  // Retrieve event info
  const SG::AuxElement::ConstAccessor<float> acc_mu("mu");
  const SG::AuxElement::ConstAccessor<int> acc_nVtxPU("nVtxPU");
  vars.mu = acc_mu(xTau);
  vars.nVtxPU = acc_nVtxPU(xTau);

  // Retrieve cluster moments
  xTau.detail(xAOD::TauJetParameters::ClustersMeanCenterLambda, vars.center_lambda);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanFirstEngDens, vars.first_eng_dens);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanEMProbability,vars.eprobability);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanSecondLambda, vars.second_lambda);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanPresamplerFrac, vars.presampler_frac);

  if(!m_in_trigger) {

    // Retrieve pantau and LC-precalib TES
    vars.etaConstituent = xTau.etaPanTauCellBased();
    float ptLC = xTau.ptDetectorAxis();
    float ptConstituent = xTau.ptPanTauCellBased();
    const SG::AuxElement::ConstAccessor<float> acc_pt_combined("pt_combined");
    vars.ptCombined = acc_pt_combined(xTau);

    if(vars.ptCombined>0.) {
      vars.ptLC_D_ptCombined = ptLC / vars.ptCombined;
      vars.ptConstituent_D_ptCombined = ptConstituent / vars.ptCombined;
    }
    else {
      xTau.setP4(xAOD::TauJetParameters::FinalCalib, 1., vars.etaConstituent, xTau.phiPanTauCellBased(), 0);
      // apply MVA calibration as default
      xTau.setP4(1., vars.etaConstituent, xTau.phiPanTauCellBased(), 0);
      return StatusCode::SUCCESS;
    }

    // Retrieve substructure info
    const SG::AuxElement::ConstAccessor<float> acc_PanTauBDT_1p0n_vs_1p1n("PanTau_BDTValue_1p0n_vs_1p1n");
    const SG::AuxElement::ConstAccessor<float> acc_PanTauBDT_1p1n_vs_1pXn("PanTau_BDTValue_1p1n_vs_1pXn");
    const SG::AuxElement::ConstAccessor<float> acc_PanTauBDT_3p0n_vs_3pXn("PanTau_BDTValue_3p0n_vs_3pXn");
    vars.PanTauBDT_1p0n_vs_1p1n = acc_PanTauBDT_1p0n_vs_1p1n(xTau);
    vars.PanTauBDT_1p1n_vs_1pXn = acc_PanTauBDT_1p1n_vs_1pXn(xTau);
    vars.PanTauBDT_3p0n_vs_3pXn = acc_PanTauBDT_3p0n_vs_3pXn(xTau);
    vars.nTracks = (float)xTau.nTracks();
    xTau.detail(xAOD::TauJetParameters::PFOEngRelDiff, vars.PFOEngRelDiff);
    
    float ptMVA = float( vars.ptCombined * reader->GetResponse() );
    if(ptMVA<1) ptMVA=1;
    xTau.setP4(xAOD::TauJetParameters::FinalCalib, ptMVA, vars.etaConstituent, xTau.phiPanTauCellBased(), 0);

    // apply MVA calibration as default
    xTau.setP4(ptMVA, vars.etaConstituent, xTau.phiPanTauCellBased(), 0);
  }
  else {

    vars.ptDetectorAxis = xTau.ptDetectorAxis();
    vars.etaDetectorAxis = xTau.etaDetectorAxis();

    const SG::AuxElement::ConstAccessor<float> acc_UpsilonCluster("UpsilonCluster");
    const SG::AuxElement::ConstAccessor<float> acc_LeadClusterFrac("LeadClusterFrac");
    vars.upsilon_cluster = acc_UpsilonCluster(xTau);
    vars.lead_cluster_frac = acc_LeadClusterFrac(xTau);

    float ptMVA = float( vars.ptDetectorAxis * reader->GetResponse() );
    if(ptMVA<1) ptMVA=1;

    // this may have to be changed if we apply a calo-only MVA calibration first, followed by a calo+track MVA calibration
    // in which case, the calo-only would be TauJetParameters::TrigCaloOnly, and the final one TauJetParameters::FinalCalib
    xTau.setP4(xAOD::TauJetParameters::FinalCalib, ptMVA, vars.etaDetectorAxis, xTau.phiDetectorAxis(), 0);
    
    // apply MVA calibration
    xTau.setP4(ptMVA, vars.etaDetectorAxis, xTau.phiDetectorAxis(), 0);
  }
  
  ATH_MSG_DEBUG("final calib:" << xTau.pt() << " " << xTau.eta() << " " << xTau.phi() << " " << xTau.e());

  return StatusCode::SUCCESS;
}
