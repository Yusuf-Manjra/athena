/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
-------------------
ATLAS Collaboration
***************************************************************************/

//<<<<<< INCLUDES >>>>>>


#include "SiSpacePointFormation/SiTrackerSpacePointFinder.h"

// For processing clusters
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

// Space point Classes,
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"


// general Atlas classes
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadCondHandle.h"

#include "AthenaMonitoring/Monitored.h"
namespace InDet {

//------------------------------------------------------------------------
  SiTrackerSpacePointFinder::SiTrackerSpacePointFinder(const std::string& name,
      ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator),
    m_Sct_clcontainerKey("SCT_Clusters"),
    m_Pixel_clcontainerKey("PixelClusters"),
    m_selectPixels(true),
    m_selectSCTs(true),
    m_overlap(true),  // process overlaps of SCT wafers.
    m_allClusters(false),  // process all clusters without limits.
    m_overlapLimitOpposite(2.8),  // overlap limit for opposite-neighbour.
    m_overlapLimitPhi(5.64),  // overlap limit for phi-neighbours.
    m_overlapLimitEtaMin(1.68),  // low overlap limit for eta-neighbours.
    m_overlapLimitEtaMax(3.0),  // high overlap limit for eta-neighbours.
    m_overrideBS(false),
    m_xVertex(0.),
    m_yVertex(0.),
    m_zVertex(0.),
    m_numberOfEvents(0), m_numberOfPixel(0), m_numberOfSCT(0),
    m_sctCacheHits(0), m_pixCacheHits(0),
    m_cachemode(false),
    m_idHelper(nullptr),
    m_idHelperPixel(nullptr),
    m_SpacePointContainer_SCTKey("SCT_SpacePoints"),
    m_SpacePointContainerPixelKey("PixelSpacePoints"),
    m_spacepointoverlapCollectionKey("OverlapSpacePoints"),
    m_SpacePointCache_SCTKey(""),
    m_SpacePointCache_PixKey(""),
    m_SiSpacePointMakerTool("InDet::SiSpacePointMakerTool", this)
{
  //Use the same space point name both for internal use (for graphics) end
  // for storing in TDS. If a name is the empty string, do not retrieve
  // those clusters.
  declareProperty("SCT_ClustersName", m_Sct_clcontainerKey, "SCT clContainer" );
  declareProperty("PixelsClustersName", m_Pixel_clcontainerKey, "Pixel clContainer");

  declareProperty("SpacePointsSCTName", m_SpacePointContainer_SCTKey, "SpacePoint SCT container");
  declareProperty("SpacePointsPixelName", m_SpacePointContainerPixelKey, "SpacePoint Pixel container");
  declareProperty("SpacePointsOverlapName", m_spacepointoverlapCollectionKey, "Space Point Overlap collection" );
  
  
  declareProperty("SiSpacePointMakerTool", m_SiSpacePointMakerTool);
  declareProperty("BeamPositionKey", m_beamSpotKey);
  declareProperty("ProcessPixels", m_selectPixels);
  declareProperty("ProcessSCTs", m_selectSCTs);
  declareProperty("ProcessOverlaps", m_overlap);
  declareProperty("AllClusters", m_allClusters);
  declareProperty("OverlapLimitOpposite", m_overlapLimitOpposite);
  declareProperty("OverlapLimitPhi", m_overlapLimitPhi);
  declareProperty("OverlapLimitEtaMin", m_overlapLimitEtaMin);
  declareProperty("OverlapLimitEtaMax", m_overlapLimitEtaMax);
  declareProperty("OverrideBeamSpot", m_overrideBS);
  declareProperty("VertexX", m_xVertex);
  declareProperty("VertexY", m_yVertex);
  declareProperty("VertexZ", m_zVertex);

  declareProperty("SpacePointCacheSCT", m_SpacePointCache_SCTKey);
  declareProperty("SpacePointCachePix", m_SpacePointCache_PixKey);

}
//--------------------------------------------------------------------------
SiTrackerSpacePointFinder::~SiTrackerSpacePointFinder()
{
}

//-----------------------------------------------------------------------
StatusCode SiTrackerSpacePointFinder::initialize()
{
  ATH_MSG_DEBUG( "SiTrackerSpacePointFinder::initialize()" );

  // Check that clusters, space points and ids have names
  if (m_selectSCTs && m_Sct_clcontainerKey.key().empty()){
    ATH_MSG_FATAL( "SCTs selected and no name set for SCT clusters");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_Sct_clcontainerKey.initialize(m_selectSCTs) );

  if (m_selectPixels && m_Pixel_clcontainerKey.key().empty()){
    ATH_MSG_FATAL( "Pixels selected and no name set for Pixel clusters");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_Pixel_clcontainerKey.initialize(m_selectPixels) );

  if (m_selectSCTs && m_SpacePointContainer_SCTKey.key().empty()){
    ATH_MSG_FATAL( "No name set for SCT space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_SpacePointContainer_SCTKey.initialize(m_selectSCTs) );

  if (m_selectPixels && m_SpacePointContainerPixelKey.key().empty()){
    ATH_MSG_FATAL( "No name set for Pixels space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_SpacePointContainerPixelKey.initialize(m_selectPixels) );

  if (m_spacepointoverlapCollectionKey.key().empty()){
    ATH_MSG_FATAL( "No name set for overlap space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_spacepointoverlapCollectionKey.initialize() );

  // create containers (requires the Identifier Helpers)
  if (m_selectPixels){
    ATH_CHECK(detStore()->retrieve(m_idHelperPixel,"PixelID"));
    ATH_CHECK(m_pixelDetEleCollKey.initialize());
  }

  if (m_selectSCTs) {
    ATH_CHECK(detStore()->retrieve(m_idHelper,"SCT_ID"));

    // Initialize the key of input SiElementPropertiesTable and SiDetectorElementCollection for SCT
    ATH_CHECK(m_SCTPropertiesKey.initialize());
    ATH_CHECK(m_SCTDetEleCollKey.initialize());
  }

  ATH_CHECK(m_SiSpacePointMakerTool.retrieve());
  ATH_CHECK(m_beamSpotKey.initialize(!m_overrideBS));

  ATH_CHECK(m_SpacePointCache_SCTKey.initialize(!m_SpacePointCache_SCTKey.key().empty()));
  ATH_CHECK(m_SpacePointCache_PixKey.initialize(!m_SpacePointCache_PixKey.key().empty()));
  m_cachemode = !m_SpacePointCache_SCTKey.key().empty() || !m_SpacePointCache_PixKey.key().empty();

  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());

  ATH_MSG_INFO( "SiTrackerSpacePointFinder::initialized for package version " << PACKAGE_VERSION );
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------

StatusCode SiTrackerSpacePointFinder::execute (const EventContext& ctx) const
{


  ++m_numberOfEvents;
  const InDetDD::SiDetectorElementCollection* elements = nullptr;
  const SiElementPropertiesTable* properties = nullptr;
  
  auto nReceivedClustersSCT = Monitored::Scalar<int>( "numSctClusters" , 0 );
  auto nReceivedClustersPIX = Monitored::Scalar<int>( "numPixClusters" , 0 );
  auto nSCTspacePoints = Monitored::Scalar<int>( "numSctSpacePoints"   , 0 );
  auto nPIXspacePoints = Monitored::Scalar<int>( "numPixSpacePoints"   , 0 );

  if (m_selectSCTs) {
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey, ctx);
    elements = sctDetEle.retrieve();
    if (elements==nullptr) {
      ATH_MSG_FATAL("Pointer of SiDetectorElementCollection (" << m_SCTDetEleCollKey.fullKey() << ") could not be retrieved");
      return StatusCode::SUCCESS;
    }
    SG::ReadCondHandle<SiElementPropertiesTable> sctProperties(m_SCTPropertiesKey, ctx);
    properties = sctProperties.retrieve();
    if (properties==nullptr) {
      ATH_MSG_FATAL("Pointer of SiElementPropertiesTable (" << m_SCTPropertiesKey.fullKey() << ") could not be retrieved");
      return StatusCode::SUCCESS;
    }
  }
  SPFCache r_cache(ctx);
  if (! m_overrideBS){
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    const InDet::BeamSpotData* beamSpot = *beamSpotHandle;
    r_cache.vertex = beamSpot->beamVtx().position();
  } else {
    r_cache.vertex = Amg::Vector3D(m_xVertex,m_yVertex,m_zVertex);
  }

  // register the IdentifiableContainer into StoreGate
  SG::WriteHandle<SpacePointContainer> spacePointContainerPixel;
  if (m_selectPixels){
    spacePointContainerPixel = SG::WriteHandle<SpacePointContainer>( m_SpacePointContainerPixelKey, ctx );
    if(m_SpacePointCache_PixKey.key().empty()){
       ATH_CHECK( spacePointContainerPixel.record( std::make_unique<SpacePointContainer>(m_idHelperPixel->wafer_hash_max()) ) );
       ATH_MSG_DEBUG("Created SpacePointContainer " << m_SpacePointContainerPixelKey.key() << " N= " << m_idHelperPixel->wafer_hash_max());
    }else{
       SG::UpdateHandle<SpacePointCache> updateH( m_SpacePointCache_PixKey, ctx );
       ATH_CHECK(updateH.isValid());
       ATH_CHECK( spacePointContainerPixel.record( std::make_unique<SpacePointContainer>(updateH.ptr()) ) );
       ATH_MSG_DEBUG("Created SpacePointContainer " << m_SpacePointContainerPixelKey.key() << " from cache " << m_SpacePointCache_PixKey.key());
    }
  }

  SG::WriteHandle<SpacePointContainer> spacePointContainer_SCT;
  if (m_selectSCTs){
    spacePointContainer_SCT = SG::WriteHandle<SpacePointContainer>( m_SpacePointContainer_SCTKey, ctx );
    if(m_SpacePointCache_SCTKey.key().empty()){
       ATH_CHECK( spacePointContainer_SCT.record( std::make_unique<SpacePointContainer>(m_idHelper->wafer_hash_max()) ) );
       ATH_MSG_DEBUG("Created SpacePointContainer " << m_SpacePointContainer_SCTKey.key() << " N= " << m_idHelper->wafer_hash_max());
    }else{
       SG::UpdateHandle<SpacePointCache> updateH( m_SpacePointCache_SCTKey, ctx );
       ATH_CHECK(updateH.isValid());
       ATH_CHECK( spacePointContainer_SCT.record( std::make_unique<SpacePointContainer>(updateH.ptr()) ) );
       ATH_MSG_DEBUG("Created SpacePointContainer " << m_SpacePointContainer_SCTKey.key() << " from cache " << m_SpacePointCache_SCTKey.key());
    }
  }

  SG::WriteHandle<SpacePointOverlapCollection> spacepointoverlapCollection( m_spacepointoverlapCollectionKey, ctx );
  ATH_CHECK( spacepointoverlapCollection.record( std::make_unique<SpacePointOverlapCollection>() ) );

  ATH_MSG_DEBUG( "Container '" << spacepointoverlapCollection.name() << "' initialised" );

  int sctCacheCount = 0;
  int pixCacheCount = 0;

  if (m_selectSCTs){

    // retrieve SCT cluster container
    SG::ReadHandle<SCT_ClusterContainer> sct_clcontainer( m_Sct_clcontainerKey, ctx );
    if (!sct_clcontainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< sct_clcontainer.name() << " !" << endmsg;
      return StatusCode::RECOVERABLE;
    }

    r_cache.SCTCContainer = sct_clcontainer.cptr();

    ATH_MSG_DEBUG( "SCT Cluster container found: " << sct_clcontainer->size() << " collections" );
    nReceivedClustersSCT = sct_clcontainer->size();
    // Get hold of all clusters and iterate through them.
    // Pixel clusters will be converted immediately to pixel space points.
    // For SCT clusters, posssible pairs will be found and space points computed.
    // It is possible, though unlikely, that there are no clusters for the
    // event. Therefore we do not terminate the job if we don't find any.
    SCT_ClusterContainer::const_iterator it = sct_clcontainer->begin();
    SCT_ClusterContainer::const_iterator itend = sct_clcontainer->end();

    for (; it != itend; ++it){
      const SCT_ClusterCollection *colNext=&(**it);
      // Create SpacePointCollection
      IdentifierHash idHash = colNext->identifyHash();
      SpacePointContainer::IDC_WriteHandle lock = spacePointContainer_SCT->getWriteHandle(idHash);
      if(lock.alreadyPresent()){
          ATH_MSG_DEBUG("SCT Hash " << idHash << " is already in cache");
          ++sctCacheCount;
          continue; //Skip if already present in cache
      }

      Identifier elementID = colNext->identify();
      auto spacepointCollection = std::make_unique<SpacePointCollection>(idHash);
      spacepointCollection->setIdentifier(elementID);

      if ( colNext->size() != 0){
        addSCT_SpacePoints(colNext, properties, elements,
                           spacepointCollection.get(), spacepointoverlapCollection.ptr(), r_cache);
      } else {
        ATH_MSG_DEBUG( "Empty SCT cluster collection" );
      }
      size_t size = spacepointCollection->size();
      if (size == 0){
        ATH_MSG_VERBOSE( "SiTrackerSpacePointFinder algorithm found no space points" );
      } else {
        //In a MT environment the cache maybe filled by another thread in which case this will delete the duplicate
        StatusCode sc= lock.addOrDelete( std::move(spacepointCollection) );
        if (sc.isFailure()){
          ATH_MSG_ERROR( "Failed to add SpacePoints to container" );
          return StatusCode::RECOVERABLE;
        }
        ATH_MSG_VERBOSE( size << " SpacePoints successfully added to Container !" );
      }
    }
  }

  if (m_selectPixels)
  {

    SG::ReadHandle<PixelClusterContainer> pixel_clcontainer( m_Pixel_clcontainerKey, ctx );
    if (!pixel_clcontainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< pixel_clcontainer.name() << " !" << endmsg;
      return StatusCode::RECOVERABLE;
    }

    ATH_MSG_DEBUG( "Data object " << pixel_clcontainer.name() << " found" );

    // loop over Pixel clusterCollections
    PixelClusterContainer::const_iterator colNext = pixel_clcontainer->begin();
    PixelClusterContainer::const_iterator lastCol = pixel_clcontainer->end();

    nReceivedClustersPIX = pixel_clcontainer->size();

    int numColl=0;
    for (; colNext != lastCol; ++colNext)
    {
      ATH_MSG_VERBOSE( "Collection num " << numColl++ );
      IdentifierHash idHash = (*colNext)->identifyHash();
      SpacePointContainer::IDC_WriteHandle lock = spacePointContainerPixel->getWriteHandle(idHash);
      if(lock.alreadyPresent()){
          ATH_MSG_DEBUG("pixel Hash " << idHash << " is already in cache");
          ++pixCacheCount;
          continue;
      }
      // Create SpacePointCollection
      Identifier elementID = (*colNext)->identify();
      auto spacepointCollection = std::make_unique< SpacePointCollection >(idHash);
      spacepointCollection->setIdentifier(elementID);

      if ((*colNext)->size() != 0)
      {
        m_SiSpacePointMakerTool->fillPixelSpacePointCollection(*colNext,spacepointCollection.get());
      }
      else
      {
        ATH_MSG_DEBUG( "Empty pixel cluster collection" );
      }
      size_t size = spacepointCollection->size();
      if (size == 0)
      {
        ATH_MSG_DEBUG( "SiTrackerSpacePointFinder algorithm found no space points" );
      }
      else
      {
        StatusCode sc = lock.addOrDelete( std::move(spacepointCollection) );
        if (sc.isFailure())
        {
          ATH_MSG_ERROR( "Failed to add SpacePoints to container" );
          return StatusCode::RECOVERABLE;
        }
        ATH_MSG_VERBOSE( size
            << " SpacePoints successfully added to Container !" );
      }
    }
  }

  // store the overlap space points.
  // check that the set isn't empty.
  if (spacepointoverlapCollection->size()==0)
  {
    ATH_MSG_DEBUG( "No overlap space points found" );
  }
  else
  {
    ATH_MSG_DEBUG( spacepointoverlapCollection->size() <<" overlap space points registered." );
  }
  if (m_selectPixels) {
    auto c = spacePointContainerPixel->numberOfCollections();
    m_numberOfPixel += c;
    nPIXspacePoints  = c;
  }
  if (m_selectSCTs) {
    auto c = spacePointContainer_SCT->numberOfCollections();
    m_numberOfSCT   += c;
    nSCTspacePoints  = c;
  }
  if(m_cachemode)//Prevent unnecessary atomic counting
  {
     m_sctCacheHits  += sctCacheCount;
     m_pixCacheHits  += pixCacheCount;
  }

  auto mon = Monitored::Group( m_monTool, nReceivedClustersPIX,nReceivedClustersSCT, nPIXspacePoints, nSCTspacePoints );

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode SiTrackerSpacePointFinder::finalize()
{
  ATH_MSG_INFO( "SiTrackerSpacePointFinder::finalize()" );
  ATH_MSG_INFO( m_numberOfEvents << " events processed" );
  ATH_MSG_INFO( m_numberOfPixel << " pixel collections processed" );
  ATH_MSG_INFO( m_numberOfSCT << " sct collections processed" );
  if(m_cachemode){
    //These are debug messages because they can be indeterminate in an MT environment and it could
    //lead to confusing log comparisons.
    ATH_MSG_DEBUG( m_sctCacheHits << " sct cache hits" );
    ATH_MSG_DEBUG( m_pixCacheHits << " pix cache hits" );
  }
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

void SiTrackerSpacePointFinder::
addSCT_SpacePoints(const SCT_ClusterCollection* next,
                   const SiElementPropertiesTable* properties,
                   const InDetDD::SiDetectorElementCollection* elements,
    SpacePointCollection* spacepointCollection, SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache &r_cache) const
{

  // Do nothing unless this is a side 1 detector (strips of const phi).
  IdentifierHash thisHash(next->identifyHash());

  // if it is not the stereo side
  const InDetDD::SiDetectorElement *element = elements->getDetectorElement(thisHash);
  if (element && !(element->isStereo())){
    //if (m_idHelper->side(thisID)==1){
    // Space points are created from clusters for all possibly
    // overlapping pairs in the given range if m_overlap=true.
    // Otherwise just the opposite pair are processed.
    // Pick up the five neighbours of detector, and check ranges
    // for which overlap is possible. 
    // "check1" is used for opposite and eta overlaps.
    // check2 for phi overlaps

    const std::vector<IdentifierHash>*
      others(properties->neighbours(thisHash));
    if (others==0 || others->empty() ) return;
    std::vector<IdentifierHash>::const_iterator otherHash = others->begin();

    bool overlapColl = false;
    // check opposite wafer
    checkForSCT_Points(next, *otherHash,
                       elements,
                       -m_overlapLimitOpposite, +m_overlapLimitOpposite,
                       spacepointCollection,overlapColl,spacepointOverlapCollection, r_cache);

    if (! m_overlap) return;

    // check phi overlaps
    overlapColl = true;
    ++otherHash;
    if (otherHash == others->end() ) return;
    float hwidth(properties->halfWidth(thisHash));
    // half-width of wafer

    checkForSCT_Points(next, *otherHash,
                       elements,
                       -hwidth, -hwidth+m_overlapLimitPhi,
                       +hwidth-m_overlapLimitPhi, +hwidth,spacepointOverlapCollection, r_cache);
    ++otherHash;
    if (otherHash == others->end() ) return;
    checkForSCT_Points(next, *otherHash,
                       elements,
                       +hwidth-m_overlapLimitPhi, +hwidth,
                       -hwidth, -hwidth+m_overlapLimitPhi,spacepointOverlapCollection, r_cache);

    // if barrel, check the eta overlaps too.
    // In this case, action depends on whether layer is even or odd
    // Also, whether we look at "lower in eta", which comes first,
    // or "higher".
    ++otherHash;
    if (otherHash == others->end() )  return;
    // get Identifier without hashing or so
    Identifier thisID = element->identify();

    //if (m_idHelper->barrel_ec(thisID)==0)
    if (m_idHelper->is_barrel(thisID))
    {
      //if (m_idHelper->layer_disk(thisID)==0 || m_idHelper->layer_disk(thisID)==2)
      if (m_idHelper->layer_disk(thisID)%2 == 0)
      {
        checkForSCT_Points(next, *otherHash,
                           elements,
                           +m_overlapLimitEtaMin,
                           +m_overlapLimitEtaMax,
                           spacepointCollection,overlapColl,spacepointOverlapCollection, r_cache);
        ++otherHash;
        if (otherHash == others->end() )  return;

        checkForSCT_Points(next, *otherHash,
                           elements,
                           -m_overlapLimitEtaMax,
                           -m_overlapLimitEtaMin,
                           spacepointCollection,overlapColl,spacepointOverlapCollection, r_cache);
      }else{
        checkForSCT_Points(next, *otherHash,
                           elements,
                           -m_overlapLimitEtaMax,
                           -m_overlapLimitEtaMin,
                           spacepointCollection,overlapColl,spacepointOverlapCollection, r_cache);
        ++otherHash;
        if (otherHash == others->end() )  return;


        checkForSCT_Points(next, *otherHash,
                           elements,
                           +m_overlapLimitEtaMin,
                           +m_overlapLimitEtaMax,
                           spacepointCollection,overlapColl,spacepointOverlapCollection, r_cache);
      }
    }
  }
}

//--------------------------------------------------------------------------

void SiTrackerSpacePointFinder::
checkForSCT_Points(const SCT_ClusterCollection* clusters1,
                   const IdentifierHash id2,
                   const InDetDD::SiDetectorElementCollection* elements,
                   double min, double max,
                   SpacePointCollection* spacepointCollection, bool overlapColl, SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache &r_cache) const
{


  // Get the cluster collections for these two detectors.
  // Require that (xPhi2 - xPhi1) lie in the range specified.
  // Used for opposite and eta modules

  //indexFindPtr is faster in the MT implementation
  const SCT_ClusterCollection * clusters2 = r_cache.SCTCContainer->indexFindPtr(id2);
  if (clusters2==nullptr) return;

  if (!overlapColl) {
    m_SiSpacePointMakerTool->fillSCT_SpacePointCollection(clusters1, clusters2, min, max, m_allClusters, r_cache.vertex, elements, spacepointCollection);
  }
  else {
    m_SiSpacePointMakerTool->fillSCT_SpacePointEtaOverlapCollection(clusters1, clusters2, min, max, m_allClusters, r_cache.vertex, elements, spacepointOverlapCollection);
  }
}
//--------------------------------------------------------------------------
void SiTrackerSpacePointFinder::
checkForSCT_Points(const SCT_ClusterCollection* clusters1,
                   const IdentifierHash id2,
                   const InDetDD::SiDetectorElementCollection* elements,
    double min1, double max1, double min2, double max2, SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache &r_cache) const
{

  // get the cluster collections for these two detectors. Clus1 must lie
  // within min1 and max1 and clus between min2 and max2. Used for phi
  // overlaps.
  const SCT_ClusterCollection * clusters2 (r_cache.SCTCContainer->indexFindPtr(id2));
  if (clusters2==nullptr) return;

  m_SiSpacePointMakerTool->fillSCT_SpacePointPhiOverlapCollection(clusters1, clusters2, min1, max1, min2, max2, m_allClusters, r_cache.vertex, elements, spacepointOverlapCollection);
}

} //namespace
