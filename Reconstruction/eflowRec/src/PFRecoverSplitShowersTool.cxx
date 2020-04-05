/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFRecoverSplitShowersTool.h"
#include "eflowRec/eflowUtil.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowCaloObjectMaker.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowCellList.h"
#include "eflowRec/IEFlowCellEOverPTool.h"
#include "eflowRec/PFTrackClusterMatchingTool.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"
#include "eflowRec/eflowSubtractor.h"
#include "eflowRec/PFSubtractionStatusSetter.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

using namespace eflowSubtract;

PFRecoverSplitShowersTool::PFRecoverSplitShowersTool(const std::string& type,const std::string& name,const IInterface* parent):
  base_class(type, name, parent),
  m_eflowCaloObjectContainer(0),
  m_binnedParameters(std::make_unique<eflowEEtaBinnedParameters>()),
  m_nTrackClusterMatches(0)
{
}

PFRecoverSplitShowersTool::~PFRecoverSplitShowersTool() {}

StatusCode PFRecoverSplitShowersTool::initialize(){

  if (m_matchingTool.retrieve().isFailure()){
    ATH_MSG_WARNING("Couldn't retrieve PFTrackClusterMatchingTool");
    return StatusCode::SUCCESS;
  }

  if (m_theEOverPTool.retrieve().isFailure()){
    ATH_MSG_WARNING("Cannot find eflowEOverPTool");
    return StatusCode::SUCCESS;
  }

  if (m_theEOverPTool->execute(m_binnedParameters.get()).isFailure()){
    ATH_MSG_WARNING("Could not execute eflowCellEOverPTool");
    return StatusCode::SUCCESS;
  }

  const double gaussianRadius = 0.032;
  const double gaussianRadiusError = 1.0e-3;
  const double maximumRadiusSigma = 3.0;
  
  m_integrator = std::make_unique<eflowLayerIntegrator>(gaussianRadius, gaussianRadiusError, maximumRadiusSigma, m_isHLLHC);

  return StatusCode::SUCCESS;
}

void PFRecoverSplitShowersTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, eflowRecTrackContainer*, eflowRecClusterContainer*){

  ATH_MSG_DEBUG("Executing");

  m_eflowCaloObjectContainer = theEflowCaloObjectContainer;

  recycleTracksClusters();

  /* Add each track to its matching cluster's eflowCaloObject */
  int const nOriginalObj = matchAndCreateEflowCaloObj();

  /* Extrapolate tracks, match clusters, do shower simulation, run cell subtraction */
  performRecovery(nOriginalObj);

}

StatusCode PFRecoverSplitShowersTool::finalize(){

  ATH_MSG_INFO("Produced " << m_nTrackClusterMatches << " track-cluster matches.");

  return StatusCode::SUCCESS;

}
void PFRecoverSplitShowersTool::recycleTracksClusters() {
  getTracksToRecover();
  getClustersToConsider();

}

void PFRecoverSplitShowersTool::getClustersToConsider() {

  m_clustersToConsider.clear();

  for (auto thisEflowCaloObject : *m_eflowCaloObjectContainer){

    if (thisEflowCaloObject->nClusters() == 0) { continue; }

    for(unsigned i=0; i<thisEflowCaloObject->nClusters(); ++i){
        /* Skip empty clusters (subtraction remnants) */
        const CaloClusterCellLink* theCellLink = thisEflowCaloObject->efRecCluster(i)->getCluster()->getCellLinks();
        if (0 == (int)theCellLink->size()){ continue; }

        thisEflowCaloObject->efRecCluster(i)->clearTrackMatches();
        m_clustersToConsider.push_back(thisEflowCaloObject->efRecCluster(i));
        thisEflowCaloObject->clearClusters();
    }
  }

  std::sort(m_clustersToConsider.begin(),m_clustersToConsider.end(),eflowRecCluster::SortDescendingPt());
}

