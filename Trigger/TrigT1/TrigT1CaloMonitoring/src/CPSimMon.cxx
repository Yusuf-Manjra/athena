/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CPSimMon.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHOR:   Peter Faulkner
//           
//
// ********************************************************************

#include <utility>
#include <cmath>

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigT1CaloEvent/CMXCPHits.h"
#include "TrigT1CaloEvent/CMXCPTob.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPMTobRoI.h"
#include "TrigT1CaloEvent/RODHeader.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/CPAlgorithm.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloToolInterfaces/IL1EmTauTools.h"
#include "TrigT1CaloToolInterfaces/IL1CPCMXTools.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"  
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

#include "CPSimMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
CPSimMon::CPSimMon(const std::string & type, 
	  	   const std::string & name,
		   const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_emTauTool("LVL1::L1EmTauTools/L1EmTauTools"),
    m_cpCmxTool("LVL1::L1CPCMXTools/L1CPCMXTools"),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),   
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"), 
    m_debug(false), m_rodTES(0), m_overlapPresent(false), m_limitedRoi(0),
    m_histBooked(false),
    m_h_cpm_em_2d_etaPhi_tt_PpmEqCore(0),
    m_h_cpm_em_2d_etaPhi_tt_PpmNeCore(0),
    m_h_cpm_em_2d_etaPhi_tt_PpmNoCore(0),
    m_h_cpm_em_2d_etaPhi_tt_CoreNoPpm(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmEqCore(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmNeCore(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmNoCore(0),
    m_h_cpm_had_2d_etaPhi_tt_CoreNoPpm(0),
    m_h_cpm_em_2d_etaPhi_tt_PpmEqOverlap(0),
    m_h_cpm_em_2d_etaPhi_tt_PpmNeOverlap(0),
    m_h_cpm_em_2d_etaPhi_tt_PpmNoOverlap(0),
    m_h_cpm_em_2d_etaPhi_tt_OverlapNoPpm(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmEqOverlap(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmNeOverlap(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmNoOverlap(0),
    m_h_cpm_had_2d_etaPhi_tt_OverlapNoPpm(0),
    m_h_cpm_2d_tt_PpmEqCpmFpga(0),
    m_h_cpm_2d_tt_PpmNeCpmFpga(0),
    m_h_cpm_2d_tt_PpmNoCpmFpga(0),
    m_h_cpm_2d_tt_CpmNoPpmFpga(0),
    m_h_cpm_2d_roi_EmEnergySimEqData(0),
    m_h_cpm_2d_roi_EmEnergySimNeData(0),
    m_h_cpm_2d_roi_EmEnergySimNoData(0),
    m_h_cpm_2d_roi_EmEnergyDataNoSim(0),
    m_h_cpm_2d_roi_TauEnergySimEqData(0),
    m_h_cpm_2d_roi_TauEnergySimNeData(0),
    m_h_cpm_2d_roi_TauEnergySimNoData(0),
    m_h_cpm_2d_roi_TauEnergyDataNoSim(0),
    m_h_cpm_2d_roi_EmIsolSimEqData(0),
    m_h_cpm_2d_roi_EmIsolSimNeData(0),
    m_h_cpm_2d_roi_EmIsolSimNoData(0),
    m_h_cpm_2d_roi_EmIsolDataNoSim(0),
    m_h_cpm_2d_roi_TauIsolSimEqData(0),
    m_h_cpm_2d_roi_TauIsolSimNeData(0),
    m_h_cpm_2d_roi_TauIsolSimNoData(0),
    m_h_cpm_2d_roi_TauIsolDataNoSim(0),
    m_h_cpm_2d_etaPhi_roi_SimEqData(0),
    m_h_cpm_2d_etaPhi_roi_SimNeData(0),
    m_h_cpm_2d_etaPhi_roi_SimNoData(0),
    m_h_cpm_2d_etaPhi_roi_DataNoSim(0),
    m_h_cmx_2d_tob_LeftEnergySimEqData(0),
    m_h_cmx_2d_tob_LeftEnergySimNeData(0),
    m_h_cmx_2d_tob_LeftEnergySimNoData(0),
    m_h_cmx_2d_tob_LeftEnergyDataNoSim(0),
    m_h_cmx_2d_tob_RightEnergySimEqData(0),
    m_h_cmx_2d_tob_RightEnergySimNeData(0),
    m_h_cmx_2d_tob_RightEnergySimNoData(0),
    m_h_cmx_2d_tob_RightEnergyDataNoSim(0),
    m_h_cmx_2d_tob_LeftIsolSimEqData(0),
    m_h_cmx_2d_tob_LeftIsolSimNeData(0),
    m_h_cmx_2d_tob_LeftIsolSimNoData(0),
    m_h_cmx_2d_tob_LeftIsolDataNoSim(0),
    m_h_cmx_2d_tob_RightIsolSimEqData(0),
    m_h_cmx_2d_tob_RightIsolSimNeData(0),
    m_h_cmx_2d_tob_RightIsolSimNoData(0),
    m_h_cmx_2d_tob_RightIsolDataNoSim(0),
    m_h_cmx_2d_tob_OverflowSimEqData(0),
    m_h_cmx_2d_tob_OverflowSimNeData(0),
    m_h_cmx_2d_etaPhi_tob_SimEqData(0),
    m_h_cmx_2d_etaPhi_tob_SimNeData(0),
    m_h_cmx_2d_etaPhi_tob_SimNoData(0),
    m_h_cmx_2d_etaPhi_tob_DataNoSim(0),
    m_h_cmx_1d_thresh_SumsSimEqData(0),
    m_h_cmx_1d_thresh_SumsSimNeData(0),
    m_h_cmx_1d_thresh_SumsSimNoData(0),
    m_h_cmx_1d_thresh_SumsDataNoSim(0),
    m_h_cmx_2d_thresh_LeftSumsSimEqData(0),
    m_h_cmx_2d_thresh_LeftSumsSimNeData(0),
    m_h_cmx_2d_thresh_RightSumsSimEqData(0),
    m_h_cmx_2d_thresh_RightSumsSimNeData(0),
    m_h_cmx_2d_topo_SimEqData(0),
    m_h_cmx_2d_topo_SimNeData(0),
    m_h_cmx_2d_topo_SimNoData(0),
    m_h_cmx_2d_topo_DataNoSim(0),
    m_h_cpm_2d_SimEqDataOverview(0),
    m_h_cpm_2d_SimNeDataOverview(0),
    m_h_cpm_1d_SimNeDataSummary(0),
    m_v_2d_MismatchEvents(0)
/*---------------------------------------------------------*/
{
  declareProperty("EmTauTool", m_emTauTool);
  declareProperty("CPCMXTool", m_cpCmxTool);
  declareProperty("ErrorTool", m_errorTool);
  declareProperty("HistogramTool", m_histTool);

  declareProperty("CPMTowerLocation",
                 m_cpmTowerLocation  = LVL1::TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CPMTowerLocationOverlap",
                 m_cpmTowerLocationOverlap  =
		           LVL1::TrigT1CaloDefs::CPMTowerLocation+"Overlap");
  declareProperty("CMXCPTobLocation",
                 m_cmxCpTobLocation  = LVL1::TrigT1CaloDefs::CMXCPTobLocation);
  declareProperty("CMXCPHitsLocation",
                 m_cmxCpHitsLocation = LVL1::TrigT1CaloDefs::CMXCPHitsLocation);
  declareProperty("CPMTobRoILocation",
                 m_cpmTobRoiLocation = LVL1::TrigT1CaloDefs::CPMTobRoILocation);
  declareProperty("TriggerTowerLocation",
                 m_triggerTowerLocation =
		                 LVL1::TrigT1CaloDefs::TriggerTowerLocation);
  declareProperty("RodHeaderLocation",
                 m_rodHeaderLocation = LVL1::TrigT1CaloDefs::RODHeaderLocation);
  declareProperty("ErrorLocation",
                 m_errorLocation     = "L1CaloCPMMismatchVector");

  declareProperty("RootDirectory", m_rootDir = "L1Calo/CPM");
}

/*---------------------------------------------------------*/
CPSimMon::~CPSimMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode CPSimMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;

  m_debug = msgLvl(MSG::DEBUG);

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_emTauTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1EmTauTools" << endreq;
    return sc;
  }

  sc = m_cpCmxTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1CPCMXTools" << endreq;
    return sc;
  }

  sc = m_errorTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                    << endreq;
    return sc;
  }

  sc = m_histTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

  /*---------------------------------------------------------*/
  StatusCode CPSimMon::finalize()
  /*---------------------------------------------------------*/
  {
  return StatusCode::SUCCESS;
  }
  
  StatusCode CPSimMon::bookHistogramsRecurrent() {
  msg(MSG::DEBUG) << "bookHistograms entered" << endreq;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
  	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  if ( newEventsBlock || newLumiBlock ) { }
  
  if( newRun ) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    std::string dir1(m_rootDir + "/Errors/Transmission_Simulation");
    MonGroup monShift( this, dir1, run, attr );
    MonGroup monExpert( this, dir1, run, attr );
    MonGroup monCPMin( this, dir1 + "/PPM2CPMTowers", run, attr );
    MonGroup monRoIs( this, dir1 + "/Towers2RoIs", run, attr );
    MonGroup monEvent1( this, dir1 + "/MismatchEventNumbers", run, attr, "", "eventSample" );
    std::string dir2(m_rootDir + "_CMX/Errors/Transmission_Simulation");
    MonGroup monCMXin( this, dir2 + "/RoIs2TOBs", run, attr);
    MonGroup monCMXSums( this, dir2 + "/TOBs2HitSums", run, attr );
    MonGroup monCMXTopo( this, dir2 + "/TOBs2Topo", run, attr );
    MonGroup monEvent2( this, dir2 + "/MismatchEventNumbers", run, attr, "", "eventSample" );

  // CPMTowers

  m_histTool->setMonGroup(&monCPMin);

  m_h_cpm_em_2d_etaPhi_tt_PpmEqCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmEqCore",
    "EM Core CPM Tower/PPM Tower Non-zero Matches");
  m_h_cpm_em_2d_etaPhi_tt_PpmNeCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmNeCore",
    "EM Core CPM Tower/PPM Tower Non-zero Mismatches");
  m_h_cpm_em_2d_etaPhi_tt_PpmNoCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmNoCore",
    "EM PPM Towers but no Core CPM Towers");
  m_h_cpm_em_2d_etaPhi_tt_CoreNoPpm = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_CoreNoPpm",
    "EM Core CPM Towers but no PPM Towers");
  m_h_cpm_had_2d_etaPhi_tt_PpmEqCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmEqCore",
    "HAD Core CPM Tower/PPM Tower Non-zero Matches");
  m_h_cpm_had_2d_etaPhi_tt_PpmNeCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmNeCore",
    "HAD Core CPM Tower/PPM Tower Non-zero Mismatches");
  m_h_cpm_had_2d_etaPhi_tt_PpmNoCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmNoCore",
    "HAD PPM Towers but no Core CPM Towers");
  m_h_cpm_had_2d_etaPhi_tt_CoreNoPpm = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_CoreNoPpm",
    "HAD Core CPM Towers but no PPM Towers");
  m_h_cpm_em_2d_etaPhi_tt_PpmEqOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmEqOverlap",
    "EM Overlap CPM Tower/PPM Tower Non-zero Matches");
  m_h_cpm_em_2d_etaPhi_tt_PpmNeOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmNeOverlap",
    "EM Overlap CPM Tower/PPM Tower Non-zero Mismatches");
  m_h_cpm_em_2d_etaPhi_tt_PpmNoOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmNoOverlap",
    "EM PPM Towers but no Overlap CPM Towers");
  m_h_cpm_em_2d_etaPhi_tt_OverlapNoPpm = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_OverlapNoPpm",
    "EM Overlap CPM Towers but no PPM Towers");
  m_h_cpm_had_2d_etaPhi_tt_PpmEqOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmEqOverlap",
    "HAD Overlap CPM Tower/PPM Tower Non-zero Matches");
  m_h_cpm_had_2d_etaPhi_tt_PpmNeOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmNeOverlap",
    "HAD Overlap CPM Tower/PPM Tower Non-zero Mismatches");
  m_h_cpm_had_2d_etaPhi_tt_PpmNoOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmNoOverlap",
    "HAD PPM Towers but no Overlap CPM Towers");
  m_h_cpm_had_2d_etaPhi_tt_OverlapNoPpm = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_OverlapNoPpm",
    "HAD Overlap CPM Towers but no PPM Towers");

  m_h_cpm_2d_tt_PpmEqCpmFpga = m_histTool->bookCPMCrateModuleVsFPGA(
    "cpm_2d_tt_PpmEqCpmFpga", "CPM Tower/PPM Tower Non-zero Matches by FPGA");
  m_h_cpm_2d_tt_PpmNeCpmFpga = m_histTool->bookCPMCrateModuleVsFPGA(
   "cpm_2d_tt_PpmNeCpmFpga", "CPM Tower/PPM Tower Non-zero Mismatches by FPGA");
  m_h_cpm_2d_tt_PpmNoCpmFpga = m_histTool->bookCPMCrateModuleVsFPGA(
    "cpm_2d_tt_PpmNoCpmFpga", "PPM Towers but no CPM Towers by FPGA");
  m_h_cpm_2d_tt_CpmNoPpmFpga = m_histTool->bookCPMCrateModuleVsFPGA(
    "cpm_2d_tt_CpmNoPpmFpga", "CPM Towers but no PPM Towers by FPGA");

  // RoIs

  m_histTool->setMonGroup(&monRoIs);

  m_h_cpm_2d_roi_EmEnergySimEqData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_EmEnergySimEqData", "CPM RoI EM Energy Data/Simulation Non-zero Matches");
  m_h_cpm_2d_roi_EmEnergySimNeData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_EmEnergySimNeData", "CPM RoI EM Energy Data/Simulation Non-zero Mismatches");
  m_h_cpm_2d_roi_EmEnergySimNoData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_EmEnergySimNoData", "CPM RoI EM Energy Simulation but no Data");
  m_h_cpm_2d_roi_EmEnergyDataNoSim = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_EmEnergyDataNoSim", "CPM RoI EM Energy Data but no Simulation");
  m_h_cpm_2d_roi_TauEnergySimEqData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_TauEnergySimEqData", "CPM RoI Tau Energy Data/Simulation Non-zero Matches");
  m_h_cpm_2d_roi_TauEnergySimNeData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_TauEnergySimNeData", "CPM RoI Tau Energy Data/Simulation Non-zero Mismatches");
  m_h_cpm_2d_roi_TauEnergySimNoData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_TauEnergySimNoData", "CPM RoI Tau Energy Simulation but no Data");
  m_h_cpm_2d_roi_TauEnergyDataNoSim = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_TauEnergyDataNoSim", "CPM RoI Tau Energy Data but no Simulation");

  m_h_cpm_2d_roi_EmIsolSimEqData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_EmIsolSimEqData", "CPM RoI EM Isolation Data/Simulation Non-zero Matches");
  m_h_cpm_2d_roi_EmIsolSimNeData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_EmIsolSimNeData", "CPM RoI EM Isolation Data/Simulation Non-zero Mismatches");
  m_h_cpm_2d_roi_EmIsolSimNoData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_EmIsolSimNoData", "CPM RoI EM Isolation Simulation but no Data");
  m_h_cpm_2d_roi_EmIsolDataNoSim = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_EmIsolDataNoSim", "CPM RoI EM Isolation Data but no Simulation");
  m_h_cpm_2d_roi_TauIsolSimEqData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_TauIsolSimEqData", "CPM RoI Tau Isolation Data/Simulation Non-zero Matches");
  m_h_cpm_2d_roi_TauIsolSimNeData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_TauIsolSimNeData", "CPM RoI Tau Isolation Data/Simulation Non-zero Mismatches");
  m_h_cpm_2d_roi_TauIsolSimNoData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_TauIsolSimNoData", "CPM RoI Tau Isolation Simulation but no Data");
  m_h_cpm_2d_roi_TauIsolDataNoSim = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_TauIsolDataNoSim", "CPM RoI Tau Isolation Data but no Simulation");

  m_h_cpm_2d_etaPhi_roi_SimEqData = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_SimEqData", "CPM RoI Data/Simulation Non-zero Matches");
  m_h_cpm_2d_etaPhi_roi_SimNeData = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_SimNeData", "CPM RoI Data/Simulation Non-zero Mismatches");
  m_h_cpm_2d_etaPhi_roi_SimNoData = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_SimNoData", "CPM RoI Simulation but no Data");
  m_h_cpm_2d_etaPhi_roi_DataNoSim = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_DataNoSim", "CPM RoI Data but no Simulation");

  // CMX-CP TOBs

  m_histTool->setMonGroup(&monCMXin);

  m_h_cmx_2d_tob_LeftEnergySimEqData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_LeftEnergySimEqData", "CMX TOB Left Energy Data/Simulation Non-zero Matches");
  m_h_cmx_2d_tob_LeftEnergySimNeData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_LeftEnergySimNeData", "CMX TOB Left Energy Data/Simulation Non-zero Mismatches");
  m_h_cmx_2d_tob_LeftEnergySimNoData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_LeftEnergySimNoData", "CMX TOB Left Energy Simulation but no Data");
  m_h_cmx_2d_tob_LeftEnergyDataNoSim = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_LeftEnergyDataNoSim", "CMX TOB Left Energy Data but no Simulation");
  m_h_cmx_2d_tob_RightEnergySimEqData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_RightEnergySimEqData", "CMX TOB Right Energy Data/Simulation Non-zero Matches");
  m_h_cmx_2d_tob_RightEnergySimNeData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_RightEnergySimNeData", "CMX TOB Right Energy Data/Simulation Non-zero Mismatches");
  m_h_cmx_2d_tob_RightEnergySimNoData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_RightEnergySimNoData", "CMX TOB Right Energy Simulation but no Data");
  m_h_cmx_2d_tob_RightEnergyDataNoSim = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_RightEnergyDataNoSim", "CMX TOB Right Energy Data but no Simulation");

  m_h_cmx_2d_tob_LeftIsolSimEqData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_LeftIsolSimEqData", "CMX TOB Left Isolation Data/Simulation Non-zero Matches");
  m_h_cmx_2d_tob_LeftIsolSimNeData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_LeftIsolSimNeData", "CMX TOB Left Isolation Data/Simulation Non-zero Mismatches");
  m_h_cmx_2d_tob_LeftIsolSimNoData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_LeftIsolSimNoData", "CMX TOB Left Isolation Simulation but no Data");
  m_h_cmx_2d_tob_LeftIsolDataNoSim = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_LeftIsolDataNoSim", "CMX TOB Left Isolation Data but no Simulation");
  m_h_cmx_2d_tob_RightIsolSimEqData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_RightIsolSimEqData", "CMX TOB Right Isolation Data/Simulation Non-zero Matches");
  m_h_cmx_2d_tob_RightIsolSimNeData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_RightIsolSimNeData", "CMX TOB Right Isolation Data/Simulation Non-zero Mismatches");
  m_h_cmx_2d_tob_RightIsolSimNoData = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_RightIsolSimNoData", "CMX TOB Right Isolation Simulation but no Data");
  m_h_cmx_2d_tob_RightIsolDataNoSim = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
    "cmx_2d_tob_RightIsolDataNoSim", "CMX TOB Right Isolation Data but no Simulation");

  m_h_cmx_2d_tob_OverflowSimEqData = m_histTool->bookCPMCrateModuleVsCMX(
    "cmx_2d_tob_OverflowSimEqData", "CMX TOB RoI Overflow Bit Data/Simulation Matches");
  m_h_cmx_2d_tob_OverflowSimNeData = m_histTool->bookCPMCrateModuleVsCMX(
    "cmx_2d_tob_OverflowSimNeData", "CMX TOB RoI Overflow Bit Data/Simulation Mismatches");

  m_h_cmx_2d_etaPhi_tob_SimEqData = m_histTool->bookCPMRoIEtaVsPhi(
    "cmx_2d_etaPhi_tob_SimEqData", "CMX TOB Data/Simulation Non-zero Matches");
  m_h_cmx_2d_etaPhi_tob_SimNeData = m_histTool->bookCPMRoIEtaVsPhi(
    "cmx_2d_etaPhi_tob_SimNeData", "CMX TOB Data/Simulation Non-zero Mismatches");
  m_h_cmx_2d_etaPhi_tob_SimNoData = m_histTool->bookCPMRoIEtaVsPhi(
    "cmx_2d_etaPhi_tob_SimNoData", "CMX TOB Simulation but no Data");
  m_h_cmx_2d_etaPhi_tob_DataNoSim = m_histTool->bookCPMRoIEtaVsPhi(
    "cmx_2d_etaPhi_tob_DataNoSim", "CMX TOB Data but no Simulation");

  // Local/Remote/Total sums

  m_histTool->setMonGroup(&monCMXSums);

  m_h_cmx_1d_thresh_SumsSimEqData = m_histTool->bookCPMSumCMX(
    "cmx_1d_thresh_SumsSimEqData",
    "CMX Hit Sums Data/Simulation Non-zero Matches");
  m_h_cmx_1d_thresh_SumsSimNeData = m_histTool->bookCPMSumCMX(
    "cmx_1d_thresh_SumsSimNeData",
    "CMX Hit Sums Data/Simulation Non-zero Mismatches");
  m_h_cmx_1d_thresh_SumsSimNoData = m_histTool->bookCPMSumCMX(
    "cmx_1d_thresh_SumsSimNoData", "CMX Hit Sums Simulation but no Data");
  m_h_cmx_1d_thresh_SumsDataNoSim = m_histTool->bookCPMSumCMX(
    "cmx_1d_thresh_SumsDataNoSim", "CMX Hit Sums Data but no Simulation");

  m_h_cmx_2d_thresh_LeftSumsSimEqData = m_histTool->bookCPMSumVsThreshold(
    "cmx_2d_thresh_LeftSumsSimEqData",
    "CMX Hit Sums Left Data/Simulation Threshold Non-zero Matches", 0);
  m_h_cmx_2d_thresh_LeftSumsSimNeData = m_histTool->bookCPMSumVsThreshold(
    "cmx_2d_thresh_LeftSumsSimNeData",
    "CMX Hit Sums Left Data/Simulation Threshold Non-zero Mismatches", 0);
  m_h_cmx_2d_thresh_RightSumsSimEqData = m_histTool->bookCPMSumVsThreshold(
    "cmx_2d_thresh_RightSumsSimEqData",
    "CMX Hit Sums Right Data/Simulation Threshold Non-zero Matches", 1);
  m_h_cmx_2d_thresh_RightSumsSimNeData = m_histTool->bookCPMSumVsThreshold(
    "cmx_2d_thresh_RightSumsSimNeData",
    "CMX Hit Sums Right Data/Simulation Threshold Non-zero Mismatches", 1);

  // Topo output information

  m_histTool->setMonGroup(&monCMXTopo);

  m_h_cmx_2d_topo_SimEqData = m_histTool->book2F(
    "cmx_2d_topo_SimEqData", "CMX Topo Output Data/Simulation Non-zero Matches",
    8, 0, 8, 3, 0, 3);
  setLabelsTopo(m_h_cmx_2d_topo_SimEqData);
  m_h_cmx_2d_topo_SimNeData = m_histTool->book2F(
    "cmx_2d_topo_SimNeData", "CMX Topo Output Data/Simulation Non-zero Mismatches",
    8, 0, 8, 3, 0, 3);
  setLabelsTopo(m_h_cmx_2d_topo_SimNeData);
  m_h_cmx_2d_topo_SimNoData = m_histTool->book2F(
    "cmx_2d_topo_SimNoData", "CMX Topo Output Simulation but no Data",
    8, 0, 8, 3, 0, 3);
  setLabelsTopo(m_h_cmx_2d_topo_SimNoData);
  m_h_cmx_2d_topo_DataNoSim = m_histTool->book2F(
    "cmx_2d_topo_DataNoSim", "CMX Topo Output Data but no Simulation",
    8, 0, 8, 3, 0, 3);
  setLabelsTopo(m_h_cmx_2d_topo_DataNoSim);

  // Summary

  m_histTool->setMonGroup(&monExpert);

  m_h_cpm_2d_SimEqDataOverview = m_histTool->book2F("cpm_2d_SimEqDataOverview",
   "CP Transmission/Comparison with Simulation Overview - Events with Matches",
             64, 0, 64, NumberOfSummaryBins, 0, NumberOfSummaryBins);
  m_histTool->cpmCMXCrateModule(m_h_cpm_2d_SimEqDataOverview);
  setLabels(m_h_cpm_2d_SimEqDataOverview, false);

  m_h_cpm_2d_SimNeDataOverview = m_histTool->book2F("cpm_2d_SimNeDataOverview",
   "CP Transmission/Comparison with Simulation Overview - Events with Mismatches",
             64, 0, 64, NumberOfSummaryBins, 0, NumberOfSummaryBins);
  m_histTool->cpmCMXCrateModule(m_h_cpm_2d_SimNeDataOverview);
  setLabels(m_h_cpm_2d_SimNeDataOverview, false);

  m_histTool->setMonGroup(&monShift);

  m_h_cpm_1d_SimNeDataSummary = m_histTool->book1F("cpm_1d_SimNeDataSummary",
   "CP Transmission/Comparison with Simulation Mismatch Summary;;Events",
    NumberOfSummaryBins, 0, NumberOfSummaryBins);
  setLabels(m_h_cpm_1d_SimNeDataSummary);

  // Mismatch Event Number Samples

  m_histTool->setMonGroup(&monEvent1);

  TH2I_LW* hist = 0;
  m_v_2d_MismatchEvents.clear();
  m_v_2d_MismatchEvents.resize(7, hist);
  hist = m_histTool->bookCPMEventVsCrateModule("cpm_em_2d_tt_MismatchEvents",
                                     "CPM Towers EM Mismatch Event Numbers");
  m_v_2d_MismatchEvents[0] = hist;
  hist = m_histTool->bookCPMEventVsCrateModule("cpm_had_2d_tt_MismatchEvents",
                                     "CPM Towers Had Mismatch Event Numbers");
  m_v_2d_MismatchEvents[1] = hist;
  hist = m_histTool->bookCPMEventVsCrateModule("cpm_2d_roi_EmMismatchEvents",
                                     "CPM RoIs EM Mismatch Event Numbers");
  m_v_2d_MismatchEvents[2] = hist;
  hist = m_histTool->bookCPMEventVsCrateModule("cpm_2d_roi_TauMismatchEvents",
                                     "CPM RoIs Tau Mismatch Event Numbers");
  m_v_2d_MismatchEvents[3] = hist;

  m_histTool->setMonGroup(&monEvent2);

  hist = m_histTool->bookCPMEventVsCrateModule("cmx_2d_tob_LeftMismatchEvents",
                                     "CMX TOBs Left Mismatch Event Numbers");
  m_v_2d_MismatchEvents[4] = hist;
  hist = m_histTool->bookCPMEventVsCrateModule("cmx_2d_tob_RightMismatchEvents",
                                     "CMX TOBs Right Mismatch Event Numbers");
  m_v_2d_MismatchEvents[5] = hist;

  hist = m_histTool->bookEventNumbers("cmx_2d_thresh_SumsMismatchEvents",
                     "CMX Hit Sums Mismatch Event Numbers", 24, 0., 24.);
  m_histTool->cpmCrateCMX(hist, 0, false);
  m_histTool->cpmCrateCMX(hist, 8, false);
  m_histTool->cpmCrateCMX(hist, 16, false);
  hist->GetYaxis()->SetBinLabel(1, "Local 0/0");
  hist->GetYaxis()->SetBinLabel(9, "Remote 0/0");
  hist->GetYaxis()->SetBinLabel(15, "Total 3/0");
  hist->GetYaxis()->SetBinLabel(17, "Topo 0/0");
  m_v_2d_MismatchEvents[6] = hist;

  m_histTool->unsetMonGroup();
  m_histBooked = true;

  } // end if (isNewRun ...

  msg(MSG::DEBUG) << "Leaving bookHistograms" << endreq;
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode CPSimMon::fillHistograms()
/*---------------------------------------------------------*/
{
  if (m_debug) msg(MSG::DEBUG) << "fillHistograms entered" << endreq;

  if (!m_histBooked) {
    if (m_debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endreq;
    return StatusCode::SUCCESS;
  }
  
  // Skip events believed to be corrupt or with ROB errors

  if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError()) {
    if (m_debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endreq;
    return StatusCode::SUCCESS;
  }

  // NB. Collection retrieves wrapped in m_storeGate->contains<..>(..)
  // are for those not expected to be on ESD. They should be on bytestream.

  StatusCode sc;

  //Retrieve Trigger Towers from SG
  const TriggerTowerCollection* triggerTowerTES = 0; 
  sc = evtStore()->retrieve(triggerTowerTES, m_triggerTowerLocation); 
  if( sc.isFailure()  ||  !triggerTowerTES ) {
    msg(MSG::DEBUG) << "No Trigger Tower container found" << endreq; 
  }

  //Retrieve Core and Overlap CPM Towers from SG
  const CpmTowerCollection* cpmTowerTES = 0; 
  const CpmTowerCollection* cpmTowerOvTES = 0; 
  sc = evtStore()->retrieve(cpmTowerTES, m_cpmTowerLocation); 
  if( sc.isFailure()  ||  !cpmTowerTES ) {
    msg(MSG::DEBUG) << "No Core CPM Tower container found" << endreq; 
  }
  if (evtStore()->contains<CpmTowerCollection>(m_cpmTowerLocationOverlap)) {
    sc = evtStore()->retrieve(cpmTowerOvTES, m_cpmTowerLocationOverlap); 
  } else sc = StatusCode::FAILURE;
  if( sc.isFailure()  ||  !cpmTowerOvTES ) {
    msg(MSG::DEBUG) << "No Overlap CPM Tower container found" << endreq; 
  }
  m_overlapPresent = cpmTowerOvTES != 0;
  
  //Retrieve CPM RoIs from SG
  const CpmTobRoiCollection* cpmRoiTES = 0;
  sc = evtStore()->retrieve( cpmRoiTES, m_cpmTobRoiLocation);
  if( sc.isFailure()  ||  !cpmRoiTES ) {
    msg(MSG::DEBUG) << "No DAQ CPM TOB RoIs container found" << endreq;
  }

  //Retrieve ROD Headers from SG
  m_limitedRoi = 0;
  m_rodTES = 0;
  if (evtStore()->contains<RodHeaderCollection>(m_rodHeaderLocation)) {
    sc = evtStore()->retrieve( m_rodTES, m_rodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if( sc.isFailure()  ||  !m_rodTES ) {
    msg(MSG::DEBUG) << "No ROD Header container found" << endreq;
  }
  
  //Retrieve CMX-CP TOBs from SG
  const CmxCpTobCollection* cmxCpTobTES = 0;
  sc = evtStore()->retrieve( cmxCpTobTES, m_cmxCpTobLocation);
  if( sc.isFailure()  ||  !cmxCpTobTES ) {
    msg(MSG::DEBUG) << "No CMX-CP TOB container found" << endreq; 
  }
  
  //Retrieve CMX-CP Hits from SG
  const CmxCpHitsCollection* cmxCpHitsTES = 0;
  sc = evtStore()->retrieve( cmxCpHitsTES, m_cmxCpHitsLocation);
  if( sc.isFailure()  ||  !cmxCpHitsTES ) {
    msg(MSG::DEBUG) << "No CMX-CP Hits container found" << endreq; 
  }

  // Maps to simplify comparisons
  
  TriggerTowerMap ttMap;
  CpmTowerMap     cpMap;
  CpmTowerMap     ovMap;
  CpmTobRoiMap    crMap;
  CmxCpTobMap     cbMap;
  CmxCpHitsMap    cmMap;
  setupMap(triggerTowerTES, ttMap);
  setupMap(cpmTowerTES, cpMap);
  setupMap(cpmTowerOvTES, ovMap);
  setupMap(cpmRoiTES, crMap);
  std::vector<int> parityMap(s_crates * s_cmxs * s_modules);
  setupMap(cmxCpTobTES, cbMap, &parityMap);
  setupMap(cmxCpHitsTES, cmMap);

  // Vectors for error overview bits;
  const int vecsizeCpm = 2 * s_crates * s_modules;
  const int vecsizeCmx = 2 * s_crates * s_cmxs;
  ErrorVector errorsCPM(vecsizeCpm);
  ErrorVector errorsCMX(vecsizeCmx);

  // Compare Trigger Towers and CPM Towers from data

  bool overlap = false;
  bool mismatchCore = false;
  bool mismatchOverlap = false;
  mismatchCore = compare(ttMap, cpMap, errorsCPM, overlap);
  if (m_overlapPresent) {
    overlap = true;
    mismatchOverlap = compare(ttMap, ovMap, errorsCPM, overlap);
  }

  // Compare RoIs simulated from CPM Towers with CPM RoIs from data

  CpmTobRoiCollection* cpmRoiSIM = 0;
  if (cpmTowerTES || cpmTowerOvTES) {
    cpmRoiSIM = new CpmTobRoiCollection;
    if (mismatchCore || mismatchOverlap) {
      simulate(cpMap, ovMap, cpmRoiSIM);
    } else {
      simulate(cpMap, cpmRoiSIM);
    }
  }
  CpmTobRoiMap crSimMap;
  setupMap(cpmRoiSIM, crSimMap);
  compare(crSimMap, crMap, errorsCPM);
  crSimMap.clear();
  delete cpmRoiSIM;

  // Compare CMX-CP TOBs simulated from CPM RoIs with CMX-CP TOBs from data

  CmxCpTobCollection* cmxCpTobSIM = 0;
  if (cpmRoiTES) {
    cmxCpTobSIM = new CmxCpTobCollection;
    simulate(cpmRoiTES, cmxCpTobSIM);
  }
  CmxCpTobMap cbSimMap;
  setupMap(cmxCpTobSIM, cbSimMap);
  compare(cbSimMap, cbMap, parityMap, errorsCPM, errorsCMX);
  cbSimMap.clear();
  delete cmxCpTobSIM;

  // Compare Local sums simulated from CMX-CP TOBs with Local sums from data

  CmxCpHitsCollection* cmxLocalSIM = 0;
  if (cmxCpTobTES) {
    cmxLocalSIM = new CmxCpHitsCollection;
    simulate(cmxCpTobTES, cmxLocalSIM, LVL1::CMXCPHits::LOCAL);
  }
  CmxCpHitsMap cmxLocalSimMap;
  setupMap(cmxLocalSIM, cmxLocalSimMap);
  compare(cmxLocalSimMap, cmMap, errorsCMX, LVL1::CMXCPHits::LOCAL);
  cmxLocalSimMap.clear();
  delete cmxLocalSIM;

  // Compare Local sums with Remote sums from data

  compare(cmMap, cmMap, errorsCMX, LVL1::CMXCPHits::REMOTE_0);

  // Compare Total sums simulated from Remote sums with Total sums from data

  CmxCpHitsCollection* cmxTotalSIM = 0;
  if (cmxCpHitsTES) {
    cmxTotalSIM = new CmxCpHitsCollection;
    simulate(cmxCpHitsTES, cmxTotalSIM);
  }
  CmxCpHitsMap cmxTotalSimMap;
  setupMap(cmxTotalSIM, cmxTotalSimMap);
  compare(cmxTotalSimMap, cmMap, errorsCMX, LVL1::CMXCPHits::TOTAL);
  cmxTotalSimMap.clear();
  delete cmxTotalSIM;

  // Compare Topo output information simulated from CMX-CP TOBs with Topo info from data

  CmxCpHitsCollection* cmxTopoSIM = 0;
  if (cmxCpTobTES) {
    cmxTopoSIM = new CmxCpHitsCollection;
    simulate(cmxCpTobTES, cmxTopoSIM, LVL1::CMXCPHits::TOPO_CHECKSUM);
  }
  CmxCpHitsMap cmxTopoSimMap;
  setupMap(cmxTopoSIM, cmxTopoSimMap);
  compare(cmxTopoSimMap, cmMap, errorsCMX, LVL1::CMXCPHits::TOPO_CHECKSUM);
  cmxTopoSimMap.clear();
  delete cmxTopoSIM;

  // Update error summary plots

  ErrorVector crateErr(s_crates);
  const int cpmBins = s_crates * s_modules;
  const int cmxBins = s_crates * s_cmxs;
  for (int err = 0; err < NumberOfSummaryBins; ++err) {
    int error = 0;
    for (int loc = 0; loc < cpmBins; ++loc) {
      if ((errorsCPM[loc] >> err) & 0x1) {
        m_h_cpm_2d_SimEqDataOverview->Fill(loc, err, 1.);
      }
      if ((errorsCPM[loc + cpmBins] >> err) & 0x1) {
        m_h_cpm_2d_SimNeDataOverview->Fill(loc, err, 1.);
	error = 1;
	crateErr[loc/s_modules] |= (1 << err);
	if (m_v_2d_MismatchEvents[err]) {
	  m_histTool->fillEventNumber(m_v_2d_MismatchEvents[err], loc);
        }
      }
      if (loc < cmxBins) {
        if ((errorsCMX[loc] >> err) & 0x1) {
          m_h_cpm_2d_SimEqDataOverview->Fill(loc+cpmBins, err, 1.);
        }
        if ((errorsCMX[loc + cmxBins] >> err) & 0x1) {
          m_h_cpm_2d_SimNeDataOverview->Fill(loc+cpmBins, err, 1.);
	  error = 1;
	  crateErr[loc/s_cmxs] |= (1 << err);
	  int offset = 0;
	  if (err == RemoteSumMismatch || err == TotalSumMismatch)
	                                offset = 8;
	  else if (err == TopoMismatch) offset = 16;
	  if (m_v_2d_MismatchEvents[6]) {
	    m_histTool->fillEventNumber(m_v_2d_MismatchEvents[6], loc+offset);
	  }
        }
      }
    }
    if (error) m_h_cpm_1d_SimNeDataSummary->Fill(err);
  }

  // Save error vector for global summary

  ErrorVector* save = new ErrorVector(crateErr);
  sc = evtStore()->record(save, m_errorLocation);
  if (sc != StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Error recording CPM mismatch vector in TES "
                    << endreq;
    return sc;
  }

  if (m_debug) msg(MSG::DEBUG) << "Leaving fillHistograms" << endreq;

  return StatusCode::SUCCESS;
}

  StatusCode CPSimMon::procHistograms() {  
  msg(MSG::DEBUG) << "procHistograms entered" << endreq;

  if ( endOfLumiBlock || endOfRun) {
  }

  return StatusCode::SUCCESS;
}

//  Compare Trigger Towers and CPM Towers

bool CPSimMon::compare(const TriggerTowerMap& ttMap,
                       const CpmTowerMap& cpMap, ErrorVector& errors,
                       bool overlap)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Trigger Towers and CPM Towers" << endreq;
  }
 
  bool mismatch = false;
  LVL1::CoordToHardware converter;
  TriggerTowerMap::const_iterator ttMapIter    = ttMap.begin();
  TriggerTowerMap::const_iterator ttMapIterEnd = ttMap.end();
  CpmTowerMap::const_iterator     cpMapIter    = cpMap.begin();
  CpmTowerMap::const_iterator     cpMapIterEnd = cpMap.end();

  while (ttMapIter != ttMapIterEnd || cpMapIter != cpMapIterEnd) {

    int ttKey = 0;
    int cpKey = 0;
    int ttEm  = 0;
    int ttHad = 0;
    int cpEm  = 0;
    int cpHad = 0;
    double eta = 0.;
    double phi = 0.;
    int key = 0;

    if (ttMapIter != ttMapIterEnd) ttKey = ttMapIter->first;
    if (cpMapIter != cpMapIterEnd) cpKey = cpMapIter->first;

    if ((cpMapIter == cpMapIterEnd) ||
                 ((ttMapIter != ttMapIterEnd) && (cpKey > ttKey))) {

      // TriggerTower but no CPMTower

      const LVL1::TriggerTower* tt = ttMapIter->second;
      ++ttMapIter;
      eta = tt->eta();
      phi = tt->phi();
      if (overlap) { // skip non-overlap TTs
        const LVL1::Coordinate coord(phi, eta);
	const int crate = converter.cpCrateOverlap(coord);
        if (crate >= s_crates) continue;
      }
      ttEm  = tt->emEnergy();
      ttHad = tt->hadEnergy();
      key = ttKey;

    } else if ((ttMapIter == ttMapIterEnd) ||
                 ((cpMapIter != cpMapIterEnd) && (ttKey > cpKey))) {

      // CPMTower but no TriggerTower

      const LVL1::CPMTower* cp = cpMapIter->second;
      ++cpMapIter;
      eta = cp->eta();
      phi = cp->phi();
      cpEm  = cp->emEnergy();
      cpHad = cp->hadEnergy();
      key = cpKey;

    } else {

      // Have both

      const LVL1::TriggerTower* tt = ttMapIter->second;
      const LVL1::CPMTower*     cp = cpMapIter->second;
      ++ttMapIter;
      ++cpMapIter;
      eta = tt->eta();
      phi = tt->phi();
      ttEm  = tt->emEnergy();
      ttHad = tt->hadEnergy();
      cpEm  = cp->emEnergy();
      cpHad = cp->hadEnergy();
      key = ttKey;
    }

    if (!ttEm && !ttHad && !cpEm && !cpHad) continue;
    
    //  Fill in error plots

    const LVL1::Coordinate coord(phi, eta);
    const int crate = (overlap) ? converter.cpCrateOverlap(coord)
                                : converter.cpCrate(coord);
    const int cpm   = (overlap) ? converter.cpModuleOverlap(coord)
                                : converter.cpModule(coord);
    if (crate >= s_crates || cpm > s_modules) continue;
    const int loc = crate * s_modules + cpm - 1;
    const int cpmBins = s_crates * s_modules;
    const int bitEm  = (1 << EMTowerMismatch);
    const int bitHad = (1 << HadTowerMismatch);
    double phiFPGA = phi;
    if (overlap) {
      const double twoPi    = 2.*M_PI;
      const double piByFour = M_PI/4.;
      if (phi > 7.*piByFour)   phiFPGA -= twoPi;
      else if (phi < piByFour) phiFPGA += twoPi;
    }
    const int loc2 = fpga(crate, phiFPGA);

    TH2F_LW* hist1 = 0;
    TH2F_LW* hist2 = 0;
    if (ttEm && ttEm == cpEm) { // non-zero match
      errors[loc] |= bitEm;
      hist1 = (overlap) ? m_h_cpm_em_2d_etaPhi_tt_PpmEqOverlap
                        : m_h_cpm_em_2d_etaPhi_tt_PpmEqCore;
      hist2 = m_h_cpm_2d_tt_PpmEqCpmFpga;
    } else if (ttEm != cpEm) {  // mis-match
      mismatch = true;
      errors[loc+cpmBins] |= bitEm;
      if (ttEm && cpEm) {       // non-zero mis-match
        hist1 = (overlap) ? m_h_cpm_em_2d_etaPhi_tt_PpmNeOverlap
	                  : m_h_cpm_em_2d_etaPhi_tt_PpmNeCore;
        hist2 = m_h_cpm_2d_tt_PpmNeCpmFpga;
      } else if (!cpEm) {       // no cp
	hist1 = (overlap) ? m_h_cpm_em_2d_etaPhi_tt_PpmNoOverlap
	                  : m_h_cpm_em_2d_etaPhi_tt_PpmNoCore;
	hist2 = m_h_cpm_2d_tt_PpmNoCpmFpga;
      } else {                  // no tt
	hist1 = (overlap) ? m_h_cpm_em_2d_etaPhi_tt_OverlapNoPpm
	                  : m_h_cpm_em_2d_etaPhi_tt_CoreNoPpm;
	hist2 = m_h_cpm_2d_tt_CpmNoPpmFpga;
      }
      if (m_debug) {
        msg(MSG::DEBUG) << " EMTowerMismatch key/eta/phi/crate/cpm/tt/cp: "
                        << key << "/" << eta << "/" << phi << "/" << crate
			<< "/" << cpm << "/" << ttEm << "/" << cpEm << endreq;
      }
    }
    if (hist1) m_histTool->fillCPMEtaVsPhi(hist1, eta, phi);
    if (hist2) hist2->Fill(loc, loc2);

    hist1 = 0;
    hist2 = 0;
    if (ttHad && ttHad == cpHad) { // non-zero match
      errors[loc] |= bitHad;
      hist1 = (overlap) ? m_h_cpm_had_2d_etaPhi_tt_PpmEqOverlap
                        : m_h_cpm_had_2d_etaPhi_tt_PpmEqCore;
      hist2 = m_h_cpm_2d_tt_PpmEqCpmFpga;
    } else if (ttHad != cpHad) {   // mis-match
      mismatch = true;
      errors[loc+cpmBins] |= bitHad;
      if (ttHad && cpHad) {        // non-zero mis-match
        hist1 = (overlap) ? m_h_cpm_had_2d_etaPhi_tt_PpmNeOverlap
	                  : m_h_cpm_had_2d_etaPhi_tt_PpmNeCore;
        hist2 = m_h_cpm_2d_tt_PpmNeCpmFpga;
      } else if (!cpHad) {         // no cp
	hist1 = (overlap) ? m_h_cpm_had_2d_etaPhi_tt_PpmNoOverlap
	                  : m_h_cpm_had_2d_etaPhi_tt_PpmNoCore;
	hist2 = m_h_cpm_2d_tt_PpmNoCpmFpga;
      } else {                     // no tt
	hist1 = (overlap) ? m_h_cpm_had_2d_etaPhi_tt_OverlapNoPpm
	                  : m_h_cpm_had_2d_etaPhi_tt_CoreNoPpm;
	hist2 = m_h_cpm_2d_tt_CpmNoPpmFpga;
      }
      if (m_debug) {
        msg(MSG::DEBUG) << " HadTowerMismatch key/eta/phi/crate/cpm/tt/cp: "
                        << key << "/" << eta << "/" << phi << "/" << crate
			<< "/" << cpm << "/" << ttHad << "/" << cpHad << endreq;
      }
    }
    if (hist1) m_histTool->fillCPMEtaVsPhi(hist1, eta, phi);
    if (hist2) hist2->Fill(loc, loc2+1);
  }

  return mismatch;
}

//  Compare Simulated RoIs with data

void CPSimMon::compare(const CpmTobRoiMap& roiSimMap, const CpmTobRoiMap& roiMap,
                                                           ErrorVector& errors)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare Simulated RoIs with data" << endreq;

  LVL1::CPRoIDecoder decoder;
  CpmTobRoiMap::const_iterator simMapIter    = roiSimMap.begin();
  CpmTobRoiMap::const_iterator simMapIterEnd = roiSimMap.end();
  CpmTobRoiMap::const_iterator datMapIter    = roiMap.begin();
  CpmTobRoiMap::const_iterator datMapIterEnd = roiMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    unsigned int simEnergy = 0;
    unsigned int datEnergy = 0;
    unsigned int simIsol = 0;
    unsigned int datIsol = 0;
    const LVL1::CPMTobRoI* roi = 0;

    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
                ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulated RoI but no data RoI

      roi     = simMapIter->second;
      simEnergy = roi->energy();
      simIsol   = roi->isolation();
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
                ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data RoI but no simulated RoI

      roi     = datMapIter->second;
      datEnergy = roi->energy();
      datIsol   = roi->isolation();
      ++datMapIter;

    } else {

      // Have both

      const LVL1::CPMTobRoI* roiS = simMapIter->second;
      roi     = datMapIter->second;
      simEnergy = roiS->energy();
      simIsol   = roiS->isolation();
      datEnergy = roi->energy();
      datIsol   = roi->isolation();
      ++simMapIter;
      ++datMapIter;
    }

    if (!simEnergy && !simIsol && !datEnergy && !datIsol) continue;

    //  Check LimitedRoISet bit

    const int crate = roi->crate();
    if (!datEnergy && !datIsol && limitedRoiSet(crate)) continue;
    
    //  Fill in error plots

    const int cpm   = roi->cpm();
    const int chip  = roi->chip();
    const int local = roi->location();
    const int type  = roi->type();
    const int locX  = crate * s_modules + cpm - 1;
    const int locY  = chip * 8 + local;
    const int cpmBins = s_crates * s_modules;
    const int bit = (1 << ((type) ? TauRoIMismatch : EMRoIMismatch));
    const LVL1::CoordinateRange coord(decoder.coordinate((roi->roiWord())<<2)); // hack till updated
    const double eta = coord.eta();
    const double phi = coord.phi();

    TH2F_LW* hist = 0;
    if (simEnergy || datEnergy) {
      if (simEnergy == datEnergy) {
        errors[locX] |= bit;
        hist = (type) ? m_h_cpm_2d_roi_TauEnergySimEqData
                      : m_h_cpm_2d_roi_EmEnergySimEqData;
      } else {
        errors[locX+cpmBins] |= bit;
        if (simEnergy && datEnergy) {
          hist = (type) ? m_h_cpm_2d_roi_TauEnergySimNeData
	                : m_h_cpm_2d_roi_EmEnergySimNeData;
        } else if (simEnergy && !datEnergy) {
          hist = (type) ? m_h_cpm_2d_roi_TauEnergySimNoData
	                : m_h_cpm_2d_roi_EmEnergySimNoData;
        } else {
          hist = (type) ? m_h_cpm_2d_roi_TauEnergyDataNoSim
	                : m_h_cpm_2d_roi_EmEnergyDataNoSim;
        }
      }
      if (hist) hist->Fill(locX, locY);
    }
    hist = 0;
    if (simIsol || datIsol) {
      if (simIsol == datIsol) {
        errors[locX] |= bit;
        hist = (type) ? m_h_cpm_2d_roi_TauIsolSimEqData
                      : m_h_cpm_2d_roi_EmIsolSimEqData;
      } else {
        errors[locX+cpmBins] |= bit;
        if (simIsol && datIsol) {
          hist = (type) ? m_h_cpm_2d_roi_TauIsolSimNeData
	                : m_h_cpm_2d_roi_EmIsolSimNeData;
        } else if (simIsol && !datIsol) {
          hist = (type) ? m_h_cpm_2d_roi_TauIsolSimNoData
	                : m_h_cpm_2d_roi_EmIsolSimNoData;
        } else {
          hist = (type) ? m_h_cpm_2d_roi_TauIsolDataNoSim
	                : m_h_cpm_2d_roi_EmIsolDataNoSim;
        }
      }
      if (hist) hist->Fill(locX, locY);
    }
    // do we need this?
    hist = 0;
    if (simEnergy == datEnergy && simIsol == datIsol) {
      hist = m_h_cpm_2d_etaPhi_roi_SimEqData;
    } else {
      if ((simEnergy || simIsol) && (datEnergy || datIsol)) {
        hist = m_h_cpm_2d_etaPhi_roi_SimNeData;
      } else if (!datEnergy && !datIsol) {
        hist = m_h_cpm_2d_etaPhi_roi_SimNoData;
      } else {
        hist = m_h_cpm_2d_etaPhi_roi_DataNoSim;
      }
    }
    if (hist) m_histTool->fillCPMRoIEtaVsPhi(hist, eta, phi);

  }
}

