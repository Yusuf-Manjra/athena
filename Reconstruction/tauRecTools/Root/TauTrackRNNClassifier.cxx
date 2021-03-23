/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ASG include(s)
#include "PathResolver/PathResolver.h"

// xAOD include(s)
#include "xAODTracking/TrackParticle.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauxAODHelpers.h"

// local include(s)
#include "tauRecTools/TauTrackRNNClassifier.h"
#include "tauRecTools/HelperFunctions.h"

#include <fstream>

using namespace tauRecTools;

//==============================================================================
// class TauTrackRNNClassifier
//==============================================================================

//______________________________________________________________________________
TauTrackRNNClassifier::TauTrackRNNClassifier(const std::string& name)
  : TauRecToolBase(name) {
  declareProperty("classifyLRT", m_classifyLRT = true);
}

//______________________________________________________________________________
TauTrackRNNClassifier::~TauTrackRNNClassifier()
{
}

//______________________________________________________________________________
StatusCode TauTrackRNNClassifier::initialize()
{
  for (const auto& classifier : m_vClassifier){
    ATH_MSG_INFO("Intialize TauTrackRNNClassifier tool : " << classifier );
    ATH_CHECK(classifier.retrieve());
  }
 
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTrackRNNClassifier::executeTrackClassifier(xAOD::TauJet& xTau, xAOD::TauTrackContainer& tauTrackCon) const {

  std::vector<xAOD::TauTrack*> vTracks = xAOD::TauHelpers::allTauTracksNonConst(&xTau, &tauTrackCon);

  for (xAOD::TauTrack* xTrack : vTracks) {
    // reset all track flags and set status to unclassified
    xTrack->setFlag(xAOD::TauJetParameters::classifiedCharged, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedConversion, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedIsolation, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedFake, false);
    xTrack->setFlag(xAOD::TauJetParameters::unclassified, true);
  }

  // don't classify LRTs even if LRTs were associated with taus in TauTrackFinder
  if(!m_classifyLRT) {
    std::vector<xAOD::TauTrack*> vLRTs;
    std::vector<xAOD::TauTrack*>::iterator it = vTracks.begin(); 
    while(it != vTracks.end()) {      
      if((*it)->flag(xAOD::TauJetParameters::LargeRadiusTrack)) {	
	vLRTs.push_back(*it);
        it = vTracks.erase(it);
      }
      else {
	++it;
      }
    }

    // decorate LRTs with default RNN scores
    for (auto classifier : m_vClassifier) {
      ATH_CHECK(classifier->classifyTracks(vLRTs, xTau, true));
    }
  }

  // classify tracks
  for (auto classifier : m_vClassifier) {
    ATH_CHECK(classifier->classifyTracks(vTracks, xTau));
  }

  std::vector< ElementLink< xAOD::TauTrackContainer > >& tauTrackLinks(xTau.allTauTrackLinksNonConst());
  std::sort(tauTrackLinks.begin(), tauTrackLinks.end(), sortTracks);
  float charge=0.0;
  for( const xAOD::TauTrack* trk : xTau.tracks(xAOD::TauJetParameters::classifiedCharged) ){
    charge += trk->track()->charge();
  }
  xTau.setCharge(charge);
  xTau.setDetail(xAOD::TauJetParameters::nChargedTracks, (int) xTau.nTracks());
  xTau.setDetail(xAOD::TauJetParameters::nIsolatedTracks, (int) xTau.nTracks(xAOD::TauJetParameters::classifiedIsolation));

  // decorations for now, may be turned into Aux
  static const SG::AuxElement::Accessor<int> nTrkConv("nConversionTracks");
  static const SG::AuxElement::Accessor<int> nTrkFake("nFakeTracks");
  nTrkConv(xTau) = (int) xTau.nTracks(xAOD::TauJetParameters::classifiedConversion);
  nTrkFake(xTau) = (int) xTau.nTracks(xAOD::TauJetParameters::classifiedFake);

  //set modifiedIsolationTrack
  for (xAOD::TauTrack* xTrack : vTracks) {
    if( not xTrack->flag(xAOD::TauJetParameters::classifiedCharged) and xTrack->flag(xAOD::TauJetParameters::passTrkSelector) ) {
      xTrack->setFlag(xAOD::TauJetParameters::modifiedIsolationTrack, true);
    }
    else {
      xTrack->setFlag(xAOD::TauJetParameters::modifiedIsolationTrack, false);
    }
  }
  xTau.setDetail(xAOD::TauJetParameters::nModifiedIsolationTracks, (int) xTau.nTracks(xAOD::TauJetParameters::modifiedIsolationTrack));

  return StatusCode::SUCCESS;
}

//==============================================================================
// class TrackRNN
//==============================================================================

//______________________________________________________________________________
TrackRNN::TrackRNN(const std::string& name)
  : TauRecToolBase(name)
  , m_inputWeightsPath("")
{
  // for conversion compatibility cast nTracks 
  int nMaxNtracks = 0;
  declareProperty( "InputWeightsPath", m_inputWeightsPath );
  declareProperty( "MaxNtracks",  nMaxNtracks);
  m_nMaxNtracks = (unsigned int)nMaxNtracks;
}

//______________________________________________________________________________
TrackRNN::~TrackRNN()
{
}

//______________________________________________________________________________
StatusCode TrackRNN::initialize()
{  
  ATH_CHECK(addWeightsFile());
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackRNN::classifyTracks(std::vector<xAOD::TauTrack*>& vTracks, xAOD::TauJet& xTau, bool skipTracks) const
{
  if(vTracks.empty()) {
    return StatusCode::SUCCESS;
  }

  static const SG::AuxElement::Accessor<float> idScoreCharged("rnn_chargedScore");
  static const SG::AuxElement::Accessor<float> idScoreIso("rnn_isolationScore");
  static const SG::AuxElement::Accessor<float> idScoreConv("rnn_conversionScore");
  static const SG::AuxElement::Accessor<float> idScoreFake("rnn_fakeScore");

  // don't classify tracks, set default decorations
  if(skipTracks) {
    for(xAOD::TauTrack* track : vTracks) {
      idScoreCharged(*track) = 0.;
      idScoreConv(*track) = 0.;
      idScoreIso(*track) = 0.;
      idScoreFake(*track) = 0.;
    }
    return StatusCode::SUCCESS;
  }

  std::sort(vTracks.begin(), vTracks.end(), [](const xAOD::TauTrack * a, const xAOD::TauTrack * b) {return a->pt() > b->pt();});

  VectorMap valueMap;
  ATH_CHECK(calulateVars(vTracks, xTau, valueMap));

  SeqNodeMap seqInput;
  NodeMap nodeInput;
  
  seqInput["input_1"] = valueMap; 

  VectorMap mValue = m_RNNClassifier->scan(nodeInput, seqInput, "time_distributed_2");

  std::vector<double> vClassProb(5);

  for (unsigned int i = 0; i < vTracks.size(); ++i){

    if(i >= m_nMaxNtracks && m_nMaxNtracks > 0){
      vClassProb[0] = 0.0;
      vClassProb[1] = 0.0;
      vClassProb[2] = 0.0;
      vClassProb[3] = 0.0;
      vClassProb[4] = 1.0;
    }else{
      vClassProb[0] = mValue["type_0"][i];
      vClassProb[1] = mValue["type_1"][i];
      vClassProb[2] = mValue["type_2"][i];
      vClassProb[3] = mValue["type_3"][i];
      vClassProb[4] = 0.0;
    }

    idScoreCharged(*vTracks[i]) = vClassProb[0];
    idScoreConv(*vTracks[i]) = vClassProb[1];
    idScoreIso(*vTracks[i]) = vClassProb[2];
    idScoreFake(*vTracks[i]) = vClassProb[3];

    int iMaxIndex = 0;
    for (unsigned int j = 1; j < vClassProb.size(); ++j){
      if(vClassProb[j] > vClassProb[iMaxIndex]) iMaxIndex = j;
    }

    if(iMaxIndex < 4) {
      vTracks[i]->setFlag(xAOD::TauJetParameters::unclassified, false);
    }

    if(iMaxIndex == 3){
      vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedFake, true);
    }else if(iMaxIndex == 0){
      vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedCharged, true);
    }else if(iMaxIndex == 1){
      vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedConversion, true);
    }else if(iMaxIndex == 2){
      vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedIsolation, true);
    }
  }
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
//StatusCode TrackRNN::classifyTriggerTrack(xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau, const xAOD::TauTrack* lead_track, double mu)
//{
//  // NOT YET IMPLEMENTED 
//  return StatusCode::SUCCESS;
//}