void PFRecoverSplitShowersTool::getTracksToRecover() {

  m_tracksToRecover.clear(); 
  for (auto thisEflowCaloObject : *m_eflowCaloObjectContainer){

    /* Skip isolated tracks if flag set */
    if (!m_recoverIsolatedTracks && thisEflowCaloObject->nClusters() == 0) {
      unsigned int nTrk = thisEflowCaloObject->nTracks();
      // But make sure we get eflowObjects from them
      for (unsigned int iTrk = 0; iTrk < nTrk; ++iTrk) {
	      eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrk);
      	if (!thisEfRecTrack->isSubtracted()) thisEfRecTrack->setSubtracted();
      }
      continue;
    }

    /* Add all tracks on the eflowCaloObject that haven't been subtracted yet*/
    std::vector<eflowRecTrack*> updatedTracks; updatedTracks.clear();
    unsigned int nTracks = thisEflowCaloObject->nTracks();

    /* For cluster-only CaloObjects */
    if(nTracks == 0) continue;

    /* For track-only and track-cluster CaloObjects */
    for (unsigned int iTrack = 0; iTrack < nTracks; ++iTrack){
      eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrack);
      if (thisEfRecTrack->isSubtracted()){
        updatedTracks.push_back(thisEfRecTrack);
        continue;
      }
      thisEfRecTrack->clearClusterMatches();
      m_tracksToRecover.push_back(thisEfRecTrack);
    }
    thisEflowCaloObject->clearTracks();
    if (!updatedTracks.empty()) {
      thisEflowCaloObject->addTracks(updatedTracks);
      updatedTracks.clear();
    } else {
      thisEflowCaloObject->clearLinks();
    }
  }

  std::sort(m_tracksToRecover.begin(),m_tracksToRecover.end(),eflowRecTrack::SortDescendingPt());

}

int PFRecoverSplitShowersTool::matchAndCreateEflowCaloObj() {
  /* Cache the original number of eflowCaloObjects */
  const int nCaloObj = m_eflowCaloObjectContainer->size();

  /* loop tracks in m_tracksToRecover and do matching */
  for (auto thisEfRecTrack : m_tracksToRecover){
 
    /* No point to do anything if bin does not exist */
    if (!thisEfRecTrack->hasBin()) {
      std::unique_ptr<eflowCaloObject> thisEflowCaloObject = std::make_unique<eflowCaloObject>();
      thisEflowCaloObject->addTrack(thisEfRecTrack);
      m_eflowCaloObjectContainer->push_back(std::move(thisEflowCaloObject));
      continue;
    }
    if (msgLvl(MSG::DEBUG)){
      const xAOD::TrackParticle* track = thisEfRecTrack->getTrack();
      ATH_MSG_DEBUG("Recovering charged EFO with e,eta and phi " << track->e() << ", " << track->eta() << " and " << track->phi());
    }
    /* Get list of matched clusters */
    std::vector<eflowRecCluster*> matchedClusters = m_matchingTool->doMatches(thisEfRecTrack, m_clustersToConsider, -1);

    if (msgLvl(MSG::DEBUG)){
      for (auto thisEFRecCluster : matchedClusters) ATH_MSG_DEBUG("Have matched cluster with e, eta, phi of " << thisEFRecCluster->getCluster()->e() << ", " <<  thisEFRecCluster->getCluster()->eta() << " and " << thisEFRecCluster->getCluster()->phi());
    }

    if (matchedClusters.empty()) { continue; }

    m_nTrackClusterMatches += matchedClusters.size();
    /* Matched cluster: create TrackClusterLink and add it to both the track and the cluster (eflowCaloObject will be created later) */
    for (auto efRecCluster : matchedClusters){
      eflowTrackClusterLink* trackClusterLink = eflowTrackClusterLink::getInstance(thisEfRecTrack,efRecCluster);
      thisEfRecTrack->addClusterMatch(trackClusterLink);
      efRecCluster->addTrackMatch(trackClusterLink);
    }
  }

  /* Create all eflowCaloObjects that contain eflowRecCluster */
  eflowCaloObjectMaker makeCaloObject;
  int nCaloObjects = makeCaloObject.makeTrkCluCaloObjects(m_tracksToRecover, m_clustersToConsider,
                                                          m_eflowCaloObjectContainer);
  ATH_MSG_DEBUG("PFRecoverSplitShowersTool created " << nCaloObjects << " CaloObjects");

  /* integrate cells; determine FLI; eoverp */
  for (unsigned int iCalo = nCaloObj; iCalo < m_eflowCaloObjectContainer->size(); ++iCalo) {
    eflowCaloObject* thisEflowCaloObject = m_eflowCaloObjectContainer->at(iCalo);
    thisEflowCaloObject->simulateShower(m_integrator.get(), m_binnedParameters.get(), m_useUpdated2015ChargedShowerSubtraction);
  }
  return nCaloObj;
}