//  Compare simulated CMX TOBs with data

void CPSimMon::compare(const CmxCpTobMap& simMap, const CmxCpTobMap& datMap,
                       const std::vector<int> parityMap,
                       ErrorVector& errorsCPM, ErrorVector& errorsCMX)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare simulated CMX TOBs with data" << endreq;

  LVL1::CPRoIDecoder decoder;
  CmxCpTobMap::const_iterator simMapIter    = simMap.begin();
  CmxCpTobMap::const_iterator simMapIterEnd = simMap.end();
  CmxCpTobMap::const_iterator datMapIter    = datMap.begin();
  CmxCpTobMap::const_iterator datMapIterEnd = datMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey    = 0;
    int datKey    = 0;
    int simEnergy = 0;
    int simIsol   = 0;
    int simOvf    = 0;
    int datEnergy = 0;
    int datIsol   = 0;
    int datOvf    = 0;

    const LVL1::CMXCPTob* tob = 0;
    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
                  ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulated TOB but no data TOB

      tob = simMapIter->second;
      simEnergy = tob->energy();
      simIsol   = tob->isolation();
      simOvf    = (LVL1::DataError(tob->error())).get(LVL1::DataError::Overflow);
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
                  ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data TOB but no simulated TOB

      tob = datMapIter->second;
      datEnergy = tob->energy();
      datIsol   = tob->isolation();
      datOvf    = (LVL1::DataError(tob->error())).get(LVL1::DataError::Overflow);
      ++datMapIter;

    } else {

      // Have both

      const LVL1::CMXCPTob* tobS = simMapIter->second;
      tob = datMapIter->second;
      simEnergy = tobS->energy();
      simIsol   = tobS->isolation();
      simOvf    = (LVL1::DataError(tobS->error())).get(LVL1::DataError::Overflow);
      datEnergy = tob->energy();
      datIsol   = tob->isolation();
      datOvf    = (LVL1::DataError(tob->error())).get(LVL1::DataError::Overflow);
      ++simMapIter;
      ++datMapIter;
    }

    if (!simEnergy && !simIsol && !datEnergy && !datIsol) continue;

    //  Check LimitedRoISet bit and ParityMerge bit

    const int crate = tob->crate();
    if (!simEnergy && !simIsol && limitedRoiSet(crate)) continue;
    const int cpm   = tob->cpm();
    const int cmx   = tob->cmx();
    if (!datEnergy && !datIsol) {
      const int index = (crate*s_cmxs + cmx)*s_modules + cpm - 1;
      if (parityMap[index]) continue;
    }
    
    //  Fill in error plots

    const int chip  = tob->chip();
    const int loc   = tob->location();
    const int locX = crate * s_modules + cpm - 1;
    const int locY = chip * 4 + loc;
    const int loc2 = crate * s_cmxs + cmx;
    const int cpmBins = s_crates * s_modules;
    const int cmxBins = s_crates * s_cmxs;
    const int bit = (1 << ((cmx) ? RightCMXTobMismatch : LeftCMXTobMismatch));
    const uint32_t roiWord = ((((((crate<<4)+cpm)<<4)+chip)<<2)+loc)<<18;
    const LVL1::CoordinateRange coord(decoder.coordinate(roiWord)); // hack till updated
    const double eta = coord.eta();
    const double phi = coord.phi();

    TH2F_LW* hist = 0;
    if (simEnergy || datEnergy) {
      if (simEnergy == datEnergy) {
        errorsCPM[locX] |= bit;
        errorsCMX[loc2] |= bit;
        hist = (cmx) ? m_h_cmx_2d_tob_RightEnergySimEqData
                     : m_h_cmx_2d_tob_LeftEnergySimEqData;
      } else {
        errorsCPM[locX+cpmBins] |= bit;
        errorsCMX[loc2+cmxBins] |= bit;
        if (simEnergy && datEnergy) {
          hist = (cmx) ? m_h_cmx_2d_tob_RightEnergySimNeData
	               : m_h_cmx_2d_tob_LeftEnergySimNeData;
        } else if (simEnergy && !datEnergy) {
          hist = (cmx) ? m_h_cmx_2d_tob_RightEnergySimNoData
	               : m_h_cmx_2d_tob_LeftEnergySimNoData;
        } else {
          hist = (cmx) ? m_h_cmx_2d_tob_RightEnergyDataNoSim
	               : m_h_cmx_2d_tob_LeftEnergyDataNoSim;
        }
      }
      if (hist) hist->Fill(locX, locY);
    }
    hist = 0;
    if (simIsol || datIsol) {
      if (simIsol == datIsol) {
        errorsCPM[locX] |= bit;
        errorsCMX[loc2] |= bit;
        hist = (cmx) ? m_h_cmx_2d_tob_RightIsolSimEqData
                     : m_h_cmx_2d_tob_LeftIsolSimEqData;
      } else {
        errorsCPM[locX+cpmBins] |= bit;
        errorsCMX[loc2+cmxBins] |= bit;
        if (simIsol && datIsol) {
          hist = (cmx) ? m_h_cmx_2d_tob_RightIsolSimNeData
	               : m_h_cmx_2d_tob_LeftIsolSimNeData;
        } else if (simIsol && !datIsol) {
          hist = (cmx) ? m_h_cmx_2d_tob_RightIsolSimNoData
	               : m_h_cmx_2d_tob_LeftIsolSimNoData;
        } else {
          hist = (cmx) ? m_h_cmx_2d_tob_RightIsolDataNoSim
	               : m_h_cmx_2d_tob_LeftIsolDataNoSim;
        }
      }
      if (hist) hist->Fill(locX, locY);
    }
    hist = 0;
    if (simOvf || datOvf) {
      if (simOvf == datOvf) {
        errorsCPM[locX] |= bit;
	errorsCMX[loc2] |= bit;
	hist = m_h_cmx_2d_tob_OverflowSimEqData;
      } else {
	errorsCPM[locX+cpmBins] |= bit;
	errorsCMX[loc2+cmxBins] |= bit;
        hist = m_h_cmx_2d_tob_OverflowSimNeData;
      }
      if (hist) hist->Fill(locX, cmx);
    }
    // do we need this?
    hist = 0;
    if (simEnergy == datEnergy && simIsol == datIsol) {
      hist = m_h_cmx_2d_etaPhi_tob_SimEqData;
    } else {
      if ((simEnergy || simIsol) && (datEnergy || datIsol)) {
        hist = m_h_cmx_2d_etaPhi_tob_SimNeData;
      } else if (!datEnergy && !datIsol) {
        hist = m_h_cmx_2d_etaPhi_tob_SimNoData;
      } else {
        hist = m_h_cmx_2d_etaPhi_tob_DataNoSim;
      }
    }
    if (hist) m_histTool->fillCPMRoIEtaVsPhi(hist, eta, phi);

  }
}

