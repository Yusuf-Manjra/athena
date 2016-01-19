/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTMuonMonTool.h
 *   Class declaration for HLTMuonMonTool
 *   
 *                          
 *                          
 *    @author name (uid@cern.ch)   
 * 
 *   
 */

#ifndef HLTMUONMONTOOL_H
#define HLTMUONMONTOOL_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include <string>
#include <vector>

#include <TH1.h>
#include <TH2.h>
#include <TRandom3.h>
#include "TMath.h"

/* namespace Rec
{
  class IMuonCombinedSelectorTool;
  } */

///Concrete monitoring tool derived from MonitorToolBase
class HLTMuonMonTool : public IHLTMonTool
{
  
 public:
  HLTMuonMonTool(   const std::string & type, 
		    const std::string & name,
		    const IInterface* parent); 
  virtual ~HLTMuonMonTool();
  
  StatusCode init();
  StatusCode book();
  StatusCode fill();
  StatusCode proc();

  // YY added for ES bits
  std::vector<std::string> getESbits();
  bool isPassedES(std::vector<std::string>&, std::string&);

  //==public functions for muFast

  //==public functions for muComb

  //==public functions for muIso

  //==public functions for TileMu

  //==public functions for MuonEF

  //==public functions for MuGirl

  //==public functions for MuZTP

  enum ptThresEtaRegion { BRL=0, EC1=1, EC2=2, EC3=3 };

 private:
  //== common
  ActiveStoreSvc* m_activeStore;
  Trig::ExpertMethods* m_ExpertMethods;

  //init() for each HLT Algorithm
  StatusCode initMuFastDQA();
  StatusCode initMuCombDQA();
  StatusCode initMuIsoDQA();
  StatusCode initTileMuDQA();
  StatusCode initMuonEFDQA();
  StatusCode initMuGirlDQA();
  StatusCode initMuZTPDQA();
  
  //book() for each HLT Algorithm
  std::string histdir;
  std::string histdirmufast;
  std::string histdirmucomb;
  std::string histdirmuiso;
  std::string histdirtilemu;
  std::string histdirmuonef;
  std::string histdirmugirl;
  std::string histdirmuztp;
  std::string histdireff;
  std::string histdireffnumdenom;
  std::string histdirrate;
  std::string histdirrateratio;
  std::string histdircoverage;
  std::string histdirdist2d;

  StatusCode bookCommonDQA();
  StatusCode bookChainDQA();
  StatusCode bookChainDQA_MSonly(const std::string& chainName );
  StatusCode bookChainDQA_standard(const std::string& chainName );
  StatusCode bookChainDQA_generic(const std::string& chainName, bool isdIsoOffline);
  StatusCode bookMuFastDQA();
  StatusCode bookMuCombDQA();
  StatusCode bookMuIsoDQA();
  StatusCode bookTileMuDQA();
  StatusCode bookMuonEFDQA();
  StatusCode bookMuGirlDQA();
  StatusCode bookMuZTPDQA();

  //fill() for each HLT Algorithm
  StatusCode fillCommonDQA();
  StatusCode fillChainDQA();
  StatusCode fillChainDQA_MSonly(const std::string& chainName, const std::string & histcName);
  StatusCode fillChainDQA_standard(const std::string& chainName, const std::string & histcName);
  StatusCode fillChainDQA_generic(const std::string& chainName, const std::string & histcName,  bool isIsolOffline);
  StatusCode fillMuFastDQA();
  StatusCode fillMuCombDQA();
  StatusCode fillMuIsoDQA();
  StatusCode fillTileMuDQA();
  StatusCode fillMuonEFDQA();
  StatusCode fillMuGirlDQA();
  StatusCode fillMuZTPDQA();
  StatusCode fillMuZTPDQA_wrapper();