void PFRecoverSplitShowersTool::performSubtraction(eflowCaloObject* thisEflowCaloObject) {

  PFSubtractionStatusSetter pfSubtractionStatusSetter;

  for (unsigned iTrack = 0; iTrack < thisEflowCaloObject->nTracks(); ++iTrack) {
    eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrack);
    /* Get matched cluster via Links */
    std::vector<eflowRecCluster*> matchedClusters;
    matchedClusters.clear();
    std::vector<eflowTrackClusterLink*> links = thisEfRecTrack->getClusterMatches();
    for ( auto thisEFlowTrackClusterLink : links) matchedClusters.push_back(thisEFlowTrackClusterLink->getCluster());

    /* Do subtraction */
    std::vector<std::pair<xAOD::CaloCluster*, bool> > clusterSubtractionList;
    clusterSubtractionList.reserve(matchedClusters.size());
    std::map<xAOD::CaloCluster*, double> clusterEnergyMap;
    for (auto thisEFlowRecCluster : matchedClusters) {
      xAOD::CaloCluster* thisCluster = thisEFlowRecCluster->getCluster();
      clusterSubtractionList.push_back(std::pair(thisCluster,false));
      clusterEnergyMap[thisCluster] = thisCluster->e();
    }

    if (getSumEnergy(clusterSubtractionList) - thisEfRecTrack->getEExpect() < m_subtractionSigmaCut
        * sqrt(thisEfRecTrack->getVarEExpect())) {
      /* Check if we can annihilate right away */
      Subtractor::annihilateClusters(clusterSubtractionList);
      //Now we should mark all of these clusters as being subtracted
      //Now need to mark which clusters were modified in the subtraction procedure
      pfSubtractionStatusSetter.markAnnihStatus(*thisEflowCaloObject);
    } else {
      /* Subtract the track from all matched clusters */
      Subtractor::subtractTracksFromClusters(thisEfRecTrack, clusterSubtractionList);
          
      /* Annihilate the cluster(s) if the remnant is small (i.e. below k*sigma) */
      if (getSumEnergy(clusterSubtractionList) < m_subtractionSigmaCut
          * sqrt(thisEfRecTrack->getVarEExpect())) {
        Subtractor::annihilateClusters(clusterSubtractionList);
	      //Now we should mark all of these clusters as being subtracted
	      pfSubtractionStatusSetter.markAnnihStatus(*thisEflowCaloObject);
      }
      else{
        //Else now to mark which clusters were modified in the subtraction procedure
        std::vector<float> clusterSubtractedEnergyRatios;
        for (auto thisCluster: clusterSubtractionList) {
          if (fabs(thisCluster.first->e() - clusterEnergyMap[thisCluster.first]) > 0.0001) clusterSubtractedEnergyRatios.push_back(thisCluster.first->e()/clusterEnergyMap[thisCluster.first]);
          else clusterSubtractedEnergyRatios.push_back(NAN);
        }

	      pfSubtractionStatusSetter.markSubtractionStatus(clusterSubtractionList, clusterSubtractedEnergyRatios, *thisEflowCaloObject);
	
        } 

    }
    /* Flag tracks as subtracted */
    thisEfRecTrack->setSubtracted();
  }
}

void PFRecoverSplitShowersTool::performRecovery(int const nOriginalObj) {
  unsigned int nEFCaloObs = m_eflowCaloObjectContainer->size();
  for (unsigned int iCalo = nOriginalObj; iCalo < nEFCaloObs; ++iCalo) {
    eflowCaloObject* thisEflowCaloObject = m_eflowCaloObjectContainer->at(iCalo);
    performSubtraction(thisEflowCaloObject);
  }

}

double PFRecoverSplitShowersTool::getSumEnergy(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& clusters) {
  double result = 0.0;
  for (auto thisPair : clusters) result += (thisPair.first)->e();
  return result;
}