//  Compare Simulated CMX Hit Sums and Data CMX Hit Sums

void CPSimMon::compare(const CmxCpHitsMap& cmxSimMap,
                       const CmxCpHitsMap& cmxMap,
                       ErrorVector& errors, int selection)
{
  msg(MSG::DEBUG) << "Compare Simulated CMX Hit Sums and Data CMX Hit Sums"
                  << endreq;

  const bool local  = (selection == LVL1::CMXCPHits::LOCAL);
  const bool remote = (selection == LVL1::CMXCPHits::REMOTE_0);
  const bool total  = (selection == LVL1::CMXCPHits::TOTAL);
  const bool topo   = (selection == LVL1::CMXCPHits::TOPO_CHECKSUM);
  if (!local && !remote && !total && !topo) return;
  std::vector<unsigned int> hits0Sim(s_crates*s_cmxs);
  std::vector<unsigned int> hits1Sim(s_crates*s_cmxs);
  std::vector<unsigned int> hits0(s_crates*s_cmxs);
  std::vector<unsigned int> hits1(s_crates*s_cmxs);
  CmxCpHitsMap::const_iterator cmxSimMapIter    = cmxSimMap.begin();
  CmxCpHitsMap::const_iterator cmxSimMapIterEnd = cmxSimMap.end();
  CmxCpHitsMap::const_iterator cmxMapIter       = cmxMap.begin();
  CmxCpHitsMap::const_iterator cmxMapIterEnd    = cmxMap.end();

  while (cmxSimMapIter != cmxSimMapIterEnd || cmxMapIter != cmxMapIterEnd) {

    int cmxSimKey = 0;
    int cmxKey    = 0;
    unsigned int cmxSimHits0 = 0;
    unsigned int cmxSimHits1 = 0;
    unsigned int cmxHits0 = 0;
    unsigned int cmxHits1 = 0;
    int crate  = 0;
    int cmx    = 0;
    int source = 0;

    if (cmxSimMapIter != cmxSimMapIterEnd) cmxSimKey = cmxSimMapIter->first;
    if (cmxMapIter    != cmxMapIterEnd)    cmxKey    = cmxMapIter->first;

    if ((cmxMapIter == cmxMapIterEnd) ||
             ((cmxSimMapIter != cmxSimMapIterEnd) && (cmxKey > cmxSimKey))) {

      // Sim CMX Hits but no Data CMX Hits

      const LVL1::CMXCPHits* cmxS = cmxSimMapIter->second;
      ++cmxSimMapIter;
      source = cmxS->source();
      if (local  && source != LVL1::CMXCPHits::LOCAL) continue;
      if (remote && source != LVL1::CMXCPHits::LOCAL) continue;
      if (total  && source != LVL1::CMXCPHits::TOTAL) continue;
      if (topo   && source != LVL1::CMXCPHits::TOPO_CHECKSUM      &&
                    source != LVL1::CMXCPHits::TOPO_OCCUPANCY_MAP &&
		    source != LVL1::CMXCPHits::TOPO_OCCUPANCY_COUNTS) continue;
      cmxSimHits0 = cmxS->hits0();
      cmxSimHits1 = cmxS->hits1();
      crate       = cmxS->crate();
      cmx         = cmxS->cmx();

    } else if ((cmxSimMapIter == cmxSimMapIterEnd) ||
                 ((cmxMapIter != cmxMapIterEnd) && (cmxSimKey > cmxKey))) {

      // Data CMX Hits but no Sim CMX Hits

      const LVL1::CMXCPHits* cmxD = cmxMapIter->second;
      ++cmxMapIter;
      source   = cmxD->source();
      if (local  && source != LVL1::CMXCPHits::LOCAL)    continue;
      if (remote && source != LVL1::CMXCPHits::REMOTE_0 &&
                    source != LVL1::CMXCPHits::REMOTE_1 &&
		    source != LVL1::CMXCPHits::REMOTE_2) continue;
      if (total  && source != LVL1::CMXCPHits::TOTAL)    continue;
      if (topo   && source != LVL1::CMXCPHits::TOPO_CHECKSUM      &&
                    source != LVL1::CMXCPHits::TOPO_OCCUPANCY_MAP &&
		    source != LVL1::CMXCPHits::TOPO_OCCUPANCY_COUNTS) continue;
      cmxHits0 = cmxD->hits0();
      cmxHits1 = cmxD->hits1();
      crate    = cmxD->crate();
      cmx      = cmxD->cmx();

    } else {

      // Have both

      const LVL1::CMXCPHits* cmxS = cmxSimMapIter->second;
      const LVL1::CMXCPHits* cmxD = cmxMapIter->second;
      ++cmxSimMapIter;
      ++cmxMapIter;
      source   = cmxS->source();
      if (local  && source != LVL1::CMXCPHits::LOCAL)    continue;
      if (remote && source != LVL1::CMXCPHits::LOCAL    &&
                    source != LVL1::CMXCPHits::REMOTE_0 &&
                    source != LVL1::CMXCPHits::REMOTE_1 &&
		    source != LVL1::CMXCPHits::REMOTE_2) continue;
      if (total  && source != LVL1::CMXCPHits::TOTAL)    continue;
      if (topo   && source != LVL1::CMXCPHits::TOPO_CHECKSUM      &&
                    source != LVL1::CMXCPHits::TOPO_OCCUPANCY_MAP &&
		    source != LVL1::CMXCPHits::TOPO_OCCUPANCY_COUNTS) continue;
      cmxSimHits0 = cmxS->hits0();
      cmxSimHits1 = cmxS->hits1();
      cmxHits0    = cmxD->hits0();
      cmxHits1    = cmxD->hits1();
      crate       = cmxS->crate();
      cmx         = cmxS->cmx();
    }

    if (!cmxSimHits0 && !cmxSimHits1 && !cmxHits0 && !cmxHits1) continue;
    
    //  Fill in error plots

    if (local || total) {
      int loc = crate * s_cmxs + cmx;
      const int cmxBins = s_crates * s_cmxs;
      const int bit = (local) ? (1 << LocalSumMismatch)
                              : (1 << TotalSumMismatch);
      TH1F_LW* hist = 0;
      if (cmxSimHits0 == cmxHits0 && cmxSimHits1 == cmxHits1) {
        errors[loc] |= bit;
	hist = m_h_cmx_1d_thresh_SumsSimEqData;
      } else {
        errors[loc+cmxBins] |= bit;
	if ((cmxSimHits0 || cmxSimHits1) && (cmxHits0 || cmxHits1)) {
	  hist = m_h_cmx_1d_thresh_SumsSimNeData;
	} else if (!cmxHits0 && !cmxHits1) {
	  hist = m_h_cmx_1d_thresh_SumsSimNoData;
	} else hist = m_h_cmx_1d_thresh_SumsDataNoSim;
      }
      loc = (local) ? loc : 14+cmx;
      if (hist) hist->Fill(loc);

      loc /= 2;
      const int nThresh = 8;
      const int thrLen  = 3;
      TH2F_LW* hist1 = (cmx) ? m_h_cmx_2d_thresh_RightSumsSimEqData
                             : m_h_cmx_2d_thresh_LeftSumsSimEqData;
      TH2F_LW* hist2 = (cmx) ? m_h_cmx_2d_thresh_RightSumsSimNeData
                             : m_h_cmx_2d_thresh_LeftSumsSimNeData;
      int same = m_histTool->thresholdsSame(cmxHits0, cmxSimHits0, nThresh,
                                                                   thrLen);
      int diff = m_histTool->thresholdsDiff(cmxHits0, cmxSimHits0, nThresh,
                                                                   thrLen);
      m_histTool->fillXVsThresholds(hist1, loc, same, nThresh, 1);
      m_histTool->fillXVsThresholds(hist2, loc, diff, nThresh, 1);
      same = m_histTool->thresholdsSame(cmxHits1, cmxSimHits1, nThresh, thrLen);
      diff = m_histTool->thresholdsDiff(cmxHits1, cmxSimHits1, nThresh, thrLen);
      const int offset = nThresh;
      m_histTool->fillXVsThresholds(hist1, loc, same, nThresh, 1, offset);
      m_histTool->fillXVsThresholds(hist2, loc, diff, nThresh, 1, offset);
    } else if (remote) {
      if (source == LVL1::CMXCPHits::LOCAL) {
        if (crate != s_crates-1) {
	  hits0Sim[crate*s_cmxs+cmx] = cmxSimHits0;
	  hits1Sim[crate*s_cmxs+cmx] = cmxSimHits1;
        }
      } else {
        const int remCrate = source - LVL1::CMXCPHits::REMOTE_0;
	hits0[remCrate*s_cmxs+cmx] = cmxHits0;
	hits1[remCrate*s_cmxs+cmx] = cmxHits1;
      }
    } else {
      const int locX = crate * s_cmxs + cmx;
      const int locY = source - LVL1::CMXCPHits::TOPO_CHECKSUM;
      const int cmxBins = s_crates * s_cmxs;
      const int bit = (1 << TopoMismatch);
      TH2F_LW* hist = 0;
      if (cmxSimHits0 == cmxHits0 && cmxSimHits1 == cmxHits1) {
        errors[locX] |= bit;
	hist = m_h_cmx_2d_topo_SimEqData;
      } else {
        errors[locX+cmxBins] |= bit;
	if ((cmxSimHits0 || cmxSimHits1) && (cmxHits0 || cmxHits1)) {
	  hist = m_h_cmx_2d_topo_SimNeData;
	} else if (!cmxHits0 && !cmxHits1) {
	  hist = m_h_cmx_2d_topo_SimNoData;
	} else hist = m_h_cmx_2d_topo_DataNoSim;
      }
      if (hist) hist->Fill(locX, locY);
    }
  }
  if (remote) {
    for (int crate = 0; crate < s_crates-1; ++crate) {
      for (int cmx = 0; cmx < s_cmxs; ++cmx) {
        int loc = crate * s_cmxs + cmx;
        const int cmxBins = s_crates * s_cmxs;
        const int bit = (1 << RemoteSumMismatch);
        const unsigned int hd0 = hits0[loc];
        const unsigned int hd1 = hits1[loc];
        const unsigned int hs0 = hits0Sim[loc];
        const unsigned int hs1 = hits1Sim[loc];

        if (!hd0 && !hd1 && !hs0 && !hs1) continue;

        TH1F_LW* hist = 0;
        if (hs0 == hd0 && hs1 == hd1) {
          errors[loc] |= bit;
	  hist = m_h_cmx_1d_thresh_SumsSimEqData;
        } else {
          errors[loc+cmxBins] |= bit;
	  if ((hs0 || hs1) && (hd0 || hd1)) {
	    hist = m_h_cmx_1d_thresh_SumsSimNeData;
	  } else if (!hd0 && !hd1) {
	    hist = m_h_cmx_1d_thresh_SumsSimNoData;
	  } else hist = m_h_cmx_1d_thresh_SumsDataNoSim;
        }
        loc += 8;
        if (hist) hist->Fill(loc);

        loc /= 2;
        const int nThresh = 8;
        const int thrLen  = 3;
        TH2F_LW* hist1 = (cmx) ? m_h_cmx_2d_thresh_RightSumsSimEqData
                               : m_h_cmx_2d_thresh_LeftSumsSimEqData;
        TH2F_LW* hist2 = (cmx) ? m_h_cmx_2d_thresh_RightSumsSimNeData
                               : m_h_cmx_2d_thresh_LeftSumsSimNeData;
        int same = m_histTool->thresholdsSame(hd0, hs0, nThresh, thrLen);
        int diff = m_histTool->thresholdsDiff(hd0, hs0, nThresh, thrLen);
        m_histTool->fillXVsThresholds(hist1, loc, same, nThresh, 1);
        m_histTool->fillXVsThresholds(hist2, loc, diff, nThresh, 1);
        same = m_histTool->thresholdsSame(hd1, hs1, nThresh, thrLen);
        diff = m_histTool->thresholdsDiff(hd1, hs1, nThresh, thrLen);
        const int offset = nThresh;
        m_histTool->fillXVsThresholds(hist1, loc, same, nThresh, 1, offset);
        m_histTool->fillXVsThresholds(hist2, loc, diff, nThresh, 1, offset);
      }
    }
  }
}

