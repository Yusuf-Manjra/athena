///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EgammaPhysValMonitoringTool.cxx 
// Implementation file for class EgammaPhysValMonitoringTool
// Author:
/////////////////////////////////////////////////////////////////// 

// PhysVal includes
#include "EgammaPhysValMonitoringTool.h"

// STL includes
#include <vector>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaDefs.h" 
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODTruth/xAODTruthHelpers.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

#include "StoreGate/ReadHandle.h"

#include <iostream>
using namespace std;
using namespace MCTruthPartClassifier;

namespace EgammaPhysValMonitoring {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

EgammaPhysValMonitoringTool::EgammaPhysValMonitoringTool( const std::string& type, 
							  const std::string& name, 
							  const IInterface* parent ):
  ManagedMonitorToolBase( type, name, parent ),
  m_oElectronValidationPlots(0, "Electron/"),
  m_oPhotonValidationPlots(0, "Photon/")
{    
}

// Destructor
///////////////
EgammaPhysValMonitoringTool::~EgammaPhysValMonitoringTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode EgammaPhysValMonitoringTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");    
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_CHECK(m_truthClassifier.retrieve());  

  ATH_CHECK(m_photonContainerKey.initialize());
  ATH_CHECK(m_electronContainerKey.initialize());
  ATH_CHECK(m_electronContainerFrwdKey.initialize());
  ATH_CHECK(m_truthParticleContainerKey.initialize(m_isMC));
  ATH_CHECK(m_egammaTruthContainerKey.initialize(m_isMC));

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::bookHistograms() 
//-------------------------------------------------------------------
{
  ATH_MSG_INFO ("Booking hists " << name() << "...");

  m_oElectronValidationPlots.initialize();
  std::vector<HistData> hists = m_oElectronValidationPlots.retrieveBookedHistograms();
  for (auto &hist : hists){
    ATH_MSG_INFO ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }

  m_oPhotonValidationPlots.initialize();
  hists = m_oPhotonValidationPlots.retrieveBookedHistograms();
  for (auto &hist : hists){
    ATH_MSG_INFO ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }



  return StatusCode::SUCCESS;      
}


//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::fillHistograms()
//-------------------------------------------------------------------
{
  ATH_MSG_DEBUG ("Filling hists " << name() << "...");

  SG::ReadHandle< xAOD::TruthParticleContainer> truthParticles;
  
  if(m_isMC){// return StatusCode::SUCCESS;
    
    truthParticles = SG::ReadHandle< xAOD::TruthParticleContainer> ( m_egammaTruthContainerKey );
    
    //filling truth iso (prompt) particles from egammaTruthParticles container (containing only iso particles)
    // validity check is only really needed for serial running. Remove when MT is only way.
    if (!truthParticles.isValid()) {
      ATH_MSG_ERROR ("Couldn't retrieve Truth container with key: " << m_egammaTruthContainerKey.key());
      return StatusCode::FAILURE;
    }
    
    for(const auto truthParticle : *truthParticles){
      
      //--electrons
      if (abs(truthParticle->pdgId()) == 11 && truthParticle->status() == 1  && truthParticle->barcode() < 1000000) {
	m_oElectronValidationPlots.m_oTruthIsoPlots.fill(*truthParticle);     	
      } //-- end electrons
      
	//--photons 	   
      if (abs(truthParticle->pdgId()) == 22 && truthParticle->status() == 1 && truthParticle->barcode() < 1000000){
	m_oPhotonValidationPlots.m_oTruthIsoPlots.fill(*truthParticle);
	//-- filling conversions
	const xAOD::TruthParticle* tmp = xAOD::TruthHelpers::getTruthParticle( *truthParticle ); //20.7.0.1
	//      const xAOD::TruthParticle* tmp = xAOD::EgammaHelpers::getTruthParticle( truthParticle ); 
	bool isTrueConv = false;
	float trueR = -999;
	float truthEta = -999;
	if( tmp && tmp->hasDecayVtx() ){
	  float x = tmp->decayVtx()->x();
	  float y = tmp->decayVtx()->y();
	  trueR = sqrt( x*x + y*y );
	}
	
	if (tmp != nullptr ) {
	  truthEta = tmp->eta();
	  isTrueConv = xAOD::EgammaHelpers::isTrueConvertedPhoton(tmp);//rel20
	} 
	
	m_oPhotonValidationPlots.convTruthR->Fill(trueR);
	m_oPhotonValidationPlots.convTruthRvsEta->Fill(trueR,truthEta);
	if(isTrueConv) m_oPhotonValidationPlots.m_oTruthIsoConvPlots.fill(*truthParticle);
	if(!isTrueConv) m_oPhotonValidationPlots.m_oTruthIsoUncPlots.fill(*truthParticle);
	
	
	
	const xAOD::Photon* recoPhoton = xAOD::EgammaHelpers::getRecoPhoton( truthParticle ); 
	if( recoPhoton ){
	  m_oPhotonValidationPlots.convTruthMatchedR->Fill(trueR);
	  m_oPhotonValidationPlots.convTruthMatchedRvsEta->Fill(trueR,truthEta);
	  
	  m_oPhotonValidationPlots.m_oTruthRecoPlots.fill(*truthParticle);
	  if(isTrueConv){
	    m_oPhotonValidationPlots.m_oTruthRecoConvPlots.fill(*truthParticle);
	  }else{
	    m_oPhotonValidationPlots.m_oTruthRecoUncPlots.fill(*truthParticle);
	  }
	  bool val_loose=false;    
	  recoPhoton->passSelection(val_loose, "Loose");
	  if(val_loose) {
	    m_oPhotonValidationPlots.m_oTruthRecoLoosePlots.fill(*truthParticle);
	    if(isTrueConv){
	      m_oPhotonValidationPlots.m_oTruthRecoLooseConvPlots.fill(*truthParticle);
	    }else{
	      m_oPhotonValidationPlots.m_oTruthRecoLooseUncPlots.fill(*truthParticle);
	    }
	  }//--  end truth loose
	  bool val_tight=false;    
	  recoPhoton->passSelection(val_tight, "Tight");
	  if(val_tight) {
	    m_oPhotonValidationPlots.m_oTruthRecoTightPlots.fill(*truthParticle);
	    if(isTrueConv){
	      m_oPhotonValidationPlots.m_oTruthRecoTightConvPlots.fill(*truthParticle);
	    }else{
	      m_oPhotonValidationPlots.m_oTruthRecoTightUncPlots.fill(*truthParticle);
	    }
	  }//--  end truth tight
	}//--  end recoPhoton
      }//-- end Photons
    } // -- end fill histos iso particles 
    
    
      //filling all truth particles from TruthParticles container (possibly will be deleted, also possibly to fill only prompt particles)
    SG::ReadHandle< xAOD::TruthParticleContainer> truthallParticles( m_truthParticleContainerKey );
    if (!truthallParticles.isValid()) {
      ATH_MSG_ERROR ("Couldn't retrieve Truth container with key: " << m_truthParticleContainerKey.key());
      return StatusCode::FAILURE;
    }         
    
    
    std::pair<ParticleType,ParticleOrigin>  partClass;
    MCTruthPartClassifier::ParticleType type;
    
    for(const auto truthallParticle : *truthallParticles){// Electrons and photons from standard TruthParticle container
      
      //--electrons
      if (abs(truthallParticle->pdgId()) == 11 && truthallParticle->status() == 1  && truthallParticle->barcode() < 1000000) {
	partClass=std::make_pair(Unknown,NonDefined);
	type = Unknown;
	
	if(truthallParticle->isAvailable <int>("truthType")) {
	  MCTruthPartClassifier::ParticleType type   = (MCTruthPartClassifier::ParticleType) truthallParticle->auxdata< int >("truthType");
	  if(type==IsoElectron) m_oElectronValidationPlots.m_oTruthAllIsoPlots.fill(*truthallParticle);
	} else m_oElectronValidationPlots.m_oTruthAllPlots.fill(*truthallParticle); 
      } //-- end electrons
      
	//--photons 	   
      if (abs(truthallParticle->pdgId()) == 22 && truthallParticle->status() == 1 && truthallParticle->barcode() < 1000000){
	
	partClass=std::make_pair(Unknown,NonDefined);
	type = Unknown;
	
	m_oPhotonValidationPlots.m_oTruthAllPlots.fill(*truthallParticle);
	
	if(truthallParticle->isAvailable <int>("truthType")) {
	  type   = (MCTruthPartClassifier::ParticleType) truthallParticle->auxdata< int >("truthType");
	  
	} 
	
	if(type!=IsoPhoton) continue;
	if(truthallParticle->pt()*0.001>20. && fabs(truthallParticle->eta())<2.47){
	  m_oPhotonValidationPlots.m_oTruthAllIsoPlots.fill(*truthallParticle);
#ifdef MCTRUTHCLASSIFIER_CONST
	  IMCTruthClassifier::Info info;
	  m_truthClassifier->particleTruthClassifier (truthallParticle, &info);
	  ParticleOutCome photOutCome = info.particleOutCome;
#else
	  ParticleOutCome photOutCome = m_truthClassifier->getParticleOutCome();
#endif

	  float convTruthR = 9999.;
	  if(truthallParticle->decayVtx()) convTruthR = truthallParticle->decayVtx()->perp();
	  //std::cout<<"Truth Conversion R "<<convTruthR<<std::endl;
	  //m_oPhotonValidationPlots.convTruthR->Fill(convTruthR);
      
	  //fill only iso photon for conv and not converted
	  if (photOutCome == Converted&&convTruthR<800.)   
	    m_oPhotonValidationPlots.m_oTruthAllIsoConvPlots.fill(*truthallParticle);
	  else 
	    m_oPhotonValidationPlots.m_oTruthAllIsoUncPlots.fill(*truthallParticle);
	}//end cuts on truth
      } // -- end photons
 
    } 
  } 


  //---------Electrons----------------------
  if(!fillRecoElecHistograms(truthParticles.ptr())) {
    ATH_MSG_ERROR ("Filling reco elecectron hists  failed " << name() << "...");
    return StatusCode::FAILURE;
  }
  //---------Frwd Electrons----------------------  
  if(!fillRecoFrwdElecHistograms(truthParticles.ptr())) {
    ATH_MSG_ERROR ("Filling reco frwd elecectron hists  failed " << name() << "...");
    return StatusCode::FAILURE;
  }
  //----------Photons
  if(!fillRecoPhotHistograms(truthParticles.ptr())) {
    ATH_MSG_ERROR ("Filling reco photon  hists  failed " << name() << "...");
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::fillRecoElecHistograms(const xAOD::TruthParticleContainer* truthParticles)
//-------------------------------------------------------------------
{
  ATH_MSG_DEBUG ("Filling reco electron hists " << name() << "...");
  
  SG::ReadHandle<xAOD::ElectronContainer> Electrons( m_electronContainerKey );
  if (!Electrons.isValid()) {
    ATH_MSG_ERROR ("Couldn't retrieve Electron container with key: " << m_electronContainerKey.key());
    return StatusCode::FAILURE;
  } 
  
  int numofele=0;
  
  
  for(const auto electron : *Electrons){
    bool isElecPrompt=false;

    if(!(electron->isGoodOQ (xAOD::EgammaParameters::BADCLUSELECTRON))) continue;

    if(electron->isAvailable <int>("truthType")) {
      MCTruthPartClassifier::ParticleType type = (MCTruthPartClassifier::ParticleType) electron->auxdata<int>("truthType");
      if(type==MCTruthPartClassifier::IsoElectron) {isElecPrompt=true;
	//fill energy scale
	const xAOD::TruthParticle* thePart = xAOD::TruthHelpers::getTruthParticle(*electron); // 20.7.X.Y.I
//	const xAOD::TruthParticle* thePart = xAOD::EgammaHelpers::getTruthParticle(electron);
	  if(thePart) {
            float EtLin = (electron->pt()-thePart->pt())/thePart->pt();
	    m_oElectronValidationPlots.res_et->Fill(thePart->pt()*0.001,EtLin);
	    m_oElectronValidationPlots.res_eta->Fill(thePart->eta(),EtLin);
	    m_oElectronValidationPlots.matrix->Fill(electron->pt()*0.001,thePart->pt()*0.001);
	    m_oElectronValidationPlots.pt_ratio->Fill(thePart->pt()*0.001,electron->pt()/thePart->pt());
           }else {
	cout<<"Truth particle associated not in egamma truth collection"<<endl;
             }
	}
      
    } else if(m_isMC){ if(Match(electron,11, truthParticles)!=0 ) isElecPrompt=true;}
    
    
    m_oElectronValidationPlots.fill(*electron,isElecPrompt);
    if(electron->author()&xAOD::EgammaParameters::AuthorElectron||
       electron->author()&xAOD::EgammaParameters::AuthorAmbiguous)   numofele++;
    
  } 
  
  m_oElectronValidationPlots.m_oCentralElecPlots.nParticles->Fill(numofele);
  
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::fillRecoFrwdElecHistograms(const xAOD::TruthParticleContainer* truthParticles)
//-------------------------------------------------------------------
{
  ATH_MSG_DEBUG ("Filling reco frwd electron hists " << name() << "...");

  SG::ReadHandle<xAOD::ElectronContainer> ElectronsFrwd( m_electronContainerFrwdKey );
  if(!ElectronsFrwd.isValid() ) {
    ATH_MSG_ERROR ("Couldn't retrieve Forward Electrons container with key: " << m_electronContainerFrwdKey.key());
    return StatusCode::FAILURE;
  }
  
  int numoffrwdele=0;
  
  for(const auto frwdelectron : *ElectronsFrwd){
    if(!(frwdelectron->isGoodOQ (xAOD::EgammaParameters::BADCLUSELECTRON))) continue;
    bool isElecPrompt=false;
    if (m_isMC) {
      if((Match(frwdelectron,11, truthParticles)!=0 )) isElecPrompt=true;
    }
    m_oElectronValidationPlots.fill(*frwdelectron,isElecPrompt);
    numoffrwdele++;
    isElecPrompt=false;
  }
  
  m_oElectronValidationPlots.m_oFrwdElecPlots.nParticles->Fill(numoffrwdele);
  
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::fillRecoPhotHistograms(const xAOD::TruthParticleContainer* truthParticles)
//-------------------------------------------------------------------
{
  ATH_MSG_DEBUG ("Filling reco photon  hists " << name() << "...");
 
  SG::ReadHandle<xAOD::PhotonContainer> Photons( m_photonContainerKey );
  if (!Photons.isValid()) {
    ATH_MSG_ERROR ("Couldn't retrieve Photons container with key: " << m_photonContainerKey.key());
    return StatusCode::FAILURE;
  } 
  
  int numofPhot=0;
  int numofTopo=0;
  int numofAmb=0; 
  int numPhotAll=0; 
  int numofCnv=0;

  for(auto photon : *Photons){
    bool isPhotPrompt=false;
    if (photon->author()&xAOD::EgammaParameters::AuthorCaloTopo35) continue;//21.0.>7
    if(!(photon->isGoodOQ (xAOD::EgammaParameters::BADCLUSPHOTON))) continue;
      if(photon->isAvailable <int>("truthType")) {
      MCTruthPartClassifier::ParticleType type = (MCTruthPartClassifier::ParticleType) photon->auxdata<int>("truthType");
      if(type==MCTruthPartClassifier::IsoPhoton) {isPhotPrompt=true;
	
	//fill energy scale
 	const xAOD::TruthParticle* thePart = xAOD::TruthHelpers::getTruthParticle(*photon);//20.7.X.Y.I
	//	const xAOD::TruthParticle* thePart = xAOD::EgammaHelpers::getTruthParticle(photon);
	if(thePart&&thePart->pt()*0.001>20.) {
            float EtLin = (photon->pt()-thePart->pt())/thePart->pt();
	    m_oPhotonValidationPlots.res_et->Fill(thePart->pt()*0.001,EtLin);
	    m_oPhotonValidationPlots.res_eta->Fill(thePart->eta(),EtLin);
	    if (abs(EtLin)<0.2){
		m_oPhotonValidationPlots.res_et_cut->Fill(thePart->pt()*0.001,EtLin);
	        m_oPhotonValidationPlots.res_eta_cut->Fill(thePart->eta(),EtLin);
	    }
          }else {
	    cout<<"Truth particle associated not in egamma truth collection"<<endl;
          }
       }

    } else if(m_isMC){if(Match(photon,22, truthParticles)!=0 ) isPhotPrompt=true;}    

    m_oPhotonValidationPlots.fill(*photon, isPhotPrompt);
    if(photon->author()&xAOD::EgammaParameters::AuthorPhoton&&photon->pt()*0.001>7.)           numofPhot++;
    else if(photon->pt()*0.001<7.)  numofTopo++;  
    else if(photon->author()&xAOD::EgammaParameters::AuthorAmbiguous&&photon->pt()*0.001>7.)   numofAmb++; 
    if(xAOD::EgammaHelpers::isConvertedPhoton(photon)&&photon->pt()*0.001>7.)                  numofCnv++;
  }
  numPhotAll = numofPhot+numofTopo+numofAmb;
  m_oPhotonValidationPlots.m_oAllPlots.m_nParticles->Fill(numPhotAll);
  m_oPhotonValidationPlots.m_oPhotPlots.m_nParticles->Fill(numofPhot);
  m_oPhotonValidationPlots.m_oTopoPhotPlots.m_nParticles->Fill(numofTopo);    
  m_oPhotonValidationPlots.m_oAmbPhotPlots.m_nParticles->Fill(numofAmb);
  m_oPhotonValidationPlots.m_oConvPhotPlots.m_nParticles->Fill(numofCnv);
 

  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------
StatusCode EgammaPhysValMonitoringTool::procHistograms() 
//-------------------------------------------------------------------
  {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    m_oElectronValidationPlots.finalize();
    m_oPhotonValidationPlots.finalize();
    return StatusCode::SUCCESS;
  }

//--------------------------------------------------------------------------------------------
const xAOD::TruthParticle* EgammaPhysValMonitoringTool::Match(const xAOD::Egamma* particle,
							      int pdg,
							      const xAOD::TruthParticleContainer* truthParticles) const {
//-------------------------------------------------------------------------------------------
  float currentdr = 0.05;
  const xAOD::TruthParticle* matchedTruthParticle = nullptr;
  for (auto truthParticle: *truthParticles){
    if (abs(truthParticle->pdgId()) != pdg || truthParticle->status() != 1) continue;
    float dr = particle->p4().DeltaR(truthParticle->p4());
    if (dr < currentdr){
      currentdr = dr;
      matchedTruthParticle = truthParticle;
    }
  }
  return matchedTruthParticle;
}


}

