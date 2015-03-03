/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// TrigFastTrackFinder.cxx
// -------------------------------
// ATLAS Collaboration
//
// package created 16/04/2013 by Dmitry Emeliyanov (see ChangeLog for more details)
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <algorithm>
#include <array>

#include <tbb/parallel_for.h>
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h" 

#include "TrkParameters/TrackParameters.h" 
#include "TrkTrack/Track.h" 
#include "TrkTrack/TrackInfo.h" 

#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "IRegionSelector/IRegSelSvc.h"

#include "TrigInDetEvent/TrigSiSpacePointBase.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 

#include "TrigInDetPattRecoEvent/TrigSpacePointStorage.h"
#include "TrigInDetPattRecoEvent/TrigL2SpacePointStorage.h"
#include "TrigInDetPattRecoEvent/TrigL2SpacePointStorageFiller.h"
#include "TrigInDetPattRecoEvent/TrigL2TimeoutException.h"
#include "TrigInDetPattRecoEvent/TrigInDetTriplet.h"

#include "InDetRecToolInterfaces/ISiTrackMaker.h" 
#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"
#include "TrigInDetPattRecoTools/TrigTrackSeedGenerator.h"

#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "TrigInDetToolInterfaces/ITrigSpacePointConversionTool.h"
#include "TrigInDetToolInterfaces/ITrigL2SpacePointTruthTool.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"

#include "SiSpacePointsSeed/SiSpacePointsSeed.h"
#include "TrigFastTrackFinder/TrigFastTrackFinder.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

template <class SRC>
inline const DataVector<TrigInDetTrack>** dvec_cast(SRC** ptr) { 
  return (const DataVector<TrigInDetTrack>**)(ptr); 
} 

TrigFastTrackFinder::TrigFastTrackFinder(const std::string& name, ISvcLocator* pSvcLocator) : 

  HLT::FexAlgo(name, pSvcLocator), 
  m_numberingTool("TrigL2LayerNumberTool"), 
  m_spacePointTool("TrigSpacePointConversionTool"),
  m_TrigL2SpacePointTruthTool("TrigL2SpacePointTruthTool"),
  m_trackMaker("InDet::SiTrackMaker_xk/InDetTrigSiTrackMaker"),
  m_trigInDetTrackFitter("TrigInDetTrackFitter"),
  m_trackSummaryTool("Trk::ITrackSummaryTool/ITrackSummaryTool"),
  m_MagFieldSvc("AtlasFieldSvc",this->name()),
  m_shift_x(0.0),
  m_shift_y(0.0),
  m_ftkMode(false),
  m_useBeamSpot(true),
  m_doTrigInDetTrack(true),
  m_nfreeCut(5), 
  m_nTracks(0),
  m_nPixSPsInRoI(0),
  m_nSCTSPsInRoI(0),
  m_currentStage(-1),
  m_attachedFeatureName(""),
  m_attachedFeatureName_TIDT("")
{

  /** Doublet finding properties. */
  declareProperty("Doublet_FilterRZ",            m_tcs.m_doubletFilterRZ = true);

  /** Triplet finding properties. */

  declareProperty("Triplet_D0Max",            m_tcs.m_tripletD0Max      = 4.0);
  declareProperty("Triplet_D0_PPS_Max",       m_tcs.m_tripletD0_PPS_Max = 1.7);
  declareProperty("Triplet_nMaxPhiSlice",     m_tcs.m_nMaxPhiSlice = 53);
  declareProperty("Triplet_MaxBufferLength",     m_tcs.m_maxTripletBufferLength = 3);
  declareProperty("TripletDoPSS",            m_tcs.m_tripletDoPSS = false);


  m_tcs.m_magFieldZ = 2.0;//switched to configured value in getMagField()

  declareProperty( "VertexSeededMode",    m_vertexSeededMode = false);

  declareProperty("Triplet_MinPtFrac",        m_tripletMinPtFrac = 0.3);
  declareProperty("pTmin",                    m_pTmin = 1000.0);
  declareProperty("TrackInitialD0Max",            m_initialD0Max      = 10.0);

  declareProperty("doSeedRedundancyCheck",            m_checkSeedRedundancy = false);

  declareProperty( "MinHits",               m_minHits = 5 );

  declareProperty("DoTrigInDetTrack",            m_doTrigInDetTrack  = true);
  declareProperty( "OutputCollectionSuffix",m_outputCollectionSuffix = "");
 
  declareProperty( "UseBeamSpot",           m_useBeamSpot = true);
  declareProperty( "FreeClustersCut"   ,m_nfreeCut      );
  declareProperty( "SpacePointProviderTool", m_spacePointTool  );
  declareProperty( "LayerNumberTool", m_numberingTool  );

  declareProperty( "offlineTrackMaker", m_trackMaker);
  declareProperty( "trigInDetTrackFitter",   m_trigInDetTrackFitter );

  declareProperty("TrackSummaryTool", m_trackSummaryTool);
  declareProperty( "TrigL2SpacePointTruthTool", m_TrigL2SpacePointTruthTool);
  declareProperty( "retrieveBarCodes", m_retrieveBarCodes = false);
  declareProperty( "SignalBarCodes", m_vSignalBarCodes);
  declareProperty( "MinSignalSPs", m_minSignalSPs = 3);

  m_countTotalRoI=0;
  m_countRoIwithEnoughHits=0;
  m_countRoIwithTracks=0;
  m_l1Id=99999999;
  
  // declare monitoring histograms

  declareMonitoredStdContainer("trk_pt",         m_trk_pt);
  declareMonitoredStdContainer("trk_a0",         m_trk_a0);
  declareMonitoredStdContainer("trk_z0",         m_trk_z0);
  declareMonitoredStdContainer("trk_phi0",       m_trk_phi0);
  declareMonitoredStdContainer("trk_eta",        m_trk_eta);
  declareMonitoredStdContainer("trk_chi2dof",    m_trk_chi2dof);
  declareMonitoredStdContainer("trk_nSiHits",    m_trk_nSiHits);
  declareMonitoredStdContainer("trk_nPIXHits",   m_trk_nPIXHits);
  declareMonitoredStdContainer("trk_nSCTHits",   m_trk_nSCTHits);
  declareMonitoredStdContainer("trk_a0beam",     m_trk_a0beam);
  declareMonitoredStdContainer("trk_dPhi0",      m_trk_dPhi0);
  declareMonitoredStdContainer("trk_dEta" ,      m_trk_dEta);

  declareMonitoredVariable("roi_nTracks",m_nTracks);
  declareMonitoredVariable("roi_nSPsPIX",m_nPixSPsInRoI);
  declareMonitoredVariable("roi_nSPsSCT",m_nSCTSPsInRoI);
  declareMonitoredVariable("roi_lastStageExecuted",m_currentStage);
  declareMonitoredVariable("roi_eta", m_roiEta);
  declareMonitoredVariable("roi_etaWidth", m_roiEtaWidth);
  declareMonitoredVariable("roi_phi", m_roiPhi);
  declareMonitoredVariable("roi_phiWidth", m_roiPhiWidth);
  declareMonitoredVariable("roi_nSPs", m_roi_nSPs);
  declareMonitoredVariable("time_PattRecoOnly",m_timePattReco);

  // Z-vertexing 
  declareMonitoredVariable("roi_nZvertices",m_nZvertices);
  declareMonitoredStdContainer("roi_zVertices",m_zVertices);
  ////Spacepoints
  //declareMonitoredStdContainer("sp_x" ,m_sp_x);
  //declareMonitoredStdContainer("sp_y" ,m_sp_y);
  //declareMonitoredStdContainer("sp_z" ,m_sp_z);
  //declareMonitoredStdContainer("sp_r" ,m_sp_r);
}