// Set labels for Overview and summary histograms

void CPSimMon::setLabels(LWHist* hist, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  // Simulation steps in red (#color[2]) depend on Trigger Menu
  axis->SetBinLabel(1+EMTowerMismatch,     "EM tt");
  axis->SetBinLabel(1+HadTowerMismatch,    "Had tt");
  axis->SetBinLabel(1+EMRoIMismatch,       "#color[2]{EM RoIs}");
  axis->SetBinLabel(1+TauRoIMismatch,      "#color[2]{Tau RoIs}");
  axis->SetBinLabel(1+LeftCMXTobMismatch,  "#splitline{Left}{TOBs}");
  axis->SetBinLabel(1+RightCMXTobMismatch, "#splitline{Right}{TOBs}");
  axis->SetBinLabel(1+LocalSumMismatch,    "#color[2]{#splitline{Local}{Sums}}");
  axis->SetBinLabel(1+RemoteSumMismatch,   "#splitline{Remote}{Sums}");
  axis->SetBinLabel(1+TotalSumMismatch,    "#splitline{Total}{Sums}");
  axis->SetBinLabel(1+TopoMismatch,        "#splitline{Topo}{Info}");
}

// Set labels for Topo histograms

void CPSimMon::setLabelsTopo(TH2F_LW* hist)
{
  m_histTool->cpmCrateCMX(hist);
  LWHist::LWHistAxis* axis = hist->GetYaxis();
  axis->SetBinLabel(1, "Checksum");
  axis->SetBinLabel(2, "Map");
  axis->SetBinLabel(3, "Counts");
  axis->SetTitle("Topo Info");
}

