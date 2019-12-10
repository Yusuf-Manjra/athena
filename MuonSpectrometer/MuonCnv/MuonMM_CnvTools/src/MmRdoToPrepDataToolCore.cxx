/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtRdoToPrepDataTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MmRdoToPrepDataToolCore.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "TrkEventPrimitives/LocalDirection.h"

// BS access
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include "MMClusterization/IMMClusterBuilderTool.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

Muon::MmRdoToPrepDataToolCore::MmRdoToPrepDataToolCore(const std::string& t,
					       const std::string& n,
					       const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_muonMgr(0),
  m_fullEventDone(false),
  m_mmPrepDataContainer(0),
  m_clusterBuilderTool("Muon::SimpleMMClusterBuilderTool/SimpleMMClusterBuilderTool",this)
{
  declareInterface<Muon::IMuonRdoToPrepDataTool>(this);

  //  template for property decalration
  declareProperty("OutputCollection",    m_mmPrepDataContainerKey = std::string("MM_Measurements"),
		  "Muon::MMPrepDataContainer to record");
  declareProperty("InputCollection",    m_rdoContainerKey = std::string("MMRDO"),
		  "Muon::MMPrepDataContainer to record");
  
  declareProperty("MergePrds", m_merge = true);
  declareProperty("ClusterBuilderTool",m_clusterBuilderTool);
}


Muon::MmRdoToPrepDataToolCore::~MmRdoToPrepDataToolCore()
{

}

StatusCode Muon::MmRdoToPrepDataToolCore::initialize()
{  
  ATH_MSG_DEBUG(" in initialize()");
  
  StatusCode sc = detStore()->retrieve( m_muonMgr );
  if (sc.isFailure()) {
    ATH_MSG_FATAL(" Cannot retrieve MuonReadoutGeometry ");
    return sc;
  }
  
  ATH_CHECK( m_muonIdHelperTool.retrieve() );

  // check if the initialization of the data container is success
  ATH_CHECK(m_mmPrepDataContainerKey.initialize());
  ATH_CHECK(m_rdoContainerKey.initialize());

  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::MmRdoToPrepDataToolCore::finalize()
{
  //  if (0 != m_mmPrepDataContainer) m_mmPrepDataContainer->release();
  return StatusCode::SUCCESS;

}

StatusCode Muon::MmRdoToPrepDataToolCore::processCollection(const MM_RawDataCollection *rdoColl, 
							std::vector<IdentifierHash>& idWithDataVect)
{
  ATH_MSG_DEBUG(" ***************** Start of process MM Collection");

  bool merge = m_merge;
// protect for large splashes 
  if(rdoColl->size()>100) merge = true; 

  const IdentifierHash hash = rdoColl->identifierHash();

  MMPrepDataCollection* prdColl = nullptr;
  
  // check if the collection already exists, otherwise add it
  if ( m_mmPrepDataContainer->indexFind(hash) != m_mmPrepDataContainer->end() ) {

    ATH_MSG_DEBUG("In processCollection: collection already contained in the MM PrepData container");
    return StatusCode::FAILURE;
  } 
  else {
    prdColl = new MMPrepDataCollection(hash);
    idWithDataVect.push_back(hash);
    
    // set the offline identifier of the collection Id
    IdContext context = m_muonIdHelperTool->mmIdHelper().module_context();
    Identifier moduleId;
    int getId = m_muonIdHelperTool->mmIdHelper().get_id(hash,moduleId,&context);
    if ( getId != 0 ) {
      ATH_MSG_ERROR("Could not convert the hash Id: " << hash << " to identifier");
    } 
    else {
      ATH_MSG_DEBUG(" dump moduleId " << moduleId );
      prdColl->setIdentifier(moduleId);
    }

    if (StatusCode::SUCCESS != m_mmPrepDataContainer->addCollection(prdColl, hash)) {
      ATH_MSG_DEBUG("In processCollection - Couldn't record in the Container MM Collection with hashID = "
		    << (int)hash );
      return StatusCode::FAILURE;
    }

  }

  std::vector<MMPrepData> MMprds;
  // convert the RDO collection to a PRD collection
  MM_RawDataCollection::const_iterator it = rdoColl->begin();
  for ( ; it != rdoColl->end() ; ++it ) {

    ATH_MSG_DEBUG("Adding a new MM PrepRawData");

    const MM_RawData* rdo = *it;
    const Identifier rdoId = rdo->identify();
//    const Identifier elementId = m_muonIdHelperTool->mmIdHelper().elementID(rdoId);
    ATH_MSG_DEBUG(" dump rdo " << m_muonIdHelperTool->toString(rdoId ));
    const int time = rdo->time();
    const int charge = rdo->charge();
    int channel = rdo->channel();
    std::vector<Identifier> rdoList;
    Identifier parentID = m_muonIdHelperTool->mmIdHelper().parentID(rdoId);
    Identifier layid = m_muonIdHelperTool->mmIdHelper().channelID(parentID, m_muonIdHelperTool->mmIdHelper().multilayer(rdoId), m_muonIdHelperTool->mmIdHelper().gasGap(rdoId),1);
    Identifier prdId = m_muonIdHelperTool->mmIdHelper().channelID(parentID, m_muonIdHelperTool->mmIdHelper().multilayer(rdoId), m_muonIdHelperTool->mmIdHelper().gasGap(rdoId),channel);
    ATH_MSG_DEBUG(" channel RDO " << channel << " channel from rdoID " << m_muonIdHelperTool->mmIdHelper().channel(rdoId));
    rdoList.push_back(prdId);

    // get the local and global positions
    const MuonGM::MMReadoutElement* detEl = m_muonMgr->getMMReadoutElement(layid);
    Amg::Vector2D localPos;

    bool getLocalPos = detEl->stripPosition(prdId,localPos);
    if ( !getLocalPos ) {
      ATH_MSG_WARNING("Could not get the local strip position for MM");
      continue;
    }
    int stripNumberRDOId = detEl->stripNumber(localPos,layid);
    ATH_MSG_DEBUG(" check strip nr RDOId " << stripNumberRDOId );
    Amg::Vector3D globalPos;
    bool getGlobalPos = detEl->stripGlobalPosition(prdId,globalPos);
    if ( !getGlobalPos ) {
      ATH_MSG_WARNING("Could not get the global strip position for MM");
      continue;
    }

//    const Trk::Surface& surf = detEl->surface(rdoId);
//    const Amg::Vector3D* globalPos = surf.localToGlobal(localPos);
    const Amg::Vector3D globalDir(globalPos.x(), globalPos.y(), globalPos.z());
    Trk::LocalDirection localDir;
    const Trk::PlaneSurface& psurf = detEl->surface(layid);
    Amg::Vector2D lpos;
    psurf.globalToLocal(globalPos,globalPos,lpos);
    psurf.globalToLocalDirection(globalDir, localDir);
    float inAngle_XZ = fabs( localDir.angleXZ() / CLHEP::degree);
   
    ATH_MSG_DEBUG(" Surface centre x " << psurf.center().x() << " y " << psurf.center().y() << " z " << psurf.center().z() );
    ATH_MSG_DEBUG(" localPos x " << localPos.x() << " localPos y " << localPos.y() << " lpos recalculated 0 " << lpos[0] << " lpos y " << lpos[1]);

    Amg::Vector3D  gdir = psurf.transform().linear()*Amg::Vector3D(0.,1.,0.);
    ATH_MSG_DEBUG(" MM detector surface direction phi " << gdir.phi() << " global radius hit " << globalPos.perp() << " phi pos " << globalPos.phi() << " global z " << globalPos.z());      

    // get for now a temporary error matrix -> to be fixed
    double resolution = 0.07;
    if (fabs(inAngle_XZ)>3) resolution = ( -.001/3.*fabs(inAngle_XZ) ) + .28/3.;
    double errX = 0;
    const MuonGM::MuonChannelDesign* design = detEl->getDesign(layid);
    if( !design ){
      ATH_MSG_WARNING("Failed to get design for " << m_muonIdHelperTool->toString(layid) );
    }else{
      errX = fabs(design->inputPitch)/sqrt(12);
      ATH_MSG_DEBUG(" strips inputPitch " << design->inputPitch << " error " << errX);
    }
// add strip width to error
    resolution = sqrt(resolution*resolution+errX*errX);

    Amg::MatrixX* cov = new Amg::MatrixX(1,1);
    cov->setIdentity();
    (*cov)(0,0) = resolution*resolution;  

    if(!merge) {
      prdColl->push_back(new MMPrepData(prdId,hash,localPos,rdoList,cov,detEl,time,charge));
    } else {
       MMPrepData mpd = MMPrepData(prdId,hash,localPos,rdoList,cov,detEl,time,charge);
       // set the hash of the MMPrepData such that it contains the correct value in case it gets used in SimpleMMClusterBuilderTool::getClusters
       mpd.setHashAndIndex(hash,0);
       MMprds.push_back(mpd);
    } 
  }

  if(merge) {
    std::vector<MMPrepData*> clusters;

    /// reconstruct the clusters
    ATH_CHECK(m_clusterBuilderTool->getClusters(MMprds,clusters));

    for (unsigned int i = 0 ; i<clusters.size() ; ++i ) {
      MMPrepData* prdN = clusters.at(i);
      prdN->setHashAndIndex(prdColl->identifyHash(), prdColl->size());
      prdColl->push_back(prdN);
    } 

  }



  return StatusCode::SUCCESS;
}


Muon::MmRdoToPrepDataToolCore::SetupMM_PrepDataContainerStatus Muon::MmRdoToPrepDataToolCore::setupMM_PrepDataContainer() 
{
  return FAILED;
}


const MM_RawDataContainer* Muon::MmRdoToPrepDataToolCore::getRdoContainer() {

  auto rdoContainerHandle  = SG::makeHandle(m_rdoContainerKey);
  if(rdoContainerHandle.isValid()) {
    ATH_MSG_DEBUG("MM_getRdoContainer success");
    return rdoContainerHandle.cptr();  
  }
  ATH_MSG_WARNING("Retrieval of MM_RawDataContainer failed !");

  return nullptr;
}


void Muon::MmRdoToPrepDataToolCore::processRDOContainer( std::vector<IdentifierHash>& idWithDataVect ) 
{

  ATH_MSG_DEBUG("In processRDOContainer");
  const MM_RawDataContainer* rdoContainer = getRdoContainer();
  if (!rdoContainer) {
    return;
  }
  
  // run in unseeded mode
  for (MM_RawDataContainer::const_iterator it = rdoContainer->begin(); it != rdoContainer->end(); ++it ) {
    
    auto rdoColl = *it;
    if (rdoColl->empty()) continue;
    ATH_MSG_DEBUG("New RDO collection with " << rdoColl->size() << "MM Hits");
    if(processCollection(rdoColl, idWithDataVect).isFailure()) {
      ATH_MSG_DEBUG("processCsm returns a bad StatusCode - keep going for new data collections in this event");
    }
  } 
  
  
  return;
}


// methods for ROB-based decoding
StatusCode Muon::MmRdoToPrepDataToolCore::decode( std::vector<IdentifierHash>& idVect, 
					       std::vector<IdentifierHash>& idWithDataVect )
{
  // clear the output vector of selected data
  idWithDataVect.clear();
  
  //is idVect a right thing to use here? to be reviewed maybe
  ATH_MSG_DEBUG("Size of the RDO container to be decoded: " << idVect.size() ); 

  SetupMM_PrepDataContainerStatus containerRecordStatus = setupMM_PrepDataContainer();

  if ( containerRecordStatus == FAILED ) {
    return StatusCode::FAILURE;
  } 

  processRDOContainer(idWithDataVect);

  // check if the full event has already been decoded
  if ( m_fullEventDone ) {
    ATH_MSG_DEBUG ("Full event dcoded, nothing to do");
    return StatusCode::SUCCESS;
  } 

 
  return StatusCode::SUCCESS;
} 

StatusCode Muon::MmRdoToPrepDataToolCore::decode( const std::vector<uint32_t>& robIds, 
					       const std::vector<IdentifierHash>& chamberHashInRobs )
{
  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );
  ATH_MSG_DEBUG("Size of the chamberHash" << chamberHashInRobs.size() );

  return StatusCode::SUCCESS;
}

StatusCode Muon::MmRdoToPrepDataToolCore::decode( const std::vector<uint32_t>& robIds )
{

  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );

  return StatusCode::SUCCESS;
}


// printout methods
void Muon::MmRdoToPrepDataToolCore::printInputRdo() {


  return;
}


void Muon::MmRdoToPrepDataToolCore::printPrepData() {


  return;
}