  //proc() for each HLT Algorithm
  StatusCode procChainDQA();
  StatusCode procChainDQA_MSonly( const std::string& chainName );
  StatusCode procChainDQA_standard( const std::string& chainName );
  StatusCode procChainDQA_generic( const std::string& chainName );
  StatusCode procChainDQA_HighPt();
  StatusCode procMuFastDQA();
  StatusCode procMuCombDQA();
  StatusCode procMuIsoDQA();
  StatusCode procTileMuDQA();
  StatusCode procMuonEFDQA();
  StatusCode procMuGirlDQA();
  StatusCode procMuZTPDQA();
  
  // private functions
  float getPtThresTrigMuonEFSA(const int thres, const float eta);
  float getPtThresTrigMuonEFCB(const int thres, const float eta);
  HLTMuonMonTool::ptThresEtaRegion getEtaRegion(const float eta);

  //private parameters/functions for common

  static const int EVENT=0;
  static const int MUFAST=1;
  static const int MUFASTFOUND=2;
  static const int MUFASTDFOUND=3;
  static const int MUCOMB=4;
  static const int MUCOMBFOUND=5;
  static const int MUISO=6;
  static const int MUISOFOUND=7;
  static const int TILEMU=8;
  static const int TILEMUFOUND=9;
  static const int TILEMUTRACKFOUND=10;
  static const int MUONEF=11;
  static const int MUONEFMSFOUND=12;
  static const int MUONEFSAFOUND=13;
  static const int MUONEFCBFOUND=14;
  static const int MUZTP=15;//added by marx


  static const int OFLFOUND=20;//any offline muon
  static const int OFLMSFOUND=21;//MS
  static const int OFLSAFOUND=22;//Standalone
  static const int OFLCBFOUND=23;//CB muon
  static const int OFLCBSELFOUND=24;//CB muon with selection (|eta|<2.4, pt>4.0GeV)
  static const int OFLLPTFOUND=25;//LPT

  int m_lumiblock;
  int m_event;

  bool   m_HI_PP_Key;
  //flags for trigger decision
  std::vector<std::string> m_chainsRate;

  std::vector<std::string> m_chainsMSonly;
  std::vector<std::string> m_chainsStandard;
  std::vector<std::string> m_chainsMG;  // added YY 2 Oct 2010
  std::vector<std::string> m_chainsMI;  // added tomoe
  std::vector<std::string> m_chainsGeneric;  // added YY 21 Feb 2012
  std::vector<std::string> m_chainsEFFS;  
  std::vector<std::string> m_histChainGeneric;  
  std::vector<std::string> m_histChainEFiso;  
  std::vector<std::string> m_histChainMSonly;  
  std::vector<std::string> m_histChainEFFS ;  
  std::vector<std::string> m_chainsEFiso;  
  std::vector<std::string> m_chainsL2Standard;
  std::vector<std::string> m_chainsL2passHLT;
  std::map<std::string, std::string> m_ztpmap; // added marx
  std::map<std::string, int> m_ztp_isomap; // added MO 28 March 2012
  std::map<std::string, std::string> m_ztp_l2map; // added MO 28 March 2012
  std::map<std::string, std::string> m_ztp_l1map; // added MO 28 March 2012
  std::map<std::string, double> m_ztpptcut; // added marx
  std::map<std::string, std::string> m_lowerChains;
  std::map<std::string, int>  m_thresh;
  std::map<std::string, std::string> m_chainsEF_L2_map;  // add by Yuan

  // YY ES chain name - obsolete 21/01/11
  std::vector<std::string> m_ESchain;
  std::string m_ESchainName;
  std::string m_FS_pre_trigger;
  std::string m_FS_pre_trigger_second;

  // YY MSonly_based monitoring - added 26.06.2011
  std::string m_MSchainName;
  std::vector<std::string> m_MSchain;
  std::string m_hptName;
  std::string m_EFAlgName[4];
  // =======
  std::vector<std::string> m_chainsOverlap;