// Set up TriggerTower map

void CPSimMon::setupMap(const TriggerTowerCollection* coll,
                                 TriggerTowerMap& map)
{
  if (coll) {
    LVL1::TriggerTowerKey towerKey;
    TriggerTowerCollection::const_iterator pos  = coll->begin();
    TriggerTowerCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const double eta = (*pos)->eta();
      if (eta > -2.5 && eta < 2.5 &&
                     ((*pos)->emEnergy() > 0 || (*pos)->hadEnergy() > 0)) {
        const double phi = (*pos)->phi();
        const int key = towerKey.ttKey(phi, eta);
        map.insert(std::make_pair(key, *pos));
      }
    }
  }
}

// Set up CpmTower map

void CPSimMon::setupMap(const CpmTowerCollection* coll, CpmTowerMap& map)
{
  if (coll) {
    LVL1::TriggerTowerKey towerKey;
    CpmTowerCollection::const_iterator pos  = coll->begin();
    CpmTowerCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const double eta = (*pos)->eta();
      const double phi = (*pos)->phi();
      const int key = towerKey.ttKey(phi, eta);
      map.insert(std::make_pair(key, *pos));
    }
  }
}

// Set up CpmTobRoi map

void CPSimMon::setupMap(const CpmTobRoiCollection* coll, CpmTobRoiMap& map)
{
  if (coll) {
    CpmTobRoiCollection::const_iterator pos  = coll->begin();
    CpmTobRoiCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int cpm   = (*pos)->cpm();
      const int chip  = (*pos)->chip();
      const int loc   = (*pos)->location();
      const int type  = (*pos)->type();
      const int key   = (((((((type << 2) | crate) << 4) | cpm) << 3) | chip) << 3) | loc;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

// Set up CmxCpTob map

void CPSimMon::setupMap(const CmxCpTobCollection* coll, CmxCpTobMap& map,
                                             std::vector<int>* parityMap)
{
  if (coll) {
    CmxCpTobCollection::const_iterator pos  = coll->begin();
    CmxCpTobCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int cpm   = (*pos)->cpm();
      const int cmx   = (*pos)->cmx();
      const int chip  = (*pos)->chip();
      const int loc   = (*pos)->location();
      const int key   = (((((((crate << 1) | cmx) << 4) | cpm) << 4) | chip) << 2) | loc;
      map.insert(std::make_pair(key, *pos));
      if (parityMap) {
	LVL1::DataError err((*pos)->error());
	const int parity = err.get(LVL1::DataError::ParityMerge);
	if (parity) {
          const int index = (crate*s_cmxs + cmx)*s_modules + cpm - 1;
	  (*parityMap)[index] = parity;
        }
      }
    }
  }
}

// Set up CmxCpHits map

void CPSimMon::setupMap(const CmxCpHitsCollection* coll, CmxCpHitsMap& map)
{
  if (coll) {
    CmxCpHitsCollection::const_iterator pos  = coll->begin();
    CmxCpHitsCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate  = (*pos)->crate();
      const int cmx    = (*pos)->cmx();
      const int source = (*pos)->source();
      const int key  = (crate*2 +cmx) * 8 + source;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

// Simulate CPM RoIs from CPM Towers

void CPSimMon::simulate(const CpmTowerMap towers, const CpmTowerMap towersOv,
                              CpmTobRoiCollection* rois)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CPM TOB RoIs from CPM Towers" << endreq;

  // Process a crate at a time to use overlap data
  std::vector<CpmTowerMap> crateMaps(s_crates);
  LVL1::CoordToHardware converter;
  CpmTowerCollection* tempColl = new CpmTowerCollection;
  CpmTowerMap::const_iterator iter  = towers.begin();
  CpmTowerMap::const_iterator iterE = towers.end();
  for (; iter != iterE; ++iter) {
    LVL1::CPMTower* tt = ttCheck(iter->second, tempColl);
    const LVL1::Coordinate coord(tt->phi(), tt->eta());
    const int crate = converter.cpCrate(coord);
    if (crate >= s_crates) continue;
    crateMaps[crate].insert(std::make_pair(iter->first, tt));
  }
  // If overlap data not present take from core data
  iter  = (m_overlapPresent) ? towersOv.begin() : towers.begin();
  iterE = (m_overlapPresent) ? towersOv.end()   : towers.end();
  for (; iter != iterE; ++iter) {
    LVL1::CPMTower* tt = ttCheck(iter->second, tempColl);
    const LVL1::Coordinate coord(tt->phi(), tt->eta());
    const int crate = converter.cpCrateOverlap(coord);
    if (crate >= s_crates) continue;
    crateMaps[crate].insert(std::make_pair(iter->first, tt));
  }
  for (int crate = 0; crate < s_crates; ++crate) {
    InternalRoiCollection* intRois = new InternalRoiCollection;
    m_emTauTool->findRoIs(&crateMaps[crate], intRois);
    CpmTobRoiCollection* roiTemp = new CpmTobRoiCollection(SG::VIEW_ELEMENTS);
    m_cpCmxTool->formCPMTobRoI(intRois, roiTemp);
    CpmTobRoiCollection::iterator roiIter  = roiTemp->begin();
    CpmTobRoiCollection::iterator roiIterE = roiTemp->end();
    for (; roiIter != roiIterE; ++roiIter) {
      if ((*roiIter)->crate() == crate) {
        rois->push_back(*roiIter);
      }
    }
    delete roiTemp;
    delete intRois;
  }
  delete tempColl;
}

// Simulate CPM RoIs from CPM Towers quick version

void CPSimMon::simulate(const CpmTowerMap towers, CpmTobRoiCollection* rois)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CPM TOB RoIs from CPM Towers" << endreq;

  InternalRoiCollection* intRois = new InternalRoiCollection;
  m_emTauTool->findRoIs(&towers, intRois);
  m_cpCmxTool->formCPMTobRoI(intRois, rois);
  delete intRois;
}

// Simulate CMX-CP TOBs from CPM RoIs

void CPSimMon::simulate(const CpmTobRoiCollection* rois,
                              CmxCpTobCollection* tobs)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CMX TOBs from CPM TOB RoIs"
                               << endreq;

  m_cpCmxTool->formCMXCPTob(rois, tobs);
}