//--------------------------------------------------------------------------

TrigFastTrackFinder::~TrigFastTrackFinder() {}

//-----------------------------------------------------------------------

HLT::ErrorCode TrigFastTrackFinder::hltInitialize() {

  ATH_MSG_DEBUG("TrigFastTrackFinder::initialize() "  << PACKAGE_VERSION);

  if ( timerSvc() ) {
    m_SpacePointConversionTimer = addTimer("SpacePointConversion"); 
    m_PatternRecoTimer          = addTimer("PattReco","PattReco_nSP");
    m_TripletMakingTimer        = addTimer("Triplets","Triplets_nSP");
    m_CombTrackingTimer         = addTimer("CmbTrack","CmbTrack_nTr");
    m_TrackFitterTimer          = addTimer("TrackFitter","TrackFitter_nTracks");
  }

  if(m_ftkMode) {
    // retrieve the FTK data reader here
  }

  StatusCode sc= m_trackSummaryTool.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("unable to locate track summary tool");
    return HLT::BAD_JOB_SETUP;
  }

  ATH_MSG_DEBUG(" TrigFastTrackFinder : MinHits set to " << m_minHits);

  if (m_useBeamSpot) {
    StatusCode scBS = service("BeamCondSvc", m_iBeamCondSvc);
    if (scBS.isFailure() || m_iBeamCondSvc == 0) {
      m_iBeamCondSvc = 0;
      ATH_MSG_WARNING("Could not retrieve Beam Conditions Service. ");
    }
  }

  sc=m_numberingTool.retrieve(); 
  if(sc.isFailure()) { 
    ATH_MSG_ERROR("Could not retrieve "<<m_numberingTool); 
    return HLT::BAD_JOB_SETUP;
   } 

  sc = m_spacePointTool.retrieve();
  if(sc.isFailure()) { 
    ATH_MSG_ERROR("Could not retrieve "<<m_spacePointTool); 
    return HLT::BAD_JOB_SETUP;
  }

  sc = m_MagFieldSvc.retrieve();
  if(sc.isFailure()) 
    {
      ATH_MSG_ERROR("Unable to retrieve Athena MagFieldService");
      return HLT::BAD_JOB_SETUP;
    }
  
  sc = m_trackMaker.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Could not retrieve "<<m_trackMaker); 
    return HLT::BAD_JOB_SETUP;
  }
  sc = m_trigInDetTrackFitter.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Could not retrieve "<<m_trigInDetTrackFitter); 
    return HLT::BAD_JOB_SETUP;
  }

  //Get ID helper
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_ERROR("Could not get AtlasDetectorID helper AtlasID");
    return HLT::BAD_JOB_SETUP;
  }
  
  if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) {
    ATH_MSG_ERROR("Could not get Pixel ID helper");
    return HLT::BAD_JOB_SETUP;
  }
  
  if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) { 
    ATH_MSG_ERROR("Could not get Pixel ID helper");
    return StatusCode::FAILURE;
  }
  
  if ( m_outputCollectionSuffix != "" ) {
    m_attachedFeatureName = string("TrigFastTrackFinder_") + m_outputCollectionSuffix;
    m_attachedFeatureName_TIDT = string("TrigFastTrackFinder_TrigInDetTrack") + m_outputCollectionSuffix;
  }
  else {
    m_attachedFeatureName      = string("TrigFastTrackFinder_");
    m_attachedFeatureName_TIDT = string("TrigFastTrackFinder_TrigInDetTrack");
  }

  if (m_retrieveBarCodes) {
    m_nSignalPresent=0;
    m_nSignalDetected=0;
    m_nSignalTracked=0;
    m_nSignalClones=0;
    sc = m_TrigL2SpacePointTruthTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("Unable to locate SpacePoint-to-Truth associator tool " << m_TrigL2SpacePointTruthTool);
      return HLT::BAD_JOB_SETUP;
    }
  }
  
  ATH_MSG_DEBUG(" Feature set recorded with Key " << m_attachedFeatureName);
  ATH_MSG_DEBUG(" Feature set recorded with Key " << m_attachedFeatureName_TIDT);
  ATH_MSG_DEBUG(" Initialized successfully"); 
  return HLT::OK;
}