  // YY ES chain information treated like independent trigger // modifiled for HI 15.10.2011
  enum indexES { ESSTD, ESTAG, ESID, ESINDEP, ESHIL1, ESHIID, ESHIINDEP }; // adding ESINDEP 20.1.2012
  std::string m_triggerES[ESHIINDEP + 1];
  bool m_passedES[ESHIINDEP + 1]; // std, tag, id
  bool CB_mon_ESbr[ESHIINDEP + 1];
  bool MS_mon_ESbr[ESHIINDEP + 1];

  //AI 20100824
  std::vector<std::string> m_allESchain;

  //trigger chains to calculate rate in events taken by independent triggers
  std::vector<std::string> m_chains2;
  enum indexINDEP { INDORTH, INDEGAMMA, INDMET, INDJET, INDTAU, INDMBIAS };
  std::string m_trigger[INDMBIAS + 1];
  bool m_passed[INDMBIAS + 1];//all, egamma, tau, jet, met, minbias independent
  int m_maxindep;
  int m_maxESbr;

  // add by Yuan
  TRandom3   m_random3;

  //private parameters/functions for muFast
  double calc_dR(double,double,double,double);    
  double calc_dphi(double,double);
  bool checkOfflineSAdR();
  bool checkOfflineCBdR();

  void refill_2d1d_coarse(TH2F *, TH1F *);
  
  StatusCode fillRecMuon();
  void fillTriggerOverlap();
  void procTriggerOverlap();

  StatusCode fillL1MuRoI();
  
  std::vector<double> m_RecMuonCB_pt;
  std::vector<double> m_RecMuonCB_eta;
  std::vector<double> m_RecMuonCB_phi;
  std::vector<double> m_RecMuonCB_charge;
  std::vector<bool>   m_RecMuonCB_isGoodCB; // YY added

  std::vector<double> m_RecMuonSA_pt;
  std::vector<double> m_RecMuonSA_eta;
  std::vector<double> m_RecMuonSA_phi;
  std::vector<double> m_RecMuonSA_d0;
  std::vector<double> m_RecMuonSA_z0;
  std::vector<double> m_RecMuonSA_charge;
  std::vector<bool>   m_RecMuonSA_hasCB; // YY added: for removing nonCombined tracks for SA efficiency
  std::vector<bool>   m_RecMuonSA_isGoodCB; // YY added

  //private parameters/functions for muComb

  //private parameters/functions for muIso
  std::vector<double> m_RecMuonCB_eloss;
  std::vector<double> m_RecMuonCB_etcone;
  std::vector<double> m_RecMuonCB_ptcone;
  std::vector<double> m_RecMuonCB_etcone30;
  std::vector<double> m_RecMuonCB_ptcone30;
  

  //private parameters/functions for TileMu

  std::vector<float> eta_RecCB;
  std::vector<float> phi_RecCB;
  std::vector<float> pt_RecCB;

  std::vector<float> eta_Tile;
  std::vector<float> phi_Tile;

  std::vector<float> eta_TileTrack;
  std::vector<float> phi_TileTrack;
  std::vector<float> pt_TileTrack;

  //private parameters/functions for MuonEF
  std::vector<double> pt_EFMSmuon;
  std::vector<double> eta_EFMSmuon;
  std::vector<double> phi_EFMSmuon;
  std::vector<double> pt_EFSAmuon;
  std::vector<double> eta_EFSAmuon;
  std::vector<double> phi_EFSAmuon;
  std::vector<double> pt_EFCBmuon;
  std::vector<double> eta_EFCBmuon;
  std::vector<double> phi_EFCBmuon;

  std::vector<int> index_EFMS_RecoMS_corr;
  std::vector<int> index_EFSA_RecoSA_corr;
  std::vector<int> index_EFCB_RecoCB_corr;

  // private parameters/functions for MuonEF efficiencies studies wrt LVL2 MuFAST
  std::vector<float> pt_LVL2FASTmuon;
  std::vector<float> eta_LVL2FASTmuon;
  std::vector<float> phi_LVL2FASTmuon;

