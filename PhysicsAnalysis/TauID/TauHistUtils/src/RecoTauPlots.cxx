/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/RecoTauPlots.h"

namespace Tau{

	RecoTauPlots::RecoTauPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):PlotBase(pParent, sDir),
	m_oParamPlots(this, "", sTauJetContainerName),
	m_sTauJetContainerName(sTauJetContainerName){	
	}
	
	RecoTauPlots::~RecoTauPlots()
	{
	}
  void RecoTauPlots::initializePlots(){
    
    //  m_oParamPlots.initialize();


    /*+++++++++++++++++++++++++++++++++++++++++++++++++
      +++++++++++++++++++EFLOW_REC+++++++++++++++++++++
      +++++++++++++++++++++++++++++++++++++++++++++++++*/


	// m_eflowRec_charPFO_Pt      = Book1D("eflowRec_PFO_c_Pt",m_sTauJetContainerName + " Charged PFO pt; pt; # PFO",200,0.,200000.);
	// m_eflowRec_charPFO_Eta      = Book1D("eflowRec_PFO_c_Eta",m_sTauJetContainerName + " Charged PFO eta; eta; # PFO",128,-3.2,3.2);
	// m_eflowRec_charPFO_Phi      = Book1D("eflowRec_PFO_c_Phi",m_sTauJetContainerName + " Charged PFO phi; phi; # PFO",128,-3.2,3.2);
	// m_eflowRec_charPFO_E      = Book1D("eflowRec_PFO_c_E",m_sTauJetContainerName + " Charged PFO E; E; # PFO",200,0.,200000.);
	// m_eflowRec_neuPFO_Pt      = Book1D("eflowRec_PFO_n_Pt",m_sTauJetContainerName + " Neutral PFO pt; pt; # PFO",200,0.,200000.);
	// m_eflowRec_neuPFO_Eta      = Book1D("eflowRec_PFO_n_Eta",m_sTauJetContainerName + " Neutral PFO eta; eta; # PFO",128,-3.2,3.2);
	// m_eflowRec_neuPFO_Phi      = Book1D("eflowRec_PFO_n_Phi",m_sTauJetContainerName + " Neutral PFO phi; phi; # PFO",128,-3.2,3.2);
	// m_eflowRec_neuPFO_E      = Book1D("eflowRec_PFO_n_E",m_sTauJetContainerName + " Neutral PFO E; E; # PFO",200,0.,200000.);
	// m_eflowRec_neuPFO_bdtPi0Score = Book1D("eflowRec_PFO_n_bdtPi0Score",m_sTauJetContainerName + " Neutral PFO BDT Pi0 Score; Score; # PFO",60,-.6,.6);


    /*+++++++++++++++++++++++++++++++++++++++++++++++++
      +++++++++++++++++++CellBased+++++++++++++++++++++
      +++++++++++++++++++++++++++++++++++++++++++++++++*/

	m_cellBased_charPFO_Pt      = Book1D("cellBased_PFO_c_Pt",m_sTauJetContainerName + " Charged PFO pt; pt; # PFO",200,0.,200000.);
	m_cellBased_charPFO_Eta      = Book1D("cellBased_PFO_c_Eta",m_sTauJetContainerName + " Charged PFO eta; eta; # PFO",128,-3.2,3.2);
	m_cellBased_charPFO_Phi      = Book1D("cellBased_PFO_c_Phi",m_sTauJetContainerName + " Charged PFO phi; phi; # PFO",128,-3.2,3.2);
	m_cellBased_charPFO_E      = Book1D("cellBased_PFO_c_E",m_sTauJetContainerName + " Charged PFO E; E; # PFO",200,0.,200000.);
	m_cellBased_neuPFO_Pt      = Book1D("cellBased_PFO_n_Pt",m_sTauJetContainerName + " Neutral PFO pt; pt; # PFO",200,0.,200000.);
	m_cellBased_neuPFO_Eta      = Book1D("cellBased_PFO_n_Eta",m_sTauJetContainerName + " Neutral PFO eta; eta; # PFO",128,-3.2,3.2);
	m_cellBased_neuPFO_Phi      = Book1D("cellBased_PFO_n_Phi",m_sTauJetContainerName + " Neutral PFO phi; phi; # PFO",128,-3.2,3.2);
	m_cellBased_neuPFO_E      = Book1D("cellBased_PFO_n_E",m_sTauJetContainerName + " Neutral PFO E; E; # PFO",200,0.,200000.);
	m_cellBased_neuPFO_bdtPi0Score = Book1D("cellBased_PFO_n_bdtPi0Score",m_sTauJetContainerName + " Neutral PFO BDT Pi0 Score; Score; # PFO",60,-1.0,1.0);


    /*+++++++++++++++++++++++++++++++++++++++++++++++++
      +++++++++++++++++++PANTAU++++++++++++++++++++++++
      +++++++++++++++++++++++++++++++++++++++++++++++++*/
	m_pantau_CellBasedInput_isPanTauCandidate   = Book1D("pantau_CellBasedInput_isPanTauCandidate",m_sTauJetContainerName +"pantau_CellBasedInput_isPanTauCandidate", 2 ,0 ,2);
	m_pantau_CellBasedInput_DecayMode   = Book1D("pantau_CellBasedInput_DecayMode",m_sTauJetContainerName +"pantau_CellBasedInput_DecayMode", 6, -0.5, 5.5);
	m_pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n = Book1D("pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n",m_sTauJetContainerName + "pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n",30,-1.,1.);
	m_pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn = Book1D("pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn",m_sTauJetContainerName + "pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn",30,-1.,1.);
	m_pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn = Book1D("pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn",m_sTauJetContainerName + "pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn",30,-1.,1.);
	m_pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts = Book1D("pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts",m_sTauJetContainerName + "pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts",10.,-0.5,9.5);
	m_pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt = Book1D("pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt",m_sTauJetContainerName + "pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt",30,0.,6.);
	m_pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts = Book1D("pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts",m_sTauJetContainerName + "pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts",30,0.,0.6);
	m_pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM = Book1D("pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM",m_sTauJetContainerName + "pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM",30,0.,6.);
	m_pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1 = Book1D("pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1",m_sTauJetContainerName + "pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1",30,-1.5,1.5);
	m_pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2 = Book1D("pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2",m_sTauJetContainerName + "pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2",30,-1.5,1.5);
	m_pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts = Book1D("pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts",m_sTauJetContainerName + "pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts",30,0.,1.);
	m_pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts = Book1D("pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts",m_sTauJetContainerName + "pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts",30,-0.5,1.2);
	m_pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed = Book1D("pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed",m_sTauJetContainerName + "pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed",10,-0.5,9.5);
	m_pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged = Book1D("pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged",m_sTauJetContainerName + "pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged",30,0.,0.6);

	m_pantau_eflowRecInput_isPanTauCandidate = Book1D("pantau_eflowRecInput_isPanTauCandidate",m_sTauJetContainerName + "pantau_eflowRecInput_isPanTauCandidate",2,0,2);
	m_pantau_eflowRecInput_DecayMode = Book1D("pantau_eflowRecInput_DecayMode",m_sTauJetContainerName + "pantau_eflowRecInput_DecayMode",6,-0.5,5.5);
	m_pantau_eflowRecInput_BDTValue_1p0n_vs_1p1n = Book1D("pantau_eflowRecInput_BDTValue_1p0n_vs_1p1n",m_sTauJetContainerName + "pantau_eflowRecInput_BDTValue_1p0n_vs_1p1n",30,-1.,1.);
	m_pantau_eflowRecInput_BDTValue_1p1n_vs_1pXn = Book1D("pantau_eflowRecInput_BDTValue_1p1n_vs_1pXn",m_sTauJetContainerName + "pantau_eflowRecInput_BDTValue_1p1n_vs_1pXn",30,-1.,1.);
	m_pantau_eflowRecInput_BDTValue_3p0n_vs_3pXn = Book1D("pantau_eflowRecInput_BDTValue_3p0n_vs_3pXn",m_sTauJetContainerName + "pantau_eflowRecInput_BDTValue_3p0n_vs_3pXn",30,-1.,1.);
	m_pantau_eflowRecInput_BDTVar_Basic_NPi0NeutConsts = Book1D("pantau_eflowRecInput_BDTVar_Basic_NPi0NeutConsts",m_sTauJetContainerName + "pantau_eflowRecInput_BDTVar_Basic_NPi0NeutConsts",10.,-0.5,9.5);
	m_pantau_eflowRecInput_BDTVar_Basic_NNeutralConsts = Book1D("pantau_eflowRecInput_BDTVar_Basic_NNeutralConsts",m_sTauJetContainerName + "pantau_eflowRecInput_BDTVar_Basic_NNeutralConsts",10.,-0.5,9.5);
	m_pantau_eflowRecInput_BDTVar_Charged_Ratio_EtOverEtAllConsts = Book1D("pantau_eflowRecInput_BDTVar_Charged_Ratio_EtOverEtAllConsts",m_sTauJetContainerName + "pantau_eflowRecInput_BDTVar_Charged_Ratio_EtOverEtAllConsts",30,0.,1.);
	m_pantau_eflowRecInput_BDTVar_Neutral_HLV_SumM = Book1D("pantau_eflowRecInput_BDTVar_Neutral_HLV_SumM",m_sTauJetContainerName + "pantau_eflowRecInput_BDTVar_Neutral_HLV_SumM",30,0.,6.);
	m_pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_EtSort_1 = Book1D("pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_EtSort_1",m_sTauJetContainerName + "pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_EtSort_1",30,-1.5,1.5);
	m_pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2 = Book1D("pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2",m_sTauJetContainerName + "pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2",30,-1.5,1.5);
	m_pantau_eflowRecInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts = Book1D("pantau_eflowRecInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts",m_sTauJetContainerName + "pantau_eflowRecInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts",30,-0.5,1.2);
	m_pantau_eflowRecInput_BDTVar_Neutral_Mean_DRToLeading_WrtEtAllConsts = Book1D("pantau_eflowRecInput_BDTVar_Neutral_Mean_DRToLeading_WrtEtAllConsts",m_sTauJetContainerName + "pantau_eflowRecInput_BDTVar_Neutral_Mean_DRToLeading_WrtEtAllConsts",30,-0.5,1.2);
	m_pantau_eflowRecInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged = Book1D("pantau_eflowRecInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged",m_sTauJetContainerName + "pantau_eflowRecInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged",30,0.,0.6);
  }
    
    void RecoTauPlots::fill(const xAOD::TauJet& thisTau) {

	//Access Particle Flow Object

	/*+++++++++++++++++++++++++++++++++++++++++++++++++
      +++++++++++++++++++EFLOW_REC+++++++++++++++++++++
      +++++++++++++++++++++++++++++++++++++++++++++++++*/

    //Charged Pions
    // std::vector< ElementLink< xAOD::PFOContainer > > eflowRec_chargedPFO = thisTau.eflowRec_Charged_PFOLinks();
    // std::vector< ElementLink< xAOD::PFOContainer > >::iterator first_eflowRec_chargedPFO = eflowRec_chargedPFO.begin();
    // std::vector< ElementLink< xAOD::PFOContainer > >::iterator last_eflowRec_chargedPFO = eflowRec_chargedPFO.end();

    // for (; first_eflowRec_chargedPFO != last_eflowRec_chargedPFO; ++first_eflowRec_chargedPFO){
    //   ElementLink<xAOD::PFOContainer> thisLink = *first_eflowRec_chargedPFO;
    //   const xAOD::PFO* thisPFO = *thisLink;
    //   m_eflowRec_charPFO_Pt->Fill( thisPFO->pt()); 
    //   m_eflowRec_charPFO_Eta->Fill( thisPFO->eta()); 
    //   m_eflowRec_charPFO_Phi->Fill( thisPFO->phi()); 
    //   m_eflowRec_charPFO_E->Fill( thisPFO->e());  
    // } 
    
    // //Neutral Pions
    // std::vector< ElementLink< xAOD::PFOContainer > > eflowRec_neutralPFO = thisTau.eflowRec_Neutral_PFOLinks();
    // std::vector< ElementLink< xAOD::PFOContainer > >::iterator first_eflowRec_neutralPFO = eflowRec_neutralPFO.begin();
    // std::vector< ElementLink< xAOD::PFOContainer > >::iterator last_eflowRec_neutralPFO = eflowRec_neutralPFO.end();
    // for (; first_eflowRec_neutralPFO != last_eflowRec_neutralPFO; ++first_eflowRec_neutralPFO){
    //   ElementLink<xAOD::PFOContainer> thisLink = *first_eflowRec_neutralPFO;
    //   const xAOD::PFO* thisPFO = *thisLink;
    //   m_eflowRec_neuPFO_Pt->Fill( thisPFO->pt()); 
    //   m_eflowRec_neuPFO_Eta->Fill( thisPFO->eta()); 
    //   m_eflowRec_neuPFO_Phi->Fill( thisPFO->phi()); 
    //   m_eflowRec_neuPFO_E->Fill( thisPFO->e());  
    //   m_eflowRec_neuPFO_bdtPi0Score->Fill(thisPFO->bdtPi0Score());
    // } 

    /*+++++++++++++++++++++++++++++++++++++++++++++++++
      +++++++++++++++++++CellBased+++++++++++++++++++++
      +++++++++++++++++++++++++++++++++++++++++++++++++*/

    //Charged Pions
    std::vector< ElementLink< xAOD::PFOContainer > > cellBased_chargedPFO = thisTau.protoChargedPFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > >::iterator first_cellBased_chargedPFO = cellBased_chargedPFO.begin();
    std::vector< ElementLink< xAOD::PFOContainer > >::iterator last_cellBased_chargedPFO = cellBased_chargedPFO.end();

    for (; first_cellBased_chargedPFO != last_cellBased_chargedPFO; ++first_cellBased_chargedPFO){
      ElementLink<xAOD::PFOContainer> thisLink = *first_cellBased_chargedPFO;
      const xAOD::PFO* thisPFO = *thisLink;
      m_cellBased_charPFO_Pt->Fill( thisPFO->pt()); 
      m_cellBased_charPFO_Eta->Fill( thisPFO->eta()); 
      m_cellBased_charPFO_Phi->Fill( thisPFO->phi()); 
      m_cellBased_charPFO_E->Fill( thisPFO->e());  
    } 
    
    //Neutral Pions
    std::vector< ElementLink< xAOD::PFOContainer > > cellBased_neutralPFO = thisTau.protoNeutralPFOLinks();
    std::vector< ElementLink< xAOD::PFOContainer > >::iterator first_cellBased_neutralPFO = cellBased_neutralPFO.begin();
    std::vector< ElementLink< xAOD::PFOContainer > >::iterator last_cellBased_neutralPFO = cellBased_neutralPFO.end();
    for (; first_cellBased_neutralPFO != last_cellBased_neutralPFO; ++first_cellBased_neutralPFO){
      ElementLink<xAOD::PFOContainer> thisLink = *first_cellBased_neutralPFO;
      const xAOD::PFO* thisPFO = *thisLink;
      m_cellBased_neuPFO_Pt->Fill( thisPFO->pt()); 
      m_cellBased_neuPFO_Eta->Fill( thisPFO->eta()); 
      m_cellBased_neuPFO_Phi->Fill( thisPFO->phi()); 
      m_cellBased_neuPFO_E->Fill( thisPFO->e());  
      m_cellBased_neuPFO_bdtPi0Score->Fill(thisPFO->bdtPi0Score());
    } 




    /*+++++++++++++++++++++++++++++++++++++++++++++++++
      +++++++++++++++++++PANTAU+++++++++++++++++++++
      +++++++++++++++++++++++++++++++++++++++++++++++++*/

    int panTauIntiger = 0;
    float panTauFloat = 0;
    
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_isPanTauCandidate,panTauIntiger);
    m_pantau_CellBasedInput_isPanTauCandidate->Fill(panTauIntiger);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_DecayMode,panTauIntiger);
    m_pantau_CellBasedInput_DecayMode->Fill(panTauIntiger);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n,panTauFloat);
    m_pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn,panTauFloat);
    m_pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn,panTauFloat);
    m_pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts,panTauIntiger);
    m_pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts->Fill(panTauIntiger);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt,panTauFloat);
    m_pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts,panTauFloat);
    m_pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM,panTauFloat);
    m_pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1,panTauFloat);
    m_pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2,panTauFloat);
    m_pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts,panTauFloat);
    m_pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts,panTauFloat);
    m_pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed,panTauFloat);
    m_pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged,panTauFloat);
    m_pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_isPanTauCandidate,panTauIntiger);
    m_pantau_eflowRecInput_isPanTauCandidate->Fill(panTauIntiger);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_DecayMode,panTauIntiger);
    m_pantau_eflowRecInput_DecayMode->Fill(panTauIntiger);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTValue_1p0n_vs_1p1n,panTauFloat);
    m_pantau_eflowRecInput_BDTValue_1p0n_vs_1p1n->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTValue_1p1n_vs_1pXn,panTauFloat);
    m_pantau_eflowRecInput_BDTValue_1p1n_vs_1pXn->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTValue_3p0n_vs_3pXn,panTauFloat);
    m_pantau_eflowRecInput_BDTValue_3p0n_vs_3pXn->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTVar_Basic_NPi0NeutConsts,panTauIntiger);
    m_pantau_eflowRecInput_BDTVar_Basic_NPi0NeutConsts->Fill(panTauIntiger);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTVar_Basic_NNeutralConsts,panTauIntiger);
    m_pantau_eflowRecInput_BDTVar_Basic_NNeutralConsts->Fill(panTauIntiger);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTVar_Charged_Ratio_EtOverEtAllConsts,panTauFloat);
    m_pantau_eflowRecInput_BDTVar_Charged_Ratio_EtOverEtAllConsts->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTVar_Neutral_HLV_SumM,panTauFloat);
    m_pantau_eflowRecInput_BDTVar_Neutral_HLV_SumM->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_EtSort_1,panTauFloat);
    m_pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_EtSort_1->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2,panTauFloat);
    m_pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts,panTauFloat);
    m_pantau_eflowRecInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTVar_Neutral_Mean_DRToLeading_WrtEtAllConsts,panTauFloat);
    m_pantau_eflowRecInput_BDTVar_Neutral_Mean_DRToLeading_WrtEtAllConsts->Fill(panTauFloat);
    thisTau.panTauDetail( xAOD::TauJetParameters::PanTauDetails::pantau_eflowRecInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged,panTauFloat);
    m_pantau_eflowRecInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged->Fill(panTauFloat);

 
    m_oParamPlots.fill(thisTau);
  }

}
