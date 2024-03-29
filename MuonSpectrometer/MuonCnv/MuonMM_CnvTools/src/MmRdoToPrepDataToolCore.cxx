/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "MmRdoToPrepDataToolCore.h"

#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "TrkEventPrimitives/LocalDirection.h"

// BS access
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

namespace {
  // Count hits with negative charge, which indicates bad calibration
  std::atomic<bool> hitNegativeCharge{false};
  std::atomic<bool> invalidLocalPos{false};
}
class NswCalibDbTimeChargeData;

Muon::MmRdoToPrepDataToolCore::MmRdoToPrepDataToolCore(const std::string& t,
					       const std::string& n,
					       const IInterface*  p )
  :
  base_class(t,n,p) {}


StatusCode Muon::MmRdoToPrepDataToolCore::initialize()
{  
  ATH_MSG_DEBUG(" in initialize()");  
  ATH_CHECK(m_idHelperSvc.retrieve());
  // check if the initialization of the data container is success
  ATH_CHECK(m_mmPrepDataContainerKey.initialize());
  ATH_CHECK(m_rdoContainerKey.initialize());
  ATH_CHECK(m_muDetMgrKey.initialize());
  ATH_CHECK(m_calibTool.retrieve());
  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::MmRdoToPrepDataToolCore::processCollection(Muon::MMPrepDataContainer* mmPrepDataContainer,
                                                            const std::vector<IdentifierHash>& idsToDecode,
                                                            const MM_RawDataCollection *rdoColl, 
							std::vector<IdentifierHash>& idWithDataVect) const
{
  ATH_MSG_DEBUG(" ***************** Start of process MM Collection");
  const MmIdHelper& id_helper = m_idHelperSvc->mmIdHelper();
  const EventContext& ctx = Gaudi::Hive::currentContext();
  bool merge = m_merge;
// protect for large splashes 
  if(rdoColl->size()>100) merge = true; 

  const IdentifierHash hash = rdoColl->identifierHash();

  // check if we actually want to decode this RDO collection
  if(idsToDecode.size() > 0 and std::find(idsToDecode.begin(), idsToDecode.end(), hash)==idsToDecode.end()) {
    ATH_MSG_DEBUG("Hash ID " << hash << " not in input list, ignore");
    return StatusCode::SUCCESS;
  } else ATH_MSG_DEBUG("Going to decode " << hash);

  // check if the collection already exists, otherwise add it
  if ( mmPrepDataContainer->indexFindPtr(hash) != nullptr) {

    ATH_MSG_DEBUG("In processCollection: collection already contained in the MM PrepData container");
    idWithDataVect.push_back(hash);
    return StatusCode::SUCCESS;
  } 

  // Get write handle for this collection
  MMPrepDataContainer::IDC_WriteHandle lock = mmPrepDataContainer->getWriteHandle( hash );
  // Check if collection already exists (via the cache, i.e. in online trigger mode)
  if( lock.OnlineAndPresentInAnotherView() ) {
    ATH_MSG_DEBUG("In processCollection: collection already available in the MM PrepData container (via cache)");
    idWithDataVect.push_back(hash);
    return StatusCode::SUCCESS;
  }
  std::unique_ptr<MMPrepDataCollection> prdColl = std::make_unique<MMPrepDataCollection>(hash);
  idWithDataVect.push_back(hash);
    
  // set the offline identifier of the collection Id
  IdContext context = id_helper.module_context();
  Identifier moduleId;
  int getId = id_helper.get_id(hash,moduleId,&context);
  if ( getId != 0 ) {
    ATH_MSG_ERROR("Could not convert the hash Id: " << hash << " to identifier");
  } 
  else {
    ATH_MSG_DEBUG(" dump moduleId " << moduleId );
    prdColl->setIdentifier(moduleId);
  }  

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_muDetMgrKey,ctx};
  const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
  if(!MuonDetMgr){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
    return StatusCode::FAILURE;
  }
 
  std::vector<MMPrepData> MMprds;
  // convert the RDO collection to a PRD collection
  for (const MM_RawData* rdo : *rdoColl ) {
    ATH_MSG_DEBUG("Adding a new MM PrepRawData");

    const Identifier rdoId = rdo->identify();
    if (!m_idHelperSvc->isMM(rdoId)) {
      ATH_MSG_WARNING("given Identifier "<<rdoId.get_compact()<<" ("<<id_helper.print_to_string(rdoId)<<") is no MicroMega Identifier, continuing");
      continue;
    }
    ATH_MSG_DEBUG(" dump rdo " << m_idHelperSvc->toString(rdoId ));
    
    int channel = rdo->channel();
    std::vector<Identifier> rdoList;
    Identifier parentID = id_helper.parentID(rdoId);
    Identifier layid = id_helper.channelID(parentID, id_helper.multilayer(rdoId), id_helper.gasGap(rdoId),1);
    Identifier prdId = id_helper.channelID(parentID, id_helper.multilayer(rdoId), id_helper.gasGap(rdoId),channel);
    ATH_MSG_DEBUG(" channel RDO " << channel << " channel from rdoID " << id_helper.channel(rdoId));
    rdoList.push_back(prdId);
 
    // get the local and global positions
    const MuonGM::MMReadoutElement* detEl = MuonDetMgr->getMMReadoutElement(layid);
    Amg::Vector2D localPos{Amg::Vector2D::Zero()};

    bool getLocalPos = detEl->stripPosition(prdId, localPos);
    if ( !getLocalPos ) {
      if (!invalidLocalPos || msgLvl(MSG::DEBUG)){
          ATH_MSG_WARNING("Could not get the local strip position for "<<m_idHelperSvc->toString(prdId));
          invalidLocalPos = true;
      }
      continue;
    }

    Amg::Vector3D globalPos{0.,0.,0.};
    bool getGlobalPos = detEl->stripGlobalPosition(prdId,globalPos);
    if ( !getGlobalPos ) {
      ATH_MSG_WARNING("Could not get the global strip position for MM");
      continue;
    }
    NSWCalib::CalibratedStrip calibStrip;
    ATH_CHECK (m_calibTool->calibrateStrip(ctx, rdo, calibStrip));
    if (calibStrip.charge < 0) {
        if (!hitNegativeCharge || msgLvl(MSG::DEBUG)){
          ATH_MSG_WARNING("One MM RDO or more, such as one with pdo = "<<rdo->charge() << " counts, corresponds to a negative charge (" << calibStrip.charge << "). Skipping these RDOs");
          hitNegativeCharge = true;
        }
        continue;
    }

    Trk::LocalDirection localDir;
    const Trk::PlaneSurface& psurf = detEl->surface(layid);
    Amg::Vector2D lpos{Amg::Vector2D::Zero()};
    psurf.globalToLocal(globalPos,globalPos,lpos);
    psurf.globalToLocalDirection(globalPos, localDir);
       
    ATH_MSG_DEBUG(" Surface centre x " << psurf.center().x() << " y " << psurf.center().y() << " z " << psurf.center().z() );
    ATH_MSG_DEBUG(" localPos x " << localPos.x() << " localPos y " << localPos.y() << " lpos recalculated 0 " << lpos[0] << " lpos y " << lpos[1]);

    Amg::Vector3D  gdir = psurf.transform().linear()*Amg::Vector3D(0.,1.,0.);
    ATH_MSG_DEBUG(" MM detector surface direction phi " << gdir.phi() << " global radius hit " << globalPos.perp() << " phi pos " << globalPos.phi() << " global z " << globalPos.z());      

    auto cov = Amg::MatrixX(2,2);
    cov.setIdentity();
    (cov)(0,0) = calibStrip.resTransDistDrift;  
    (cov)(1,1) = calibStrip.resLongDistDrift;
    localPos.x() += calibStrip.dx;

    if(!merge) {
       	// storage will be handeled by Store Gate
        std::unique_ptr<MMPrepData> mpd =
          std::make_unique<MMPrepData>(prdId,
                                       hash,
                                       localPos,
                                       rdoList,
                                       cov,
                                       detEl,
                                       calibStrip.time,
                                       calibStrip.charge,
                                       calibStrip.distDrift);
        mpd->setAuthor(Muon::MMPrepData::Author::RDOTOPRDConverter);
        prdColl->push_back(std::move(mpd));

    } else {
      MMPrepData mpd = MMPrepData(prdId,
                                  hash,
                                  localPos,
                                  rdoList,
                                  cov,
                                  detEl,
                                  calibStrip.time,
                                  calibStrip.charge,
                                  calibStrip.distDrift);
      if (mpd.charge() < m_singleStripChargeCut) continue;
      // set the hash of the MMPrepData such that it contains the correct value
      // in case it gets used in SimpleMMClusterBuilderTool::getClusters
      mpd.setHashAndIndex(hash, 0);
      mpd.setAuthor(Muon::MMPrepData::Author::RDOTOPRDConverter);
      MMprds.push_back(std::move(mpd));
    } 
  }

  if(merge) {
    std::vector<std::unique_ptr<Muon::MMPrepData>>  clusters;

    /// reconstruct the clusters
    ATH_CHECK(m_clusterBuilderTool->getClusters(MMprds,clusters));

    for (std::unique_ptr<Muon::MMPrepData>& prdN : clusters ) {
      prdN->setHashAndIndex(prdColl->identifyHash(), prdColl->size());
      prdColl->push_back(std::move(prdN));
    } 

  }//merge

  // now write the collection
  ATH_CHECK( lock.addOrDelete(std::move( prdColl ) ) );
  ATH_MSG_DEBUG("PRD hash " << hash << " has been moved to container");

  return StatusCode::SUCCESS;
}


