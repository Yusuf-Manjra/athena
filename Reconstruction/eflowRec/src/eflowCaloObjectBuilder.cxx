/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowCaloObjectBuilder.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  8th November, 2001

********************************************************************/

#include "eflowRec/eflowUtil.h"

// INCLUDE HEADER FILES:

#include "eflowRec/eflowCaloObjectBuilder.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowCellSubtractionManager.h"
#include "eflowRec/eflowBinnedParameters.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowCellEOverPTool.h"
#include "eflowRec/PFTrackClusterMatchingTool.h"

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowBaseAlg.h"
#include "eflowRec/eflowTrackExtrapolatorBaseAlgTool.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCellLink.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

#include "Particle/TrackParticle.h"
#include "ParticleEvent/TrackParticleAssocs.h"

#include "xAODMuon/Muon.h"

#include "xAODEgamma/ElectronxAODHelpers.h"

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"

// INCLUDE GAUDI HEADER FILES:

#include "GaudiKernel/SystemOfUnits.h"

#include "GaudiKernel/Property.h"

#include "GaudiKernel/ListItem.h"

#include <algorithm> 
#include <math.h>
#include <map>
#include <iostream>

//  END OF HEADER FILES INCLUDE

//  CONSTRUCTOR:
    
eflowCaloObjectBuilder::eflowCaloObjectBuilder(const std::string& name, ISvcLocator* pSvcLocator):
  eflowBaseAlg(name, pSvcLocator),
  m_tracksName("InDetTrackParticles"),
  m_eflowCaloObjectsOutputName("eflowCaloObjects01"),
  m_caloObjectContainer(0),
  m_binnedParameters(0),
  m_integrator(0),
  m_theEOverPTool("eflowCellEOverPTool",this),
  m_theTrackExtrapolatorTool("Trk::ParticleCaloExtensionTool",this),
  m_matchingTool("PFTrackClusterMatchingTool/CalObjBldMatchingTool", this),
  m_eflowMode("FullMode"),
  m_selectedElectrons(nullptr),
  m_selectedMuons(nullptr),
  m_leptonCellContainer(nullptr),
  m_egammaTrackMapName("GSFTrackAssociation"),
  m_electronsName("Electrons"),
  m_muonsName("Muons"),
  m_useLeptons(true),
  m_storeLeptonCells(false),
  m_eflowElectronsName("eflowRec_selectedElectrons_EM"),
  m_eflowLeptonCellsName("eflowRec_leptonCellContainer_EM"),
  m_nMatches(0),
  m_useUpdated2015LeptonID(false),
  m_useUpdated2015ChargedShowerSubtraction(true)
{
// The following properties can be specified at run-time
// (declared in jobOptions file)
  declareProperty("ClustersName", m_clustersName);
  declareProperty("TracksName", m_tracksName);
  declareProperty("EflowCaloObjectsOutputName", m_eflowCaloObjectsOutputName);
  declareProperty("PFTrackClusterMatchingTool", m_matchingTool, "The track-cluster matching tool");
  declareProperty("eflowCellEOverPTool", m_theEOverPTool, "Energy Flow E/P Values and Shower Paremeters Tool");
  declareProperty("TrackExtrapolatorTool", m_theTrackExtrapolatorTool, "AlgTool to use for track extrapolation");
  declareProperty("EFlowMode", m_eflowMode);
  declareProperty("egammaTrackMapName", m_egammaTrackMapName);
  declareProperty("electronsName", m_electronsName);
  declareProperty("muonsName", m_muonsName);
  declareProperty("useLeptons", m_useLeptons);
  declareProperty("storeLeptonCells", m_storeLeptonCells);
  declareProperty("eflowElectronsName", m_eflowElectronsName);
  declareProperty("eflowLeptonCellsName", m_eflowLeptonCellsName);
  declareProperty("TrackSelectionTool", m_selTool);
  declareProperty("useUpdated2015LeptonID",m_useUpdated2015LeptonID);
  declareProperty("useUpdated2015ChargedShowerSubtraction",m_useUpdated2015ChargedShowerSubtraction);
}

eflowCaloObjectBuilder::~eflowCaloObjectBuilder() {
  delete m_integrator;
  delete m_binnedParameters;
}

