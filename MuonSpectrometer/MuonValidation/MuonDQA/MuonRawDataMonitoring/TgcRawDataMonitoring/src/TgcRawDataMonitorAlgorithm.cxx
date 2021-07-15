/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRawDataMonitorAlgorithm.h"

#include "TObjArray.h"
#include "FourMomUtils/xAODP4Helpers.h"

namespace {
  // Cut values on pt bein exploited throughout the monitoring
  constexpr float pt_30_cut = 30. * Gaudi::Units::GeV;
  constexpr float pt_15_cut = 15. * Gaudi::Units::GeV;
  constexpr float pt_10_cut = 10. * Gaudi::Units::GeV;
  constexpr float pt_4_cut = 4. * Gaudi::Units::GeV;

  /// End of the barrel region
  constexpr float barrel_end = 1.05;
  constexpr float trigger_end = 2.4;

  /// Number of channels
  //constexpr int nchan_tgc_strip = 32;

  //constexpr int nchan_tgc_wire_station1_lay1[] = {105,24,23,61,92};//F,E1,E2,E3,E4
  //constexpr int nchan_tgc_wire_station1_lay2[] = {104,24,23,62,91};
  //constexpr int nchan_tgc_wire_station1_lay3[] = {105,24,23,62,91};

  //constexpr int nchan_tgc_wire_station2[] = {125,32,32,32,103,110};//F,E1,E2,E3,E4,E5

  //constexpr int nchan_tgc_wire_station3[] = {122,31,30,32,106,96};//F,E1,E2,E3,E4,E5

  //constexpr int nchan_tgc_wire_station4_fi = 32;//FI
  //constexpr int nchan_tgc_wire_station4_ei = 24;//EI
  //constexpr int nchan_tgc_wire_station4_eis = 16;//EI(S)

  // offset for better drawing
  constexpr float tgc_coin_phi_small_offset = 0.0001;
}
TgcRawDataMonitorAlgorithm::TgcRawDataMonitorAlgorithm(const std::string &name, ISvcLocator *pSvcLocator) :
  AthMonitorAlgorithm(name, pSvcLocator) {
}

StatusCode TgcRawDataMonitorAlgorithm::initialize() {
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_extrapolator.retrieve());
  ATH_CHECK(m_MuonContainerKey.initialize());
  ATH_CHECK(m_MuonRoIContainerKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_TgcPrepDataContainerKey.initialize());
  ATH_CHECK(m_TgcCoinDataContainerCurrBCKey.initialize());
  ATH_CHECK(m_TgcCoinDataContainerNextBCKey.initialize());
  ATH_CHECK(m_TgcCoinDataContainerPrevBCKey.initialize());

  m_extZposition.push_back(m_M1_Z.value());
  m_extZposition.push_back(m_M2_Z.value());
  m_extZposition.push_back(m_M3_Z.value());
  m_extZposition.push_back(m_EI_Z.value());
  m_extZposition.push_back(m_FI_Z.value());
  m_extZposition.push_back(-m_M1_Z.value());
  m_extZposition.push_back(-m_M2_Z.value());
  m_extZposition.push_back(-m_M3_Z.value());
  m_extZposition.push_back(-m_EI_Z.value());
  m_extZposition.push_back(-m_FI_Z.value());

  if(m_ctpDecMonList.value()!=""){
    m_CtpDecMonObj.clear();
    TString Str = m_ctpDecMonList.value();
    auto monTrigs = Str.Tokenize(";");
    for(int i = 0 ; i < monTrigs->GetEntries() ; i++){
      CtpDecMonObj monObj;
      monObj.trigItem = monObj.title = "dummy";
      monObj.rpcThr=monObj.tgcThr=monObj.multiplicity=0;
      monObj.tgcF=monObj.tgcC=monObj.tgcH=monObj.rpcR=monObj.rpcM=false;
      TString monTrig = monTrigs->At(i)->GetName();
      if(monTrig=="")continue;
      auto monElement = monTrig.Tokenize(",");
      for(int j = 0 ; j < monElement->GetEntries() ; j++){
	TString sysItem = monElement->At(j)->GetName();
	if(sysItem=="")continue;
	TString item = sysItem;
	item.Remove(0,4); // remove "Tit:", "CTP:", "HLT:", "RPC:", "TGC:"
	if(sysItem.BeginsWith("Tit")){
	  monObj.title = item;
	}else if(sysItem.BeginsWith("Mul")){
	  monObj.multiplicity = item.Atoi();
	}else if(sysItem.BeginsWith("CTP") || sysItem.BeginsWith("HLT")){
	  monObj.trigItem = item;
	}else if(sysItem.BeginsWith("RPC")){
	  monObj.rpcThr = item.Atoi();
	  monObj.rpcR = item.Contains("R");
	  monObj.rpcM = item.Contains("M");
	}else if(sysItem.BeginsWith("TGC")){
	  monObj.tgcThr = item.Atoi();
	  monObj.tgcF = item.Contains("F");
	  monObj.tgcC = item.Contains("C");
	  monObj.tgcH = item.Contains("H");
	}
      }
      m_CtpDecMonObj.push_back(monObj);
      monElement->Clear();
      monElement->Delete();
      delete monElement;
    }
    monTrigs->Clear();
    monTrigs->Delete();
    delete monTrigs;
  }

  return StatusCode::SUCCESS;
}

