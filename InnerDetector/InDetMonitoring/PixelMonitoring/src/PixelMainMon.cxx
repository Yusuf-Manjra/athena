/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// This tool is the head of the monitoring. It is called by the job options and
// given all the flags. This tool the does the common overhead jobs and calls
// functions which do each individual job. One function to book and fill each 
// storegate type for each environment.
// For example, filling cluster histograms for Details modules.
//
// Written by: Nathan Triplett (nathan.triplett@cern.ch), Cecile Lapoire (cecile.lapoire@cern.ch)
// Maintained by: Louise Skinnari (louise.skinnari@cern.ch)
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
//#include "TrkParameters/MeasuredAtaPlane.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkTrack/TrackCollection.h"
//#include "StoreGate/StoreGateSvc.h"
//#include "StoreGate/StoreGate.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "TH2S.h"
#include "TProfile2D.h"
#include "PixelMonitoring/DBMMon2DMaps.h"
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMonProfiles.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include <stdint.h>
#include <vector>
#include <sstream>
#include <fstream> // kazuki
#include <cstdlib> // kazuki
#include <map> // kazuki

#include "GaudiKernel/StatusCode.h"
//#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "LumiBlockComps/ILuminosityTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"

//////////////////////////////////////////////////////////////////////////////

std::vector<std::string> &splitter(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> splitter(const std::string &s, char delim) {
  std::vector<std::string> elems;
  splitter(s, delim, elems);
  return elems;
}

bool is_file_exist(const char *fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}

PixelMainMon::PixelMainMon(const std::string & type, 
   const std::string & name,
   const IInterface* parent) :
   ManagedMonitorToolBase(type, name, parent),
   m_pixelCondSummarySvc("PixelConditionsSummarySvc",name),
   m_ErrorSvc("PixelByteStreamErrorsSvc",name),
   m_pixelCableSvc("PixelCablingSvc",name),
   m_IBLParameterSvc("IBLParameterSvc",name),
   m_holeSearchTool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool"),
   m_lumiTool("LuminosityTool"),
   m_moduleTemperature(new dcsDataHolder()),
   m_coolingPipeTemperatureInlet(new dcsDataHolder()),
   m_coolingPipeTemperatureOutlet(new dcsDataHolder()),
   m_HV(new dcsDataHolder()),
   m_HV_current(new dcsDataHolder()),
   m_LV_voltage(new dcsDataHolder()),
   m_LV_current(new dcsDataHolder()),
   m_FSM_state(new dcsDataHolder()),
   m_FSM_status(new dcsDataHolder()),
   m_moduleDCSDataHolder(new moduleDcsDataHolder())
   //m_trkSummaryTool("Trk::TrackSummaryTool/InDetTrackSummaryTool")
{                                                                        //all job options flags go here
   declareProperty("PixelConditionsSummarySvc", m_pixelCondSummarySvc);
   declareProperty("PixelByteStreamErrorsSvc",  m_ErrorSvc);
   declareProperty("PixelCablingSvc",           m_pixelCableSvc);
   declareProperty("HoleSearchTool",            m_holeSearchTool);
   declareProperty("LuminosityTool",            m_lumiTool);

   //declareProperty("TrkSummaryTool",            m_trkSummaryTool);

   declareProperty("RDOName",          m_Pixel_RDOName         = "PixelRDOs");  //storegate container names
   declareProperty("RODErrorName",     m_detector_error_name   = "pixel_error_summary");   
   declareProperty("SpacePointName",   m_Pixel_SpacePointsName = "PixelSpacePoints"); 
   declareProperty("ClusterName",      m_Pixel_SiClustersName  = "PixelClusters");  
   declareProperty("TrackName",        m_TracksName            = "Pixel_Cosmic_Tracks");         

   declareProperty("onTrack",          m_doOnTrack         = false); //using inner detector tracks
   declareProperty("onPixelTrack",     m_doOnPixelTrack    = false); //using pixel only tracks
   declareProperty("do2DMaps",         m_do2DMaps          = false);   
   declareProperty("doModules",        m_doModules         = false); 
   declareProperty("doFEChipSummary",  m_doFEChipSummary   = false);
   declareProperty("doOffline",        m_doOffline         = false);
   declareProperty("doOnline",         m_doOnline          = false);
   declareProperty("doLowOccupancy",   m_doLowOccupancy    = false);
   declareProperty("doHighOccupancy",  m_doHighOccupancy   = false);
   declareProperty("doPixelOccupancy", m_doPixelOccupancy  = false); 
   declareProperty("doRodSim",         m_doRodSim          = false);
   declareProperty("doDetails",        m_doDetails         = false);
   declareProperty("doSpectrum",       m_doSpectrum        = false);
   declareProperty("doNoiseMap",       m_doNoiseMap        = false);
   declareProperty("doTiming",         m_doTiming          = false);
   declareProperty("doLumiBlock",      m_doLumiBlock       = false);
   declareProperty("doOfflineAnalysis",m_doOfflineAnalysis = false); // !!! if true using a lot of memory to be absolutely avoided for monitoring

   declareProperty("doRDO",           m_doRDO        = false); //flags to turn on/off parts of the code
   declareProperty("doErrors",        m_doRODError   = false);
   declareProperty("doSpacePoint",    m_doSpacePoint = false);
   declareProperty("doCluster",       m_doCluster    = false);
   declareProperty("doTrack",         m_doTrack      = false);
   declareProperty("doHoleSearch",    m_doHoleSearch = false);
   declareProperty("doStatus",        m_doStatus     = false);
   declareProperty("doDCS",           m_doDCS        = false);

   declareProperty("doDegFactorMap",  m_doDegFactorMap = true);
   declareProperty("doHeavyIonMon",   m_doHeavyIonMon = false);

   declareProperty("doIBL",           m_doIBL = false);
   declareProperty("doESD",           m_doESD = false);

   declareProperty("DetailsMod1",     m_DetailsMod1 = "");
   declareProperty("DetailsMod2",     m_DetailsMod2 = "");
   declareProperty("DetailsMod3",     m_DetailsMod3 = "");
   declareProperty("DetailsMod4",     m_DetailsMod4 = "");
   declareProperty("OccupancyCut",    m_occupancy_cut = 1e-5);

   m_lbRange = 3000;
   m_bcidRange = 3600;
   m_isNewRun = false;
   m_isNewLumiBlock = false;
   m_newLowStatInterval = false;
   m_doRefresh = false;
   m_doRefresh5min = false;
   m_firstBookTime = 0;
   m_currentBCID = 0;
   m_isFirstBook = false;
   m_nRefresh = 0;
   m_nRefresh5min = 0;
   m_ntracksPerEvent = 0;
   memset(m_nGood_mod, 0, sizeof(m_nGood_mod));
   memset(m_nActive_mod, 0, sizeof(m_nActive_mod));
   m_pixelid =0;
   m_event =0;
   m_event2 =0;
   m_startTime =0; 
   m_majorityDisabled =0;
   m_lumiBlockNum =0;
   m_currentTime =0;
   m_LBstartTime =0;
   m_LBendTime =0;
   m_runNum =0;
   m_idHelper =0;
   m_Pixel_clcontainer =0;
   m_Pixel_spcontainer =0;
   m_tracks =0;

   ///
   /// Initalize all pointers for histograms
   /// 
   ///
   /// Event info
   ///
   m_events_per_lumi = 0;
   m_mu_vs_bcid = 0;
   m_mu_vs_lumi = 0;
   m_storegate_errors = 0;
   ///
   /// Hits
   ///
   /// number of hits
   m_hits_per_lumi = 0;
   m_num_hits = 0;
   m_hitmap_tmp = 0;
   m_hitmap_mon = 0;
   memset(m_nhits_mod, 0, sizeof(m_nhits_mod));
   memset(m_hits_per_lumi_mod, 0, sizeof(m_hits_per_lumi_mod));
   memset(m_nlargeevt_per_lumi_mod, 0, sizeof(m_nlargeevt_per_lumi_mod));
   memset(m_totalhits_per_bcid_mod, 0, sizeof(m_totalhits_per_bcid_mod));
   /// hit occupancy
   m_avgocc_per_lumi = 0;
   m_avgocc_ratioIBLB0_per_lumi = 0;
   memset(m_avgocc_per_lumi_mod, 0, sizeof(m_avgocc_per_lumi_mod));
   memset(m_avgocc_per_bcid_mod, 0, sizeof(m_avgocc_per_bcid_mod));
   memset(m_avgocc_per_bcid_per_lumi_mod, 0, sizeof(m_avgocc_per_bcid_per_lumi_mod));
   memset(m_avgocc_active_per_lumi_mod, 0, sizeof(m_avgocc_active_per_lumi_mod));
   memset(m_maxocc_per_lumi_mod, 0, sizeof(m_maxocc_per_lumi_mod));
   memset(m_maxocc_per_bcid_mod, 0, sizeof(m_maxocc_per_bcid_mod));
   m_occupancy = 0;
   m_average_pixocc = 0;
   m_occupancy_pix_evt = 0;
   m_occupancy_10min = 0;
   m_occupancy_time1 = 0;
   m_occupancy_time2 = 0;
   m_occupancy_time3 = 0;
   memset(m_occupancy_summary_mod, 0, sizeof(m_occupancy_summary_mod));
   memset(m_nFEswithHits_mod, 0, sizeof(m_nFEswithHits_mod));
   /// hit tot
   memset(m_hit_ToT, 0, sizeof(m_hit_ToT));
   memset(m_hit_ToT_per_lumi_mod, 0, sizeof(m_hit_ToT_per_lumi_mod));
   memset(m_hit_ToT_tmp_mod, 0, sizeof(m_hit_ToT_tmp_mod));
   memset(m_hit_ToT_Mon_mod, 0, sizeof(m_hit_ToT_Mon_mod));
   memset(m_ToT_etaphi_mod, 0, sizeof(m_ToT_etaphi_mod));
   memset(m_hit_ToTMean_mod, 0, sizeof(m_hit_ToTMean_mod));
   /// timing
   m_Lvl1ID_PIX = 0;                
   m_Lvl1ID_IBL = 0;                
   m_Atlas_BCID = 0;            
   m_Atlas_BCID_hits = 0;            
   m_BCID = 0;                  
   m_BCID_Profile = 0;          
   m_Lvl1A = 0;
   memset(m_Lvl1A_mod, 0, sizeof(m_Lvl1A_mod));
   memset(m_Lvl1A_10min_mod, 0, sizeof(m_Lvl1A_10min_mod));
   memset(m_Lvl1ID_diff_mod_ATLAS_mod, 0, sizeof(m_Lvl1ID_diff_mod_ATLAS_mod));
   memset(m_diff_ROD_vs_Module_BCID_mod, 0, sizeof(m_diff_ROD_vs_Module_BCID_mod));
   m_Lvl1ID_diff_mod_ATLAS_per_LB = 0;
   m_Lvl1ID_absdiff_mod_ATLAS_per_LB = 0;
   /// Quick status
   m_nhits_L0_B11_S2_C6 = 0;
   m_occupancy_L0_B11_S2_C6 = 0;
   /// module histo
   m_hit_num_mod = 0;
   m_hiteff_mod = 0;
   m_FE_chip_hit_summary = 0;
   m_pixel_occupancy = 0;
   /// ROD Sim
   m_RodSim_BCID_minus_ToT = 0;
   m_RodSim_FrontEnd_minus_Lvl1ID = 0;
   /// details
   m_Details_mod1_num_hits = 0;
   m_Details_mod2_num_hits = 0;
   m_Details_mod3_num_hits = 0;
   m_Details_mod4_num_hits = 0;
   m_Details_mod1_occupancy = 0;
   m_Details_mod2_occupancy = 0;
   m_Details_mod3_occupancy = 0;
   m_Details_mod4_occupancy = 0;
   m_Details_mod1_ToT = 0;     
   m_Details_mod2_ToT = 0;     
   m_Details_mod3_ToT = 0;     
   m_Details_mod4_ToT = 0;  
   ///
   /// track histograms
   ///
   /// track quality
   m_track_res_phi = 0; 
   m_track_pull_phi = 0;
   m_track_res_eta = 0; 
   m_track_pull_eta = 0;
   m_track_chi2 = 0;
   /// the number of tracks
   m_tracksPerEvt_per_lumi = 0;
   m_tracksPerEvtPerMu_per_lumi = 0;
   /// track state on surface
   m_tsos_hitmap = 0;
   m_tsos_holemap = 0;
   m_tsos_outliermap = 0;
   //m_tsos_measratio = 0;
   //m_tsos_holeratio = 0;
   m_misshits_ratio = 0;
   m_tsos_holeratio_tmp = 0;
   m_tsos_holeratio_mon = 0;
   m_misshits_ratio_tmp = 0;
   m_misshits_ratio_mon = 0;
   /// hit efficiency
   memset(m_hiteff_incl_mod, 0, sizeof(m_hiteff_incl_mod));
   /// Lorentz Angle
   m_LorentzAngle_IBL = 0;
   m_LorentzAngle_IBL2D = 0;
   m_LorentzAngle_IBL3D = 0;
   m_LorentzAngle_B0 = 0;
   m_LorentzAngle_B1 = 0;
   m_LorentzAngle_B2 = 0;
   /// degradation factor
   m_degFactorMap = 0;
   m_degFactorMap_per_lumi = 0;
   /// cluster size
   memset(m_clusize_ontrack_mod, 0, sizeof(m_clusize_ontrack_mod));
   memset(m_clusize_offtrack_mod, 0, sizeof(m_clusize_offtrack_mod));
   /// module histo
   m_track_chi2_bcl1 = 0;
   m_track_chi2_bcl0 = 0;
   m_track_chi2_bclgt1 = 0;
   m_track_chi2_bcl1_highpt = 0;
   m_track_chi2_bcl0_highpt = 0;
   m_track_chi2_bclgt1_highpt = 0;
   m_clustot_vs_pt = 0;
   m_clustot_lowpt = 0;
   m_1hitclustot_lowpt = 0;
   m_2hitclustot_lowpt = 0;
   m_clustot_highpt = 0;
   m_1hitclustot_highpt = 0;
   m_2hitclustot_highpt = 0;
   m_tsos_hiteff_vs_lumi = 0;
   ///
   /// Cluster histograms
   /// 
   m_clusters_per_lumi = 0;
   memset(m_clusters_per_lumi_mod, 0, sizeof(m_clusters_per_lumi_mod));
   memset(m_clusters_col_width_per_lumi_mod, 0, sizeof(m_clusters_col_width_per_lumi_mod));
   memset(m_clusters_row_width_per_lumi_mod, 0, sizeof(m_clusters_row_width_per_lumi_mod));
   memset(m_clusters_col_width_per_bcid_mod, 0, sizeof(m_clusters_col_width_per_bcid_mod));
   memset(m_clusters_row_width_per_bcid_mod, 0, sizeof(m_clusters_row_width_per_bcid_mod));
   m_largeclusters_per_lumi = 0;
   m_verylargeclusters_per_lumi = 0;
   m_totalclusters_per_lumi = 0;
   memset(m_totalclusters_per_lumi_mod, 0, sizeof(m_totalclusters_per_lumi_mod));
   memset(m_totalclusters_per_bcid_mod, 0, sizeof(m_totalclusters_per_bcid_mod));
   m_highNclusters_per_lumi = 0;
   memset(m_cluster_ToT1d_mod, 0, sizeof(m_cluster_ToT1d_mod));
   memset(m_1cluster_ToT_mod, 0, sizeof(m_1cluster_ToT_mod));
   memset(m_2cluster_ToT_mod, 0, sizeof(m_2cluster_ToT_mod));
   memset(m_3cluster_ToT_mod, 0, sizeof(m_3cluster_ToT_mod));
   memset(m_bigcluster_ToT_mod, 0, sizeof(m_bigcluster_ToT_mod));
   memset(m_cluster_Q_mod, 0, sizeof(m_cluster_Q_mod));
   memset(m_1cluster_Q_mod, 0, sizeof(m_1cluster_Q_mod));
   memset(m_2cluster_Q_mod, 0, sizeof(m_2cluster_Q_mod));
   memset(m_3cluster_Q_mod, 0, sizeof(m_3cluster_Q_mod));
   memset(m_bigcluster_Q_mod, 0, sizeof(m_bigcluster_Q_mod));
   m_clussize_map = 0;
   m_cluscharge_map = 0;
   m_clusToT_map = 0;
   m_cluster_groupsize = 0;
   m_cluster_col_width = 0;
   m_cluster_row_width = 0;
   memset(m_cluster_col_width_mod, 0, sizeof(m_cluster_col_width_mod));
   memset(m_cluster_row_width_mod, 0, sizeof(m_cluster_row_width_mod));
   memset(m_cluster_groupsize_mod, 0, sizeof(m_cluster_groupsize_mod));
   m_cluster_LVL1A = 0;
   memset(m_cluster_LVL1A1d_mod, 0, sizeof(m_cluster_LVL1A1d_mod));
   m_clusterSize_eta = 0;
   memset(m_clusToT_vs_eta_mod, 0, sizeof(m_clusToT_vs_eta_mod));
   memset(m_ToT_vs_clussize_mod, 0, sizeof(m_ToT_vs_clussize_mod));
   memset(m_clussize_vs_eta_mod, 0, sizeof(m_clussize_vs_eta_mod));
   m_clustermap_mon = 0;
   m_clustermap_tmp = 0;
   m_cluster_occupancy = 0;
   m_cluster_occupancy_FE_B0_mon = 0;
   m_cluster_occupancy_time1 = 0;
   m_cluster_occupancy_time2 = 0;
   m_cluster_occupancy_time3 = 0;
   m_clusocc_sizenot1 = 0; 
   m_cluseff_mod = 0;
   m_cluster_ToT_mod = 0;
   m_cluster_size_mod = 0;
   m_cluster_num_mod = 0;
   m_num_clusters = 0;
   memset(m_clusters_per_track_per_lumi_mod, 0, sizeof(m_clusters_per_track_per_lumi_mod));
   memset(m_num_clusters_mod, 0, sizeof(m_num_clusters_mod));
   memset(m_cluster_occupancy_summary_mod, 0, sizeof(m_cluster_occupancy_summary_mod));
   m_cluster_LVL1A_mod = 0;
   m_clus_LVL1A_sizenot1 = 0; 
   m_clustersOnOffTrack_per_lumi = 0;
   /// Quick status
   m_clusters_onTrack_L0_B11_S2_C6 = 0;
   m_clusters_offTrack_L0_B11_S2_C6 = 0;
   ///
   /// Status
   ///
   m_Status_modules = 0;
   m_status = 0;
   m_status_mon = 0;
   m_status_LB = 0;           
   m_disabled = 0;
   m_dqStatus = 0;
   m_disabledModules_per_lumi_PIX = 0;
   memset(m_badModules_per_lumi_mod, 0, sizeof(m_badModules_per_lumi_mod));
   memset(m_disabledModules_per_lumi_mod, 0, sizeof(m_disabledModules_per_lumi_mod));
   memset(m_baddisabledModules_per_lumi_mod, 0, sizeof(m_baddisabledModules_per_lumi_mod));
   ///
   /// Errors
   ///
   memset(m_errhist_errcat_avg, 0, sizeof(m_errhist_errcat_avg));
   memset(m_errhist_tot_LB, 0, sizeof(m_errhist_tot_LB));
   m_errhist_syncerr_LB_pix = 0;
   memset(m_errhist_errcat_LB, 0, sizeof(m_errhist_errcat_LB));
   memset(m_errhist_errtype_LB, 0, sizeof(m_errhist_errtype_LB));
   m_error_time1 = 0;       
   m_error_time2 = 0;       
   m_error_time3 = 0;       
   m_errors = 0;
   memset(m_errhist_errtype_map, 0, sizeof(m_errhist_errtype_map));
   memset(m_errhist_errcat_map, 0, sizeof(m_errhist_errcat_map));
   memset(m_errhist_expert_maps, 0, sizeof(m_errhist_expert_maps));
   memset(m_errhist_expert_LB, 0, sizeof(m_errhist_expert_LB));
   memset(m_errhist_per_bit_LB, 0, sizeof(m_errhist_per_bit_LB));
   memset(m_errhist_per_type_LB, 0, sizeof(m_errhist_per_type_LB));
   memset(m_errhist_expert_fe_trunc_err_3d, 0, sizeof(m_errhist_expert_fe_trunc_err_3d));
   m_errhist_expert_servrec_ibl_unweighted = 0;
   m_errhist_expert_servrec_ibl_weighted = 0;
   m_errhist_expert_servrec_ibl_count = 0;
   memset(m_errhist_expert_LB_maps, 0, sizeof(m_errhist_expert_LB_maps));
   m_errors_LB = 0;           
   m_errors_RODSync_mod = 0;
   m_errors_ModSync_mod = 0;
   ///
   /// Space Point
   ///
   m_num_spacepoints = 0;
   m_num_spacepoints_low = 0;
   m_spHit_x = 0;
   m_spHit_y = 0; 
   m_spHit_z = 0;
   m_spHit_r = 0;
   m_spHit_phi = 0;
   m_spHit_xy = 0;
   m_spHit_rz = 0;
   ///
   /// Per 20 LB
   ///
   m_cluster_occupancy_LB = 0;
   m_cluster_ToT_mod_LB = 0;
   m_cluster_num_mod_LB = 0;
   m_hit_num_mod_LB = 0;
   m_num_hits_LB = 0;
   memset(m_hit_ToT_LB_mod, 0, sizeof(m_hit_ToT_LB_mod));
   m_cluster_ToT_LB = 0;      
   m_num_clusters_LB = 0;                                
   ///
   /// DCS Monitorning
   ///
   /// TEMPERATURE
   m_hist_moduleTemperatureEtaPhi = 0;
   memset(m_hist_moduleTemperature2Dscatter, 0, sizeof(m_hist_moduleTemperature2Dscatter));
   memset(m_hist_moduleTemperatureLB, 0, sizeof(m_hist_moduleTemperatureLB));
   m_hist_LB_staveID_moduleTemperature = 0;
   memset(m_hist_LB_moduleGroup_moduleTemperature, 0, sizeof(m_hist_LB_moduleGroup_moduleTemperature));
   /// HV
   m_hist_HVoltageEtaPhi = 0;
   memset(m_hist_HVoltage2Dscatter, 0, sizeof(m_hist_HVoltage2Dscatter));
   memset(m_hist_HVoltageLB, 0, sizeof(m_hist_HVoltageLB));
   m_hist_LB_staveID_HVoltage = 0;
   memset(m_hist_LB_moduleGroup_HVoltage, 0, sizeof(m_hist_LB_moduleGroup_HVoltage));
   /// PIPES Inlet
   //m_hist_Pipes_inletEtaPhi = 0;
   memset(m_hist_Pipes_inletLB, 0, sizeof(m_hist_Pipes_inletLB));
   m_hist_Pipes_inlet2Dscatter = 0;
   m_hist_LB_staveID_coolingPipeInlet = 0;
   /// PIPES Outlet
   //m_hist_Pipes_outletEtaPhi = 0;
   memset(m_hist_Pipes_outletLB, 0, sizeof(m_hist_Pipes_outletLB));
   m_hist_Pipes_outlet2Dscatter = 0;
   m_hist_LB_staveID_coolingPipeOutlet = 0;
   /// LV
   m_hist_LVoltageEtaPhi = 0;
   memset(m_hist_LVoltage2Dscatter, 0, sizeof(m_hist_LVoltage2Dscatter));
   m_hist_LB_staveID_LVoltage = 0;
   memset(m_hist_LB_moduleGroup_LVoltage, 0, sizeof(m_hist_LB_moduleGroup_LVoltage));
   /// LV current
   m_hist_LVcurrentEtaPhi = 0;
   memset(m_hist_LVcurrent2Dscatter, 0, sizeof(m_hist_LVcurrent2Dscatter));
   m_hist_LB_staveID_LVcurrent = 0;
   memset(m_hist_LVcurrentLB, 0, sizeof(m_hist_LVcurrentLB));
   memset(m_hist_LB_moduleGroup_LVcurrent, 0, sizeof(m_hist_LB_moduleGroup_LVcurrent));
   /// HV current
   m_hist_HVcurrentEtaPhi = 0;
   memset(m_hist_HVcurrent2Dscatter, 0, sizeof(m_hist_HVcurrent2Dscatter));
   memset(m_hist_HVcurrentLB, 0, sizeof(m_hist_HVcurrentLB));
   m_hist_LB_staveID_HVcurrent= 0;
   memset(m_hist_LB_moduleGroup_HVcurrent, 0, sizeof(m_hist_LB_moduleGroup_HVcurrent));
   /// FSM state
   m_hist_FSMstateEtaPhi = 0;
   memset(m_hist_FSMstate2Dscatter, 0, sizeof(m_hist_FSMstate2Dscatter));
   memset(m_hist_FSMstateLB, 0, sizeof(m_hist_FSMstateLB));
   m_hist_LB_staveID_FSMstate = 0;
   memset(m_hist_LB_moduleGroup_FSMstate, 0, sizeof(m_hist_LB_moduleGroup_FSMstate));
   /// FSM status
   m_hist_FSMstatusEtaPhi = 0;
   memset(m_hist_FSMstatus2Dscatter, 0, sizeof(m_hist_FSMstatus2Dscatter));
   memset(m_hist_FSMstatusLB, 0, sizeof(m_hist_FSMstatusLB));
   m_hist_LB_staveID_FSMstatus = 0;
   memset(m_hist_LB_moduleGroup_FSMstatus, 0, sizeof(m_hist_LB_moduleGroup_FSMstatus));
   /// dT
   m_hist_dTEtaPhi = 0;
   m_hist_LB_staveID_dT = 0;
   memset(m_hist_LB_moduleGroup_dT, 0, sizeof(m_hist_LB_moduleGroup_dT)); // dT := module temp - cooling pipe outlet
   /// LV Power Consumption
   m_hist_LVPowerConsumptionEtaPhi = 0;
   m_hist_LB_staveID_LVPowerConsumption = 0;
   memset(m_hist_LB_moduleGroup_LVPowerConsumption, 0, sizeof(m_hist_LB_moduleGroup_LVPowerConsumption));
   /// HV Popwer Consumtion
   m_hist_HVPowerConsumptionEtaPhi = 0;
   m_hist_LB_staveID_HVPowerConsumption = 0;
   memset(m_hist_LB_moduleGroup_HVPowerConsumption, 0, sizeof(m_hist_LB_moduleGroup_HVPowerConsumption));
   /// LV+HV Power Consumption
   m_hist_LVHVPowerConsumptionEtaPhi = 0;
   m_hist_LB_staveID_LVHVPowerConsumption = 0;
   memset(m_hist_LB_moduleGroup_LVHVPowerConsumption, 0, sizeof(m_hist_LB_moduleGroup_LVHVPowerConsumption));
   /// effective FLEX temp
   m_hist_effFLEXtempEtaPhi = 0;
   m_hist_LB_staveID_effFLEXtemp = 0;
   memset(m_hist_LB_moduleGroup_effFLEXtemp, 0, sizeof(m_hist_LB_moduleGroup_effFLEXtemp));
   /// thermal figure of merit
   m_hist_thermalFigureMeritEtaPhi = 0;
   m_hist_LB_staveID_thermalFigureMerit = 0;
   memset(m_hist_LB_moduleGroup_thermalFigureMerit, 0, sizeof(m_hist_LB_moduleGroup_thermalFigureMerit));
}
   
PixelMainMon::~PixelMainMon()
{
  delete m_moduleTemperature;
  delete m_coolingPipeTemperatureInlet;
  delete m_coolingPipeTemperatureOutlet;
  delete m_HV;
  delete m_HV_current;
  delete m_LV_voltage;
  delete m_LV_current;
  delete m_FSM_state;
  delete m_FSM_status;
  delete m_moduleDCSDataHolder;

}


StatusCode PixelMainMon::initialize()
{
   ATH_CHECK( ManagedMonitorToolBase::initialize() );
   time ( &m_startTime );  //mark time for start of run
   //m_idHelper = new AtlasDetectorID; // not need "new"

   // Retrieve tools
   if (detStore()->retrieve(m_pixelid, "PixelID").isFailure()) {
      msg(MSG::FATAL) << "Could not get Pixel ID helper" << endmsg; 
      return StatusCode::FAILURE;
   }

   // Get the dictionary manager from the detector store

   ATH_CHECK(  detStore()->retrieve(m_idHelper, "AtlasID") );

   if ( m_pixelCondSummarySvc.retrieve().isFailure() ) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL)  << "Failed to retrieve tool " << m_pixelCondSummarySvc << endmsg;
      return StatusCode::FAILURE;
   } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_pixelCondSummarySvc << endmsg;
   }

   if ( m_pixelCableSvc.retrieve().isFailure() ) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL)  << "Failed to retrieve tool " << m_pixelCableSvc << endmsg;
      return StatusCode::FAILURE;
   } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_pixelCableSvc << endmsg;
   }

   if ( m_ErrorSvc.retrieve().isFailure() ) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL)  << "Failed to retrieve tool " << m_ErrorSvc << endmsg;
      return StatusCode::FAILURE;
   } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_ErrorSvc << endmsg;
   }

   if (m_IBLParameterSvc.retrieve().isFailure()) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Could not retrieve IBLParameterSvc" << endmsg;
      return StatusCode::FAILURE;
   } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_IBLParameterSvc << endmsg;
   }

   m_doIBL = m_IBLParameterSvc->containsIBL();
   msg(MSG::INFO)<<"doIBL set to "<<m_doIBL<<endmsg;

   if(m_doIBL && !m_IBLParameterSvc->contains3D()){
      msg(MSG::WARNING)<<"Assuming hybrid 2D/3D IBL module composition, but geometry is all-planar"<<endmsg;
   }

   if(m_doHoleSearch){
      if ( m_holeSearchTool.retrieve().isFailure() ) {
         msg(MSG::FATAL) << "Failed to retrieve tool " << m_holeSearchTool << endmsg;
         return StatusCode::FAILURE;
      }else{
         msg(MSG::INFO) << "Retrieved tool " << m_holeSearchTool << endmsg;
      }
   }

   if ( m_lumiTool.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_lumiTool << endmsg;
      return StatusCode::FAILURE;
   } else {
      msg(MSG::INFO) << "Retrieved tool " << m_lumiTool << endmsg;
   }

   //if ( m_trkSummaryTool.retrieve().isFailure() ) {
   //  if ( msgLvl(MSG::ERROR) ) msg(MSG::ERROR) << "Failed to retrieve tool " << m_trkSummaryTool << endmsg;
   //} else {
   //  if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_trkSummaryTool << endmsg;
   //}

  //m_elementsMap[std::string("/PIXEL/DCS/FSMSTATE")].push_back(std::string("FSM_state"));                    // type: String4k
  //m_elementsMap[std::string("/PIXEL/DCS/FSMSTATUS")].push_back(std::string("FSM_status"));                  // type: String4k
  //m_elementsMap[std::string("/PIXEL/DCS/HV")].push_back(std::string("HV"));                                 // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/HVCURRENT")].push_back(std::string("hv_current"));                  // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/LV")].push_back(std::string("lv_voltage"));                         // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/LV")].push_back(std::string("lv_current"));                         // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/PIPES")].push_back(std::string("temp_inlet"));                      // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/PIPES")].push_back(std::string("temp_outlet"));                     // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/PLANTS")].push_back(std::string("modbus_ack_user_setpoint_temp"));  // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/PLANTS")].push_back(std::string("modbus_cooling_ready"));           // type: Bool
  //m_elementsMap[std::string("/PIXEL/DCS/TEMPERATURE")].push_back("temperature");                            // type: Float

  if (!m_doDCS) return StatusCode::SUCCESS;

  m_atrcollist.push_back(std::string("/PIXEL/DCS/TEMPERATURE")); // module
  m_atrcollist.push_back(std::string("/PIXEL/DCS/HV"));          // readout unit
  m_atrcollist.push_back(std::string("/PIXEL/DCS/FSMSTATE"));    // readout unit
  m_atrcollist.push_back(std::string("/PIXEL/DCS/FSMSTATUS"));   // readout unit
  m_atrcollist.push_back(std::string("/PIXEL/DCS/PIPES"));       // IBL stave
  m_atrcollist.push_back(std::string("/PIXEL/DCS/HVCURRENT"));   // IBL readout unit
  //m_atrcollist.push_back(std::string("/PIXEL/DCS/PLANTS"));      // 1
  m_atrcollist.push_back(std::string("/PIXEL/DCS/LV"));          // IBL readout unit
  m_currentLumiBlockNumber = 0;

  //std::string testarea = std::getenv("TestArea");
  //ifstream moduleMapfile((testarea + "/InstallArea/share/wincc2cool.csv").c_str());
  //ifstream coolingPipeMapfile((testarea + "/InstallArea/share/coolingPipeMap.csv").c_str());
  //ifstream lvMapfile((testarea + "/InstallArea/share/lvMap.csv").c_str());

  //std::string cmtpath = std::getenv("CMTPATH");
  std::string cmtpath = std::getenv("DATAPATH");
  std::vector<std::string> paths = splitter(cmtpath, ':');
  for (const auto& x : paths){
    if(is_file_exist((x + "/wincc2cool.csv").c_str())){
      ATH_MSG_INFO("initialising DCS channel maps using files in " << x);
      std::ifstream moduleMapfile((x + "/wincc2cool.csv").c_str());
      std::ifstream coolingPipeMapfile((x + "/coolingPipeMap.csv").c_str());
      std::ifstream lvMapfile((x + "/lvMap.csv").c_str());
      if( moduleMapfile.fail() || coolingPipeMapfile.fail() || lvMapfile.fail() ) {
        ATH_MSG_WARNING("initialize(): Map File do not exist. m_doDCS has been changed to False.");
        m_doDCS = false;
      }
      std::string line;
      // make a dictionary to convert module name to channel number
      int channel; std::string moduleName; std::string rest;
      std::string inletName; std::string outletName;
      std::string lvVoltageName; std::string lvCurrentName;
      //while(getline(moduleMapfile, line))
      while(moduleMapfile >> channel >> moduleName >> rest) {
        // get channel number from wincc2cool.csv
        //int channel; std::string moduleName; std::string rest;
        //channel = atoi( ( line.substr(0, line.find(",")) ).c_str() );
        //std::string tmp_line = line.substr(line.find(","), std::string::npos);
        //// get the module name from wincc2cool.csv
        //moduleName = tmp_line.substr(0, tmp_line.find(","));
        m_moduleTemperature->m_maps->insert(std::make_pair(moduleName, channel));
        m_HV->m_maps->insert(std::make_pair(moduleName, channel));
        m_moduleDCSDataHolder->m_moduleMap->insert(std::make_pair(moduleName, channel));
        ATH_MSG_DEBUG( "initialize(): channel " << channel << ", moduleName " << moduleName );
      }
      // for cooling pipe
      while(coolingPipeMapfile >> channel >> inletName >> outletName) {
        m_coolingPipeTemperatureInlet->m_maps->insert(std::make_pair(inletName, channel));
        m_coolingPipeTemperatureOutlet->m_maps->insert(std::make_pair(outletName, channel));
      }
      while(lvMapfile >> channel >> lvVoltageName >> lvCurrentName) {
        m_LV_voltage->m_maps->insert(std::make_pair(lvVoltageName, channel));
        m_LV_current->m_maps->insert(std::make_pair(lvCurrentName, channel));
      }
      break;
    }
  }

  m_name2etaIndex["A_M4_A8_2"] = 9;
  m_name2etaIndex["A_M4_A8_1"] = 8;
  m_name2etaIndex["A_M4_A7_2"] = 7;
  m_name2etaIndex["A_M4_A7_1"] = 6;
  m_name2etaIndex["A_M3_A6"] = 5;
  m_name2etaIndex["A_M3_A5"] = 4;
  m_name2etaIndex["A_M2_A4"] = 3;
  m_name2etaIndex["A_M2_A3"] = 2;
  m_name2etaIndex["A_M1_A2"] = 1;
  m_name2etaIndex["A_M1_A1"] = 0;
  m_name2etaIndex["C_M1_C1"] = -1;
  m_name2etaIndex["C_M1_C2"] = -2;
  m_name2etaIndex["C_M2_C3"] = -3;
  m_name2etaIndex["C_M2_C4"] = -4;
  m_name2etaIndex["C_M3_C5"] = -5;
  m_name2etaIndex["C_M3_C6"] = -6;
  m_name2etaIndex["C_M4_C7_1"] = -7;
  m_name2etaIndex["C_M4_C7_2"] = -8;
  m_name2etaIndex["C_M4_C8_1"] = -9;
  m_name2etaIndex["C_M4_C8_2"] = -10;

  m_name2moduleGroup["C_M4"] = 0;
  m_name2moduleGroup["C_M3"] = 1;
  m_name2moduleGroup["C_M2"] = 2;
  m_name2moduleGroup["C_M1"] = 3;
  m_name2moduleGroup["A_M1"] = 4;
  m_name2moduleGroup["A_M2"] = 5;
  m_name2moduleGroup["A_M3"] = 6;
  m_name2moduleGroup["A_M4"] = 7;

  m_fsmState2enum["READY"] = 0.;
  m_fsmState2enum["ON"] = 1.;
  m_fsmState2enum["STANDBY"] = 2.;
  m_fsmState2enum["LV_ON"] = 3.;
  m_fsmState2enum["TRANSITION"] = 4.;
  m_fsmState2enum["UNDEFINED"] = 5.;
  m_fsmState2enum["LOCKED_OUT"] = 6.;
  m_fsmState2enum["DISABLED"] = 7.;
  m_fsmState2enum["OFF"] = 8.;
  m_fsmState2enum["DEAD"] = 9.;

  m_fsmStatus2enum["OK"] = 0.;
  m_fsmStatus2enum["WARNING"] = 1.;
  m_fsmStatus2enum["ERROR"] = 2.;
  m_fsmStatus2enum["UNINITIALIZED"] = 3.;
  m_fsmStatus2enum["DEAD"] = 4.;

   return StatusCode::SUCCESS;
}


