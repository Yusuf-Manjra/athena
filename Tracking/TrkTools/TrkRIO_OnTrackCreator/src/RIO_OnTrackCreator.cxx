/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RIO_OnTrackCreator.cxx
//   AlgTool for adapting a RIO to a track candidate. It
//   automatically selects the corresponding subdet correction.
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 2004 - now: Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
///////////////////////////////////////////////////////////////////

// --- the base class
#include "TrkRIO_OnTrackCreator/RIO_OnTrackCreator.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
// --- Gaudi stuff
#include "GaudiKernel/ListItem.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"

/** il costruttore
 */
Trk::RIO_OnTrackCreator::RIO_OnTrackCreator(const std::string& t,
			      const std::string& n,
			      const IInterface* p)
  :  AthAlgTool(t,n,p),
     m_idHelper(nullptr),
     m_pixClusCor        ("InDet::PixelClusterOnTrackTool/PixelClusterOnTrackTool"),
     m_sctClusCor        ("InDet::SCT_ClusterOnTrackTool/SCT_ClusterOnTrackTool", this),
     m_trt_Cor           ("InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"),
     m_muonDriftCircleCor("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackTool"),
     m_muonClusterCor    ("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackTool"),
     m_doPixel(true),
     m_doSCT(true),
     m_doTRT(true)
 {
   m_mode     = "all";
   m_nwarning = new int(0);
   declareInterface<IRIO_OnTrackCreator>(this);
   declareProperty("ToolPixelCluster"   ,m_pixClusCor);
   declareProperty("ToolSCT_Cluster"    ,m_sctClusCor);
   declareProperty("ToolTRT_DriftCircle",m_trt_Cor);
   declareProperty("ToolMuonDriftCircle",m_muonDriftCircleCor);
   declareProperty("ToolMuonCluster"    ,m_muonClusterCor);
   declareProperty("Mode"               ,m_mode);
 }

// destructor
Trk::RIO_OnTrackCreator::~RIO_OnTrackCreator()
{
  delete m_nwarning;
} 

// initialise
StatusCode Trk::RIO_OnTrackCreator::initialize()
{
  if (AlgTool::initialize().isFailure()) return StatusCode::FAILURE; 

  if (m_mode != "all" && m_mode != "indet" &&m_mode != "muon") {
    msg(MSG::FATAL) << "Mode is set to unknown value " << m_mode << endmsg;
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( " RIO_OnTrackCreator job configuration:" << std::endl
		 << std::endl << " (i) The following RIO correction "
		 << "tools configured (depends on mode = "<< m_mode <<"):" << std::endl
		 << "     Pixel      : " << m_pixClusCor << std::endl
		 << "     SCT        : " << m_sctClusCor << std::endl
		 << "     TRT        : " << m_trt_Cor       << std::endl
		 << "     MDT        : " << m_muonDriftCircleCor << std::endl
		 << "     CSC/RPC/TGC: " << m_muonClusterCor << std::endl
       );

  // Get the correction tool to create Pixel/SCT/TRT RIO_onTrack

  if (m_mode == "all" || m_mode == "indet") {

    if (!m_pixClusCor.empty()) {
      if ( m_pixClusCor.retrieve().isFailure() ) {
	      ATH_MSG_FATAL( "Failed to retrieve tool " << m_pixClusCor);
	      return StatusCode::FAILURE;
      } 
	      ATH_MSG_INFO( "Retrieved tool " << m_pixClusCor);
      
    } else {
      m_doPixel = false;
    }

    if (!m_sctClusCor.empty()) {
      if ( m_sctClusCor.retrieve().isFailure() ) {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_sctClusCor);
	      return StatusCode::FAILURE;
      } 
	      ATH_MSG_INFO( "Retrieved tool " << m_sctClusCor);
      
    } else {
      m_doSCT = false;
    }

    if (!m_trt_Cor.empty()) {
      if ( m_trt_Cor.retrieve().isFailure() ) {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trt_Cor);
	      return StatusCode::FAILURE;
      } 
	      ATH_MSG_INFO( "Retrieved tool " << m_trt_Cor);
      
    } else {
      m_doTRT = false;
    }
  } else {
    m_trt_Cor.disable();
    m_pixClusCor.disable();
    m_sctClusCor.disable();
  }

  if (m_mode == "all" || m_mode == "muon") {
    if ( m_muonDriftCircleCor.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_muonDriftCircleCor);      
      return StatusCode::FAILURE;
    } 
      ATH_MSG_INFO( "Retrieved tool " << m_muonDriftCircleCor);
    

    if ( m_muonClusterCor.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_muonClusterCor);            
      return StatusCode::FAILURE;
    } 
      ATH_MSG_INFO( "Retrieved tool " << m_muonClusterCor);
    
  }
  else{
    m_muonClusterCor.disable();
    m_muonDriftCircleCor.disable();
  }
  
  // Set up ATLAS ID helper to be able to identify the RIO's det-subsystem.
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