StatusCode eflowCaloObjectBuilder::initialize() {

  StatusCode sc;

  // tool service
  IToolSvc* myToolSvc;
  sc = service("ToolSvc", myToolSvc);

  if (sc.isFailure()) {
    msg(MSG::WARNING) << " Tool Service Not Found" << endreq;
    return StatusCode::SUCCESS;
  }

  m_integrator = new eflowLayerIntegrator(0.032, 1.0e-3, 3.0);
  m_binnedParameters = new eflowBinnedParameters();
  eflowCellSubtractionManager::setRMaxAndWeightRange(0.75, 1.0e6);

  sc = m_theEOverPTool.retrieve();

  if (sc.isFailure()) {
    msg(MSG::WARNING) << "Cannot find eflowEOverPTool" << endreq;
    return StatusCode::SUCCESS;
  }

  if (m_matchingTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot find PFTrackClusterMatchingTool" << endreq;
  }

  sc = m_theTrackExtrapolatorTool.retrieve();

  if (sc.isFailure()) {
    msg(MSG::WARNING) << "Cannot find eflowTrackToCaloExtrapolatroTool " << endreq;
    return StatusCode::SUCCESS;
  }

  sc = m_theEOverPTool->execute(m_binnedParameters);

  if (sc.isFailure()) {
    msg(MSG::WARNING) << "Could not execute eflowCellEOverPTool " << endreq;
    return StatusCode::SUCCESS;
  }

  if (m_useLeptons) m_selectedMuons = new xAOD::MuonContainer(SG::VIEW_ELEMENTS);

  ATH_CHECK(m_selTool.retrieve());
  return sc;
}

StatusCode eflowCaloObjectBuilder::finalize() {

  msg(MSG::INFO) << "Produced " << m_nMatches << " track-cluster matches." << endreq;

  if (m_useLeptons && m_selectedMuons) delete m_selectedMuons;

  return StatusCode::SUCCESS;

}

StatusCode eflowCaloObjectBuilder::execute() {

  if (m_useLeptons) this->clearContainers();
  int debug = 0;
  StatusCode sc;

  /* Create the eflowCaloObjectContainer and register it */
  m_caloObjectContainer = new eflowCaloObjectContainer();
  sc = evtStore()->record(m_caloObjectContainer, m_eflowCaloObjectsOutputName,false);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Could not record eflowCaloObjectContainer in TDS" << endreq;
    }
    return StatusCode::SUCCESS;
  }

  if (m_useLeptons) {

    /* Put electron container and list of lepton cells into Storegate 
     By contrast muons do NOT go into storegate, because they are not needed downstream */
    if (recordLeptonContainers().isFailure()) {
      return StatusCode::SUCCESS;
    }

    /* Select some electrons */
    sc = this->selectElectrons();
    if (sc.isFailure()) {
      //won't mask out the tracks, but issue WARNING
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Did not select any electrons " << endreq;
    }

    /* Select some muons */
    sc = this->selectMuons();
    if (sc.isFailure()) {
      //won't mask out the tracks, but issue WARNING
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Did not select any muons " << endreq;
    }
  }

  /* Create eflowCaloObject static calo cluster container */
  eflowCaloObject::setClusterContainerPtr(new xAOD::CaloClusterContainer(), new xAOD::CaloClusterAuxContainer());

  /* Collect all calo clusters from all cluster containers in m_clusterList */
  if (makeClusterList().isFailure()) {
    return StatusCode::SUCCESS;
  }

  /* Apply the track selection, collect selected tracks is m_trackList */
  if (makeTrackList().isFailure()) {
    return StatusCode::SUCCESS;
  }

  /* Add each track to its best matching cluster's eflowCaloObject */
  matchTracksWithClusters(debug);

  /* Print some debug output if configured to debug mode */
  if (1 == debug) { printAllClusters(); }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////