  std::vector<float> pt_lvl2_RecoSA_corr;
  std::vector<float> eta_lvl2_RecoSA_corr;
  std::vector<float> phi_lvl2_RecoSA_corr;
  std::vector<int> index_lvl2_RecoSA_corr;

  std::vector<int> index_EFSA_RecoSA_andL2_corr;

  // private parameters/functions for MuonEF efficiencies studies wrt LVL2 MuComb
  std::vector<float> pt_LVL2COMBmuon;
  std::vector<float> eta_LVL2COMBmuon;
  std::vector<float> phi_LVL2COMBmuon;

  std::vector<float> pt_lvl2_RecoCB_corr;
  std::vector<float> eta_lvl2_RecoCB_corr;
  std::vector<float> phi_lvl2_RecoCB_corr;
  std::vector<int> index_lvl2_RecoCB_corr;
  std::vector<int> index_EFCB_RecoCB_andL2_corr;

  // Vectors of Reconstructed Variables
  std::vector<double> pt_RecMSmuon;
  std::vector<double> eta_RecMSmuon;
  std::vector<double> phi_RecMSmuon;
  std::vector<double> charge_RecMSmuon;

  std::vector<double> pt_RecSAmuon;
  std::vector<double> eta_RecSAmuon;
  std::vector<double> phi_RecSAmuon;
  std::vector<double> charge_RecSAmuon;

  std::vector<double> pt_RecCBmuon;
  std::vector<double> eta_RecCBmuon;
  std::vector<double> phi_RecCBmuon;
  std::vector<double> charge_RecCBmuon;

  //private parameters/functions for MuGirl

  //histogram vectors
  std::vector<TH1F *> m_HistoVectorMuonmuFast;
  std::vector<TH1F *> m_HistoVectorMuonmuComb;
  std::vector<TH1F *> m_HistoVectorMuonTileMu;
  std::vector<TH1F *> m_HistoVectorMuonmuIso;
  std::vector<TH1F *> m_HistoVectorMuonEFMS;
  std::vector<TH1F *> m_HistoVectorMuonEFSA;
  std::vector<TH1F *> m_HistoVectorMuonEFCB;
  
  /** muons cuts */
  double m_deltaRCut;

  // YY added: for ES bits
  std::string m_nullstr;
  std::vector<std::string> m_esvect;
  bool m_requestESchains;

  ToolHandle< Trig::IBunchCrossingTool > m_bunchTool;

  // YY added: for MuonCominedSelectorTool
  //  ToolHandle<Rec::IMuonCombinedSelectorTool> m_muonSelectorTool;  // removed for the moment

  // YY added:
  enum ieffStdAlgo { iMuFast = 0, iMuComb = 1, iEFCB = 2, iMuGirl = 3 };
  enum ieffMSAlgo { iEFSA = 1 };

  float fMuFast, fMuComb, fEFCB, fMuGirl;
  float fEFSA;

  int iSTDL, iSTDH;
  int iMSL, iMSH;

  std::vector<std::string> m_vectkwd;

  // variable to know when we should load the trigger info
  bool m_ztp_newrun;
  // isolation cut for ztp
  double m_ztp_ptcone30rel_cut;
  double m_ztp_EF_ptcone30rel_cut;
  //2d histos parameter

  static const int eta_cnbins = 26;
  Double_t bins_eta[eta_cnbins+1];
  //  = {-2.40, -1.918, -1.623, -1.348, -1.2329, -1.1479, -1.05,
  //				    -0.908, -0.791, -0.652, -0.476, -0.324, -0.132, 0,
  //				    0.132, 0.324, 0.476, 0.652, 0.791, 0.908, 1.05,
  //				    1.1479, 1.2329, 1.348, 1.623, 1.1918, 2.40};
  static const int phi_cnbins = 16;
  Double_t bins_phi[phi_cnbins+1];
  //  for (int iphi = 0; iphi <= phi_nbins; iphi++) {
  //    bins_eta[iphi] = -PI/8.*(Double_t)iphi;
  //  }
  
};


#endif