// finalise
StatusCode Trk::RIO_OnTrackCreator::finalize()
{
  ATH_MSG_INFO("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}

// The sub-detector brancher algorithm
const Trk::RIO_OnTrack* 
Trk::RIO_OnTrackCreator::correct(const Trk::PrepRawData& rio,
                                 const TrackParameters& trk) const
{

  Identifier id;
  id = rio.identify();

  // --- print RIO
  ATH_MSG_VERBOSE ("RIO ID prints as "<<m_idHelper->print_to_string(id));
  ATH_MSG_VERBOSE ("RIO.locP = ("<<rio.localPosition().x()<<","<<rio.localPosition().y()<<")");

  if (m_doPixel && m_idHelper->is_pixel(id)) {
    if (m_mode == "muon") {
      ATH_MSG_WARNING("I have no tool to correct the current Pixel hit! - Giving back nil.");
      return nullptr;
    } 
      ATH_MSG_DEBUG ("RIO identified as PixelCluster.");
      return m_pixClusCor->correct(rio, trk);
    
  }

  if (m_doSCT && m_idHelper->is_sct(id)) {
    if (m_mode == "muon") {
      ATH_MSG_WARNING("I have no tool to correct the current SCT hit! - Giving back nil.");
      return nullptr;
    } 
      ATH_MSG_DEBUG ("RIO identified as SCT_Cluster.");
      return m_sctClusCor->correct(rio, trk);
    
  }

  if (m_doTRT && m_idHelper->is_trt(id)) {
    if (m_mode == "muon") {
      ATH_MSG_WARNING("I have no tool to correct a TRT DriftCircle! - Giving back nil.");
      return nullptr;
    } 
      ATH_MSG_DEBUG ("RIO identified as TRT_DriftCircle.");
      return m_trt_Cor->correct(rio, trk);
    
  }

  if (m_idHelper->is_mdt(id)){
    if (m_mode == "indet") {
      ATH_MSG_WARNING("I have no tool to correct a MDT DriftCircle! - Giving back nil.");
      return nullptr;
    } 
      ATH_MSG_DEBUG ("RIO identified as MuonDriftCircle.");
      return m_muonDriftCircleCor->correct(rio, trk);
    
  }
  
  if ( (m_idHelper->is_csc(id)) || (m_idHelper->is_rpc(id))
       || (m_idHelper->is_tgc(id)) || (m_idHelper->is_mm(id)) || (m_idHelper->is_stgc(id)) ) {
    if (m_mode == "indet") {
      ATH_MSG_WARNING("I have no tool to correct a CSC/RPC/TGC hit! - Giving back nil.");
      return nullptr;
    } 
      ATH_MSG_DEBUG ("RIO identified as MuonCluster.");
      return m_muonClusterCor->correct(rio, trk);
    
  }

  ATH_MSG_WARNING( "idHelper could not identify sub-detector for: "<<m_idHelper->print_to_string(id)<<". Return nil RIO_OnTrack");
  return nullptr;
}