StatusCode eflowCaloObjectBuilder::makeClusterList() {

  /* Retrieve the cluster container */
  const xAOD::CaloClusterContainer* thisCaloClusterContainer;
  StatusCode code = evtStore()->retrieve(thisCaloClusterContainer, m_clustersName);
  if (evtStore()->retrieve(thisCaloClusterContainer, m_clustersName).isFailure() || !thisCaloClusterContainer) {
    msg(MSG::WARNING) << " Can not retrieve cluster Container: " << m_clustersName << endreq;
    return StatusCode::SUCCESS;
  }

  /* Fill the vector of eflowRecClusters */
  m_eflowClusterList.clear();
  unsigned int nClusters = thisCaloClusterContainer->size();
  m_eflowClusterList.reserve(nClusters);
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
    eflowRecCluster* thisEFRecCluster = new eflowRecCluster(ElementLink<xAOD::CaloClusterContainer>(*thisCaloClusterContainer, iCluster));
    m_eflowClusterList.push_back(thisEFRecCluster);

    if (msgLvl(MSG::DEBUG)) {
      const xAOD::CaloCluster* thisCluster = thisCaloClusterContainer->at(iCluster);
      msg(MSG::DEBUG) << "eflowCaloObjectBuilder clus = " << thisCluster->eta() << " "
		      << thisCluster->phi() << " " << thisCluster->e()/cosh(thisCluster->eta()) << " " << endreq;
      //Not sure if we can get this
	//<< thisCluster->getNumberOfCells() << " " << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode eflowCaloObjectBuilder::makeTrackList() {
  /* Retrieve xAOD::TrackParticle Container, return 'failure' if not existing */
  const xAOD::TrackParticleContainer* trackContainer;
  StatusCode sc = evtStore()->retrieve(trackContainer, m_tracksName);
  if (sc.isFailure() || !trackContainer) {
    if (msgLvl(MSG::WARNING)) { msg(MSG::WARNING) << " No track container found in TDS" << endreq; }
    return StatusCode::FAILURE;
  }

  /* Do the track selection for tracks to be used in all of the following steps: */
  /* TODO (tuning): Check if resize(0) might be faster than clear() */
  m_eflowTrackList.clear();
  m_eflowTrackList.reserve(trackContainer->size());
  xAOD::TrackParticleContainer::const_iterator itTrackParticle = trackContainer->begin();
  int trackIndex = 0;
  for (; itTrackParticle != trackContainer->end(); ++itTrackParticle, ++trackIndex) {
    const xAOD::TrackParticle* track = (*itTrackParticle);
    if (!track) continue; // TODO: Print a WARNING here!

    bool rejectTrack((m_eflowMode == "FullMode") && !selectTrack(track));

    if (m_useLeptons) {
      bool isElectron = this->isElectron(track);
      bool isMuon = this->isMuon(track);
      if (true == isElectron || true == isMuon) rejectTrack = true;
    }

    if (!rejectTrack) {
      ElementLink<xAOD::TrackParticleContainer> trackElementLink(*trackContainer, trackIndex);
      m_eflowTrackList.push_back(new eflowRecTrack(trackElementLink, m_theTrackExtrapolatorTool));
    }
  }

  std::sort(m_eflowTrackList.begin(), m_eflowTrackList.end(), eflowRecTrack::SortDescendingPt());

  return StatusCode::SUCCESS;
}

bool eflowCaloObjectBuilder::selectTrack(const xAOD::TrackParticle* track) {

  //The upper track pt cut depends on which calorimeter shower subtraction scheme is used
  if (true == m_useUpdated2015ChargedShowerSubtraction) {
    if (track->pt()*0.001 < 100.0) return m_selTool->accept(*track, track->vertex());
    else return false;
  }
  else{
    if (track->pt()*0.001 < 40.0) return m_selTool->accept(*track, track->vertex());
    else return false;
  }

  return false;
}

void eflowCaloObjectBuilder::matchTracksWithClusters(int debug) {
  /* Loop through all tracks */
  std::vector<eflowRecTrack*>::iterator trackIter = m_eflowTrackList.begin();
  std::vector<eflowRecTrack*>::iterator trackEnd = m_eflowTrackList.end();
  for (; trackIter != trackEnd; ++trackIter) {
    eflowRecCluster* bestCluster = m_matchingTool->bestMatch(*trackIter, m_eflowClusterList);
    if (!bestCluster) {
      /* Track only (no match): Book new Calo Object with track */
      eflowCaloObject* calob = new eflowCaloObject();
      calob->addTrack(*trackIter);
      m_caloObjectContainer->push_back(calob);
      if (1 == debug) {
        std::cout << "Isolated" << printTrack((*trackIter)->getTrack()) << std::endl;
      }
    } else {
      /* Matched cluster: create TrackClusterLink and add it to both the track and the cluster (eflowCaloObject will be created later) */
      m_nMatches++;
      eflowTrackClusterLink* trackClusterLink = eflowTrackClusterLink::getInstance(*trackIter,
                                                                                   bestCluster);
      (*trackIter)->addClusterMatch(trackClusterLink);
      bestCluster->addTrackMatch(trackClusterLink);

      /* Do shower simulation:
       * - do cell integration and determine LFI
       * - run the cellSubtractionManager
       * - set expected energy to the eflowRecTrack object
       */
      simulateShower(trackClusterLink);
    }
  }

  /* Create all eflowCaloObjects that contain a cluster */
  unsigned int nClusters = m_eflowClusterList.size();
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
    /* Create the eflowCaloObject and put it in the container */
    eflowCaloObject* thisEflowCaloObject = new eflowCaloObject();
    m_caloObjectContainer->push_back(thisEflowCaloObject);

    /* Add the eflowRecCluster */
    eflowRecCluster* thisEFRecCluster = m_eflowClusterList[iCluster];
    thisEflowCaloObject->addCluster(thisEFRecCluster);

    /* Add all tracks that were matched to the cluster */
    const std::vector<eflowTrackClusterLink*>& theseTrackLinks = thisEFRecCluster->getTrackMatches();
    unsigned int nMatches = theseTrackLinks.size();
    for (unsigned int iMatch = 0; iMatch < nMatches; ++iMatch) {
      thisEflowCaloObject->addTrack(theseTrackLinks[iMatch]->getTrack(), theseTrackLinks[iMatch]);
    }
  }
}

