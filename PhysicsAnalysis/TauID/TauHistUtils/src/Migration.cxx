/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/Migration.h"

namespace Tau{

    Migration::Migration(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):PlotBase(pParent, sDir),
												m_sTauJetContainerName(sTauJetContainerName){	
    }
    
    
    void Migration::initializePlots(){
	m_migration_tauRec = Book1D("tauRec_migration",m_sTauJetContainerName + " TauRec migration",DECAYSIZE,0,DECAYSIZE);
	m_migration_eflow = Book1D("eflow_migration",m_sTauJetContainerName + " eflow migration",DECAYSIZE,0,DECAYSIZE);
	m_migration_cellBased = Book1D("cellBased_migration",m_sTauJetContainerName + " cellBased migration",DECAYSIZE,0,DECAYSIZE);
	for(int i=1; i<= DECAYSIZE;i++){
	    m_migration_tauRec->GetXaxis()->SetBinLabel(i,m_lable[i-1]);
	    m_migration_eflow->GetXaxis()->SetBinLabel(i,m_lable[i-1]);
	    m_migration_cellBased->GetXaxis()->SetBinLabel(i,m_lable[i-1]);
	}
    }
    
    void Migration::fill(const xAOD::TauJet& thisTau,int nProng, int nNeu) {
	//RecTau
	
	
	
	
	/*+++++++++++++++++++++++++++++++++++++++++++++++++
	  +++++++++++++++++++EFLOW_REC+++++++++++++++++++++
	  +++++++++++++++++++++++++++++++++++++++++++++++++*/
	
	//Charged Pions
	std::vector< ElementLink< xAOD::PFOContainer > > eflowRec_chargedPFO = thisTau.eflowRec_Charged_PFOLinks();
	int efP = eflowRec_chargedPFO.size();
	//Neutral Pions
	std::vector< ElementLink< xAOD::PFOContainer > > eflowRec_neutralPFO = thisTau.eflowRec_Pi0_PFOLinks();
	int efN = eflowRec_neutralPFO.size();
	decayModeFill(nProng,nNeu,efP,efN,m_migration_eflow);
	/*+++++++++++++++++++++++++++++++++++++++++++++++++
	  +++++++++++++++++++CellBased+++++++++++++++++++++
	  +++++++++++++++++++++++++++++++++++++++++++++++++*/
	
	//Charged Pions
	std::vector< ElementLink< xAOD::PFOContainer > > cellBased_chargedPFO = thisTau.cellBased_Charged_PFOLinks();
	int cellP = cellBased_chargedPFO.size();

	
	//Neutral Pions
	int nPi0_tau = 0;
	std::vector< ElementLink< xAOD::PFOContainer > > cellBased_neutralPFO = thisTau.cellBased_Neutral_PFOLinks();
	std::vector< ElementLink< xAOD::PFOContainer > >::iterator first_cellBased_neutralPFO = cellBased_neutralPFO.begin();
	std::vector< ElementLink< xAOD::PFOContainer > >::iterator last_cellBased_neutralPFO = cellBased_neutralPFO.end();
	for (; first_cellBased_neutralPFO != last_cellBased_neutralPFO; ++first_cellBased_neutralPFO){
	    ElementLink<xAOD::PFOContainer> thisLink = *first_cellBased_neutralPFO;
	    const xAOD::PFO* thisPFO = *thisLink;
	    int myNPi0Proto = 0;
	    if(thisPFO->attribute(xAOD::PFODetails::nPi0Proto, myNPi0Proto)) nPi0_tau+=myNPi0Proto;
	    
	} 
	decayModeFill(nProng,nNeu,cellP,nPi0_tau,m_migration_cellBased);	
	
	
	
	/*+++++++++++++++++++++++++++++++++++++++++++++++++
	  +++++++++++++++++++PANTAU+++++++++++++++++++++
	  +++++++++++++++++++++++++++++++++++++++++++++++++*/
	
    }
    void Migration::decayModeFill(int trueP,int trueN,int recP, int recN,TH1 *histo){
	if(recP == 1 && recN== 0){
	    if(trueP == 1 && trueN== 0) histo->Fill(t10r10 + 0.5);
	    if(trueP == 1 && trueN== 1) histo->Fill(t11r10 + 0.5);
	    if(trueP == 1 && trueN > 1) histo->Fill(t1xr10 + 0.5);
	    if(trueP == 3) histo->Fill(t3r1 + 0.5);
	}
	if(recP == 1 && recN== 1){
	    if(trueP == 1 && trueN== 0) histo->Fill(t10r11 + 0.5);
	    if(trueP == 1 && trueN== 1) histo->Fill(t11r11 + 0.5);
	    if(trueP == 1 && trueN > 1) histo->Fill(t1xr11 + 0.5);
	    if(trueP == 3 ) histo->Fill(t3r1 + 0.5);
	}
	if(recP == 1 && recN > 1){
	    if(trueP == 1 && trueN== 0) histo->Fill(t10r1x + 0.5);
	    if(trueP == 1 && trueN== 1) histo->Fill(t11r1x + 0.5);
	    if(trueP == 1 && trueN > 1) histo->Fill(t1xr1x + 0.5);
	    if(trueP == 3 ) histo->Fill(t3r1 + 0.5);
	}
	if(recP == 3 && recN== 0){
	    if(trueP == 1 ) histo->Fill(t1r3 + 0.5);
	    if(trueP == 3 && trueN== 0) histo->Fill(t30r30 + 0.5);
	    if(trueP == 3 && trueN>= 1) histo->Fill(t3xr30 + 0.5);
	}
	if(recP == 3 && recN>= 1){
	    if(trueP == 1 ) histo->Fill(t1r3 + 0.5);
	    if(trueP == 3 && trueN== 0) histo->Fill(t30r3x + 0.5);
	    if(trueP == 3 && trueN>= 1) histo->Fill(t3xr3x + 0.5);
	}
	return;
    }
}