//______________________________________________________________________________
StatusCode TrackRNN::addWeightsFile()
{
  std::string inputWeightsPath = find_file(m_inputWeightsPath);
  ATH_MSG_DEBUG("InputWeightsPath: " << inputWeightsPath);

  std::ifstream nn_config_istream(inputWeightsPath);
  
  lwtDev::GraphConfig NNconfig = lwtDev::parse_json_graph(nn_config_istream);
  
  m_RNNClassifier = std::make_unique<lwtDev::LightweightGraph>(NNconfig, NNconfig.outputs.begin()->first);

  if(!m_RNNClassifier) {
    ATH_MSG_FATAL("Couldn't configure neural network!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackRNN::calulateVars(const std::vector<xAOD::TauTrack*>& vTracks, const xAOD::TauJet& xTau, tauRecTools::VectorMap& valueMap) const
{
  // initialize map with values
  valueMap.clear();
  unsigned int n_timeSteps = vTracks.size();
  if(m_nMaxNtracks > 0 && n_timeSteps > m_nMaxNtracks) {
    n_timeSteps = m_nMaxNtracks;
  }

  valueMap["log(trackPt)"] = std::vector<double>(n_timeSteps);
  valueMap["log(jetSeedPt)"] = std::vector<double>(n_timeSteps);
  valueMap["(trackPt/jetSeedPt[0])"] = std::vector<double>(n_timeSteps);
  valueMap["trackEta"] = std::vector<double>(n_timeSteps);
  valueMap["z0sinThetaTJVA"] = std::vector<double>(n_timeSteps);
  valueMap["log(rConv)"] = std::vector<double>(n_timeSteps);
  valueMap["tanh(rConvII/500)"] = std::vector<double>(n_timeSteps);
  valueMap["dRJetSeedAxis"] = std::vector<double>(n_timeSteps);
  valueMap["tanh(d0/10)"] = std::vector<double>(n_timeSteps);
  valueMap["qOverP*1000"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfInnermostPixelLayerHits"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfPixelSharedHits"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfSCTSharedHits"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfTRTHits"] = std::vector<double>(n_timeSteps);
  valueMap["eProbabilityHT"] = std::vector<double>(n_timeSteps);
  valueMap["nPixHits"] = std::vector<double>(n_timeSteps);
  valueMap["nSiHits"] = std::vector<double>(n_timeSteps);
  valueMap["charge"] = std::vector<double>(n_timeSteps);

  double fTauSeedPt = xTau.ptJetSeed();
  double log_TauSeedPt = std::log(fTauSeedPt);

  unsigned int i = 0;
  for(xAOD::TauTrack* xTrack : vTracks)
    {
      const xAOD::TrackParticle* xTrackParticle = xTrack->track();

      double fTrackPt = xTrackParticle->pt();
      double fTrackEta = xTrackParticle->eta();
      double fTrackCharge = xTrackParticle->charge();
      double fZ0SinthetaTJVA = xTrack->z0sinthetaTJVA();
      double fRConv = xTrack->rConv();
      double fRConvII = xTrack->rConvII();
      double fDRJetSeedAxis = xTrack->dRJetSeedAxis(xTau);
      double fD0 = xTrack->d0TJVA();
      double fQoverP = xTrackParticle->qOverP();

      uint8_t iTracksNumberOfInnermostPixelLayerHits = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNumberOfInnermostPixelLayerHits, xAOD::numberOfInnermostPixelLayerHits) );
      uint8_t iTracksNPixelHits = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNPixelHits, xAOD::numberOfPixelHits) );
      uint8_t iTracksNPixelSharedHits = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNPixelSharedHits, xAOD::numberOfPixelSharedHits) );
      uint8_t iTracksNPixelDeadSensors = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNPixelDeadSensors, xAOD::numberOfPixelDeadSensors) );
      uint8_t iTracksNSCTHits = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNSCTHits, xAOD::numberOfSCTHits) );
      uint8_t iTracksNSCTSharedHits = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNSCTSharedHits, xAOD::numberOfSCTSharedHits) );
      uint8_t iTracksNSCTDeadSensors = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNSCTDeadSensors, xAOD::numberOfSCTDeadSensors) );
      uint8_t iTracksNTRTHighThresholdHits = 0; ATH_CHECK( xTrackParticle->summaryValue( iTracksNTRTHighThresholdHits, xAOD::numberOfTRTHighThresholdHits) );
      uint8_t iTracksNTRTHits = 0; ATH_CHECK( xTrackParticle->summaryValue( iTracksNTRTHits, xAOD::numberOfTRTHits) );

      float fTracksEProbabilityHT; ATH_CHECK( xTrackParticle->summaryValue( fTracksEProbabilityHT, xAOD::eProbabilityHT) );
  
      valueMap["log(trackPt)"][i] = std::log(fTrackPt);
      valueMap["log(jetSeedPt)"][i] = log_TauSeedPt;
      valueMap["(trackPt/jetSeedPt[0])"][i] = (fTrackPt/fTauSeedPt);
      valueMap["trackEta"][i] = fTrackEta;
      valueMap["z0sinThetaTJVA"][i] = fZ0SinthetaTJVA;
      valueMap["log(rConv)"][i] = std::log(fRConv);
      valueMap["tanh(rConvII/500)"][i] = std::tanh(fRConvII/500.0);
      valueMap["dRJetSeedAxis"][i] = fDRJetSeedAxis;
      valueMap["tanh(d0/10)"][i] = std::tanh(fD0/10.);
      valueMap["qOverP*1000"][i] = fQoverP*1000.0;
      valueMap["numberOfInnermostPixelLayerHits"][i] = (float) iTracksNumberOfInnermostPixelLayerHits;
      valueMap["numberOfPixelSharedHits"][i] = (float) iTracksNPixelSharedHits;
      valueMap["numberOfSCTSharedHits"][i] = (float) iTracksNSCTSharedHits;
      valueMap["numberOfTRTHits"][i] = (float) iTracksNTRTHits;
      valueMap["eProbabilityHT"][i] = fTracksEProbabilityHT;
      valueMap["nPixHits"][i] = (float) (iTracksNPixelHits + iTracksNPixelDeadSensors);
      valueMap["nSiHits"][i] = (float) (iTracksNPixelHits + iTracksNPixelDeadSensors + iTracksNSCTHits + iTracksNSCTDeadSensors);
      valueMap["charge"][i] = fTrackCharge;

      ++i;
      if(m_nMaxNtracks > 0 && i >= m_nMaxNtracks) {
	break;
      }
    }

  return StatusCode::SUCCESS;
} 

//______________________________________________________________________________
//StatusCode TrackRNN::setTriggerVars(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau, const xAOD::TauTrack* lead_track)
//{
//  // NOT YET IMPLEMENTED
//
//  return StatusCode::SUCCESS;
//}