//-------------------------------------------------------------------------

HLT::ErrorCode TrigFastTrackFinder::hltBeginRun()
{
  ATH_MSG_DEBUG("At BeginRun of " << name());

  //getting magic numbers from the layer numbering tool

  m_tcs.m_maxBarrelPix    = m_numberingTool->offsetBarrelSCT();
  m_tcs.m_minEndcapPix    = m_numberingTool->offsetEndcapPixels(); 
  m_tcs.m_maxEndcapPix    = m_numberingTool->offsetEndcapSCT();
  m_tcs.m_maxSiliconLayer = m_numberingTool->maxSiliconLayerNum();

  return HLT::OK;
}

//-------------------------------------------------------------------------

HLT::ErrorCode TrigFastTrackFinder::hltExecute(const HLT::TriggerElement* /*inputTE*/,
					       HLT::TriggerElement* outputTE) {

  StatusCode sc(StatusCode::SUCCESS);

  ATH_MSG_VERBOSE("TrigFastTrackFinder::execHLTAlgorithm()");

  clearMembers();

  // Retrieve vertexing information if needed

  const TrigVertexCollection* vertexCollection = nullptr;

  if(m_vertexSeededMode) {
    //HLT::ErrorCode status = getFeature(inputTE, vertexCollection,"");
    //
    //NOTE the inputTE vs outputTE difference - the feature is assumed to come from the same step in the sequence
    HLT::ErrorCode status = getFeature(outputTE, vertexCollection);
    if(status != HLT::OK) return status;
    if(vertexCollection==nullptr) return HLT::ERROR;
  }

 
  // 2. Retrieve beam spot and magnetic field information 
  //

  m_shift_x=0.0;
  m_shift_y=0.0;
  if(m_useBeamSpot && m_iBeamCondSvc) {
    getBeamSpot();
  }
  else {
    m_vertex = Amg::Vector3D(0.0,0.0,0.0);
  }
  
  getMagField();

  m_currentStage = 1;
  
  std::vector<TrigSiSpacePointBase> convertedSpacePoints;
  convertedSpacePoints.reserve(5000);
  
  if ( timerSvc() ) m_SpacePointConversionTimer->start();
  
  // 4. RoI preparation/update 
  const IRoiDescriptor* internalRoI;
  HLT::ErrorCode ec = getRoI(outputTE, internalRoI);
  if (ec!=HLT::OK) {
    return ec;
  }
  
  m_countTotalRoI++;
  m_tcs.roiDescriptor = internalRoI;
  
  sc = m_spacePointTool->getSpacePoints( *internalRoI, convertedSpacePoints, m_nPixSPsInRoI, m_nSCTSPsInRoI);
  ////Record spacepoint x and y
  //for(std::vector<TrigSiSpacePointBase>::const_iterator spIt = convertedSpacePoints.begin(); spIt != convertedSpacePoints.end(); ++spIt) {
  //  m_sp_x.push_back((*spIt).original_x());
  //  m_sp_y.push_back((*spIt).original_y());
  //  m_sp_z.push_back((*spIt).z());
  //  m_sp_r.push_back((*spIt).r());
  //}

  if(sc.isFailure()) { 
    ATH_MSG_WARNING("REGTEST / Failed to retrieve offline spacepoints ");
    return HLT::TOOL_FAILURE;
  }

  if ( timerSvc() ) m_SpacePointConversionTimer->stop();

  m_roi_nSPs = convertedSpacePoints.size();    
  ATH_MSG_VERBOSE(m_roi_nSPs <<" spacepoints found");
  
  if( m_roi_nSPs >= m_minHits ) {
    ATH_MSG_DEBUG("REGTEST / Found " << m_roi_nSPs << " space points.");
    m_countRoIwithEnoughHits++;
  }
  else {
    ATH_MSG_DEBUG("No tracks found - too few hits in ROI to run " << m_roi_nSPs);
    HLT::ErrorCode code = attachFeature(outputTE, new TrackCollection, m_attachedFeatureName);
    if (code != HLT::OK) {
      return code;
    }
    if (m_doTrigInDetTrack) {
      code = attachFeature(outputTE, new TrigInDetTrackCollection, m_attachedFeatureName_TIDT);
      return code;
    }
  }
  
  /*    
	int lCounts[20];	 
	for(int k=0;k<20;k++) lCounts[k]=0;
	for(auto sp : convertedSpacePoints) {
	lCounts[sp.layer()]++;
	}	 
	for(int k=0;k<20;k++) {
	std::cout<<"L #"<<k<<" NSP="<<lCounts[k]<<std::endl;
	}
  */
  
  if (m_retrieveBarCodes) {
    std::vector<int> vBar;
    m_TrigL2SpacePointTruthTool->getBarCodes(convertedSpacePoints,vBar);
    
    //for(auto barCode : vBar) std::cout<<"SP bar code = "<<barCode<<std::endl;
  } 
 
  m_tcs.m_tripletPtMin = m_tripletMinPtFrac*m_pTmin;
  ATH_MSG_VERBOSE("m_tcs.m_tripletPtMin: " << m_tcs.m_tripletPtMin);
  ATH_MSG_VERBOSE("m_pTmin: " << m_pTmin);

  if ( timerSvc() ) m_PatternRecoTimer->start();

  std::map<int, int> nGoodRejected;
  std::map<int, int> nGoodAccepted;
  std::map<int, int> nGoodTotal;
  if(m_retrieveBarCodes) {
    for(auto barCode : m_vSignalBarCodes) {
      nGoodRejected.insert(std::pair<int,int>(barCode,0));
      nGoodAccepted.insert(std::pair<int,int>(barCode,0));
      nGoodTotal.insert(std::pair<int,int>(barCode,0));
    }
  }

  int iSeed=0;


  if ( timerSvc() ) m_TripletMakingTimer->start();

  TRIG_TRACK_SEED_GENERATOR seedGen(m_tcs);
  seedGen.loadSpacePoints(convertedSpacePoints);
  seedGen.createSeeds();
  std::vector<TrigInDetTriplet*> triplets;
  seedGen.getSeeds(triplets);

  ATH_MSG_DEBUG("number of triplets: " << triplets.size());

  if ( timerSvc() ) {
    m_TripletMakingTimer->stop();
    m_TripletMakingTimer->propVal(m_roi_nSPs);
  }

  if ( timerSvc() ) m_CombTrackingTimer->start();

  // 8. Combinatorial tracking

  std::vector<int> vTBarCodes(triplets.size(),-1);

  if(m_retrieveBarCodes) {
    assignTripletBarCodes(triplets, vTBarCodes);
  }

  std::vector<std::tuple<bool, double,Trk::Track*>> qualityTracks; //bool used for later filtering
  qualityTracks.reserve(triplets.size());

  m_nSeeds  = 0;
  iSeed=0;

  long int trackIndex=0;

  if(m_checkSeedRedundancy) m_siClusterMap.clear();

  bool PIX = true;
  bool SCT = true;

  m_trackMaker->newTrigEvent(PIX,SCT);

  for(unsigned int tripletIdx=0;tripletIdx!=triplets.size();tripletIdx++) {

    TrigInDetTriplet* seed = triplets[tripletIdx];

    const Trk::SpacePoint* osp1 = seed->s1().offlineSpacePoint();
    const Trk::SpacePoint* osp2 = seed->s2().offlineSpacePoint();
    const Trk::SpacePoint* osp3 = seed->s3().offlineSpacePoint();

    if(m_checkSeedRedundancy) {
      //check if clusters do not belong to any track
      std::vector<Identifier> clusterIds;
      extractClusterIds(osp1, clusterIds);
      extractClusterIds(osp2, clusterIds);
      extractClusterIds(osp3, clusterIds);
      if(usedByAnyTrack(clusterIds, m_siClusterMap)) {
        continue;
      }
    }

    std::list<const Trk::SpacePoint*> spList = {osp1, osp2, osp3};

    bool trackFound=false;

    ++m_nSeeds;

    const std::list<Trk::Track*>& tracks = m_trackMaker->getTracks(spList);

    for(std::list<Trk::Track*>::const_iterator t=tracks.begin(); t!=tracks.end(); ++t) {
      if((*t)) {
        float d0 = (*t)->perigeeParameters()->parameters()[Trk::d0]; 
        if (fabs(d0) > m_initialD0Max) {
          ATH_MSG_DEBUG("REGTEST / Reject track with d0 = " << d0 << " > " << m_initialD0Max);
          qualityTracks.push_back(std::make_tuple(false,0,(*t)));//Flag track as bad, but keep in vector for later deletion
          continue;
        }
        if(m_checkSeedRedundancy) {
          //update clusterMap 
          updateClusterMap(trackIndex++, (*t), m_siClusterMap);
        }
        qualityTracks.push_back(std::make_tuple(true,-trackQuality((*t)),(*t)));
      }
    }  
    iSeed++;
    ATH_MSG_DEBUG("Found "<<tracks.size()<<" tracks using triplet");
    if(!tracks.empty()) {
      trackFound = true;
    }

    if(m_retrieveBarCodes) {
      bool goodTriplet=false;
      int foundBarCode=-1;

      for(auto barCode : m_vSignalBarCodes) {
        if (vTBarCodes[tripletIdx] == barCode) {
          foundBarCode=barCode;
          goodTriplet=true;break;
        }
      }

      if(goodTriplet) {
        (*nGoodTotal.find(foundBarCode)).second++;
        if(trackFound) (*nGoodAccepted.find(foundBarCode)).second++;
        else (*nGoodRejected.find(foundBarCode)).second++;
      }
    }
  }

  m_trackMaker->endEvent();
  for(auto& seed : triplets) delete seed;

  //clone removal
  filterSharedTracks(qualityTracks);

  TrackCollection* offlineTracks = new TrackCollection;
  offlineTracks->reserve(qualityTracks.size());
  for(const auto& q : qualityTracks) {
    if (std::get<0>(q)==true) {
      offlineTracks->push_back(std::get<2>(q));
    }
    else {
      delete std::get<2>(q);
    }
  }
  qualityTracks.clear();

  m_nTracksNew=offlineTracks->size();
  ATH_MSG_DEBUG("After clone removal "<<m_nTracksNew<<" tracks left");
  

  if ( timerSvc() ) {
    m_CombTrackingTimer->stop();
    m_CombTrackingTimer->propVal(iSeed);
    m_PatternRecoTimer->propVal( offlineTracks->size() );
    m_PatternRecoTimer->stop();
    m_timePattReco = m_PatternRecoTimer->elapsed();
  }



  if (m_retrieveBarCodes) {
    //reco. efficiency analysis
    calculateRecoEfficiency(convertedSpacePoints, nGoodTotal, nGoodAccepted);
  }

  if ( timerSvc() ) m_TrackFitterTimer->start();

  TrackCollection* fittedTracks = m_trigInDetTrackFitter->fit(*offlineTracks, m_particleHypothesis);
  delete offlineTracks;

  if( fittedTracks->empty() ) {
    ATH_MSG_DEBUG("REGTEST / No tracks fitted");
  }

  for (auto fittedTrack = fittedTracks->begin(); fittedTrack!=fittedTracks->end(); ++fittedTrack) {
    (*fittedTrack)->info().setPatternRecognitionInfo(Trk::TrackInfo::FastTrackFinderSeed);
    ATH_MSG_VERBOSE("Updating fitted track: " << **fittedTrack);
    m_trackSummaryTool->updateTrack(**fittedTrack);
    ATH_MSG_VERBOSE("Updated track: " << **fittedTrack);
  }

  if ( timerSvc() ) { 
    m_TrackFitterTimer->propVal(fittedTracks->size() );
    m_TrackFitterTimer->stop();
  }

  if( fittedTracks->empty() ) {
    ATH_MSG_DEBUG("REGTEST / No tracks reconstructed");
  }
  
  //monitor Z-vertexing
  
  m_nZvertices=m_zVertices.size();
  
  //monitor number of tracks
  m_nTracks=fittedTracks->size();	
  ATH_MSG_DEBUG("REGTEST / Found " << m_nTracks << " tracks");
  if( !fittedTracks->empty() )
    m_countRoIwithTracks++;
  
  ///////////// fill vectors of quantities to be monitored
  fillMon(*fittedTracks);
        
  m_currentStage = 4;

  HLT::ErrorCode code = attachFeature(outputTE, fittedTracks, m_attachedFeatureName);
  if ( code != HLT::OK ) {
    ATH_MSG_ERROR("REGTEST/ Write into outputTE failed");
    if (fittedTracks!=nullptr) {
      delete fittedTracks;
    }
    return code;
  }

  //TrigInDetTrack output
  if (m_doTrigInDetTrack) {
    TrigInDetTrackCollection* fittedTracks_TIDT = new TrigInDetTrackCollection;
    fittedTracks_TIDT->reserve(fittedTracks->size());
    convertToTrigInDetTrack(*fittedTracks, *fittedTracks_TIDT);
    code = attachFeature(outputTE, fittedTracks_TIDT, m_attachedFeatureName_TIDT);
    if ( code != HLT::OK ) {
      ATH_MSG_ERROR("REGTEST/ Write into outputTE failed");
      if (fittedTracks!=nullptr) {
        delete fittedTracks;
        delete fittedTracks_TIDT;
      }
      return code;
    }
  }
  
  return HLT::OK;
}

