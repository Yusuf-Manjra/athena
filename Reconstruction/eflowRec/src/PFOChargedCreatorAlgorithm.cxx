/*                                                                                                                                                                                                                                        
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration                                                                                                                                                                
*/

#include "eflowRec/PFOChargedCreatorAlgorithm.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"

#include "xAODPFlow/PFOAuxContainer.h"

PFOChargedCreatorAlgorithm::PFOChargedCreatorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{
}

StatusCode PFOChargedCreatorAlgorithm::initialize(){

  ATH_CHECK(m_eflowCaloObjectContainerReadHandleKey.initialize());

  ATH_CHECK(m_chargedPFOContainerWriteHandleKey.initialize());
  
  return StatusCode::SUCCESS;
}

StatusCode  PFOChargedCreatorAlgorithm::execute(){

  ATH_MSG_DEBUG("Processing eflowCaloObjectContainer");

  SG::WriteHandle<xAOD::PFOContainer> chargedPFOContainerWriteHandle(m_chargedPFOContainerWriteHandleKey);
  ATH_CHECK(chargedPFOContainerWriteHandle.record(std::make_unique<xAOD::PFOContainer>(),std::make_unique<xAOD::PFOAuxContainer>()));
  
  /* Create Charged PFOs from all eflowCaloObjects */
  SG::ReadHandle<eflowCaloObjectContainer> eflowCaloObjectContainerReadHandle(m_eflowCaloObjectContainerReadHandleKey);
  for (auto thisEflowCaloObject : *eflowCaloObjectContainerReadHandle) createChargedPFO(*thisEflowCaloObject,true,chargedPFOContainerWriteHandle);

  return StatusCode::SUCCESS;  
}

StatusCode PFOChargedCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS; }

