
#include "src/SiSpacePointsSeedMaker.h"
#include "GaudiKernel/EventContext.h"

#include "Acts/Definitions/Units.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

#include "InDetPrepRawData/SiCluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "ActsTrkEvent/SpacePoint.h"
#include "ActsTrkEvent/Seed.h"
#include "SiSpacePoint/SCT_SpacePoint.h"
#include "SiSpacePoint/PixelSpacePoint.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"

//for validation
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "CxxUtils/checker_macros.h"


namespace ActsTrk {

  SiSpacePointsSeedMaker::SiSpacePointsSeedMaker(const std::string &t, const std::string &n, const IInterface *p)
    : base_class(t, n, p)
  {}

  StatusCode SiSpacePointsSeedMaker::initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() << "..." );

    ATH_MSG_DEBUG( "Properties Summary:" );
    ATH_MSG_DEBUG( "   " << m_pixel );
    ATH_MSG_DEBUG( "   " << m_strip );
    ATH_MSG_DEBUG( "   " << m_useOverlap );
    ATH_MSG_DEBUG( "   " << m_writeNtuple );

    if (not m_pixel and not m_strip) {
      ATH_MSG_ERROR("Activate seeding on at least one between Pixel and Strip space point collections!");
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_spacepointsPixel.initialize(m_pixel and m_doSpacePointConversion) );
    ATH_CHECK( m_spacepointsStrip.initialize(m_strip and m_doSpacePointConversion) );
    ATH_CHECK( m_spacepointsOverlap.initialize(m_strip and m_useOverlap and m_doSpacePointConversion) );

    ATH_CHECK( m_actsSpacepointsPixel.initialize(m_pixel and not m_doSpacePointConversion) );
    ATH_CHECK( m_actsSpacepointsStrip.initialize(m_strip and not m_doSpacePointConversion) );
    ATH_CHECK( m_actsSpacepointsOverlap.initialize(m_strip and m_useOverlap and not m_doSpacePointConversion) );

    ATH_CHECK( m_pixelClusterContainerKey.initialize(m_pixel and not m_doSpacePointConversion) );
    ATH_CHECK( m_stripClusterContainerKey.initialize(m_strip and not m_doSpacePointConversion) );

    if (m_pixel)
      ATH_CHECK( m_seedsToolPixel.retrieve() );

    if (m_strip)
      ATH_CHECK( m_seedsToolStrip.retrieve() );

    ATH_CHECK( m_prdToTrackMap.initialize(SG::AllowEmpty) );

    ATH_CHECK( m_beamSpotKey.initialize() );
    ATH_CHECK( m_fieldCondObjInputKey.initialize() );

    // Validation
    if (m_writeNtuple) 
      ATH_CHECK( InitTree() );

    return StatusCode::SUCCESS;
  }

  // ===================================================================== //

  StatusCode
  SiSpacePointsSeedMaker::InitTree()
  {
    ATH_CHECK( service("THistSvc", m_thistSvc)  );
    std::string tree_name = std::string("SeedTree_") + name();
    std::replace( tree_name.begin(), tree_name.end(), '.', '_' );

    m_outputTree = new TTree( tree_name.c_str() , "ActsSeedMakerValTool");

    m_outputTree->Branch("eventNumber",    &m_eventNumber,"eventNumber/L");
    m_outputTree->Branch("d0",             &m_d0);
    m_outputTree->Branch("z0",             &m_z0);
    m_outputTree->Branch("pt",             &m_pt);
    m_outputTree->Branch("eta",            &m_eta);
    m_outputTree->Branch("x1",             &m_x1);
    m_outputTree->Branch("x2",             &m_x2);
    m_outputTree->Branch("x3",             &m_x3);
    m_outputTree->Branch("y1",             &m_y1);
    m_outputTree->Branch("y2",             &m_y2);
    m_outputTree->Branch("y3",             &m_y3);
    m_outputTree->Branch("z1",             &m_z1);
    m_outputTree->Branch("z2",             &m_z2);
    m_outputTree->Branch("z3",             &m_z3);
    m_outputTree->Branch("r1",             &m_r1);
    m_outputTree->Branch("r2",             &m_r2);
    m_outputTree->Branch("r3",             &m_r3);
    m_outputTree->Branch("quality",        &m_quality);
    m_outputTree->Branch("seedType",       &m_type);
    m_outputTree->Branch("givesTrack",     &m_givesTrack);
    m_outputTree->Branch("dzdr_b",         &m_dzdr_b);
    m_outputTree->Branch("dzdr_t",         &m_dzdr_t);
    m_outputTree->Branch("track_pt",       &m_trackPt);
    m_outputTree->Branch("track_eta",      &m_trackEta);

    std::string full_tree_name = "/" + m_treeFolder + "/" + tree_name;
    ATH_CHECK( m_thistSvc->regTree( full_tree_name.c_str(), m_outputTree ) );

    return StatusCode::SUCCESS;
  }

  void
  SiSpacePointsSeedMaker::newSpacePoint(InDet::SiSpacePointsSeedMakerEventData& data,
					    const Trk::SpacePoint* const& sp) const
  {
    float r[15];

    auto globPos = sp->globalPosition();
    r[0] = static_cast<float>(globPos.x());
    r[1] = static_cast<float>(globPos.y());
    r[2] = static_cast<float>(globPos.z());

    // adding additional information from strip or pixel
    if (not sp->clusterList().second)
      pixInform(sp, r);
    else
      stripInform(data, sp, r);

    data.l_ITkSpacePointForSeed.emplace_back(ITk::SiSpacePointForSeed(sp, r));
    data.ns++;
  }

  void
  SiSpacePointsSeedMaker::newSpacePoint(InDet::SiSpacePointsSeedMakerEventData& data,
                                        const ActsTrk::SpacePoint* const& sp) const
  {
    data.v_ActsSpacePointForSeed.emplace_back(sp);
    data.ns++;
  }

  void SiSpacePointsSeedMaker::pixInform(const Trk::SpacePoint* const& sp,
					     float *r)
  {
    const InDet::SiCluster *cl = static_cast<const InDet::SiCluster *>(sp->clusterList().first);
    const InDetDD::SiDetectorElement *de = cl->detectorElement();
    const Amg::Transform3D &Tp = de->surface().transform();
    r[3] = float(Tp(0, 2));
    r[4] = float(Tp(1, 2));
    r[5] = float(Tp(2, 2));
  }


  void SiSpacePointsSeedMaker::stripInform(InDet::SiSpacePointsSeedMakerEventData& data,
					       const Trk::SpacePoint* const& sp, 
					       float *r)
  {
    const InDet::SiCluster *c0 = static_cast<const InDet::SiCluster *>(sp->clusterList().first);
    const InDet::SiCluster *c1 = static_cast<const InDet::SiCluster *>(sp->clusterList().second);
    const InDetDD::SiDetectorElement *d0 = c0->detectorElement();
    const InDetDD::SiDetectorElement *d1 = c1->detectorElement();

    Amg::Vector2D lc0 = c0->localPosition();
    Amg::Vector2D lc1 = c1->localPosition();

    std::pair<Amg::Vector3D, Amg::Vector3D> e0 =
        (d0->endsOfStrip(InDetDD::SiLocalPosition(lc0.y(), lc0.x(), 0.)));
    std::pair<Amg::Vector3D, Amg::Vector3D> e1 =
        (d1->endsOfStrip(InDetDD::SiLocalPosition(lc1.y(), lc1.x(), 0.)));

    Amg::Vector3D s0(.5 * (e0.first + e0.second));
    Amg::Vector3D s1(.5 * (e1.first + e1.second));

    Amg::Vector3D b0(.5 * (e0.second - e0.first));
    Amg::Vector3D b1(.5 * (e1.second - e1.first));
    Amg::Vector3D d02(s0 - s1);

    // b0
    r[3] = float(b0[0]);
    r[4] = float(b0[1]);
    r[5] = float(b0[2]);

    // b1
    r[6] = float(b1[0]);
    r[7] = float(b1[1]);
    r[8] = float(b1[2]);

    // r0-r2
    r[9] = float(d02[0]);
    r[10] = float(d02[1]);
    r[11] = float(d02[2]);

    // r0
    r[12] = float(s0[0]) - data.xbeam[0];
    r[13] = float(s0[1]) - data.ybeam[0];
    r[14] = float(s0[2]) - data.zbeam[0];
  }

  StatusCode 
  SiSpacePointsSeedMaker::retrievePixel(const EventContext& ctx,
					    InDet::SiSpacePointsSeedMakerEventData& data,
					    const Trk::PRDtoTrackMap* prd_to_track_map_cptr) const
  {

    if (m_doSpacePointConversion) {
      SG::ReadHandle< ::SpacePointContainer > pixel_handle = SG::makeHandle( m_spacepointsPixel, ctx );
      ATH_CHECK( pixel_handle.isValid() );
      const ::SpacePointContainer* pixel_container = pixel_handle.get();

      /// loop over the pixel space points
      for (const ::SpacePointCollection *spc : *pixel_container) {
        for (const Trk::SpacePoint *sp : *spc) {
          /// if we use the PRD to track map and this SP has already been used in a track, bail out
          if ( prd_to_track_map_cptr and
              isUsed(sp, *prd_to_track_map_cptr) )
            continue;
          newSpacePoint(data, sp);
        }
      }
    } else {
      // get the ActsTrk::SpacePointContainer and loop on entries to check which space point
      // you want to use for seeding

      SG::ReadHandle< ActsTrk::SpacePointContainer > inputSpacePointContainer( m_actsSpacepointsPixel, ctx );
      if (!inputSpacePointContainer.isValid()){
        ATH_MSG_FATAL("ActsTrk::SpacePointContainer with key " << m_actsSpacepointsPixel.key() << " is not available...");
        return StatusCode::FAILURE;
      }
      const ActsTrk::SpacePointContainer* inputSpacePointCollection = inputSpacePointContainer.cptr();
      // TODO: here you need to write some lines to implement the
      // check on the used PDRs in previous tracking passes
      for (const ActsTrk::SpacePoint * sp : *inputSpacePointCollection) {
        newSpacePoint(data, sp);
      }

    }

    return StatusCode::SUCCESS;
  }

  StatusCode
  SiSpacePointsSeedMaker::retrieveStrip(const EventContext& ctx,
					    InDet::SiSpacePointsSeedMakerEventData& data,
					    const Trk::PRDtoTrackMap* prd_to_track_map_cptr) const
  {
    if (m_doSpacePointConversion) {
      SG::ReadHandle< ::SpacePointContainer > strip_handle = SG::makeHandle( m_spacepointsStrip, ctx );
      ATH_CHECK( strip_handle.isValid() );
      const ::SpacePointContainer *strip_container = strip_handle.get();
      for (const ::SpacePointCollection *spc : *strip_container) {
        for (const Trk::SpacePoint *sp : *spc) {
          /// as for the pixel, veto already used SP if we are using the PRD to track map in later passes of track finding.
          if ( prd_to_track_map_cptr and
              isUsed(sp, *prd_to_track_map_cptr) )
            continue;
          newSpacePoint(data, sp);
        }
      }
    } else {
      // get the ActsTrk::SpacePointContainer and loop on entries to check which space point
      // you want to use for seeding

      SG::ReadHandle< ActsTrk::SpacePointContainer > inputSpacePointContainer( m_actsSpacepointsStrip, ctx );
      if (!inputSpacePointContainer.isValid()){
        ATH_MSG_FATAL("ActsTrk::SpacePointContainer with key " << m_actsSpacepointsStrip.key() << " is not available...");
        return StatusCode::FAILURE;
      }
      const ActsTrk::SpacePointContainer* inputSpacePointCollection = inputSpacePointContainer.cptr();
      // TODO: here you need to write some lines to implement the
      // check on the used PDRs in previous tracking passes
      for (const ActsTrk::SpacePoint * sp : *inputSpacePointCollection) {
        newSpacePoint(data, sp);
      }

    }


    return StatusCode::SUCCESS;
  }

  StatusCode 
  SiSpacePointsSeedMaker::retrieveOverlap(const EventContext& ctx,
					      InDet::SiSpacePointsSeedMakerEventData& data,
					      const Trk::PRDtoTrackMap* prd_to_track_map_cptr) const
  {
    if (m_doSpacePointConversion) {
      SG::ReadHandle< ::SpacePointOverlapCollection > overlap_handle = SG::makeHandle( m_spacepointsOverlap, ctx );
      ATH_CHECK( overlap_handle.isValid() );
      const ::SpacePointOverlapCollection *overlap_container = overlap_handle.get();

      for (const Trk::SpacePoint *sp : *overlap_container) {
        /// usual rejection of SP used in previous track finding passes if we run with the PRT to track map
        if (prd_to_track_map_cptr and
            isUsed(sp, *prd_to_track_map_cptr))
          continue;
        newSpacePoint(data, sp);
      }
    } else {
      // get the ActsTrk::SpacePointContainer and loop on entries to check which space point
      // you want to use for seeding

      SG::ReadHandle< ActsTrk::SpacePointContainer > inputSpacePointContainer( m_actsSpacepointsOverlap, ctx );
      if (!inputSpacePointContainer.isValid()){
        ATH_MSG_FATAL("ActsTrk::SpacePointContainer with key " << m_actsSpacepointsOverlap.key() << " is not available...");
        return StatusCode::FAILURE;
      }
      const ActsTrk::SpacePointContainer* inputSpacePointCollection = inputSpacePointContainer.cptr();
      // TODO: here you need to write some lines to implement the
      // check on the used PDRs in previous tracking passes
      for (const ActsTrk::SpacePoint * sp : *inputSpacePointCollection) {
        newSpacePoint(data, sp);
      }

    }


    return StatusCode::SUCCESS;
  }

  void
  SiSpacePointsSeedMaker::buildBeamFrameWork(const EventContext& ctx,
						 InDet::SiSpacePointsSeedMakerEventData& data) const
  {
    SG::ReadCondHandle< InDet::BeamSpotData > beamSpotHandle { m_beamSpotKey, ctx };
    
    const Amg::Vector3D &cb = beamSpotHandle->beamPos();
    double tx = std::tan(beamSpotHandle->beamTilt(0));
    double ty = std::tan(beamSpotHandle->beamTilt(1));

    double phi = std::atan2(ty, tx);
    double theta = std::acos(1. / std::sqrt(1. + tx * tx + ty * ty));
    double sinTheta = std::sin(theta);
    double cosTheta = std::cos(theta);
    double sinPhi = std::sin(phi);
    double cosPhi = std::cos(phi);

    data.xbeam[0] = static_cast<float>(cb.x());
    data.xbeam[1] = static_cast<float>(cosTheta * cosPhi * cosPhi + sinPhi * sinPhi);
    data.xbeam[2] = static_cast<float>(cosTheta * sinPhi * cosPhi - sinPhi * cosPhi);
    data.xbeam[3] = -static_cast<float>(sinTheta * cosPhi);

    data.ybeam[0] = static_cast<float>(cb.y());
    data.ybeam[1] = static_cast<float>(cosTheta * cosPhi * sinPhi - sinPhi * cosPhi);
    data.ybeam[2] = static_cast<float>(cosTheta * sinPhi * sinPhi + cosPhi * cosPhi);
    data.ybeam[3] = -static_cast<float>(sinTheta * sinPhi);

    data.zbeam[0] = static_cast<float>(cb.z());
    data.zbeam[1] = static_cast<float>(sinTheta * cosPhi);
    data.zbeam[2] = static_cast<float>(sinTheta * sinPhi);
    data.zbeam[3] = static_cast<float>(cosTheta);
  }


  // ===================================================================== //  
  // Interface Methods
  // ===================================================================== //  

  void
  SiSpacePointsSeedMaker::newEvent(const EventContext& ctx,
				       InDet::SiSpacePointsSeedMakerEventData& data,
				       int iteration) const
  {
    // Store iteration into data for use in other methods
    data.iteration = iteration;
    if (iteration < 0)
      data.iteration = 0;

    // At the beginning of each iteration
    // clearing list of space points to be used for seeding
    data.l_ITkSpacePointForSeed.clear();
    data.v_ActsSpacePointForSeed.clear();

    // clearing list of produced seeds
    data.i_ITkSeeds.clear();
    data.i_ITkSeed = data.i_ITkSeeds.begin();

    // First iteration:
    // -- event-specific configuration, i.e. beamspot and magnetic field
    // -- for default case: producing SSS
    // -- for fast tracking: producing PPP
    // Second iteration:
    // -- for default case: producing PPP
    // -- no additional iteration is foreseen for fast tracking case

    bool isPixel = (m_fastTracking or data.iteration == 1) and m_pixel;
    bool isStrip = not m_fastTracking and data.iteration == 0 and m_strip;

    // The Acts Seed tool requires beamspot information for the space points already here
    if (data.iteration == 0) 
      buildBeamFrameWork(ctx, data);

    // initialising the number of space points as well
    data.ns = 0;

    // Retrieve the Trk::PRDtoTrackMap
    const Trk::PRDtoTrackMap *prd_to_track_map_cptr = nullptr;
    if ( not m_prdToTrackMap.empty() ) {
      SG::ReadHandle<Trk::PRDtoTrackMap> prd_handle = SG::makeHandle( m_prdToTrackMap, ctx );
      if ( not prd_handle.isValid() ) {
        ATH_MSG_ERROR("Failed to read PRD to track association map: " << m_prdToTrackMap.key());
      }
      prd_to_track_map_cptr = prd_handle.get();
    }

    // Only retrieving the collections needed for the seed formation,
    // depending on the tool configuration and iteration

    // Retrieve Pixels
    if (isPixel and not retrievePixel(ctx, data, prd_to_track_map_cptr).isSuccess() ) {
      if (m_doSpacePointConversion)
        ATH_MSG_ERROR("Error while retrieving Pixel space points with key " << m_spacepointsPixel.key());
      else
        ATH_MSG_ERROR("Error while retrieving Pixel space points with key " << m_actsSpacepointsPixel.key());
    }

    // Retrieve Strips
    if (isStrip and not retrieveStrip(ctx, data, prd_to_track_map_cptr).isSuccess() ) {
      if (m_doSpacePointConversion)
        ATH_MSG_ERROR("Error while retrieving Strip space points with key " << m_spacepointsStrip.key());
      else
        ATH_MSG_ERROR("Error while retrieving Strip space points with key " << m_actsSpacepointsStrip.key());
    }

    // Retrieve Overlaps, will go into Strip collection
    if ((isStrip and m_useOverlap) and not retrieveOverlap(ctx, data, prd_to_track_map_cptr).isSuccess() ) {
      if (m_doSpacePointConversion)
        ATH_MSG_ERROR("Error while retrieving Strip Overlap space points with key " <<  m_spacepointsOverlap.key());
      else
        ATH_MSG_ERROR("Error while retrieving Strip Overlap space points with key " <<  m_actsSpacepointsOverlap.key());
    }    

    data.initialized = true;
  }

  void
  SiSpacePointsSeedMaker::find3Sp(const EventContext& ctx,
				      InDet::SiSpacePointsSeedMakerEventData& data,
				      const std::list<Trk::Vertex>& /*lv*/) const
  {
    // Fast return if no sps are collected
    if (m_doSpacePointConversion and data.l_ITkSpacePointForSeed.empty() )
      return;
    if (not m_doSpacePointConversion and data.v_ActsSpacePointForSeed.empty() )
      return;

    // Acts Seed Tool requires both MagneticFieldContext and BeamSpotData
    // we need to retrieve them both from StoreGate

    // Read the Beam Spot information
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    if ( not beamSpotHandle.isValid() ) {
      ATH_MSG_ERROR( "Error while retrieving beam spot" );
      return;
    }
    if (beamSpotHandle.cptr() == nullptr) {
      ATH_MSG_ERROR("Retrieved Beam Spot Handle contains a nullptr");
      return;
    }
    auto beamSpotData = beamSpotHandle.cptr();
    
    // Read the b-field information
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle { m_fieldCondObjInputKey, ctx };
    if ( not readHandle.isValid() ) {
      ATH_MSG_ERROR( "Error while retrieving Atlas Field Cache Cond DB" );
      return;
    }
    const AtlasFieldCacheCondObj* fieldCondObj{ *readHandle };
    if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
      return;
    }
    
    // Get the magnetic field
    // Using ACTS classes in order to be sure we are consistent
    Acts::MagneticFieldContext magFieldContext(fieldCondObj);


    // Beam Spot Position
    Acts::Vector2 beamPos( beamSpotData->beamPos()[ Amg::x ] * Acts::UnitConstants::mm,
			   beamSpotData->beamPos()[ Amg::y ] * Acts::UnitConstants::mm );
    
    // Magnetic Field
    ATLASMagneticFieldWrapper magneticField;
    Acts::MagneticFieldProvider::Cache magFieldCache = magneticField.makeCache( magFieldContext );
    Acts::Vector3 bField = *magneticField.getField( Acts::Vector3(beamPos.x(), beamPos.y(), 0),
						    magFieldCache );


    // Now we get care of the input space points!
    // Need to convert these space points to Acts EDM
    std::unique_ptr<ActsTrk::SpacePointContainer> actsSpContainer =
      std::make_unique<ActsTrk::SpacePointContainer>();
    std::unique_ptr<ActsTrk::SpacePointData> actsSpData =
      std::make_unique<ActsTrk::SpacePointData>();

    // this is use for a fast retrieval of the space points later
    std::vector<ITk::SiSpacePointForSeed*> sp_storage;

    if (m_doSpacePointConversion) {

      actsSpContainer->reserve(data.l_ITkSpacePointForSeed.size());
      actsSpData->reserve(data.l_ITkSpacePointForSeed.size());
      sp_storage.reserve(data.l_ITkSpacePointForSeed.size());

      std::size_t el_index = 0;
      for (auto& sp : data.l_ITkSpacePointForSeed) {

        // add to storage
        sp_storage.push_back(&sp);

        // Get position and covariance
        Acts::Vector3 position { sp.x(), sp.y(), sp.z() };
        Acts::Vector2 covariance { sp.covr(), sp.covz() };

        // This index correspond to the space point in
        // the input_space_points collection (data.l_ITkSpacePointForSeed)
        // (used later for storing the seed into data)
        boost::container::static_vector<std::size_t, 2> indexes( {el_index++} );
        std::unique_ptr<ActsTrk::SpacePoint> toAdd =
        std::make_unique<ActsTrk::SpacePoint>( position,
                                               covariance,
                                               *actsSpData.get(),
                                               indexes);
        actsSpContainer->push_back( std::move(toAdd) );
        data.v_ActsSpacePointForSeed.emplace_back( actsSpContainer->back() );
      }
    }

    // We can now run the Acts Seeding
    std::unique_ptr< ActsTrk::SeedContainer > seedPtrs = std::make_unique< ActsTrk::SeedContainer >();

    // select the ACTS seeding tool to call, if for PPP or SSS
    bool isPixel = (m_fastTracking or data.iteration == 1) and m_pixel;
    std::string combinationType = isPixel ? "PPP" : "SSS";
    ATH_MSG_DEBUG("Running Seed Finding (" << combinationType << ") ...");

    StatusCode sc;

    if (isPixel)
      sc = m_seedsToolPixel->createSeeds( ctx,
                                          data.v_ActsSpacePointForSeed,
                                          *beamSpotData,
                                          magFieldContext,
                                          *seedPtrs.get() );
    else
      sc = m_seedsToolStrip->createSeeds( ctx,
                                          data.v_ActsSpacePointForSeed,
                                          *beamSpotData,
                                          magFieldContext,
                                          *seedPtrs.get() );

    if (sc == StatusCode::FAILURE) {
      ATH_MSG_ERROR("Error during seed production (" << combinationType << ")");
      return;
    }

    ATH_MSG_DEBUG("    \\__ Created " << seedPtrs->size() << " seeds");

    // Store seeds to data
    // We need now to convert the output to Athena object once again (i.e. ITk::SiSpacePointForSeed)
    // The seeds will be stored in data.i_ITkSeeds (both PPP and SSS seeds)

    if (m_doSpacePointConversion) {
      for (const ActsTrk::Seed* seed : *seedPtrs.get()) {
        std::size_t bottom_idx = seed->sp()[0]->measurementIndexes()[0];
        std::size_t medium_idx = seed->sp()[1]->measurementIndexes()[0];
        std::size_t top_idx = seed->sp()[2]->measurementIndexes()[0];

        // Get the space point from storaga and not from std::list
        ITk::SiSpacePointForSeed *bottom_sp = new ITk::SiSpacePointForSeed(*sp_storage.at(bottom_idx));
        ITk::SiSpacePointForSeed *medium_sp = new ITk::SiSpacePointForSeed(*sp_storage.at(medium_idx));
        ITk::SiSpacePointForSeed *top_sp    = new ITk::SiSpacePointForSeed(*sp_storage.at(top_idx));

        // Estimate parameters and attach values to SPs
        estimateParameters(isPixel ? SeedStrategy::PPP : SeedStrategy::SSS,
                           data,
                           bottom_sp, medium_sp, top_sp,
                           300. * bField[2] / 1000.);

        data.i_ITkSeeds.emplace_back(ITk::SiSpacePointsProSeed(bottom_sp,
                                                               medium_sp,
                                                               top_sp,
                                                               seed->z()));
      }
    } else {

      if (isPixel) {
        SG::ReadHandle<xAOD::PixelClusterContainer> inputClusterContainer( m_pixelClusterContainerKey, ctx );
        if (!inputClusterContainer.isValid()){
          ATH_MSG_FATAL("xAOD::PixelClusterContainer with key " << m_pixelClusterContainerKey.key() << " is not available...");
          return;
        }
        const xAOD::PixelClusterContainer inputContainer = *inputClusterContainer.cptr();

        for (const ActsTrk::Seed* seed : *seedPtrs.get()) {
          // creating ITk::SiSpacePointForSeed for bottom, middle and top sps
          // first we need the space points
          std::size_t bottom_idx = seed->sp()[0]->measurementIndexes()[0];
          std::size_t medium_idx = seed->sp()[1]->measurementIndexes()[0];
          std::size_t top_idx    = seed->sp()[2]->measurementIndexes()[0];

          std::array<const xAOD::PixelCluster*, 3> pixel_cluster{ inputContainer.at(bottom_idx),
                                                                  inputContainer.at(medium_idx),
                                                                  inputContainer.at(top_idx) };

          const SG::AuxElement::Accessor< ElementLink< InDet::PixelClusterCollection > > pixelLinkAcc("pixelClusterLink");
          if (!pixelLinkAcc.isAvailable(*pixel_cluster[0])){
            ATH_MSG_FATAL("no pixelClusterLink for cluster associated to bottom sp!");
            return;
          }
          if (!pixelLinkAcc.isAvailable(*pixel_cluster[1])){
            ATH_MSG_FATAL("no pixelClusterLink for cluster associated to middle sp!");
            return;
          }
          if (!pixelLinkAcc.isAvailable(*pixel_cluster[2])){
            ATH_MSG_FATAL("no pixelClusterLink for cluster associated to top sp!");
            return;
          }

          std::array<InDet::PixelSpacePoint*, 3> spacePoints { new InDet::PixelSpacePoint(pixel_cluster[0]->identifierHash(),
                                                                                          *(pixelLinkAcc(*pixel_cluster[0]))),
                                                               new InDet::PixelSpacePoint(pixel_cluster[1]->identifierHash(),
                                                                                          *(pixelLinkAcc(*pixel_cluster[1]))),
                                                               new InDet::PixelSpacePoint(pixel_cluster[2]->identifierHash(),
                                                                                          *(pixelLinkAcc(*pixel_cluster[2])))};

          float r[15];
          r[0] = seed->sp()[0]->x();
          r[1] = seed->sp()[0]->y();
          r[2] = seed->sp()[0]->z();
          pixInform(spacePoints[0], r);
          ITk::SiSpacePointForSeed *bottom_sp = new ITk::SiSpacePointForSeed(spacePoints[0], &(r[0]));

          r[0] = seed->sp()[1]->x();
          r[1] = seed->sp()[1]->y();
          r[2] = seed->sp()[1]->z();
          pixInform(spacePoints[1], r);
          ITk::SiSpacePointForSeed *medium_sp = new ITk::SiSpacePointForSeed(spacePoints[1], &(r[0]));

          r[0] = seed->sp()[2]->x();
          r[1] = seed->sp()[2]->y();
          r[2] = seed->sp()[2]->z();
          pixInform(spacePoints[2], r);
          ITk::SiSpacePointForSeed *top_sp    = new ITk::SiSpacePointForSeed(spacePoints[2], &(r[0]));

          // Estimate parameters and attach values to SPs
          estimateParameters(SeedStrategy::PPP,
                             data,
                             bottom_sp, medium_sp, top_sp,
                             300. * bField[2] / 1000.);

          data.i_ITkSeeds.emplace_back(ITk::SiSpacePointsProSeed(bottom_sp,
                                                                 medium_sp,
                                                                 top_sp,
                                                                 seed->z()));
        }
      } else {
        SG::ReadHandle<xAOD::StripClusterContainer> inputClusterContainer( m_stripClusterContainerKey, ctx );
        if (!inputClusterContainer.isValid()){
          ATH_MSG_FATAL("xAOD::PixelClusterContainer with key " << m_stripClusterContainerKey.key() << " is not available...");
          return;
        }
        const xAOD::StripClusterContainer inputContainer = *inputClusterContainer.cptr();

        for (const ActsTrk::Seed* seed : *seedPtrs.get()) {
          // creating ITk::SiSpacePointForSeed for bottom, middle and top sps
          // first we need the space points
          auto& bottom_idx = seed->sp()[0]->measurementIndexes();
          auto& medium_idx = seed->sp()[1]->measurementIndexes();
          auto& top_idx    = seed->sp()[2]->measurementIndexes();

          std::array<const xAOD::StripCluster*, 6> strip_cluster{ inputContainer.at(bottom_idx[0]), inputContainer.at(bottom_idx[1]),
                                                                  inputContainer.at(medium_idx[0]), inputContainer.at(medium_idx[1]),
                                                                  inputContainer.at(top_idx[0])   , inputContainer.at(top_idx[1]) };

          const SG::AuxElement::Accessor< ElementLink< InDet::SCT_ClusterCollection > > stripLinkAcc("sctClusterLink");
          if (!stripLinkAcc.isAvailable(*strip_cluster[0]) or !stripLinkAcc.isAvailable(*strip_cluster[1])){
            ATH_MSG_FATAL("no sctClusterLink for clusters associated to bottom sp!");
            return;
          }
          if (!stripLinkAcc.isAvailable(*strip_cluster[2]) or !stripLinkAcc.isAvailable(*strip_cluster[3])){
            ATH_MSG_FATAL("no sctClusterLink for clusters associated to middle sp!");
            return;
          }
          if (!stripLinkAcc.isAvailable(*strip_cluster[4]) or !stripLinkAcc.isAvailable(*strip_cluster[5])){
            ATH_MSG_FATAL("no sctClusterLink for clusters associated to top sp!");
            return;
          }

          std::array<InDet::SCT_SpacePoint*, 3> spacePoints { new InDet::SCT_SpacePoint({strip_cluster[0]->identifierHash(), strip_cluster[1]->identifierHash()},
                                                                                        Amg::Vector3D(seed->sp()[0]->x(), seed->sp()[0]->y(), seed->sp()[0]->z()),
                                                                                        {*(stripLinkAcc(*strip_cluster[0])), *(stripLinkAcc(*strip_cluster[1]))}),
                                                              new InDet::SCT_SpacePoint({strip_cluster[2]->identifierHash(), strip_cluster[3]->identifierHash()},
                                                                                        Amg::Vector3D(seed->sp()[1]->x(), seed->sp()[1]->y(), seed->sp()[1]->z()),
                                                                                        {*(stripLinkAcc(*strip_cluster[2])), *(stripLinkAcc(*strip_cluster[3]))}),
                                                              new InDet::SCT_SpacePoint({strip_cluster[4]->identifierHash(), strip_cluster[5]->identifierHash()},
                                                                                        Amg::Vector3D(seed->sp()[2]->x(), seed->sp()[2]->y(), seed->sp()[2]->z()),
                                                                                        {*(stripLinkAcc(*strip_cluster[4])), *(stripLinkAcc(*strip_cluster[5]))})};

          float r[15];
          r[0] = seed->sp()[0]->x();
          r[1] = seed->sp()[0]->y();
          r[2] = seed->sp()[0]->z();
          stripInform(data, spacePoints[0], r);
          ITk::SiSpacePointForSeed *bottom_sp = new ITk::SiSpacePointForSeed(spacePoints[0], &(r[0]));

          r[0] = seed->sp()[1]->x();
          r[1] = seed->sp()[1]->y();
          r[2] = seed->sp()[1]->z();
          stripInform(data, spacePoints[1], r);
          ITk::SiSpacePointForSeed *medium_sp = new ITk::SiSpacePointForSeed(spacePoints[1], &(r[0]));

          r[0] = seed->sp()[2]->x();
          r[1] = seed->sp()[2]->y();
          r[2] = seed->sp()[2]->z();
          stripInform(data, spacePoints[2], r);
          ITk::SiSpacePointForSeed *top_sp    = new ITk::SiSpacePointForSeed(spacePoints[2], &(r[0]));

          // Estimate parameters and attach values to SPs
          estimateParameters(SeedStrategy::SSS,
                             data,
                             bottom_sp, medium_sp, top_sp,
                             300. * bField[2] / 1000.);

          data.i_ITkSeeds.emplace_back(ITk::SiSpacePointsProSeed(bottom_sp,
                                                                 medium_sp,
                                                                 top_sp,
                                                                 seed->z()));
        }
      }
    }

    data.i_ITkSeed = data.i_ITkSeeds.begin();
    data.nprint = 1;
    dump(data, msg(MSG::DEBUG));
  }

  const InDet::SiSpacePointsSeed*
  SiSpacePointsSeedMaker::next(const EventContext& /*ctx*/,
				   InDet::SiSpacePointsSeedMakerEventData& data) const
  { 

    if (data.i_ITkSeed == data.i_ITkSeeds.end()) return nullptr;
    // InDet::SiSpacePointsSeed seedOutput;
    return getSeed(*(data.i_ITkSeed++));
  }

  void
  SiSpacePointsSeedMaker::writeNtuple(const InDet::SiSpacePointsSeed* seed,
					  const Trk::Track* track, 
					  int seedType,
					  long eventNumber) const
  {
    if (not m_writeNtuple) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    if(track != nullptr) {
      m_trackPt = (track->trackParameters()->front()->pT())/1000.f;
      m_trackEta = std::abs(track->trackParameters()->front()->eta());
    }
    else {
      m_trackPt = -1.;
      m_trackEta = -1.; 
    }
    m_d0           =   seed->d0();
    m_z0           =   seed->zVertex();
    m_eta          =   seed->eta();
    m_x1           =   seed->x1();
    m_x2           =   seed->x2();
    m_x3           =   seed->x3();
    m_y1           =   seed->y1();
    m_y2           =   seed->y2();
    m_y3           =   seed->y3();      
    m_z1           =   seed->z1();
    m_z2           =   seed->z2();
    m_z3           =   seed->z3();
    m_r1           =   seed->r1();
    m_r2           =   seed->r2();
    m_r3           =   seed->r3();
    m_type         =   seedType;
    m_dzdr_b       =   seed->dzdr_b();
    m_dzdr_t       =   seed->dzdr_t();
    m_pt           =   seed->pt();
    m_givesTrack   =   !(track == nullptr);
    m_eventNumber  =   eventNumber;

    // Ok: protected by mutex.
    TTree* outputTree ATLAS_THREAD_SAFE = m_outputTree;
    outputTree->Fill();
  }

  bool
  SiSpacePointsSeedMaker::getWriteNtupleBoolProperty() const
  { return m_writeNtuple; }

  MsgStream&
  SiSpacePointsSeedMaker::dump(InDet::SiSpacePointsSeedMakerEventData& data,
				   MsgStream& out) const
  {
    if (data.nprint)
      return dumpEvent(data, out);
    return dumpConditions(data, out);
  }


  void SiSpacePointsSeedMaker::estimateParameters(const SeedStrategy /*seed_strategy*/,
						      InDet::SiSpacePointsSeedMakerEventData& /*data*/,
						      ITk::SiSpacePointForSeed*& bottom,
						      ITk::SiSpacePointForSeed*& medium,
						      ITk::SiSpacePointForSeed*& top,
						      float pTPerHelixRadius) const
  {
    // ACTS has these functions in Core, we may want to switch to those
    // The following is mostly taken from ACTS

    auto extractCoordinates = 
      [] (const ITk::SiSpacePointForSeed* sp) -> std::array<float,4>
      {
	std::array<float, 4> coordinates {sp->x(), sp->y(), sp->z(), sp->radius()};
	return coordinates;
      };

    auto extractQuantities = 
      [] (const std::array<float, 4>& sp,
	  const std::array<float, 4>& spM,
	  bool isBottom) -> std::array<float, 5>
      {
	auto& [xM, yM, zM, rM] = spM;
	auto& [xO, yO, zO, rO] = sp;

	float cosPhiM = xM / rM;
	float sinPhiM = yM / rM;
	float deltaX = xO - xM;
	float deltaY = yO - yM;
	float deltaZ = zO - zM;
	float x = deltaX * cosPhiM + deltaY * sinPhiM;
	float y = deltaY * cosPhiM - deltaX * sinPhiM;
	float iDeltaR2 = 1. / (deltaX * deltaX + deltaY * deltaY);
	float iDeltaR = std::sqrt(iDeltaR2);
	int bottomFactor = 1 * (int(not isBottom)) - 1 * (int(isBottom));
	float cot_theta = deltaZ * iDeltaR * bottomFactor;

	// cotTheta, Zo, iDeltaR, U, V
	std::array<float, 5> params =
	{ 
	  cot_theta, 
	  zM - rM * cot_theta,
	  iDeltaR,
	  x * iDeltaR2,
	  y * iDeltaR2
	};

	return params;	
      };

    auto coo_b = extractCoordinates(bottom);
    auto coo_m = extractCoordinates(medium);
    auto coo_t = extractCoordinates(top);

    // Compute the variables we need
    auto [cotThetaB, Zob, iDeltaRB, Ub, Vb] = extractQuantities(coo_b, coo_m, true);
    auto [cotThetaT, Zot, iDeltaRT, Ut, Vt] = extractQuantities(coo_t, coo_m, false);

    float squarediDeltaR2B = iDeltaRB*iDeltaRB;
    float squarediDeltaR2T = iDeltaRB*iDeltaRT;
    float squarediDeltaR = std::min(squarediDeltaR2B, squarediDeltaR2T);

    auto& [xB, yB, zB, rB] = coo_b;
    auto& [xM, yM, zM, rM] = coo_m;
    auto& [xT, yT, zT, rT] = coo_t;

    float ax = xM / rM;
    float ay = yM/ rM;

    float dxb = xM - xB;
    float dyb = yM - yB;
    float dzb = zM - zB;
    float xb = dxb * ax + dyb *ay;
    float yb = dyb * ax - dxb * ay; 
    float dxyb = xb * xb + yb * yb;                                                                                                                                                                                                                                            
    float drb = std::sqrt( xb*xb + yb*yb + dzb*dzb );

    float dxt = xT - xM;
    float dyt = yT - yM;
    float dzt = zT - zM;
    float xt = dxt * ax + dyt *ay;
    float yt = dyt * ax - dxt * ay;
    float dxyt = xt * xt + yt * yt;
    float drt = std::sqrt( xt*xt + yt*yt + dzt*dzt );

    float tzb = dzb * std::sqrt( 1./dxyb );
    float tzt = dzt * std::sqrt( 1./dxyt );

    float sTzb2 = std::sqrt(1 + tzb*tzb);

    float dU = Ut - Ub;
    if (dU == 0.) {
      return;
    }

    float A = (Vt - Vb) / dU;
    float S2 = 1. + A * A;
    float B = Vb - A * Ub;
    float B2 = B * B;

    // dzdr
    float dzdr_b = (zM - zB) / (rM - rB);
    float dzdr_t = (zT - zM) / (rT - rM);    

    // eta
    float theta = std::atan(1. / std::sqrt(cotThetaB * cotThetaT));
    float eta = -std::log(std::tan(0.5 * theta));

    // pt
    float pt = pTPerHelixRadius * std::sqrt(S2 / B2) / 2.;

    // d0
    float d0 = std::abs((A - B * rM) * rM);

    // curvature 
    // not used in current version of the code. We may want to use it later
    //    float curvature = B / std::sqrt(S2);

    // Attach variables to SPs
    top->setDR(drt);
    top->setDZDR(dzdr_t);
    top->setScorePenalty( std::abs((tzb - tzt) / (squarediDeltaR * sTzb2)) );
    top->setParam(d0);
    top->setEta(eta);
    top->setPt(pt);

    bottom->setDR(drb);
    bottom->setDZDR(dzdr_b);
  }
  

  ///////////////////////////////////////////////////////////////////
  // Dumps conditions information into the MsgStream
  ///////////////////////////////////////////////////////////////////

  MsgStream &SiSpacePointsSeedMaker::dumpConditions(InDet::SiSpacePointsSeedMakerEventData& data, MsgStream &out) const
  {
    std::string s2, s3, s4, s5;

    int n = 42-m_spacepointsPixel.key().size();
    s2.append(n,' ');
    s2.append("|");
    n     = 42-m_spacepointsStrip.key().size();
    s3.append(n,' ');
    s3.append("|");
    n     = 42-m_spacepointsOverlap.key().size();
    s4.append(n,' ');
    s4.append("|");
    n     = 42-m_beamSpotKey.key().size();
    s5.append(n,' ');
    s5.append("|");

    out<<"|---------------------------------------------------------------------|"
       <<endmsg;
    out<<"| Pixel    space points   | "<<m_spacepointsPixel.key() <<s2
       <<endmsg;
    out<<"| Strip    space points   | "<<m_spacepointsStrip.key()<<s3
       <<endmsg;
    out<<"| Overlap  space points   | "<<m_spacepointsOverlap.key()<<s4
       <<endmsg;
    out<<"| BeamConditionsService   | "<<m_beamSpotKey.key()<<s5
       <<endmsg;
    out<<"| usePixel                | "
       <<std::setw(12)<<m_pixel 
       <<"                              |"<<endmsg;
    out<<"| useStrip                | "
       <<std::setw(12)<<m_strip
       <<"                              |"<<endmsg;
    out<<"| useStripOverlap         | "
       <<std::setw(12)<<m_useOverlap
       <<"                              |"<<endmsg;
    out<<"|---------------------------------------------------------------------|"
       <<endmsg;
    out<<"| Beam X center           | "
       <<std::setw(12)<<std::setprecision(5)<<data.xbeam[0]
       <<"                              |"<<endmsg;
    out<<"| Beam Y center           | "
       <<std::setw(12)<<std::setprecision(5)<<data.ybeam[0]
       <<"                              |"<<endmsg;
    out<<"| Beam Z center           | "
       <<std::setw(12)<<std::setprecision(5)<<data.zbeam[0]
       <<"                              |"<<endmsg;
    out<<"| Beam X-axis direction   | "
       <<std::setw(12)<<std::setprecision(5)<<data.xbeam[1]
       <<std::setw(12)<<std::setprecision(5)<<data.xbeam[2]
       <<std::setw(12)<<std::setprecision(5)<<data.xbeam[3]
       <<"      |"<<endmsg;
    out<<"| Beam Y-axis direction   | "
       <<std::setw(12)<<std::setprecision(5)<<data.ybeam[1]
       <<std::setw(12)<<std::setprecision(5)<<data.ybeam[2]
       <<std::setw(12)<<std::setprecision(5)<<data.ybeam[3]
       <<"      |"<<endmsg;
    out<<"| Beam Z-axis direction   | "
       <<std::setw(12)<<std::setprecision(5)<<data.zbeam[1]
       <<std::setw(12)<<std::setprecision(5)<<data.zbeam[2]
       <<std::setw(12)<<std::setprecision(5)<<data.zbeam[3]
       <<"      |"<<endmsg;
    out<<"|---------------------------------------------------------------------|"
       <<endmsg;
    return out;

  }

  ///////////////////////////////////////////////////////////////////
  // Dumps event information into the MsgStream
  ///////////////////////////////////////////////////////////////////

  MsgStream &SiSpacePointsSeedMaker::dumpEvent(InDet::SiSpacePointsSeedMakerEventData& data,
						   MsgStream &out) 
  {
    out<<"|---------------------------------------------------------------------|"
       <<endmsg;
    out<<"| ns                       | "
       <<std::setw(12)<<data.ns
       <<"                              |"<<endmsg;
    out<<"| seeds                   | "
       <<std::setw(12)<<data.i_ITkSeeds.size()
       <<"                              |"<<endmsg;
    out<<"|---------------------------------------------------------------------|"
       <<endmsg;
    return out;

  }

  const InDet::SiSpacePointsSeed* SiSpacePointsSeedMaker::getSeed(ITk::SiSpacePointsProSeed& proSeed) const {

    InDet::SiSpacePointsSeed* seed = new InDet::SiSpacePointsSeed(proSeed.spacepoint0()->spacepoint,
                                                                  proSeed.spacepoint1()->spacepoint,
                                                                  proSeed.spacepoint2()->spacepoint,
                                                                  double(proSeed.z()));
    seed->setD0(proSeed.spacepoint2()->param());
    seed->setEta(proSeed.spacepoint2()->eta());
    seed->setX1(proSeed.spacepoint0()->x());
    seed->setX2(proSeed.spacepoint1()->x());
    seed->setX3(proSeed.spacepoint2()->x());
    seed->setY1(proSeed.spacepoint0()->y());
    seed->setY2(proSeed.spacepoint1()->y());
    seed->setY3(proSeed.spacepoint2()->y());
    seed->setZ1(proSeed.spacepoint0()->z());
    seed->setZ2(proSeed.spacepoint1()->z());
    seed->setZ3(proSeed.spacepoint2()->z());
    seed->setR1(proSeed.spacepoint0()->radius());
    seed->setR2(proSeed.spacepoint1()->radius());
    seed->setR3(proSeed.spacepoint2()->radius());
    seed->setDZDR_B(proSeed.spacepoint0()->dzdr());
    seed->setDZDR_T(proSeed.spacepoint2()->dzdr());
    seed->setPt(proSeed.spacepoint2()->pt());

    return seed;
  }
  
}
