/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include <memory>

#include "TrigFastTrackFinder.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"

#include "CxxUtils/phihelper.h"

#include "AthenaMonitoringKernel/Monitored.h"


#include "PathResolver/PathResolver.h"

//for UTT
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"

TrigFastTrackFinder::TrigFastTrackFinder(const std::string& name, ISvcLocator* pSvcLocator) :

  AthReentrantAlgorithm(name, pSvcLocator),
  m_numberingTool("TrigL2LayerNumberTool"),
  m_spacePointTool("TrigSpacePointConversionTool"),
  m_trackMaker("InDet::SiTrackMaker_xk/InDetTrigSiTrackMaker"),
  m_trigInDetTrackFitter("TrigInDetTrackFitter"),
  m_trigZFinder("TrigZFinder/TrigZFinder", this ),
  m_trackSummaryTool("Trk::ITrackSummaryTool/ITrackSummaryTool"),
  m_accelTool("TrigInDetAccelerationTool"),
  m_accelSvc("TrigInDetAccelerationSvc", name),
  m_doCloneRemoval(true),
  m_useBeamSpot(true),
  m_doZFinder(false),
  m_doZFinderOnly(false),
  m_storeZFinderVertices(false),
  m_nfreeCut(5),
  m_countTotalRoI(0),
  m_countRoIwithEnoughHits(0),
  m_countRoIwithTracks(0),
  m_pixelId(0),
  m_sctId(0),
  m_idHelper(0),
  m_particleHypothesis(Trk::pion),
  m_useNewLayerNumberScheme(false),
  m_useGPU(false),
  m_LRTmode(false),
  m_LRTD0Min(0.0),
  m_LRTHardMinPt(0.0),
  m_trigseedML_LUT(""),
  m_doHitDV(false),
  m_doHitDV_Seeding(false),
  m_dodEdxTrk(false)
{

  /** Doublet finding properties. */
  declareProperty("Doublet_FilterRZ",          m_tcs.m_doubletFilterRZ = true);
  declareProperty("DoubletDR_Max",             m_tcs.m_doublet_dR_Max = 270.0);
  declareProperty("DoubletDR_Max_Confirm",     m_tcs.m_doublet_dR_Max_Confirm = 150.0);
  declareProperty("SeedRadBinWidth",           m_tcs.m_seedRadBinWidth = 2.0);

  /** Triplet finding properties. */

  declareProperty("Triplet_D0Max",            m_tcs.m_tripletD0Max      = 4.0);
  declareProperty("Triplet_D0_PPS_Max",       m_tcs.m_tripletD0_PPS_Max = 1.7);
  declareProperty("Triplet_nMaxPhiSlice",     m_tcs.m_nMaxPhiSlice = 53);
  declareProperty("Triplet_MaxBufferLength",     m_tcs.m_maxTripletBufferLength = 3);
  declareProperty("TripletDoPSS",            m_tcs.m_tripletDoPSS = false);
  declareProperty("TripletDoPPS",            m_tcs.m_tripletDoPPS = true);
  declareProperty("TripletDoConfirm",        m_tcs.m_tripletDoConfirm = false);
  declareProperty("TripletMaxCurvatureDiff",    m_tcs.m_curv_delta = 0.001);//for the triplet confirmation
  declareProperty("Triplet_DtCut",            m_tcs.m_tripletDtCut      = 10.0);//i.e. 10*sigma_MS

  /** settings for the ML-enhanced track seeding */

  declareProperty("UseTrigSeedML",              m_tcs.m_useTrigSeedML = 0);
  declareProperty("TrigSeedML_LUT",             m_trigseedML_LUT = "trigseed_ml_pixel_barrel_kde.lut");
  declareProperty("maxEC_Pixel_cluster_length", m_tcs.m_maxEC_len = 1.5);

  declareProperty( "VertexSeededMode",    m_vertexSeededMode = false);
  declareProperty( "doZFinder",           m_doZFinder = true);
  declareProperty( "doZFinderOnly",       m_doZFinderOnly = false);

  declareProperty( "doFastZVertexSeeding",        m_doFastZVseeding = true);
  declareProperty( "zVertexResolution",           m_tcs.m_zvError = 10.0);
  declareProperty( "StoreZFinderVertices",        m_storeZFinderVertices = false );


  declareProperty("Triplet_MinPtFrac",        m_tripletMinPtFrac = 0.3);
  declareProperty("pTmin",                    m_pTmin = 1000.0);
  declareProperty("TrackInitialD0Max",            m_initialD0Max      = 10.0);
  declareProperty("TrackZ0Max",                   m_Z0Max      = 300.0);

  declareProperty("doSeedRedundancyCheck",            m_checkSeedRedundancy = false);

  declareProperty( "MinHits",               m_minHits = 5 );

  declareProperty("TracksName",
                  m_outputTracksKey = std::string("TrigFastTrackFinder_Tracks"),
                  "TrackCollection name");

  declareProperty("inputTracksName",
                  m_inputTracksKey = std::string(""),
                  "TrackCollection name");

  declareProperty("RoIs", m_roiCollectionKey = std::string("OutputRoIs"), "RoIs to read in");

  declareProperty( "UseBeamSpot",           m_useBeamSpot = true);
  declareProperty( "FreeClustersCut"   ,m_nfreeCut      );
  declareProperty( "SpacePointProviderTool", m_spacePointTool  );
  declareProperty( "LayerNumberTool", m_numberingTool  );

  declareProperty( "initialTrackMaker", m_trackMaker);
  declareProperty( "trigInDetTrackFitter",   m_trigInDetTrackFitter );
  declareProperty( "trigZFinder",   m_trigZFinder );

  declareProperty("TrackSummaryTool", m_trackSummaryTool);
  declareProperty( "doResMon",       m_doResMonitoring = true);

  declareProperty("doCloneRemoval", m_doCloneRemoval = true);


  declareProperty("useNewLayerNumberScheme", m_useNewLayerNumberScheme = false);

  declareProperty("useGPU", m_useGPU = false);

  declareProperty("LRT_Mode", m_LRTmode);
  declareProperty("LRT_D0Min", m_LRTD0Min=0.0);
  declareProperty("LRT_HardMinPt", m_LRTHardMinPt=0.0);

  // UTT
  declareProperty("doHitDV",         m_doHitDV         = false);
  declareProperty("doHitDV_Seeding", m_doHitDV_Seeding = false);
  declareProperty("dodEdxTrk",       m_dodEdxTrk       = false);

}

//--------------------------------------------------------------------------

TrigFastTrackFinder::~TrigFastTrackFinder() {}

//-----------------------------------------------------------------------

StatusCode TrigFastTrackFinder::initialize() {

  ATH_CHECK(m_roiCollectionKey.initialize() );
  ATH_CHECK(m_outputTracksKey.initialize() );

  // optional input tracks collection if present the clusters on previously found tracks are not used to form seeds
  if (m_LRTmode) {
    ATH_CHECK(m_inputTracksKey.initialize( !m_inputTracksKey.key().empty() ) );
  }
  // optional PRD to track association map
  ATH_CHECK(m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty() ));


  ATH_CHECK(m_beamSpotKey.initialize());



  ATH_MSG_DEBUG(" TrigFastTrackFinder : MinHits set to " << m_minHits);

  ATH_CHECK(m_numberingTool.retrieve());

  ATH_CHECK(m_spacePointTool.retrieve());

  ATH_CHECK(m_trackMaker.retrieve());

  ATH_CHECK(m_trigInDetTrackFitter.retrieve());

  if (m_doZFinder) {
    ATH_CHECK(m_trigZFinder.retrieve());
  }
  else {
    m_trigZFinder.disable();
  }

  ATH_CHECK(m_trackSummaryTool.retrieve());

  //Get ID helper
  ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));

  ATH_CHECK(detStore()->retrieve(m_pixelId, "PixelID"));

  ATH_CHECK(detStore()->retrieve(m_sctId, "SCT_ID"));

  // monitoring
  if ( !m_monTool.empty() ) {
     ATH_CHECK(m_monTool.retrieve() );
  }
  else {
     ATH_MSG_INFO("Monitoring tool is empty");
  }

  ATH_MSG_DEBUG(" doResMon " << m_doResMonitoring);

  if(m_useGPU) {//for GPU acceleration
    ATH_CHECK(m_accelSvc.retrieve());
    ATH_CHECK(m_accelSvc->isReady());
    ATH_CHECK(m_accelTool.retrieve());
  }

  ATH_MSG_INFO("Use GPU acceleration : "<<std::boolalpha<<m_useGPU);

  if (m_LRTmode) {
    ATH_MSG_INFO(" FTF configures in Large Radius Tracking Mode");
    // set TrigTrackSeedGenerator to LRTmode
    m_tcs.m_LRTmode=m_LRTmode;

  }

  if(m_tcs.m_useTrigSeedML > 0) {
    //LUT params
    int lut_w = 30;
    int lut_h = 45;
    float lut_range[4] = {0.0,3.0,0.0,9.0};
    TrigSeedML_LUT L(1,lut_w,lut_h,lut_range);
    //read data from LUT file
    std::string lut_fileName = PathResolver::find_file(m_trigseedML_LUT, "DATAPATH");
    if (lut_fileName.empty()) {
      ATH_MSG_ERROR("Cannot find TrigSeedML LUT file " << lut_fileName);
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_INFO(lut_fileName);
      std::ifstream ifs(lut_fileName.c_str());
      int row, col0, col1;
      while(!ifs.eof()) {
        ifs >> row >> col0 >> col1;
        if(ifs.eof()) break;
        for(int c=col0;c<=col1;c++) L.setBin(row, c);
      }
      ifs.close();
      ATH_MSG_INFO("TrigSeedML LUT initialized from file " << m_trigseedML_LUT);
      m_tcs.m_vLUT.push_back(L);
    }
  }

  // UTT read/write handles
  if( m_doHitDV ) {
    ATH_CHECK(m_recJetRoiCollectionKey.initialize());
    ATH_CHECK(m_hitDVTrkKey.initialize());
    ATH_CHECK(m_hitDVSPKey.initialize());
  }
  if( m_dodEdxTrk ) {
     ATH_CHECK(m_dEdxTrkKey.initialize());
     ATH_CHECK(m_dEdxHitKey.initialize());
  }


  ATH_MSG_DEBUG("FTF : " << name()  );
  ATH_MSG_DEBUG("	m_tcs.m_doubletFilterRZ    : " <<  m_tcs.m_doubletFilterRZ     );
  ATH_MSG_DEBUG("	m_tcs.m_doublet_dR_Max     : " <<  m_tcs.m_doublet_dR_Max      );
  ATH_MSG_DEBUG("	m_tcs.m_doublet_dR_Max_Confirm  : " << m_tcs.m_doublet_dR_Max_Confirm   );
  ATH_MSG_DEBUG("	m_tcs.m_seedRadBinWidth    : " <<  m_tcs.m_seedRadBinWidth     );
  ATH_MSG_DEBUG("	m_tcs.m_tripletD0Max       : " <<  m_tcs.m_tripletD0Max        );
  ATH_MSG_DEBUG("	m_tcs.m_tripletD0_PPS_Max  : " <<  m_tcs.m_tripletD0_PPS_Max   );
  ATH_MSG_DEBUG("	m_tcs.m_nMaxPhiSlice       : " <<  m_tcs.m_nMaxPhiSlice        );
  ATH_MSG_DEBUG("	m_tcs.m_maxTripletBufferLength  : " << m_tcs.m_maxTripletBufferLength   );
  ATH_MSG_DEBUG("	m_tcs.m_tripletDoPSS       : " <<  m_tcs.m_tripletDoPSS        );
  ATH_MSG_DEBUG("	m_tcs.m_tripletDoPPS       : " <<  m_tcs.m_tripletDoPPS        );
  ATH_MSG_DEBUG("	m_tcs.m_tripletDoConfirm   : " <<  m_tcs.m_tripletDoConfirm    );
  ATH_MSG_DEBUG("	m_tcs.m_curv_delta         : " <<  m_tcs.m_curv_delta          );
  ATH_MSG_DEBUG("	m_tcs.m_tripletDtCut       : " <<  m_tcs.m_tripletDtCut        );
  ATH_MSG_DEBUG("	m_tcs.m_useTrigSeedML      : " <<  m_tcs.m_useTrigSeedML       );
  ATH_MSG_DEBUG("	m_trigseedML_LUT           : " <<  m_trigseedML_LUT     );
  ATH_MSG_DEBUG("	m_tcs.m_maxEC_len          : " <<  m_tcs.m_maxEC_len    );
  ATH_MSG_DEBUG("	m_vertexSeededMode         : " <<  m_vertexSeededMode   );
  ATH_MSG_DEBUG("	m_doZFinder                : " <<  m_doZFinder          );
  ATH_MSG_DEBUG("	m_doZFinderOnly            : " <<  m_doZFinderOnly      );
  ATH_MSG_DEBUG("	m_doFastZVseeding          : " <<  m_doFastZVseeding    );
  ATH_MSG_DEBUG("	m_tcs.m_zvError            : " <<  m_tcs.m_zvError      );
  ATH_MSG_DEBUG("	m_storeZFinderVertices     : " <<  m_storeZFinderVertices   );
  ATH_MSG_DEBUG("	m_tripletMinPtFrac         : " <<  m_tripletMinPtFrac   );
  ATH_MSG_DEBUG("	m_pTmin                    : " <<  m_pTmin              );
  ATH_MSG_DEBUG("	m_initialD0Max             : " <<  m_initialD0Max       );
  ATH_MSG_DEBUG("	m_Z0Max                    : " <<  m_Z0Max              );
  ATH_MSG_DEBUG("	m_checkSeedRedundancy      : " <<  m_checkSeedRedundancy   );
  ATH_MSG_DEBUG("	m_minHits                  : " <<  m_minHits            );
  ATH_MSG_DEBUG("	"  );
  ATH_MSG_DEBUG("	m_useBeamSpot              : " <<  m_useBeamSpot       );
  ATH_MSG_DEBUG("	m_nfreeCut                 : " <<  m_nfreeCut          );
  ATH_MSG_DEBUG("	m_spacePointTool           : " <<  m_spacePointTool    );
  ATH_MSG_DEBUG("	m_numberingTool            : " <<  m_numberingTool     );
  ATH_MSG_DEBUG("	m_trackMaker               : " <<  m_trackMaker        );
  ATH_MSG_DEBUG("	m_trigInDetTrackFitter     : " <<  m_trigInDetTrackFitter   );
  ATH_MSG_DEBUG("	m_trigZFinder              : " <<  m_trigZFinder       );
  ATH_MSG_DEBUG("	m_trackSummaryTool         : " <<  m_trackSummaryTool  );
  ATH_MSG_DEBUG("	m_doResMonitoring          : " <<  m_doResMonitoring   );
  ATH_MSG_DEBUG("	m_doCloneRemoval           : " <<  m_doCloneRemoval    );
  ATH_MSG_DEBUG("	m_useNewLayerNumberScheme  : " <<  m_useNewLayerNumberScheme   );
  ATH_MSG_DEBUG("	m_useGPU                   : " <<  m_useGPU            );
  ATH_MSG_DEBUG("	m_LRTmode                  : " <<  m_LRTmode           );
  ATH_MSG_DEBUG("	m_doHitDV                  : " <<  m_doHitDV      );
  ATH_MSG_DEBUG("	m_dodEdxTrk                : " <<  m_dodEdxTrk         );

  ATH_MSG_DEBUG(" Initialized successfully");


  return StatusCode::SUCCESS;
}