const MM_RawDataContainer* Muon::MmRdoToPrepDataToolCore::getRdoContainer() const
{
  auto rdoContainerHandle  = SG::makeHandle(m_rdoContainerKey);
  if(rdoContainerHandle.isValid()) {
    ATH_MSG_DEBUG("MM_getRdoContainer success");
    return rdoContainerHandle.cptr();  
  }
  ATH_MSG_WARNING("Retrieval of MM_RawDataContainer failed !");

  return nullptr;
}


void Muon::MmRdoToPrepDataToolCore::processRDOContainer( Muon::MMPrepDataContainer* mmPrepDataContainer,
                                                         const std::vector<IdentifierHash>& idsToDecode,
                                                         std::vector<IdentifierHash>& idWithDataVect ) const
{

  ATH_MSG_DEBUG("In processRDOContainer");
  const MM_RawDataContainer* rdoContainer = getRdoContainer();
  if (!rdoContainer) {
    return;
  }
  
  for (MM_RawDataContainer::const_iterator it = rdoContainer->begin(); it != rdoContainer->end(); ++it ) {
    
    auto rdoColl = *it;
    if (rdoColl->empty()) continue;
    ATH_MSG_DEBUG("New RDO collection with " << rdoColl->size() << "MM Hits");
  
    if(processCollection(mmPrepDataContainer, idsToDecode, rdoColl, idWithDataVect).isFailure()) {
      ATH_MSG_DEBUG("processCsm returns a bad StatusCode - keep going for new data collections in this event");
    }
  } 
  
  
  return;
}