double TrigFastTrackFinder::trackQuality(const Trk::Track* Tr) {
 
  DataVector<const Trk::TrackStateOnSurface>::const_iterator  
    m  = Tr->trackStateOnSurfaces()->begin(), 
    me = Tr->trackStateOnSurfaces()->end  ();
  
  double quality = 0. ;
  const double W       = 17.;

  for(; m!=me; ++m) {
    const Trk::FitQualityOnSurface* fq =  (*m)->fitQualityOnSurface();
    if(!fq) continue;
    
    double x2 = fq->chiSquared();
    double q;
    if(fq->numberDoF() == 2) q = (1.2*(W-x2*.5)); 
    else                     q =      (W-x2    );
    if(q < 0.) q = 0.; quality+=q;
  }
  return quality;
}

void TrigFastTrackFinder::filterSharedTracks(std::vector<std::tuple<bool, double,Trk::Track*>>& QT) {

  std::set<const Trk::PrepRawData*> clusters;
    
  const Trk::PrepRawData* prd[100];
  
  std::sort(QT.begin(), QT.end(),
      [](const std::tuple<bool, double, Trk::Track*>& lhs, const std::tuple<bool, double, Trk::Track*>& rhs) {
        return std::get<1>(lhs) < std::get<1>(rhs); } );

  for (auto& q : QT) {
    DataVector<const Trk::MeasurementBase>::const_iterator 
      m  = std::get<2>(q)->measurementsOnTrack()->begin(), 
      me = std::get<2>(q)->measurementsOnTrack()->end  ();
    
    int nf = 0, nc = 0; 
    for(; m!=me; ++m ) {

      const Trk::PrepRawData* pr = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
      if(pr) {
        ++nc;
        if(clusters.find(pr)==clusters.end()) {prd[nf++]=pr; if(nf==100) break;}
      }
    }
    if((nf >= m_nfreeCut) || (nf == nc) ) {
      for(int n=0; n!=nf; ++n) clusters.insert(prd[n]);
    }
    else  {
      std::get<0>(q) = false;
    }
  }
}

