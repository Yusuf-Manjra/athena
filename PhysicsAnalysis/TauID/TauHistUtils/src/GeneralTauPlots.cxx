/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TauHistUtils/GeneralTauPlots.h"

namespace Tau{

	GeneralTauPlots::GeneralTauPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):PlotBase(pParent, sDir),
	m_oParamPlots(this, "", sTauJetContainerName),
	m_sTauJetContainerName(sTauJetContainerName){	
	}
	GeneralTauPlots::~GeneralTauPlots()
	{
	}

void GeneralTauPlots::initializePlots(){

  //  m_oParamPlots.initialize();
	m_tauCharge      = Book1D("Charge",m_sTauJetContainerName + " Tau charge; charge; # Taus",7,-3.,4.);
	m_tauNCoreTracks = Book1D("NCoreTracks",m_sTauJetContainerName + " Tau n core tracks; nCore; # Taus",10,0.,10.);
	m_tauNWideTracks = Book1D("NWideTracks",m_sTauJetContainerName + " Tau n wide tracks; nWide; # Taus",10,0.,10.);
	m_id_BDTJetScore      = Book1D("id_BDTJetScore",m_sTauJetContainerName + " BDTJetScore ; BDTJetScore; # Tau",10,0.,1.05);
	m_pt_jetBDTloose      = Book1D("Pt_jetBDTloose",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,300.);
	m_pt_jetBDTmed        = Book1D("Pt_jetBDTmed",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,300.);
	m_pt_jetBDTtight      = Book1D("Pt_jetBDTtigth",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,300.);


}

void GeneralTauPlots::fill(const xAOD::TauJet& tau) {
  m_oParamPlots.fill(tau);
  m_tauCharge->Fill(tau.charge(), 1.); 
  m_tauNCoreTracks->Fill(tau.nTracks(), 1.);
  //  m_tauNWideTracks->Fill(tau.nWideTracks(), 1.);
  m_tauNWideTracks->Fill(tau.nTracks(xAOD::TauJetParameters::classifiedIsolation), 1.); // 
  if( tau.hasDiscriminant(xAOD::TauJetParameters::BDTJetScore) )m_id_BDTJetScore->Fill(tau.discriminant(xAOD::TauJetParameters::BDTJetScore));
  if(tau.isTau(xAOD::TauJetParameters::JetBDTSigLoose))   m_pt_jetBDTloose->Fill( tau.pt()/1000,1);
  if(tau.isTau(xAOD::TauJetParameters::JetBDTSigMedium))  m_pt_jetBDTmed  ->Fill( tau.pt()/1000,1);
  if(tau.isTau(xAOD::TauJetParameters::JetBDTSigTight))   m_pt_jetBDTtight->Fill( tau.pt()/1000,1);

}


}