std::string eflowCaloObjectBuilder::printTrack(const xAOD::TrackParticle* track) {
  std::stringstream result;
  result << " track with E, eta and phi "<< track->e() << ", " << track->eta() << " and " << track->phi();
  return result.str();
}

std::string eflowCaloObjectBuilder::printCluster(const xAOD::CaloCluster* cluster) {
  std::stringstream result;
  result << " cluster with E, eta and phi of " << cluster->e() << ", " << cluster->eta() << " and " << cluster->phi();
  return result.str();
}

void eflowCaloObjectBuilder::simulateShower(eflowTrackClusterLink* trackClusterLink) {

  eflowRecTrack* efRecTrack(trackClusterLink->getTrack());

  std::vector<eflowRecCluster*>::iterator itCluster = m_eflowClusterList.begin();
  std::vector<eflowRecCluster*>::iterator endCluster = m_eflowClusterList.end();
  
  std::vector<eflowRecCluster*> RSSclusters;

  float totalE=0.0;
  float trkEta=efRecTrack->getTrackCaloPoints().getEM2etaPhiPos().getEta();//get EM2 eta
  for(; itCluster != endCluster; ++itCluster) {
    if ((*itCluster)->getCluster()->e()>0.){
      float disphi=efRecTrack->getTrackCaloPoints().getEM2etaPhiPos().getPhi().getAbsDifference((*itCluster)->getCluster()->phi());
      float diseta = trkEta - (*itCluster)->getCluster()->eta();
      if (0.15*0.15>disphi*disphi+diseta*diseta) totalE += (*itCluster)->getCluster()->e(); 
      if (0.2*0.2>disphi*disphi+diseta*diseta) {
	eflowRecCluster* thisEfRecCluster = *itCluster;
	RSSclusters.push_back(thisEfRecCluster); 
      }//make list of clusters in 0.2 cone
    }//+ve energy clusters only are considered
  }//cluster loop

  double trackEM1eta = efRecTrack->getTrackCaloPoints().getEM1eta();
  double trackE = efRecTrack->getTrack()->e();

  if (!m_binnedParameters->binExists(trackE, trackEM1eta)) {
    double eta = efRecTrack->getTrack()->eta();
    msg(MSG::WARNING) << "Found bin fail! eta = " << eta << ", etaEM1 = " << trackEM1eta << ", e = " << trackE << endreq;
    efRecTrack->setHasBin(false);
    return;
  }

  /* Determine LFI */
  m_integrator->measureNewClus(trackClusterLink);
  eflowFirstIntENUM j1st = m_integrator->getFirstIntLayer();

  /* "Fill" the cellSubtractionManager */
  eflowCellSubtractionManager& cellSubtractionManager = efRecTrack->getCellSubtractionManager();
  m_binnedParameters->getOrdering(cellSubtractionManager, trackE, trackEM1eta, j1st);

  /* Set expected energy in the eflowRecTrack object */
  const double eExpect = cellSubtractionManager.fudgeMean() * trackE;
  const double sigmaEExpect = cellSubtractionManager.fudgeStdDev() * trackE;

  float pull=(totalE-eExpect)/sigmaEExpect;
  double trackPt = efRecTrack->getTrack()->pt();
  efRecTrack->setpull15(pull);
  if (pull>0.0+(log10(40000)-log10(trackPt))*33.2 && sigmaEExpect!=0.0 && true == m_useUpdated2015ChargedShowerSubtraction){ 
    efRecTrack->setSubtracted(); //this tricks eflowRec into thinking this track was subtracted, and hence no further subtraction will be done
    efRecTrack->setIsInDenseEnvironment();
    //recalculate the LHED and the ordering  and find the new  expected E + sigma of expected E (the new LHED can change the latter two values we find in the look up tables)
    m_integrator->measureNewClus(RSSclusters, efRecTrack);
    j1st = m_integrator->getFirstIntLayer();
    eflowCellSubtractionManager ranking;
    m_binnedParameters->getOrdering(ranking, trackE, trackEM1eta, j1st);
    efRecTrack->setEExpect(ranking.fudgeMean() * trackE, fabs(ranking.fudgeStdDev()*trackE)*fabs(ranking.fudgeStdDev()*trackE)); 
  }//if the cut in the pull vs track pt plane tells us the environment is too dense for the charged shower subtraction to work
  else {
    efRecTrack->setEExpect(eExpect, sigmaEExpect*sigmaEExpect);
  }//ok to do subtraction, and so we just set the usual expected E + sigma of expected E needed for subtraction

}

