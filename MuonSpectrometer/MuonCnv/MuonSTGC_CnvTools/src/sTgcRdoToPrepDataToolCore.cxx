/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "sTgcRdoToPrepDataToolCore.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

Muon::sTgcRdoToPrepDataToolCore::sTgcRdoToPrepDataToolCore(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_muonMgr(0),
  m_fullEventDone(false),
  m_stgcPrepDataContainer(0),
  m_clusterBuilderTool("Muon::SimpleSTgcClusterBuilderTool/SimpleSTgcClusterBuilderTool",this)
{
  declareInterface<Muon::IMuonRdoToPrepDataTool>(this);
  
  //  template for property decalration
  declareProperty("OutputCollection",    m_stgcPrepDataContainerKey = std::string("STGC_Measurements"),
		  "Muon::sTgcPrepDataContainer to record");
  declareProperty("InputCollection",    m_rdoContainerKey = std::string("sTGCRDO"),
		  "RDO container to read");
  declareProperty("Merge",  m_merge = true);
  declareProperty("ClusterBuilderTool",m_clusterBuilderTool);
}

StatusCode Muon::sTgcRdoToPrepDataToolCore::initialize()
{  
  ATH_MSG_DEBUG(" in initialize()");
  
  /// get the detector descriptor manager
  StatusCode sc = detStore()->retrieve( m_muonMgr );
  if (sc.isFailure()) {
    ATH_MSG_FATAL(" Cannot retrieve MuonReadoutGeometry ");
    return sc;
  }
  
  ATH_CHECK( m_muonIdHelperTool.retrieve() );

  // check if the initialization of the data container is success
  ATH_CHECK(m_stgcPrepDataContainerKey.initialize());

  ATH_CHECK(m_rdoContainerKey.initialize());


  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::sTgcRdoToPrepDataToolCore::finalize()
{
  return StatusCode::SUCCESS;

}

StatusCode Muon::sTgcRdoToPrepDataToolCore::processCollection(const STGC_RawDataCollection *rdoColl, std::vector<IdentifierHash>& idWithDataVect)
{

  const IdentifierHash hash = rdoColl->identifyHash();

  ATH_MSG_DEBUG(" ***************** Start of process STGC Collection with hash Id: " << hash);

  sTgcPrepDataCollection* prdColl = nullptr;
  
  // check if the collection already exists, otherwise add it
  if ( m_stgcPrepDataContainer->indexFind(hash) != m_stgcPrepDataContainer->end() ) {

    ATH_MSG_DEBUG("In processCollection: collection already contained in the MM PrepData container");
    return StatusCode::FAILURE;
  } 
  else {
    prdColl = new sTgcPrepDataCollection(hash);
    idWithDataVect.push_back(hash);

    // set the offline identifier of the collection Id
    IdContext context = m_muonIdHelperTool->stgcIdHelper().module_context();
    Identifier moduleId;
    int getId = m_muonIdHelperTool->stgcIdHelper().get_id(hash,moduleId,&context);
    if ( getId != 0 ) {
      ATH_MSG_ERROR("Could not convert the hash Id: " << hash << " to identifier");
    } 
    else {
      prdColl->setIdentifier(moduleId);
    }

    if (StatusCode::SUCCESS != m_stgcPrepDataContainer->addCollection(prdColl, hash)) {
      ATH_MSG_DEBUG("In processCollection - Couldn't record in the Container MM Collection with hashID = "
		    << (int)hash );
      return StatusCode::FAILURE;
    }

  }

  std::vector<sTgcPrepData> sTgcStripPrds;
  std::vector<sTgcPrepData> sTgcWirePrds;
  std::vector<sTgcPrepData> sTgcPadPrds;
  // convert the RDO collection to a PRD collection
  STGC_RawDataCollection::const_iterator it = rdoColl->begin();
  for ( ; it != rdoColl->end() ; ++it ) {

    ATH_MSG_DEBUG("Adding a new sTgc PrepRawData");

    const STGC_RawData* rdo = *it;
    const Identifier rdoId = rdo->identify();
    std::vector<Identifier> rdoList;
    rdoList.push_back(rdoId);
    
    // get the local and global positions
    const MuonGM::sTgcReadoutElement* detEl = m_muonMgr->getsTgcReadoutElement(rdoId);
    Amg::Vector2D localPos;

    bool getLocalPos = detEl->stripPosition(rdoId,localPos);
    if ( !getLocalPos ) {
      ATH_MSG_ERROR("Could not get the local strip position for sTgc");
      return StatusCode::FAILURE;
    } 

    // get the resolution from strip width
    const int gasGap  = m_muonIdHelperTool->stgcIdHelper().gasGap(rdoId);
    const int channel = m_muonIdHelperTool->stgcIdHelper().channel(rdoId);

    const int charge = (int) rdo->charge();
    const int rdoTime = (int) rdo->time();
    const uint16_t bcTag = rdo->bcTag();

    // to be fixed: for now do not set the resolution, it will be added in the 
    // next update
    double width = 0.;

    int channelType = m_muonIdHelperTool->stgcIdHelper().channelType(rdoId);

    ATH_MSG_DEBUG("Adding a new STGC PRD, gasGap: " << gasGap << " channel: " << channel 
		  << " type: " << channelType );

    if (channelType ==0) { // Pads
      const MuonGM::MuonPadDesign* design = detEl->getPadDesign(rdoId);
      if (!design) {
        ATH_MSG_WARNING("Failed to get design for sTGC pad" );
      } else {
        double widthOLD = design->channelWidth(localPos,true);  //channelWidth returns the Phi Angle for pads, does not account for staggering.
        width = design->inputPhiPitch*M_PI/180.*(detEl->globalPosition().perp());
        ATH_MSG_DEBUG( " Pad old width " << widthOLD << " new width " << width); 
      } 
    } else if (channelType == 1) { // Strips 
      const MuonGM::MuonChannelDesign* design = detEl->getDesign(rdoId);
      if (!design) ATH_MSG_WARNING("Failed to get design for sTGC strip " );
      else 
        width = design->inputPitch;
    } else if (channelType == 2) { // Wires
      const MuonGM::MuonChannelDesign* design = detEl->getDesign(rdoId);
      if (!design) {
        ATH_MSG_WARNING("Failed to get design for sTGC wire" );
      } else { 
        double widthOLD = design->channelWidth(localPos);
// scale error 
        width = design->groupWidth*fabs(design->inputPitch);
        ATH_MSG_DEBUG( " Wires old width " << widthOLD << " new width " << width); 
      } 
   }

    else {
       ATH_MSG_ERROR("Invalid channel Type for sTGC");
       return StatusCode::FAILURE;
    }

    double resolution = width/sqrt(12.); 

    Amg::MatrixX* cov = new Amg::MatrixX(1,1);
    cov->setIdentity();
    (*cov)(0,0) = resolution*resolution;  

    ATH_MSG_DEBUG("Adding a new STGC PRD, gasGap: " << gasGap << " channel: " << channel << " type: " << channelType << " resolution " << resolution );

    if(m_merge) {
      // eta strips 

      if (channelType==0) {
        //
        // check if there is already a pad with smaller time
        //
        bool addPad = true;
        for ( const auto& it : sTgcPadPrds ) { 
          if ( it.identify()==rdoId && it.time()<rdoTime ) {
            addPad = false;
          }
        }
        if (addPad) {
          sTgcPadPrds.emplace_back(rdoId,hash,localPos,rdoList,cov,detEl,charge,rdoTime,bcTag);
        }
      } 
      else if(channelType==1) {
        //
        // check if the same strip is already present, with a smaller time
        //
        bool addStrip = true;
        for ( const auto&  it : sTgcStripPrds ) {
          if ( it.identify()==rdoId && it.time()<rdoTime ) {
            addStrip = false;
          }
        } 
        if ( addStrip ) {  
          sTgcStripPrds.emplace_back(rdoId,hash,localPos,rdoList,cov,detEl,charge,rdoTime,bcTag);
        }
      } 
      else if (channelType==2) { 
        //
        // check if the same strip is already present, with a smaller time
        //
        bool addWire = true;
        for ( const auto& it : sTgcWirePrds ) {
          if ( it.identify()==rdoId && it.time()<rdoTime ) {
            addWire = false;
          }
        } 
        // wires
        if ( addWire ) { 
          sTgcWirePrds.push_back(sTgcPrepData(rdoId,hash,localPos,rdoList,cov,detEl,charge,rdoTime,bcTag));
        }
      } 
      else {
        ATH_MSG_ERROR("Unknown sTGC channel type");
        return StatusCode::FAILURE;
      }
    } else {
      // 
      // if not merging just add the PRD to the collection
      //
      prdColl->push_back(new sTgcPrepData(rdoId,hash,localPos,rdoList,cov,detEl,charge,rdoTime,bcTag));
    } 
  } //end it = rdoColl

  if(m_merge) {
    // merge the eta and phi prds that fire closeby strips or wires
    std::vector<Muon::sTgcPrepData*> sTgcStripClusters;
    std::vector<Muon::sTgcPrepData*> sTgcWireClusters;
    //
    // Clusterize strips
    //
    ATH_CHECK(m_clusterBuilderTool->getClusters(sTgcStripPrds,sTgcStripClusters));
    //
    // Clusterize wires
    //
    ATH_CHECK(m_clusterBuilderTool->getClusters(sTgcWirePrds,sTgcWireClusters));
    //
    // Add the clusters to the event store ( do not clusterize wires for now )
    //
    for ( auto it : sTgcStripClusters ) {
      it->setHashAndIndex(prdColl->identifyHash(), prdColl->size());
      prdColl->push_back(it);
    } 
    for ( auto it : sTgcWireClusters ) {
      it->setHashAndIndex(prdColl->identifyHash(), prdColl->size());
      prdColl->push_back(it);
    } 
  }


  // clear vector and delete elements
  sTgcStripPrds.clear();
  sTgcWirePrds.clear();
  sTgcPadPrds.clear();

  return StatusCode::SUCCESS;
}


Muon::sTgcRdoToPrepDataToolCore::SetupSTGC_PrepDataContainerStatus Muon::sTgcRdoToPrepDataToolCore::setupSTGC_PrepDataContainer() 
{

  if(!evtStore()->contains<Muon::sTgcPrepDataContainer>(m_stgcPrepDataContainerKey.key())){    
    m_fullEventDone=false;
    
    SG::WriteHandle< Muon::sTgcPrepDataContainer > handle(m_stgcPrepDataContainerKey);
    StatusCode status = handle.record(std::make_unique<Muon::sTgcPrepDataContainer>(m_muonIdHelperTool->stgcIdHelper().module_hash_max()));
    
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of STGC PrepData Container at " << m_stgcPrepDataContainerKey.key()); 
      return FAILED;
    }
    m_stgcPrepDataContainer = handle.ptr();
    return ADDED;
    
  }
  return ALREADYCONTAINED;
}


const STGC_RawDataContainer* Muon::sTgcRdoToPrepDataToolCore::getRdoContainer() {

  auto rdoContainerHandle  = SG::makeHandle(m_rdoContainerKey);
  if(rdoContainerHandle.isValid()) {
    ATH_MSG_DEBUG("STGC_getRdoContainer success");
    return rdoContainerHandle.cptr();  
  }
  ATH_MSG_WARNING("Retrieval of STGC_RawDataContainer failed !");

  return nullptr;
}


void Muon::sTgcRdoToPrepDataToolCore::processRDOContainer( std::vector<IdentifierHash>& idWithDataVect ) 
{


  ATH_MSG_DEBUG("In processRDOContainer");
  const STGC_RawDataContainer* rdoContainer = getRdoContainer();
  if (!rdoContainer) {
    return;
  }
  
  // run in unseeded mode
  for (STGC_RawDataContainer::const_iterator it = rdoContainer->begin(); it != rdoContainer->end(); ++it ) {
    
    auto rdoColl = *it;
    if (rdoColl->empty()) continue;
    ATH_MSG_DEBUG("New RDO collection with " << rdoColl->size() << "STGC Hits");

    if(processCollection(rdoColl, idWithDataVect).isFailure()) {
      ATH_MSG_DEBUG("processCsm returns a bad StatusCode - keep going for new data collections in this event");
    }
  } 
  
  
  return;
}


// methods for ROB-based decoding
StatusCode Muon::sTgcRdoToPrepDataToolCore::decode( std::vector<IdentifierHash>& idVect, 
						std::vector<IdentifierHash>& idWithDataVect )
{

  ATH_MSG_DEBUG("Size of the input hash id vector: " << idVect.size());

  // clear the output vector of selected data
  idWithDataVect.clear();
  
  SetupSTGC_PrepDataContainerStatus containerRecordStatus = setupSTGC_PrepDataContainer();

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

StatusCode Muon::sTgcRdoToPrepDataToolCore::decode( const std::vector<uint32_t>& robIds )
{

  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );

  return StatusCode::SUCCESS;
}

// printout methods
void Muon::sTgcRdoToPrepDataToolCore::printInputRdo() {


  return;
}


void Muon::sTgcRdoToPrepDataToolCore::printPrepData() {


  return;
}