// methods for ROB-based decoding
StatusCode Muon::MmRdoToPrepDataToolCore::decode( std::vector<IdentifierHash>& idVect, 
					       std::vector<IdentifierHash>& idWithDataVect ) const
{
  // clear the output vector of selected data
  idWithDataVect.clear();
  
  //is idVect a right thing to use here? to be reviewed maybe
  ATH_MSG_DEBUG("Size of the RDO container to be decoded: " << idVect.size() ); 

  Muon::MMPrepDataContainer* mmPrepDataContainer = setupMM_PrepDataContainer();

  if ( !mmPrepDataContainer ) {
    return StatusCode::FAILURE;
  } 

  processRDOContainer(mmPrepDataContainer, idVect, idWithDataVect);

  return StatusCode::SUCCESS;
} 

StatusCode Muon::MmRdoToPrepDataToolCore::decode( const std::vector<uint32_t>& robIds, 
					       const std::vector<IdentifierHash>& chamberHashInRobs ) const
{
  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );
  ATH_MSG_DEBUG("Size of the chamberHash" << chamberHashInRobs.size() );

  return StatusCode::SUCCESS;
}

StatusCode Muon::MmRdoToPrepDataToolCore::decode( const std::vector<uint32_t>& robIds ) const
{

  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );

  return StatusCode::SUCCESS;
}


// printout methods
void Muon::MmRdoToPrepDataToolCore::printInputRdo() const {


  return;
}


void Muon::MmRdoToPrepDataToolCore::printPrepData() const {


  return;
}