//-------------------------------------------------------------------------

StatusCode TrigFastTrackFinder::start()
{
  //getting magic numbers from the layer numbering tool

  m_tcs.m_maxBarrelPix    = m_numberingTool->offsetBarrelSCT();
  m_tcs.m_minEndcapPix    = m_numberingTool->offsetEndcapPixels();
  m_tcs.m_maxEndcapPix    = m_numberingTool->offsetEndcapSCT();
  m_tcs.m_maxSiliconLayer = m_numberingTool->maxSiliconLayerNum();
  m_tcs.m_layerGeometry.clear();

  if(m_useNewLayerNumberScheme) {
    const std::vector<TRIG_INDET_SI_LAYER>* pVL = m_numberingTool->layerGeometry();
    std::copy(pVL->begin(),pVL->end(),std::back_inserter(m_tcs.m_layerGeometry));
  }

  m_tcs.m_tripletPtMin = m_tripletMinPtFrac*m_pTmin;

  return StatusCode::SUCCESS;
}


StatusCode TrigFastTrackFinder::execute(const EventContext& ctx) const {

  //RoI preparation/update

  SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection(m_roiCollectionKey, ctx);

  ATH_CHECK(roiCollection.isValid());

  TrigRoiDescriptor internalRoI;

  if ( roiCollection->size()>1 ) ATH_MSG_WARNING( "More than one Roi in the collection: " << m_roiCollectionKey << ", this is not supported - use a composite Roi" );
  if ( roiCollection->size()>0 ) internalRoI = **roiCollection->begin();

  //  internalRoI.manageConstituents(false);//Don't try to delete RoIs at the end
  m_countTotalRoI++;

  SG::WriteHandle<TrackCollection> outputTracks(m_outputTracksKey, ctx);
  outputTracks = std::make_unique<TrackCollection>();

  const TrackCollection* inputTracks = nullptr;
  if (m_LRTmode) {
    if (!m_inputTracksKey.key().empty()){
      SG::ReadHandle<TrackCollection> inputTrackHandle(m_inputTracksKey, ctx);
      ATH_CHECK(inputTrackHandle.isValid());
      inputTracks = inputTrackHandle.ptr();
    }
  }
  InDet::ExtendedSiTrackMakerEventData_xk trackEventData(m_prdToTrackMap, ctx);
  ATH_CHECK(findTracks(trackEventData, internalRoI, inputTracks, *outputTracks, ctx));

  return StatusCode::SUCCESS;
}



StatusCode TrigFastTrackFinder::findTracks(InDet::SiTrackMakerEventData_xk &trackEventData,
                                           const TrigRoiDescriptor& roi,
                                           const TrackCollection* inputTracks,
                                           TrackCollection& outputTracks,
                                           const EventContext& ctx) const {

  ATH_MSG_DEBUG("Input RoI " << roi);

  auto mnt_roi_nTracks = Monitored::Scalar<int>("roi_nTracks", 0);
  auto mnt_roi_nSPs    = Monitored::Scalar<int>("roi_nSPs",    0);
  auto mnt_roi_nSPsPIX = Monitored::Scalar<int>("roi_nSPsPIX", 0);
  auto mnt_roi_nSPsSCT = Monitored::Scalar<int>("roi_nSPsSCT", 0);
  auto monSP = Monitored::Group(m_monTool, mnt_roi_nSPsPIX, mnt_roi_nSPsSCT);

  auto mnt_timer_Total                 = Monitored::Timer<std::chrono::milliseconds>("TIME_Total");
  auto mnt_timer_SpacePointConversion  = Monitored::Timer<std::chrono::milliseconds>("TIME_SpacePointConversion");
  auto mnt_timer_PatternReco           = Monitored::Timer<std::chrono::milliseconds>("TIME_PattReco");
  auto mnt_timer_TripletMaking         = Monitored::Timer<std::chrono::milliseconds>("TIME_Triplets");
  auto mnt_timer_CombTracking          = Monitored::Timer<std::chrono::milliseconds>("TIME_CmbTrack");
  auto mnt_timer_TrackFitter           = Monitored::Timer<std::chrono::milliseconds>("TIME_TrackFitter");
  auto mnt_timer_HitDV                 = Monitored::Timer<std::chrono::milliseconds>("TIME_HitDV");
  auto mnt_timer_dEdxTrk               = Monitored::Timer<std::chrono::milliseconds>("TIME_dEdxTrk");
  auto monTime = Monitored::Group(m_monTool, mnt_roi_nTracks, mnt_roi_nSPs, mnt_timer_Total, mnt_timer_SpacePointConversion,
				  mnt_timer_PatternReco, mnt_timer_TripletMaking, mnt_timer_CombTracking, mnt_timer_TrackFitter,
				  mnt_timer_HitDV, mnt_timer_dEdxTrk);

  auto mnt_roi_lastStageExecuted = Monitored::Scalar<int>("roi_lastStageExecuted", 0);
  auto monDataError              = Monitored::Group(m_monTool, mnt_roi_lastStageExecuted);

  mnt_timer_Total.start();
  mnt_timer_SpacePointConversion.start();


  mnt_roi_lastStageExecuted = 1;

  std::vector<TrigSiSpacePointBase> convertedSpacePoints;
  convertedSpacePoints.reserve(5000);

  std::map<Identifier, std::vector<long int> > siClusterMap;

  if (m_LRTmode) {
    // In LRT mode read the input track collection and enter the clusters on track into the cluster map so these are not used for seeding
    if (!m_inputTracksKey.key().empty()) {
      ATH_MSG_DEBUG("LRT Mode: Got input track collection with "<<inputTracks->size()<< "tracks");
      long int trackIndex=0;
      for (auto t:*inputTracks) {
	updateClusterMap(trackIndex++, t, siClusterMap);
      }
    }
    ATH_CHECK(m_spacePointTool->getSpacePoints(roi, convertedSpacePoints, mnt_roi_nSPsPIX, mnt_roi_nSPsSCT, ctx, &siClusterMap));
  } else {
    ATH_CHECK(m_spacePointTool->getSpacePoints(roi, convertedSpacePoints, mnt_roi_nSPsPIX, mnt_roi_nSPsSCT, ctx));
  }


  mnt_timer_SpacePointConversion.stop();
  mnt_roi_nSPs    = mnt_roi_nSPsPIX + mnt_roi_nSPsSCT;

  if( mnt_roi_nSPs >= m_minHits ) {
    ATH_MSG_DEBUG("REGTEST / Found " << mnt_roi_nSPs << " space points.");
    ATH_MSG_DEBUG("REGTEST / Found " << mnt_roi_nSPsPIX << " Pixel space points.");
    ATH_MSG_DEBUG("REGTEST / Found " << mnt_roi_nSPsSCT << " SCT space points.");
    m_countRoIwithEnoughHits++;
  }
  else {
    ATH_MSG_DEBUG("No tracks found - too few hits in ROI to run " << mnt_roi_nSPs);
    return StatusCode::SUCCESS;
  }

  mnt_roi_lastStageExecuted = 2;


  /// this uses move semantics so doesn't do a deep copy, so ...
  std::unique_ptr<TrigRoiDescriptor> tmpRoi = std::make_unique<TrigRoiDescriptor>(roi);
  /// need to disable managment of the constituents
  tmpRoi->manageConstituents(false);
  auto vertices = std::make_unique<TrigVertexCollection>();
  std::vector<float> vZv;

  if (m_doZFinder) {
    auto mnt_timer_ZFinder = Monitored::Timer<std::chrono::milliseconds>("TIME_ZFinder");
    auto monTimeZFinder    = Monitored::Group(m_monTool, mnt_timer_ZFinder);
    mnt_timer_ZFinder.start();

    /// create a new internal superRoi - should really record this
    tmpRoi = std::make_unique<TrigRoiDescriptor>(true);
    tmpRoi->setComposite(true);

    vertices = std::make_unique<TrigVertexCollection>(*m_trigZFinder->findZ( convertedSpacePoints, roi));

    ATH_MSG_DEBUG("vertices->size(): " << vertices->size());


    if ( m_doFastZVseeding ) {
      vZv.reserve(vertices->size());
      for (const auto vertex : *vertices) {
        ATH_MSG_DEBUG("REGTEST / ZFinder vertex: " << *vertex);
        float z      = vertex->z();
        float zMinus = z - 7.0;
        float zPlus  = z + 7.0;
        TrigRoiDescriptor* newRoi =  new TrigRoiDescriptor(roi.eta(), roi.etaMinus(), roi.etaPlus(),
            roi.phi(), roi.phiMinus(), roi.phiPlus(), z, zMinus, zPlus);
        tmpRoi->push_back(newRoi);
        vZv.push_back(z);
      }

      ATH_MSG_DEBUG("REGTEST / tmpRoi: " << *tmpRoi);
    }

    mnt_timer_ZFinder.stop();

    if (  m_doZFinderOnly ) {
      /// write vertex collection ...
      /// TODO: add vertices collection handling here,
      /// should not be 0 at this point unless fastZVseeding
      /// is enabled
      return StatusCode::SUCCESS;
    }
  }


  mnt_roi_lastStageExecuted = 3;

  mnt_timer_PatternReco.start();

  mnt_timer_TripletMaking.start();

  std::vector<TrigInDetTriplet> triplets;


  if(!m_useGPU) {
    TRIG_TRACK_SEED_GENERATOR seedGen(m_tcs);

    seedGen.loadSpacePoints(convertedSpacePoints);

    if (m_doZFinder && m_doFastZVseeding) {
      seedGen.createSeeds(tmpRoi.get(), vZv);
    }
    else {
      seedGen.createSeeds(tmpRoi.get());
    }

    seedGen.getSeeds(triplets);
  }
  else {
    //GPU offloading begins ...

    makeSeedsOnGPU(m_tcs, tmpRoi.get(), convertedSpacePoints, triplets);

    //GPU offloading ends ...
  }

  ATH_MSG_DEBUG("number of triplets: " << triplets.size());
  mnt_timer_TripletMaking.stop();
  mnt_roi_lastStageExecuted = 4;

  mnt_timer_CombTracking.start();

  // 8. Combinatorial tracking

  std::vector<std::tuple<bool, double,Trk::Track*>> qualityTracks; //bool used for later filtering
  qualityTracks.reserve(triplets.size());

  auto mnt_roi_nSeeds  = Monitored::Scalar<int>("roi_nSeeds",  0);
  auto monTrk_seed = Monitored::Group(m_monTool, mnt_roi_nSeeds);

  long int trackIndex=0;

  bool PIX = true;
  bool SCT = true;
  m_trackMaker->newTrigEvent(ctx, trackEventData, PIX, SCT);

  for(unsigned int tripletIdx=0;tripletIdx!=triplets.size();tripletIdx++) {

    const TrigInDetTriplet &seed = triplets[tripletIdx];

    const Trk::SpacePoint* osp1 = seed.s1().offlineSpacePoint();
    const Trk::SpacePoint* osp2 = seed.s2().offlineSpacePoint();
    const Trk::SpacePoint* osp3 = seed.s3().offlineSpacePoint();

    if(m_checkSeedRedundancy) {
      //check if clusters do not belong to any track
      std::vector<Identifier> clusterIds;
      extractClusterIds(osp1, clusterIds);
      extractClusterIds(osp2, clusterIds);
      extractClusterIds(osp3, clusterIds);
      if(usedByAnyTrack(clusterIds, siClusterMap)) {
        continue;
      }
    }

    std::vector<const Trk::SpacePoint*> spVec = {osp1, osp2, osp3};

    ++mnt_roi_nSeeds;

    std::list<Trk::Track*> tracks = m_trackMaker->getTracks(ctx, trackEventData, spVec);

    for(std::list<Trk::Track*>::const_iterator t=tracks.begin(); t!=tracks.end(); ++t) {
      if((*t)) {
        float d0 = (*t)->perigeeParameters()==0 ? 10000.0 : (*t)->perigeeParameters()->parameters()[Trk::d0];
        if (std::abs(d0) > m_initialD0Max) {
          ATH_MSG_DEBUG("REGTEST / Reject track with d0 = " << d0 << " > " << m_initialD0Max);
          qualityTracks.push_back(std::make_tuple(false,0,(*t)));//Flag track as bad, but keep in vector for later deletion
          continue;
        }
        if(m_checkSeedRedundancy) {
          //update clusterMap
          updateClusterMap(trackIndex++, (*t), siClusterMap);
        }
        if(m_doCloneRemoval) {
          qualityTracks.push_back(std::make_tuple(true, -trackQuality((*t)), (*t)));
        }
        else {
          qualityTracks.push_back(std::make_tuple(true, 0, (*t)));
        }
      }
    }
    ATH_MSG_VERBOSE("Found "<<tracks.size()<<" tracks using triplet");
  }

  m_trackMaker->endEvent(trackEventData);

  //clone removal
  if(m_doCloneRemoval) {
    filterSharedTracks(qualityTracks);
  }

  TrackCollection initialTracks;
  initialTracks.reserve(qualityTracks.size());
  for(const auto& q : qualityTracks) {
    if (std::get<0>(q)==true) {
      initialTracks.push_back(std::get<2>(q));
    }
    else {
      delete std::get<2>(q);
    }
  }
  qualityTracks.clear();

  ATH_MSG_DEBUG("After clone removal "<<initialTracks.size()<<" tracks left");


  mnt_timer_CombTracking.stop();
  mnt_timer_PatternReco.stop();

  mnt_roi_lastStageExecuted = 5;

  mnt_timer_TrackFitter.start();

  if( ! m_dodEdxTrk ) {
     m_trigInDetTrackFitter->fit(initialTracks, outputTracks, ctx, m_particleHypothesis);
  }
  else {
     TrackCollection outputTrackswTP;
     m_trigInDetTrackFitter->fit(initialTracks, outputTracks, outputTrackswTP, ctx, m_particleHypothesis, true); // add TP to TSoS for dEdx

     // large dEdx finding
     mnt_timer_dEdxTrk.start();
     for(auto t=outputTrackswTP.begin(); t!=outputTrackswTP.end();t++) { m_trackSummaryTool->updateTrack(**t); }
     ATH_CHECK(finddEdxTrk(ctx,outputTrackswTP));
  }

  if( m_dodEdxTrk ) mnt_timer_dEdxTrk.stop(); // to include timing to destroy TrackCollection object

  if( outputTracks.empty() ) {
     ATH_MSG_DEBUG("REGTEST / No tracks fitted");
  }

  size_t counter(1);
  for (auto fittedTrack = outputTracks.begin(); fittedTrack!=outputTracks.end(); ) {
    if ((*fittedTrack)->perigeeParameters()){
      float d0 = (*fittedTrack)->perigeeParameters()->parameters()[Trk::d0];
      float z0 = (*fittedTrack)->perigeeParameters()->parameters()[Trk::z0];
      if (std::abs(d0) > m_initialD0Max || std::abs(z0) > m_Z0Max) {
        if(m_LRTmode){
          ATH_MSG_DEBUG("REGTEST / Reject track after fit with d0 = " << d0 << " z0= "  << z0
              << " larger than limits (" << m_initialD0Max << ", " << m_Z0Max << ")");
        }else{
          ATH_MSG_WARNING("REGTEST / Reject track after fit with d0 = " << d0 << " z0= "  << z0
              << " larger than limits (" << m_initialD0Max << ", " << m_Z0Max << ")");
        }
        ATH_MSG_DEBUG(**fittedTrack);
        fittedTrack = outputTracks.erase(fittedTrack);
        continue;
      }

      if(m_LRTmode){
        //reject tracks which have a d0 below a cut but only when an input track collection (from ftf) is also present
        if(m_LRTD0Min>0.0){
          if(std::abs(d0) < m_LRTD0Min && !m_inputTracksKey.key().empty()){
            ATH_MSG_DEBUG("REGTEST / Reject track after fit for min d0 (" << d0 << " < " << m_LRTD0Min <<")");
            fittedTrack = outputTracks.erase(fittedTrack);
            continue;
          }
        }

        //calculate pt
        float trkPt = 0.0;
        if(m_LRTHardMinPt > 0.0){
          //avoid a floating poitn error
          if(std::abs((*fittedTrack)->perigeeParameters()->parameters()[Trk::qOverP]) >= 1e-9){
            trkPt = std::sin((*fittedTrack)->perigeeParameters()->parameters()[Trk::theta])/std::abs((*fittedTrack)->perigeeParameters()->parameters()[Trk::qOverP]);

            if(trkPt < m_LRTHardMinPt){
              ATH_MSG_DEBUG("REGTEST / Reject track after fit for min pt (" << trkPt << " < " << m_LRTHardMinPt <<")");
              fittedTrack = outputTracks.erase(fittedTrack);
              continue;
            }
          }
        }
      }
    }

    (*fittedTrack)->info().setPatternRecognitionInfo(Trk::TrackInfo::FastTrackFinderSeed);
    ATH_MSG_VERBOSE("Updating fitted track: " << counter);
    ATH_MSG_VERBOSE(**fittedTrack);
    m_trackSummaryTool->updateTrack(**fittedTrack);
    ATH_MSG_VERBOSE("Updated track: " << counter);
    ATH_MSG_VERBOSE(**fittedTrack);
    counter++; fittedTrack++;
  }

  mnt_timer_TrackFitter.stop();
  mnt_timer_Total.stop();

  if( outputTracks.empty() ) {
    ATH_MSG_DEBUG("REGTEST / No tracks reconstructed");
  }
  mnt_roi_lastStageExecuted = 6;

  // find L1 J seeded Hit-based displaced vertex
  if( m_doHitDV ) {
     mnt_timer_HitDV.start();
     ATH_CHECK(findHitDV(ctx,convertedSpacePoints,outputTracks));
     mnt_timer_HitDV.stop();
  }

  //monitor Z-vertexing

  //monitor number of tracks
  ATH_MSG_DEBUG("REGTEST / Found " << outputTracks.size() << " tracks");
  if( !outputTracks.empty() )
    m_countRoIwithTracks++;

  ///////////// fill vectors of quantities to be monitored
  fillMon(outputTracks, *vertices, roi, ctx);

  mnt_roi_lastStageExecuted = 7;

  return StatusCode::SUCCESS;
}