//---------------------------------------------------------------------------

HLT::ErrorCode TrigFastTrackFinder::hltFinalize()
{

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("TrigFastTrackFinder::finalize() - TrigFastTrackFinder Statistics: ");
  ATH_MSG_INFO("RoI processed: " <<  m_countTotalRoI);
  ATH_MSG_INFO("RoI with enough SPs : " <<  m_countRoIwithEnoughHits);
  ATH_MSG_INFO("RoI with Track(s)  Total/goodZvertex/badZvertex: " << m_countRoIwithTracks);
  if (m_retrieveBarCodes) {
    ATH_MSG_INFO("Number of signal tracks present " << m_nSignalPresent);
    ATH_MSG_INFO("Number of signal seeds  found   " << m_nSignalDetected);
    ATH_MSG_INFO("Number of signal tracks found   " << m_nSignalTracked);
    if(m_nSignalPresent!=0) {
      ATH_MSG_INFO("Track seeding  efficiency      " << (100.0*m_nSignalDetected/m_nSignalPresent) <<" % ");
      ATH_MSG_INFO("Track seeding  redundancy      " << (100.0*m_nSignalClones/m_nSignalPresent) << " %");
      ATH_MSG_INFO("Track finding efficiency       " << (100.0*m_nSignalTracked/m_nSignalPresent) << " %");
    }
  }
  ATH_MSG_INFO("=========================================================");

  return HLT::OK;
}

void TrigFastTrackFinder::updateClusterMap(long int trackIdx, const Trk::Track* pTrack, std::map<Identifier, std::vector<long int> >& clusterMap) {
  //loop over clusters

  for(auto tMOT = pTrack->measurementsOnTrack()->begin(); tMOT != pTrack->measurementsOnTrack()->end(); ++tMOT) { 
  
    const InDet::SiClusterOnTrack* siCLOT = dynamic_cast<const InDet::SiClusterOnTrack*>(*tMOT); 
    if (siCLOT==NULL) continue;
    const InDet::SiCluster* siCL = dynamic_cast<const InDet::SiCluster*>(siCLOT->prepRawData()); 
    if (siCL==NULL) continue;
    Identifier id = siCL->identify(); 
    clusterMap[id].push_back(trackIdx);
    //no sorting is needed as the vectors are sorted by the algorithm design
    //due to monotonically increasing trackIdx
    // std::map<Identifier, std::vector<long int> >::iterator itm = clusterMap.find(id);
    //std::sort((*itm).second.begin(),(*itm).second.end());
    //std::copy((*itm).second.begin(),(*itm).second.end(),std::ostream_iterator<long int>(std::cout," "));
    //std::cout<<std::endl;
  }
}

