/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRAWDATAMONITORING_TGCRAWDATAMONITORALGORITHM_H
#define TGCRAWDATAMONITORING_TGCRAWDATAMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include <memory>
#include <vector>
#include <set>

class TgcRawDataMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TgcRawDataMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TgcRawDataMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  
  struct MyMuon{
    const xAOD::Muon* muon{};
    std::vector<double> extPosZ;
    std::vector<TVector3> extPos;
    std::vector<TVector3> extVec;
    std::set<int> matchedL1ThrExclusive;
    std::set<int> matchedL1ThrInclusive;
    bool matchedL1Charge{};
    bool passBW3Coin{};
    bool passInnerCoin{};
    bool passGoodMF{};
    bool passIsMoreCandInRoI{};
    void clear(){
      extPosZ.clear();
      extPos.clear();
      extVec.clear();
      matchedL1ThrExclusive.clear();
      matchedL1ThrInclusive.clear();
    }
  };
  struct TgcHit{
    float x{};
    float y{};
    float z{};
    float shortWidth{};
    float longWidth{};
    float length{};
    int isStrip{};
    int gasGap{};
    int channel{};
    int eta{};
    int phi{};
    int station{};
    int bunch{};
    int sector{};
    int f{};
    int E{};
    int M{};
    int iphi{};
    int ieta{};
    int L{};
    TString name;
    int istation{};
    int igasGap{};
    int iside{};
    TString side;
  };
  struct TgcTrig{
    int lb{};
    float x_In{};
    float y_In{};
    float z_In{};
    float x_Out{};
    float y_Out{};
    float z_Out{};
    float eta{};
    float phi{};
    float etain{};
    float etaout{};
    float width_In{};
    float width_Out{};
    float width_R{};
    float width_Phi{};
    int isAside{};
    int isForward{};
    int isStrip{};
    int isInner{};
    int isPositiveDeltaR{};
    int type{};
    int trackletId{};
    int trackletIdStrip{};
    int sector{};
    int roi{};
    int pt{};
    int delta{};
    int sub{};
    int veto{};
    int bunch{};
    int inner{};
  };
  struct CtpDecMonObj{
    TString trigItem;
    TString title;
    long unsigned int multiplicity{};
    int rpcThr{};
    int tgcThr{};
    int sys{};//system: 1 for barrel, 2 for endcap, 3 for forward
    int threshold{};
    int charge{};
    bool tgcF{}; // full-station flag
    bool tgcC{}; // inner-coincidence flag
    bool tgcH{}; // hot roi mask flag
    bool rpcR{}; // masking feet trigger
    bool rpcM{}; // isMoreCand
    double eta{};
    double phi{};
    unsigned int roiWord{};
  };
  
 private:
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey{this,"MuonContainerName","Muons","Offline muon track container"};
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_MuonRoIContainerKey{this,"MuonRoIContainerName","LVL1MuonRoIs","L1 muon RoI container"};
  
  SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_TgcPrepDataContainerKey{this,"TgcPrepDataContainerName","TGC_MeasurementsAllBCs","current BC TGC PRD"};
  SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_TgcCoinDataContainerCurrBCKey{this,"TgcCoinDataContainerCurrBCName","TrigT1CoinDataCollection","TGC Coin Data Container CurrBC"};
  SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_TgcCoinDataContainerNextBCKey{this,"TgcCoinDataContainerNextBCName","TrigT1CoinDataCollectionNextBC","TGC Coin Data Container NextBC"};
  SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_TgcCoinDataContainerPrevBCKey{this,"TgcCoinDataContainerPrevBCName","TrigT1CoinDataCollectionPriorBC","TGC Coin Data Container PrevBC"};
  
  StringProperty m_packageName{this,"PackageName","TgcRawDataMonitor","group name for histograming"};
  StringProperty m_ctpDecMonList{this,"CtpDecisionMoniorList","Tit:L1_2MU4,Mul:2,HLT:HLT_2mu4,RPC:1,TGC:1;","list of L1MU items to be monitored for before/after CTP decision"};
  BooleanProperty m_monitorTriggerMultiplicity{this,"MonitorTriggerMultiplicity",false,"start monitoring tirgger multiplicity performance"};
  BooleanProperty m_printAvailableMuonTriggers{this,"PrintAvailableMuonTriggers",false,"debugging purpose. print out all available muon triggers in the event"};
  BooleanProperty m_useNonMuonTriggers{this,"UseNonMuonTriggers",true,"muon-orthogonal triggers for muon-unbiased measurement"};
  BooleanProperty m_TagAndProbe{this,"TagAndProbe",true,"switch to perform tag-and-probe method"};
  BooleanProperty m_TagAndProbeZmumu{this,"TagAndProbeZmumu",false,"switch to perform tag-and-probe method Z->mumu"};
  BooleanProperty m_anaTgcPrd{this,"AnaTgcPrd",false,"switch to perform analysis on TGC PRD/Coin"};
  BooleanProperty m_anaOfflMuon{this,"AnaOfflMuon",true,"switch to perform analysis on xAOD::Muon"};
  BooleanProperty m_anaMuonRoI{this,"AnaMuonRoI",true,"switch to perform analysis on xAOD::LVL1MuonRoI"};
  DoubleProperty m_trigMatchWindow{this,"TrigMatchingWindow",0.005,"Window size in R for trigger matching"};
  DoubleProperty m_l1trigMatchWindow1{this,"L1TrigMatchingWindow1",0.15,"Window size in R for L1 trigger matching: param 1"};
  DoubleProperty m_l1trigMatchWindow2{this,"L1TrigMatchingWindow2",0.3,"Window size in R for L1 trigger matching: param 2"};
  DoubleProperty m_l1trigMatchWindow3{this,"L1TrigMatchingWindow3",-0.01,"Window size in R for L1 trigger matching: param 3"};
  DoubleProperty m_l1trigMatchWindow4{this,"L1TrigMatchingWindow4",0.36,"Window size in R for L1 trigger matching: param 4"};
  DoubleProperty m_l1trigMatchWindow5{this,"L1TrigMatchingWindow5",-0.0016,"Window size in R for L1 trigger matching: param 5"};
  DoubleProperty m_isolationWindow{this,"IsolationWindow",0.1,"Window size in R for isolation with other muons"};
  BooleanProperty m_requireIsolated{this,"RequireIsolated",true,"Probe muon should be isolated from other muons"};
  DoubleProperty m_M1_Z{this,"M1_Z",13605.0,"z-position of TGC M1-station in mm for track extrapolate"};
  DoubleProperty m_M2_Z{this,"M2_Z",14860.0,"z-position of TGC M2-station in mm for track extrapolate"};
  DoubleProperty m_M3_Z{this,"M3_Z",15280.0,"z-position of TGC M3-station in mm for track extrapolate"};
  DoubleProperty m_EI_Z{this,"EI_Z",7425.0,"z-position of TGC EI-station in mm for track extrapolate"};
  DoubleProperty m_FI_Z{this,"FI_Z",7030.0,"z-position of TGC FI-station in mm for track extrapolate"};
  DoubleProperty m_muonMass{this,"MuonMass",105.6583755,"muon invariant mass in MeV"};
  DoubleProperty m_zMass{this,"ZMass",91187.6,"muon invariant mass in MeV"};
  DoubleProperty m_zMassWindow{this,"ZMassWindow",10000,"muon invariant mass half-window in MeV"};
  DoubleProperty m_endcapPivotPlaneMinimumRadius{this,"endcapPivotPlaneMinimumRadius",0.,"minimum radius of pivot plane in endcap region"};
  DoubleProperty m_endcapPivotPlaneMaximumRadius{this,"endcapPivotPlaneMaximumRadius", 11977.,"maximum radius of pivot plane in endcap region"};
  DoubleProperty m_barrelPivotPlaneHalfLength{this,"barrelPivotPlaneHalfLength", 9500.,"half length of pivot plane in barrel region"};
  
  std::vector<double> m_extZposition;
  std::vector<CtpDecMonObj> m_CtpDecMonObj;

  using MonVariables=std::vector < std::reference_wrapper < Monitored::IMonitoredVariable >>;
  void fillTgcCoin(const std::vector<TgcTrig>&, const std::string& ) const;

  /* track extrapolator tool */
  enum TargetDetector { UNDEF, TGC, RPC };
  void extrapolate(const xAOD::Muon*, MyMuon&) const;
  bool extrapolate(const xAOD::TrackParticle* trackParticle,
		   const Amg::Vector3D& pos,
		   const int detector,
		   Amg::Vector2D& eta,
		   Amg::Vector2D& phi,
		   Amg::Vector3D& mom) const;
  std::unique_ptr<const Trk::TrackParameters>
    extrapolateToTGC(const Trk::TrackStateOnSurface* tsos,
		     const Amg::Vector3D& pos,
		     Amg::Vector2D& distance) const;
  std::unique_ptr<const Trk::TrackParameters>  
    extrapolateToRPC(const Trk::TrackStateOnSurface* tsos,
		     const Amg::Vector3D& pos,
		     Amg::Vector2D& distance) const;
  double getError(const std::vector<double>& inputVec) const;
  ToolHandle<Trk::IExtrapolator> m_extrapolator{this,"TrackExtrapolator","Trk::Extrapolator/AtlasExtrapolator","Track extrapolator"};
};
#endif