double TrigFastTrackFinder::trackQuality(const Trk::Track* Tr) const {

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
    if(q < 0.) q = 0.;
    quality+=q;
  }
  return quality;
}

void TrigFastTrackFinder::filterSharedTracks(std::vector<std::tuple<bool, double,Trk::Track*>>& QT) const {

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

StatusCode TrigFastTrackFinder::finalize()
{

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("TrigFastTrackFinder::finalize() - TrigFastTrackFinder Statistics: ");
  ATH_MSG_INFO("RoI processed: " <<  m_countTotalRoI);
  ATH_MSG_INFO("RoI with enough SPs : " <<  m_countRoIwithEnoughHits);
  ATH_MSG_INFO("RoI with Track(s)  Total/goodZvertex/badZvertex: " << m_countRoIwithTracks);
  ATH_MSG_INFO("=========================================================");

  return StatusCode::SUCCESS;
}

void TrigFastTrackFinder::updateClusterMap(long int trackIdx, const Trk::Track* pTrack, std::map<Identifier, std::vector<long int> >& clusterMap) const {
  //loop over clusters

  for(auto tMOT = pTrack->measurementsOnTrack()->begin(); tMOT != pTrack->measurementsOnTrack()->end(); ++tMOT) {

    const InDet::SiClusterOnTrack* siCLOT = dynamic_cast<const InDet::SiClusterOnTrack*>(*tMOT);
    if (siCLOT==nullptr) continue;
    const InDet::SiCluster* siCL = dynamic_cast<const InDet::SiCluster*>(siCLOT->prepRawData());
    if (siCL==nullptr) continue;
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

void TrigFastTrackFinder::extractClusterIds(const Trk::SpacePoint* pSP, std::vector<Identifier>& vIds) const {
  const InDet::SiCluster* pCL = dynamic_cast<const InDet::SiCluster*>(pSP->clusterList().first);
  if(pCL!=nullptr) vIds.push_back(pCL->identify());
  //check second cluster : SCT uv clusters only !
  pCL = dynamic_cast<const InDet::SiCluster*>(pSP->clusterList().second);
  if(pCL!=nullptr) vIds.push_back(pCL->identify());
}

bool TrigFastTrackFinder::usedByAnyTrack(const std::vector<Identifier>& vIds, std::map<Identifier, std::vector<long int> >& clusterMap) const {

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

void TrigFastTrackFinder::getBeamSpot(float& shift_x, float& shift_y, const EventContext& ctx) const {
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
  Amg::Vector3D vertex = beamSpotHandle->beamPos();
  ATH_MSG_VERBOSE("Beam spot position " << vertex);
  double xVTX = vertex.x();
  double yVTX = vertex.y();
  double zVTX = vertex.z();
  double tiltXZ = beamSpotHandle->beamTilt(0);
  double tiltYZ = beamSpotHandle->beamTilt(1);
  shift_x = xVTX - tiltXZ*zVTX;//correction for tilt
  shift_y = yVTX - tiltYZ*zVTX;//correction for tilt
  ATH_MSG_VERBOSE("Beam center position:  " << shift_x <<"  "<< shift_y);
}


void TrigFastTrackFinder::fillMon(const TrackCollection& tracks, const TrigVertexCollection& vertices,
                                  const TrigRoiDescriptor& roi, const EventContext& ctx) const {
  float shift_x = 0;
  float shift_y = 0;
  if(m_useBeamSpot) {
    getBeamSpot(shift_x, shift_y, ctx);
  }
  auto mnt_roi_eta      = Monitored::Scalar<float>("roi_eta",      0.0);
  auto mnt_roi_phi      = Monitored::Scalar<float>("roi_phi",      0.0);
  auto mnt_roi_etaWidth = Monitored::Scalar<float>("roi_etaWidth", 0.0);
  auto mnt_roi_phiWidth = Monitored::Scalar<float>("roi_phiWidth", 0.0);
  auto mnt_roi_z        = Monitored::Scalar<float>("roi_z",        0.0);
  auto mnt_roi_zWidth   = Monitored::Scalar<float>("roi_zWidith",  0.0);
  auto mnt_roi_nTracks  = Monitored::Scalar<int>("roi_nTracks", 0);
  auto monRoI           = Monitored::Group(m_monTool, mnt_roi_eta, mnt_roi_phi, mnt_roi_etaWidth, mnt_roi_phiWidth, mnt_roi_z, mnt_roi_zWidth, mnt_roi_nTracks);

  for(unsigned int i=0; i<roi.size(); i++) {
     mnt_roi_eta      = (roi.at(i))->eta();
     mnt_roi_phi      = (roi.at(i))->phi();
     mnt_roi_etaWidth = (roi.at(i))->etaPlus() - (roi.at(i))->etaMinus();
     mnt_roi_phiWidth = CxxUtils::wrapToPi((roi.at(i))->phiPlus() - (roi.at(i))->phiMinus());
     mnt_roi_z        = (roi.at(i))->zed();
     mnt_roi_zWidth   = (roi.at(i))->zedPlus() - (roi.at(i))->zedMinus();
  }

  std::vector<float> mnt_trk_pt;
  std::vector<float> mnt_trk_a0;
  std::vector<float> mnt_trk_z0;
  std::vector<float> mnt_trk_phi0;
  std::vector<float> mnt_trk_eta;
  std::vector<float> mnt_trk_chi2dof;
  std::vector<float> mnt_trk_nSiHits;
  std::vector<float> mnt_trk_nPIXHits;
  std::vector<float> mnt_trk_nSCTHits;
  std::vector<float> mnt_trk_a0beam;
  std::vector<float> mnt_trk_dPhi0;
  std::vector<float> mnt_trk_dEta;

  auto mon_pt       = Monitored::Collection("trk_pt",       mnt_trk_pt);
  auto mon_a0       = Monitored::Collection("trk_a0",       mnt_trk_a0);
  auto mon_z0       = Monitored::Collection("trk_z0",       mnt_trk_z0);
  auto mon_phi0     = Monitored::Collection("trk_phi0",     mnt_trk_phi0);
  auto mon_eta      = Monitored::Collection("trk_eta",      mnt_trk_eta);
  auto mon_chi2dof  = Monitored::Collection("trk_chi2dof",  mnt_trk_chi2dof);
  auto mon_nSiHits  = Monitored::Collection("trk_nSiHits",  mnt_trk_nSiHits);
  auto mon_nPIXHits = Monitored::Collection("trk_nPIXHits", mnt_trk_nPIXHits);
  auto mon_nSCTHits = Monitored::Collection("trk_nSCTHits", mnt_trk_nSCTHits);
  auto mon_a0beam   = Monitored::Collection("trk_a0beam",   mnt_trk_a0beam);
  auto mon_dPhi0    = Monitored::Collection("trk_dPhi0",    mnt_trk_dPhi0);
  auto mon_dEta     = Monitored::Collection("trk_dEta",     mnt_trk_dEta);
  auto monTrk       = Monitored::Group(m_monTool, mon_pt, mon_a0, mon_z0, mon_phi0, mon_eta, mon_chi2dof,
				       mon_nSiHits, mon_nPIXHits, mon_nSCTHits, mon_a0beam, mon_dPhi0, mon_dEta);

  std::vector<float> mnt_roi_zVertices;
  auto mon_roi_nZvertices = Monitored::Scalar<int>("roi_nZvertices", 0);
  auto mon_roi_zVertices  = Monitored::Collection("roi_zVertices", mnt_roi_zVertices);
  auto monVtx = Monitored::Group(m_monTool, mon_roi_nZvertices, mon_roi_zVertices);
  mon_roi_nZvertices = vertices.size();
  for (const auto vertex : vertices) {
    mnt_roi_zVertices.push_back(vertex->z());
  }
  mnt_roi_nTracks = tracks.size();


  for (auto track : tracks) {
    const Trk::TrackParameters* trackPars = track->perigeeParameters();
    if(trackPars==nullptr) {
      continue;
    }

    if(trackPars->covariance()==nullptr) {
      continue;
    }

    float a0 = trackPars->parameters()[Trk::d0];
    float z0 = trackPars->parameters()[Trk::z0];
    float phi0 = trackPars->parameters()[Trk::phi0];
    float theta = trackPars->parameters()[Trk::theta];
    float eta = -log(tan(0.5*theta));
    mnt_trk_a0.push_back(a0);
    mnt_trk_z0.push_back(z0);
    mnt_trk_phi0.push_back(phi0);
    mnt_trk_a0beam.push_back(a0+shift_x*sin(phi0)-shift_y*cos(phi0));
    mnt_trk_eta.push_back(eta);
    for(unsigned int i=0; i<roi.size(); i++) {
       mnt_trk_dPhi0.push_back(CxxUtils::wrapToPi(phi0 - (roi.at(i))->phi()));
       mnt_trk_dEta.push_back(eta - (roi.at(i))->eta());
    }

    float qOverP = trackPars->parameters()[Trk::qOverP];
    if (qOverP==0) {
      ATH_MSG_DEBUG("REGTEST / q/p == 0, adjusting to 1e-12");
      qOverP = 1e-12;
    }
    float pT=sin(theta)/qOverP;

    const Trk::FitQuality* fq = track->fitQuality();
    float chi2 = 1e8;
    if (fq) {
      ATH_MSG_VERBOSE("Fitted chi2: " << fq->chiSquared());
      ATH_MSG_VERBOSE("Fitted ndof: " << fq->numberDoF());
      if(fq->numberDoF()!=0) {
        chi2 = fq->chiSquared()/fq->numberDoF();
      }
    }
    mnt_trk_pt.push_back(pT);
    mnt_trk_chi2dof.push_back(chi2);

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
    mnt_trk_nPIXHits.push_back(nPix);
    mnt_trk_nSCTHits.push_back(nSct/2);
    mnt_trk_nSiHits.push_back(nPix + nSct/2);

    ATH_MSG_DEBUG("REGTEST / track npix/nsct/phi0/pt/eta/d0/z0/chi2: " <<
        nPix   << " / "  <<
        nSct/2 << " / "  <<
        phi0   << " / "  <<
        pT     << " / " <<
        eta    << " / " <<
        a0     << " / " <<
        z0     << " / " <<
        chi2);
    // tighter selection for unbiased residuals
    bool goodTrack = std::fabs(pT)>1000. && (nPix + nSct/2) > 3 && nSct > 0;
    if (goodTrack && m_doResMonitoring) {
      runResidualMonitoring(*track, ctx);
    }
  }
}

void TrigFastTrackFinder::runResidualMonitoring(const Trk::Track& track, const EventContext& ctx) const {

  std::vector<float> mnt_layer_IBL;
  std::vector<float> mnt_layer_PixB;
  std::vector<float> mnt_layer_PixE;
  std::vector<float> mnt_layer_SCTB;
  std::vector<float> mnt_layer_SCTE;
  std::vector<float> mnt_hit_IBLPhiResidual;
  std::vector<float> mnt_hit_IBLEtaResidual;
  std::vector<float> mnt_hit_IBLPhiPull;
  std::vector<float> mnt_hit_IBLEtaPull;
  std::vector<float> mnt_hit_PIXBarrelPhiResidual;
  std::vector<float> mnt_hit_PIXBarrelEtaResidual;
  std::vector<float> mnt_hit_PIXBarrelPhiPull;
  std::vector<float> mnt_hit_PIXBarrelEtaPull;
  std::vector<float> mnt_hit_SCTBarrelResidual;
  std::vector<float> mnt_hit_SCTBarrelPull;
  std::vector<float> mnt_hit_PIXEndcapPhiResidual;
  std::vector<float> mnt_hit_PIXEndcapEtaResidual;
  std::vector<float> mnt_hit_PIXEndcapPhiPull;
  std::vector<float> mnt_hit_PIXEndcapEtaPull;
  std::vector<float> mnt_hit_SCTEndcapResidual;
  std::vector<float> mnt_hit_SCTEndcapPull;
  std::vector<float> mnt_hit_PIXBarrelL1PhiResidual;
  std::vector<float> mnt_hit_PIXBarrelL1EtaResidual;
  std::vector<float> mnt_hit_PIXBarrelL2PhiResidual;
  std::vector<float> mnt_hit_PIXBarrelL2EtaResidual;
  std::vector<float> mnt_hit_PIXBarrelL3PhiResidual;
  std::vector<float> mnt_hit_PIXBarrelL3EtaResidual;
  std::vector<float> mnt_hit_PIXEndcapL1PhiResidual;
  std::vector<float> mnt_hit_PIXEndcapL1EtaResidual;
  std::vector<float> mnt_hit_PIXEndcapL2PhiResidual;
  std::vector<float> mnt_hit_PIXEndcapL2EtaResidual;
  std::vector<float> mnt_hit_PIXEndcapL3PhiResidual;
  std::vector<float> mnt_hit_PIXEndcapL3EtaResidual;
  std::vector<float> mnt_hit_SCTBarrelL1PhiResidual;
  std::vector<float> mnt_hit_SCTBarrelL2PhiResidual;
  std::vector<float> mnt_hit_SCTBarrelL3PhiResidual;
  std::vector<float> mnt_hit_SCTBarrelL4PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL1PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL2PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL3PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL4PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL5PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL6PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL7PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL8PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL9PhiResidual;
  auto mon_layer_IBL  = Monitored::Collection("layer_IBL", mnt_layer_IBL);
  auto mon_layer_PixB = Monitored::Collection("layer_PixB",mnt_layer_PixB);
  auto mon_layer_PixE = Monitored::Collection("layer_PixE",mnt_layer_PixE);
  auto mon_layer_SCTB = Monitored::Collection("layer_SCTB",mnt_layer_SCTB);
  auto mon_layer_SCTE = Monitored::Collection("layer_SCTE",mnt_layer_SCTE);
  auto mon_hit_IBLPhiResidual = Monitored::Collection("hit_IBLPhiResidual",mnt_hit_IBLPhiResidual);
  auto mon_hit_IBLEtaResidual = Monitored::Collection("hit_IBLEtaResidual",mnt_hit_IBLEtaResidual);
  auto mon_hit_IBLPhiPull = Monitored::Collection("hit_IBLPhiPull",mnt_hit_IBLPhiPull);
  auto mon_hit_IBLEtaPull = Monitored::Collection("hit_IBLEtaPull",mnt_hit_IBLEtaPull);
  auto mon_hit_PIXBarrelPhiResidual = Monitored::Collection("hit_PIXBarrelPhiResidual",mnt_hit_PIXBarrelPhiResidual);
  auto mon_hit_PIXBarrelEtaResidual = Monitored::Collection("hit_PIXBarrelEtaResidual",mnt_hit_PIXBarrelEtaResidual);
  auto mon_hit_PIXBarrelPhiPull = Monitored::Collection("hit_PIXBarrelPhiPull",mnt_hit_PIXBarrelPhiPull);
  auto mon_hit_PIXBarrelEtaPull = Monitored::Collection("hit_PIXBarrelEtaPull",mnt_hit_PIXBarrelEtaPull);
  auto mon_hit_SCTBarrelResidual = Monitored::Collection("hit_SCTBarrelResidual",mnt_hit_SCTBarrelResidual);
  auto mon_hit_SCTBarrelPull = Monitored::Collection("hit_SCTBarrelPull",mnt_hit_SCTBarrelPull);
  auto mon_hit_PIXEndcapPhiResidual = Monitored::Collection("hit_PIXEndcapPhiResidual",mnt_hit_PIXEndcapPhiResidual);
  auto mon_hit_PIXEndcapEtaResidual = Monitored::Collection("hit_PIXEndcapEtaResidual",mnt_hit_PIXEndcapEtaResidual);
  auto mon_hit_PIXEndcapPhiPull = Monitored::Collection("hit_PIXEndcapPhiPull",mnt_hit_PIXEndcapPhiPull);
  auto mon_hit_PIXEndcapEtaPull = Monitored::Collection("hit_PIXEndcapEtaPull",mnt_hit_PIXEndcapEtaPull);
  auto mon_hit_SCTEndcapResidual = Monitored::Collection("hit_SCTEndcapResidual",mnt_hit_SCTEndcapResidual);
  auto mon_hit_SCTEndcapPull = Monitored::Collection("hit_SCTEndcapPull",mnt_hit_SCTEndcapPull);
  auto mon_hit_PIXBarrelL1PhiResidual = Monitored::Collection("hit_PIXBarrelL1PhiResidual",mnt_hit_PIXBarrelL1PhiResidual);
  auto mon_hit_PIXBarrelL1EtaResidual = Monitored::Collection("hit_PIXBarrelL1EtaResidual",mnt_hit_PIXBarrelL1EtaResidual);
  auto mon_hit_PIXBarrelL2PhiResidual = Monitored::Collection("hit_PIXBarrelL2PhiResidual",mnt_hit_PIXBarrelL2PhiResidual);
  auto mon_hit_PIXBarrelL2EtaResidual = Monitored::Collection("hit_PIXBarrelL2EtaResidual",mnt_hit_PIXBarrelL2EtaResidual);
  auto mon_hit_PIXBarrelL3PhiResidual = Monitored::Collection("hit_PIXBarrelL3PhiResidual",mnt_hit_PIXBarrelL3PhiResidual);
  auto mon_hit_PIXBarrelL3EtaResidual = Monitored::Collection("hit_PIXBarrelL3EtaResidual",mnt_hit_PIXBarrelL3EtaResidual);
  auto mon_hit_PIXEndcapL1PhiResidual = Monitored::Collection("hit_PIXEndcapL1PhiResidual",mnt_hit_PIXEndcapL1PhiResidual);
  auto mon_hit_PIXEndcapL1EtaResidual = Monitored::Collection("hit_PIXEndcapL1EtaResidual",mnt_hit_PIXEndcapL1EtaResidual);
  auto mon_hit_PIXEndcapL2PhiResidual = Monitored::Collection("hit_PIXEndcapL2PhiResidual",mnt_hit_PIXEndcapL2PhiResidual);
  auto mon_hit_PIXEndcapL2EtaResidual = Monitored::Collection("hit_PIXEndcapL2EtaResidual",mnt_hit_PIXEndcapL2EtaResidual);
  auto mon_hit_PIXEndcapL3PhiResidual = Monitored::Collection("hit_PIXEndcapL3PhiResidual",mnt_hit_PIXEndcapL3PhiResidual);
  auto mon_hit_PIXEndcapL3EtaResidual = Monitored::Collection("hit_PIXEndcapL3EtaResidual",mnt_hit_PIXEndcapL3EtaResidual);
  auto mon_hit_SCTBarrelL1PhiResidual = Monitored::Collection("hit_SCTBarrelL1PhiResidual",mnt_hit_SCTBarrelL1PhiResidual);
  auto mon_hit_SCTBarrelL2PhiResidual = Monitored::Collection("hit_SCTBarrelL2PhiResidual",mnt_hit_SCTBarrelL2PhiResidual);
  auto mon_hit_SCTBarrelL3PhiResidual = Monitored::Collection("hit_SCTBarrelL3PhiResidual",mnt_hit_SCTBarrelL3PhiResidual);
  auto mon_hit_SCTBarrelL4PhiResidual = Monitored::Collection("hit_SCTBarrelL4PhiResidual",mnt_hit_SCTBarrelL4PhiResidual);
  auto mon_hit_SCTEndcapL1PhiResidual = Monitored::Collection("hit_SCTEndcapL1PhiResidual",mnt_hit_SCTEndcapL1PhiResidual);
  auto mon_hit_SCTEndcapL2PhiResidual = Monitored::Collection("hit_SCTEndcapL2PhiResidual",mnt_hit_SCTEndcapL2PhiResidual);
  auto mon_hit_SCTEndcapL3PhiResidual = Monitored::Collection("hit_SCTEndcapL3PhiResidual",mnt_hit_SCTEndcapL3PhiResidual);
  auto mon_hit_SCTEndcapL4PhiResidual = Monitored::Collection("hit_SCTEndcapL4PhiResidual",mnt_hit_SCTEndcapL4PhiResidual);
  auto mon_hit_SCTEndcapL5PhiResidual = Monitored::Collection("hit_SCTEndcapL5PhiResidual",mnt_hit_SCTEndcapL5PhiResidual);
  auto mon_hit_SCTEndcapL6PhiResidual = Monitored::Collection("hit_SCTEndcapL6PhiResidual",mnt_hit_SCTEndcapL6PhiResidual);
  auto mon_hit_SCTEndcapL7PhiResidual = Monitored::Collection("hit_SCTEndcapL7PhiResidual",mnt_hit_SCTEndcapL7PhiResidual);
  auto mon_hit_SCTEndcapL8PhiResidual = Monitored::Collection("hit_SCTEndcapL8PhiResidual",mnt_hit_SCTEndcapL8PhiResidual);
  auto mon_hit_SCTEndcapL9PhiResidual = Monitored::Collection("hit_SCTEndcapL9PhiResidual",mnt_hit_SCTEndcapL9PhiResidual);

  auto monRes = Monitored::Group(m_monTool, mon_layer_IBL, mon_layer_PixB, mon_layer_PixE, mon_layer_SCTB, mon_layer_SCTE, mon_hit_IBLPhiResidual, mon_hit_IBLEtaResidual, mon_hit_IBLPhiPull, mon_hit_IBLEtaPull, mon_hit_PIXBarrelPhiResidual, mon_hit_PIXBarrelEtaResidual, mon_hit_PIXBarrelPhiPull, mon_hit_PIXBarrelEtaPull, mon_hit_SCTBarrelResidual, mon_hit_SCTBarrelPull, mon_hit_PIXEndcapPhiResidual, mon_hit_PIXEndcapEtaResidual, mon_hit_PIXEndcapPhiPull, mon_hit_PIXEndcapEtaPull, mon_hit_SCTEndcapResidual, mon_hit_SCTEndcapPull, mon_hit_PIXBarrelL1PhiResidual, mon_hit_PIXBarrelL1EtaResidual, mon_hit_PIXBarrelL2PhiResidual, mon_hit_PIXBarrelL2EtaResidual, mon_hit_PIXBarrelL3PhiResidual, mon_hit_PIXBarrelL3EtaResidual, mon_hit_PIXEndcapL1PhiResidual, mon_hit_PIXEndcapL1EtaResidual, mon_hit_PIXEndcapL2PhiResidual, mon_hit_PIXEndcapL2EtaResidual, mon_hit_PIXEndcapL3PhiResidual, mon_hit_PIXEndcapL3EtaResidual, mon_hit_SCTBarrelL1PhiResidual, mon_hit_SCTBarrelL2PhiResidual, mon_hit_SCTBarrelL3PhiResidual, mon_hit_SCTBarrelL4PhiResidual, mon_hit_SCTEndcapL1PhiResidual, mon_hit_SCTEndcapL2PhiResidual, mon_hit_SCTEndcapL3PhiResidual, mon_hit_SCTEndcapL4PhiResidual, mon_hit_SCTEndcapL5PhiResidual, mon_hit_SCTEndcapL6PhiResidual, mon_hit_SCTEndcapL7PhiResidual, mon_hit_SCTEndcapL8PhiResidual, mon_hit_SCTEndcapL9PhiResidual);

  std::vector<TrigL2HitResidual> vResid;
  vResid.clear();
  StatusCode scRes = m_trigInDetTrackFitter->getUnbiasedResiduals(track,vResid, ctx);
  if(!scRes.isSuccess()) return;
  for(std::vector<TrigL2HitResidual>::iterator it=vResid.begin();it!=vResid.end();++it) {
    Identifier id = it->identify();
    int pixlayer= (m_pixelId->layer_disk(id) );
    int sctlayer= (m_sctId->layer_disk(id) );

    switch(it->regionId()) {
      case Region::PixBarrel :
        mnt_layer_PixB.push_back(pixlayer);
        mnt_hit_PIXBarrelPhiResidual.push_back(it->phiResidual());
        mnt_hit_PIXBarrelPhiPull.push_back(it->phiPull());
        mnt_hit_PIXBarrelEtaResidual.push_back(it->etaResidual());
        mnt_hit_PIXBarrelEtaPull.push_back(it->etaPull());
        if (pixlayer == 1) {
          mnt_hit_PIXBarrelL1PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXBarrelL1EtaResidual.push_back(it->etaResidual());
        }
        if (pixlayer == 2) {
          mnt_hit_PIXBarrelL2PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXBarrelL2EtaResidual.push_back(it->etaResidual());
        }
        if (pixlayer == 3) {
          mnt_hit_PIXBarrelL3PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXBarrelL3EtaResidual.push_back(it->etaResidual());
        }
        break;
      case Region::PixEndcap :
        ATH_MSG_DEBUG("Pixel Endcap "  );
        mnt_layer_PixE.push_back(pixlayer);
        mnt_hit_PIXEndcapPhiResidual.push_back(it->phiResidual());
        mnt_hit_PIXEndcapPhiPull.push_back(it->phiPull());
        mnt_hit_PIXEndcapEtaResidual.push_back(it->etaResidual());
        mnt_hit_PIXEndcapEtaPull.push_back(it->etaPull());
        if (pixlayer == 0) {
          mnt_hit_PIXEndcapL1PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXEndcapL1EtaResidual.push_back(it->etaResidual());
        }
        if (pixlayer == 1) {
          mnt_hit_PIXEndcapL2PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXEndcapL2EtaResidual.push_back(it->etaResidual());
        }
        if (pixlayer == 2) {
          mnt_hit_PIXEndcapL3PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXEndcapL3EtaResidual.push_back(it->etaResidual());
        }
        break;
      case Region::SctBarrel :
        mnt_layer_SCTB.push_back(sctlayer);
        mnt_hit_SCTBarrelResidual.push_back(it->phiResidual());
        mnt_hit_SCTBarrelPull.push_back(it->phiPull());
        if (sctlayer == 0) {
          mnt_hit_SCTBarrelL1PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 1) {
          mnt_hit_SCTBarrelL2PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 2) {
          mnt_hit_SCTBarrelL3PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 3) {
          mnt_hit_SCTBarrelL4PhiResidual.push_back(it->phiResidual());
        }
        break;
      case Region::SctEndcap :
        ATH_MSG_DEBUG("SCT Endcap"  );
        mnt_layer_SCTE.push_back(sctlayer);
        mnt_hit_SCTEndcapResidual.push_back(it->phiResidual());
        mnt_hit_SCTEndcapPull.push_back(it->phiPull());
        if (sctlayer == 0) {
          mnt_hit_SCTEndcapL1PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 1) {
          mnt_hit_SCTEndcapL2PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 2) {
          mnt_hit_SCTEndcapL3PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 3) {
          mnt_hit_SCTEndcapL4PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 4) {
          mnt_hit_SCTEndcapL5PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 5) {
          mnt_hit_SCTEndcapL6PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 6) {
          mnt_hit_SCTEndcapL7PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 7) {
          mnt_hit_SCTEndcapL8PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 8) {
          mnt_hit_SCTEndcapL9PhiResidual.push_back(it->phiResidual());
        }
        break;
      case Region::IBL :
        mnt_layer_IBL.push_back(pixlayer);
        if (m_tcs.m_maxSiliconLayer==32) {
          mnt_hit_IBLPhiResidual.push_back(it->phiResidual());
          mnt_hit_IBLPhiPull.push_back(it->phiPull());
          mnt_hit_IBLEtaResidual.push_back(it->etaResidual());
          mnt_hit_IBLEtaPull.push_back(it->etaPull());
        }
        else {//No IBL, fill pixel histograms instead
          ATH_MSG_DEBUG("IBL wrong region"  );
          mnt_hit_PIXBarrelPhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXBarrelPhiPull.push_back(it->phiPull());
          mnt_hit_PIXBarrelEtaResidual.push_back(it->etaResidual());
          mnt_hit_PIXBarrelEtaPull.push_back(it->etaPull());
        }
        break;
      case Region::Undefined :
        ATH_MSG_DEBUG("Undefined ID region");
        break;
    }
  }
}

void TrigFastTrackFinder::makeSeedsOnGPU(const TrigCombinatorialSettings& tcs, const IRoiDescriptor* roi, const std
::vector<TrigSiSpacePointBase>& vsp, std::vector<TrigInDetTriplet>& output) const {

  output.clear();

  TrigAccel::DATA_EXPORT_BUFFER* dataBuffer = new TrigAccel::DATA_EXPORT_BUFFER(5000);//i.e. 5KB

  size_t actualSize = m_accelTool->exportSeedMakingJob(tcs, roi, vsp, *dataBuffer);

  ATH_MSG_DEBUG("SeedMakingJob is ready, data size for transfer = " <<actualSize);

  std::shared_ptr<TrigAccel::OffloadBuffer> pBuff = std::make_shared<TrigAccel::OffloadBuffer>(dataBuffer);

  TrigAccel::Work* pJob = m_accelSvc->createWork(TrigAccel::InDetJobControlCode::MAKE_SEEDS, pBuff);

  if(pJob) {
    ATH_MSG_DEBUG("Work item created for task "<<TrigAccel::InDetJobControlCode::MAKE_SEEDS);

    pJob->run();


    std::shared_ptr<TrigAccel::OffloadBuffer> pOB = pJob->getOutput();

    TrigAccel::OUTPUT_SEED_STORAGE* pOutput = reinterpret_cast<TrigAccel::OUTPUT_SEED_STORAGE *>(pOB->m_rawBuffer);

    ATH_MSG_DEBUG("Found "<<pOutput->m_nSeeds<<" triplets on GPU");

    int nTriplets = pOutput->m_nSeeds;

    //copy seeds into the output buffer

    output.clear();

    for(int k=0;k<nTriplets;k++) {
      const TrigSiSpacePointBase& SPi = vsp[pOutput->m_innerIndex[k]];
      const TrigSiSpacePointBase& SPm = vsp[pOutput->m_middleIndex[k]];
      const TrigSiSpacePointBase& SPo = vsp[pOutput->m_outerIndex[k]];
      output.emplace_back(SPi, SPm, SPo, pOutput->m_Q[k]);
    }
  }

  delete pJob;
  delete dataBuffer;
}

StatusCode TrigFastTrackFinder::findHitDV(const EventContext& ctx, const std::vector<TrigSiSpacePointBase>& convertedSpacePoints,
					  const TrackCollection& outputTracks) const
{
   // Output containers & writeHandle
   auto hitDVTrkContainer    = std::make_unique<xAOD::TrigCompositeContainer>();
   auto hitDVTrkContainerAux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
   hitDVTrkContainer->setStore(hitDVTrkContainerAux.get());
   SG::WriteHandle<xAOD::TrigCompositeContainer> hitDVTrkHandle(m_hitDVTrkKey, ctx);

   auto hitDVSPContainer     = std::make_unique<xAOD::TrigCompositeContainer>();
   auto hitDVSPContainerAux  = std::make_unique<xAOD::TrigCompositeAuxContainer>();
   hitDVSPContainer->setStore(hitDVSPContainerAux.get());
   SG::WriteHandle<xAOD::TrigCompositeContainer> hitDVSPHandle(m_hitDVSPKey, ctx);

   // select good tracks
   const float  TRKCUT_PT               = 0.5;
   const float  TRKCUT_A0BEAM           = 2.5;
   const int    TRKCUT_N_HITS_INNERMOST = 1;
   const int    TRKCUT_N_HITS_PIX       = 2;
   const int    TRKCUT_N_HITS           = 4;
   std::unordered_map<const Trk::PrepRawData*, int> umap_fittedTrack_prd;
   int fittedTrack_id = -1;
   std::vector<int>   v_dvtrk_id;
   std::vector<float> v_dvtrk_pt;
   std::vector<float> v_dvtrk_eta;
   std::vector<float> v_dvtrk_phi;
   std::vector<int>   v_dvtrk_n_hits_innermost;
   std::vector<int>   v_dvtrk_n_hits_pix;
   std::vector<int>   v_dvtrk_n_hits_sct;
   std::vector<float> v_dvtrk_a0beam;
   for (auto track : outputTracks) {
      if ( ! track->perigeeParameters() ) continue;
      if ( ! track->trackSummary() )      continue;
      int n_hits_innermost = track->trackSummary()->get(Trk::SummaryType::numberOfInnermostPixelLayerHits);
      int n_hits_pix       = track->trackSummary()->get(Trk::SummaryType::numberOfPixelHits);
      int n_hits_sct       = track->trackSummary()->get(Trk::SummaryType::numberOfSCTHits);
      if( n_hits_innermost < TRKCUT_N_HITS_INNERMOST )  continue;
      if( n_hits_pix < TRKCUT_N_HITS_PIX )              continue;
      if( (n_hits_pix+n_hits_sct) < TRKCUT_N_HITS )     continue;
      float theta  = track->perigeeParameters()->parameters()[Trk::theta];
      float qOverP = track->perigeeParameters()->parameters()[Trk::qOverP];
      if( std::abs(qOverP)<1e-12 ) qOverP = 1e-12;
      float pt = std::sin(theta)/qOverP;
      pt /= 1000.0;
      if( pt < TRKCUT_PT ) continue;
      float a0   = track->perigeeParameters()->parameters()[Trk::d0];
      float phi0 = track->perigeeParameters()->parameters()[Trk::phi0];
      float shift_x = 0; float shift_y = 0;
      if( m_useBeamSpot ) getBeamSpot(shift_x, shift_y, ctx);
      float a0beam = a0 + shift_x*sin(phi0)-shift_y*cos(phi0);
      if( std::abs(a0beam) > TRKCUT_A0BEAM ) continue;

      // track is selected
      fittedTrack_id++;
      ATH_MSG_DEBUG("Selected track pT = " << pt);
      DataVector<const Trk::MeasurementBase>::const_iterator
	 m  = track->measurementsOnTrack()->begin(),
	 me = track->measurementsOnTrack()->end  ();
      for(; m!=me; ++m ) {
	 const Trk::PrepRawData* prd = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
	 if( prd == nullptr ) continue;
	 if( umap_fittedTrack_prd.find(prd) == umap_fittedTrack_prd.end() ) {
	    umap_fittedTrack_prd.insert(std::make_pair(prd,fittedTrack_id));
	 }
      }

      float eta = -std::log(std::tan(0.5*theta));
      float phi = track->perigeeParameters()->parameters()[Trk::phi];
      v_dvtrk_id.push_back(fittedTrack_id);
      v_dvtrk_pt.push_back(pt*1000.0);
      v_dvtrk_eta.push_back(eta);
      v_dvtrk_phi.push_back(phi);
      v_dvtrk_n_hits_innermost.push_back(n_hits_innermost);
      v_dvtrk_n_hits_pix.push_back(n_hits_pix);
      v_dvtrk_n_hits_sct.push_back(n_hits_sct);
      v_dvtrk_a0beam.push_back(a0beam);
   }
   ATH_MSG_DEBUG("Nr of selected tracks = " << fittedTrack_id);
   ATH_MSG_DEBUG("Nr of PRDs used by selected tracks = " << umap_fittedTrack_prd.size());

   // space points
   int n_sp           = 0;
   int n_sp_usedByTrk = 0;
   std::vector<float> v_sp_eta;
   std::vector<float> v_sp_r;
   std::vector<float> v_sp_phi;
   std::vector<int>   v_sp_layer;
   std::vector<int>   v_sp_isPix;
   std::vector<int>   v_sp_isSct;
   std::vector<int>   v_sp_usedTrkId;

   for(unsigned int iSp=0; iSp<convertedSpacePoints.size(); iSp++) {

      const Trk::SpacePoint* sp = convertedSpacePoints[iSp].offlineSpacePoint();
      const Amg::Vector3D& pos_sp = sp->globalPosition();
      float sp_x = pos_sp[Amg::x];
      float sp_y = pos_sp[Amg::y];
      float sp_z = pos_sp[Amg::z];
      TVector3 p3Sp(sp_x,sp_y,sp_z);
      float sp_eta = p3Sp.Eta();
      float sp_phi = p3Sp.Phi();

      // whether used by selected tracks
      const Trk::PrepRawData* prd = sp->clusterList().first;
      int usedTrack_id = -1;
      if( prd != nullptr && umap_fittedTrack_prd.find(prd) != umap_fittedTrack_prd.end() ) {
	 ATH_MSG_DEBUG("prd first is there in umap");
	 usedTrack_id = umap_fittedTrack_prd[prd];
      }
      else {
	 const Trk::PrepRawData* prd = sp->clusterList().second;
	 if( prd != nullptr && umap_fittedTrack_prd.find(prd) != umap_fittedTrack_prd.end() ) {
	    ATH_MSG_DEBUG("prd second is there in umap");
	    usedTrack_id = umap_fittedTrack_prd[prd];
	 }
      }

      //
      n_sp++;
      if( usedTrack_id != -1 ) n_sp_usedByTrk++;
      int  layer = convertedSpacePoints[iSp].layer();
      int  isPix = convertedSpacePoints[iSp].isPixel() ? 1 : 0;
      int  isSct = convertedSpacePoints[iSp].isSCT() ?   1 : 0;
      float sp_r = convertedSpacePoints[iSp].r();

      v_sp_eta.push_back(sp_eta);
      v_sp_r.push_back(sp_r);
      v_sp_phi.push_back(sp_phi);
      v_sp_layer.push_back(layer);
      v_sp_isPix.push_back(isPix);
      v_sp_isSct.push_back(isSct);
      v_sp_usedTrkId.push_back(usedTrack_id);

      ATH_MSG_DEBUG("+++ SP eta / phi / layer / ixPix / usedTrack_id = " << sp_eta << " / " << sp_phi << " / " << layer << " / " << isPix << " / " << usedTrack_id);

   }
   ATH_MSG_DEBUG("Nr of SPs = " << n_sp);
   ATH_MSG_DEBUG("Nr of SPs used by selected tracks = " << n_sp_usedByTrk);

   // Seed
   std::vector<float> v_seeds_eta;
   std::vector<float> v_seeds_phi;

   if( m_doHitDV_Seeding ) {

      // space-point based (unseeded mode)
      ATH_CHECK( findSPSeeds(v_sp_eta, v_sp_phi, v_sp_layer, v_sp_usedTrkId, v_seeds_eta, v_seeds_phi) );
      ATH_MSG_DEBUG("Nr of SP seeds = " << v_seeds_eta.size());

      // add J1 J30 seeds
      const unsigned int L1JET_ET_CUT = 30;

      auto recJetRoiCollectionHandle = SG::makeHandle( m_recJetRoiCollectionKey, ctx );
      const DataVector<LVL1::RecJetRoI> *recJetRoiCollection = recJetRoiCollectionHandle.cptr();
      if (!recJetRoiCollectionHandle.isValid()){
	 ATH_MSG_ERROR("ReadHandle for DataVector<LVL1::RecJetRoI> key:" << m_recJetRoiCollectionKey.key() << " isn't Valid");
	 return StatusCode::FAILURE;
      }
      for (size_t size=0; size<recJetRoiCollection->size(); ++size){
	 const LVL1::RecJetRoI* recRoI = recJetRoiCollection->at(size);
	 if( recRoI == nullptr ) continue;
	 bool isSeed = false;
	 for( const unsigned int thrMapping : recRoI->thresholdsPassed()) {
	    double thrValue = recRoI->triggerThreshold(thrMapping) * Gaudi::Units::GeV;
	    if( thrValue >= L1JET_ET_CUT ) {
	       isSeed = true;
	       break;
	    }
	 }
	 if( ! isSeed ) continue;
	 // Convert to ATLAS phi convention: see RoIResultToAOD.cxx
	 float roiPhi = recRoI->phi();
	 if( roiPhi > TMath::Pi() ) roiPhi -= 2 * TMath::Pi();
	 v_seeds_eta.push_back(recRoI->eta());
	 v_seeds_phi.push_back(roiPhi);
      }
      ATH_MSG_DEBUG("Nr of SP + L1_J30 seeds = " << v_seeds_eta.size());
   }

   // fill objects
   
   // track
   const float TRKCUT_DELTA_R_TO_SEED = 1.0;
   for(unsigned int iTrk=0; iTrk<v_dvtrk_pt.size(); ++iTrk) {
      float trk_eta = v_dvtrk_eta[iTrk];
      float trk_phi = v_dvtrk_phi[iTrk];
      if( m_doHitDV_Seeding ) {
	 bool isNearSeed = false;
	 for (unsigned int iSeed=0; iSeed<v_seeds_eta.size(); ++iSeed) {
	    float seed_eta = v_seeds_eta[iSeed];
	    float seed_phi = v_seeds_phi[iSeed];
	    float dR = deltaR(trk_eta,trk_phi,seed_eta,seed_phi);
	    if( dR <= TRKCUT_DELTA_R_TO_SEED ) { isNearSeed = true; break; }
	 }
	 if( ! isNearSeed ) continue;
      }
      xAOD::TrigComposite *hitDVTrk = new xAOD::TrigComposite();
      hitDVTrk->makePrivateStore();
      hitDVTrkContainer->push_back(hitDVTrk);
      hitDVTrk->setDetail<int>  ("dvtrk_id",  v_dvtrk_id[iTrk]);
      hitDVTrk->setDetail<float>("dvtrk_pt",  v_dvtrk_pt[iTrk]);
      hitDVTrk->setDetail<float>("dvtrk_eta", v_dvtrk_eta[iTrk]);
      hitDVTrk->setDetail<float>("dvtrk_phi", v_dvtrk_phi[iTrk]);
      hitDVTrk->setDetail<int>  ("dvtrk_n_hits_innermost", v_dvtrk_n_hits_innermost[iTrk]);
      hitDVTrk->setDetail<int>  ("dvtrk_n_hits_pix", v_dvtrk_n_hits_pix[iTrk]); 
      hitDVTrk->setDetail<int>  ("dvtrk_n_hits_sct", v_dvtrk_n_hits_sct[iTrk]);
      hitDVTrk->setDetail<float>("dvtrk_a0beam",     v_dvtrk_a0beam[iTrk]); 
   }

   // space points
   const float SPCUT_DELTA_R_TO_SEED = 0.6;
   for(unsigned int iSp=0; iSp<v_sp_eta.size(); ++iSp) {
      float sp_eta = v_sp_eta[iSp];
      float sp_phi = v_sp_phi[iSp];
      if( m_doHitDV_Seeding ) {
	 bool isNearSeed = false;
	 for (unsigned int iSeed=0; iSeed<v_seeds_eta.size(); ++iSeed) {
	    float seed_eta = v_seeds_eta[iSeed];
	    float seed_phi = v_seeds_phi[iSeed];
	    float dR = deltaR(sp_eta,sp_phi,seed_eta,seed_phi);
	    if( dR <= SPCUT_DELTA_R_TO_SEED ) { isNearSeed = true; break; }
	 }
	 if( ! isNearSeed ) continue;
      }
      xAOD::TrigComposite *hitDVSP = new xAOD::TrigComposite();
      hitDVSP->makePrivateStore();
      hitDVSPContainer->push_back(hitDVSP);
      hitDVSP->setDetail<float>("sp_eta",       v_sp_eta[iSp]);
      hitDVSP->setDetail<float>("sp_r",         v_sp_r[iSp]);
      hitDVSP->setDetail<float>("sp_phi",       v_sp_phi[iSp]);
      hitDVSP->setDetail<int>  ("sp_layer",     v_sp_layer[iSp]);
      hitDVSP->setDetail<int>  ("sp_isPix",     v_sp_isPix[iSp]);
      hitDVSP->setDetail<int>  ("sp_isSct",     v_sp_isSct[iSp]);
      hitDVSP->setDetail<int>  ("sp_usedTrkId", v_sp_usedTrkId[iSp]);
   }

   // record
   ATH_CHECK(hitDVTrkHandle.record(std::move(hitDVTrkContainer), std::move(hitDVTrkContainerAux)));
   ATH_CHECK(hitDVSPHandle.record(std::move(hitDVSPContainer), std::move(hitDVSPContainerAux)));

   return StatusCode::SUCCESS;
}

StatusCode TrigFastTrackFinder::findSPSeeds( const std::vector<float>& v_sp_eta, const std::vector<float>& v_sp_phi,
					     const std::vector<int>& v_sp_layer, const std::vector<int>& v_sp_usedTrkId,
					     std::vector<float>& seeds_eta, std::vector<float>& seeds_phi ) const
{
   seeds_eta.clear();
   seeds_phi.clear();

   const int   NBINS_ETA = 50;
   const float ETA_MIN   = -2.5; 
   const float ETA_MAX   =  2.5;

   const int   NBINS_PHI = 80;
   const float PHI_MIN   = -4.0; 
   const float PHI_MAX   =  4.0;

   std::string hname;

   hname = "ly6_h2_nsp";
   TH2F*    ly6_h2_nsp = new TH2F(hname.c_str(),hname.c_str(),NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);
   hname = "ly7_h2_nsp";
   TH2F*    ly7_h2_nsp = new TH2F(hname.c_str(),hname.c_str(),NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);

   hname = "ly6_h2_nsp_notrk";
   TH2F*    ly6_h2_nsp_notrk = new TH2F(hname.c_str(),hname.c_str(),NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);
   hname = "ly7_h2_nsp_notrk";
   TH2F*    ly7_h2_nsp_notrk = new TH2F(hname.c_str(),hname.c_str(),NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);

   for(unsigned int iSeed=0; iSeed<v_sp_eta.size(); ++iSeed) {

      int sp_layer = v_sp_layer[iSeed];
      float sp_eta = v_sp_eta[iSeed];
      int ilayer = getSPLayer(sp_layer,sp_eta);
      if( ilayer<6 ) continue;

      int sp_trkid = v_sp_usedTrkId[iSeed];
      bool isUsedByTrk = (sp_trkid != -1);
      float sp_phi = v_sp_phi[iSeed];

      bool fill_out_of_pi = false;
      float sp_phi2 = 0;
      if( sp_phi < 0 ) {
	 sp_phi2 = 2*TMath::Pi() + sp_phi;
	 if( sp_phi2 < PHI_MAX ) fill_out_of_pi = true;
      }
      else {
	 sp_phi2 = -2*TMath::Pi() + sp_phi;
	 if( PHI_MIN < sp_phi2 ) fill_out_of_pi = true;
      }
      if( ilayer==6 ) {
	                      ly6_h2_nsp->Fill(sp_eta,sp_phi);
	 if( fill_out_of_pi ) ly6_h2_nsp->Fill(sp_eta,sp_phi2);
	 if( ! isUsedByTrk )                  ly6_h2_nsp_notrk->Fill(sp_eta,sp_phi);
	 if( ! isUsedByTrk && fill_out_of_pi) ly6_h2_nsp_notrk->Fill(sp_eta,sp_phi2);
      }
      if( ilayer==7 ) {
	                      ly7_h2_nsp->Fill(sp_eta,sp_phi);
	 if( fill_out_of_pi ) ly7_h2_nsp->Fill(sp_eta,sp_phi2);
	 if( ! isUsedByTrk )                  ly7_h2_nsp_notrk->Fill(sp_eta,sp_phi);
	 if( ! isUsedByTrk && fill_out_of_pi) ly7_h2_nsp_notrk->Fill(sp_eta,sp_phi2);
      }
   }

   ATH_MSG_DEBUG("looking for ly6/ly6 doublet seeds");

   // (idx, sort/weight, eta, phi)
   std::vector<std::tuple<int,float,float,float>> QT;

   for(int ly6_ieta=1; ly6_ieta<=NBINS_ETA; ly6_ieta++) {
      float ly6_eta = (ly6_h2_nsp->GetXaxis()->GetBinLowEdge(ly6_ieta) + ly6_h2_nsp->GetXaxis()->GetBinUpEdge(ly6_ieta))/2.0;
      for(int ly6_iphi=1; ly6_iphi<=NBINS_PHI; ly6_iphi++) {
	 float ly6_phi = (ly6_h2_nsp->GetYaxis()->GetBinLowEdge(ly6_iphi) + ly6_h2_nsp->GetYaxis()->GetBinUpEdge(ly6_iphi))/2.0;

	 float ly6_nsp       = ly6_h2_nsp      ->GetBinContent(ly6_ieta,ly6_iphi);
	 float ly6_nsp_notrk = ly6_h2_nsp_notrk->GetBinContent(ly6_ieta,ly6_iphi);
	 float ly6_frac      = ( ly6_nsp > 0 ) ? ly6_nsp_notrk / ly6_nsp : 0;
	 if( ly6_nsp < 10 || ly6_frac < 0.85 ) continue;

	 float ly7_frac_max = 0;
	 float ly7_eta_max  = 0;
	 float ly7_phi_max  = 0;
	 for(int ly7_ieta=std::max(1,ly6_ieta-1); ly7_ieta<std::min(NBINS_ETA,ly6_ieta+1); ly7_ieta++) {
	    for(int ly7_iphi=std::max(1,ly6_iphi-1); ly7_iphi<=std::min(NBINS_PHI,ly6_iphi+1); ly7_iphi++) {
	       float ly7_nsp       = ly7_h2_nsp      ->GetBinContent(ly7_ieta,ly7_iphi);
	       float ly7_nsp_notrk = ly7_h2_nsp_notrk->GetBinContent(ly7_ieta,ly7_iphi);
	       float ly7_frac      = ( ly7_nsp > 0 ) ? ly7_nsp_notrk / ly7_nsp : 0;
	       if( ly7_nsp < 10 ) continue;
	       if( ly7_frac > ly7_frac_max ) {
		  ly7_frac_max = ly7_frac;
		  ly7_eta_max  = (ly7_h2_nsp->GetXaxis()->GetBinLowEdge(ly7_ieta) + ly7_h2_nsp->GetXaxis()->GetBinUpEdge(ly7_ieta))/2.0;
		  ly7_phi_max  = (ly7_h2_nsp->GetXaxis()->GetBinLowEdge(ly7_iphi) + ly7_h2_nsp->GetXaxis()->GetBinUpEdge(ly7_iphi))/2.0;
	       }
	    }
	 }
	 if( ly7_frac_max < 0.85 ) continue;
	 //
	 float wsum = ly6_frac + ly7_frac_max;
	 float weta = (ly6_eta*ly6_frac + ly7_eta_max*ly7_frac_max) / wsum;
	 float wphi = (ly6_phi*ly6_frac + ly7_phi_max*ly7_frac_max) / wsum;
	 float w = wsum / 2.0;
	 QT.push_back(std::make_tuple(-1,w,weta,wphi));
      }
   }
   delete ly6_h2_nsp;
   delete ly7_h2_nsp;
   delete ly6_h2_nsp_notrk;
   delete ly7_h2_nsp_notrk;
   ATH_MSG_DEBUG("nr of ly6/ly7 doublet candidate seeds=" << QT.size() << ", doing clustering...");

   // sort
   std::sort(QT.begin(), QT.end(),
	     [](const std::tuple<int,float,float,float>& lhs, const std::tuple<int,float,float,float>& rhs) {
		return std::get<1>(lhs) > std::get<1>(rhs); } );
   
   // clustering
   const double CLUSTCUT_DIST      = 0.2;
   const double CLUSTCUT_SEED_FRAC = 0.9;

   std::vector<float> seeds_wsum;

   for(unsigned int i=0; i<QT.size(); i++) {
      float phi  = std::get<3>(QT[i]);
      float eta  = std::get<2>(QT[i]);
      float w    = std::get<1>(QT[i]);
      if(i==0) {
	 seeds_eta.push_back(w*eta); seeds_phi.push_back(w*phi);
	 seeds_wsum.push_back(w);
	 continue;
      }
      const int IDX_INITIAL = 100;
      float dist_min = 100.0;
      int idx_min     = IDX_INITIAL;
      for(unsigned j=0; j<seeds_eta.size(); j++) {
	 float ceta = seeds_eta[j]/seeds_wsum[j];
	 float cphi = seeds_phi[j]/seeds_wsum[j];
	 // intentionally calculate in this way as phi is defined beyond -Pi/Pi (no boundary)
	 float deta = std::fabs(ceta-eta);
	 float dphi = std::fabs(cphi-phi);
	 float dist = std::sqrt(dphi*dphi+deta*deta);
	 if( dist < dist_min ) {
	    dist_min = dist;
	    idx_min  = j; 
	 }
      }
      int match_idx = IDX_INITIAL;
      if( idx_min != IDX_INITIAL ) {
	 if( dist_min < CLUSTCUT_DIST ) { match_idx = idx_min; }
      }
      if( match_idx == IDX_INITIAL ) {
	 if( w > CLUSTCUT_SEED_FRAC && dist_min > CLUSTCUT_DIST ) {
	    seeds_eta.push_back(w*eta); seeds_phi.push_back(w*phi);
	    seeds_wsum.push_back(w);
	 }
	 continue;
      }
      float new_eta   = seeds_eta[match_idx]  + w*eta;
      float new_phi   = seeds_phi[match_idx]  + w*phi;
      float new_wsum  = seeds_wsum[match_idx] + w;
      seeds_eta[match_idx]   = new_eta;
      seeds_phi[match_idx]   = new_phi;
      seeds_wsum[match_idx]  = new_wsum;
   }
   QT.clear();
   for(unsigned int i=0; i<seeds_eta.size(); i++) {
      float eta = seeds_eta[i] / seeds_wsum[i];
      float phi = seeds_phi[i] / seeds_wsum[i];
      seeds_eta[i] = eta;
      seeds_phi[i] = phi;
      if( phi < -TMath::Pi() ) phi =  2*TMath::Pi() + phi;
      if( phi >  TMath::Pi() ) phi = -2*TMath::Pi() + phi;
      seeds_phi[i] = phi;
   }
   ATH_MSG_DEBUG("after clustering, nr of seeds = " << seeds_eta.size());

   // delete overlap (can happen at phi=-Pi/Pi bounadry)
   std::vector<unsigned int> idx_to_delete;
   for(unsigned int i=0; i<seeds_eta.size(); i++) {
      if( std::find(idx_to_delete.begin(),idx_to_delete.end(),i) != idx_to_delete.end() ) continue;
      float eta_i = seeds_eta[i];
      float phi_i = seeds_phi[i];
      for(unsigned int j=i+1; j<seeds_eta.size(); j++) {
	 if( std::find(idx_to_delete.begin(),idx_to_delete.end(),j) != idx_to_delete.end() ) continue;
	 float eta_j = seeds_eta[j];
	 float phi_j = seeds_phi[j];
	 float dr = deltaR(eta_i,phi_i,eta_j,phi_j);
	 if( dr < CLUSTCUT_DIST ) idx_to_delete.push_back(j);
      }
   }
   ATH_MSG_DEBUG("nr of duplicated seeds to be removed = " << idx_to_delete.size());
   if( idx_to_delete.size() > 0 ) {
      std::sort(idx_to_delete.begin(),idx_to_delete.end());
      for(unsigned int j=idx_to_delete.size(); j>0; j--) {
	 unsigned int idx = idx_to_delete[j-1];
	 seeds_eta.erase(seeds_eta.begin()+idx);
	 seeds_phi.erase(seeds_phi.begin()+idx);
      }
   }

   ATH_MSG_DEBUG("nr of ly6/ly7 seeds=" << seeds_eta.size());

   // return
   return StatusCode::SUCCESS;
}

float TrigFastTrackFinder::deltaR(float eta_1, float phi_1, float eta_2, float phi_2) const {
   float dPhi = phi_1 - phi_2;
   if (dPhi < -TMath::Pi()) dPhi += 2*TMath::Pi();
   if (dPhi >  TMath::Pi()) dPhi -= 2*TMath::Pi();
   float dEta = eta_1 - eta_2;
   return sqrt(dPhi*dPhi+dEta*dEta);
}

int TrigFastTrackFinder::getSPLayer(int layer, float eta) const
{
   float abseta = std::fabs(eta);

   // Pixel barrel or SCT barrel
   if( 0<=layer && layer <=7 ) {
      ATH_MSG_DEBUG("layer=" << layer << ", eta=" << abseta);
      return layer;
   }

   int base = 0;

   // 
   const float PixBR6limit = 1.29612;
   const float PixBR5limit = 1.45204;
   const float PixBR4limit = 1.64909;
   const float PixBR3limit = 1.90036;
   const float PixBR2limit = 2.2146;

   // Pixel Endcap #1
   base = 8;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Pix EC1, eta=" << abseta);
      if( abseta > PixBR2limit ) return 2;
      return 3;
   }

   // Pixel Endcap #2
   base = 9;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Pix EC2, eta=" << abseta);
      if( abseta > PixBR2limit ) return 2;
      return 3;
   }

   // Pixel Endcap #3
   base = 10;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Pix EC3, eta=" << abseta);
      return 3;
   }

   // SCT Endcap #1
   base = 11;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC1, eta=" << abseta);
      if( abseta < PixBR6limit )      return 7;
      else if( abseta < PixBR5limit ) return 6;
      return 5;
   }

   // SCT Endcap #2
   base = 12;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC2, eta=" << abseta);
      if( abseta < PixBR5limit )      return 7;
      else if( abseta < PixBR4limit ) return 6;
      return 4;
   }

   // SCT Endcap #3
   base = 13;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC3, eta=" << abseta);
      if( abseta < PixBR4limit ) return 7;
      return 5;
   }

   // SCT Endcap #4
   base = 14;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC4, eta=" << abseta);
      if( abseta < PixBR4limit ) return 6;
      else if( abseta < PixBR3limit ) return 6;
      return 4;
   }

   // SCT Endcap #5
   base = 15;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC5, eta=" << abseta);
      if( abseta < PixBR3limit ) return 7;
      return 5;
   }

   // SCT Endcap #6
   base = 16;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC6, eta=" << abseta);
      if( abseta < PixBR3limit ) return 6;
      return 4;
   }

   // SCT Endcap #7
   base = 17;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC7, eta=" << abseta);
      if( abseta < PixBR3limit ) return 7;
      return 5;
   }

   // SCT Endcap #8
   base = 18;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC8, eta=" << abseta);
      if( abseta < PixBR3limit ) return 7;
      return 6;
   }

   // SCT Endcap #9
   base = 19;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_DEBUG("Sct EC9, eta=" << abseta);
      return 7;
   }

   return 0;
}