void TrigFastTrackFinder::extractClusterIds(const Trk::SpacePoint* pSP, std::vector<Identifier>& vIds) {
  const InDet::SiCluster* pCL = dynamic_cast<const InDet::SiCluster*>(pSP->clusterList().first);
  if(pCL!=NULL) vIds.push_back(pCL->identify());
  //check second cluster : SCT uv clusters only !
  pCL = dynamic_cast<const InDet::SiCluster*>(pSP->clusterList().second);
  if(pCL!=NULL) vIds.push_back(pCL->identify());
}

bool TrigFastTrackFinder::usedByAnyTrack(const std::vector<Identifier>& vIds, std::map<Identifier, std::vector<long int> >& clusterMap) {

  std::vector<long int> xSection;
  //initializing
  std::map<Identifier, std::vector<long int> >::iterator itm0 = clusterMap.find(*vIds.begin());
  if(itm0 == clusterMap.end()) return false;
  xSection.reserve((*itm0).second.size());
  std::copy((*itm0).second.begin(), (*itm0).second.end(), std::back_inserter(xSection));
  std::vector<Identifier>::const_iterator it = vIds.begin();it++;
  for(;it!=vIds.end();++it) {
    std::map<Identifier, std::vector<long int> >::iterator itm1 = clusterMap.find(*it);
    if(itm1 == clusterMap.end()) return false;
    std::vector<long int> tmp;
    std::set_intersection(xSection.begin(), xSection.end(), (*itm1).second.begin(),(*itm1).second.end(), std::back_inserter(tmp));
    if(tmp.empty()) return false;
    //update xSection
    xSection.clear();
    xSection.reserve(tmp.size());
    std::copy(tmp.begin(), tmp.end(), std::back_inserter(xSection));
  }
  return !xSection.empty();
}

int TrigFastTrackFinder::findBarCodeInData(int barCode, const std::vector<TrigSiSpacePointBase>& vSP) {
  int nFound=0;
  std::set<int> layerSet;
  for(auto sp : vSP) {
    if(barCode==sp.barCode()) {
      nFound++;
      layerSet.insert(sp.layer());
    }
  }
  if(int(layerSet.size())<m_minSignalSPs) {//less than N unique layers
    nFound=0;
  }
  return nFound;
}

void TrigFastTrackFinder::showBarCodeInData(int barCode, const std::vector<TrigSiSpacePointBase>& vSP) {
  for(auto sp : vSP) {
    if(barCode==sp.barCode()) {
      ATH_MSG_DEBUG("L="<<sp.layer()<<" r="<<sp.r()<<" z="<<sp.z());
    }
  }
}

int TrigFastTrackFinder::findBarCodeInTriplets(int barCode, const std::vector<std::shared_ptr<TrigInDetTriplet>>& vTR) {
  int nFound=0;
  for(auto tr : vTR) {
    bool found = (barCode == tr->s1().barCode()) && (barCode == tr->s2().barCode()) && (barCode == tr->s3().barCode());
    if(found) {
      nFound++;
    }
  }
  return nFound;
}

void TrigFastTrackFinder::assignTripletBarCodes(const std::vector<std::shared_ptr<TrigInDetTriplet>>& vTR, std::vector<int>& vBar) {
  int iTR=0;
  for(auto tr : vTR) {
    bool good = (tr->s1().barCode() == tr->s2().barCode()) && (tr->s3().barCode() == tr->s2().barCode());
    good = good && (tr->s1().barCode() > 0);
    if(good) {
      vBar[iTR] = tr->s1().barCode();
    }
    iTR++;
  }
}

void TrigFastTrackFinder::assignTripletBarCodes(const std::vector<TrigInDetTriplet*>& vTR, std::vector<int>& vBar) {
  int iTR=0;
  for(auto tr : vTR) {
    bool good = (tr->s1().barCode() == tr->s2().barCode()) && (tr->s3().barCode() == tr->s2().barCode());
    good = good && (tr->s1().barCode() > 0);
    if(good) {
      vBar[iTR] = tr->s1().barCode();
    }
    iTR++;
  }
}

void TrigFastTrackFinder::getBeamSpot() {
  m_vertex = m_iBeamCondSvc->beamPos();
  ATH_MSG_VERBOSE("Beam spot position " << m_vertex);
  double xVTX = m_vertex.x();
  double yVTX = m_vertex.y();
  double zVTX = m_vertex.z();
  double tiltXZ = m_iBeamCondSvc->beamTilt(0);
  double tiltYZ = m_iBeamCondSvc->beamTilt(1);
  m_shift_x = xVTX - tiltXZ*zVTX;//correction for tilt
  m_shift_y = yVTX - tiltYZ*zVTX;//correction for tilt	
  ATH_MSG_VERBOSE("Beam center position:  " << m_shift_x <<"  "<< m_shift_y);
}

void TrigFastTrackFinder::getMagField() {
  Amg::Vector3D bField;
  m_MagFieldSvc->getField(&m_vertex, &bField);
  bField*=1000.0;//Convert to Tesla
  m_tcs.m_magFieldZ = bField.z();//configured value
  ATH_MSG_VERBOSE("bField.x(): " << bField.x());
  ATH_MSG_VERBOSE("bField.y(): " << bField.y());
  ATH_MSG_VERBOSE("bField.z(): " << bField.z());
}