StatusCode TgcRawDataMonitorAlgorithm::fillHistograms(const EventContext &ctx) const {

  // Print out all available muon triggers
  // This is to be used when making a list of triggers
  // to be monitored, and writted in .py config file
  // The defult should be FALSE
  if( m_printAvailableMuonTriggers.value() ){
    if( getTrigDecisionTool().empty() ){
      ATH_MSG_ERROR("TDT is not availeble");
      return StatusCode::FAILURE;
    }else{
      std::set<TString> available_muon_triggers;
      auto chainGroup = getTrigDecisionTool()->getChainGroup("HLT_.*");
      if( chainGroup != nullptr ){
	auto triggerList = chainGroup->getListOfTriggers();
	if( !triggerList.empty() ){
	  for(const auto &trig : triggerList) {
	    TString thisTrig = trig;
	    if( !thisTrig.Contains("mu") && !thisTrig.Contains("MU"))continue;
	    if(getTrigDecisionTool()->getNavigationFormat() == "TriggerElement") { // run 2 access
	      auto fc = getTrigDecisionTool()->features(thisTrig.Data(),TrigDefs::alsoDeactivateTEs);
	      for(const auto& comb : fc.getCombinations()){
		auto initRoIs = comb.get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
		for(const auto& roi : initRoIs){
		  if( roi.empty() )continue;
		  if( roi.cptr()==nullptr ) continue;
		  // found an available muon trigger here
		  available_muon_triggers.insert(thisTrig);
		}
	      }
	    }else{ // run 3 access
	      auto features =getTrigDecisionTool()->features<xAOD::MuonContainer>(thisTrig.Data(), TrigDefs::includeFailedDecisions);
	      for(const auto& muLinkInfo : features) {
		if( !muLinkInfo.isValid() )continue;
		auto roiLinkInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(muLinkInfo.source, "initialRoI");
		if( !roiLinkInfo.isValid() )continue;
		auto roiEL = roiLinkInfo.link;
		if( !roiEL.isValid() )continue;
		auto roi = *roiEL;
		if( roi==nullptr ) continue;
		// found an available muon trigger here
		available_muon_triggers.insert(thisTrig);
	      }
	    }
	  }
	}
      }
      for(const auto& trig : available_muon_triggers){
	ATH_MSG_INFO("Available Muon Trigger: " << trig);
      }
      available_muon_triggers.clear();
      return StatusCode::SUCCESS;
    }
  } ///////////////End of printing out available muon triggers

  MonVariables variables;

  auto bcid = Monitored::Scalar<int>("bcid", GetEventInfo(ctx)->bcid());
  auto pileup = Monitored::Scalar<int>("pileup", lbAverageInteractionsPerCrossing(ctx));
  auto lumiBlock = Monitored::Scalar<int>("lumiBlock", GetEventInfo(ctx)->lumiBlock());

  variables.push_back(bcid);
  variables.push_back(pileup);
  variables.push_back(lumiBlock);

  if (!m_anaMuonRoI.value()) {
    fill(m_packageName, variables);
    variables.clear();
    return StatusCode::SUCCESS;
  }

  const xAOD::MuonRoIContainer *rois = nullptr;
  if (!m_MuonRoIContainerKey.empty()) {
    /* raw LVL1MuonRoIs distributions */
    SG::ReadHandle<xAOD::MuonRoIContainer > handle( m_MuonRoIContainerKey, ctx);
    if(!handle.isValid()) {
      ATH_MSG_ERROR("evtStore() does not contain muon RoI Collection with name " << m_MuonRoIContainerKey);
      return StatusCode::FAILURE;
    }
    rois = handle.cptr();
    if (!rois) {
      ATH_MSG_ERROR("evtStore() does not contain muon RoI Collection with name " << m_MuonRoIContainerKey);
      return StatusCode::FAILURE;
    }

    MonVariables  roi_variables;
    auto roi_bcid = Monitored::Scalar<int>("roi_bcid", GetEventInfo(ctx)->bcid());
    roi_variables.push_back(roi_bcid);
    auto roi_pileup = Monitored::Scalar<int>("roi_pileup", lbAverageInteractionsPerCrossing(ctx));
    roi_variables.push_back(roi_pileup);
    auto roi_lumiBlock = Monitored::Scalar<int>("roi_lumiBlock", GetEventInfo(ctx)->lumiBlock());
    roi_variables.push_back(roi_lumiBlock);
    auto roi_eta = Monitored::Collection("roi_eta", *rois, [](const xAOD::MuonRoI *m) {
	return m->eta();
      });
    roi_variables.push_back(roi_eta);
    auto roi_eta_rpc = Monitored::Collection("roi_eta_rpc", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() == xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_rpc);
    auto roi_eta_tgc = Monitored::Collection("roi_eta_tgc", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() != xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_tgc);
    auto roi_eta_wInnCoin = Monitored::Collection("roi_eta_wInnCoin", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getInnerCoincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_wInnCoin);
    auto roi_eta_wBW3Coin = Monitored::Collection("roi_eta_wBW3Coin", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getBW3Coincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_wBW3Coin);
    auto roi_eta_wInnCoinVeto = Monitored::Collection("roi_eta_wInnCoinVeto", *rois, [](const xAOD::MuonRoI *m) {
	return (!m->getInnerCoincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_wInnCoinVeto);
    auto roi_eta_wBW3CoinVeto = Monitored::Collection("roi_eta_wBW3CoinVeto", *rois, [](const xAOD::MuonRoI *m) {
	return (!m->getBW3Coincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->eta()):(-10);
      });
    roi_variables.push_back(roi_eta_wBW3CoinVeto);
    auto roi_phi = Monitored::Collection("roi_phi", *rois, [](const xAOD::MuonRoI *m) {
	return m->phi();
      });
    roi_variables.push_back(roi_phi);
    auto roi_phi_rpc = Monitored::Collection("roi_phi_rpc", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() == xAOD::MuonRoI::Barrel) ? m->phi() : -10;
      });
    roi_variables.push_back(roi_phi_rpc);
    auto roi_phi_tgc = Monitored::Collection("roi_phi_tgc", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() != xAOD::MuonRoI::Barrel) ? m->phi() : -10;
      });
    roi_variables.push_back(roi_phi_tgc);
    auto roi_phi_wInnCoin = Monitored::Collection("roi_phi_wInnCoin", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getInnerCoincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->phi()):(-10);;
      });
    roi_variables.push_back(roi_phi_wInnCoin);
    auto roi_phi_wBW3Coin = Monitored::Collection("roi_phi_wBW3Coin", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getBW3Coincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->phi()):(-10);;
      });
    roi_variables.push_back(roi_phi_wBW3Coin);
    auto roi_phi_wInnCoinVeto = Monitored::Collection("roi_phi_wInnCoinVeto", *rois, [](const xAOD::MuonRoI *m) {
	return (!m->getInnerCoincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->phi()):(-10);;
      });
    roi_variables.push_back(roi_phi_wInnCoinVeto);
    auto roi_phi_wBW3CoinVeto = Monitored::Collection("roi_phi_wBW3CoinVeto", *rois, [](const xAOD::MuonRoI *m) {
	return (!m->getBW3Coincidence() && m->getSource() != xAOD::MuonRoI::Barrel)?(m->phi()):(-10);;
      });
    roi_variables.push_back(roi_phi_wBW3CoinVeto);
    auto roi_thr = Monitored::Collection("roi_thr", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber();
      });
    roi_variables.push_back(roi_thr);
    auto roi_rpc = Monitored::Collection("roi_rpc", *rois, [](const xAOD::MuonRoI *m) {
	return m->getSource() == xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_rpc);
    auto roi_tgc = Monitored::Collection("roi_tgc", *rois, [](const xAOD::MuonRoI *m) {
	return m->getSource() != xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_tgc);
    auto roi_barrel = Monitored::Collection("roi_barrel", *rois, [](const xAOD::MuonRoI *m) {
	return m->getSource() == xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_barrel);
    auto roi_endcap = Monitored::Collection("roi_endcap", *rois, [](const xAOD::MuonRoI *m) {
	return m->getSource() == xAOD::MuonRoI::Endcap;
      });
    roi_variables.push_back(roi_endcap);
    auto roi_forward = Monitored::Collection("roi_forward", *rois, [](const xAOD::MuonRoI *m) {
	return m->getSource() == xAOD::MuonRoI::Forward;
      });
    roi_variables.push_back(roi_forward);
    auto roi_phi_barrel = Monitored::Collection("roi_phi_barrel", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() == xAOD::MuonRoI::Barrel) ? m->phi() : -10;
      });
    roi_variables.push_back(roi_phi_barrel);
    auto roi_phi_endcap = Monitored::Collection("roi_phi_endcap", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() == xAOD::MuonRoI::Endcap) ? m->phi() : -10;
      });
    roi_variables.push_back(roi_phi_endcap);
    auto roi_phi_forward = Monitored::Collection("roi_phi_forward", *rois, [](const xAOD::MuonRoI *m) {
	return (m->getSource() == xAOD::MuonRoI::Forward) ? m->phi() : -10;
      });
    roi_variables.push_back(roi_phi_forward);
    auto roi_sideA = Monitored::Collection("roi_sideA", *rois, [](const xAOD::MuonRoI *m) {
	return m->getHemisphere() == xAOD::MuonRoI::Positive;
      });
    roi_variables.push_back(roi_sideA);
    auto roi_sideC = Monitored::Collection("roi_sideC", *rois, [](const xAOD::MuonRoI *m) {
	return m->getHemisphere() == xAOD::MuonRoI::Negative;
      });
    roi_variables.push_back(roi_sideC);
    auto thrmask1 = Monitored::Collection("thrmask1", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 1;
      });
    roi_variables.push_back(thrmask1);
    auto thrmask2 = Monitored::Collection("thrmask2", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 2;
      });
    roi_variables.push_back(thrmask2);
    auto thrmask3 = Monitored::Collection("thrmask3", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 3;
      });
    roi_variables.push_back(thrmask3);
    auto thrmask4 = Monitored::Collection("thrmask4", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 4;
      });
    roi_variables.push_back(thrmask4);
    auto thrmask5 = Monitored::Collection("thrmask5", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 5;
      });
    roi_variables.push_back(thrmask5);
    auto thrmask6 = Monitored::Collection("thrmask6", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 6;
      });
    roi_variables.push_back(thrmask6);
    auto thrmask7 = Monitored::Collection("thrmask7", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 7;
      });
    roi_variables.push_back(thrmask7);
    auto thrmask8 = Monitored::Collection("thrmask8", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 8;
      });
    roi_variables.push_back(thrmask8);
    auto thrmask9 = Monitored::Collection("thrmask9", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 9;
      });
    roi_variables.push_back(thrmask9);
    auto thrmask10 = Monitored::Collection("thrmask10", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 10;
      });
    roi_variables.push_back(thrmask10);
    auto thrmask11 = Monitored::Collection("thrmask11", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 11;
      });
    roi_variables.push_back(thrmask11);
    auto thrmask12 = Monitored::Collection("thrmask12", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 12;
      });
    roi_variables.push_back(thrmask12);
    auto thrmask13 = Monitored::Collection("thrmask13", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 13;
      });
    roi_variables.push_back(thrmask13);
    auto thrmask14 = Monitored::Collection("thrmask14", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 14;
      });
    roi_variables.push_back(thrmask14);
    auto thrmask15 = Monitored::Collection("thrmask15", *rois, [](const xAOD::MuonRoI *m) {
	return m->getThrNumber() == 15;
      });
    roi_variables.push_back(thrmask15);
    auto roi_charge = Monitored::Collection("roi_charge", *rois, [](const xAOD::MuonRoI* m) {
	return (m->getCharge()==xAOD::MuonRoI::Neg)?(-1):((m->getCharge()==xAOD::MuonRoI::Pos)?(+1):(0));
      });
    roi_variables.push_back(roi_charge);
    auto roi_bw3coin = Monitored::Collection("roi_bw3coin",*rois,[](const xAOD::MuonRoI* m) {
	return m->getBW3Coincidence();
      });
    roi_variables.push_back(roi_bw3coin);
    auto roi_bw3coinveto = Monitored::Collection("roi_bw3coinveto",*rois,[](const xAOD::MuonRoI* m) {
	return !m->getBW3Coincidence() && m->getSource()!=xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_bw3coinveto);
    auto roi_inncoin = Monitored::Collection("roi_inncoin",*rois,[](const xAOD::MuonRoI* m) {
	return m->getInnerCoincidence();
      });
    roi_variables.push_back(roi_inncoin);
    auto roi_innveto = Monitored::Collection("roi_innveto",*rois,[](const xAOD::MuonRoI* m) {
	return !m->getInnerCoincidence() && m->getSource()!=xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_innveto);
    auto roi_goodmf = Monitored::Collection("roi_goodmf",*rois,[](const xAOD::MuonRoI* m) {
	return m->getGoodMF();
      });
    roi_variables.push_back(roi_goodmf);
    auto roi_badmf = Monitored::Collection("roi_badmf",*rois,[](const xAOD::MuonRoI* m){
	return !m->getGoodMF() && m->getSource()!=xAOD::MuonRoI::Barrel;
      });
    roi_variables.push_back(roi_badmf);
    auto roi_ismorecand = Monitored::Collection("roi_ismorecand",*rois,[](const xAOD::MuonRoI* m){
	return m->isMoreCandInRoI();
      });
    roi_variables.push_back(roi_ismorecand);
    auto roi_posCharge = Monitored::Collection("roi_posCharge",*rois,[](const xAOD::MuonRoI* m){
	return m->getCharge()==xAOD::MuonRoI::Pos;
      });
    roi_variables.push_back(roi_posCharge);
    auto roi_negCharge = Monitored::Collection("roi_negCharge",*rois,[](const xAOD::MuonRoI* m){
	return m->getCharge()==xAOD::MuonRoI::Neg;
      });
    roi_variables.push_back(roi_negCharge);
    fill(m_packageName, roi_variables);
    roi_variables.clear();
  }
  if (!m_anaOfflMuon.value()) {
    fill(m_packageName, variables);
    variables.clear();
    return StatusCode::SUCCESS;
  }

  if ( !getTrigDecisionTool().empty() && rois != nullptr ) {
    std::set<unsigned int> allCands;
    std::set<unsigned int> ctpMuonCands;
    std::set<unsigned int> inputMuonCands;
    std::map<unsigned int,double> roiEta;
    std::map<unsigned int,double> roiPhi;
    MonVariables  ctpMonVariables;
    std::vector<double> roiEta_inOk_outOk;
    std::vector<double> roiEta_inNg_outOk;
    std::vector<double> roiEta_inOk_outNg;
    std::vector<double> roiPhi_inOk_outOk;
    std::vector<double> roiPhi_inNg_outOk;
    std::vector<double> roiPhi_inOk_outNg;
    std::vector<int> roiMatching_CTPin;
    std::vector<int> roiMatching_CTPout;
    for(const auto& monObj : m_CtpDecMonObj){
      // collecting roiWords out of the CTP decision
      if(getTrigDecisionTool()->getNavigationFormat() == "TriggerElement") { // run 2 access
	auto fc = getTrigDecisionTool()->features(monObj.trigItem.Data(),TrigDefs::alsoDeactivateTEs);
	for(const auto& comb : fc.getCombinations()){
	  auto initRoIs = comb.get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
	  for(const auto& roi : initRoIs){
	    if( roi.empty() )continue;
	    if( roi.cptr()==nullptr ) continue;
	    ctpMuonCands.insert(roi.cptr()->roiWord());
	    allCands.insert(roi.cptr()->roiWord());
	    roiEta[roi.cptr()->roiWord()] = roi.cptr()->eta();
	    roiPhi[roi.cptr()->roiWord()] = roi.cptr()->phi();
	  }
	}
      }else{ // run 3 access
	auto initialRoIs = getTrigDecisionTool()->features<TrigRoiDescriptorCollection>(monObj.trigItem.Data(), TrigDefs::includeFailedDecisions, "", TrigDefs::lastFeatureOfType, "initialRoI");
	for(const auto& roiLinkInfo : initialRoIs) {
	  if( !roiLinkInfo.isValid() )continue;
	  auto roiEL = roiLinkInfo.link;
	  if( !roiEL.isValid() )continue;
	  auto roi = *roiEL;
	  if( roi==nullptr ) continue;
	  ctpMuonCands.insert(roi->roiWord());
	  allCands.insert(roi->roiWord());
	  roiEta[roi->roiWord()] = roi->eta();
	  roiPhi[roi->roiWord()] = roi->phi();
	}
      }
      // collecting roiWords out of RPC/TGC
      for(const auto roi : *rois){
	if(roi->getSource()==xAOD::MuonRoI::Barrel){
	  if(roi->getThrNumber()<monObj.rpcThr)continue;
	  if(monObj.rpcM && !roi->isMoreCandInRoI())continue;
	}else{
	  if(roi->getThrNumber()<monObj.tgcThr)continue;
	  if(monObj.tgcF && !roi->getBW3Coincidence())continue;
	  if(monObj.tgcC && !roi->getInnerCoincidence())continue;
	  if(monObj.tgcH && !roi->getGoodMF())continue;
	}
	inputMuonCands.insert(roi->roiWord());
	allCands.insert(roi->roiWord());
	roiEta[roi->roiWord()] = roi->eta();
	roiPhi[roi->roiWord()] = roi->phi();
      }

      if(ctpMuonCands.size()>=monObj.multiplicity || inputMuonCands.size()>=monObj.multiplicity){
	for(const auto& roiWord : allCands){
	  bool ctp_in  = inputMuonCands.find(roiWord)!=inputMuonCands.end();
	  bool ctp_out = ctpMuonCands.find(roiWord)!=ctpMuonCands.end();
	  roiMatching_CTPin.push_back(ctp_in?1:0);
	  roiMatching_CTPout.push_back(ctp_out?1:0);
	  if(ctp_in && ctp_out){ // good
	    roiEta_inOk_outOk.push_back(roiEta[roiWord]);
	    roiPhi_inOk_outOk.push_back(roiPhi[roiWord]);
	  }else if(!ctp_in && ctp_out){ // fake output from CTP (must be a bug)
	    roiEta_inNg_outOk.push_back(roiEta[roiWord]);
	    roiPhi_inNg_outOk.push_back(roiPhi[roiWord]);
	  }else if(ctp_in && !ctp_out){ // event dropped at CTP (overlap removal? bug?)
	    roiEta_inOk_outNg.push_back(roiEta[roiWord]);
	    roiPhi_inOk_outNg.push_back(roiPhi[roiWord]);
	  }
	}
	auto ctpMultiplicity = Monitored::Scalar<int>(Form("ctpMultiplicity_%s",monObj.title.Data()),ctpMuonCands.size());
	ctpMonVariables.push_back(ctpMultiplicity);
	auto rawMultiplicity = Monitored::Scalar<int>(Form("rawMultiplicity_%s",monObj.title.Data()),inputMuonCands.size());
	ctpMonVariables.push_back(rawMultiplicity);
	auto countDiff = Monitored::Scalar<int>(Form("countDiff_%s",monObj.title.Data()),ctpMuonCands.size()-inputMuonCands.size());
	ctpMonVariables.push_back(countDiff);
	auto val_roiEta_inOk_outOk = Monitored::Collection(Form("roiEta_inOk_outOk_%s",monObj.title.Data()), roiEta_inOk_outOk);
	auto val_roiEta_inNg_outOk = Monitored::Collection(Form("roiEta_inNg_outOk_%s",monObj.title.Data()), roiEta_inNg_outOk);
	auto val_roiEta_inOk_outNg = Monitored::Collection(Form("roiEta_inOk_outNg_%s",monObj.title.Data()), roiEta_inOk_outNg);
	auto val_roiPhi_inOk_outOk = Monitored::Collection(Form("roiPhi_inOk_outOk_%s",monObj.title.Data()), roiPhi_inOk_outOk);
	auto val_roiPhi_inNg_outOk = Monitored::Collection(Form("roiPhi_inNg_outOk_%s",monObj.title.Data()), roiPhi_inNg_outOk);
	auto val_roiPhi_inOk_outNg = Monitored::Collection(Form("roiPhi_inOk_outNg_%s",monObj.title.Data()), roiPhi_inOk_outNg);
	ctpMonVariables.push_back(val_roiEta_inOk_outOk);
	ctpMonVariables.push_back(val_roiEta_inNg_outOk);
	ctpMonVariables.push_back(val_roiEta_inOk_outNg);
	ctpMonVariables.push_back(val_roiPhi_inOk_outOk);
	ctpMonVariables.push_back(val_roiPhi_inNg_outOk);
	ctpMonVariables.push_back(val_roiPhi_inOk_outNg);
	auto val_roiMatching_CTPin = Monitored::Collection(Form("roiMatching_CTPin_%s",monObj.title.Data()), roiMatching_CTPin);
	auto val_roiMatching_CTPout = Monitored::Collection(Form("roiMatching_CTPout_%s",monObj.title.Data()), roiMatching_CTPout);
	ctpMonVariables.push_back(val_roiMatching_CTPin);
	ctpMonVariables.push_back(val_roiMatching_CTPout);
	fill(m_packageName, ctpMonVariables);
      }
      ctpMuonCands.clear();
      inputMuonCands.clear();
      allCands.clear();
      roiEta.clear();
      roiPhi.clear();
      ctpMonVariables.clear();
      roiEta_inOk_outOk.clear();
      roiEta_inNg_outOk.clear();
      roiEta_inOk_outNg.clear();
      roiPhi_inOk_outOk.clear();
      roiPhi_inNg_outOk.clear();
      roiPhi_inOk_outNg.clear();
      roiMatching_CTPin.clear();
      roiMatching_CTPout.clear();
    }
  }

  SG::ReadHandle < xAOD::MuonContainer > muons(m_MuonContainerKey, ctx);
  if (!muons.isValid()) {
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name " << m_MuonContainerKey);
    return StatusCode::FAILURE;
  }

  // check trigger information
  bool nonMuonTriggerFired = false;
  std::set<TString> list_of_single_muon_triggers;
  if ( !getTrigDecisionTool().empty() ){
    auto chainGroup = getTrigDecisionTool()->getChainGroup("HLT_.*");
    if( chainGroup != nullptr ){
      auto triggerList = chainGroup->getListOfTriggers();
      if( !triggerList.empty() ){
	for(const auto &trig : triggerList) {
	  TString thisTrig = trig;
	  if( thisTrig.BeginsWith("HLT_mu") ){ // muon triggers
	    // look for only single-muon triggers
	    if ( thisTrig.Contains("-") ) continue; // skipping L1Topo item
	    if ( thisTrig.Contains("msonly") ) continue; // only combined muon
	    if ( thisTrig.Contains("2MU") ) continue; // no L12MU
	    if ( thisTrig.Contains("3MU") ) continue; // no L13MU
	    if ( thisTrig.Contains("4MU") ) continue; // no L14MU
	    if ( thisTrig.Contains("mu") ) {
	      TString tmp = thisTrig;
	      tmp.ReplaceAll("mu","ZZZ");
	      auto arr = tmp.Tokenize("ZZZ");
	      auto n = arr->GetEntries();
	      arr->Clear();
	      arr->Delete();
	      delete arr;
	      if ( n != 2 ) continue; // exact one 'mu' letter
	    }
	    if ( thisTrig.Contains("MU") ) {
	      TString tmp = thisTrig;
	      tmp.ReplaceAll("MU","ZZZ");
	      auto arr = tmp.Tokenize("ZZZ");
	      auto n = arr->GetEntries();
	      arr->Clear();
	      arr->Delete();
	      delete arr;
	      if ( n > 2 ) continue; // only one 'MU' letter if exists
	    }
	    list_of_single_muon_triggers.insert( thisTrig );
	  }else{
	    if( m_useNonMuonTriggers.value() == false ) continue;
	    // look for muon-orthogonal triggers and if they fired
	    if(nonMuonTriggerFired)continue; // already checked, skipping
	    if(thisTrig.Contains("mu")) continue;
	    if(thisTrig.Contains("MU")) continue;
	    if(getTrigDecisionTool()->isPassed(thisTrig.Data(),TrigDefs::Physics)) nonMuonTriggerFired = true;
	  }
	}
      }
    }
  }

  std::vector < MyMuon > mymuons;
  for (const auto muon : *muons) {

    // skip if muon is empty
    if (muon == nullptr) continue;

    // standard quality cuts for muons
    if (muon->pt() < 1000.) continue;
    if (muon->muonType() != xAOD::Muon::Combined) continue;
    if (muon->author() != xAOD::Muon::MuidCo && muon->author() != xAOD::Muon::STACO) continue;
    if (muon->quality() != xAOD::Muon::Tight && muon->quality() != xAOD::Muon::Medium) continue;

    // initialize for muon-isolation check
    bool isolated = true;

    // initialize for tag-and-probe check
    bool probeOK = true;
    if( !nonMuonTriggerFired && m_TagAndProbe.value() ) probeOK = false; // t&p should be performed

    // OK, let's start looking at the second muons
    for(const auto muon2 : *muons){

      // skip if muon is empty
      if (muon2 == nullptr) continue;

      // skip the same muon candidate
      if( muon == muon2 )continue;

      // skip possible mismeasured muons
      if( muon2->pt() < 1000. ) continue;

      // check muon-muon isolation using the loosest-quality muons
      if ( isolated ){
	double dr = xAOD::P4Helpers::deltaR(muon,muon2,false);
	if( dr < m_isolationWindow.value() ) isolated = false;
      }

      // no need to check further if probeOK is already True
      // 0) if muon-orthogonal triggers are avaialble/fired
      // 1) if we don't use tag-and-probe
      // 2) if TrigDecTool is not available
      // 3) if the second muon matches the trigger requirement
      if(probeOK)continue;

      //  standard quality cuts for muons
      if( muon2->muonType()!=xAOD::Muon::Combined )continue;
      if( muon2->author()!=xAOD::Muon::MuidCo && muon2->author()!=xAOD::Muon::STACO )continue;
      if( muon2->quality()!=xAOD::Muon::Tight && muon2->quality()!=xAOD::Muon::Medium )continue;

      // loop over the single muon triggers if at least one of them matches this second muon
      for (const auto &trigName : list_of_single_muon_triggers) {
	// check if this particular tirgger has fired in this event
	if (!getTrigDecisionTool()->isPassed(trigName.Data(),TrigDefs::Physics)) continue;
	ATH_MSG_DEBUG("This muon trigger, " << trigName << ", is fired in this event!!");
	// check if this second muon matches the HLT muon trigger
	if(getTrigDecisionTool()->getNavigationFormat() == "TriggerElement") { // run 2 access
	  ATH_MSG_DEBUG("Trying Run2-style feature access");
	  auto fc = getTrigDecisionTool()->features(trigName.Data(),TrigDefs::Physics);
	  for(const auto& comb : fc.getCombinations()){
	    if(!comb.active())continue;
	    auto MuFeatureContainers = comb.get<xAOD::MuonContainer>("MuonEFInfo",TrigDefs::Physics);
	    for(const auto& mucont : MuFeatureContainers){
	      if(mucont.empty())continue;
	      if(mucont.te()==nullptr)continue;
	      if(!mucont.te()->getActiveState())continue;
	      for(const auto hltmu : *mucont.cptr()){
		if (hltmu == nullptr) continue; // skip if hltmu is empty
		if (hltmu->pt() < 1000.)continue; // skip if pT is very small
		double dr = xAOD::P4Helpers::deltaR(muon2,hltmu,false);
		if( dr < m_trigMatchWindow.value() ){
		  ATH_MSG_DEBUG("Trigger matched: "<<trigName<<" dR=" << dr );
		  probeOK = true;
		}
		if(probeOK) break; // no need to check further if probeOK is already True
	      }// end loop of mucont.cptr()
	      if(probeOK) break; // no need to check further if probeOK is already True
	    }// end loop of MuonFeatureContainers
	    if(probeOK) break; // no need to check further if probeOK is already True
	  }//end loop of Combinations
	}else{ // run 3 access
	  ATH_MSG_DEBUG("Trying Run3-style feature access");
	  auto features = getTrigDecisionTool()->features < xAOD::MuonContainer > (trigName.Data(), TrigDefs::Physics, "HLT_MuonsCB_RoI");
	  for (const auto& aaa : features) {
	    if (!aaa.isValid()) continue;
	    auto hltmu_link = aaa.link;
	    if (!hltmu_link.isValid()) continue;
	    auto hltmu = *hltmu_link;
	    if (hltmu == nullptr) continue; // skip if hltmu is empty
	    if (hltmu->pt() < 1000.)continue; // skip if pT is very small
	    double dr = xAOD::P4Helpers::deltaR(muon2,hltmu,false);
	    if( dr < m_trigMatchWindow.value() ){
	      ATH_MSG_DEBUG("Trigger matched: "<<trigName<<" dR=" << dr );
	      probeOK = true;
	    }
	    if(probeOK) break; // no need to check further if probeOK is already True
	  } // end loop of features
	} // end IF Run2 or Run3 feature access
	if(probeOK) break; // no need to check further if probeOK is already True
      } // end loop of single muon triggers
      // check if the second muon matches the single muon trigger
      if(!probeOK) continue;
      ATH_MSG_DEBUG("Basic Tag-and-Probe is OK");
      // check further if this muon pair satisfies Z->mumu criteria
      if( m_TagAndProbeZmumu.value() && muon->charge() != muon2->charge() ){
	double m2 = 2. * muon->pt() * muon2->pt() * ( std::cosh(muon->eta() - muon2->eta()) - std::cos(muon->phi() - muon2->phi()) );
	double m = (m2>0.) ? ( std::sqrt(m2) ) : (0.);
	double mdiff = std::abs( m - m_zMass.value() );
	probeOK = mdiff < m_zMassWindow.value();
	ATH_MSG_DEBUG("Checking Zmumu cut: " << probeOK);
      }
      ATH_MSG_DEBUG("Final condition of probleOK for this muon is: " << probeOK);
      if(probeOK) break; // no need to check further if probeOK is already True
    } // end loop of the second muons
    // check if the isolation requirement is OK
    if(m_requireIsolated.value() && !isolated)continue;
    // check if the tag-and-probe requirement is OK
    if(!probeOK)continue;

    MyMuon mymuon;
    /* fill basic info */
    mymuon.muon = muon;
    /* fill extrapolation info (only to TGC) */
    extrapolate( muon, mymuon );
    /* L1Muon RoI matching */
    mymuon.matchedL1Charge=false;
    mymuon.passBW3Coin=false;
    mymuon.passInnerCoin=false;
    mymuon.passGoodMF=false;
    mymuon.passIsMoreCandInRoI=false;
    float max_dr = 999;
    float pt = mymuon.muon->pt();
    if (pt > pt_15_cut) max_dr = m_l1trigMatchWindow1.value();
    else if (pt > pt_10_cut) max_dr = m_l1trigMatchWindow2.value() + m_l1trigMatchWindow3.value() * pt / Gaudi::Units::GeV;
    else max_dr = m_l1trigMatchWindow4.value() + m_l1trigMatchWindow5.value() * pt / Gaudi::Units::GeV;
    if (!rois) {
      ATH_MSG_DEBUG("No RoI matching possible as no container has been retrieved");
      mymuons.push_back(mymuon);
      continue;
    }
    for(const auto roi : *rois){
      double dr = xAOD::P4Helpers::deltaR(*muon,roi->eta(),roi->phi(),false);
      if( dr < max_dr ){
	mymuon.matchedL1ThrExclusive.insert( roi->getThrNumber() );
	if(muon->charge()<0 && roi->getCharge()==xAOD::MuonRoI::Neg)mymuon.matchedL1Charge|=true;
	else if(muon->charge()>0 && roi->getCharge()==xAOD::MuonRoI::Pos)mymuon.matchedL1Charge|=true;
	mymuon.passBW3Coin|=roi->getBW3Coincidence();
	mymuon.passInnerCoin|=roi->getInnerCoincidence();
	mymuon.passGoodMF|=roi->getGoodMF();
	mymuon.passIsMoreCandInRoI|=roi->isMoreCandInRoI();
      }
    }
    const bool L1ThrsEmpt = mymuon.matchedL1ThrExclusive.empty();
    for (int ithr = 0; ithr < 16 && !L1ThrsEmpt; ++ithr) {
      bool pass = false;
      for (const auto &thr : mymuon.matchedL1ThrExclusive) {
	if (thr >= ithr) {
	  pass = true;
	  break;
	}
      }
      if (pass) mymuon.matchedL1ThrInclusive.insert(ithr);
    }

    /* store MyMuon */
    mymuons.push_back(mymuon);
  }

  list_of_single_muon_triggers.clear();

  auto muon_eta4gev = Monitored::Collection("muon_eta4gev",mymuons,[](const MyMuon& m){
      return (m.muon->pt()>pt_4_cut)?m.muon->eta():-10;
    });
  variables.push_back(muon_eta4gev);
  auto muon_phi4gev = Monitored::Collection("muon_phi4gev",mymuons,[](const MyMuon& m){
      return (m.muon->pt()>pt_4_cut)?m.muon->phi():-10;
    });
  variables.push_back(muon_phi4gev);
  auto muon_eta = Monitored::Collection("muon_eta", mymuons, [](const MyMuon &m) {
      return (m.muon->pt() > pt_30_cut) ? m.muon->eta() : -10;
    });
  variables.push_back(muon_eta);
  auto muon_phi = Monitored::Collection("muon_phi", mymuons, [](const MyMuon &m) {
      return (m.muon->pt() > pt_30_cut) ? m.muon->phi() : -10;
    });
  variables.push_back(muon_phi);
  auto muon_phi_rpc = Monitored::Collection("muon_phi_rpc", mymuons, [](const MyMuon &m) {
      return (std::abs(m.muon->eta()) < barrel_end && m.muon->pt() > pt_30_cut) ? m.muon->phi() : -10;
    });
  variables.push_back(muon_phi_rpc);
  auto muon_phi_tgc = Monitored::Collection("muon_phi_tgc", mymuons, [](const MyMuon &m) {
      return (std::abs(m.muon->eta()) > barrel_end && std::abs(m.muon->eta()) < trigger_end && m.muon->pt() > pt_30_cut) ? m.muon->phi() : -10;
    });
  variables.push_back(muon_phi_tgc);
  auto muon_pt_rpc = Monitored::Collection("muon_pt_rpc", mymuons, [](const MyMuon &m) {
      return (std::abs(m.muon->eta()) < barrel_end) ? m.muon->pt() / Gaudi::Units::GeV : -10;
    });
  variables.push_back(muon_pt_rpc);
  auto muon_pt_tgc = Monitored::Collection("muon_pt_tgc", mymuons, [](const MyMuon &m) {
      return (std::abs(m.muon->eta()) > barrel_end && std::abs(m.muon->eta()) < trigger_end) ? m.muon->pt() / Gaudi::Units::GeV : -10;
    });
  variables.push_back(muon_pt_tgc);
  auto muon_l1passThr1 = Monitored::Collection("muon_l1passThr1", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(1) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr1);
  auto muon_l1passThr2 = Monitored::Collection("muon_l1passThr2", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(2) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr2);
  auto muon_l1passThr3 = Monitored::Collection("muon_l1passThr3", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(3) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr3);
  auto muon_l1passThr4 = Monitored::Collection("muon_l1passThr4", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(4) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr4);
  auto muon_l1passThr5 = Monitored::Collection("muon_l1passThr5", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(5) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr5);
  auto muon_l1passThr6 = Monitored::Collection("muon_l1passThr6", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(6) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr6);
  auto muon_l1passThr7 = Monitored::Collection("muon_l1passThr7", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(7) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr7);
  auto muon_l1passThr8 = Monitored::Collection("muon_l1passThr8", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(8) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr8);
  auto muon_l1passThr9 = Monitored::Collection("muon_l1passThr9", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(9) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr9);
  auto muon_l1passThr10 = Monitored::Collection("muon_l1passThr10", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(10) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr10);
  auto muon_l1passThr11 = Monitored::Collection("muon_l1passThr11", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(11) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr11);
  auto muon_l1passThr12 = Monitored::Collection("muon_l1passThr12", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(12) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr12);
  auto muon_l1passThr13 = Monitored::Collection("muon_l1passThr13", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(13) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr13);
  auto muon_l1passThr14 = Monitored::Collection("muon_l1passThr14", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(14) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr14);
  auto muon_l1passThr15 = Monitored::Collection("muon_l1passThr15", mymuons, [](const MyMuon &m) {
      return m.matchedL1ThrInclusive.find(15) != m.matchedL1ThrInclusive.end();
    });
  variables.push_back(muon_l1passThr15);
  auto muon_l1passCharge = Monitored::Collection("muon_l1passCharge",mymuons,[](const MyMuon& m){
      return m.matchedL1Charge;
    });
  variables.push_back(muon_l1passCharge);
  auto muon_l1passBW3Coin = Monitored::Collection("muon_l1passBW3Coin",mymuons,[](const MyMuon& m){
      return m.passBW3Coin;
    });
  variables.push_back(muon_l1passBW3Coin);
  auto muon_l1passBW3CoinVeto = Monitored::Collection("muon_l1passBW3CoinVeto",mymuons,[](const MyMuon& m){
      return !m.passBW3Coin;
    });
  variables.push_back(muon_l1passBW3CoinVeto);
  auto muon_l1passInnerCoin = Monitored::Collection("muon_l1passInnerCoin",mymuons,[](const MyMuon& m){
      return m.passInnerCoin;
    });
  variables.push_back(muon_l1passInnerCoin);
  auto muon_l1passInnerCoinVeto = Monitored::Collection("muon_l1passInnerCoinVeto",mymuons,[](const MyMuon& m){
      return !m.passInnerCoin;
    });
  variables.push_back(muon_l1passInnerCoinVeto);
  auto muon_l1passGoodMF = Monitored::Collection("muon_l1passGoodMF",mymuons,[](const MyMuon& m){
      return m.passGoodMF;
    });
  variables.push_back(muon_l1passGoodMF);
  auto muon_l1passBadMF = Monitored::Collection("muon_l1passBadMF",mymuons,[](const MyMuon& m){
      return !m.passGoodMF;
    });
  variables.push_back(muon_l1passBadMF);
  auto muon_l1passIsMoreCandInRoI = Monitored::Collection("muon_l1passIsMoreCandInRoI",mymuons,[](const MyMuon& m){
      return m.passIsMoreCandInRoI;
    });
  variables.push_back(muon_l1passIsMoreCandInRoI);

  if (!m_anaTgcPrd.value()) {
    fill(m_packageName, variables);
    variables.clear();
    for(auto& aa : mymuons)aa.clear();
    mymuons.clear();
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle < Muon::TgcPrepDataContainer > tgcPrd(m_TgcPrepDataContainerKey, ctx);
  if (!tgcPrd.isValid()) {
    ATH_MSG_ERROR("evtStore() does not contain TgcPrepDataContainer with name " << m_TgcPrepDataContainerKey);
    return StatusCode::FAILURE;
  }
  const TgcIdHelper &tgcIdHelper = m_idHelperSvc->tgcIdHelper();
  std::vector < TgcHit > tgcHits;
  std::set < TString > chamber_list;
  std::map<TString, std::vector<TgcHit>> tgcHitsMap;
  std::map<TString, std::vector<int>> tgcHitPhiMap;
  std::map<TString, std::vector<int>> tgcHitEtaMap;
  std::map<TString, std::vector<int>> tgcHitPhiMapGlobal;
  std::map<TString, std::vector<int>> tgcHitTiming;
  std::vector <int> vec_bw24sectors; // 1..12 BW-A, -1..-12 BW-C
  std::vector <int> vec_bw24sectors_wire;
  std::vector <int> vec_bw24sectors_strip;
  std::vector <int> vec_bwfulleta; // 0(Forward), 1..4(M1), 1..5(M2,M3)
  std::vector <int> vec_bwfulleta_wire;
  std::vector <int> vec_bwfulleta_strip;
  std::vector <int> vec_bwtiming;
  std::vector <int> vec_bwtiming_wire;
  std::vector <int> vec_bwtiming_strip;
  for (const auto tgccnt : *tgcPrd) {
    for (const auto data : *tgccnt) {
      TgcHit tgcHit;
      int bunch = -10;
      if ((data->getBcBitMap() & Muon::TgcPrepData::BCBIT_PREVIOUS) == Muon::TgcPrepData::BCBIT_PREVIOUS) bunch = -1;
      if ((data->getBcBitMap() & Muon::TgcPrepData::BCBIT_CURRENT) == Muon::TgcPrepData::BCBIT_CURRENT) bunch = 0;
      if ((data->getBcBitMap() & Muon::TgcPrepData::BCBIT_NEXT) == Muon::TgcPrepData::BCBIT_NEXT) bunch = +1;
      const MuonGM::TgcReadoutElement *element = data->detectorElement();
      const Identifier id = data->identify();
      const int gasGap = tgcIdHelper.gasGap(id);
      const int channel = tgcIdHelper.channel(id);
      const bool isStrip = tgcIdHelper.isStrip(id);
      const Amg::Vector3D &pos = isStrip ? element->stripPos(gasGap, channel) : element->gangPos(gasGap, channel);
      tgcHit.x = pos[0];
      tgcHit.y = pos[1];
      tgcHit.z = pos[2];
      if (isStrip) {
	tgcHit.shortWidth = element->stripShortWidth(gasGap, channel);
	tgcHit.longWidth = element->stripLongWidth(gasGap, channel);
	tgcHit.length = element->stripLength(gasGap, channel);
      } else {
	tgcHit.shortWidth = element->gangShortWidth(gasGap, channel);
	tgcHit.longWidth = element->gangLongWidth(gasGap, channel);
	tgcHit.length = element->gangLength(gasGap, channel);
      }
      tgcHit.isStrip = tgcIdHelper.isStrip(id);
      tgcHit.gasGap = tgcIdHelper.gasGap(id);
      tgcHit.channel = tgcIdHelper.channel(id);
      tgcHit.eta = tgcIdHelper.stationEta(id);
      tgcHit.phi = tgcIdHelper.stationPhi(id);
      tgcHit.station = tgcIdHelper.stationName(id);
      tgcHit.bunch = bunch;

      tgcHit.igasGap = tgcHit.gasGap;
      tgcHit.ieta = tgcHit.eta;
      tgcHit.iphi = tgcHit.phi;
      tgcHit.side = (tgcHit.ieta > 0) ? ("A") : ("C");
      tgcHit.iside = (tgcHit.ieta > 0) ? (0) : (1);
      tgcHit.M = 0;
      tgcHit.istation = tgcHit.station;
      if (tgcHit.istation == 41 || tgcHit.istation == 42) tgcHit.M = 1;
      else if (tgcHit.istation == 43 || tgcHit.istation == 44) tgcHit.M = 2;
      else if (tgcHit.istation == 45 || tgcHit.istation == 46) tgcHit.M = 3;
      else if (tgcHit.istation == 47 || tgcHit.istation == 48) tgcHit.M = 4; // EIFI
      if (tgcHit.M == 0) {
	ATH_MSG_ERROR("unknown station: " << tgcHit.istation);
      }

      if (tgcHit.M != 4) { // Big Wheel, (M1,M2,M3)
	if (tgcHit.istation % 2 == 0) { // Endcap
	  int iphi2 = tgcHit.iphi + 1; // 2,3,4,..,49
	  if (iphi2 >= 48) iphi2 -= 48; // 0,1,2,3 ..., 47
	  tgcHit.sector = int(iphi2 / 4) + 1; // 1,2,3,,,12
	  tgcHit.f = iphi2 - (tgcHit.sector - 1) * 4; // 0,1,2,3
	  tgcHit.E = (tgcHit.M == 1) ? (5 - TMath::Abs(tgcHit.ieta)) : (6 - TMath::Abs(tgcHit.ieta));
	  tgcHit.L = tgcHit.igasGap;
	  tgcHit.name = Form("%s%02iM%02if%02iE%02iL%02i%s", tgcHit.side.Data(), tgcHit.sector, tgcHit.M, tgcHit.f, tgcHit.E, tgcHit.L, (tgcHit.isStrip > 0) ? ("S") : ("W"));
	} else { // Forward
	  int iphi2 = tgcHit.iphi; // 1,2,3,4,..,25
	  if (iphi2 >= 24) iphi2 -= 24; // 0,1,2,3 ...,23
	  tgcHit.sector = int(iphi2 / 2) + 1; // 1,2,3,,,12
	  tgcHit.f = iphi2 - (tgcHit.sector - 1) * 2; // 0,1
	  if (tgcHit.f == 1) tgcHit.f = 2; //0,2
	  tgcHit.E = 0; // F
	  tgcHit.L = tgcHit.igasGap;
	  tgcHit.name = Form("%s%02iM%02if%02iF00L%02i%s", tgcHit.side.Data(), tgcHit.sector, tgcHit.M, tgcHit.f, tgcHit.L, (tgcHit.isStrip > 0) ? ("S") : ("W"));
	}
      } else { // Small Wheel (M4)
	if (tgcHit.istation == 47) { // FI
	  tgcHit.sector = 0;
	  tgcHit.f = tgcHit.iphi; // 1,2,3..24
	  tgcHit.E = 0;
	  tgcHit.L = tgcHit.igasGap;
	  tgcHit.name = Form("%s00M04f%02iF00L%02i%s", tgcHit.side.Data(), tgcHit.f, tgcHit.L, (tgcHit.isStrip > 0) ? ("S") : ("W"));
	} else if (tgcHit.istation == 48) { // EI
	  int iphi2 = (tgcHit.iphi >= 21) ? (tgcHit.iphi - 21) : (tgcHit.iphi); // 0,1,2,..,20
	  if (iphi2 >= 0 && iphi2 <= 2) {
	    tgcHit.sector = 1;
	    tgcHit.f = iphi2; // 0,1,2
	  } else if (iphi2 >= 3 && iphi2 <= 5) {
	    tgcHit.sector = 3;
	    tgcHit.f = iphi2 - 3; // 0,1,2
	  } else if (iphi2 >= 6 && iphi2 <= 8) {
	    tgcHit.sector = 5;
	    tgcHit.f = iphi2 - 6; // 0,1,2
	  } else if (iphi2 >= 9 && iphi2 <= 10) {
	    tgcHit.sector = 7;
	    tgcHit.f = iphi2 - 9 + 1; // 1,2
	  } else if (iphi2 >= 11 && iphi2 <= 13) {
	    tgcHit.sector = 9;
	    tgcHit.f = iphi2 - 11; // 0,1,2
	  } else if (iphi2 >= 14 && iphi2 <= 15) {
	    tgcHit.sector = 11;
	    tgcHit.f = iphi2 - 13; // 1,2
	  } else if (iphi2 >= 16 && iphi2 <= 18) {
	    tgcHit.sector = 13;
	    tgcHit.f = iphi2 - 16; // 0,1,2
	  } else if (iphi2 >= 19 && iphi2 <= 20) {
	    tgcHit.sector = 15;
	    tgcHit.f = iphi2 - 19 + 1; // 1,2
	  }
	  tgcHit.E = 1;
	  tgcHit.L = tgcHit.igasGap;
	  tgcHit.name = Form("%s%02iM04f%02iE01L%02i%s", tgcHit.side.Data(), tgcHit.sector, tgcHit.f, tgcHit.L, (tgcHit.isStrip > 0) ? ("S") : ("W"));
	} else {
	  ATH_MSG_ERROR("Unknown detector");
	}
      }
      tgcHits.push_back(tgcHit);
      tgcHitsMap[tgcHit.name].push_back(tgcHit);
      chamber_list.insert(tgcHit.name);

      TString station_name = Form("%sM%02i%s", tgcHit.side.Data(), tgcHit.M, (tgcHit.isStrip > 0) ? ("S") : ("W"));
      int phimap_index = 0;
      int etamap_index = 0;
      int phimap_global_index = 0; // no empty bins compare to the above index
      if (tgcHit.M == 1) {
	phimap_index = (tgcHit.sector - 1) * 4 + tgcHit.f + 1;
	int tmpeta = (tgcHit.E == 0) ? (5) : (tgcHit.E);
	etamap_index = (tmpeta - 1) * 3 + tgcHit.L;
	int tmpphi = tgcHit.f;
	if (tgcHit.E == 0 && tgcHit.f == 2) tmpphi = 1;
	if (tgcHit.E > 0) phimap_global_index = (tmpeta - 1) * 144 + (tgcHit.sector - 1) * 12 + tmpphi * 3 + tgcHit.L;
	else phimap_global_index = 576 + (tgcHit.sector - 1) * 6 + tmpphi * 3 + tgcHit.L;
      } else if (tgcHit.M == 2 || tgcHit.M == 3) {
	phimap_index = (tgcHit.sector - 1) * 4 + tgcHit.f + 1;
	int tmpeta = (tgcHit.E == 0) ? (6) : (tgcHit.E);
	etamap_index = (tmpeta - 1) * 2 + tgcHit.L;
	int tmpphi = tgcHit.f;
	if (tgcHit.E == 0 && tgcHit.f == 2) tmpphi = 1;
	if (tgcHit.E > 0) phimap_global_index = (tmpeta - 1) * 96 + (tgcHit.sector - 1) * 8 + tmpphi * 2 + tgcHit.L;
	else phimap_global_index = 480 + (tgcHit.sector - 1) * 4 + tmpphi * 2 + tgcHit.L;
      } else if (tgcHit.M == 4) {
	phimap_index = tgcHit.iphi;
	int tmpeta = (tgcHit.E == 0) ? (2) : (tgcHit.E);
	etamap_index = (tmpeta - 1) * 2 + tgcHit.L;
	if (tgcHit.E > 0) phimap_global_index = (tgcHit.iphi - 1) * 2 + tgcHit.L;
	else phimap_global_index = 42 + (tgcHit.iphi - 1) * 2 + tgcHit.L;
      }
      tgcHitPhiMap[station_name].push_back(phimap_index);
      tgcHitEtaMap[station_name].push_back(etamap_index);
      tgcHitPhiMapGlobal[station_name].push_back(phimap_global_index);
      tgcHitTiming[station_name].push_back(bunch);
      if(tgcHit.M!=4){
	vec_bw24sectors.push_back((tgcHit.iside>0)?(tgcHit.sector):(-tgcHit.sector));
	vec_bwfulleta.push_back(tgcHit.E);
	vec_bwtiming.push_back(tgcHit.bunch);
	if(tgcHit.isStrip>0){
	  vec_bw24sectors_strip.push_back((tgcHit.iside>0)?(tgcHit.sector):(-tgcHit.sector));
	  vec_bwfulleta_strip.push_back(tgcHit.E);
	  vec_bwtiming_strip.push_back(tgcHit.bunch);
	}else{
	  vec_bw24sectors_wire.push_back((tgcHit.iside>0)?(tgcHit.sector):(-tgcHit.sector));
	  vec_bwfulleta_wire.push_back(tgcHit.E);
	  vec_bwtiming_wire.push_back(tgcHit.bunch);
	}
      }
    }
  }

  auto hit_n = Monitored::Scalar<int>("hit_n", tgcHits.size());
  variables.push_back(hit_n);
  auto hit_bunch = Monitored::Collection("hit_bunch", tgcHits, [](const TgcHit &m) {
      return m.bunch;
    });
  variables.push_back(hit_bunch);
  auto hit_sideA = Monitored::Collection("hit_sideA", tgcHits, [](const TgcHit &m) {
      return m.z > 0;
    });
  variables.push_back(hit_sideA);
  auto hit_sideC = Monitored::Collection("hit_sideC", tgcHits, [](const TgcHit &m) {
      return m.z < 0;
    });
  variables.push_back(hit_sideC);
  auto lb_for_hit = Monitored::Scalar<int>("lb_for_hit", GetEventInfo(ctx)->lumiBlock());
  variables.push_back(lb_for_hit);
  auto bw24sectors = Monitored::Collection("hit_bw24sectors", vec_bw24sectors, [](const int &m) {
      return m;
    });
  variables.push_back(bw24sectors);
  auto bw24sectors_strip = Monitored::Collection("hit_bw24sectors_strip", vec_bw24sectors_strip, [](const int &m) {
      return m;
    });
  variables.push_back(bw24sectors_strip);
  auto bw24sectors_wire = Monitored::Collection("hit_bw24sectors_wire", vec_bw24sectors_wire, [](const int &m) {
      return m;
    });
  variables.push_back(bw24sectors_wire);
  auto bwfulleta = Monitored::Collection("hit_bwfulleta", vec_bwfulleta, [](const int &m) {
      return m;
    });
  variables.push_back(bwfulleta);
  auto bwfulleta_strip = Monitored::Collection("hit_bwfulleta_strip", vec_bwfulleta_strip, [](const int &m) {
      return m;
    });
  variables.push_back(bwfulleta_strip);
  auto bwfulleta_wire = Monitored::Collection("hit_bwfulleta_wire", vec_bwfulleta_wire, [](const int &m) {
      return m;
    });
  variables.push_back(bwfulleta_wire);
  auto bwtiming = Monitored::Collection("hit_bwtiming", vec_bwtiming, [](const int &m) {
      return m;
    });
  variables.push_back(bwtiming);
  auto bwtiming_strip = Monitored::Collection("hit_bwtiming_strip", vec_bwtiming_strip, [](const int &m) {
      return m;
    });
  variables.push_back(bwtiming_strip);
  auto bwtiming_wire = Monitored::Collection("hit_bwtiming_wire", vec_bwtiming_wire, [](const int &m) {
      return m;
    });
  variables.push_back(bwtiming_wire);



  for (const auto &chamber_name : chamber_list) {
    auto hits_on_a_chamber = Monitored::Collection(Form("hits_on_%s", chamber_name.Data()), tgcHitsMap[chamber_name], [](const TgcHit &m) {
	return m.channel;
      });
    fill(m_packageName, hits_on_a_chamber);
  }

  for (const auto &phimap : tgcHitPhiMap) {
    auto x = Monitored::Collection(Form("x_%s", phimap.first.Data()), tgcHitEtaMap[phimap.first], [](const int &m) {
	return m;
      });
    auto y = Monitored::Collection(Form("y_%s", phimap.first.Data()), phimap.second, [](const int &m) {
	return m;
      });
    fill(m_packageName, x, y);
  }
  for (const auto &phimap : tgcHitPhiMapGlobal) {
    auto x = Monitored::Scalar<int>(Form("lb_for_%s", phimap.first.Data()), GetEventInfo(ctx)->lumiBlock());
    auto y = Monitored::Collection(Form("%s", phimap.first.Data()), phimap.second, [](const int &m) {
	return m;
      });
    auto z = Monitored::Collection(Form("timing_for_%s", phimap.first.Data()), tgcHitTiming[phimap.first], [](const int &m) {
	return m;
      });
    fill(m_packageName, x, y, z);
  }

  tgcHits.clear();
  chamber_list.clear();
  for(auto& aa : tgcHitsMap)aa.second.clear();
  tgcHitsMap.clear();
  for(auto& aa : tgcHitPhiMap)aa.second.clear();
  tgcHitPhiMap.clear();
  for(auto& aa : tgcHitEtaMap)aa.second.clear();
  tgcHitEtaMap.clear();
  for(auto& aa : tgcHitPhiMapGlobal)aa.second.clear();
  tgcHitPhiMapGlobal.clear();
  for(auto& aa : tgcHitTiming)aa.second.clear();
  tgcHitTiming.clear();
  vec_bw24sectors.clear();
  vec_bw24sectors_wire.clear();
  vec_bw24sectors_strip.clear();
  vec_bwfulleta.clear();
  vec_bwfulleta_wire.clear();
  vec_bwfulleta_strip.clear();
  vec_bwtiming.clear();
  vec_bwtiming_wire.clear();
  vec_bwtiming_strip.clear();

  SG::ReadHandle < Muon::TgcCoinDataContainer > tgcCoinCurr(m_TgcCoinDataContainerCurrBCKey, ctx);
  if (!tgcCoinCurr.isValid()) {
    ATH_MSG_ERROR("evtStore() does not contain TgcCoinDataContainer with name " << m_TgcCoinDataContainerCurrBCKey);
    return StatusCode::FAILURE;
  }
  SG::ReadHandle < Muon::TgcCoinDataContainer > tgcCoinNext(m_TgcCoinDataContainerNextBCKey, ctx);
  if (!tgcCoinNext.isValid()) {
    ATH_MSG_ERROR("evtStore() does not contain TgcCoinDataContainer with name " << m_TgcCoinDataContainerNextBCKey);
    return StatusCode::FAILURE;
  }
  SG::ReadHandle < Muon::TgcCoinDataContainer > tgcCoinPrev(m_TgcCoinDataContainerPrevBCKey, ctx);
  if (!tgcCoinPrev.isValid()) {
    ATH_MSG_ERROR("evtStore() does not contain TgcCoinDataContainer with name " << m_TgcCoinDataContainerPrevBCKey);
    return StatusCode::FAILURE;
  }
  std::map<int, SG::ReadHandle<Muon::TgcCoinDataContainer> > tgcCoin;
  tgcCoin[0] = tgcCoinCurr;
  tgcCoin[+1] = tgcCoinNext;
  tgcCoin[-1] = tgcCoinPrev;
  std::vector < TgcTrig > tgcTrigs_SL;
  std::vector < TgcTrig > tgcTrigs_SL_Endcap;
  std::vector < TgcTrig > tgcTrigs_SL_Forward;
  std::vector < TgcTrig > tgcTrigs_HPT_Wire;
  std::vector < TgcTrig > tgcTrigs_HPT_Endcap_Wire;
  std::vector < TgcTrig > tgcTrigs_HPT_Forward_Wire;
  std::vector < TgcTrig > tgcTrigs_HPT_Strip;
  std::vector < TgcTrig > tgcTrigs_HPT_Endcap_Strip;
  std::vector < TgcTrig > tgcTrigs_HPT_Forward_Strip;
  std::vector < TgcTrig > tgcTrigs_LPT_Wire;
  std::vector < TgcTrig > tgcTrigs_LPT_Endcap_Wire;
  std::vector < TgcTrig > tgcTrigs_LPT_Forward_Wire;
  std::vector < TgcTrig > tgcTrigs_LPT_Strip;
  std::vector < TgcTrig > tgcTrigs_LPT_Endcap_Strip;
  std::vector < TgcTrig > tgcTrigs_LPT_Forward_Strip;
  int n_TgcCoin_detElementIsNull = 0;
  int n_TgcCoin_postOutPtrIsNull = 0;
  for (auto thisCoin : tgcCoin) {
    int bunch = thisCoin.first;
    for (const auto tgccnt : *(thisCoin.second)) {
      for (const auto data : *tgccnt) {

	if ( data->detectorElementOut() == nullptr ) n_TgcCoin_detElementIsNull++;
	if ( data->posOutPtr() == nullptr ) n_TgcCoin_postOutPtrIsNull++;
	if ( data->detectorElementOut() == nullptr ||
	     data->posOutPtr() == nullptr )continue; // to avoid FPE

	TgcTrig tgcTrig;
	tgcTrig.lb = GetEventInfo(ctx)->lumiBlock();
	const int type = data->type();
	const Amg::Vector3D &posIn = data->globalposIn();
	tgcTrig.x_In = posIn[0];
	tgcTrig.y_In = posIn[1];
	tgcTrig.z_In = posIn[2];
	const Amg::Vector3D &posOut = data->globalposOut();
	tgcTrig.x_Out = posOut[0];
	tgcTrig.y_Out = posOut[1];
	tgcTrig.z_Out = posOut[2];
	tgcTrig.eta = posOut.eta();
	tgcTrig.phi = posOut.phi();
	tgcTrig.width_In = data->widthIn();
	tgcTrig.width_Out = data->widthOut();
	if (type == Muon::TgcCoinData::TYPE_SL) {
	  const Amg::MatrixX &matrix = data->errMat();
	  tgcTrig.width_R = matrix(0, 0);
	  tgcTrig.width_Phi = matrix(1, 1);
	} else {
	  tgcTrig.width_R = 0.;
	  tgcTrig.width_Phi = 0.;
	}
	int etaout = 0;
	int etain = 0;
	const Identifier tcdidout = data->channelIdOut();
	if (tcdidout.is_valid()) {
	  etaout = std::abs(int(m_idHelperSvc->tgcIdHelper().stationEta(tcdidout)));
	}
	const Identifier tcdidin  = data->channelIdIn();
	if (tcdidin.is_valid()) {
	  etain  = std::abs(int(m_idHelperSvc->tgcIdHelper().stationEta(tcdidin)));
	}
	tgcTrig.etaout = etaout;
	tgcTrig.etain = etain;
	tgcTrig.isAside = data->isAside();
	tgcTrig.isForward = data->isForward();
	tgcTrig.isStrip = data->isStrip();
	tgcTrig.isInner = data->isInner();
	tgcTrig.isPositiveDeltaR = data->isPositiveDeltaR();
	tgcTrig.type = type;
	tgcTrig.trackletId = data->trackletId();
	tgcTrig.trackletIdStrip = data->trackletIdStrip();
	tgcTrig.sector = (data->isAside())?(data->phi()):(-data->phi());
	tgcTrig.roi = data->roi();
	tgcTrig.pt = data->pt();
	tgcTrig.delta = data->delta();
	tgcTrig.sub = data->sub();
	tgcTrig.veto = data->veto();
	tgcTrig.bunch = bunch;
	tgcTrig.inner = data->inner();
	if (type == Muon::TgcCoinData::TYPE_SL && !tgcTrig.isForward) {
	  tgcTrigs_SL_Endcap.push_back(tgcTrig);
	  tgcTrigs_SL.push_back(tgcTrig);
	}else if (type == Muon::TgcCoinData::TYPE_SL && tgcTrig.isForward) {
	  tgcTrigs_SL_Forward.push_back(tgcTrig);
	  tgcTrigs_SL.push_back(tgcTrig);
	}else if(type == Muon::TgcCoinData::TYPE_HIPT && !tgcTrig.isForward){
	  if(tgcTrig.isStrip){
	    tgcTrigs_HPT_Endcap_Strip.push_back(tgcTrig);
	    tgcTrigs_HPT_Strip.push_back(tgcTrig);
	  }else{
	    tgcTrigs_HPT_Endcap_Wire.push_back(tgcTrig);
	    tgcTrigs_HPT_Wire.push_back(tgcTrig);
	  }
	}else if(type == Muon::TgcCoinData::TYPE_HIPT && tgcTrig.isForward){
	  if(tgcTrig.isStrip){
	    tgcTrigs_HPT_Forward_Strip.push_back(tgcTrig);
	    tgcTrigs_HPT_Strip.push_back(tgcTrig);
	  }else{
	    tgcTrigs_HPT_Forward_Wire.push_back(tgcTrig);
	    tgcTrigs_HPT_Wire.push_back(tgcTrig);
	  }
	}else if(type == Muon::TgcCoinData::TYPE_TRACKLET && !tgcTrig.isForward){
	  if(tgcTrig.isStrip){
	    tgcTrigs_LPT_Endcap_Strip.push_back(tgcTrig);
	    tgcTrigs_LPT_Strip.push_back(tgcTrig);
	  }else{
	    tgcTrigs_LPT_Endcap_Wire.push_back(tgcTrig);
	    tgcTrigs_LPT_Wire.push_back(tgcTrig);
	  }
	}else if(type == Muon::TgcCoinData::TYPE_TRACKLET && tgcTrig.isForward){
	  if(tgcTrig.isStrip){
	    tgcTrigs_LPT_Forward_Strip.push_back(tgcTrig);
	    tgcTrigs_LPT_Strip.push_back(tgcTrig);
	  }else{
	    tgcTrigs_LPT_Forward_Wire.push_back(tgcTrig);
	    tgcTrigs_LPT_Wire.push_back(tgcTrig);
	  }
	}
      }
    }
  }

  auto mon_nTgcCoin_detElementIsNull = Monitored::Scalar<int>("nTgcCoinDetElementIsNull", n_TgcCoin_detElementIsNull);
  auto mon_nTgcCoin_postOutPtrIsNull = Monitored::Scalar<int>("nTgcCoinPostOutPtrIsNull", n_TgcCoin_postOutPtrIsNull);
  variables.push_back(mon_nTgcCoin_detElementIsNull);
  variables.push_back(mon_nTgcCoin_postOutPtrIsNull);

  fillTgcCoin(tgcTrigs_SL,"SL");
  fillTgcCoin(tgcTrigs_SL_Endcap,"SL_Endcap");
  fillTgcCoin(tgcTrigs_SL_Forward,"SL_Forward");

  fillTgcCoin(tgcTrigs_HPT_Wire,"HPT_Wire");
  fillTgcCoin(tgcTrigs_HPT_Endcap_Wire,"HPT_Endcap_Wire");
  fillTgcCoin(tgcTrigs_HPT_Forward_Wire,"HPT_Forward_Wire");

  fillTgcCoin(tgcTrigs_HPT_Strip,"HPT_Strip");
  fillTgcCoin(tgcTrigs_HPT_Endcap_Strip,"HPT_Endcap_Strip");
  fillTgcCoin(tgcTrigs_HPT_Forward_Strip,"HPT_Forward_Strip");

  fillTgcCoin(tgcTrigs_LPT_Wire,"LPT_Wire");
  fillTgcCoin(tgcTrigs_LPT_Endcap_Wire,"LPT_Endcap_Wire");
  fillTgcCoin(tgcTrigs_LPT_Forward_Wire,"LPT_Forward_Wire");

  fillTgcCoin(tgcTrigs_LPT_Strip,"LPT_Strip");
  fillTgcCoin(tgcTrigs_LPT_Endcap_Strip,"LPT_Endcap_Strip");
  fillTgcCoin(tgcTrigs_LPT_Forward_Strip,"LPT_Forward_Strip");

  fill(m_packageName, variables);
  variables.clear();
  for(auto& aa : mymuons)aa.clear();
  mymuons.clear();

  tgcCoin.clear();
  tgcTrigs_SL.clear();
  tgcTrigs_SL_Endcap.clear();
  tgcTrigs_SL_Forward.clear();
  tgcTrigs_HPT_Wire.clear();
  tgcTrigs_HPT_Endcap_Wire.clear();
  tgcTrigs_HPT_Forward_Wire.clear();
  tgcTrigs_HPT_Strip.clear();
  tgcTrigs_HPT_Endcap_Strip.clear();
  tgcTrigs_HPT_Forward_Strip.clear();
  tgcTrigs_LPT_Wire.clear();
  tgcTrigs_LPT_Endcap_Wire.clear();
  tgcTrigs_LPT_Forward_Wire.clear();
  tgcTrigs_LPT_Strip.clear();
  tgcTrigs_LPT_Endcap_Strip.clear();
  tgcTrigs_LPT_Forward_Strip.clear();

  return StatusCode::SUCCESS;
}

void TgcRawDataMonitorAlgorithm::fillTgcCoin(const std::vector<TgcTrig>& tgcTrigs, const std::string & type ) const {

  MonVariables variables;

  auto coin_lb = Monitored::Collection(type+"_coin_lb", tgcTrigs, [](const TgcTrig &m) {
      return m.lb;
    });
  variables.push_back(coin_lb);
  auto coin_eta = Monitored::Collection(type+"_coin_eta", tgcTrigs, [](const TgcTrig &m) {
      return m.eta;
    });
  variables.push_back(coin_eta);
  auto coin_phi = Monitored::Collection(type+"_coin_phi", tgcTrigs, [](const TgcTrig &m) {
      return m.phi + tgc_coin_phi_small_offset;
    });
  variables.push_back(coin_phi);
  auto coin_bunch = Monitored::Collection(type+"_coin_bunch", tgcTrigs, [](const TgcTrig &m) {
      return m.bunch;
    });
  variables.push_back(coin_bunch);
  auto coin_roi = Monitored::Collection(type+"_coin_roi", tgcTrigs, [](const TgcTrig &m) {
      return m.roi;
    });
  variables.push_back(coin_roi);
  auto coin_sector = Monitored::Collection(type+"_coin_sector", tgcTrigs, [](const TgcTrig &m) {
      return m.sector;
    });
  variables.push_back(coin_sector);
  auto coin_pt = Monitored::Collection(type+"_coin_pt", tgcTrigs, [](const TgcTrig &m) {
      return m.pt;
    });
  variables.push_back(coin_pt);
  auto coin_veto = Monitored::Collection(type+"_coin_veto", tgcTrigs, [](const TgcTrig &m) {
      return m.veto;
    });
  variables.push_back(coin_veto);
  auto coin_veto_sector = Monitored::Collection(type+"_coin_veto_sector", tgcTrigs, [](const TgcTrig &m) {
      return (m.veto==1)?(m.sector):(-1);
    });
  variables.push_back(coin_veto_sector);
  auto coin_veto_roi = Monitored::Collection(type+"_coin_veto_roi", tgcTrigs, [](const TgcTrig &m) {
      return (m.veto==1)?(m.roi):(-1);
    });
  variables.push_back(coin_veto_roi);
  auto coin_isPositiveDeltaR = Monitored::Collection(type+"_coin_isPositiveDeltaR", tgcTrigs, [](const TgcTrig &m) {
      return m.isPositiveDeltaR;
    });
  variables.push_back(coin_isPositiveDeltaR);

  auto coin_cutmask_pt1 = Monitored::Collection(type+"_coin_cutmask_pt1", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==1);
    });
  variables.push_back(coin_cutmask_pt1);
  auto coin_cutmask_pt2 = Monitored::Collection(type+"_coin_cutmask_pt2", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==2);
    });
  variables.push_back(coin_cutmask_pt2);
  auto coin_cutmask_pt3 = Monitored::Collection(type+"_coin_cutmask_pt3", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==3);
    });
  variables.push_back(coin_cutmask_pt3);
  auto coin_cutmask_pt4 = Monitored::Collection(type+"_coin_cutmask_pt4", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==4);
    });
  variables.push_back(coin_cutmask_pt4);
  auto coin_cutmask_pt5 = Monitored::Collection(type+"_coin_cutmask_pt5", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==5);
    });
  variables.push_back(coin_cutmask_pt5);
  auto coin_cutmask_pt6 = Monitored::Collection(type+"_coin_cutmask_pt6", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==6);
    });
  variables.push_back(coin_cutmask_pt6);
  auto coin_cutmask_pt7 = Monitored::Collection(type+"_coin_cutmask_pt7", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==7);
    });
  variables.push_back(coin_cutmask_pt7);
  auto coin_cutmask_pt8 = Monitored::Collection(type+"_coin_cutmask_pt8", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==8);
    });
  variables.push_back(coin_cutmask_pt8);
  auto coin_cutmask_pt9 = Monitored::Collection(type+"_coin_cutmask_pt9", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==9);
    });
  variables.push_back(coin_cutmask_pt9);
  auto coin_cutmask_pt10 = Monitored::Collection(type+"_coin_cutmask_pt10", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==10);
    });
  variables.push_back(coin_cutmask_pt10);
  auto coin_cutmask_pt11 = Monitored::Collection(type+"_coin_cutmask_pt11", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==11);
    });
  variables.push_back(coin_cutmask_pt11);
  auto coin_cutmask_pt12 = Monitored::Collection(type+"_coin_cutmask_pt12", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==12);
    });
  variables.push_back(coin_cutmask_pt12);
  auto coin_cutmask_pt13 = Monitored::Collection(type+"_coin_cutmask_pt13", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==13);
    });
  variables.push_back(coin_cutmask_pt13);
  auto coin_cutmask_pt14 = Monitored::Collection(type+"_coin_cutmask_pt14", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==14);
    });
  variables.push_back(coin_cutmask_pt14);
  auto coin_cutmask_pt15 = Monitored::Collection(type+"_coin_cutmask_pt15", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==15);
    });
  variables.push_back(coin_cutmask_pt15);

  fill(m_packageName, variables);
  variables.clear();
}
///////////////////////////////////////////////////////////////
void TgcRawDataMonitorAlgorithm::extrapolate(const xAOD::Muon *muon, MyMuon &mymuon) const {
    const xAOD::TrackParticle *track = muon->trackParticle(xAOD::Muon::CombinedTrackParticle);
    if (!track) return;
    mymuon.extPos.reserve(m_extZposition.size());
    mymuon.extVec.reserve(m_extZposition.size());
    for (const auto &z : m_extZposition) {
        Amg::Vector2D etaDeta(0, 0);
        Amg::Vector2D phiDphi(0, 0);
        Amg::Vector3D mom(0, 0, 0);
        Amg::Vector3D extrapolateTo(0, 0, z);
        if (extrapolate(track, extrapolateTo, TGC, etaDeta, phiDphi, mom)) {
            double pt = extrapolateTo.z() / std::sinh(etaDeta[0]);
            TVector3 vec, pos;
            pos.SetPtEtaPhi(pt, etaDeta[0], phiDphi[0]);
            vec.SetXYZ(mom.x(), mom.y(), mom.z());
            mymuon.extPosZ.push_back(z);
            mymuon.extPos.push_back(pos);
            mymuon.extVec.push_back(vec);
        }
    }
}
///////////////////////////////////////////////////////////////
/* track extrapolator tool */
bool TgcRawDataMonitorAlgorithm::extrapolate(const xAOD::TrackParticle *trackParticle, const Amg::Vector3D &pos, const int detector, Amg::Vector2D &eta, Amg::Vector2D &phi, Amg::Vector3D &mom) const {
    if (!trackParticle) {
        return false;
    }
    const Trk::Track *track = trackParticle->track();
    if (!track) {
        return false;
    }
    const DataVector<const Trk::TrackStateOnSurface> *trackOnSurf = track->trackStateOnSurfaces();
    if (!trackOnSurf) {
        return false;
    }
    std::vector<double> vExtEta, vExtPhi, vExtPx, vExtPy, vExtPz, vExtDeltaEta, vExtDeltaPhi, vExtDistance;
    int counter = 0;
    for (auto it : *trackOnSurf) {
        if (!it->type(Trk::TrackStateOnSurface::Measurement)) {
            continue;
        }
        std::unique_ptr<const Trk::TrackParameters> point = nullptr;
        Amg::Vector2D distance(-1.e30, -1.e30);
        if (detector == TgcRawDataMonitorAlgorithm::TGC) {
            point = extrapolateToTGC(it, pos, distance);
        } else if (detector == TgcRawDataMonitorAlgorithm::RPC) {
            point = extrapolateToRPC(it, pos, distance);
        } else {
            continue;
        }
        if (!point) {
            continue;
        }
        vExtEta.push_back(point->position().eta());
        vExtPhi.push_back(point->position().phi());
        vExtPx.push_back(point->momentum().x());
        vExtPy.push_back(point->momentum().y());
        vExtPz.push_back(point->momentum().z());
        vExtDistance.push_back(distance.y());
        counter++;
    }
    if (counter == 0) {
        return false;
    }
    double minimum = 1.e30;
    int number = -1;
    for (int ii = 0; ii < counter; ii++) {
        if (minimum > vExtDistance.at(ii)) {
            minimum = vExtDistance.at(ii);
            number = ii;
        }
    }
    if (number == -1) {
        return false;
    }
    eta[0] = vExtEta.at(number);
    phi[0] = vExtPhi.at(number);
    eta[1] = getError(vExtEta);
    phi[1] = getError(vExtPhi);
    mom[0] = vExtPx.at(number);
    mom[1] = vExtPy.at(number);
    mom[2] = vExtPz.at(number);
    return true;
}
///////////////////////////////////////////////////////////////
std::unique_ptr<const Trk::TrackParameters> TgcRawDataMonitorAlgorithm::extrapolateToTGC(const Trk::TrackStateOnSurface *tsos, const Amg::Vector3D &pos, Amg::Vector2D &distance) const {
    const Trk::TrackParameters *track = tsos->trackParameters();
    if (!track || dynamic_cast<const Trk::AtaStraightLine*>(track) == 0) {
        return nullptr;
    }
    double targetZ = pos.z();
    double trackZ = track->position().z();
    if (std::abs(trackZ) < std::abs(targetZ) - 2000. || std::abs(trackZ) > std::abs(targetZ) + 2000.) {
        return nullptr;
    }
    auto matrix = std::make_unique<Amg::Transform3D>();
    matrix->setIdentity();
    matrix->translation().z() = targetZ;
    auto disc = std::make_unique < Trk::DiscSurface > (*matrix, m_endcapPivotPlaneMinimumRadius.value(), m_endcapPivotPlaneMaximumRadius.value());
    distance[0] = trackZ;
    distance[1] = std::abs(trackZ - targetZ);
    const bool boundaryCheck = true;
    auto param = std::unique_ptr<const Trk::TrackParameters>(m_extrapolator->extrapolate(*track, *disc, Trk::anyDirection, boundaryCheck, Trk::muon));
    if (!param) {
        return nullptr;
    }
    // We want disc
    if (param->surfaceType() != Trk::SurfaceType::Disc) {
        return nullptr;
    }
    return param;
}
///////////////////////////////////////////////////////////////
std::unique_ptr<const Trk::TrackParameters> TgcRawDataMonitorAlgorithm::extrapolateToRPC(const Trk::TrackStateOnSurface *tsos, const Amg::Vector3D &pos, Amg::Vector2D &distance) const {
    const Trk::TrackParameters *track = tsos->trackParameters();
    if (!track || dynamic_cast<const Trk::AtaStraightLine*>(track) == 0) {
        return nullptr;
    }
    double radius = pos.perp();
    double trackRadius = track->position().perp();
    if (trackRadius < radius - 2000. || trackRadius > radius + 2000.) {
        return nullptr;
    }
    Amg::Transform3D matrix;
    matrix.setIdentity();
    auto cylinder = std::make_unique < Trk::CylinderSurface > (matrix, radius, m_barrelPivotPlaneHalfLength.value());

    distance[0] = trackRadius;
    distance[1] = trackRadius - radius;
    const bool boundaryCheck = true;
    auto param = std::unique_ptr<const Trk::TrackParameters>(m_extrapolator->extrapolate(*track, *cylinder, Trk::anyDirection, boundaryCheck, Trk::muon));

    if (!param) {
        return nullptr;
    }
    //It has to be cylinder
    if (param->surfaceType() != Trk::SurfaceType::Cylinder) {
        return nullptr;
    }
    return param;
}
///////////////////////////////////////////////////////////////
double TgcRawDataMonitorAlgorithm::getError(const std::vector<double> &inputVec) const {
    /// Assign dummy value of 1 as error
    if (inputVec.empty()) return 1.;

    const int nSize = inputVec.size();
    double sum = 0;
    double sum2 = 0;
    for (int ii = 0; ii < nSize; ii++) {
        sum += inputVec.at(ii);
    }
    const double mean = sum / nSize;
    for (int jj = 0; jj < nSize; jj++) {
        sum2 += std::pow((inputVec.at(jj) - mean), 2);
    }
    const double stdDev = std::sqrt(sum2 / nSize);
    return stdDev;
}