StatusCode TrigFastTrackFinder::finddEdxTrk(const EventContext& ctx, const TrackCollection& outputTracks) const
{
   auto dEdxTrkContainer    = std::make_unique<xAOD::TrigCompositeContainer>();
   auto dEdxTrkContainerAux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
   dEdxTrkContainer->setStore(dEdxTrkContainerAux.get());
   SG::WriteHandle<xAOD::TrigCompositeContainer> dEdxTrkHandle(m_dEdxTrkKey, ctx);

   auto dEdxHitContainer    = std::make_unique<xAOD::TrigCompositeContainer>();
   auto dEdxHitContainerAux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
   dEdxHitContainer->setStore(dEdxHitContainerAux.get());
   SG::WriteHandle<xAOD::TrigCompositeContainer> dEdxHitHandle(m_dEdxHitKey, ctx);

   std::vector<float> mnt_dedx;
   std::vector<int>   mnt_dedx_nusedhits;
   auto mon_dedx           = Monitored::Collection("trk_dedx",           mnt_dedx);
   auto mon_dedx_nusedhits = Monitored::Collection("trk_dedx_nusedhits", mnt_dedx_nusedhits);
   auto mondEdx            = Monitored::Group(m_monTool, mon_dedx, mon_dedx_nusedhits);

   int i_track=0;

   ATH_MSG_DEBUG("========== in finddEdxTrk ==========");

   const float  TRKCUT_A0BEAM       = 2.5;
   const int    TRKCUT_N_HITS_INNER = 1;
   const int    TRKCUT_N_HITS_PIX   = 2;
   const int    TRKCUT_N_HITS       = 4;

   const float  TRKCUT_PTGEV_LOOSE  =  3.0;
   const float  TRKCUT_PTGEV_TIGHT  = 10.0;
   const float  TRKCUT_DEDX_LOOSE   =  1.25;
   const float  TRKCUT_DEDX_TIGHT   =  1.55;

   for (auto track : outputTracks) {

      ATH_MSG_DEBUG("+++++++ i_track: " << i_track << " +++++++");

      i_track++;
      if ( ! track->perigeeParameters() ) continue;
      if ( ! track->trackSummary() )      continue;
      float n_hits_innermost = 0;  float n_hits_next_to_innermost = 0; float n_hits_inner = 0; float n_hits_pix = 0; float n_hits_sct = 0;
      n_hits_innermost = (float)track->trackSummary()->get(Trk::SummaryType::numberOfInnermostPixelLayerHits);
      n_hits_next_to_innermost = (float)track->trackSummary()->get(Trk::SummaryType::numberOfNextToInnermostPixelLayerHits);
      n_hits_inner = n_hits_innermost + n_hits_next_to_innermost;
      n_hits_pix = (float)track->trackSummary()->get(Trk::SummaryType::numberOfPixelHits);
      n_hits_sct = (float)track->trackSummary()->get(Trk::SummaryType::numberOfSCTHits);
      if( n_hits_inner < TRKCUT_N_HITS_INNER )      continue;
      if( n_hits_pix < TRKCUT_N_HITS_PIX )          continue;
      if( (n_hits_pix+n_hits_sct) < TRKCUT_N_HITS ) continue;
      float theta = track->perigeeParameters()->parameters()[Trk::theta];
      float pt    = std::abs(1./track->perigeeParameters()->parameters()[Trk::qOverP]) * sin(theta);
      float ptgev = pt / 1000.0;
      if( ptgev < TRKCUT_PTGEV_LOOSE ) continue;
      float a0   = track->perigeeParameters()->parameters()[Trk::d0];
      float phi0 = track->perigeeParameters()->parameters()[Trk::phi0];
      float shift_x = 0; float shift_y = 0;
      if( m_useBeamSpot ) getBeamSpot(shift_x, shift_y, ctx);
      float a0beam = a0 + shift_x*sin(phi0)-shift_y*cos(phi0);
      if( std::abs(a0beam) > TRKCUT_A0BEAM ) continue;

      ATH_MSG_DEBUG("calculate dEdx -->");
      int pixelhits=0; int n_usedhits=0;
      std::vector<float> v_pixhit_dedx; std::vector<float> v_pixhit_tot; std::vector<float> v_pixhit_trkchi2; std::vector<float> v_pixhit_trkndof;
      std::vector<int> v_pixhit_iblovfl; std::vector<int> v_pixhit_loc; std::vector<int> v_pixhit_layer;
      float dedx = dEdx(track,pixelhits,n_usedhits,v_pixhit_dedx,v_pixhit_tot,v_pixhit_trkchi2,v_pixhit_trkndof,
			v_pixhit_iblovfl,v_pixhit_loc,v_pixhit_layer);
      ATH_MSG_DEBUG("--> dedx = " << dedx);

      mnt_dedx.push_back(dedx);
      mnt_dedx_nusedhits.push_back(n_usedhits);

      bool hpt = (ptgev >= TRKCUT_PTGEV_TIGHT && dedx >= TRKCUT_DEDX_LOOSE);
      bool lpt = (ptgev >= TRKCUT_PTGEV_LOOSE && dedx >= TRKCUT_DEDX_TIGHT);
      if( ! hpt && ! lpt ) continue;

      xAOD::TrigComposite *dEdxTrk = new xAOD::TrigComposite();
      dEdxTrk->makePrivateStore();
      dEdxTrkContainer->push_back(dEdxTrk);
      dEdxTrk->setDetail<int>  ("trk_id",   i_track);
      dEdxTrk->setDetail<float>("trk_pt",   pt);
      float eta = -log(tan(0.5*theta));
      dEdxTrk->setDetail<float>("trk_eta",  eta);
      dEdxTrk->setDetail<float>("trk_phi",  phi0);
      dEdxTrk->setDetail<float>("trk_dedx", dedx);
      dEdxTrk->setDetail<int>  ("trk_dedx_n_usedhits",  n_usedhits);
      dEdxTrk->setDetail<float>("trk_a0beam",           a0beam);
      dEdxTrk->setDetail<float>("trk_n_hits_innermost", n_hits_innermost);
      dEdxTrk->setDetail<float>("trk_n_hits_inner",     n_hits_inner);
      dEdxTrk->setDetail<float>("trk_n_hits_pix",       n_hits_pix);
      dEdxTrk->setDetail<float>("trk_n_hits_sct",       n_hits_sct);

      for(unsigned int i=0; i<v_pixhit_dedx.size(); i++) {
	 xAOD::TrigComposite *dEdxHit = new xAOD::TrigComposite();
	 dEdxHit->makePrivateStore();
	 dEdxHitContainer->push_back(dEdxHit);
	 dEdxHit->setDetail<int>  ("hit_trkid",   i_track);
	 dEdxHit->setDetail<float>("hit_dedx",    v_pixhit_dedx[i]);
	 dEdxHit->setDetail<float>("hit_tot",     v_pixhit_tot[i]);
	 dEdxHit->setDetail<float>("hit_trkchi2", v_pixhit_trkchi2[i]);
	 dEdxHit->setDetail<float>("hit_trkndof", v_pixhit_trkndof[i]);
	 dEdxHit->setDetail<int>  ("hit_iblovfl", v_pixhit_iblovfl[i]);
	 dEdxHit->setDetail<int>  ("hit_loc",     v_pixhit_loc[i]);
	 dEdxHit->setDetail<int>  ("hit_layer",   v_pixhit_layer[i]);
      }
   }

   ATH_CHECK(dEdxTrkHandle.record(std::move(dEdxTrkContainer), std::move(dEdxTrkContainerAux)));
   ATH_CHECK(dEdxHitHandle.record(std::move(dEdxHitContainer), std::move(dEdxHitContainerAux)));
   return StatusCode::SUCCESS;
}