HLT::ErrorCode TrigFastTrackFinder::getRoI(const HLT::TriggerElement* outputTE, const IRoiDescriptor*& roi)
{
  
  const TrigRoiDescriptor* externalRoI = nullptr;
  HLT::ErrorCode ec = getFeature(outputTE, externalRoI);
  if(ec != HLT::OK) {
    ATH_MSG_ERROR("REGTEST / Failed to find RoiDescriptor");
    return HLT::NAV_ERROR;
  }

  if(externalRoI==nullptr) {
    ATH_MSG_ERROR("REGTEST / null RoiDescriptor");
    return HLT::NAV_ERROR;
  }

  roi = externalRoI;
  m_roiEta = roi->eta();
  m_roiEtaWidth = roi->etaPlus() - roi->etaMinus();
  m_roiPhi = roi->phi();
  m_roiPhiWidth = HLT::wrapPhi(roi->phiPlus() - roi->phiMinus());
  ATH_MSG_DEBUG("REGTEST / RoI" << *roi);

  return HLT::OK;
}

void TrigFastTrackFinder::clearMembers() {
  m_nTracks = 0;
  m_zVertices.clear();

  m_trk_pt.clear();
  m_trk_a0.clear();
  m_trk_z0.clear();
  m_trk_phi0.clear();
  m_trk_eta.clear();
  m_trk_chi2dof.clear();
  m_trk_nSiHits.clear();
  m_trk_nPIXHits.clear();
  m_trk_nSCTHits.clear();
  m_trk_a0beam.clear();
  m_trk_dPhi0.clear();
  m_trk_dEta.clear();

  //m_sp_x.clear();
  //m_sp_y.clear();
  //m_sp_z.clear();
  //m_sp_r.clear();

 

  m_nPixSPsInRoI=0;
  m_nSCTSPsInRoI=0;
  m_currentStage=0;
  m_roi_nSPs=0;
  m_nZvertices=0;
}

void TrigFastTrackFinder::calculateRecoEfficiency(const std::vector<TrigSiSpacePointBase>& convertedSpacePoints,
						  const std::map<int,int>& nGoodTotal, 
						  const std::map<int,int>& nGoodAccepted) {

  //reco. efficiency analysis
  for(auto barCode : m_vSignalBarCodes) {
    int nSignalSPs = findBarCodeInData(barCode, convertedSpacePoints);
    if(nSignalSPs<m_minSignalSPs) continue;
    m_nSignalPresent+=1;
    int nSignalTracks = (*nGoodTotal.find(barCode)).second;
    if(nSignalTracks==0) {
      continue;
    }
    m_nSignalDetected+=1;
    m_nSignalClones+=nSignalTracks;
    
    int nGoodTripletsAccepted = (*nGoodAccepted.find(barCode)).second;
    if(nGoodTripletsAccepted==0) continue;
    m_nSignalTracked+=1;
  } 
}

void TrigFastTrackFinder::fillMon(const TrackCollection& tracks) {
  size_t size = tracks.size();
  m_trk_pt.reserve(size);
  m_trk_a0.reserve(size);
  m_trk_z0.reserve(size);
  m_trk_phi0.reserve(size);
  m_trk_eta.reserve(size);
  m_trk_chi2dof.reserve(size);
  m_trk_nSiHits.reserve(size);
  m_trk_nPIXHits.reserve(size);
  m_trk_nSCTHits.reserve(size);
  m_trk_a0beam.reserve(size);
  m_trk_dPhi0.reserve(size);
  m_trk_dEta.reserve(size);
  for (auto track : tracks) {
    const Trk::TrackParameters* trackPars = track->perigeeParameters();
    if(trackPars==nullptr) {
      continue;
    }

    if(trackPars->covariance()==nullptr) {
      continue;
    }

    float a0 = trackPars->parameters()[Trk::d0]; 
    m_trk_a0.push_back(a0);
    float z0 = trackPars->parameters()[Trk::z0]; 
    m_trk_z0.push_back(z0);
    float phi0 = trackPars->parameters()[Trk::phi0]; 
    m_trk_phi0.push_back(phi0);
    m_trk_a0beam.push_back(a0+m_shift_x*sin(phi0)-m_shift_y*cos(phi0));
    float dPhi0 = HLT::wrapPhi(phi0 - m_roiPhi);
    m_trk_dPhi0.push_back(dPhi0);
    float theta = trackPars->parameters()[Trk::theta]; 
    float eta = -log(tan(0.5*theta)); 
    m_trk_eta.push_back(eta);
    m_trk_dEta.push_back(eta - m_roiEta);

    float qOverP = trackPars->parameters()[Trk::qOverP]; 
    float pT=sin(theta)/qOverP;
    m_trk_pt.push_back(pT);

    const Trk::FitQuality* fq = track->fitQuality();
    float chi2 = 1e8;
    if (fq) {
      ATH_MSG_VERBOSE("Fitted chi2: " << fq->chiSquared());
      ATH_MSG_VERBOSE("Fitted ndof: " << fq->numberDoF());
      if(fq->numberDoF()!=0) {
        chi2 = fq->chiSquared()/fq->numberDoF();
      }
    }
    m_trk_chi2dof.push_back(chi2);
    int nPix=0, nSct=0;

    for(auto tSOS = track->trackStateOnSurfaces()->begin();  
        tSOS!=track->trackStateOnSurfaces()->end(); ++tSOS) { 
      if ((*tSOS)->type(Trk::TrackStateOnSurface::Perigee) == false) {
        const Trk::FitQualityOnSurface* fq =  (*tSOS)->fitQualityOnSurface(); 
        if(!fq) continue; 
        int nd = fq->numberDoF(); 
        if(nd==2) nPix++;
        if(nd==1) nSct++;
      }
    }
    m_trk_nPIXHits.push_back(nPix); 
    m_trk_nSCTHits.push_back(nSct/2); 
    m_trk_nSiHits.push_back(nPix + nSct/2); 

    ATH_MSG_DEBUG("REGTEST / track npix/nsct/phi0/pt/eta/d0/z0/chi2: " <<
        nPix   << " / "  << 
        nSct/2 << " / "  << 
        phi0   << " / "  << 
        pT     << " / " << 
        eta    << " / " << 
        a0     << " / " <<
        z0     << " / " <<
        chi2);
  }
}

