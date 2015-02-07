/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMETMonitoring/HLTMETMonTool.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <functional>

#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TH2F.h"

//___________________________________________________________________________________________________________
HLTMETMonTool::HLTMETMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : IHLTMonTool(type, name, parent),
    m_debuglevel(false) {

  ATH_MSG_DEBUG("in HLTMETMonTool::HLTMETMonTool()");
  
  declareProperty("PrintMETTriggerStats", m_is_print_met_trig_stats=true);
  declareProperty("MakeHistosForAllMETTriggers", m_is_make_histos_for_all_met_trig=false);
  declareProperty("MakeExpertHistos", m_is_make_expert_histos=true);
  declareProperty("MonPathBase", m_mon_path="/HLT/METMon");
  declareProperty("DoTriggerEfficiency", m_is_do_trigger_effic=true);

  // Triggers to monitor
  declareProperty("METTriggersL1", m_met_triggers_l1);
  declareProperty("METTriggersHLT", m_met_triggers_hlt);
  declareProperty("SampleSelectionTriggers", m_sample_selection_triggers);

  // Met keys
  declareProperty("L1_ROI_Key", m_lvl1_roi_key="LVL1EnergySumRoI");
  declareProperty("HLT_METKey", m_hlt_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET");
  declareProperty("HLT_FEBKey", m_hlt_met_feb_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEB");
  declareProperty("HLT_TCKey",  m_hlt_met_topocl_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl");
  declareProperty("Off_METKey", m_off_met_key="MET_Reference_AntiKt4LCTopo");  

  declareProperty("CompNames", m_compNames);
  declareProperty("BitNames", m_bitNames);

  // histograms limits
  // et
  declareProperty("EtMin",  m_et_min=-13.5, "Minimum Et");
  declareProperty("EtMax",  m_et_max=601.5, "Maximum Et");
  declareProperty("EtBins", m_et_bins=205,  "Number of bins Et");
  // sumet
  declareProperty("SumetMin",  m_sumet_min=-27.,  "Minimum Sumet");
  declareProperty("SumetMax",  m_sumet_max=1803., "Maximum Sumet");
  declareProperty("SumetBins", m_sumet_bins=305,  "Number of bins Sumet");
  // phi, dphi
  declareProperty("PhiMin",  m_phi_min=-3.1416, "Minimum Phi");
  declareProperty("PhiMax",  m_phi_max=3.1416,  "Maximum Phi");
  declareProperty("PhiBins", m_phi_bins=32,     "Number of bins Phi, dPhi");
  // dEt, dEx, dEy
  declareProperty("dEtMin",  m_det_min=-100., "Minimum dEt, dEx, dEy");
  declareProperty("dEtMax",  m_det_max=100.,  "Maximum dEt, dEx, dEy");
  declareProperty("dEtBins", m_det_bins=100,  "Number of bins dEt, dEx, dEy");
}


//___________________________________________________________________________________________________________
HLTMETMonTool::~HLTMETMonTool() {
  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HLTMETMonTool::~HLTMETMonTool()");
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::init() {

  // init message stream
  m_log->setLevel(outputLevel());
  m_debuglevel = (m_log->level() <= MSG::DEBUG);

  if (m_debuglevel) {
    ATH_MSG_DEBUG("in HLTMETMonTool::init()");
    ATH_MSG_DEBUG("HLTMETMonTool: L1Key  = "     << m_lvl1_roi_key);
    ATH_MSG_DEBUG("HLTMETMonTool: HLT_METKey = " << m_hlt_met_key);
    ATH_MSG_DEBUG("HLTMETMonTool: HLT_FEBKey = " << m_hlt_met_feb_key);
    ATH_MSG_DEBUG("HLTMETMonTool: HLT_TCKey = "  << m_hlt_met_topocl_key);
    ATH_MSG_DEBUG("HLTMETMonTool: Off_METKey = " << m_off_met_key);
  }

  // point to the signatures we want to make histos for
  if (m_is_make_histos_for_all_met_trig) {
    m_l1_met_signatures_tolook = &m_all_l1_met_triggers;
    m_hlt_met_signatures_tolook = &m_all_hlt_met_triggers;
  } else {
    m_l1_met_signatures_tolook = &m_l1_met_signatures;
    m_hlt_met_signatures_tolook = &m_hlt_met_signatures;
  }
  
  return StatusCode::SUCCESS;
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::book() {

  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HLTMETMonTool::book()");

  // get all met triggers
  if(m_is_make_histos_for_all_met_trig) 
    getAllMETTriggers();  
  else { 
    // met triggers specified in the input list
    // check if input met triggers are in trigger configuration
    if(m_met_triggers_l1.size())
      checkTriggers(m_met_triggers_l1, m_l1_met_signatures);
    if(m_met_triggers_hlt.size())
      checkTriggers(m_met_triggers_hlt, m_hlt_met_signatures);
  }
  
  // sample selection triggers specified in the input list
  if(m_sample_selection_triggers.size()) {
    // check if input sample selection triggers are in triggger configuration
    // put the correct met triggers in m_sample_selection_signatures
    checkTriggers(m_sample_selection_triggers, m_sample_selection_signatures);
  }

  // Create Shifter and Expert folders
  m_shifter_path = m_mon_path + "/Shifter";
  m_expert_path  = m_mon_path + "/Expert";

  addMonGroup(new MonGroup(this, m_shifter_path, run));
  addMonGroup(new MonGroup(this, m_expert_path, run));

  //-- Book shifter histograms
  // L1 histograms
  std::string mongroupName = m_shifter_path + "/L1";
  addMonGroup(new MonGroup(this, mongroupName.c_str(), run));
  setCurrentMonGroup(mongroupName);

  addHistogram(new TH1F("L1_METx", "L1 METx (GeV);METx (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("L1_METy", "L1 METy (GeV);METy (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("L1_MEx_log", "L1 Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 55, -4.125, 4.125));
  addHistogram(new TH1F("L1_MEy_log", "L1 Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 55, -4.125, 4.125));
  addHistogram(new TH1F("L1_MET_log", "L1 |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 40, -1.875, 4.125));
  addHistogram(new TH1F("L1_SumEt_log", "L1 Sum |E_{T}|;log_{10}(SumE_{T}/GeV)", 40, -1.875, 4.125));
  
  addBasicL1Histograms();

  // HLT histograms
  mongroupName = m_shifter_path + "/HLT";
  addMonGroup(new MonGroup(this, mongroupName.c_str(), run));
  setCurrentMonGroup(mongroupName);

  addHistogram(new TH1F("HLT_MEx_lin", "HLT Missing E_{x};E_{x} (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("HLT_MEy_lin", "HLT Missing E_{y};E_{y} (GeV)", 199, -298.5,  298.5));

  addBasicHLTHistograms();
  addMoreHLTHistograms();
  addHLTStatusHistogram();
  addL1vsHLTHistograms();

  // HTL_FEB histograms
  mongroupName = m_shifter_path + "/HLT_FEB";
  addMonGroup(new MonGroup(this, mongroupName.c_str(), run));
  setCurrentMonGroup(mongroupName);

  addHistogram(new TH1F("HLT_FEB_MEx_lin", "HLT Missing E_{x};E_{x} (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("HLT_FEB_MEy_lin", "HLT Missing E_{y};E_{y} (GeV)", 199, -298.5,  298.5));

  addBasicHLTHistograms();
  addMoreHLTHistograms();

  // HTL_topocl histograms
  mongroupName = m_shifter_path + "/HLT_topocl";
  addMonGroup(new MonGroup(this, mongroupName.c_str(), run));
  setCurrentMonGroup(mongroupName);

  addHistogram(new TH1F("HLT_topocl_MEx_lin", "HLT Missing E_{x};E_{x} (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("HLT_topocl_MEy_lin", "HLT Missing E_{y};E_{y} (GeV)", 199, -298.5,  298.5));

  addBasicHLTHistograms();
  addMoreHLTHistograms();

  // Offline histograms
  mongroupName = m_shifter_path + "/Off";
  addMonGroup(new MonGroup(this, mongroupName.c_str(), run));
  setCurrentMonGroup(mongroupName);

  addHistogram(new TH1F("Off_MEx_lin", "Offline Missing E_{x};E_{x} (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("Off_MEy_lin", "Offline Missing E_{y};E_{y} (GeV)", 199, -298.5,  298.5));

  addBasicOffHistograms();
  addL1vsOffHistograms();
  addHLTvsOffHistograms();

  // DQFlags
  // std::string DQFlagmongroupName = m_mon_path + "/DQFlags";
  // addMonGroup (new MonGroup(this, DQFlagmongroupName.c_str(), run));
  // setCurrentMonGroup(DQFlagmongroupName);
  // addDQFlagHistograms();
    
  //-- Book expert histograms 
  if (m_is_make_expert_histos) {
    if (m_l1_met_signatures_tolook->size())
      bookExpertL1Histograms();
    if (m_hlt_met_signatures_tolook->size())
      bookExpertHLTHistograms();
    if (m_is_do_trigger_effic)
      bookExpertEfficHistograms();
  }
  
  return StatusCode::SUCCESS;
} 


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fill() {

  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HLTMETMonTool:fill()");

  trigger_decision();

  StatusCode sc = fillMETHistos();
  if (sc.isFailure()) {
    ATH_MSG_WARNING("The method fillMETHistos(...) failed");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::proc() {

  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HLTMETMonTool::proc()");

  if(m_is_print_met_trig_stats)
    printMETTriggerStats();

  std::map<std::string, int>::const_iterator it_s;
  std::map<std::string, int>::const_iterator it;
  TH1 *m_OffMET_et_denom;
  TH1 *m_OffMET_phi_denom;
  TH1 *m_OffMET_et_num;
  TH1 *m_OffMET_phi_num;
  std::string levels[2] = {"L1", "HLT"};
  std::vector<std::map<std::string,int>*> m_met_signatures_tolook;
  m_met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
  m_met_signatures_tolook.push_back(m_hlt_met_signatures_tolook);
  unsigned int size = m_met_signatures_tolook.size();

  if (m_is_do_trigger_effic) {

    //for(it_s = m_sample_selection_signatures.begin(); it_s != m_sample_selection_signatures.end(); it_s++) {
    //  setCurrentMonGroup(m_mon_path + "/Expert/Efficiency/"+it_s->first);
      setCurrentMonGroup(m_shifter_path + "/Off");
      m_OffMET_et_denom = hist("OffMET_et");
      m_OffMET_phi_denom = hist("OffMET_phi");
      m_OffMET_et_denom->Sumw2();
      m_OffMET_phi_denom->Sumw2();
      for (unsigned int i=0; i<size; i++) {
        for(it = m_met_signatures_tolook[i]->begin(); it != m_met_signatures_tolook[i]->end(); it++) {
          setCurrentMonGroup(m_expert_path + "/Efficiency/"+it->first);
          m_OffMET_et_num = hist("OffMET_et");
          m_OffMET_phi_num = hist("OffMET_phi");
          m_OffMET_et_num->Sumw2();
          m_OffMET_phi_num->Sumw2();
          std::string et_effic_name = levels[i] + "_et_effic_wrt_OffMET";
          std::string phi_effic_name = levels[i] + "_phi_effic_wrt_OffMET";
          hist(et_effic_name)->Divide(m_OffMET_et_num, m_OffMET_et_denom);
          hist(phi_effic_name)->Divide(m_OffMET_phi_num, m_OffMET_phi_denom);
        }
      }
  }

  return StatusCode::SUCCESS;
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::bookXSCalibHistograms(int level) {
  // histograms to monitor the XS calibration by plotting
  // MEx resolution vs Sqrt(SumET) for zero-bias events

  // binning in sqrt(SumET)
  int nS = 100;
  float Swidth = 0.2; // 0.1 GeV^0.5/bin
  float Smin = 0;
  float Smax = Smin + Swidth*nS;
  // binning in MET
  int nM = 101;
  float Mwidth = 3;   // 3 GeV/bin
  float Mmin = -1.5;
  float Mmax = Mmin + Mwidth*nM;

  TString title  = " MET vs #sqrt{SumET}";
  TString Slabel = " #sqrt{SumET/GeV}";
  TString Mlabel = " MET/GeV";

  // binning in MEx, MEy
  int nMx = 201;
  float Mxwidth = 3;   // 3 GeV/bin
  float Mxmin = -601.5;
  float Mxmax = Mxmin + Mxwidth*nMx;
  TString titleX = "MEx vs #sqrt{SumET}";
  TString MlabelX = "MEx/GeV";
  TString titleY = "MEy vs #sqrt{SumET}";
  TString MlabelY = "MEy/GeV";

  if(level == 1) {
    addHistogram(new TH2F("L1MEx_VS_SqrtSET_All",TString("L1")+titleX+" (All Events);L1"+Slabel+";L1"+MlabelX,nS,Smin,Smax,nMx,Mxmin,Mxmax));
    addHistogram(new TH2F("L1MEy_VS_SqrtSET_All",TString("L1")+titleY+" (All Events);L1"+Slabel+";L1"+MlabelY,nS,Smin,Smax,nMx,Mxmin,Mxmax));
    addHistogram(new TH2F("L1MET_VS_SqrtSET_All",TString("L1")+title+" (All Events);L1"+Slabel+";L1"+Mlabel,nS,Smin,Smax,nM,Mmin,Mmax));
    addHistogram(new TH2F("L1MEx_VS_SqrtSET_RD0",TString("L1")+titleX+" (ZB Events);L1"+Slabel+";L1"+MlabelX,nS,Smin,Smax,nMx,Mxmin,Mxmax));
    addHistogram(new TH2F("L1MEy_VS_SqrtSET_RD0",TString("L1")+titleY+" (ZB Events);L1"+Slabel+";L1"+MlabelY,nS,Smin,Smax,nMx,Mxmin,Mxmax));
    addHistogram(new TH2F("L1MET_VS_SqrtSET_RD0",TString("L1")+title+" (ZB Events);L1"+Slabel+";L1"+Mlabel,nS,Smin,Smax,nM,Mmin,Mmax));
  }
  
  if(level == 2) {
    addHistogram(new TH2F("HLTMEx_VS_SqrtSET_All",TString("HLT")+titleX+" (All Events);HLT"+Slabel+";HLT"+MlabelX,nS,Smin,Smax,nMx,Mxmin,Mxmax));
    addHistogram(new TH2F("HLTMEy_VS_SqrtSET_All",TString("HLT")+titleY+" (All Events);HLT"+Slabel+";HLT"+MlabelY,nS,Smin,Smax,nMx,Mxmin,Mxmax));
    addHistogram(new TH2F("HLTMET_VS_SqrtSET_All",TString("HLT")+title+" (All Events);HLT"+Slabel+";HLT"+Mlabel,nS,Smin,Smax,nM,Mmin,Mmax));
    addHistogram(new TH2F("HLTMEx_VS_SqrtSET_RD0",TString("HLT")+titleX+" (ZB Events);HLT"+Slabel+";HLT"+MlabelX,nS,Smin,Smax,nMx,Mxmin,Mxmax));
    addHistogram(new TH2F("HLTMEy_VS_SqrtSET_RD0",TString("HLT")+titleY+" (ZB Events);HLT"+Slabel+";HLT"+MlabelY,nS,Smin,Smax,nMx,Mxmin,Mxmax));
    addHistogram(new TH2F("HLTMET_VS_SqrtSET_RD0",TString("HLT")+title+" (ZB Events);HLT"+Slabel+";HLT"+Mlabel,nS,Smin,Smax,nM,Mmin,Mmax));
  }

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::bookExpertL1Histograms() {

  std::string monGroupName = m_expert_path + "/L1";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));

  std::map<std::string,int>::const_iterator it;
  for (it = m_l1_met_signatures_tolook->begin(); it != m_l1_met_signatures_tolook->end(); it++) {

    std::string m_expert_path_trigger = monGroupName + "/" + it->first;

    //declare a group of histograms
    addMonGroup(new MonGroup(this, m_expert_path_trigger, run));
    setCurrentMonGroup(m_expert_path_trigger);

    addBasicL1Histograms();
    addBasicHLTHistograms();
    addBasicOffHistograms();
    addL1vsOffHistograms();
    
    // trigger efficiency histograms
    addHistogram(new TH1F("L1_et_effic_wrt_OffMET", "et_effic_wrt_OffMET_L1 ; Et", m_et_bins, m_et_min, m_et_max));
    addHistogram(new TH1F("L1_phi_effic_wrt_OffMET", "phi_effic_wrt_OffMET_L1 ; Phi", m_phi_bins, m_phi_min, m_phi_max));
    
    if (m_debuglevel)
      ATH_MSG_DEBUG("in HLTMETMonTool::bookL1Histograms: " << m_expert_path_trigger << "/" << " Histograms" << " booked successfully");
  }
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::bookExpertHLTHistograms() {

  std::string monGroupName = m_expert_path + "/HLT";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));

  std::map<std::string,int>::const_iterator it;

  for (it = m_hlt_met_signatures_tolook->begin(); it != m_hlt_met_signatures_tolook->end(); it++) {
    std::string m_expert_path_trigger = monGroupName + "/" + it->first;

    //declare a group of histograms
    addMonGroup(new MonGroup(this, m_expert_path_trigger, run));
    setCurrentMonGroup(m_expert_path_trigger);

    addBasicHLTHistograms();
    addL1vsHLTHistograms();
    addBasicOffHistograms();
    addHLTvsOffHistograms();

    // trigger efficiency histograms
    addHistogram(new TH1F("EF_et_effic_wrt_OffMET", "et_effic_wrt_OffMET_EF ; Et", m_et_bins, m_et_min, m_et_max));
    addHistogram(new TH1F("EF_phi_effic_wrt_OffMET", "phi_effic_wrt_OffMET_EF ; Phi", m_phi_bins, m_phi_min, m_phi_max));

    if (m_debuglevel)
      ATH_MSG_DEBUG("in HLTMETMonTool::bookHLTHistograms: " << m_expert_path_trigger << "/" << " Histograms" << " booked successfully");

  }
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::bookExpertEfficHistograms() {

  std::string monGroupName = m_expert_path + "/Efficiency";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));

  std::map<std::string,int>::const_iterator it;

  //  for (it_s = m_sample_selection_signatures.begin(); it_s != m_sample_selection_signatures.end(); it_s++) {
  //    std::string m_expert_path_trigmetmonitoring = m_generic_path_trigmetmonitoring + "/Expert/Efficiency/" + it_s->first;
  //    //declare a group of histograms
  //    addMonGroup(new MonGroup(this, m_expert_path_trigmetmonitoring, run));
  //    setCurrentMonGroup(m_expert_path_trigmetmonitoring);
  //    addBasicOffHistograms();  
  
  std::vector<std::map<std::string,int>*> m_met_signatures_tolook;
  m_met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
  m_met_signatures_tolook.push_back(m_hlt_met_signatures_tolook);
  unsigned int size = m_met_signatures_tolook.size();
  std::string levels[2] = {"L1", "HLT"};
  
  for (unsigned int i=0; i<size; i++) {
    for (it = m_met_signatures_tolook[i]->begin(); it != m_met_signatures_tolook[i]->end(); it++) {
      
      std::string m_expert_path_trigger = monGroupName + "/" + it->first;

      //declare a group of histograms
      addMonGroup(new MonGroup (this, m_expert_path_trigger, run));
      setCurrentMonGroup(m_expert_path_trigger);
      addBasicOffHistograms();
   
      // trigger efficiency histograms
      std::string name = levels[i] + "_et_effic_wrt_OffMET";
      std::string title = "et_effic_wrt_OffMET_" + levels[i] + " ; Et";
      addHistogram(new TH1F(name.c_str(), title.c_str(), m_et_bins, m_et_min, m_et_max));
      name = levels[i] + "_phi_effic_wrt_OffMET";
      title = "phi_effic_wrt_OffMET_" + levels[i] + " ; Phi";
      addHistogram(new TH1F(name.c_str(), title.c_str(), m_phi_bins, m_phi_min, m_phi_max));
    
      if (m_debuglevel) {
        ATH_MSG_DEBUG("in HLTMETMonTool::bookEfficHistograms: " << m_expert_path_trigger
                      << "/" << " Histograms" << " booked successfully");
      }
    } // end of loop over l* met signatures to look
  } // end of loop over levels
    //  } // end of loop over m_sample_selection_triggers
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addBasicL1Histograms() {
  addHistogram(new TH1F("L1_MET",      "L1 MET (GeV);MET (GeV)",                      m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("L1_MET_lin1", "L1 |Missing E_{T}| (0-10 GeV);ME_{T} (GeV)",  110, -0.5, 10.5));
  addHistogram(new TH1F("L1_SumEt",    "L1 SumEt (GeV);SumEt (GeV)",                  m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("L1_MET_phi",  "L1 MET #phi (rad);MET #phi (rad)",            m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1_MET_phi1", "L1 MET #phi(|Missing E_{T}|);MET #phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
  //bookXSCalibHistograms(1);
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addBasicHLTHistograms() {
  addHistogram(new TH1F("HLT_MET_lin",   "HLT |Missing E_{T}|;ME_{T} (GeV)",            m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("HLT_MET_lin1",  "HLT |Missing E_{T}| (0-10 GeV);ME_{T} (GeV)", 110, -0.5, 10.5));
  addHistogram(new TH1F("HLT_SumEt_lin", "HLT Sum |E_{T}|;SumE_{T} (GeV)",              m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("HLT_MET_phi",   "HLT MET #phi (rad);#phi (rad)",               m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_MET_phi1",  "HLT MET #phi(|Missing E_{T}|);#phi (rad)",    m_phi_bins, m_phi_min, m_phi_max));
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addDQFlagHistograms() {

  float fMaxEFC = float(m_compNames.size());
  fMaxEFC -= 0.5;
  int fBinEFC = int(m_compNames.size());
  TH2F *h2 = new TH2F("compN_compSumEt_lin",   "HLT Sum |E_{T}| VS component;;SumE_{T} (GeV)",fBinEFC, -0.5, fMaxEFC, 236, -30,   2802);
  TH2F *h2f = new TH2F("compN_HLT_MET_status", "HLT MET Status VS component;;",fBinEFC, -0.5, fMaxEFC,32, -0.5, 31.5);
  for (size_t cn = 0; cn < m_compNames.size(); cn++) { // 25 bins
    if(cn < m_compNames.size()) h2->GetXaxis()->SetBinLabel(cn+1, m_compNames[cn].c_str());
    if(cn < m_compNames.size()) h2f->GetXaxis()->SetBinLabel(cn+1, m_compNames[cn].c_str());
  }
  addHLTStatusHistogram();
  addBasicHLTHistograms();
  
  addHistogram(new TH1F("HLT_MEx_log",   "HLT Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 55, -4.125, 4.125));
  addHistogram(new TH1F("HLT_MEy_log",   "HLT Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 55, -4.125, 4.125));
  for (size_t j = 0; j < 32; j++) {
    if(j < m_bitNames.size()) h2f->GetYaxis()->SetBinLabel(j+1, m_bitNames[j].c_str());
  }
  addHistogram(h2f); 
  addHistogram(h2);

  //TH2I *h2i = new TH2I("trmet_lbn_flag",   "Lumi-block based flags;lbn;lbn",101,-0.5,100.5, 101, -0.5, 100.5);
  //h2i->SetBit(TH1::kCanRebin);
  //addHistogram(h2i);
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addMoreHLTHistograms() {
  
  // histogram definitions
  // signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
  // central bin is +/- 0.075 and contains values whose abs < 1.189 GeV
  // limits are +/- 1.189 TeV
  addHistogram( new TH1F("HLT_MEx_log", "HLT Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 27, -4.125, 4.125));
  addHistogram( new TH1F("HLT_MEy_log", "HLT Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 27, -4.125, 4.125));
  addHistogram( new TH1F("HLT_MEz_lin", "HLT Missing E_{z};ME_{z} (GeV)",100, -298.5,298.5));
  addHistogram( new TH1F("HLT_MEz_log", "HLT Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)",27, -4.125, 4.125));
  addHistogram( new TH1F("HLT_ME_lin",  "HLT Missing Energy;ME (GeV)",102, -13.5,601.5));
  addHistogram( new TH1F("HLT_ME_log",  "HLT Missing Energy;log_{10}(ME/GeV)", 24, -0.15, 7.05));

  // signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
  // bin centered on zero is +/- 0.075 and contains values whose abs < 1.189 GeV
  // limits are -74.99 GeV and 2.371 TeV
  addHistogram(new TH1F("HLT_MET_log",   "HLT |Missing E_{T}|;log_{10}(ME_{T}/GeV)",         20, -1.875, 4.125));
  addHistogram(new TH1F("HLT_SumEt_log", "HLT Sum |E_{T}|;log_{10}(SumE_{T}/GeV)",           20, -1.875, 4.125));
  addHistogram(new TH1F("HLT_SumE_lin",   "HLT Sum |E|;SumE (GeV)",153, -27.,1803.));
  addHistogram(new TH1F("HLT_SumE_log",   "HLT Sum |E|;log_{10}(SumE/GeV)",20, -1.875, 4.125));

  // EF MET all components in one 2d histogram
  float fMaxEFC = float(m_compNames.size());
  fMaxEFC -= 0.5;
  int fBinEFC = int(m_compNames.size());
  TH2F *h2f[10] = {0};
  h2f[0] = new TH2F("compN_compEx",      "HLT MissingE_{x} VS component;;sgn(ME_{x}) log_{10}(E_{x}/GeV)", fBinEFC, -0.5, fMaxEFC, 27, -4.125, 4.125);
  h2f[1] = new TH2F("compN_compEy",      "HLT Missing E_{y} VS component;;sgn(ME_{y}) log_{10}(E_{y}/GeV)", fBinEFC, -0.5, fMaxEFC, 27, -4.125, 4.125);
  h2f[2] = new TH2F("compN_compEz",      "HLT Missing E_{z} VS component;;sgn(ME_{z}) log_{10}(E_{z}/GeV)", fBinEFC, -0.5, fMaxEFC, 27, -4.125, 4.125);
  h2f[3] = new TH2F("compN_compEt",      "HLT MissingE_{T} VS component;;sgn(ME_{T}) log_{10}(ME_{T}/GeV)", fBinEFC, -0.5, fMaxEFC, 20, -1.875, 4.125);
  h2f[4] = new TH2F("compN_compSumEt",   "HLT Sum |E_{T}| VS component;;log_{10}(SumE_{T}/GeV)", fBinEFC, -0.5, fMaxEFC, 20, -1.875, 4.125);
  h2f[5] = new TH2F("compN_compSumE",    "HLT Sum |E| VS component;;log_{10}(SumE/GeV)", fBinEFC, -0.5, fMaxEFC, 20, -1.875, 4.125);
  h2f[6] = new TH2F("compN_compEt_lin",  "HLT MissingE_{T} VS component;;ME_{T} (GeV)", fBinEFC, -0.5, fMaxEFC, 102, -13.5, 601.5);
  h2f[7] = new TH2F("compN_compSumEt_lin",   "HLT Sum |E_{T}| VS component;;SumE_{T} (GeV)", fBinEFC, -0.5, fMaxEFC, 236, -30,   2802); 
  h2f[8] = new TH2F("compN_HLT_MET_status",   "HLT MET Status VS component;;", fBinEFC, -0.5, fMaxEFC,32, -0.5, 31.5);
  h2f[9] = new TH2F("compN_HLT_usedChannels", "HLT Used Channels VS component;;log_{10}(N > 0)", fBinEFC, -0.5, fMaxEFC,13,-0.5,6.0);

  for (size_t k = 0; k < 10; k++) { // 10 hists
    for (size_t cn = 0; cn < m_compNames.size(); cn++) { // 25 bins
      if(cn < m_compNames.size()) h2f[k]->GetXaxis()->SetBinLabel(cn+1, m_compNames[cn].c_str());
    }
    // for status v. component, set bin labels for Y axis
    if (k == 8) {
      for (size_t j = 0; j < m_bitNames.size(); j++) {
        if(j < m_bitNames.size()) h2f[k]->GetYaxis()->SetBinLabel(j+1, m_bitNames[j].c_str());
      }
      h2f[k]->GetYaxis()->SetLabelFont(42);
      h2f[k]->GetYaxis()->SetLabelOffset(0.002);
      h2f[k]->GetYaxis()->SetLabelSize(0.03);
    }
    if (k == 9) {
      // 10^x (x = 0.5, 1.0 1.5 ....)
      /*std::string binlb_nc[13] = { "none", "[0-3]", "[4-9]", "[10-31]", 
        "[32-99]", "[100-316]", "[317-999]", "[1-3.16]10^{3}", "[0.32 - 1]10^{4}", 
        "[1 - 3.2]10^{4}", "[0.32 - 1]10^{5}", "[1 - 3.2]10^{5}", "[0.32 - 1]10^{6}" };
      for(size_t l = 0; l < 13; l++) {*/
      h2f[k]->GetYaxis()->SetBinLabel(1,"none");
      //}
    }
    addHistogram(h2f[k]);
  }

  int phibins = 16; double phimin = m_phi_min; double phimax = m_phi_max;
  int etabins = 24; double etamin = -4.8;   double etamax =  4.8;

  for (unsigned int j = 0; j < m_compNames.size(); j++) {
    std::string name = Form("compN_EtaPhi_%02d", j);
    std::string title = m_compNames[j];
    title += ": N(#eta, #phi);#eta;#phi [rad];";
    TH2F* h = new TH2F(name.c_str(), title.c_str(), etabins, etamin, etamax, phibins, phimin, phimax);
    addHistogram(h);

    name = Form("compEt_lin_EtaPhi_%02d", j);
    title = m_compNames[j];
    title += ": MissingE_{T}(#eta, #phi); #eta; #phi [rad];ME_{T}/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins, etamin, etamax, phibins, phimin, phimax);
    addHistogram(h);
    
    name = Form("compSumEt_lin_EtaPhi_%02d", j);
    title = m_compNames[j];
    title += ": Sum |E_{T}|(#eta, #phi); #eta; #phi [rad];Sum|E_{T}|/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins, etamin, etamax, phibins, phimin, phimax);
    addHistogram(h);
    
    name = Form("compSumE_lin_EtaPhi_%02d", j);
    title = m_compNames[j];
    title += ": Sum |E|(#eta, #phi); #eta; #phi [rad];Sum|E|/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins, etamin, etamax, phibins, phimin, phimax);
    addHistogram(h);
    
  }
  bookXSCalibHistograms(3);

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addHLTStatusHistogram() {
  TH1F *h1i = new TH1F("HLT_MET_status", "HLT MET Status",32, -0.5, 31.5);
  for (size_t j = 0; j < m_bitNames.size(); j++) {
   if(j < m_bitNames.size()) h1i->GetXaxis()->SetBinLabel(j+1, m_bitNames[j].c_str());
  }
  addHistogram(h1i);
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addBasicOffHistograms() {
  // OffMET
  // et sumet phi histograms
  addHistogram(new TH1F("OffMET_et", "et_OffMET ; Et", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("OffMET_sumet", "sumet_OffMET ; sumEt", m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("OffMET_phi", "phi_OffMET ; Phi", m_phi_bins, m_phi_min, m_phi_max));
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1vsHLTHistograms() {

  addHistogram(new TH2F("L1MET_HLTMET_EtCor", "L1 vs HLT Missing E_{T} ; HLTME_{T} (GeV); L1ME_{T} (GeV)", m_et_bins, m_et_min, m_et_max, m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH2F("L1MET_HLTMET_SumEtCor", "L1 vs HLT Sum |E_{T}| ; HLTSum|E_{T}| (GeV); L1Sum|E_{T}| (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max, m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH2F("L1MET_HLTMET_PhiCor", "L1 vs HLT #phi ; EF #phi (rad); L1 #phi (rad)", m_phi_bins, m_phi_min, m_phi_max, m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1MET_HLTMET_dEt",  "#DeltaE_{T}(L1-HLT) ; #DeltaE_{T} (GeV)", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("L1MET_HLTMET_dPhi", "#Delta#phi(L1-HLT) ; #Delta#phi (GeV)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1MET_HLTMET_dEx",  "#DeltaE_{x}(L1-HLT) ; #DeltaE_{x} (GeV)", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("L1MET_HLTMET_dEy",  "#DeltaE_{y}(L1-HLT) ; #DeltaE_{y} (GeV)", m_det_bins, m_det_min, m_det_max));

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1vsOffHistograms() {

  addHistogram(new TH2F("L1MET_OffMET_EtCor", "EtCor_L1MET_OffMET ; OffMET_Et ; L1MET_Et", m_et_bins, m_et_min, m_et_max, m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH2F("L1MET_OffMET_SumEtCor", "SumEtCor_L1MET_OffMET ; OffMET_SumEt ; L1MET_SumEt", m_sumet_bins, m_sumet_min, m_sumet_max, m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH2F("L1MET_OffMET_PhiCor", "PhiCor_L1MET_OffMET ; OffMET_Phi ; L1MET_Phi", m_phi_bins, m_phi_min, m_phi_max, m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1MET_OffMET_dEt", "dEt_L1MET_OffMET ; dEt", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("L1MET_OffMET_dPhi", "dPhi_L1MET_OffMET ; dPhi", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1MET_OffMET_dEx", "dEx_L1MET_OffMET ; dEx", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("L1MET_OffMET_dEy", "dEy_L1MET_OffMET ; dEy", m_det_bins, m_det_min, m_det_max));
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addHLTvsOffHistograms() {

  addHistogram(new TH2F("HLTMET_OffMET_EtCor", "EtCor_HLTMET_OffMET; OffMET_Et ; HLTMET_Et", m_et_bins, m_et_min, m_et_max, m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH2F("HLTMET_OffMET_SumEtCor", "SumEtCor_HLTMET_OffMET; OffMET_SumEt ; HLTMET_SumEt", m_sumet_bins, m_sumet_min, m_sumet_max, m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH2F("HLTMET_OffMET_PhiCor", "PhiCor_HLTMET_OffMET; OffMET_Phi ; HLTMET_Phi", m_phi_bins, m_phi_min, m_phi_max, m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLTMET_OffMET_dEt", "dEt_HLTMET_OffMET; dEt", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("HLTMET_OffMET_dPhi", "dPhi_HLTMET_OffMET; dPhi", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLTMET_OffMET_dEx", "dEx_HLTMET_OffMET; dEx", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("HLTMET_OffMET_dEy", "dEy_HLTMET_OffMET; dEy", m_det_bins, m_det_min, m_det_max));   
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::trigger_decision() {

  std::vector<std::map<std::string,int>*> m_met_signatures_tolook;
  m_met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
  m_met_signatures_tolook.push_back(m_hlt_met_signatures_tolook);

  if (m_is_print_met_trig_stats) {
    std::map<std::string,int>::iterator iter;
    for (unsigned int i = 0; i<m_met_signatures_tolook.size(); i++) {
      for (iter = m_met_signatures_tolook[i]->begin(); iter != m_met_signatures_tolook[i]->end(); ++iter) {
        std::string name = iter->first;
        if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
          iter->second +=1;
        }
      }
    }
  }
}

//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillMETHistos() {

  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HTLMETMonTool::fillMETHistos()");

  StatusCode sc = StatusCode::SUCCESS;

  // retrieve xAOD L1 ROI 
  m_l1_roi_cont = 0;
  sc = m_storeGate->retrieve(m_l1_roi_cont, m_lvl1_roi_key);
  if(sc.isFailure() || !m_l1_roi_cont) {
    ATH_MSG_DEBUG ("Could not retrieve LVL1_RoIs with key \"" << m_lvl1_roi_key << "\" from TDS"  );
  }

  // retrieve EF containers
  m_hlt_met_cont = 0;
  sc = m_storeGate->retrieve(m_hlt_met_cont, m_hlt_met_key);
  if (sc.isFailure() || !m_hlt_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met with " << m_hlt_met_cont->size() << " elements");

  m_hlt_met_feb_cont = 0;
  sc = m_storeGate->retrieve(m_hlt_met_feb_cont, m_hlt_met_feb_key);
  if (sc.isFailure() || !m_hlt_met_feb_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met with " << m_hlt_met_feb_cont->size() << " elements");

  m_hlt_met_topocl_cont = 0;
  sc = m_storeGate->retrieve(m_hlt_met_topocl_cont, m_hlt_met_topocl_key);
  if (sc.isFailure() || !m_hlt_met_topocl_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_met_topocl_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met with " << m_hlt_met_topocl_cont->size() << " elements");

  // retrieve Offline MET container
  m_off_met_cont = 0;
  sc = m_storeGate->retrieve(m_off_met_cont, m_off_met_key);
  if (sc.isFailure() || !m_off_met_cont) {
    ATH_MSG_DEBUG("Could not retrieve Reconstructed MET term with Key " << m_off_met_key << " : m_off_met_cont = 0");
  }
  else if (m_debuglevel) {
    ATH_MSG_DEBUG("Got Reconstructed MET term with key " << m_off_met_key);
    ATH_MSG_DEBUG("m_off_met_cont = " << m_off_met_cont);
  }

  // fill shifter histograms
  sc = fillL1ShifterHistos(); 
  sc = fillHLTShifterHistos();
  sc = fillOffShifterHistos();

  // fill expert histograms
  sc = fillExpertHistos();

  return sc;
}

StatusCode HLTMETMonTool::fillL1ShifterHistos() {

  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HLTMETMonTool::fillL1ShiftHistos()");

  if(!m_l1_roi_cont) return StatusCode::SUCCESS;

  std::string mongroupName = m_shifter_path + "/L1";
  setCurrentMonGroup(mongroupName.c_str());

  TH1 *h(0);
  //TH2 *h2(0);
  float epsilon = 1e-6;  // 1 keV
  float epsilon_l1met = 1.e-3; // 1 MeV
  
  setCurrentMonGroup(mongroupName.c_str());
  float m_lvl1_mex = -9e9;
  float m_lvl1_mey = -9e9;
  float m_lvl1_met = -9e9;
  float m_lvl1_set = -9e9;
  float m_lvl1_phi = -9e9;
  float m_lvl1_mex_log = -9e9;
  float m_lvl1_mey_log = -9e9;
  float m_lvl1_met_log = -9e9;
  float m_lvl1_set_log = -9e9;
  
  m_lvl1_mex = - (m_l1_roi_cont->energyX())/CLHEP::GeV;
  m_lvl1_mey = - (m_l1_roi_cont->energyY())/CLHEP::GeV;
  m_lvl1_met = sqrt(m_lvl1_mex*m_lvl1_mex + m_lvl1_mey*m_lvl1_mey);
  m_lvl1_phi = atan2f(m_lvl1_mey,m_lvl1_mex);
  m_lvl1_set = (m_l1_roi_cont->energyT())/CLHEP::GeV; 

  if(m_lvl1_met > epsilon_l1met) {
    if((h = hist("L1_METx")))   h->Fill(m_lvl1_mex);
    if((h = hist("L1_METy")))   h->Fill(m_lvl1_mey);
    if((h = hist("L1_MET")))   h->Fill(m_lvl1_met);
    if((h = hist("L1_MET_lin1")))   h->Fill(m_lvl1_met);
    if((h = hist("L1_SumEt")))   h->Fill(m_lvl1_set);
    if((h = hist("L1_MET_phi")))   h->Fill(m_lvl1_phi);
    if((h = hist("L1_MET_phi1")))   h->Fill(m_lvl1_phi,m_lvl1_met);
  }
  if (fabsf(m_lvl1_mex)>epsilon)
    m_lvl1_mex_log = copysign(log10(fabsf(m_lvl1_mex)), m_lvl1_mex);
  else
    m_lvl1_mex_log = 0.01;
  
  if (fabsf(m_lvl1_mey)>epsilon)
    m_lvl1_mey_log = copysign(log10(fabsf(m_lvl1_mey)), m_lvl1_mey);
  else
    m_lvl1_mey_log = 0.01;
  
  if (fabsf(m_lvl1_met)>epsilon)
    m_lvl1_met_log = copysign(log10(fabsf(m_lvl1_met)), m_lvl1_met);
  else
    m_lvl1_met_log = 0.01;
  
  if (fabsf(m_lvl1_set)>epsilon)
    m_lvl1_set_log = copysign(log10(fabsf(m_lvl1_set)), m_lvl1_set);
  else
    m_lvl1_set_log = 0.01;
  
  if(m_lvl1_met > epsilon_l1met) {
    if((h = hist("L1_MEx_log")))   h->Fill(m_lvl1_mex_log);
    if((h = hist("L1_MEy_log")))   h->Fill(m_lvl1_mey_log);
    if((h = hist("L1_MET_log")))   h->Fill(m_lvl1_met_log);
    if((h = hist("L1_SumEt_log"))) h->Fill(m_lvl1_set_log);
  }
  float m_sqrt_set = sqrt(m_lvl1_set > epsilon ? m_lvl1_set:0.);
  // if((h2 = hist2("L1MET_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_lvl1_met);
  // if((h2 = hist2("L1MEx_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_lvl1_mex);
  // if((h2 = hist2("L1MEy_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_lvl1_mey);
  // bool pass_zb = getTDT()->isPassed("L1_RD0_FILLED", TrigDefs::eventAccepted);
  // if(pass_zb) {
  //   if((h2 = hist2("L1MET_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_lvl1_met);
  //   if((h2 = hist2("L1MEx_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_lvl1_mex);
  //   if((h2 = hist2("L1MEy_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_lvl1_mey);
  // }
  //}

  return StatusCode::SUCCESS; 
}

//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillHLTShifterHistos() {

  if (m_hlt_met_cont && m_hlt_met_cont->size()>0)
    m_hlt_met = m_hlt_met_cont->at(0);
  else {
    ATH_MSG_DEBUG("in fillHLTShiftHistos: HLT container empty");
    return StatusCode::SUCCESS;
  }

  TH1 *h(0);
  TH2 *h2(0);
  std::string mongroupName = m_shifter_path + "/HLT";
  setCurrentMonGroup(mongroupName.c_str());	

  float m_etx_log = -9e9;
  float m_ety_log = -9e9;
  float m_etz_log = -9e9;
  float m_met_log = -9e9;
  float m_set_log = -9e9;
  float m_me_log = -9e9;
  float m_se_log = -9e9;
  float m_etx_lin = -9e9;
  float m_ety_lin = -9e9;
  float m_etz_lin = -9e9;
  float m_met_lin = -9e9;
  float m_set_lin = -9e9;
  float m_se_lin = -9e9;
  float m_me_lin = -9e9;
  float m_phi = -9e9;

  float m_ETx     = m_hlt_met->ex();
  float m_ETy     = m_hlt_met->ey();
  float m_ETz     = m_hlt_met->ez();
  float m_MET     = sqrt(m_ETx*m_ETx+m_ETy*m_ETy);
  float m_SumEt   = m_hlt_met->sumEt();
  float m_SumE    = m_hlt_met->sumE();

  // monitoring values (in CLHEP::GeV)
  m_etx_lin = m_ETx/CLHEP::GeV; 
  m_ety_lin = m_ETy/CLHEP::GeV; 
  m_etz_lin = m_ETz/CLHEP::GeV;
  m_me_lin  = sqrt(m_etx_lin*m_etx_lin+m_ety_lin*m_ety_lin+m_etz_lin*m_etz_lin);
  m_met_lin = m_MET/CLHEP::GeV; 
  m_set_lin = m_SumEt/CLHEP::GeV;
  m_se_lin  = m_SumE/CLHEP::GeV; 
  m_phi = atan2f(m_ETy, m_ETx);

  float epsilon = 1e-6;  // 1 keV
  if (m_me_lin>epsilon)  m_me_log  = log10(fabsf(m_me_lin)); // underflow otherwise

  epsilon=1.189;
  if (fabsf(m_etx_lin)>epsilon)
    m_etx_log = copysign(log10(fabsf(m_etx_lin)), m_etx_lin);
  else
    m_etx_log = 0.01;
  if (fabsf(m_ety_lin)>epsilon)
    m_ety_log = copysign(log10(fabsf(m_ety_lin)), m_ety_lin);
  else
    m_ety_log = 0.01;
  if (fabsf(m_etz_lin)>epsilon)
    m_etz_log = copysign(log10(fabsf(m_etz_lin)), m_etz_lin);
  else
    m_etz_log = 0.01;

  if (fabsf(m_met_lin)>epsilon)
    m_met_log = copysign(log10(fabsf(m_met_lin)), m_met_lin);
  else
    m_met_log = 0.01;
  if (fabsf(m_set_lin)>epsilon)
    m_set_log = copysign(log10(fabsf(m_set_lin)), m_set_lin);
  else
    m_set_log = 0.01;
  if (fabsf(m_se_lin)>epsilon)
    m_se_log  = copysign(log10(fabsf(m_se_lin)), m_se_lin);
  else
    m_se_log = 0.01;

  if((h = hist("HLT_MEx_lin")     ))  h->Fill(m_etx_lin);
  if((h = hist("HLT_MEy_lin")     ))  h->Fill(m_ety_lin);
  if((h = hist("HLT_MEz_lin")     ))  h->Fill(m_etz_lin);
  if((h = hist("HLT_ME_lin")      ))  h->Fill(m_me_lin);
  if((h = hist("HLT_MET_lin")     ))  h->Fill(m_met_lin);
  if((h = hist("HLT_MET_lin1")    ))  h->Fill(m_met_lin);
  if((h = hist("HLT_SumEt_lin")   ))  h->Fill(m_set_lin);
  if((h = hist("HLT_SumE_lin")    ))  h->Fill(m_se_lin);
  if((h = hist("HLT_MET_phi")     ))  h->Fill(m_phi);
  if((h = hist("HLT_MET_phi1")    ))  h->Fill(m_phi,m_met_lin);

  if((h = hist("HLT_MEx_log")     ))  h->Fill(m_etx_log);
  if((h = hist("HLT_MEy_log")     ))  h->Fill(m_ety_log);
  if((h = hist("HLT_MEz_log")     ))  h->Fill(m_etz_log);
  if((h = hist("HLT_ME_log")      ))  h->Fill(m_me_log);
  if((h = hist("HLT_MET_log")     ))  h->Fill(m_met_log);
  if((h = hist("HLT_SumEt_log")   ))  h->Fill(m_set_log);
  if((h = hist("HLT_SumE_log")    ))  h->Fill(m_se_log);

  // EF FEB
  if (m_hlt_met_feb_cont && m_hlt_met_feb_cont->size()>0)
    m_hlt_met = m_hlt_met_feb_cont->at(0);
  else {
    ATH_MSG_DEBUG("in fillHLTShiftHistos: HLT FEB container empty");
    return StatusCode::SUCCESS;
  }

  std::string HLTFEBmongroupName = m_shifter_path + "/HLT_FEB";
  setCurrentMonGroup(HLTFEBmongroupName.c_str());	

  m_ETx     = m_hlt_met->ex();
  m_ETy     = m_hlt_met->ey();

  // monitoring values (in CLHEP::GeV)
  m_etx_lin = m_ETx/CLHEP::GeV; 
  m_ety_lin = m_ETy/CLHEP::GeV; 

  if((h = hist("HLTFEB_MEx_lin")   ))   h->Fill(m_etx_lin);
  if((h = hist("HLTFEB_MEy_lin")   ))   h->Fill(m_ety_lin);

  // EF topocl
  if (m_hlt_met_topocl_cont && m_hlt_met_topocl_cont->size()>0)
    m_hlt_met = m_hlt_met_topocl_cont->at(0);
  else {
    ATH_MSG_DEBUG("in fillHLTShiftHistos: HLT topocl container empty");
    return StatusCode::SUCCESS;
  }

  std::string HLTtopoclmongroupName = m_shifter_path + "/HLT_topocl";
  setCurrentMonGroup(HLTtopoclmongroupName.c_str());

  m_ETx     = m_hlt_met->ex();
  m_ETy     = m_hlt_met->ey();

  m_etx_lin = m_ETx/CLHEP::GeV; 
  m_ety_lin = m_ETy/CLHEP::GeV; 

  if((h = hist("HLTtopocl_MEx_lin") ))   h->Fill(m_etx_lin);
  if((h = hist("HLTtopocl_MEy_lin") ))   h->Fill(m_ety_lin);
  
  float m_sqrt_set = sqrt(m_set_lin > epsilon ? m_set_lin : 0.);
  if((h2 = hist2("HLTMET_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_met_lin);
  if((h2 = hist2("HLTMEx_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_etx_lin);
  if((h2 = hist2("HLTMEy_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_ety_lin);
  //bool pass_zb = getTDT()->isPassed("L1_RD0_FILLED", TrigDefs::eventAccepted);
  // if(pass_zb) {
  //   if((h2 = hist2("EFMET_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_met_lin);
  //   if((h2 = hist2("EFMEx_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_etx_lin);
  //   if((h2 = hist2("EFMEy_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_ety_lin);
  // }


  //std::vector<int> m_status_flag(32,0);
  //TH1F *h1i(0);
  //  bool m_fill_stat = false;
  //  if((h1i = (TH1F *) hist("HLT_MET_status"))) m_fill_stat = true;
  //for (int i=0; i<32; ++i) {
  //    unsigned mask = (1<<i);
  //    if (m_hlt_met_cont->->getFlag() & mask) {
  //     if(m_fill_stat && h1i) h1i->Fill(i,1.);
  //     //m_status_flag[i] = 1;
  //   } else {
  //     if(m_fill_stat && h1i) h1i->Fill(i,0);
  //     //m_status_flag[i] = 0;
  //   }
  // }

  // fill correlation histograms -- L1MET vs EFMET
  // if (m_l1_roi_cont) {

  //   float sumet = (m_l1_roi_cont->energyT())/CLHEP::GeV;
  //   float ex = - (m_l1_roi_cont->energyX())/CLHEP::GeV;
  //   float ey = - (m_l1_roi_cont->energyY())/CLHEP::GeV;
  //   float et = sqrt(ex*ex + ey*ey);
  //   float phi = atan2f(ey, ex);
  //   float dphi = signed_delta_phi(phi, m_phi);

  //   if((h2 = hist2("L1MET_HLTMET_EtCor")))    h2->Fill(m_met_lin, et);
  //   if((h2 = hist2("L1MET_HLTMET_SumEtCor"))) h2->Fill(m_set_lin, sumet);
  //   if((h2 = hist2("L1MET_HLTMET_PhiCor")))   h2->Fill(m_phi, phi);

  //   if((h = hist("L1MET_HLTMET_dEt")))   h->Fill(et - m_met_lin);
  //   if((h = hist("L1MET_HLTMET_dPhi")))  h->Fill(dphi);
  //   if((h = hist("L1MET_HLTMET_dEx")))   h->Fill(ex - m_etx_lin);
  //   if((h = hist("L1MET_HLTMET_dEy")))   h->Fill(ey - m_ety_lin); 
  // }

  // unsigned int Nc = missETEF->getNumOfComponents();
  // unsigned int compNc = m_compNamesEF.size();
  // Nc = Nc > compNc ? compNc : Nc;

  // if (Nc > 0) {

  //   for (unsigned int i = 0; i < Nc; ++i) { // loop over components
  //     float ex =                missETEF->getExComponent(i)/CLHEP::GeV;
  //     float ey =                missETEF->getEyComponent(i)/CLHEP::GeV;
  //     float ez =                missETEF->getEzComponent(i)/CLHEP::GeV;
  //     float et =                sqrt(ex*ex+ey*ey);
  //     float sumE =              missETEF->getSumEComponent(i)/CLHEP::GeV;
  //     float sumEt =             missETEF->getSumEtComponent(i)/CLHEP::GeV;

  //     float m_comp_ex_log = -9e9;
  //     float m_comp_ey_log = -9e9;
  //     float m_comp_ez_log = -9e9;
  //     float m_comp_et_log = -9e9;
  //     float m_comp_sum_et_log = -9e9;
  //     float m_comp_sum_e_log = -9e9;
  //     float m_comp_sum_et_lin = -9e9;
  //     float m_comp_sum_e_lin = -9e9;
  //     float m_comp_et_lin = -9e9;

  //     if (fabsf(ex)>epsilon)
  //       m_comp_ex_log = copysign(log10f(fabsf(ex)), ex);
  //     else
  //       m_comp_ex_log = 0.01;

  //     if (fabsf(ey)>epsilon)
  //       m_comp_ey_log = copysign(log10f(fabsf(ey)), ey);
  //     else
  //       m_comp_ey_log = 0.01;

  //     if (fabsf(ez)>epsilon)
  //       m_comp_ez_log = copysign(log10f(fabsf(ez)), ez);
  //     else
  //       m_comp_ez_log = 0.01;

  //     m_comp_et_lin = et;
  //     if (et>1e-6) // i.e. et > 0
  //       m_comp_et_log = log10f(et);
  //     else
  //       m_comp_et_log = -100; // underflow

  //     m_comp_sum_et_lin = sumEt;
  //     m_comp_sum_e_lin = sumE;
      
  //     if (fabsf(sumEt)>epsilon)
  //       m_comp_sum_et_log = copysign(log10f(fabsf(sumEt)), sumEt);
  //     else
  //       m_comp_sum_et_log = 0.01;
  //     if (fabsf(sumE)>epsilon)
  //       m_comp_sum_e_log = copysign(log10f(fabsf(sumE)), sumE);
  //     else
  //       m_comp_sum_e_log = 0.01;

  //     if((h2 = hist2("compN_compEx")))      h2->Fill(i,m_comp_ex_log);
  //     if((h2 = hist2("compN_compEy")))      h2->Fill(i,m_comp_ey_log);
  //     if((h2 = hist2("compN_compEz")))      h2->Fill(i,m_comp_ez_log);
  //     if((h2 = hist2("compN_compEt")))      h2->Fill(i,m_comp_et_log);
  //     if((h2 = hist2("compN_compSumEt")))      h2->Fill(i,m_comp_sum_et_log);
  //     if((h2 = hist2("compN_compSumE")))      h2->Fill(i,m_comp_sum_e_log);
  //     if((h2 = hist2("compN_compEt_lin")))      h2->Fill(i,m_comp_et_lin);
  //     if((h2 = hist2("compN_compSumEt_lin")))      h2->Fill(i,m_comp_sum_et_lin);
      
  //     bool m_fill_stat = false;
  //     if((h2 = hist2("compN_EF_MET_status"))) m_fill_stat = true;
  //     // loop over status flags
  //     for (int j=0; j<32; ++j) {
  //       unsigned mask = (1<<j);
  //       if (missETEF->getStatus(i) & mask) {
  //         if(m_fill_stat && h2) h2->Fill(i,j);
  //       }
  //     }
     
  //     float nchan_used = fabsf(missETEF->getUsedChannels(i));
  //     if(nchan_used > epsilon) nchan_used = log10f(nchan_used);
  //     else nchan_used = -1.;
  //     if((h2 = hist2("compN_EF_usedChannels"))) h2->Fill(i,nchan_used);

  //     CLHEP::Hep3Vector v(ex,ey,ez);
  //     double eta = v.eta();
  //     double phi = v.phi();

  //     if((h2 = hist2(Form("compN_EtaPhi_%02d", i))))      h2->Fill(eta, phi);
  //     if((h2 = hist2(Form("compEt_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_et_lin);
  //     if((h2 = hist2(Form("compSumEt_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_sum_et_lin);
  //     if((h2 = hist2(Form("compSumE_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_sum_e_lin);

  //   } // loop over components
  // }

  return StatusCode::SUCCESS; 
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillOffShifterHistos() {

  if (m_debuglevel) ATH_MSG_DEBUG("HLTMETMonTool : in fillOffShiftHistos()");

  if (m_off_met_cont && m_off_met_cont->size()>0) {
    m_off_met = (*m_off_met_cont)["FinalClus"];
  }
  else {
    ATH_MSG_DEBUG("in fillOffShiftHistos: Offline container empty");
    return StatusCode::SUCCESS;
  }
  
  TH1 *h(0);
  TH2 *h2(0);

  std::string OffmongroupName = m_shifter_path + "/Off";
  setCurrentMonGroup(OffmongroupName.c_str());

  float ex_OffMET = (m_off_met->mpx())/CLHEP::GeV;
  float ey_OffMET = (m_off_met->mpy())/CLHEP::GeV;
  float et_OffMET = sqrt(ex_OffMET*ex_OffMET+ey_OffMET*ey_OffMET);
  float sumet_OffMET = (m_off_met->sumet())/CLHEP::GeV;
  float phi_OffMET = atan2(ey_OffMET, ex_OffMET); 

  if((h = hist("OffMET_et")))    h->Fill(et_OffMET);
  if((h = hist("OffMET_sumet"))) h->Fill(sumet_OffMET);
  if((h = hist("OffMET_phi")))   h->Fill(phi_OffMET);
  if((h = hist("Off_MEx_lin")))  h->Fill(ex_OffMET);
  if((h = hist("Off_MEy_lin")))  h->Fill(ey_OffMET);

  // fill correlation histograms -- L1MET vs RecMET
  // if (m_l1_roi_cont) {
    
  //   float sumet = (m_l1_roi_cont->energyT())/CLHEP::GeV;
  //   float ex = - (m_l1_roi_cont->energyX())/CLHEP::GeV;
  //   float ey = - (m_l1_roi_cont->energyY())/CLHEP::GeV;
  //   float et = sqrt(ex*ex + ey*ey);
  //   float phi = atan2f(ey, ex);
  //   float dphi = signed_delta_phi(phi, phi_OffMET);

  //   if((h2 = hist2("L1MET_OffMET_EtCor")))    h2->Fill(et_OffMET, et);
  //   if((h2 = hist2("L1MET_OffMET_SumEtCor"))) h2->Fill(sumet_OffMET, sumet);
  //   if((h2 = hist2("L1MET_OffMET_PhiCor")))   h2->Fill(phi_OffMET, phi);
    
  //   if((h = hist("L1MET_OffMET_dEt")))   h->Fill(et - et_OffMET);
  //   if((h = hist("L1MET_OffMET_dPhi")))   h->Fill(dphi);
  //   if((h = hist("L1MET_OffMET_dEx")))   h->Fill(ex - ex_OffMET);
  //   if((h = hist("L1MET_OffMET_dEy")))   h->Fill(ey - ey_OffMET); 
  // }
  
  // // fill correlation histograms -- EFMET vs RecMET
  // if (m_hlt_met) {
  //   float sumet = (m_hlt_met->sumEt())/CLHEP::GeV;
  //   float ex = (m_hlt_met->ex())/CLHEP::GeV;
  //   float ey = (m_hlt_met->ey())/CLHEP::GeV;
  //   float et = sqrt(ex*ex+ey*ey);
  //   float phi = atan2f(ey, ex);
  //   float dphi = signed_delta_phi(phi, phi_OffMET);

  //   if((h2 = hist2("HLTMET_OffMET_EtCor")))    h2->Fill(et_OffMET, et);
  //   if((h2 = hist2("HLTMET_OffMET_SumEtCor"))) h2->Fill(sumet_OffMET, sumet);
  //   if((h2 = hist2("HLTMET_OffMET_PhiCor")))   h2->Fill(phi_OffMET, phi);

  //   if((h = hist("HLTMET_OffMET_dEt")))   h->Fill(et - et_OffMET);
  //   if((h = hist("HLTMET_OffMET_dPhi")))  h->Fill(dphi);
  //   if((h = hist("HLTMET_OffMET_dEx")))   h->Fill(ex - ex_OffMET);
  //   if((h = hist("HLTMET_OffMET_dEy")))   h->Fill(ey - ey_OffMET);
  // }
  
  return StatusCode::SUCCESS; 
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillExpertHistos() {

  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HTLMETMonTool::fillExpertHistos()");

  StatusCode sc = StatusCode::SUCCESS;

  TH1 *h(0);
  TH2 *h2(0);

  // first store the values so that we don't need to retrieve/ealculate them again and again  
  double et_L1MET=0., sumet_L1MET=0., phi_L1MET=0., ex_L1MET=0., ey_L1MET=0.;
  double et_HLTMET=0., sumet_HLTMET=0., phi_HLTMET=0., ex_HLTMET=0., ey_HLTMET=0.;
  double et_OffMET=0., sumet_OffMET=0., phi_OffMET=0., ex_OffMET=0., ey_OffMET=0.;

  // L1 MET
  if (m_l1_roi_cont) {
    sumet_L1MET = (m_l1_roi_cont->energyT())/CLHEP::GeV;
    ex_L1MET = - (m_l1_roi_cont->energyX())/CLHEP::GeV;
    ey_L1MET = - (m_l1_roi_cont->energyY())/CLHEP::GeV;
    et_L1MET = sqrt(ex_L1MET*ex_L1MET + ey_L1MET*ey_L1MET);
    phi_L1MET = atan2(ey_L1MET, ex_L1MET);
  }
  
  // HLT MET
  if (m_hlt_met_cont && m_hlt_met_cont->size()) {  
    sumet_HLTMET = (m_hlt_met_cont->at(0)->sumEt())/CLHEP::GeV;
    ex_HLTMET = (m_hlt_met_cont->at(0)->ex())/CLHEP::GeV;
    ey_HLTMET = (m_hlt_met_cont->at(0)->ey())/CLHEP::GeV;
    et_HLTMET = sqrt(ex_HLTMET*ex_HLTMET+ey_HLTMET*ey_HLTMET);
    phi_HLTMET = atan2(ey_HLTMET, ex_HLTMET);
  }
  
  // Offline MET
  if (m_off_met_cont && m_off_met_cont->size()) {
    ex_OffMET = (m_off_met_cont->at(0)->mpx())/CLHEP::GeV;
    ey_OffMET = (m_off_met_cont->at(0)->mpy())/CLHEP::GeV;
    et_OffMET = sqrt(ex_OffMET*ex_OffMET+ey_OffMET*ey_OffMET);
    sumet_OffMET = (m_off_met_cont->at(0)->sumet())/CLHEP::GeV;
    phi_OffMET = atan2(ey_OffMET, ex_OffMET);
  }

  // now fill the histograms
  std::string expert_partial_path = m_expert_path + "/L1/";
      
  std::map<std::string,int>::const_iterator it;

  // fill expert histograms for L1 MET Triggers
  for (it = m_l1_met_signatures_tolook->begin(); it != m_l1_met_signatures_tolook->end(); it++) {
    std::string name = it->first;
    if(getTDT()->isPassed(name, TrigDefs::eventAccepted)) {

      setCurrentMonGroup(expert_partial_path+name);
      
      // L1 MET
      if (m_l1_roi_cont) {
        if((h = hist("L1_MET")))     h->Fill(et_L1MET);
        if((h = hist("L1_SumEt")))   h->Fill(sumet_L1MET);
        if((h = hist("L1_MET_phi"))) h->Fill(phi_L1MET);

        // fill correlation if((h = histograms -- L1MET vs OffMET
        if (m_off_met) {
          if((h2 = hist2("L1MET_OffMET_EtCor")))    h2->Fill(et_OffMET, et_L1MET);
          if((h2 = hist2("L1MET_OffMET_SumEtCor"))) h2->Fill(sumet_OffMET, sumet_L1MET);
          if((h2 = hist2("L1MET_OffMET_PhiCor")))   h2->Fill(phi_OffMET, phi_L1MET);
          if((h = hist("L1MET_OffMET_dEt")))        h->Fill(et_L1MET - et_OffMET);
          double dphi = signed_delta_phi(phi_L1MET, phi_OffMET);
          if((h = hist("L1MET_OffMET_dPhi")))   h->Fill(dphi);
          if((h = hist("L1MET_OffMET_dEx")))   h->Fill(ex_L1MET - ex_OffMET);
          if((h = hist("L1MET_OffMET_dEy")))   h->Fill(ey_L1MET - ey_OffMET); 
        }
      }
      
      // HLT MET
      if (m_hlt_met) {
        if((h = hist("HLT_MET_lin")   ))   h->Fill(et_HLTMET);
        if((h = hist("HLT_SumEt_lin") ))   h->Fill(sumet_HLTMET);
        if((h = hist("HLT_MET_phi")   ))   h->Fill(phi_HLTMET);
      }

      // Offline MET
      if (m_off_met) {
        if((h = hist("OffMET_et")))    h->Fill(et_OffMET);
        if((h = hist("OffMET_sumet"))) h->Fill(sumet_OffMET);
        if((h = hist("OffMET_phi")))   h->Fill(phi_OffMET);
      }
    }
  }
  
  // fill expert histograms for HLT
  expert_partial_path = m_expert_path + "/HLT/";

  for (it = m_hlt_met_signatures_tolook->begin(); it != m_hlt_met_signatures_tolook->end(); it++) {
    
    std::string name = it->first;
    if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
      setCurrentMonGroup(expert_partial_path+name);
    
      // HLT MET
      if (m_hlt_met) {
        if((h = hist("HLT_MET_lin")      ))   h->Fill(et_HLTMET);
        if((h = hist("HLT_SumEt_lin")    ))   h->Fill(sumet_HLTMET);
        if((h = hist("HLT_MET_phi")      ))   h->Fill(phi_HLTMET);
        
        // correlation histograms -- L1 vs HLT
        if (m_l1_roi_cont) {
          if((h2 = hist2("L1MET_HLTMET_EtCor")))     h2->Fill(et_HLTMET, et_L1MET);
          if((h2 = hist2("L1MET_HLTMET_SumEtCor")))  h2->Fill(sumet_HLTMET, sumet_L1MET);
          if((h = hist("L1MET_HLTMET_dEt")))   h->Fill(et_L1MET - et_HLTMET);
          
          double dphi = signed_delta_phi(phi_L1MET, phi_HLTMET);
          if((h = hist("L1MET_HLTMET_dPhi")))   h->Fill(dphi);
          if((h = hist("L1MET_HLTMET_dEx")))   h->Fill(ex_L1MET - ex_HLTMET);
          if((h = hist("L1MET_HLTMET_dEy")))   h->Fill(ey_L1MET - ey_HLTMET); 
        }
        
        // fill correlation histograms -- HLTMET vs OffMET
        if (m_off_met) {
          if((h2 = hist2("HLTMET_OffMET_EtCor")))    h2->Fill(et_OffMET, et_HLTMET);
          if((h2 = hist2("HLTMET_OffMET_SumEtCor"))) h2->Fill(sumet_OffMET, sumet_HLTMET);
          if((h2 = hist2("HLTMET_OffMET_PhiCor")))   h2->Fill(phi_OffMET, phi_HLTMET);
          if((h = hist("HLTMET_OffMET_dEt")))        h->Fill(et_HLTMET - et_OffMET);
          double dphi = signed_delta_phi(phi_HLTMET, phi_OffMET);
          if((h = hist("HLTMET_OffMET_dPhi")))  h->Fill(dphi);
          if((h = hist("HLTMET_OffMET_dEx")))   h->Fill(ex_HLTMET - ex_OffMET);
          if((h = hist("HLTMET_OffMET_dEy")))   h->Fill(ey_HLTMET - ey_OffMET);
        }
      }
      // Offline MET
      if (m_off_met) {
        if((h = hist("OffMET_et")))    h->Fill(et_OffMET);
        if((h = hist("OffMET_sumet"))) h->Fill(sumet_OffMET);
        if((h = hist("OffMET_phi")))   h->Fill(phi_OffMET);
      }
    } 
  }
  
  // fill efficiency histograms
  if (!m_is_do_trigger_effic) return sc;
  
  if (m_debuglevel) 
    ATH_MSG_DEBUG("filling efficiency histograms()");
  
  if (!m_off_met) return sc;
  
  expert_partial_path = m_expert_path + "/Efficiency/";

  // std::map<std::string,int>::const_iterator it_s;
  std::vector<std::map<std::string,int>*> m_met_signatures_tolook;
  m_met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
  m_met_signatures_tolook.push_back(m_hlt_met_signatures_tolook);
  unsigned int size = m_met_signatures_tolook.size();
  
  // for (it_s = m_sample_selection_signatures.begin(); it_s != m_sample_selection_signatures.end(); it_s++) {
  //   if (getTDT()->isPassed(it_s->first, TrigDefs::eventAccepted)) {
  //     std::string tmp_MonGroup = expertpartialMonGroup + it_s->first;
  //     std::string tmp_partialMonGroup = tmp_MonGroup + "/";
  //     setCurrentMonGroup(tmp_MonGroup);
  //     if((h = hist("RecMET_et")))   h->Fill(et_RecMET);
  //     if((h = hist("RecMET_sumet")))   h->Fill(sumet_RecMET);
  //     if((h = hist("RecMET_phi")))   h->Fill(phi_RecMET);

  for (unsigned int i=0; i<size; i++) {
    for (it = m_met_signatures_tolook[i]->begin(); it != m_met_signatures_tolook[i]->end(); it++) {
      std::string name = it->first;
      if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
        setCurrentMonGroup(expert_partial_path + name);
        if((h = hist("OffMET_et")))    h->Fill(et_OffMET);
        if((h = hist("OffMET_sumet"))) h->Fill(sumet_OffMET);
        if((h = hist("OffMET_phi")))   h->Fill(phi_OffMET);
      }
    }
  }

  return sc;
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::getAllMETTriggers() {

  // clear the list from previous run
  m_all_l1_met_triggers.clear();
  m_all_hlt_met_triggers.clear();

  const std::vector<std::string> TrigL1Items = getTDT()->getListOfTriggers("L1_.*");
  std::vector<std::string>::const_iterator iter;

  for (iter = TrigL1Items.begin(); iter != TrigL1Items.end(); ++iter) {
    std::string L1name = *iter;
    std::string tmp_trigType = L1name.substr(0,5);
    *m_log << MSG::INFO  << "Trigger Item " << L1name  << " defined " << endreq;
    // store L1 MET trigger item names
    if (tmp_trigType=="L1_TE" || tmp_trigType=="L1_XE")
      m_all_l1_met_triggers.insert(std::map<std::string,int>::value_type(L1name,0));
  }

  const std::vector<std::string> TrigHLTItems = getTDT()->getListOfTriggers("HLT_.*");
  std::vector<std::string>::const_iterator chain;

  for (chain= TrigHLTItems.begin(); chain!= TrigHLTItems.end(); ++chain) {
    std::string HLTname = *chain;
    //float prescale = (*chain)->prescale();
    float prescale = getTDT()->getPrescale(*chain);
    
    // store HLT MET trigger item names divided in L2 and EF
    std::string tmp_trigType =  HLTname.substr(0,5);
    ATH_MSG_INFO("Trigger Item " << HLTname  << " defined with prescale " << prescale << " , " << tmp_trigType);
    m_all_hlt_met_triggers.insert(std::map<std::string,int>::value_type(HLTname,0));
  }
}

//___________________________________________________________________________________________________________
std::string HLTMETMonTool::get_trigger_level(std::string item)
{
  size_t n = item.find_first_of('_');
  return item.substr(0, n);
}

void HLTMETMonTool::checkTriggers(std::vector<std::string>& m_triggers, std::map<std::string,int> &m_signatures) {

  const std::vector<std::string> TrigL1Items = getTDT()->getListOfTriggers("L1_.*");
  std::vector<std::string>::const_iterator iter;
 
  const std::vector<std::string> TrigEFItems = getTDT()->getListOfTriggers("EF_.*");
  const std::vector<std::string> TrigHLTItems = getTDT()->getListOfTriggers("HLT_.*");

  std::vector<std::string>::const_iterator chain;

  for (unsigned int it = 0; it < m_triggers.size(); ++it) { // Loop over trigger items
    std::string item = m_triggers[it];
    std::string tmp_trigLevel = get_trigger_level(item); //item.substr(0,3);

    if (m_debuglevel)
      ATH_MSG_DEBUG("signature " << item << " requested");

    // check your trigger is defined in the menu
    bool triggerFound = false;
    if (tmp_trigLevel == "L1") {
      for (iter=TrigL1Items.begin(); iter!=TrigL1Items.end(); ++iter) {
        std::string L1name = *iter;
        if (L1name == item) {
          triggerFound = true;
          m_signatures.insert(std::map<std::string,int>::value_type(item, 0));
        }
      }
      if (!triggerFound) {
        ATH_MSG_DEBUG("L1 signature " << item << " requested but not found in the menu");
      }
    } 
    else if (tmp_trigLevel == "EF") {
      for (chain=TrigEFItems.begin(); chain!=TrigEFItems.end(); ++chain) {
        std::string EFname = *chain;
        if (EFname == item) {
          triggerFound = true;
          m_signatures.insert(std::map<std::string,int>::value_type(item, 0));
        }
      }
      if (!triggerFound)
        ATH_MSG_DEBUG("HLT signature " << item << " requested but not found in the menu");
    }
    else if (tmp_trigLevel == "HLT") {
      for (chain=TrigHLTItems.begin(); chain!=TrigHLTItems.end(); ++chain) {
        std::string HLTname = *chain;
        if (HLTname == item) {
          triggerFound = true;
          m_signatures.insert(std::map<std::string,int>::value_type(item, 0));
        }
      }
      if (!triggerFound)
        ATH_MSG_DEBUG("HLT signature " << item << " requested but not found in the menu");
    }
  }

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::printMETTriggerStats() {

  // L1
  ATH_MSG_INFO("Level-1 Met Triggers to look: " << m_l1_met_signatures_tolook->size());
  std::map<std::string,int>::iterator iter;
  for (iter = m_l1_met_signatures_tolook->begin(); iter!= m_l1_met_signatures_tolook->end(); ++iter) {
    ATH_MSG_INFO("Events passing " << iter->first << ": " << iter->second);
  }

  // EF
  ATH_MSG_INFO("HLT Met Triggers to look: "  << m_hlt_met_signatures_tolook->size());
  for (iter = m_hlt_met_signatures_tolook->begin(); iter != m_hlt_met_signatures_tolook->end(); ++iter) {
    ATH_MSG_INFO("Events passing " << iter->first << ": " << iter->second);
  }
}


//___________________________________________________________________________________________________________
double HLTMETMonTool::signed_delta_phi(double phi1, double phi2) {

  double phia = phi1;
  double phib = phi2;
  if (phi1>M_PI) phia = phi1 - 2* M_PI;
  if (phi2>M_PI) phib = phi2 - 2*M_PI;

  double dphi = phia - phib;
  if (dphi>M_PI) dphi -= 2*M_PI;
  if (dphi<(-1.*M_PI)) dphi += 2*M_PI;
  return dphi;
}