// Simulate CMX Hit sums from CMX TOBs

void CPSimMon::simulate(const CmxCpTobCollection* tobs,
                              CmxCpHitsCollection* hits, int selection)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CMX Hit sums from CMX TOBs"
                               << endreq;

  if (selection == LVL1::CMXCPHits::LOCAL) {
    m_cpCmxTool->formCMXCPHitsCrate(tobs, hits);
  } else if (selection == LVL1::CMXCPHits::TOPO_CHECKSUM) {
    m_cpCmxTool->formCMXCPHitsTopo(tobs, hits);
  }
}

// Simulate CMX Total Hit sums from Remote/Local

void CPSimMon::simulate(const CmxCpHitsCollection* hitsIn,
                              CmxCpHitsCollection* hitsOut)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CMX Total Hit sums from Remote/Local"
                               << endreq;

  m_cpCmxTool->formCMXCPHitsSystem(hitsIn, hitsOut);
}

// Return EM FPGA for given crate/phi

int CPSimMon::fpga(int crate, double phi)
{
  const double phiGran = M_PI/32.;
  const double phiBase = M_PI/2. * double(crate);
  const int phiBin = int(floor((phi - phiBase) / phiGran)) + 2;
  return 2 * (phiBin/2);
}

// Return a tower with zero energy if parity bit is set