StatusCode PixelMainMon::bookHistograms() 
{ 
  // m_isNewRun = newRun;
  // m_isNewLumiBlock = newLumiBlock;
  // m_newLowStatInterval = newLowStatInterval;
  //m_doOnline = true;

   const EventInfo* thisEventInfo;
   if(evtStore()->retrieve(thisEventInfo) != StatusCode::SUCCESS) {
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "No EventInfo object found" << endmsg;
   }else{
      m_lumiBlockNum = thisEventInfo->event_ID()->lumi_block();

      if(m_doOnline){
         m_runNum = thisEventInfo->event_ID()->run_number();
         std::stringstream runNumStr;
         runNumStr << m_runNum;
         m_histTitleExt = " (Run " + runNumStr.str() + ")";
      }else{
         m_histTitleExt = "";
      }
      if ( newLumiBlockFlag() ) {
         m_LBstartTime = thisEventInfo->event_ID()->time_stamp();
      }
      if( !m_isFirstBook ){
         m_firstBookTime = thisEventInfo->event_ID()->time_stamp();
         m_isFirstBook = true;
      }
   }

   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "bookHistograms()" << endmsg; 

   if(m_environment==AthenaMonManager::tier0Raw&&!m_doOnline)
   {
     //m_doRDO        = false;
     //m_doRODError   = false;
     //m_doSpacePoint = false;
     //m_doCluster    = false;
     //m_doStatus     = false;
     //m_doTrack      = false;
   }
   if(m_environment==AthenaMonManager::tier0ESD&&!m_doOnline)
   {
     m_doRDO        = false;
     m_doRODError   = false;
     m_doSpacePoint = false;
     m_doCluster    = false;
     m_doStatus     = false;
     m_doDCS        = false;
     m_doTrack      = false;
   }

   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "[PixelMonitoring] flags in bookHisto" << endmsg;
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_doESD       " << m_doESD            << endmsg;
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_doRDO       " << m_doRDO            << endmsg;
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_doRODError  " << m_doRODError       << endmsg;
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_doSpacePoint" << m_doSpacePoint     << endmsg;
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_doCluster   " << m_doCluster        << endmsg;
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_doStatus    " << m_doStatus         << endmsg;
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_doDCS       " << m_doDCS            << endmsg;
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_doTrack     " << m_doTrack          << endmsg;


   ///
   /// Book histograms
   ///

   if(m_doLumiBlock){
      if(m_doRDO){                                                    
	if (BookHitsLumiBlockMon().isFailure()) {
	  if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book lowStat histograms" << endmsg; 
	}
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking RDO for lowStat" << endmsg;  
      }
      if(m_doRODError){
        if (BookRODErrorLumiBlockMon().isFailure()) {
	  if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book lowStat histograms" << endmsg; 
	}
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking ROD Error for lowStat" << endmsg;  
      }
      if(m_doCluster){
        if (BookClustersLumiBlockMon().isFailure()) {
	  if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book lowStat histograms" << endmsg; 
	}
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Cluster for lowStat" << endmsg;  
      }
      if(m_doStatus){
        if (BookStatusLumiBlockMon().isFailure()) {
	  if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book lowStat histograms" << endmsg; 
	}
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Status for lowStat" << endmsg;  
      }
   }

   if( m_doRDO ){  // only RAWtoESD
   ///
   /// Mu vs LB (BCID)
   ///
     std::string path_hits = "Pixel/Hits";
     if(m_doOnTrack)      path_hits.replace(path_hits.begin(), path_hits.end(), "Pixel/HitsOnTrack");
     if(m_doOnPixelTrack) path_hits.replace(path_hits.begin(), path_hits.end(), "Pixel/HitsOnPixelTrack");
     MonGroup hitsHistos(   this, path_hits.c_str(),  run, ATTRIB_MANAGED ); //declare a group of histograms
     StatusCode sc;
     sc = hitsHistos.regHist(m_mu_vs_bcid = TProfile_LW::create("Interactions_vs_bcid", "<Interactions> vs BCID;BCID;<#Interactions/event>"    , m_bcidRange,-0.5,-0.5+(1.0*m_bcidRange)));
     sc = hitsHistos.regHist(m_mu_vs_lumi = TProfile_LW::create("Interactions_vs_lumi", "<Interactions> vs LB;lumi block;<#Interactions/event>", m_lbRange,-0.5,-0.5+(1.0*m_lbRange)));
     sc = hitsHistos.regHist(m_events_per_lumi = TH1F_LW::create("Events_per_lumi", "nEvents vs LB;lumi block;#events", m_lbRange,-0.5,-0.5+(1.0*m_lbRange)));
   ///
   /// Storegate errors
   ///
   std::string path = "Pixel/Errors";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnPixelTrack");
   MonGroup errorHistos( this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms
   sc = errorHistos.regHist(m_storegate_errors = TH2F_LW::create("storegate_errors",  ("Storegate Errors" + m_histTitleExt + ";Container Name;Error Type").c_str(), 6,0.5,6.5,5,0.5,5.5));
   if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endmsg; 
   
   ///
   ///Set lables and style for error histogram
   ///
   m_storegate_errors->SetOption("colz");
   const char *xlabel[6] = { "RDOs", "SpacePoints", "Clusters", "Tracks", "RODErrors", "DCS" };
   for (int i=0; i<6; i++){
      if(m_storegate_errors) m_storegate_errors->GetXaxis()->SetBinLabel( i+1, xlabel[i] );  // bin 0 is underflow
   }
   const char *ylabel[5] = { "Not flagged", "Container not found", "Can't retrieve container", "Container empty", "Data is bad"};
   for (int i=0; i<5; i++){
      if(m_storegate_errors) m_storegate_errors->GetYaxis()->SetBinLabel( i+1, ylabel[i] );  // bin 0 is underflow
   }
   }
   
   ///
   /// Initialize histograms
   ///
   if(m_doTrack)
   {
     if (BookTrackMon().isFailure()) {
       if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endmsg; 
     }
     if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Track" << endmsg;  
   }
   if(m_doRDO)
   {                                                    
     if (BookHitsMon().isFailure()) {
       if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endmsg; 
     }
     if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking RDO" << endmsg;  
   }
   if(m_doRODError) 
   {
     if (BookRODErrorMon().isFailure()) {
       if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endmsg; 
     }
     if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking ROD Error" << endmsg;  
   }
   if(m_doSpacePoint)
   {
     if (BookSpacePointMon().isFailure()) {
       if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endmsg; 
     }
     if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking SP" << endmsg;  
   }
   if(m_doCluster)
   {
     if (BookClustersMon().isFailure()) {
       if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endmsg; 
     }
     if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Cluster" << endmsg;  
   }
   if(m_doStatus)
   {
     if (BookStatusMon().isFailure()) {
       if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endmsg; 
     }
     if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Status" << endmsg;  
   }
   if(m_doDCS)
   {
     if (BookPixelDCSMon().isFailure()) {
       if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endmsg; 
     }
     if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking DCS" << endmsg;  
   }

   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PixelMainMon::fillHistograms() //get called twice per event 
{
   //Get required services, we want to repeat as little as possible in the function calls
   // Part 1: Get the messaging service, print where you are                

   m_event++;       
   m_majorityDisabled = false;

   const EventInfo* thisEventInfo;
   if(evtStore()->retrieve(thisEventInfo) != StatusCode::SUCCESS) {
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "No EventInfo object found" << endmsg;
   }else{
      m_currentTime = thisEventInfo->event_ID()->time_stamp(); 
      m_currentBCID = thisEventInfo->event_ID()->bunch_crossing_id();
      //msg(MSG::INFO) << "First booking time:" << m_firstBookTime << " Current time: " << m_currentTime << endmsg;
      unsigned int currentdiff = (m_currentTime - m_firstBookTime)/100;
      unsigned int currentdiff5min = (m_currentTime - m_firstBookTime)/300;
      /// for 100 sec
      if( currentdiff > m_nRefresh ){
         m_doRefresh = true;
         m_nRefresh = currentdiff;
      //   msg(MSG::INFO) << "m_doRefresh:" << m_doRefresh << " " << m_nRefresh << endmsg;
      }else{
         m_doRefresh = false;
      }
      /// for 5min
      if( currentdiff5min > m_nRefresh5min ){
         m_doRefresh5min = true;
         m_nRefresh5min = currentdiff5min;
      //   msg(MSG::INFO) << "m_doRefresh:" << m_doRefresh << " " << m_nRefresh << endmsg;
      }else{
         m_doRefresh5min = false;
      }

   }


   PixelID::const_id_iterator idIt    = m_pixelid->wafer_begin();
   PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();

   for( int i=0; i<PixLayerIBL2D3D::COUNT; i++){
      m_nGood_mod[i] = 0;
      m_nActive_mod[i] = 0;
   }

   for (; idIt != idItEnd; ++idIt)
   {
      Identifier WaferID = *idIt;
      IdentifierHash id_hash = m_pixelid->wafer_hash(WaferID);

      int pixlayeribl2d3d = GetPixLayerID(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_doIBL);
      if( pixlayeribl2d3d == PixLayer::kIBL ){
         pixlayeribl2d3d = GetPixLayerIDIBL2D3D(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_pixelid->eta_module(WaferID), m_doIBL);
      }
      if( pixlayeribl2d3d == 99 ) continue;
      if(m_pixelCondSummarySvc->isActive(id_hash) == true){
         m_nActive_mod[pixlayeribl2d3d]++;
      }
      if(m_pixelCondSummarySvc->isActive(id_hash) == true && m_pixelCondSummarySvc->isGood(id_hash) == true ){
         m_nGood_mod[pixlayeribl2d3d]++;
      }
   }
   m_nActive_mod[PixLayerIBL2D3D::kIBL] = 2*m_nActive_mod[PixLayerIBL2D3D::kIBL2D] + m_nActive_mod[PixLayerIBL2D3D::kIBL3D];
   m_nGood_mod[PixLayerIBL2D3D::kIBL] = 2*m_nGood_mod[PixLayerIBL2D3D::kIBL2D] + m_nGood_mod[PixLayerIBL2D3D::kIBL3D];

   ////////////////////////////////////////////////////////////////
   // Fill methods go here
   ////////////////////////////////////////////////////////////////

   ///
   /// Event Info
   ///
   if( m_doRDO ){  // only RAWtoESD
     if(m_mu_vs_bcid) m_mu_vs_bcid->Fill( m_currentBCID, thisEventInfo->actualInteractionsPerCrossing() );
     if(m_mu_vs_lumi) m_mu_vs_lumi->Fill( m_manager->lumiBlockNumber(), thisEventInfo->actualInteractionsPerCrossing());
     if(m_events_per_lumi) m_events_per_lumi->Fill(m_manager->lumiBlockNumber());
   }
   
   ///
   /// Pixel Info
   ///
   if(m_doStatus) { //fill status histograms first, to check number of disabled modules
     if(FillStatusMon().isFailure()) {
       if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endmsg; 
     }
   }

   //if(m_doRODError&&evtStore()->contains<PixelRODErrorCollection>(m_detector_error_name))
   if(m_doRODError)
   {
     if (FillRODErrorMon().isFailure()) {
       if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endmsg; 
     }
   }else{
      if(m_storegate_errors) m_storegate_errors->Fill(5.,1.);
   }

   /// Track
   if(m_doTrack){
      if(evtStore()->contains< TrackCollection >(m_TracksName)){
	if (FillTrackMon().isFailure()) {
          if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endmsg; 
        }
      }else if(m_storegate_errors) m_storegate_errors->Fill(4.,2.);
   }else{
      if(m_storegate_errors) m_storegate_errors->Fill(4.,1.);
   }

   /// Hits
   if(m_doRDO){
     if(evtStore()->contains<PixelRDO_Container>(m_Pixel_RDOName) ) {
       if (FillHitsMon().isFailure()) {
	 if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endmsg; 
       }
     }
     else if(m_storegate_errors) m_storegate_errors->Fill(1.,2.); 
   }else{
      if(m_storegate_errors) m_storegate_errors->Fill(1.,1.); 
   }

   /// Cluster
   if(m_doCluster){
      if(evtStore()->contains<InDet::PixelClusterContainer>(m_Pixel_SiClustersName)){
	if (FillClustersMon().isFailure()) {
          if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endmsg; 
        }
      }else if(m_storegate_errors) m_storegate_errors->Fill(3.,2.);
   }else{
      if(m_storegate_errors) m_storegate_errors->Fill(3.,1.);
   }

   /// Space Point
   if(m_doSpacePoint)
   {
      if(evtStore()->contains<SpacePointContainer>(m_Pixel_SpacePointsName)){
	if (FillSpacePointMon().isFailure()) {
          if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endmsg; 
        }
      }else if(m_storegate_errors) m_storegate_errors->Fill(2.,2.);
   }else{
      if(m_storegate_errors) m_storegate_errors->Fill(2.,1.);
   }


   /// DCS
   if(m_doDCS){
     if (FillPixelDCSMon().isFailure()) {
       if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endmsg;
     }
   }else{
      if(m_storegate_errors)m_storegate_errors->Fill(6.,1.);
   }


   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PixelMainMon::procHistograms()
{ 

  if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG)  << "finalize()" << endmsg;

  if ( endOfLumiBlockFlag() )
    {
      m_LBendTime = m_currentTime;
      //if (m_doTrack) { sc=ProcTrackMon(); }
      //if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not proc histograms" << endmsg; 
    }
  
  if ( !m_doOnline && endOfRunFlag() )
    {
      if (m_doRDO) {
        if (ProcHitsMon().isFailure()) {
          if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not proc histograms" << endmsg; 
        }
      }
      if (m_doCluster) {
        if (ProcClustersMon().isFailure()) {
          if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not proc histograms" << endmsg; 
        }
      }
      if (m_doStatus) {
        if (ProcStatusMon().isFailure()) {
          if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not proc histograms" << endmsg; 
        }
      }
      if (m_doDCS) {
        if (ProcPixelDCSMon().isFailure()) {
          if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not proc histograms" << endmsg; 
        }
      }
      if (m_doTrack) {
        if (ProcTrackMon().isFailure()) {
          if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not proc histograms" << endmsg; 
        }
      }
    }
  
  return StatusCode::SUCCESS;
}