void eflowCaloObjectBuilder::printAllClusters() {
  std::vector<eflowRecCluster*>::iterator itCluster = m_eflowClusterList.begin();
  std::vector<eflowRecCluster*>::iterator endCluster = m_eflowClusterList.end();
  for (; itCluster != endCluster; ++itCluster) {
    if ((*itCluster)->getNTracks() == 0) {
      std::cout << "Isolated" << printCluster((*itCluster)->getCluster()) << std::endl;
    } else {
      std::cout << "Matched" << printCluster((*itCluster)->getCluster()) << std::endl;
      int nTrackMatches = (*itCluster)->getNTracks();
      std::vector<eflowTrackClusterLink*> theTrackLinks = (*itCluster)->getTrackMatches();
      for (int iTrackMatch = 0; iTrackMatch < nTrackMatches; ++iTrackMatch) {
        std::cout << "Matched" << printTrack(theTrackLinks[iTrackMatch]->getTrack()->getTrack()) << std::endl;
      }
    }
  }
}

StatusCode eflowCaloObjectBuilder::recordLeptonContainers(){

  m_selectedElectrons = new xAOD::ElectronContainer(SG::VIEW_ELEMENTS);

  StatusCode sc = evtStore()->record(m_selectedElectrons,m_eflowElectronsName,false);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)
        << "Could not record egammaContainer in TDS"
        << endreq;
    return sc;
  }

  if (true == m_storeLeptonCells) {
    m_leptonCellContainer = new ConstDataVector<CaloCellContainer>(SG::VIEW_ELEMENTS);

    //record the cell container
    sc = evtStore()->record(m_leptonCellContainer, m_eflowLeptonCellsName, false);

    if (sc.isFailure()) {
      if (msgLvl(MSG::WARNING))
        msg(MSG::WARNING) << "Could not record eflowRec LeptonCellContainer in TDS" << endreq;
      return sc;
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode eflowCaloObjectBuilder::selectMuons() {

  const xAOD::MuonContainer* muonContainer(NULL);
  StatusCode sc = evtStore()->retrieve(muonContainer, m_muonsName);
  if (sc.isFailure() || !muonContainer) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " No xAOD Muon container found in TDS with the name " << m_muonsName << endreq;
    //fall back on staco muons, so we can run on older files without third chain
    return this->selectStacoMuons();
  }

  xAOD::MuonContainer::const_iterator firstMuon = muonContainer->begin();
  xAOD::MuonContainer::const_iterator lastMuon = muonContainer->end();

  for (; firstMuon != lastMuon; ++firstMuon) {
    const xAOD::Muon* theMuon = *firstMuon;

    if (true == m_useUpdated2015LeptonID){
      //Details of medium muons are here:
      //https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool
      //No need to ask for combined muon, by construction other muons will not have ID track - we just ask for medium muons

      xAOD::Muon::Quality muonQuality = theMuon->quality();
      if( muonQuality <= xAOD::Muon::Medium) {    
	if (m_selectedMuons) {
	  m_selectedMuons->push_back(const_cast<xAOD::Muon*>(theMuon));
	} else if (msgLvl(MSG::WARNING)) {
	  msg(MSG::WARNING) << " Invalid pointer to m_selectedMuons in selectMuons " << std::endl;
	}
	if (true == m_storeLeptonCells) this->storeMuonCells(theMuon);
      }//Medium muons
    }//new lepton ID
    else {
      //This is what was used in 20.1.X 2015 data taking
      xAOD::Muon::MuonType muonType = theMuon->muonType();
      if (xAOD::Muon::Combined == muonType && theMuon->pt() > 5000.) {

	const ElementLink<xAOD::TrackParticleContainer> theLink = theMuon->primaryTrackParticleLink();
	if (theLink.isValid()) {
	  const xAOD::TrackParticle* primary_track = *theLink;
	  if (primary_track) {

	    float chi2 = primary_track->chiSquared();
	    float numberOfDoF = primary_track->numberDoF();
	    float chi2OverDoF = chi2 / numberOfDoF;
	    if (chi2OverDoF < 10) {
	      if (m_selectedMuons) {
		m_selectedMuons->push_back(const_cast<xAOD::Muon*>(theMuon));
	      } else if (msgLvl(MSG::WARNING)) {
		msg(MSG::WARNING) << " Invalid pointer to m_selectedMuons in selectMuons " << std::endl;
	      }
	      if (true == m_storeLeptonCells) this->storeMuonCells(theMuon);
	    } //chi2 cut
	  } else if (msgLvl(MSG::WARNING))
	    msg(MSG::WARNING) << " This muon has an invalid pointer to its primary track " << endreq;
	} else if (msgLvl(MSG::WARNING))
	  msg(MSG::WARNING) << " This muon has an invalid link to its primary track " << endreq;
      } //isCombinedMuon with pt > 5 GeV
    }//old lepton ID
  } //muon loop

  return StatusCode::SUCCESS;
}

StatusCode eflowCaloObjectBuilder::selectStacoMuons() {

  const xAOD::MuonContainer* muonContainer(NULL);
  StatusCode sc = evtStore()->retrieve(muonContainer, "StacoMuonCollection");
  if (sc.isFailure() || !muonContainer) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " No Staco Muon container found in TDS" << endreq;
    return StatusCode::FAILURE;
  }

  xAOD::MuonContainer::const_iterator firstMuon = muonContainer->begin();
  xAOD::MuonContainer::const_iterator lastMuon = muonContainer->end();

  for (; firstMuon != lastMuon ; ++firstMuon) {
    const xAOD::Muon* theMuon = *firstMuon;
    if (theMuon){
      if (theMuon->pt() > 6000 && fabs(theMuon->eta()) < 2.5) {
	xAOD::Muon::MuonType muonType = theMuon->muonType();
	if (xAOD::Muon::Combined == muonType || xAOD::Muon::SegmentTagged == muonType){
	  xAOD::Muon::Quality quality = theMuon->quality();
          if (xAOD::Muon::Loose == quality) {
	    const xAOD::TrackParticle* ID_track = *theMuon->inDetTrackParticleLink();

            if (ID_track) {
	      
	      //in xAOD version have remvoed b-layer cuts which appear not to be required according to MCP twiki
	      uint8_t numberOfPixelHits = 0, numberOfPixelDeadSensors = 0;
	      bool gotValue = ID_track->summaryValue(numberOfPixelHits, xAOD::numberOfPixelHits);
	      if (false == gotValue) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not get number of pixel hits " << endreq;
	      gotValue = ID_track->summaryValue(numberOfPixelDeadSensors, xAOD::numberOfPixelDeadSensors);
	      if (false == gotValue) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not get number of pixel dead sensors " << endreq;

	      if ( (numberOfPixelHits + numberOfPixelDeadSensors) >= 1){
		uint8_t numberOfSCTHits = 0, numberOfSCTDeadSensors = 0;

		gotValue = ID_track->summaryValue(numberOfSCTHits, xAOD::numberOfSCTHits);
		if (false == gotValue) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not get number of SCT hits " << endreq;
		gotValue = ID_track->summaryValue(numberOfSCTDeadSensors, xAOD::numberOfSCTDeadSensors);
		if (false == gotValue) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not get number of SCT dead sensors " << endreq;

		if ( (numberOfSCTHits + numberOfSCTDeadSensors) >= 5){

		  uint8_t numberOfPixelHoles = 0, numberOfSCTHoles = 0;
		  gotValue = ID_track->summaryValue(numberOfPixelHoles, xAOD::numberOfPixelHoles);
		  if (false == gotValue) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not get number of pixel holes " << endreq;
		  gotValue = ID_track->summaryValue(numberOfSCTHoles, xAOD::numberOfSCTHoles);
		  if (false == gotValue) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not get number of SCT holes " << endreq;

		  if ( (numberOfPixelHoles + numberOfSCTHoles) < 3){

		    uint8_t  numberOfTRTOutliers = 0, numberOfTRTHits = 0;
		    gotValue = ID_track->summaryValue(numberOfTRTOutliers,xAOD::numberOfTRTOutliers);
		    if (false == gotValue) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not get number of TRT outliers " << endreq;
		    gotValue = ID_track->summaryValue(numberOfTRTHits, xAOD::numberOfTRTHits);
		    if (false == gotValue) if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not get number of TRT Hits " << endreq;
		    int numberOfTRTTotal = numberOfTRTOutliers + numberOfTRTHits;

		    double trackEta = -log(tan(ID_track->theta()));
		    
		    bool TRTsuccess = true; 
                          
		    if (fabs(trackEta) < 1.9 && fabs(trackEta) > 0.1){
		      if (numberOfTRTTotal <= 5 ||  numberOfTRTOutliers >= 0.9*(float)numberOfTRTTotal)  TRTsuccess = false;
		    }
		    else {
		      if ( numberOfTRTTotal > 5 && numberOfTRTOutliers >= 0.9*(float)numberOfTRTTotal)  TRTsuccess = false;
		    }

		    if (true == TRTsuccess){
		      if (m_selectedMuons) m_selectedMuons->push_back(const_cast<xAOD::Muon*>(theMuon));
		      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Invalid pointer to m_selectedMuons in selectStacoMuons " << std::endl;
		      if (true == m_storeLeptonCells) this->storeMuonCells(theMuon);
		    }//select this muon
		  }//sct + pixel holes cut
		}//sct hits cut
	      }//pixel hits cut
	    }//if valid track pointer
            else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon does not have an ID track " << endreq;
          }//loose muon
        }//author
      }//kinematic cuts
    }//muon pointer valid
    else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon is a NULL pointer " << endreq;
  }//muon loop

  return StatusCode::SUCCESS;
}