LVL1::CPMTower* CPSimMon::ttCheck(LVL1::CPMTower* tt,
                                  CpmTowerCollection* coll)
{
  const LVL1::DataError emError(tt->emError());
  const LVL1::DataError hadError(tt->hadError());
  const int emParity  = emError.get(LVL1::DataError::Parity);
  const int hadParity = hadError.get(LVL1::DataError::Parity);
  if ((emParity && tt->emEnergy()) || (hadParity && tt->hadEnergy())) {
    const int peak = tt->peak();
    std::vector<int> emEnergyVec(tt->emEnergyVec());
    std::vector<int> hadEnergyVec(tt->hadEnergyVec());
    std::vector<int> emErrorVec(tt->emErrorVec());
    std::vector<int> hadErrorVec(tt->hadErrorVec());
    if (emParity)  emEnergyVec[peak]  = 0;
    if (hadParity) hadEnergyVec[peak] = 0;
    LVL1::CPMTower* ct = new LVL1::CPMTower(tt->phi(), tt->eta(),
        emEnergyVec, emErrorVec, hadEnergyVec, hadErrorVec, peak);
    coll->push_back(ct);
    return ct;
  }
  return tt;
}

// Check if LimitedRoISet bit set

bool CPSimMon::limitedRoiSet(int crate)
{
  if (m_rodTES) {
    m_limitedRoi = 0;
    RodHeaderCollection::const_iterator rodIter  = m_rodTES->begin();
    RodHeaderCollection::const_iterator rodIterE = m_rodTES->end();
    for (; rodIter != rodIterE; ++rodIter) {
      const LVL1::RODHeader* rod = *rodIter;
      const int rodCrate = rod->crate() - 8;
      if (rodCrate >= 0 && rodCrate < s_crates
          && rod->dataType() == 1 && rod->limitedRoISet()) {
        m_limitedRoi |= (1<<rodCrate);
      }
    }
    m_rodTES = 0;
  }
  return (((m_limitedRoi>>crate)&0x1) == 1);
}
// ============================================================================
} // end namespace
// ============================================================================