float TrigFastTrackFinder::dEdx(const Trk::Track* track, int& pixelhits, int& n_usedhits,
				std::vector<float>& v_pixhit_dedx,    std::vector<float>& v_pixhit_tot,
				std::vector<float>& v_pixhit_trkchi2, std::vector<float>& v_pixhit_trkndof,
				std::vector<int>&   v_pixhit_iblovfl, std::vector<int>&   v_pixhit_loc,     std::vector<int>& v_pixhit_layer) const
{
   const float Pixel_sensorthickness=.025;      // 250 microns Pixel Planars
   const float IBL_3D_sensorthickness=.023;     // 230 microns IBL 3D
   const float IBL_PLANAR_sensorthickness=.020; // 200 microns IBL Planars

   const float energyPair = 3.68e-6; // Energy in MeV to create an electron-hole pair in silicon
   const float sidensity = 2.329;    // silicon density in g cm^-3

   float conversion_factor=energyPair/sidensity;

   // Loop over pixel hits on track, and calculate dE/dx:

   pixelhits  = 0;
   n_usedhits = 0;

   v_pixhit_dedx.clear();
   v_pixhit_tot.clear();
   v_pixhit_trkchi2.clear();
   v_pixhit_trkndof.clear();
   v_pixhit_iblovfl.clear();
   v_pixhit_loc.clear();
   v_pixhit_layer.clear();

   const int PIXLOC_IBL_PL = 0;
   const int PIXLOC_IBL_3D = 1;
   const int PIXLOC_PIX_LY = 2;
   const int PIXLOC_PIX_EC = 3;
   const int PIXLOC_IBL_UNKNOWN = 4;
   const int PIXLOC_PIX_UNKNOWN = 5;

   std::multimap<float,int> dEdxMap;
   int   n_usedIBLOverflowHits=0;
   float dEdxValue = 0;

   // Check for track states:
   const DataVector<const Trk::TrackStateOnSurface>* recoTrackStates = track->trackStateOnSurfaces();
   if (recoTrackStates) {

      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIter    = recoTrackStates->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIterEnd = recoTrackStates->end();

      int i_tsos=0;

      // Loop over track states on surfaces (i.e. generalized hits):
      for ( ; tsosIter != tsosIterEnd; ++tsosIter) {

	 ATH_MSG_DEBUG("-------- TSoS: " << i_tsos++  << " --------");

	 const Trk::MeasurementBase *measurement = (*tsosIter)->measurementOnTrack();
	 if ( measurement == nullptr ) {
	    ATH_MSG_DEBUG("no measurement on this TSoS, skip it");
	    continue;
	 }
	 const Trk::TrackParameters* tp = (*tsosIter)->trackParameters();
	 if( tp == nullptr ) {
	    ATH_MSG_DEBUG("no trackParameters() to this TSoS, skip it");
	    continue;
	 }
	 const InDet::PixelClusterOnTrack *pixclus = dynamic_cast<const InDet::PixelClusterOnTrack*>(measurement);
	 if ( pixclus == nullptr ) {
	    ATH_MSG_DEBUG("this TSoS is not Pixel, skip it");
	    continue;
	 }
	 const InDet::PixelCluster* prd = pixclus->prepRawData();
	 if( prd == nullptr ) {
	    ATH_MSG_DEBUG("no PrepRawData(), skip it");
	    continue;
	 }

	 float dotProd  = tp->momentum().dot(tp->associatedSurface().normal());
	 float cosalpha = std::abs(dotProd/tp->momentum().mag());
	 ATH_MSG_DEBUG("dotProd / cosalpha = " << dotProd  << " / " << cosalpha);
	 if (std::abs(cosalpha)<.16) continue;

	 const std::vector<int>& v_tots = prd->totList();
	 float charge = prd->totalCharge();
	 float tot    = prd->totalToT();
	 ATH_MSG_DEBUG("charge / ToT = " << charge << " / " << tot);
	 charge *= cosalpha; // path length correction

	 double locx = pixclus->localParameters()[Trk::locX];
	 double locy = pixclus->localParameters()[Trk::locY];
	 int    bec        = m_pixelId->barrel_ec(pixclus->identify());
	 int    layer      = m_pixelId->layer_disk(pixclus->identify());
	 int    eta_module = m_pixelId->eta_module(pixclus->identify()); //check eta module to select thickness

	 float chi2  = 0.;
	 float ndof  = 0.;
	 const Trk::FitQualityOnSurface* fq = (*tsosIter)->fitQualityOnSurface();
	 if( fq != nullptr ) {
	    chi2  = fq->chiSquared();
	    ndof  = fq->doubleNumberDoF();
	 }

	 int iblOverflow=0; // keep track if this is IBL overflow
	 float thickness=0;
	 int loc=-1;

	 if ( (bec==0) and (layer==0) ){ // IBL
	    const float overflowIBLToT = 16; // m_overflowIBLToT = m_offlineCalibSvc->getIBLToToverflow();
	    for (int pixToT : v_tots) {
	       if (pixToT >= overflowIBLToT) {
		  iblOverflow = 1; // overflow pixel hit -- flag cluster
		  break; //no need to check other hits of this cluster
	       }
	    }
	    if(iblOverflow==1) ATH_MSG_DEBUG("IBL overflow");

	    if(((eta_module>=-10 && eta_module<=-7)||(eta_module>=6 && eta_module<=9)) && (std::abs(locy)<10. && (locx>-8.33 && locx <8.3)) ){ // IBL 3D
	       thickness = IBL_3D_sensorthickness;
	       loc = PIXLOC_IBL_3D;
	    }
	    else if((eta_module>=-6 && eta_module<=5) && (std::abs(locy)<20. &&( locx >-8.33 && locx <8.3 )) ){ // IBL planer
	       thickness = IBL_PLANAR_sensorthickness;
	       loc = PIXLOC_IBL_PL;
	    }
	    else {
	       ATH_MSG_DEBUG("unknown IBL module");
	       loc = PIXLOC_IBL_UNKNOWN;
	    }
	 }
	 else if(bec==0 && std::abs(locy)<30. &&  (( locx > -8.20 && locx < -0.60 ) || ( locx > 0.50 && locx < 8.10 ) ) ){ //PIXEL layer
	    thickness = Pixel_sensorthickness;
	    loc = PIXLOC_PIX_LY;
	 }
	 else if(std::abs(bec) == 2 && std::abs(locy)<30. && ( ( locx > -8.15 && locx < -0.55 ) || ( locx > 0.55 && locx < 8.15 ) ) ) { //PIXEL endcap
	    thickness = Pixel_sensorthickness;
	    loc = PIXLOC_PIX_EC;
	 }
	 else {
	    ATH_MSG_DEBUG("unknown Pixel module");
	    loc = PIXLOC_IBL_UNKNOWN;
	 }

	 dEdxValue = 0;
	 if( loc != PIXLOC_IBL_UNKNOWN && loc != PIXLOC_PIX_UNKNOWN ) {
	    dEdxValue = charge*conversion_factor/thickness;
	    dEdxMap.insert(std::pair<float,int>(dEdxValue, iblOverflow));
	    pixelhits++;
	    if(iblOverflow==1)n_usedIBLOverflowHits++;
	 }
	 ATH_MSG_DEBUG("dEdx=" << dEdxValue);
	 v_pixhit_dedx.push_back(dEdxValue); v_pixhit_tot.push_back(tot); 
	 v_pixhit_trkchi2.push_back(chi2); v_pixhit_trkndof.push_back(ndof);
	 v_pixhit_iblovfl.push_back(iblOverflow); v_pixhit_loc.push_back(loc); v_pixhit_layer.push_back(layer);
      }
   }

   // Now calculate dEdx, multimap is already sorted in ascending order
   // float averageCharge=-1;

   float averagedEdx=0.;
   int IBLOverflow=0;

   int i_map=0;

   for (std::pair<float,int> itdEdx : dEdxMap) {
      ATH_MSG_DEBUG("++++++++ i_map: " << i_map++  << " ++++++++");
      if(itdEdx.second==0){
	 ATH_MSG_DEBUG("usedhits, dEdx=" << itdEdx.first);
         averagedEdx += itdEdx.first;
         n_usedhits++;
      }
      if(itdEdx.second > 0){ 
	 ATH_MSG_DEBUG("IBLOverflow");
         IBLOverflow++;
      }
      // break, skipping last or the two last elements depending on total measurements
      if (((int)pixelhits >= 5) and ((int)n_usedhits >= (int)pixelhits-2)) {
	 ATH_MSG_DEBUG("break, skipping last or two last elements");
	 break;
      }

      // break, IBL Overflow case pixelhits==3 and 4
      if((int)IBLOverflow>0 and ((int)pixelhits==3) and (int)n_usedhits==1) {
	 ATH_MSG_DEBUG("break, IBL overflow case, pixel hits=3");
	 break;
      }
      if((int)IBLOverflow>0 and ((int)pixelhits==4) and (int)n_usedhits==2) {
	 ATH_MSG_DEBUG("break, IBL overflow case, pixel hits=4");
	 break;
      }

      if (((int)pixelhits > 1) and ((int)n_usedhits >=(int)pixelhits-1)) {
	 ATH_MSG_DEBUG("break, skipping last??");
	 break; 
      }

      if((int)IBLOverflow>0 and (int)pixelhits==1){ // only IBL in overflow
	 ATH_MSG_DEBUG("break, only IBL in overflow");
         averagedEdx=itdEdx.first;
         break;
      }
   }

   if (n_usedhits > 0 or (n_usedhits==0 and(int)IBLOverflow>0 and (int)pixelhits==1)) {
      if(n_usedhits > 0) averagedEdx = averagedEdx / n_usedhits;
      //if(n_usedhits == 0 and (int)IBLOverflow > 0 and (int)pixelhits == 1) averagedEdx = averagedEdx;  //no-op
      ATH_MSG_DEBUG("=====> averaged dEdx = " << averagedEdx << " =====>");;
      ATH_MSG_DEBUG("   +++ Used hits: " << n_usedhits << ", IBL overflows: " << IBLOverflow );;
      ATH_MSG_DEBUG("   +++ Original number of measurements = " << pixelhits << " (map size = " << dEdxMap.size() << ") ");
      return averagedEdx;  
   }

   // -- false return
   ATH_MSG_DEBUG("dEdx not calculated, return 0");
   return 0.;
}
