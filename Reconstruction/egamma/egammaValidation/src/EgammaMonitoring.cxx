/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
 */

#include <memory>

#include "EgammaMonitoring.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "IHistograms.h"


EgammaMonitoring::EgammaMonitoring(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_LooseLH("AsgElectronLikelihoodTool/LooseLH"),
  m_MediumLH("AsgElectronLikelihoodTool/MediumLH"),
  m_TightLH("AsgElectronLikelihoodTool/TightLH"),
  m_IsoFixedCutTight("CP::IsolationSelectionTool/IsoFixedCutTight"),
  m_IsoFixedCutTightTrackOnly("CP::IsolationSelectionTool/IsoFixedCutTightTrackOnly"),
  m_IsoFixedCutTightCaloOnly("CP::IsolationSelectionTool/IsoFixedCutTightCaloOnly"),
  m_IsoFixedCutLoose("CP::IsolationSelectionTool/IsoFixedCutLoose"),
  m_mcTruthClassifier("MCTruthClassifier/MCTruthClassifier") {
  declareProperty("sampleType", m_sampleType = "Unknown", "Descriptive name for the processed type of particle");
}

// ******

StatusCode EgammaMonitoring::initialize() {
  ATH_MSG_INFO("********************  Running over " << m_sampleType << " ******************");
  ATH_CHECK(service("THistSvc", rootHistSvc));

  showerShapesAll = std::make_unique<egammaMonitoring::ShowerShapesHistograms>(
    "showerShapesAll","Shower Shapes ", "/MONITORING/showerShapesAll/", rootHistSvc);

  showerShapes10GeV = std::make_unique<egammaMonitoring::ShowerShapesHistograms>(
    "showerShapes10GeV","Shower Shapes - 10 GeV", "/MONITORING/showerShapes10GeV/", rootHistSvc);

  clusterAll = std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clustersAll","Clusters", "/MONITORING/clusterAll/", rootHistSvc);

  cluster10GeV= std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clusters10GeV","Clusters - 10 GeV", "/MONITORING/cluster10GeV/", rootHistSvc);

  clusterPromptAll = std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clustersPromptAll","Clusters from Prompt", "/MONITORING/clusterPromptAll/", rootHistSvc);

  clusterPrompt10GeV = std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clustersPrompt10GeV","Clusters from Prompt - 10 GeV", "/MONITORING/clusterPrompt10GeV/", rootHistSvc);

  isolationAll = std::make_unique<egammaMonitoring::IsolationHistograms>(
    "isolationAll","Isolation ", "/MONITORING/isolationAll/", rootHistSvc);

  ATH_CHECK(showerShapesAll->initializePlots());
  ATH_CHECK(showerShapes10GeV->initializePlots());
  ATH_CHECK(clusterAll->initializePlots());
  ATH_CHECK(cluster10GeV->initializePlots());
  ATH_CHECK(clusterPromptAll->initializePlots());
  ATH_CHECK(clusterPrompt10GeV->initializePlots());
  ATH_CHECK(isolationAll->initializePlots());

  if ("electron" == m_sampleType) {

    truthElectronAll = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
        "truthElectronAll","All Truth Electrons", "/MONITORING/truthElectronAll/", rootHistSvc);

    truthPromptElectronAll = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
        "truthPromptElectronAll","All Truth Prompt Electrons", "/MONITORING/truthPromptElectronAll/", rootHistSvc);

    truthElectronRecoElectronAll = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
        "truthElectronRecoElectronAll","Truth Electrons Reco Electron", "/MONITORING/truthElectronRecoElectronAll/", rootHistSvc);

    truthPromptElectronWithTrack = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
        "truthPromptElectronWithTrack","Truth Prompt Electrons With Track", "/MONITORING/truthPromptElectronWithTrack/", rootHistSvc);

    truthPromptElectronWithGSFTrack = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
        "truthPromptElectronWithGSFTrack","Truth Prompt Electrons With GSFTrack", "/MONITORING/truthPromptElectronWithGSFTrack/", rootHistSvc);

    truthPromptElectronWithReco = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
        "truthPromptElectronWithReco","Truth Prompt Electrons With GSFTrack", "/MONITORING/truthPromptElectronWithReco/", rootHistSvc);

    recoElectronAll = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::RecoElectronHistograms(
      "recoElectronAll","Electrons Reco Electron", "/MONITORING/recoElectronAll/", rootHistSvc));

    truthRecoElectronLooseLH = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
      "truthRecoElectronLooseLH","LLH Electrons Reco Electron", "/MONITORING/truthRecoElectronLooseLH/", rootHistSvc);

    truthRecoElectronMediumLH = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
        "truthRecoElectronMediumLH","MLH Electrons Reco Electron", "/MONITORING/truthRecoElectronMediumLH/", rootHistSvc);

    truthRecoElectronTightLH = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
      "truthRecoElectronTightLH","TLH Electrons Reco Electron", "/MONITORING/truthRecoElectronTightLH/", rootHistSvc);

    recoElectronIsoFixedCutTight = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
      "recoElectronIsoFixedCutTight","Isolation Fixed Cut Tight Electrons Reco Electron", "/MONITORING/recoElectronIsoFixedCutTight/", rootHistSvc);

    recoElectronIsoFixedCutTightTrackOnly = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
      "recoElectronIsoFixedCutTightTrackOnly","Isolation Fixed Cut Tight Track Only Electrons Reco Electron", "/MONITORING/recoElectronIsoFixedCutTightTrackOnly/", rootHistSvc);

    recoElectronIsoFixedCutLoose = std::make_unique<egammaMonitoring::TruthElectronHistograms>(
      "recoElectronIsoFixedCutLoose","Isolation Fixed Cut Loose Electrons Reco Electron", "/MONITORING/recoElectronIsoFixedCutLoose/", rootHistSvc);

    ATH_CHECK(recoElectronAll->initializePlots());
    ATH_CHECK(truthRecoElectronLooseLH->initializePlots());
    ATH_CHECK(truthRecoElectronMediumLH->initializePlots());
    ATH_CHECK(truthRecoElectronTightLH->initializePlots());
    ATH_CHECK(truthElectronAll->initializePlots());
    ATH_CHECK(truthPromptElectronAll->initializePlots());
    ATH_CHECK(truthElectronRecoElectronAll->initializePlots());
    ATH_CHECK(truthPromptElectronWithTrack->initializePlots());
    ATH_CHECK(truthPromptElectronWithGSFTrack->initializePlots());
    ATH_CHECK(truthPromptElectronWithReco->initializePlots());
    ATH_CHECK(recoElectronIsoFixedCutTight->initializePlots());
    ATH_CHECK(recoElectronIsoFixedCutTightTrackOnly->initializePlots());
    ATH_CHECK(recoElectronIsoFixedCutLoose->initializePlots());

  } // electron Hists

  if ("gamma" == m_sampleType) {


    recoPhotonAll = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::RecoPhotonHistograms(
        "recoPhotonAll","Reco Photon", "/MONITORING/recoPhotonAll/", rootHistSvc));

    clusterConvPhoton = std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clusterConvPhoton","Clusters from Converted Photons", "/MONITORING/clusterConvPhoton/", rootHistSvc);

    clusterConvPhotonSi = std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clusterConvPhotonSi","Clusters from Converted Photons - Si", "/MONITORING/clusterConvPhotonSi/", rootHistSvc);

    clusterConvPhotonSiSi = std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clusterConvPhotonSiSi","Clusters from Converted Photons - SiSi", "/MONITORING/clusterConvPhotonSiSi/", rootHistSvc);
   
    clusterConvPhotonTRT = std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clusterConvPhotonTRT","Clusters from Converted Photons - TRT", "/MONITORING/clusterConvPhotonTRT/", rootHistSvc);

    clusterConvPhotonTRTTRT = std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clusterConvPhotonTRTTRT","Clusters from Converted Photons - TRTTRT", "/MONITORING/clusterConvPhotonTRTTRT/", rootHistSvc);
 
    clusterConvPhotonSiTRT = std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clusterConvPhotonSiTRT","Clusters from Converted Photons - SiTRT", "/MONITORING/clusterConvPhotonSiTRT/", rootHistSvc);
    
    
    clusterUnconvPhoton = std::make_unique<egammaMonitoring::ClusterHistograms>(
    "clusterUnconvPhoton","Clusters from Converted Photons", "/MONITORING/clusterUnconvPhoton/", rootHistSvc);


    truthPhotonRecoPhoton = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthPhotonRecoPhoton","truthPhotonRecoPhoton", "/MONITORING/truthPhotonRecoPhoton/", rootHistSvc));

    truthPhotonConvPhoton = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthConvPhoton","truthConvPhoton", "/MONITORING/truthConvPhoton/", rootHistSvc));

    truthPhotonConvRecoConv = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthConvRecoConv","truthConvRecoConv", "/MONITORING/truthConvRecoConv/", rootHistSvc));

    truthPhotonConvRecoConv1Si = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthConvRecoConv1Si","truthConvRecoConv1Si", "/MONITORING/truthConvRecoConv1Si/", rootHistSvc));

    truthPhotonConvRecoConv1TRT = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthConvRecoConv1TRT","truthConvRecoConv1TRT", "/MONITORING/truthConvRecoConv1TRT/", rootHistSvc));

    truthPhotonConvRecoConv2Si = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthConvRecoConv2Si","truthConvRecoConv2Si", "/MONITORING/truthConvRecoConv2Si/", rootHistSvc));

    truthPhotonConvRecoConv2TRT = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthConvRecoConv2TRT","truthConvRecoConv2TRT", "/MONITORING/truthConvRecoConv2TRT/", rootHistSvc));

    truthPhotonConvRecoConv2SiTRT = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthConvRecoConv2SiTRT","truthConvRecoConv2SiTRT", "/MONITORING/truthConvRecoConv2SiTRT/", rootHistSvc));

    truthPhotonConvRecoUnconv= std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthConvRecoUnconv","truthConvRecoUnconv", "/MONITORING/truthConvRecoUnconv/", rootHistSvc));

    truthPhotonUnconvPhoton= std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthUnconvPhoton","truthUnconvPhoton", "/MONITORING/truthUnconvPhoton/", rootHistSvc));

    truthPhotonUnconvRecoConv= std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthUnconvRecoConv","truthUnconvRecoConv", "/MONITORING/truthUnconvRecoConv/", rootHistSvc));

    truthPhotonUnconvRecoUnconv = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
        "truthUnconvRecoUnconv","truthUnconvRecoUnconv", "/MONITORING/truthUnconvRecoUnconv/", rootHistSvc));

    recoPhotonUnconvIsoFixedCutTight = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
    "recoPhotonUnconvIsoFixedCutTight","Isolation Fixed Cut Tight Photons Reco Photon", "/MONITORING/recoPhotonUnconvIsoFixedCutTight/", rootHistSvc));

    recoPhotonUnconvIsoFixedCutTightCaloOnly = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
    "recoPhotonUnconvIsoFixedCutTightCaloOnly","Isolation Fixed Cut Tight Calo Only Photons Reco Photon", "/MONITORING/recoPhotonUnconvIsoFixedCutTightCaloOnly/", rootHistSvc));

    recoPhotonUnconvIsoFixedCutLoose = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
    "recoPhotonUnconvIsoFixedCutLoose","Isolation Fixed Cut Loose Photons Reco Photon", "/MONITORING/recoPhotonUnconvIsoFixedCutLoose/", rootHistSvc));

    recoPhotonConvIsoFixedCutTight = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
    "recoPhotonConvIsoFixedCutTight","Isolation Fixed Cut Tight Photons Reco Photon", "/MONITORING/recoPhotonConvIsoFixedCutTight/", rootHistSvc));

    recoPhotonConvIsoFixedCutTightCaloOnly = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
    "recoPhotonConvIsoFixedCutTightCaloOnly","Isolation Fixed Cut Tight Calo Only Photons Reco Photon", "/MONITORING/recoPhotonConvIsoFixedCutTightCaloOnly/", rootHistSvc));

    recoPhotonConvIsoFixedCutLoose = std::unique_ptr<egammaMonitoring::IHistograms>(new egammaMonitoring::TruthPhotonHistograms(
    "recoPhotonConvIsoFixedCutLoose","Isolation Fixed Cut Loose Photons Reco Photon", "/MONITORING/recoPhotonConvIsoFixedCutLoose/", rootHistSvc));


    ATH_CHECK(recoPhotonAll->initializePlots());
    ATH_CHECK(truthPhotonRecoPhoton->initializePlots());
    ATH_CHECK(truthPhotonConvPhoton->initializePlots());
    ATH_CHECK(truthPhotonConvRecoConv->initializePlots());
    ATH_CHECK(truthPhotonConvRecoConv1Si->initializePlots());
    ATH_CHECK(truthPhotonConvRecoConv1TRT->initializePlots());
    ATH_CHECK(truthPhotonConvRecoConv2Si->initializePlots());
    ATH_CHECK(truthPhotonConvRecoConv2TRT->initializePlots());
    ATH_CHECK(truthPhotonConvRecoConv2SiTRT->initializePlots());
    ATH_CHECK(truthPhotonConvRecoUnconv->initializePlots());
    ATH_CHECK(truthPhotonUnconvPhoton->initializePlots());
    ATH_CHECK(truthPhotonUnconvRecoConv->initializePlots());
    ATH_CHECK(truthPhotonUnconvRecoUnconv->initializePlots());
    ATH_CHECK(recoPhotonUnconvIsoFixedCutTight->initializePlots());
    ATH_CHECK(recoPhotonUnconvIsoFixedCutTightCaloOnly->initializePlots());
    ATH_CHECK(recoPhotonUnconvIsoFixedCutLoose->initializePlots());
    ATH_CHECK(recoPhotonConvIsoFixedCutTight->initializePlots());
    ATH_CHECK(recoPhotonConvIsoFixedCutTightCaloOnly->initializePlots());
    ATH_CHECK(recoPhotonConvIsoFixedCutLoose->initializePlots());

    ATH_CHECK(clusterConvPhoton->initializePlots());
    ATH_CHECK(clusterConvPhotonSi->initializePlots());
    ATH_CHECK(clusterConvPhotonSiSi->initializePlots());
    ATH_CHECK(clusterConvPhotonTRT->initializePlots());
    ATH_CHECK(clusterConvPhotonTRTTRT->initializePlots());
    ATH_CHECK(clusterConvPhotonSiTRT->initializePlots());
    ATH_CHECK(clusterUnconvPhoton->initializePlots());


  } // gamma Hists

  //*****************LLH Requirement********************
  ATH_CHECK(m_LooseLH.setProperty("WorkingPoint", "LooseLHElectron"));
  ATH_CHECK(m_LooseLH.initialize());
  //*****************MLH Requirement********************
  ATH_CHECK(m_MediumLH.setProperty("WorkingPoint", "MediumLHElectron"));
  ATH_CHECK(m_MediumLH.initialize());
  //*****************TLH Requirement********************
  ATH_CHECK(m_TightLH.setProperty("WorkingPoint", "TightLHElectron"));
  ATH_CHECK(m_TightLH.initialize());

  //*****************Iso Requirements********************
  ATH_CHECK(m_IsoFixedCutTight.setProperty("ElectronWP", "FixedCutTight"));
  ATH_CHECK(m_IsoFixedCutTight.setProperty("PhotonWP", "FixedCutTight"));
  ATH_CHECK(m_IsoFixedCutTight.initialize());
  ATH_CHECK(m_IsoFixedCutTightTrackOnly.setProperty("ElectronWP", "FixedCutTightTrackOnly"));
  ATH_CHECK(m_IsoFixedCutTightTrackOnly.initialize());
  ATH_CHECK(m_IsoFixedCutTightCaloOnly.setProperty("PhotonWP", "FixedCutTightCaloOnly"));
  ATH_CHECK(m_IsoFixedCutTightCaloOnly.initialize());
  ATH_CHECK(m_IsoFixedCutLoose.setProperty("ElectronWP", "FixedCutLoose"));
  ATH_CHECK(m_IsoFixedCutLoose.setProperty("PhotonWP", "FixedCutLoose"));
  ATH_CHECK(m_IsoFixedCutLoose.initialize());
  //*****************MC Truth Classifier Requirement********************
  ATH_CHECK(m_mcTruthClassifier.retrieve());

  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring::beginInputFile() {
  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring::firstExecute() {
  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring::execute() {

  // Retrieve things from the event store
  const xAOD::EventInfo *eventInfo = nullptr;
  ANA_CHECK(evtStore()->retrieve(eventInfo, "EventInfo"));
  const float mu = eventInfo->averageInteractionsPerCrossing();

 

  // Retrieve egamma truth particles
  const xAOD::TruthParticleContainer *egTruthParticles = nullptr;
  if (!evtStore()->retrieve(egTruthParticles, "egammaTruthParticles").isSuccess()) {
    ATH_MSG_ERROR("Failed to retrieve egamma truth particle container. Exiting.");
    return StatusCode::FAILURE;
  }

  const xAOD::TruthParticleContainer *truthParticles = nullptr;
  if (!evtStore()->retrieve(truthParticles,
                            "TruthParticles").isSuccess()) { // retrieve arguments: container type, container key
    Error("execute()", "Failed to retrieve Truth Particle container. Exiting.");
    return StatusCode::FAILURE;
  }


  if ("electron" == m_sampleType) {

    // Retrieve electrons
    const xAOD::ElectronContainer *RecoEl = nullptr;
    if (!evtStore()->retrieve(RecoEl, "Electrons").isSuccess()) {
      ATH_MSG_ERROR("Failed to retrieve electron container. Exiting.");
      return StatusCode::FAILURE;
    }

    // InDet track particles:
    const xAOD::TrackParticleContainer *InDetTracks = nullptr;
    if (!evtStore()->retrieve(InDetTracks, "InDetTrackParticles").isSuccess()) {
      ATH_MSG_ERROR( "Failed to retrieve InDetTrackParticles. Exiting.");
      return StatusCode::FAILURE;
    }

    //  track particles:
    const xAOD::TrackParticleContainer *GSFTracks = nullptr;
    if (!evtStore()->retrieve(GSFTracks, "GSFTrackParticles").isSuccess()) {
      ATH_MSG_ERROR( "Failed to retrieve GSFTrackParticles. Exiting.");
      return StatusCode::FAILURE;
    }


    ATH_MSG_DEBUG( "------------ Truth Egamma Container ---------------" );
    for (auto egtruth : *egTruthParticles) {

      if (!egtruth) continue;

      const xAOD::Electron *electron = xAOD::EgammaHelpers::getRecoElectron(egtruth);

      if (!electron) continue;

      clusterPromptAll->fill(*electron,mu);
      if (egtruth->pt() > 10*Gaudi::Units::GeV) {
        clusterPrompt10GeV->fill(*electron,mu);
      }
     

      

    }

    ATH_MSG_DEBUG( "------------ Truth Particles Container ---------------" );
    unsigned int promtpElectronTruthIndex = - 9;
    for (auto truth : *truthParticles) {

      if (!truth) continue;
      if (fabs(truth->pdgId()) != 11) continue;

      auto res = m_mcTruthClassifier->particleTruthClassifier(truth);
      MCTruthPartClassifier::ParticleOrigin TO = res.second;
      MCTruthPartClassifier::ParticleType TT = res.first;

      ATH_MSG_DEBUG( " ******** Truth particle associated to Electron Found: "
                << " STATUS  " << truth->status()
                << " type  " << truth->type()
                << " barcode  " << truth->barcode()
                << " PDG id   " << truth->pdgId()
                << " index    " << truth->index()
                << " TO  " << TO
                << " TT   " << TT
                << " eventNumber  " << eventInfo->eventNumber() );

      // Check if it is the prompt electron
      if (TO == MCTruthPartClassifier::SingleElec &&
          TT == MCTruthPartClassifier::IsoElectron && truth->barcode() == 10001) {
        truthPromptElectronAll->fill(truth);
        promtpElectronTruthIndex = truth->index();

      }


      // Check that it is not from geant4
      if ( TT != MCTruthPartClassifier::NonPrimary) truthElectronAll->fill(truth);

    }

    ATH_MSG_DEBUG( "------------ InDetTracks ---------------" );

    bool foundPromptElectron = false;

    for (auto tp : *InDetTracks) {

      if (!tp) continue;

      uint8_t nPi = 0;

      uint8_t nSCT = 0;

      tp->summaryValue(nPi, xAOD::numberOfPixelHits);

      tp->summaryValue(nSCT, xAOD::numberOfSCTHits);

      if ((nPi + nSCT) < 7) continue;

      const xAOD::TruthParticle *truth = xAOD::TruthHelpers::getTruthParticle(*tp);

      if (!truth || fabs(truth->pdgId()) != 11) continue;


      auto res2 = m_mcTruthClassifier->particleTruthClassifier(truth);
      MCTruthPartClassifier::ParticleOrigin TO = res2.second;
      MCTruthPartClassifier::ParticleType TT = res2.first;
      if (TO == MCTruthPartClassifier::SingleElec &&
          TT == MCTruthPartClassifier::IsoElectron &&
          truth->index() == promtpElectronTruthIndex && !foundPromptElectron ) {

        // we found the track from the prompt electron
        // let's count it
        truthPromptElectronWithTrack->fill(truth);
        foundPromptElectron = true;


      } else {
        const xAOD::TruthParticle *firstElTruth = xAOD::EgammaHelpers::getBkgElectronMother(truth);
        if (!firstElTruth) continue;
        // this is not a prompt electron, we need to check the parents
        // but we need to make sure that we double count if already found the track
        // foundPromptElectron will check that

        if ((firstElTruth->index() == promtpElectronTruthIndex) &&
            (!foundPromptElectron)) {
          truthPromptElectronWithTrack->fill(firstElTruth);
          foundPromptElectron = true;
        }

      }

    }

    ATH_MSG_DEBUG( "------------ GSFTracks ---------------" );

    foundPromptElectron = false;

    for (auto gsf : *GSFTracks) {

      if (!gsf) continue;

      const xAOD::TruthParticle *truth = xAOD::TruthHelpers::getTruthParticle(*gsf);

      if (!truth) continue;

      uint8_t nPi = 0;

      uint8_t nSCT = 0;

      gsf->summaryValue(nPi, xAOD::numberOfPixelHits);

      gsf->summaryValue(nSCT, xAOD::numberOfSCTHits);

      if ((nPi + nSCT) < 7) continue;

      auto res2 = m_mcTruthClassifier->particleTruthClassifier(truth);
      MCTruthPartClassifier::ParticleOrigin TO = res2.second;
      MCTruthPartClassifier::ParticleType TT = res2.first;
      auto res = m_mcTruthClassifier->checkOrigOfBkgElec(truth);
      MCTruthPartClassifier::ParticleOrigin bkgTO = res.second;
      MCTruthPartClassifier::ParticleType bkgTT = res.first;
      ATH_MSG_DEBUG(" ** Truth particle associated to track Found: "
                << " STATUS  " << truth->status()
                << " type  " << truth->type()
                << " barcode  " << truth->barcode()
                << " PDG id   " << truth->pdgId()
                << " index    " << truth->index()
                << " bkg T0  " << bkgTO
                << " bkg TT   " << bkgTT
                << " T0  " << TO
                << " TT   " << TT
                << " eventNumber  " << eventInfo->eventNumber() );

      if (TO == MCTruthPartClassifier::SingleElec &&
          TT == MCTruthPartClassifier::IsoElectron &&
          truth->index() == promtpElectronTruthIndex && !foundPromptElectron ) {

        // we found the track from the prompt electron
        // let's count it
        truthPromptElectronWithGSFTrack->fill(truth);
        foundPromptElectron = true;


      } else {
        const xAOD::TruthParticle *firstElTruth = xAOD::EgammaHelpers::getBkgElectronMother(truth);
        if (!firstElTruth) continue;
        ATH_MSG_DEBUG( "********----- Getting Mother-----********" );
        ATH_MSG_DEBUG( " STATUS  " << firstElTruth->status()
                << " type  " << firstElTruth->type()
                << " barcode  " << firstElTruth->barcode()
                << " PDG id   " << firstElTruth->pdgId()
                << " index    " << firstElTruth->index() );
        // this is not a prompt electron, we need to check the parents
        // but we need to make sure that we double count if already found the track
        // foundPromptElectron will check that
        if ((firstElTruth->index() == promtpElectronTruthIndex) &&
            (!foundPromptElectron)) {
          truthPromptElectronWithGSFTrack->fill(firstElTruth);
          foundPromptElectron = true;
        }

      }


    }


    foundPromptElectron = false;

    for (auto elrec : *RecoEl) {

      if (!elrec) continue;
      clusterAll->fill(*elrec,mu);
      if (elrec->pt() > 10*Gaudi::Units::GeV) {
        cluster10GeV->fill(*elrec,mu);
      }
      recoElectronAll->fill(*elrec);
      showerShapesAll->fill(*elrec);
      isolationAll->fill(*elrec);
      if (elrec->pt() > 10*Gaudi::Units::GeV) {
        showerShapes10GeV->fill(*elrec);
      }

      const xAOD::TruthParticle *truth = xAOD::TruthHelpers::getTruthParticle(*elrec);
      if (!truth ) continue;
      truthElectronRecoElectronAll->fill(truth, elrec);
      if (fabs(truth->pdgId()) != 11) continue;
      auto res2 = m_mcTruthClassifier->particleTruthClassifier(truth);
      MCTruthPartClassifier::ParticleOrigin TO = res2.second;
      MCTruthPartClassifier::ParticleType TT = res2.first;

      if (TO == MCTruthPartClassifier::SingleElec &&
          TT == MCTruthPartClassifier::IsoElectron &&
          truth->index() == promtpElectronTruthIndex && !foundPromptElectron ) {

        // we found the track from the prompt electron
        // let's count it
        truthPromptElectronWithReco->fill(truth, elrec);
        foundPromptElectron = true;
        if (m_LooseLH->accept(elrec)) truthRecoElectronLooseLH->fill(truth,elrec);
        if (m_MediumLH->accept(elrec)) truthRecoElectronMediumLH->fill(truth,elrec);
        if (m_TightLH->accept(elrec)) truthRecoElectronTightLH->fill(truth,elrec);
        if (m_IsoFixedCutTight->accept(*elrec)) recoElectronIsoFixedCutTight->fill(truth,elrec);
        if (m_IsoFixedCutTightTrackOnly->accept(*elrec)) recoElectronIsoFixedCutTightTrackOnly->fill(truth,elrec);
        if (m_IsoFixedCutLoose->accept(*elrec)) recoElectronIsoFixedCutLoose->fill(truth,elrec);

      } else {
        const xAOD::TruthParticle *firstElTruth = xAOD::EgammaHelpers::getBkgElectronMother(truth);
        if (!firstElTruth) continue;
        // this is not a prompt electron, we need to check the parents
        // but we need to make sure that we double count if already found the track
        // foundPromptElectron will check that
        if ((firstElTruth->index() == promtpElectronTruthIndex)) {
          if  (!foundPromptElectron) {
            foundPromptElectron = true;
            truthPromptElectronWithReco->fill(firstElTruth,elrec);
            if (m_LooseLH->accept(elrec)) truthRecoElectronLooseLH->fill(firstElTruth,elrec);
            if (m_MediumLH->accept(elrec)) truthRecoElectronMediumLH->fill(firstElTruth,elrec);
            if (m_TightLH->accept(elrec)) truthRecoElectronTightLH->fill(firstElTruth,elrec);
            if (m_IsoFixedCutTight->accept(*elrec)) recoElectronIsoFixedCutTight->fill(firstElTruth,elrec);
            if (m_IsoFixedCutTightTrackOnly->accept(*elrec)) recoElectronIsoFixedCutTightTrackOnly->fill(firstElTruth,elrec);
            if (m_IsoFixedCutLoose->accept(*elrec)) recoElectronIsoFixedCutLoose->fill(firstElTruth,elrec);
          }

        }

      }

    } // RecoEl Loop


  } // if electron


  if ("gamma" == m_sampleType) {

    const xAOD::PhotonContainer *RecoPh = nullptr;
    if (!evtStore()->retrieve(RecoPh, "Photons").isSuccess()) {
      ATH_MSG_ERROR("Failed to retrieve photon container. Exiting.");
      return StatusCode::FAILURE;
    }

    for (auto phrec : *RecoPh) {

      if (!phrec) continue;

      recoPhotonAll->fill(*phrec);
      isolationAll->fill(*phrec);
      showerShapesAll->fill(*phrec);
      clusterAll->fill(*phrec,mu);
      if (phrec->pt() > 10*Gaudi::Units::GeV) {
        cluster10GeV->fill(*phrec,mu);
      }
      if (phrec->pt() > 10*Gaudi::Units::GeV){ 
        showerShapes10GeV->fill(*phrec);
      }

    } // RecoPh Loop

    for (auto egtruth : *egTruthParticles) {

      if (!egtruth) continue;

      bool isTrueConv = xAOD::EgammaHelpers::isTrueConvertedPhoton(egtruth);
      bool isTrueLateConv = xAOD::EgammaHelpers::isTrueConvertedPhoton(egtruth, 1200) and !isTrueConv;
      const xAOD::Photon *photon = xAOD::EgammaHelpers::getRecoPhoton(egtruth);

      if (!photon) continue;

      truthPhotonRecoPhoton->fill(*egtruth);
      clusterPromptAll->fill(*photon,mu);
      if (egtruth->pt() > 10*Gaudi::Units::GeV) {
        clusterPrompt10GeV->fill(*photon,mu);
      }

      bool isRecoConv = xAOD::EgammaHelpers::isConvertedPhoton(photon);
      xAOD::EgammaParameters::ConversionType convType = xAOD::EgammaHelpers::conversionType(photon);

      if (isTrueConv) {

        truthPhotonConvPhoton->fill(*egtruth);

        if (isRecoConv) {

          truthPhotonConvRecoConv->fill(*egtruth);

          clusterConvPhoton->fill(*photon,mu);

          if (convType == xAOD::EgammaParameters::singleSi) {
            truthPhotonConvRecoConv1Si->fill(*egtruth);
            clusterConvPhotonSi->fill(*photon,mu);
          } else if (convType == xAOD::EgammaParameters::singleTRT) {
            truthPhotonConvRecoConv1TRT->fill(*egtruth);
            clusterConvPhotonTRT->fill(*photon,mu);
          } else if (convType == xAOD::EgammaParameters::doubleSi) {
            truthPhotonConvRecoConv2Si->fill(*egtruth);
            clusterConvPhotonSiSi->fill(*photon,mu);
          } else if (convType == xAOD::EgammaParameters::doubleTRT) {
            truthPhotonConvRecoConv2TRT->fill(*egtruth);
            clusterConvPhotonTRTTRT->fill(*photon,mu);
          } else if (convType == xAOD::EgammaParameters::doubleSiTRT) {
            truthPhotonConvRecoConv2SiTRT->fill(*egtruth);
            clusterConvPhotonSiTRT->fill(*photon,mu); 
          }

          if (m_IsoFixedCutTight->accept(*photon)) recoPhotonConvIsoFixedCutTight->fill(*egtruth);
          if (m_IsoFixedCutTightCaloOnly->accept(*photon)) recoPhotonConvIsoFixedCutTightCaloOnly->fill(*egtruth);
          if (m_IsoFixedCutLoose->accept(*photon)) recoPhotonConvIsoFixedCutLoose->fill(*egtruth);
        } // isRecoConv
        else {
          truthPhotonConvRecoUnconv->fill(*egtruth);
          clusterUnconvPhoton->fill(*photon,mu); 
        } 

      } //isTrueConv
      else if (!isTrueLateConv) {

        truthPhotonUnconvPhoton->fill(*egtruth);

        if (isRecoConv) truthPhotonUnconvRecoConv->fill(*egtruth);
        else           truthPhotonUnconvRecoUnconv->fill(*egtruth);

        if (m_IsoFixedCutTight->accept(*photon)) recoPhotonUnconvIsoFixedCutTight->fill(*egtruth);
        if (m_IsoFixedCutTightCaloOnly->accept(*photon)) recoPhotonUnconvIsoFixedCutTightCaloOnly->fill(*egtruth);
        if (m_IsoFixedCutLoose->accept(*photon)) recoPhotonUnconvIsoFixedCutLoose->fill(*egtruth);
      } // !isTrueLateConv
    } //egtruth Loop
  } // if gamma

  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring::finalize() {

  if ("electron" == m_sampleType) {

    egammaMonitoring::EfficiencyPlot trackEfficiency("trackingEfficiency", "/MONITORING/trackingEfficiency/", rootHistSvc );
    ATH_CHECK(trackEfficiency.divide(truthPromptElectronWithTrack.get(), truthPromptElectronAll.get()));
    egammaMonitoring::EfficiencyPlot GSFEfficiency("GSFEfficiency", "/MONITORING/GSFEfficiency/", rootHistSvc );
    ATH_CHECK(GSFEfficiency.divide(truthPromptElectronWithGSFTrack.get(),truthPromptElectronWithTrack.get()));
    egammaMonitoring::EfficiencyPlot matchingEfficiency("matchingEfficiency", "/MONITORING/matchingEfficiency/", rootHistSvc );
    ATH_CHECK(matchingEfficiency.divide(truthPromptElectronWithReco.get(), truthPromptElectronWithGSFTrack.get()));
    egammaMonitoring::EfficiencyPlot reconstructionEfficiency("reconstructionEfficiency", "/MONITORING/reconstructionEfficiency/", rootHistSvc );
    ATH_CHECK(reconstructionEfficiency.divide(truthPromptElectronWithReco.get(), truthPromptElectronAll.get()));
    egammaMonitoring::EfficiencyPlot recoElectronLooseLHEfficiency("recoElectronLooseLHEfficiency", "/MONITORING/recoElectronLooseLHEfficiency/", rootHistSvc );
    ATH_CHECK(recoElectronLooseLHEfficiency.divide(truthRecoElectronLooseLH.get(), truthPromptElectronAll.get()));
    egammaMonitoring::EfficiencyPlot recoElectronMediumLHEfficiency("recoElectronMediumLHEfficiency", "/MONITORING/recoElectronMediumLHEfficiency/", rootHistSvc );
    ATH_CHECK(recoElectronMediumLHEfficiency.divide(truthRecoElectronMediumLH.get(), truthPromptElectronAll.get()));
    egammaMonitoring::EfficiencyPlot recoElectronTightLHEfficiency("recoElectronTightLHEfficiency", "/MONITORING/recoElectronTightLHEfficiency/", rootHistSvc );
    ATH_CHECK(recoElectronTightLHEfficiency.divide( truthRecoElectronTightLH.get(), truthPromptElectronAll.get()));
    egammaMonitoring::EfficiencyPlot recoElectronIsoFixedCutTightEfficiency("recoElectronIsoFixedCutTightEfficiency", "/MONITORING/recoElectronIsoFixedCutTightEfficiency/", rootHistSvc );
    ATH_CHECK(recoElectronIsoFixedCutTightEfficiency.divide( recoElectronIsoFixedCutTight.get(), truthPromptElectronWithReco.get()));
    egammaMonitoring::EfficiencyPlot recoElectronIsoFixedCutTightTrackOnlyEfficiency("recoElectronIsoFixedCutTightTrackOnlyEfficiency", "/MONITORING/recoElectronIsoFixedCutTightTrackOnlyEfficiency/", rootHistSvc );
    ATH_CHECK(recoElectronIsoFixedCutTightTrackOnlyEfficiency.divide( recoElectronIsoFixedCutTightTrackOnly.get(), truthPromptElectronWithReco.get()));
    egammaMonitoring::EfficiencyPlot recoElectronIsoFixedCutLooseEfficiency("recoElectronIsoFixedCutLooseEfficiency", "/MONITORING/recoElectronIsoFixedCutLooseEfficiency/", rootHistSvc );
    ATH_CHECK(recoElectronIsoFixedCutLooseEfficiency.divide( recoElectronIsoFixedCutLoose.get(), truthPromptElectronWithReco.get()));
  }

  if ("gamma" == m_sampleType) {

    egammaMonitoring::EfficiencyPlot truthPhotonConvRecoConvEfficiency("truthPhotonConvRecoConvEfficiency", "/MONITORING/truthPhotonConvRecoConvEfficiency/", rootHistSvc );
    ATH_CHECK(truthPhotonConvRecoConvEfficiency.divide(truthPhotonConvRecoConv.get(),truthPhotonConvPhoton.get()));
    egammaMonitoring::EfficiencyPlot truthPhotonConvRecoConv1SiEfficiency("truthPhotonConvRecoConv1SiEfficiency", "/MONITORING/truthPhotonConvRecoConv1SiEfficiency/", rootHistSvc );
    ATH_CHECK(truthPhotonConvRecoConv1SiEfficiency.divide( truthPhotonConvRecoConv1Si.get()   , truthPhotonConvPhoton.get()));
    egammaMonitoring::EfficiencyPlot truthPhotonConvRecoConv1TRTEfficiency("truthPhotonConvRecoConv1TRTEfficiency", "/MONITORING/truthPhotonConvRecoConv1TRTEfficiency/", rootHistSvc );
    ATH_CHECK(truthPhotonConvRecoConv1TRTEfficiency.divide(truthPhotonConvRecoConv1TRT.get()  , truthPhotonConvPhoton.get()));
    egammaMonitoring::EfficiencyPlot truthPhotonConvRecoConv2SiEfficiency("truthPhotonConvRecoConv2SiEfficiency", "/MONITORING/truthPhotonConvRecoConv2SiEfficiency/", rootHistSvc );
    ATH_CHECK(truthPhotonConvRecoConv2SiEfficiency.divide(truthPhotonConvRecoConv2Si.get()   , truthPhotonConvPhoton.get()));
    egammaMonitoring::EfficiencyPlot truthPhotonConvRecoConv2TRTEfficiency("truthPhotonConvRecoConv2TRTEfficiency", "/MONITORING/truthPhotonConvRecoConv2TRTEfficiency/", rootHistSvc );
    ATH_CHECK(truthPhotonConvRecoConv2TRTEfficiency.divide(truthPhotonConvRecoConv2TRT.get()  , truthPhotonConvPhoton.get()));
    egammaMonitoring::EfficiencyPlot truthPhotonConvRecoConv2SiTRTEfficiency("truthPhotonConvRecoConv2SiTRTEfficiency", "/MONITORING/truthPhotonConvRecoConv2SiTRTEfficiency/", rootHistSvc );
    ATH_CHECK(truthPhotonConvRecoConv2SiTRTEfficiency.divide(truthPhotonConvRecoConv2SiTRT.get(), truthPhotonConvPhoton.get()));
    egammaMonitoring::EfficiencyPlot truthPhotonConvRecoUnconvEfficiency("truthPhotonConvRecoUnconvEfficiency", "/MONITORING/truthPhotonConvRecoUnconvEfficiency/", rootHistSvc );
    ATH_CHECK(truthPhotonConvRecoUnconvEfficiency.divide(truthPhotonConvRecoUnconv.get(), truthPhotonConvPhoton.get()));
    egammaMonitoring::EfficiencyPlot truthPhotonUnconvRecoUnconvEfficiency("truthPhotonUnconvRecoUnconvEfficiency", "/MONITORING/truthPhotonUnconvRecoUnconvEfficiency/", rootHistSvc );
    ATH_CHECK(truthPhotonUnconvRecoUnconvEfficiency.divide(truthPhotonUnconvRecoUnconv.get(), truthPhotonUnconvPhoton.get()));
    egammaMonitoring::EfficiencyPlot truthPhotonRecoConvEfficiency("truthPhotonRecoConvEfficiency", "/MONITORING/truthPhotonRecoConvEfficiency/", rootHistSvc );
    ATH_CHECK(truthPhotonRecoConvEfficiency.divide(truthPhotonConvRecoConv.get(), truthPhotonRecoPhoton.get()));
    egammaMonitoring::EfficiencyPlot truthPhotonRecoUnconvEfficiency("truthPhotonRecoUnconvEfficiency", "/MONITORING/truthPhotonRecoUnconvEfficiency/", rootHistSvc );
    ATH_CHECK(truthPhotonRecoUnconvEfficiency.divide(truthPhotonUnconvRecoUnconv.get(), truthPhotonRecoPhoton.get()));
    egammaMonitoring::EfficiencyPlot recoPhotonUnconvIsoFixedCutTightEfficiency("recoPhotonUnconvIsoFixedCutTightEfficiency", "/MONITORING/recoPhotonUnconvIsoFixedCutTightEfficiency/", rootHistSvc );
    ATH_CHECK(recoPhotonUnconvIsoFixedCutTightEfficiency.divide( recoPhotonUnconvIsoFixedCutTight.get(), truthPhotonUnconvRecoUnconv.get()));
    egammaMonitoring::EfficiencyPlot recoPhotonUnconvIsoFixedCutTightCaloOnlyEfficiency("recoPhotonUnconvIsoFixedCutTightCaloOnlyEfficiency", "/MONITORING/recoPhotonUnconvIsoFixedCutTightCaloOnlyEfficiency/", rootHistSvc );
    ATH_CHECK(recoPhotonUnconvIsoFixedCutTightCaloOnlyEfficiency.divide( recoPhotonUnconvIsoFixedCutTightCaloOnly.get(), truthPhotonUnconvRecoUnconv.get()));
    egammaMonitoring::EfficiencyPlot recoPhotonUnconvIsoFixedCutLooseEfficiency("recoPhotonUnconvIsoFixedCutLooseEfficiency", "/MONITORING/recoPhotonUnconvIsoFixedCutLooseEfficiency/", rootHistSvc );
    ATH_CHECK(recoPhotonUnconvIsoFixedCutLooseEfficiency.divide( recoPhotonUnconvIsoFixedCutLoose.get(), truthPhotonUnconvRecoUnconv.get()));
    egammaMonitoring::EfficiencyPlot recoPhotonConvIsoFixedCutTightEfficiency("recoPhotonConvIsoFixedCutTightEfficiency", "/MONITORING/recoPhotonConvIsoFixedCutTightEfficiency/", rootHistSvc );
    ATH_CHECK(recoPhotonConvIsoFixedCutTightEfficiency.divide( recoPhotonConvIsoFixedCutTight.get(), truthPhotonConvRecoConv.get()));
    egammaMonitoring::EfficiencyPlot recoPhotonConvIsoFixedCutTightCaloOnlyEfficiency("recoPhotonConvIsoFixedCutTightCaloOnlyEfficiency", "/MONITORING/recoPhotonConvIsoFixedCutTightCaloOnlyEfficiency/", rootHistSvc );
    ATH_CHECK(recoPhotonConvIsoFixedCutTightCaloOnlyEfficiency.divide( recoPhotonConvIsoFixedCutTightCaloOnly.get(), truthPhotonConvRecoConv.get()));
    egammaMonitoring::EfficiencyPlot recoPhotonConvIsoFixedCutLooseEfficiency("recoPhotonConvIsoFixedCutLooseEfficiency", "/MONITORING/recoPhotonConvIsoFixedCutLooseEfficiency/", rootHistSvc );
    ATH_CHECK(recoPhotonConvIsoFixedCutLooseEfficiency.divide( recoPhotonConvIsoFixedCutLoose.get(), truthPhotonConvRecoConv.get()));
  }

  return StatusCode::SUCCESS;
}