void TrigFastTrackFinder::convertToTrigInDetTrack(const TrackCollection& offlineTracks, TrigInDetTrackCollection& trigInDetTracks) {

  trigInDetTracks.reserve(offlineTracks.size());
  for (auto offlineTrack : offlineTracks) {
    const Trk::TrackParameters* trackPars = offlineTrack->perigeeParameters();
    if(trackPars==nullptr) {
      continue;
    }

    if(trackPars->covariance()==nullptr) {
      continue;
    }
    
    float d0 = trackPars->parameters()[Trk::d0]; 
    float z0 = trackPars->parameters()[Trk::z0]; 
    float phi0 = trackPars->parameters()[Trk::phi0]; 
    float theta = trackPars->parameters()[Trk::theta]; 
    float eta = -log(tan(0.5*theta)); 
    
    float qOverP = trackPars->parameters()[Trk::qOverP]; 
    float pT=sin(theta)/qOverP;

    //Calculate	covariance matrix in TID track parameter convention
    const AmgSymMatrix(5) cov_off = *(trackPars->covariance());
    float A = -0.5*((1.0+tan(0.5*theta)*tan(0.5*theta))/(tan(0.5*theta))); //deta_by_dtheta
    float B = cos(theta)/qOverP; //dpT_by_dtheta
    float C = -sin(theta)/(qOverP*qOverP); //dpT_by_dqOverP

    //std::vector<double>* cov = new std::vector<double>(15, 0);
    std::vector<double>* cov = new std::vector<double>
      {cov_off(0,0), cov_off(2,0), cov_off(1,0), A*cov_off(3,0), B*cov_off(3,0) + C*cov_off(4,0),
       cov_off(2,2), cov_off(2,1), A*cov_off(3,2), B*cov_off(3,2) + C*cov_off(4,2),
       cov_off(1,1), A*cov_off(3,1), B*cov_off(3,1) + C*cov_off(4,1),
       A*A*cov_off(3,3), A*(B*cov_off(3,3) + C*cov_off(4,3)),
       B*(B*cov_off(3,3) + 2*C*cov_off(4,3)) + C*C*cov_off(4,4)};

    if(msgLvl() <= MSG::VERBOSE) {
      ATH_MSG_DEBUG(cov_off);
      for (unsigned int i = 0; i < cov->size(); ++i) {
	msg() << MSG::DEBUG << std::fixed << std::setprecision(10) << "cov_TrigInDetTrack[" << i << "]: " << cov->at(i) << endreq; 
      }
    }

    float ed0   = sqrt(cov->at(0));
    float	ephi0 = sqrt(cov->at(5));
    float	ez0   = sqrt(cov->at(9));
    float	eeta  = sqrt(cov->at(12));
    float	epT   = sqrt(cov->at(14));
    
    //const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(d0,phi0,z0,eta,pT,nullptr); 
    const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(d0, phi0, z0, eta, pT, ed0, ephi0, ez0, eeta, epT,cov);
    std::vector<const TrigSiSpacePoint*>* pvsp = new std::vector<const TrigSiSpacePoint*>;
    TrigInDetTrack* pTrack = new TrigInDetTrack(pvsp,tidtfp);

    //calculate chi2 and ndofs

    const Trk::FitQuality* fq = offlineTrack->fitQuality();
    if (fq) {
      ATH_MSG_VERBOSE("Fitted chi2: " << fq->chiSquared());
      ATH_MSG_VERBOSE("Fitted ndof: " << fq->numberDoF());
      if(fq->numberDoF()!=0) {
	pTrack->chi2(fq->chiSquared()/fq->numberDoF());
      }
      else pTrack->chi2(1e8);
    }
    else {
      pTrack->chi2(1e8);
    }
    
    int nPix=0, nSct=0;

    for(auto tSOS = offlineTrack->trackStateOnSurfaces()->begin();  
	tSOS!=offlineTrack->trackStateOnSurfaces()->end(); ++tSOS) { 
      if ((*tSOS)->type(Trk::TrackStateOnSurface::Perigee) == false) {
	const Trk::FitQualityOnSurface* fq =  (*tSOS)->fitQualityOnSurface(); 
	if(!fq) continue; 
	int nd = fq->numberDoF(); 
	if(nd==2) nPix++;
	if(nd==1) nSct++;
      }
    }
    pTrack->NPixelSpacePoints(nPix); 
    pTrack->NSCT_SpacePoints(nSct/2); 

    long hitPattern=0x0;
    for (auto tMOT = offlineTrack->measurementsOnTrack()->begin();
	 tMOT != offlineTrack->measurementsOnTrack()->end(); ++tMOT) {
      Identifier id = (*tMOT)->associatedSurface().associatedDetectorElement()->identify();
      IdentifierHash hash = (*tMOT)->associatedSurface().associatedDetectorElement()->identifyHash();

      if(m_idHelper->is_sct(id)) {
	Identifier wafer_id = m_sctId->wafer_id(hash);
	int layId = m_sctId->layer_disk(wafer_id);
	long layer=0;
	if (m_sctId->is_barrel(wafer_id)){
	  layer = layId+m_tcs.m_maxBarrelPix;
	} else {
	  layer = layId+m_tcs.m_maxEndcapPix;
	}
	long mask = 1 << layer;
	hitPattern |= mask;
      }
      else if(m_idHelper->is_pixel(id)) {
	Identifier wafer_id = m_pixelId->wafer_id(hash);
	int layId = m_pixelId->layer_disk(wafer_id);
	long layer=0;
	if (m_pixelId->is_barrel(wafer_id)){
	  layer = layId;
	} else {
	  layer = layId+m_tcs.m_minEndcapPix;
	}
	long mask = 1 << layer;
	hitPattern |= mask;
      }
      else {
	ATH_MSG_WARNING("cannot determine detector type, hash="<<hash);
      }
    }
    pTrack->HitPattern(hitPattern);
	
    pTrack->algorithmId(TrigInDetTrack::FTF);
    trigInDetTracks.push_back(pTrack);
  }
}