bool eflowCaloObjectBuilder::isMuon(const xAOD::TrackParticle* track){

  if (m_selectedMuons){

    xAOD::MuonContainer::const_iterator firstMuon = m_selectedMuons->begin();
    xAOD::MuonContainer::const_iterator lastMuon = m_selectedMuons->end();
    
    for (; firstMuon != lastMuon ; ++firstMuon){
      const xAOD::Muon* theMuon = *firstMuon;
      if (theMuon){
	const ElementLink< xAOD::TrackParticleContainer > theLink = theMuon->inDetTrackParticleLink();
	if (theLink.isValid()){
	  const xAOD::TrackParticle* ID_track = *theLink;
	  if (ID_track){
	    if (track == ID_track) return true;
	    return false;
	  }
	  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon has a NULL pointer to the track " << endreq;
	}
	else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon has an invalid link to the track " << endreq;
      }//if muon pointer is valid
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon is a NULL pointer " << endreq;
    }//muon loop
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Invalid pointer to m_selectedMuons in isMuon " << std::endl;

  return false;
}


void eflowCaloObjectBuilder::storeMuonCells(const xAOD::Muon* muon){

  const ElementLink<xAOD::CaloClusterContainer> theLink =  muon->clusterLink(); 
  if (theLink.isValid()){
    const xAOD::CaloCluster* muonCluster = *theLink;
    if (muonCluster){
      this->storeLeptonCells(muonCluster);
      const CaloClusterCellLink* theCellLink = muonCluster->getCellLinks();
      CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
      CaloClusterCellLink::const_iterator lastCell = theCellLink->end();
      
      for (; firstCell != lastCell; ++firstCell){
      }//cell loop
      
    }
    else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon has an invalid pointer to its cluster " << endreq;
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon has an invalid element link to its cluster " << endreq;

}


StatusCode eflowCaloObjectBuilder::selectElectrons(){

  const xAOD::ElectronContainer* egammaContainer(0);
  StatusCode sc = evtStore()->retrieve(egammaContainer, m_electronsName);
  if (sc.isFailure() || !egammaContainer){
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " No Electron container found in TDS" << endreq;
    return StatusCode::FAILURE;
  }

  xAOD::ElectronContainer::const_iterator firstElectron = egammaContainer->begin();
  xAOD::ElectronContainer::const_iterator lastElectron = egammaContainer->end();

  for (; firstElectron != lastElectron; ++firstElectron){

    const xAOD::Electron* theElectron = *firstElectron;
    if (theElectron){
      if (theElectron->pt() > 10000){
        bool val_med = false;
	bool gotID = false;
	if (true == m_useUpdated2015LeptonID) gotID = theElectron->passSelection(val_med, "LHMedium");
	else gotID = theElectron->passSelection(val_med, "Medium");
	if (!gotID) {
	  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get Electron ID " << endreq;
	  continue;
	}
	if (true == val_med){
	  if (m_selectedElectrons) m_selectedElectrons->push_back(const_cast<xAOD::Electron*>(theElectron));
	  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Invalid pointer to m_selectedElectrons in selectElectrons " << std::endl;
	  if (true == m_storeLeptonCells) this->storeElectronCells(theElectron);
	}//mediumPP
      }//10GeV pt cut
    }//valid egamma pointer
    else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This electron is a NULL pointer " << endreq;
    
  }//electron loop

  return StatusCode::SUCCESS;

}

bool eflowCaloObjectBuilder::isElectron(const xAOD::TrackParticle* track){

  if (m_selectedElectrons){

    xAOD::ElectronContainer::iterator firstElectron = m_selectedElectrons->begin();
    xAOD::ElectronContainer::iterator lastElectron = m_selectedElectrons->end();
    
    for (; firstElectron != lastElectron; ++firstElectron){
      const xAOD::Electron* this_egamma = *firstElectron;
      if (this_egamma){
	unsigned int nTrack = this_egamma->nTrackParticles();
	
	if (0 != nTrack){
	  
	  const xAOD::TrackParticle* origTrack = xAOD::EgammaHelpers::getOriginalTrackParticle(this_egamma);
	  
	  if (origTrack){
	    if (track == origTrack) {
	      return true;
	    }
	  }//if valid track pointer
	  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Electron object map has NULL pointer to original TrackParticle " << endreq;
	}//if has a track
	else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Electron object has " << nTrack << " tracks " << endreq;
      }//if valid pointer
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Electron is a NULL pointer " << endreq;
    }//electron loop    
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Invalid pointer to m_selectedElectrons in isElectron " << endreq;

  return false;

}

void eflowCaloObjectBuilder::storeElectronCells(const xAOD::Egamma* electron){

  const xAOD::CaloCluster* electronCluster = electron->caloCluster(); 
  if (electronCluster){
    this->storeLeptonCells(electronCluster);      
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This electron has an invalid pointer to its cluster " << endreq;

}

void eflowCaloObjectBuilder::storeLeptonCells(const xAOD::CaloCluster* theCluster){

  const CaloClusterCellLink* theCellLink = theCluster->getCellLinks();

  if (theCellLink){
  
    CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
    CaloClusterCellLink::const_iterator lastCell = theCellLink->end();
    
    for (; firstCell != lastCell; ++firstCell){
      if (m_leptonCellContainer) m_leptonCellContainer->push_back(*firstCell);
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Invalid pointer to m_leptonCellContainer in storeLeptonCells" << endreq;
    }//cell loop
  }
 else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This cluster has an invalid pointer to its cells, in storeLeptonCells " << endreq;

}

void eflowCaloObjectBuilder::clearContainers(){
  if (m_useLeptons && m_selectedMuons) m_selectedMuons->clear();
}