void PFOChargedCreatorAlgorithm::createChargedPFO(const eflowCaloObject& energyFlowCaloObject, bool addClusters, SG::WriteHandle<xAOD::PFOContainer>& chargedPFOContainerWriteHandle){

  /* Loop over all tracks in the eflowCaloObject */
  int nTracks = energyFlowCaloObject.nTracks();
  for (int iTrack = 0; iTrack < nTracks; ++iTrack) {

    eflowRecTrack* efRecTrack = energyFlowCaloObject.efRecTrack(iTrack);

    /* Skip tracks that haven't been subtracted */
    if (false == m_eOverPMode){
      if (!efRecTrack->isSubtracted()){ continue; }
    }
    
    /* Create new xAOD::PFO */
    xAOD::PFO* thisPFO = new xAOD::PFO();
    chargedPFOContainerWriteHandle->push_back(thisPFO);

    /* Get the track elementLink and add it to the xAOD:PFO  */
    ElementLink<xAOD::TrackParticleContainer> theTrackLink = efRecTrack->getTrackElemLink();
    bool isSet = thisPFO->setTrackLink(theTrackLink);
    if (!isSet) ATH_MSG_WARNING("Could not set Track B in PFO ");
    thisPFO->setCharge(efRecTrack->getTrack()->charge());

    std::pair<double,double> etaPhi(0.0,0.0);

    if (m_eOverPMode){
      /* In EOverPMode want charged eflowObjects to have extrapolated eta,phi as coordinates
       * (needed for analysis of EOverP Data) */
      etaPhi = efRecTrack->getTrackCaloPoints().getEM2etaPhi();
        
      /*add information to xAOD*/
      xAOD::PFODetails::PFOAttributes myAttribute_layerHED = xAOD::PFODetails::PFOAttributes::eflowRec_layerHED;
      thisPFO->setAttribute<int>(myAttribute_layerHED,efRecTrack->getLayerHED() );
      
      xAOD::PFODetails::PFOAttributes myAttribute_layerVectorCellOrdering = xAOD::PFODetails::PFOAttributes::eflowRec_layerVectorCellOrdering;
      thisPFO->setAttribute<std::vector<int> >(myAttribute_layerVectorCellOrdering,efRecTrack->getLayerCellOrderVector() );
      
      xAOD::PFODetails::PFOAttributes myAttribute_radiusVectorCellOrdering = xAOD::PFODetails::PFOAttributes::eflowRec_radiusVectorCellOrdering;
      thisPFO->setAttribute<std::vector<float> >(myAttribute_radiusVectorCellOrdering,efRecTrack->getRadiusCellOrderVector() );
        
      xAOD::PFODetails::PFOAttributes myAttribute_avgEdensityVectorCellOrdering = xAOD::PFODetails::PFOAttributes::eflowRec_avgEdensityVectorCellOrdering;
      thisPFO->setAttribute<std::vector<float> >(myAttribute_avgEdensityVectorCellOrdering,efRecTrack->getAvgEDensityCellOrderVector() );              
    } else {
      /* In normal mode we want the track eta,phi at the perigee */
      etaPhi.first = efRecTrack->getTrack()->eta();
      etaPhi.second = efRecTrack->getTrack()->phi();
    }
    /* Set the 4-vector of the xAOD::PFO */
    thisPFO->setP4(efRecTrack->getTrack()->pt(), etaPhi.first, etaPhi.second, efRecTrack->getTrack()->m());

    ATH_MSG_DEBUG("Created charged PFO with E, pt, eta and phi of " << thisPFO->e() << ", " << thisPFO->pt() << ", " << thisPFO->eta() << " and " << thisPFO->phi());
    
    /* Add the amount of energy the track was expected to deposit in the calorimeter - this is needed to calculate the charged weight in the jet finding */
    xAOD::PFODetails::PFOAttributes myAttribute_tracksExpectedEnergyDeposit = xAOD::PFODetails::PFOAttributes::eflowRec_tracksExpectedEnergyDeposit;
    thisPFO->setAttribute<float>(myAttribute_tracksExpectedEnergyDeposit,efRecTrack->getEExpect() );
    
    /* Flag if this track was in a dense environment for later checking */
    xAOD::PFODetails::PFOAttributes myAttribute_isInDenseEnvironment = xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment;
    //There is some issue using bools - when written to disk they convert to chars. So lets store the bool as an int.
    thisPFO->setAttribute<int>(myAttribute_isInDenseEnvironment, (efRecTrack->isInDenseEnvironment()));

    /* Optionally we add the links to clusters to the xAOD::PFO */
    if (true == addClusters){

      std::vector<std::pair<eflowTrackClusterLink*,bool> > trackClusterLinkPairs = energyFlowCaloObject.efRecLink();

      std::vector<eflowTrackClusterLink*> thisTracks_trackClusterLinks = efRecTrack->getClusterMatches();

      /** Each eflowCaloObject has a list of clusters for all the tracks it represents.
       *  We only want the subset of the clusters matched to this track, and collect these in thisTracks_trackClusterLinksSubtracted.
       */

      std::vector<eflowTrackClusterLink*> thisTracks_trackClusterLinksSubtracted;

      for (auto trackClusterLink : thisTracks_trackClusterLinks){
        for (auto trackClusterLinkPair : trackClusterLinkPairs){
          if (trackClusterLinkPair.first == trackClusterLink && true == trackClusterLinkPair.second) {
            thisTracks_trackClusterLinksSubtracted.push_back(trackClusterLink);
          }
        }
      }

      //Now loop over the list of eflowTrackClusterLink which correspond to subtracted clusters matched to this track.
      
      for (auto trackClusterLink : thisTracks_trackClusterLinksSubtracted){	    

        eflowRecCluster* efRecCluster = trackClusterLink->getCluster();
        ElementLink<xAOD::CaloClusterContainer> theOriginalClusterLink = efRecCluster->getOriginalClusElementLink();

	      ElementLink<xAOD::CaloClusterContainer> theSisterClusterLink = (*theOriginalClusterLink)->getSisterClusterLink();
        if(theSisterClusterLink.isValid()) {
        	ATH_MSG_DEBUG("PFO with e and eta of " << thisPFO->e() << " and " << thisPFO->eta() << " is adding cluster with e, eta of " << (*theSisterClusterLink)->e() << " and " << (*theSisterClusterLink)->eta() << " an sistser has " << (*theOriginalClusterLink)->e() << " and " << (*theOriginalClusterLink)->eta());
        	bool isSet = thisPFO->setClusterLink(theSisterClusterLink);
        	if (!isSet) { ATH_MSG_WARNING( "Could not set Cluster in PFO " ); }
        } else {
        	ATH_MSG_DEBUG("PFO with e and eta of " << thisPFO->e() << " and " << thisPFO->eta() << " is adding cluster with e, eta of " << (*theOriginalClusterLink)->e() << " and " << (*theOriginalClusterLink)->eta());
        	bool isSet = thisPFO->setClusterLink(theOriginalClusterLink);
        	if (!isSet) { ATH_MSG_WARNING( "Could not set Cluster in PFO " ); }
	      }
      }//track-cluster link loop
    }//addClusters is set to true - so we added the clusters to the xAOD::PFO   

  }//loop over the tracks on the eflowCaloObject
}
