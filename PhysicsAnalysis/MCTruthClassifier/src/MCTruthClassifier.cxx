/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************************

NAME:     MCTruthClassifier.cxx 
PACKAGE:  atlasoff/PhysicsAnalysis/MCTruthClassifier

AUTHORS:  O. Fedin
CREATED:  Sep 2007

PURPOSE:  to classify  truth particles according to their origin. Based on
          the truth particle classification the tool provide classification of 
          ID and combined muon tracks, egamma electrons (including forward 
          electrons) and egamma photons. Both AOD and ESD files can be used.
          See  for details: 
    https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EgammaMCTruthClassifier  
          
Updated:  
********************************************************************************/

// INCLUDE HEADER FILES:
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IPartPropSvc.h"
//
#include "MCTruthClassifier/MCTruthClassifier.h"
//
// EDM includes
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODCaloEvent/CaloCluster.h"
//extrapolation           
#include "TrkEventPrimitives/PropDirection.h"
// for jet classification
// #include "JetEvent/JetINav4MomAssociation.h"
#include "xAODTruth/TruthParticleContainer.h"
//
#include "xAODEgamma/EgammaxAODHelpers.h"
//
// Link between HepMC and xAODTruth - need for old EDM
#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "HepMC/GenParticle.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

#include <math.h>
#include <iostream>
#include <iomanip>

using CLHEP::GeV;
using namespace MCTruthPartClassifier;
using namespace std;

// -----------------------------------------------------------------------
// Constructor 
// -----------------------------------------------------------------------
MCTruthClassifier::MCTruthClassifier(const std::string& type,
				     const std::string& name,
				     const IInterface* parent)
  :  AthAlgTool(type, name, parent),
     m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
     m_truthInConeTool ("xAOD::TruthParticlesInConeTool/TruthParticlesInConeTool")
{

  declareInterface<IMCTruthClassifier>(this);
  declareProperty("ParticleCaloExtensionTool",   m_caloExtensionTool );
  declareProperty("TruthInConeTool",               m_truthInConeTool );
  //
  declareProperty("xAODTruthParticleContainerName" , m_xaodTruthParticleContainerName  = "TruthParticles");
  declareProperty("xAODTruthLinkVector"            , m_truthLinkVecName="xAODTruthLinks");
  //
  declareProperty("deltaRMatchCut"   , m_deltaRMatchCut   = 0.2);
  declareProperty("deltaPhiMatchCut" , m_deltaPhiMatchCut = 0.2);
  declareProperty("NumOfSiHitsCut"   , m_NumOfSiHitsCut   = 3);
  declareProperty("phtdRtoTrCut"     , m_phtdRtoTrCut     = 0.1);
  declareProperty("fwrdEledRtoTrCut" , m_fwrdEledRtoTrCut  = 0.15);
  declareProperty("inclEgammaFwrdEle" , m_inclEgammaFwrdEle  = true);
  declareProperty("inclEgammaPhoton"  , m_inclEgammaPhoton   = true);
  declareProperty("ROICone"           , m_ROICone  = false);
  declareProperty("partExtrConePhi"   , m_partExtrConePhi  = 0.4);
  declareProperty("partExtrConeEta"   , m_partExtrConeEta  = 0.2);
  declareProperty("phtClasConePhi"  , m_phtClasConePhi  = 0.05);
  declareProperty("phtClasConeEta"  , m_phtClasConeEta  = 0.025);
  declareProperty("pTChargePartCut"  , m_pTChargePartCut  = 1.0);
  declareProperty("pTNeutralPartCut" , m_pTNeutralPartCut = 0.);
  declareProperty("inclG4part"       , m_inclG4part       = false);
  declareProperty( "LQpatch", m_LQpatch=false);

  declareProperty("jetPartDRMatch" , m_jetPartDRMatch = 0.4);
  declareProperty("FwdElectronTruthExtrEtaCut" , m_FwdElectronTruthExtrEtaCut = 2.4, 
		  "Cut on the eta of the truth Particles to be extrapolated for Fwd electrons");
  declareProperty("FwdElectronTruthExtrEtaWindowCut" , m_FwdElectronTruthExtrEtaWindowCut = 0.15, 
		  "Cut on the delta eta of the truth Particles to be extrapolated for Fwd electrons and the current FwdElectron");

  declareProperty( "useCaching", m_useCaching=true);
  declareProperty( "barcodeG4Shift", m_barcodeG4Shift=200001);
  const HepPDT::ParticleDataTable  m_particleTable("PDG Table");

}



// DESTRUCTOR:

MCTruthClassifier::~MCTruthClassifier()
{  }


// INITIALIZE METHOD:

StatusCode MCTruthClassifier::initialize()
{
  // MSGStream object to output messages from your algorithm
  
  MsgStream mlog( msgSvc(), name() );

  ATH_MSG_INFO( " Initializing MCTruthClassifier" );

  //define barcode scheme
  m_barcodeShift=1000000;
  if(m_barcodeG4Shift==1000001) m_barcodeShift=10000000;
  // Particle properties
  IPartPropSvc* m_partPropSvc = 0;
  static const bool CREATEINFOTTHERE(true);
  StatusCode sc =service("PartPropSvc",m_partPropSvc,CREATEINFOTTHERE);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR( " Can't get PartPropSvc" );
    return sc;
  } else ATH_MSG_DEBUG( "get   PartPropSvc" ); 

  // obtain PDT
  m_particleTable=m_partPropSvc->PDT();

  if( !m_caloExtensionTool.empty() && m_caloExtensionTool.retrieve().isFailure() ) {

    ATH_MSG_WARNING( "Cannot retrieve extrapolateToCaloTool " << m_caloExtensionTool 
                     << " - will not perform extrapolation." );
  }

  if(m_truthInConeTool.retrieve().isFailure() ) {

    ATH_MSG_ERROR( "Cannot retrieve Truth in cone Tool " << m_truthInConeTool);
  }

  return StatusCode::SUCCESS;
  
}


// FINALIZE METHOD:
  
StatusCode  MCTruthClassifier::finalize()
{
  
  return StatusCode::SUCCESS;
  
}

// ATHENA EXECUTE METHOD:

StatusCode MCTruthClassifier::execute()
{
  return StatusCode::SUCCESS;
}

//Old EDM interface
//---------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const HepMC::GenParticle  *thePart){
  //---------------------------------------------------------------------------------------
  ParticleType   partType = Unknown;     
  ParticleOrigin partOrig = NonDefined;  


  if (!thePart) return std::make_pair(partType,partOrig); 
	
  // Retrieve the links between HepMC and xAOD::TruthParticle
  const xAODTruthParticleLinkVector* truthParticleLinkVec = 0;
  if(evtStore()->retrieve(truthParticleLinkVec, m_truthLinkVecName).isFailure()){
    ATH_MSG_WARNING("Cannot retrieve TruthParticleLinkVector");
    return std::make_pair(partType,partOrig); 
  }
 
  for( const auto& entry : *truthParticleLinkVec ) {
    if(entry->first.isValid() && entry->second.isValid()
       && entry->first.cptr()->barcode() == thePart->barcode()) {
      const xAOD::TruthParticle* truthParticle = *entry->second;
      if ( !compareTruthParticles(thePart, truthParticle) ) { 
	//if the barcode/pdg id / status of the pair does not match
	//return default
	return std::make_pair(partType,partOrig); 
      }
      return particleTruthClassifier(truthParticle);
    }
  }

  return std::make_pair(partType,partOrig); 
}
//------------------------------------------------------------------------
bool MCTruthClassifier::compareTruthParticles(const HepMC::GenParticle *genPart,
					      const xAOD::TruthParticle *truthPart){
  //------------------------------------------------------------------------
  if (!genPart || !truthPart) return false;

  if (genPart->barcode() != truthPart->barcode() ||
      genPart->pdg_id() != truthPart->pdgId() ||
      genPart->status() != truthPart->status()) {
    ATH_MSG_DEBUG("HepMC::GenParticle and xAOD::TruthParticle do not match");
    return false;
  }

  return true;
}

//---------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::TruthParticle  *thePart){
  //---------------------------------------------------------------------------------------
 

  ATH_MSG_DEBUG( "Executing particleTruthClassifier" );

  ParticleType   partType = Unknown;     
  ParticleOrigin partOrig = NonDefined;  
  m_ParticleOutCome       = NonDefinedOutCome;   
  m_tauFinalStatePart.clear();
 
  m_partOriVert         = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_ParticleOutCome     = UnknownOutCome;
  m_BkgElecMother       = 0;

  m_isPrompt            = false;

  m_thePart             = thePart;
  //retrieve collection and get a pointer

  if(!thePart) return std::make_pair(partType,partOrig);

  const xAOD::TruthParticleContainer  * xTruthParticleContainer;
  StatusCode sc = evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName);
  if (sc.isFailure()||!xTruthParticleContainer){
    ATH_MSG_WARNING( "No  xAODTruthParticleContainer "<<m_xaodTruthParticleContainerName<<" found" ); 
    return std::make_pair(partType,partOrig);
  }

  ATH_MSG_DEBUG( "xAODTruthParticleContainer  " << m_xaodTruthParticleContainerName<<" successfully retrieved " );


  int iParticlePDG = thePart->pdgId();
  //status=10902 in Pythia? 
  if(thePart->status()!=1&&thePart->status()!=2&&thePart->status()!=10902) return std::make_pair(GenParticle,partOrig); 
  bool isPartHadr  = isHadron(thePart);
  if(thePart->status()==2&&(abs(iParticlePDG)!=15&&!isPartHadr)) return std::make_pair(GenParticle,partOrig); 

  //SUSY datasets: tau(satus==2)->tau(satus==2)
  if(thePart->status()==2&&abs(iParticlePDG)==15){
    const xAOD::TruthVertex*   endVert = thePart->decayVtx();
    if(endVert!=0){
      int numOfDaught=endVert->nOutgoingParticles();
      if(numOfDaught==1&&abs(endVert->outgoingParticle(0)->pdgId())==15) return std::make_pair(GenParticle,partOrig);
    }
  }

  if(thePart->status()==1&&abs(iParticlePDG)<2000040&&abs(iParticlePDG)>1000001)  return std::make_pair(SUSYParticle,partOrig);   
                                                                                                        
 
  if(thePart->status()==10902&&(abs(iParticlePDG)!=11&&abs(iParticlePDG)!=13&&abs(iParticlePDG)!=15&&abs(iParticlePDG)!=22)&&!isPartHadr) return std::make_pair(GenParticle,partOrig); 


  if(abs(iParticlePDG)==12||abs(iParticlePDG)==14||abs(iParticlePDG)==16)             return std::make_pair(Neutrino,partOrig);
  if(abs(iParticlePDG)>1000000000)                                                    return std::make_pair(NuclFrag,partOrig);

  if(abs(iParticlePDG)!=11&&abs(iParticlePDG)!=13&&abs(iParticlePDG)!=15&&abs(iParticlePDG)!=22&&!isPartHadr) return std::make_pair(partType,partOrig);
  //don't consider  generator particles 
 
  const xAOD::TruthVertex* m_partOriVert=thePart->hasProdVtx() ? thePart->prodVtx():0;

  if( m_partOriVert!=0 ) {
    for (unsigned int ipIn=0; ipIn<m_partOriVert->nIncomingParticles(); ++ipIn) {
      const xAOD::TruthParticle* theMoth=m_partOriVert->incomingParticle(ipIn);
      m_MotherPDG      = theMoth->pdgId(); 
      m_MotherBarcode  = theMoth->barcode(); 
      m_MotherStatus   = theMoth->status(); 
    } 
  }

  if(m_partOriVert==0&&thePart->barcode()>m_barcodeShift) return std::make_pair(NonPrimary,partOrig); 

  if(m_partOriVert==0 && abs(iParticlePDG)==11)  {
    // to define electron out come  status
    partOrig =  defOrigOfElectron(xTruthParticleContainer,thePart);
    return std::make_pair(UnknownElectron,partOrig);     
  } else  if(m_partOriVert==0 && abs(iParticlePDG)==13) {
    // to define electron out come  status
    partOrig =  defOrigOfMuon(xTruthParticleContainer,thePart);
    return std::make_pair(UnknownMuon,partOrig);
  } else  if(m_partOriVert==0 && abs(iParticlePDG)==15) {
    // to define electron out come  status
    partOrig =  defOrigOfTau(xTruthParticleContainer,thePart); 
    return std::make_pair(UnknownTau,partOrig);
  } else  if(m_partOriVert==0 && abs(iParticlePDG)==22) {
    // to define photon out come 
    partOrig =  defOrigOfPhoton(xTruthParticleContainer,thePart);
    return std::make_pair(UnknownPhoton,partOrig);
  }

 
  if(thePart->barcode()%m_barcodeShift == m_MotherBarcode%m_barcodeShift) return std::make_pair(NonPrimary,partOrig); 
 
  if(isPartHadr)  return std::make_pair(Hadron,partOrig); 

  if(m_partOriVert!=0&&m_MotherPDG==0&&m_partOriVert->nOutgoingParticles()==1&&m_partOriVert->nIncomingParticles()==0) {
    if(abs(iParticlePDG)==11){m_ParticleOutCome=defOutComeOfElectron(thePart); return std::make_pair(IsoElectron,SingleElec);}
    if(abs(iParticlePDG)==13){m_ParticleOutCome=defOutComeOfMuon(thePart);     return std::make_pair(IsoMuon,SingleMuon);    }
    if(abs(iParticlePDG)==15){m_ParticleOutCome=defOutComeOfTau(thePart);      return std::make_pair(IsoTau,SingleTau);      }
    if(    iParticlePDG ==22){m_ParticleOutCome=defOutComeOfPhoton(thePart);   return std::make_pair(IsoPhoton,SinglePhot);  }
  }

  if( m_MotherPDG==iParticlePDG && m_MotherStatus==3 && thePart->status()==10902)  return std::make_pair(GenParticle,partOrig); 
  
  if(abs(iParticlePDG)==11){
    partOrig =  defOrigOfElectron(xTruthParticleContainer,thePart);
    partType =  defTypeOfElectron(partOrig);
  } else  if(abs(iParticlePDG)==13) {
    partOrig =  defOrigOfMuon(xTruthParticleContainer,thePart);
    partType =  defTypeOfMuon(partOrig);
  } else  if(abs(iParticlePDG)==15) {
    partOrig =  defOrigOfTau(xTruthParticleContainer,thePart);
    partType =  defTypeOfTau(partOrig);
  } else  if(abs(iParticlePDG)==22){
    partOrig =  defOrigOfPhoton(xTruthParticleContainer,thePart);
    partType =  defTypeOfPhoton( partOrig);
  }

  ATH_MSG_DEBUG( "particleTruthClassifier  succeeded " );
  return std::make_pair(partType,partOrig);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::TrackParticle *trkPtr){
  //-----------------------------------------------------------------------------------------
  

  ATH_MSG_DEBUG( "Executing trackClassifier" );
 
  m_thePart             = 0;
  m_partOriVert         = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_isPrompt            = false;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0; 
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  m_thePart=getGenPart(trkPtr);
  
  if(!m_thePart)   return std::make_pair(parttype,partorig);


  ATH_MSG_DEBUG( "trackClassifier  succeeded " ); 
  return particleTruthClassifier(m_thePart);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::trackClassifier(const xAOD::TrackParticle *trkPtr){
  //-----------------------------------------------------------------------------------------
  // It is just to keep backward compatibility  - don't use this  methode. 
  ATH_MSG_DEBUG( "Executing trackClassifier" ); 
  return particleTruthClassifier(trkPtr);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Electron* elec){
  //-----------------------------------------------------------------------------------------
  

  ATH_MSG_DEBUG( "Executing egamma electron Classifier" );
 
  m_thePart             = 0;
  m_partOriVert         = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0; 
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  m_isPrompt            = false;

  m_egPartPtr.clear();
  m_egPartdR.clear();
  m_egPartClas.clear();

  if(elec->author()&xAOD::EgammaParameters:: AuthorFwdElectron){
    const xAOD::CaloCluster* clus=elec->caloCluster();
    m_thePart  = egammaClusMatch(clus,true);
  } else {
    const xAOD::TrackParticle* trkPtr=elec->trackParticle();
    if(!trkPtr)     {return std::make_pair(parttype,partorig);}
    m_thePart=getGenPart(trkPtr);
    if(!m_thePart)  {return std::make_pair(parttype,partorig);}
  }
  
  ATH_MSG_DEBUG( "egamma electron Classifier  succeeded " );
  return particleTruthClassifier(m_thePart);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Photon* phot){
  //-----------------------------------------------------------------------------------------
  

  ATH_MSG_DEBUG( "Executing egamma photon Classifier" ); 

  m_thePart             = 0;
  m_partOriVert         = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0; 
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  m_isPrompt            = false;
   
 
  m_cnvPhtTrkPtr.clear();
  m_cnvPhtTrPart.clear();
  m_cnvPhtPartType.clear();
  m_cnvPhtPartOrig.clear();

  m_egPartPtr.clear();
  m_egPartdR.clear();
  m_egPartClas.clear();

  const xAOD::CaloCluster* clus=phot->caloCluster();
  if(!clus) return std::make_pair(parttype,partorig);  
  if( fabs(clus->eta())> 10.0 ||fabs(clus->phi())> 6.28||(clus->et())<=0.) return std::make_pair(parttype,partorig);

  const xAOD::Vertex* VxCvPtr=phot->vertex();

  if(VxCvPtr!=0) {
    for(int itrk=0; itrk<(int) VxCvPtr->nTrackParticles(); itrk++){
      if(itrk>1) continue;
      const xAOD::TrackParticle* trkPtr  = VxCvPtr->trackParticle(itrk);
      if(!trkPtr)  continue;
      m_cnvPhtTrkPtr.push_back(trkPtr); 
      const xAOD::TruthParticle* thePart=getGenPart(trkPtr);

      std::pair<ParticleType,ParticleOrigin>  classif=particleTruthClassifier(thePart);
      m_cnvPhtTrPart.push_back(thePart);
  
      //std::pair<ParticleType,ParticleOrigin>  classif=particleTruthClassifier(trkPtr);
      // m_cnvPhtTrPart.push_back(getGenPart());

      m_cnvPhtPartType.push_back(classif.first);
      m_cnvPhtPartOrig.push_back(classif.second);
    } // end cycle itrk 

  } // VxCvPtr!=0



  m_thePart = egammaClusMatch(clus,false);


  ATH_MSG_DEBUG( "egamma photon  Classifier  succeeded " );

  return particleTruthClassifier(m_thePart);

}
//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Muon* mu ){
  //-----------------------------------------------------------------------------------------
  

  ATH_MSG_DEBUG( "Executing muon  Classifier" );
 
  m_thePart             = 0;
  m_partOriVert         = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_isPrompt            = false;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0; 
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;


  //typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;
  const xAOD::TrackParticle* trkPtr=0;

  if( mu->primaryTrackParticleLink().isValid())                 trkPtr = *mu->primaryTrackParticleLink();
  else if( mu->combinedTrackParticleLink().isValid() )          trkPtr = *mu->combinedTrackParticleLink();
  else if (mu->inDetTrackParticleLink().isValid())              trkPtr = *mu->combinedTrackParticleLink();
  else if( mu->muonSpectrometerTrackParticleLink().isValid() )  trkPtr = *mu->muonSpectrometerTrackParticleLink();

  if(!trkPtr)   return std::make_pair(parttype,partorig);
 
  m_thePart=getGenPart(trkPtr);
  if(!m_thePart)  return std::make_pair(parttype,partorig);

  ATH_MSG_DEBUG( "muon Classifier  succeeded " );
  return particleTruthClassifier(m_thePart);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::CaloCluster* clus){
  //-----------------------------------------------------------------------------------------
  
  ATH_MSG_DEBUG( "Executing egamma photon Classifier with cluster Input" );

  m_thePart             = 0;
  m_partOriVert         = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0; 
  m_probability         = 0;
  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;
  m_isPrompt            = false;
  m_egPartPtr.clear();
  m_egPartdR.clear();
  m_egPartClas.clear();

  if(!clus) return std::make_pair(parttype,partorig);  
  if( fabs(clus->eta())> 10.0 ||fabs(clus->phi())> M_PI||(clus->et())<=0.) return std::make_pair(parttype,partorig);

  m_thePart = egammaClusMatch(clus,false);


  ATH_MSG_DEBUG( "egamma photon  Classifier  succeeded " );

  return particleTruthClassifier(m_thePart);
}
//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Jet* jet, bool DR = true){
  //-----------------------------------------------------------------------------------------
  

  ATH_MSG_DEBUG( "Executing Classifier with jet Input" );

  m_thePart             = 0;
  m_deltaRMatch         =-999.;
  m_ParticleOutCome           = UnknownOutCome;
  ParticleType   parttype     = UnknownJet;
  ParticleOrigin partorig     = NonDefined;
  ParticleType   tempparttype = UnknownJet;
  std::set<const xAOD::TruthParticle*>      allJetMothers;
  std::pair<ParticleType,ParticleOrigin>   res;

  if(!jet) return std::make_pair(parttype,partorig); 

  allJetMothers.clear();

  // FIXME: uncomment
  if (!DR) { // use ghost-associated particles 
    // retrieve a truth particle association  
    //     const xAOD::JetINav4MomAssociation* truthA = jet->getAssociation<JetINav4MomAssociation>("TruthAssoc");
    //     if (!truthA)  return std::make_pair(parttype,partorig);  
    // 
    //     // loop over truth particles within the JetINav4MomAssociation objects
    //     JetINav4MomAssociation::object_iter it  = truthA->begin();
    //     JetINav4MomAssociation::object_iter itE = truthA->end();
    //     for( ; it !=itE; ++it) {
    //       const TruthParticle* tp = dynamic_cast<const TruthParticle*> (*it);
    //       const HepMC::GenParticle* thePart = tp->genParticle();
    //       if(!tp) continue;
    //       // do not look at intermediate particles
    //       if (tp->status()!=2&&tp->status()!=3&&tp->status()!=10902) continue;
    //       // determine jet origin
    //       findAllJetMothers(thePart,allJetMothers);
    //       // determine jet type
    //       if (tp->status()==3) continue; 
    //       // classify the particle
    //       tempparttype = particleTruthClassifier(thePart).first;
    //       if(tempparttype==Hadron) tempparttype=defTypeOfHadron(thePart->pdg_id());
    //       // classify the jet
    //       if (tempparttype==BBbarMesonPart || tempparttype==BottomMesonPart  || tempparttype==BottomBaryonPart)  parttype = BJet;
    //       else if (tempparttype==CCbarMesonPart || tempparttype==CharmedMesonPart || tempparttype==CharmedBaryonPart) 
    // 	{if (parttype==BJet) {} else { parttype = CJet;}}
    //       else if (tempparttype==StrangeBaryonPart||tempparttype==LightBaryonPart||tempparttype==StrangeMesonPart||tempparttype==LightMesonPart) 
    // 	{if (parttype==BJet||parttype==CJet) {} else { parttype = LJet;}}
    //       else {if (parttype==BJet||parttype==CJet||parttype==LJet) {} else { parttype = UnknownJet;}}
    // 
    //     } // end loop over truth particles
    //     
    //     // end ghost-association case
    //  
  } else { 
    // use a DR matching scheme (default)
    // retrieve collection and get a pointer 
    const xAOD::TruthParticleContainer  * xTruthParticleContainer;
    StatusCode sc = evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName);
    if (sc.isFailure()||!xTruthParticleContainer){
      ATH_MSG_WARNING( "No  xAODTruthParticleContainer "<<m_xaodTruthParticleContainerName<<" found" ); 
      return std::make_pair(parttype,partorig);
    }

    ATH_MSG_DEBUG( "xAODTruthParticleContainer  " << m_xaodTruthParticleContainerName<<" successfully retrieved " );

    // find the matching truth particles
    for(const auto thePart : *xTruthParticleContainer){
      // do not look at intermediate particles
      if(thePart->status()!=2&&thePart->status()!=3&&thePart->status()!=10902) continue;
      // match truth particles to the jet
      if (deltaR((*thePart),(*jet)) > m_jetPartDRMatch) continue;
      // determine jet origin
      findAllJetMothers(thePart,allJetMothers);
      // determine jet type
      if(thePart->status()==3) continue;
      // determine jet type
      tempparttype = particleTruthClassifier(thePart).first;
      if(tempparttype==Hadron) tempparttype=defTypeOfHadron(thePart->pdgId());
      // classify the jet
      if (tempparttype==BBbarMesonPart || tempparttype==BottomMesonPart || tempparttype==BottomBaryonPart) 
    	{parttype = BJet;}
      else if (tempparttype==CCbarMesonPart || tempparttype==CharmedMesonPart || tempparttype==CharmedBaryonPart ) 
    	{if (parttype==BJet) {} else { parttype = CJet;}}
      else if (tempparttype==StrangeBaryonPart||tempparttype==LightBaryonPart||tempparttype==StrangeMesonPart||tempparttype==LightMesonPart) 
    	{if (parttype==BJet||parttype==CJet) {} else { parttype = LJet;}}
      else {if (parttype==BJet||parttype==CJet||parttype==LJet) {} else { parttype = UnknownJet;}}

    } // end loop over truth particles  

  }// end DR scheme
  
  // clasify the jet origin
  partorig = defJetOrig(allJetMothers);

  allJetMothers.clear();

  ATH_MSG_DEBUG( " jet Classifier succeeded" );  
  return std::make_pair(parttype,partorig);
}

//---------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defJetOrig(std::set<const xAOD::TruthParticle*> allJetMothers){
  //---------------------------------------------------------------------------------
  ParticleOrigin             partOrig = NonDefined;
  std::set<const xAOD::TruthParticle*>::iterator it;

  for (it=allJetMothers.begin(); it!=allJetMothers.end(); ++it) {
    int pdg = abs((*it)->pdgId());
    if ( pdg == 6  )                        { partOrig = top;         }
    if ( pdg == 23 )                        { partOrig = ZBoson;      }
    if ( pdg == 24 && !(partOrig==top) )    { partOrig = WBoson;      }
    if ( (pdg < 6 || pdg == 21) &&
         !( partOrig==top    ||
            partOrig==ZBoson ||
            partOrig==WBoson    ) )         { partOrig = QCD;         }
    if ( pdg == 25 )                        { partOrig = Higgs;      return partOrig; }
    if ( pdg == 35 ||
         pdg == 36 ||
         pdg == 37 )                        { partOrig = HiggsMSSM;  return partOrig; }
    if ( pdg == 32 ||
         pdg == 33 ||
         pdg == 34 )                        { partOrig = HeavyBoson; return partOrig; }  
    if ( pdg == 42 )                        { partOrig = LQ;         return partOrig; }     
    if ( pdg < 2000040 &&
         pdg > 1000001 )                    { partOrig = SUSY;       return partOrig; }
  }
  return partOrig;
}
//---------------------------------------------------------------------------------------------------------
void MCTruthClassifier::findAllJetMothers(const xAOD::TruthParticle* thePart,
					  std::set<const xAOD::TruthParticle*>&allJetMothers){   
  //---------------------------------------------------------------------------------------------------------
  const xAOD::TruthVertex*  partOriVert = thePart->prodVtx();

  if (partOriVert!=0) {
    for(unsigned int ipIn=0;ipIn<partOriVert->nIncomingParticles();ipIn++){
      const xAOD::TruthParticle* theMoth=partOriVert->incomingParticle(ipIn);
      allJetMothers.insert(theMoth);
      findAllJetMothers(theMoth,allJetMothers);
    }
  }
  return ;
}

//-----------------------------------------------------------------------------------------
const xAOD::TruthParticle* MCTruthClassifier::getGenPart(const xAOD::TrackParticle *trk){
  //-----------------------------------------------------------------------------------------
  //return GenParticle corresponding to given TrackParticle
   
  ATH_MSG_DEBUG( "Executing getGenPart " ); 

  if (!trk) return 0;
 
  m_deltaRMatch =-999.;
  m_deltaPhi    =-999.;
  m_NumOfSiHits = 0; 
  m_probability = 0;

  uint8_t NumOfPixHits = 0;
  uint8_t NumOfSCTHits = 0;
  typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;

  static SG::AuxElement::Accessor<TruthLink_t>  tPL ("truthParticleLink");
  if(!tPL.isAvailable(*trk)){
    ATH_MSG_DEBUG("Track particle is not associated to truth particle");
    return 0;
  }  
 
  auto truthLink = tPL(*trk);
  if (!truthLink.isValid()){
    ATH_MSG_DEBUG("Invalid link to truth particle");
    return 0;
  }
 
  const xAOD::TruthParticle* theGenParticle=(*truthLink);
  if (!theGenParticle){
    ATH_MSG_DEBUG("Could not find truth matching for track"); 
    return 0;
  }
 
  static SG::AuxElement::Accessor<float >  tMP ("truthMatchProbability");
  if(tMP.isAvailable(*trk)){
    m_probability = tMP(*trk);
  }
  else {
    ATH_MSG_DEBUG("Truth match probability not available");
  }

  if(theGenParticle->status()==2||theGenParticle->status()==3){   
    ATH_MSG_WARNING("track matched to the truth with status "<<theGenParticle->status());
  }

  if(theGenParticle->status()==2 && 
     (abs(theGenParticle->pdgId())==11||abs(theGenParticle->pdgId())==13))
    {
      const xAOD::TruthVertex* EndVrtx=theGenParticle->decayVtx();
      const xAOD::TruthParticle* theGenPartTmp(0);
 
      if(EndVrtx!=0)
	{
	  int itr=0;
	  do { 
	    theGenPartTmp=0;
            for(unsigned int ipOut=0; ipOut<EndVrtx->nOutgoingParticles();ipOut++){
	      const xAOD::TruthParticle* theDaugt=EndVrtx->outgoingParticle(ipOut);
	      if(theDaugt->pdgId()==theGenParticle->pdgId()) theGenPartTmp=theDaugt;
	      if(theDaugt->pdgId()!=theGenParticle->pdgId()&&theDaugt->pdgId()!=22)
		theGenPartTmp=0;
	    }
	    	    
	    if(theGenPartTmp!=0) EndVrtx = theGenPartTmp->decayVtx(); else  EndVrtx = 0;
	    itr++;
	    if(itr>100) { ATH_MSG_WARNING ("getGenPart infinite while");  break;}
	  }  while (theGenPartTmp->pdgId()==theGenParticle->pdgId()&&theGenPartTmp->status()==2&&
		    EndVrtx!=0);
	  if(theGenPartTmp!=0&&theGenPartTmp->pdgId()==theGenParticle->pdgId()) {theGenParticle=theGenPartTmp;}	
	   
	}  
    
    }
  
  if (!trk->summaryValue(NumOfSCTHits, xAOD::numberOfSCTHits) )
    ATH_MSG_DEBUG("Could not retrieve number of SCT hits");
  if (!trk->summaryValue(NumOfPixHits, xAOD::numberOfPixelHits) )
    ATH_MSG_DEBUG("Could not retrieve number of Pixel hits");
 
  m_NumOfSiHits=NumOfSCTHits+NumOfPixHits;
  
  m_deltaPhi  = detPhi(theGenParticle->phi(),trk->phi());
  float  deteta  = detEta(theGenParticle->eta(),trk->eta());
  m_deltaRMatch   = rCone(m_deltaPhi, deteta);
  if(m_NumOfSiHits>m_NumOfSiHitsCut&&m_deltaRMatch >m_deltaRMatchCut) theGenParticle=NULL; 
  else if(m_NumOfSiHits<=m_NumOfSiHitsCut && m_deltaPhi > m_deltaPhiMatchCut)  theGenParticle=NULL; 
  
  ATH_MSG_DEBUG( "getGenPart  succeeded " );
  return(theGenParticle);
  
}

//-------------------------------------------------------------------------------
ParticleType MCTruthClassifier::defTypeOfElectron(ParticleOrigin EleOrig){
  //-------------------------------------------------------------------------------

 
  if( EleOrig == NonDefined)  return UnknownElectron;

  if (EleOrig == WBoson     || EleOrig == ZBoson     || EleOrig == top       ||
      EleOrig == SingleElec || EleOrig == Higgs      || EleOrig == HiggsMSSM || 
      EleOrig == HeavyBoson || EleOrig == WBosonLRSM || EleOrig == NuREle    || 
      EleOrig == NuRMu      || EleOrig == NuRTau     || EleOrig == LQ        || 
      EleOrig == SUSY       || EleOrig == DiBoson    || EleOrig == ZorHeavyBoson ||
      m_isPrompt ) { 
    return IsoElectron;
  }
  if (EleOrig == JPsi          || EleOrig == BottomMeson  || 
      EleOrig == CharmedMeson  || EleOrig == BottomBaryon || 
      EleOrig == CharmedBaryon || EleOrig == TauLep       ||
      EleOrig == Mu            || EleOrig == QuarkWeakDec){
    return  NonIsoElectron;
  }
  return BkgElectron;

  
}

//2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defOrigOfElectron(const xAOD::TruthParticleContainer* m_mcTruthTES,
                                                    const xAOD::TruthParticle  *thePart)
//-------------------------------------------------------------------------------
{

  ATH_MSG_DEBUG( "Executing DefOrigOfElectron " ); 

  int PriBarcode  = thePart->barcode()%m_barcodeShift;
  const xAOD::TruthParticle* thePriPart = barcode_to_particle(m_mcTruthTES,PriBarcode);
  if(!thePriPart) return NonDefined;
  if(abs(thePriPart->pdgId())!=11) return NonDefined;

  const xAOD::TruthVertex*   m_partOriVert = thePriPart->prodVtx();
 
  //-- to define electron outcome status
  m_ParticleOutCome = defOutComeOfElectron(thePriPart); 

  if(!m_partOriVert) return NonDefined;

  m_NumOfParents=-1;
  m_NumOfParents=m_partOriVert->nIncomingParticles();
  if(m_NumOfParents>1) 
    ATH_MSG_WARNING( "DefOrigOfElectron:: electron  has more than one mother " );

  m_Mother      = getMother(thePriPart);
  if(!m_Mother) return NonDefined;
  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();
  m_MothOriVert   = m_Mother->prodVtx();

  //---patch to fix LQ dataset problem
  if(m_LQpatch){
    if(abs(m_MotherPDG) == 11&&m_Mother->status()==2&&m_MothOriVert==0&&m_MotherPDG==thePriPart->pdgId()&&
       m_NumOfParents == 1 &&(m_partOriVert->nOutgoingParticles()==1||m_partOriVert->nOutgoingParticles()==2) ) {
      const xAOD::TruthParticle* theP(0);
      int itr=0;
      float dRMin(99999.);
      do { 
	const xAOD::TruthParticle* thePtmp = barcode_to_particle(m_mcTruthTES,m_MotherBarcode-(itr+2));
	if(abs(thePtmp->pdgId())==11&&thePtmp->pdgId()==m_MotherPDG&&thePtmp->status()==3&&thePtmp->decayVtx()==0){
	  float dphi=detPhi(m_Mother->phi(),thePtmp->phi());
	  float deta=m_Mother->eta()-thePtmp->eta();
	  float dR=rCone(dphi,deta);
	  if(dR<dRMin) {dRMin=dR; theP=thePtmp;};
	 
	} 
	itr++;
      } while (itr<4);
     
      if(dRMin>0.1) theP=0;
      if(theP!=0){
	thePriPart = theP;
	m_Mother        = getMother(thePriPart);
	if(!m_Mother)   return NonDefined;
	m_MotherPDG     = m_Mother->pdgId();
	m_MotherBarcode = m_Mother->barcode();
	m_MothOriVert   = m_Mother->prodVtx();
      }
    }
  }
  //----


  // to exclude interactions mu(barcode<10^6)->mu(barcode10^6)+e
  bool samePart=false;
  for (unsigned int ipOut=0; ipOut<m_partOriVert->nOutgoingParticles(); ++ipOut) {
    const xAOD::TruthParticle* theDaug=m_partOriVert->outgoingParticle(ipOut);
    if( m_MotherPDG==theDaug->pdgId()&&m_MotherBarcode%m_barcodeShift==theDaug->barcode()%m_barcodeShift) samePart=true;
  }

  if( ( abs(m_MotherPDG)==13||abs(m_MotherPDG)==15||abs(m_MotherPDG)==24)&& m_MothOriVert!=0&&!samePart){  
    long pPDG(0);
    const xAOD::TruthParticle*  MotherParent(0);
    do { 
      pPDG=0;
      MotherParent=getMother(m_Mother);
      if(MotherParent) pPDG = MotherParent->pdgId();

      if(abs(pPDG)==13 || abs(pPDG)==15 || abs(pPDG)==24)  m_Mother = MotherParent;
 
    }  while ((abs(pPDG)==13 || abs(pPDG)==15 || abs(pPDG)==24) );

    if(abs(pPDG)==13 || abs(pPDG)==15 || abs(pPDG)==24 || abs(pPDG)==23 || 
       abs(pPDG)==25 || abs(pPDG)==35 || abs(pPDG)==36 || abs(pPDG)==37 || 
       abs(pPDG)==32 || abs(pPDG)==33 || abs(pPDG)==34 || abs(pPDG)==6  ||
       abs(pPDG)==9900024 || abs(pPDG)== 9900012 || abs(pPDG)== 9900014 || abs(pPDG)== 9900016 ||      
       (abs(pPDG)<2000040&& abs(pPDG)>1000001))  m_Mother = MotherParent; 
  }


  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();
  m_MotherStatus  = m_Mother->status();
  m_partOriVert   = m_Mother->decayVtx();
  m_MothOriVert   = m_Mother->prodVtx();
  m_NumOfParents  = m_partOriVert->nIncomingParticles();
  m_NumOfDaug     = m_partOriVert->nOutgoingParticles();



  int  NumOfPhot(0),NumOfEl(0),NumOfPos(0),NumOfNucFr(0);
  int  NumOfquark(0),NumOfgluon(0),NumOfElNeut(0),NumOfLQ(0);
  int  NumOfMuPl(0),NumOfMuMin(0),NumOfMuNeut(0);
  int  NumOfTau(0),NumOfTauNeut(0);
  long DaugType(0);
  samePart=false;

  for (unsigned int ipOut=0; ipOut<m_partOriVert->nOutgoingParticles(); ++ipOut) {
    const xAOD::TruthParticle* theDaug=m_partOriVert->outgoingParticle(ipOut);
    DaugType = theDaug->pdgId();
    if( abs(DaugType)  < 7  ) NumOfquark++;
    if( abs(DaugType) == 21 ) NumOfgluon++;
    if( abs(DaugType) == 12 ) NumOfElNeut++;
    if( abs(DaugType) == 14 ) NumOfMuNeut++;
    if( DaugType      == 22 ) NumOfPhot++;
    if( DaugType      == 11 ) NumOfEl++;
    if( DaugType      ==-11 ) NumOfPos++;
    if( DaugType      == 13 ) NumOfMuMin++;
    if( DaugType      ==-13 ) NumOfMuPl++;
    if( abs(DaugType) == 15 ) NumOfTau++;
    if( abs(DaugType) == 16 ) NumOfTauNeut++;

    if( abs(DaugType) == 42 ) NumOfLQ++;
    if( abs(DaugType) == abs(m_MotherPDG)&&
	theDaug->barcode()%m_barcodeShift == m_MotherBarcode%m_barcodeShift ) samePart=true;
    if(m_NumOfParents==1&&(m_MotherPDG==22||abs(m_MotherPDG)==11||abs(m_MotherPDG)==13||abs(m_MotherPDG)==211)&&
       (DaugType > 1000000000||DaugType==0 ||DaugType == 2212||DaugType == 2112 || abs(DaugType) == 211|| abs(DaugType) == 111))  NumOfNucFr++;
  } // cycle itrDaug



  if (m_MotherPDG == 22&&m_MothOriVert!=0){
    // get mother of photon
    for (unsigned int ipIn=0; ipIn<m_MothOriVert->nIncomingParticles(); ++ipIn) {
      const xAOD::TruthParticle* theMother=m_MothOriVert->outgoingParticle(ipIn);
      m_PhotonMotherPDG     = theMother->pdgId();  
      m_PhotonMotherBarcode = theMother->barcode();
      m_PhotonMotherStatus  = theMother->status();
      m_PhotonMother        = theMother;

    }
  }

  // if ( m_NumOfParents == 0 && m_NumOfDaug == 1 )  return  SingleElec;   


  if ( (m_MotherPDG == 22 && m_NumOfDaug == 2 &&  NumOfEl == 1 && NumOfPos == 1)||
       (m_MotherPDG == 22 && m_NumOfDaug == 1 && (NumOfEl == 1 || NumOfPos == 1)))
    return PhotonConv;  


  //e,gamma,pi+Nuclear->NuclearFragments+nuclons+e
  if ( ( m_NumOfParents == 1&&(abs(m_MotherPDG)==22||abs(m_MotherPDG)==11||abs(m_MotherPDG)==15 ))
       &&m_NumOfDaug>1&&NumOfNucFr!=0)          return ElMagProc ;

  if ( m_NumOfParents == 1&&abs(m_MotherPDG)==211
       &&m_NumOfDaug>2&&NumOfNucFr!=0)          return ElMagProc ;

  // nuclear photo fission 
  if ( m_MotherPDG == 22 && m_NumOfDaug > 4&&NumOfNucFr!=0) 
    return ElMagProc;

  // unknown process el(pos)->el+pos??
  if( abs(m_MotherPDG) == 11 && m_NumOfDaug == 2 && NumOfEl == 1 && NumOfPos == 1 ) 
    return ElMagProc;

  // unknown process el->el+el??
  if( m_MotherPDG == 11 && m_NumOfDaug == 2 && NumOfEl == 2 && NumOfPos == 0 ) 
    return ElMagProc;
  // unknown process pos->pos+pos??
  if( m_MotherPDG == -11 && m_NumOfDaug == 2 && NumOfEl == 0 && NumOfPos == 2 ) 
  
    return ElMagProc;
  // unknown process pos/el->pos/el??
  if( abs(m_MotherPDG) == 11&&m_Mother->status()!=2&&m_MotherPDG==thePriPart->pdgId() &&  m_NumOfDaug == 1 &&!samePart) 
    return ElMagProc;

  // pi->pi+e+/e-; mu->mu+e+/e- ; 
  // gamma+ atom->gamma(the same) + e (compton scattering)
  if( m_NumOfDaug == 2 && (NumOfEl == 1 || NumOfPos == 1 )
      && abs(m_MotherPDG)!=11&&samePart)        return ElMagProc;



  if ( (m_MotherPDG == 111 && m_NumOfDaug == 3 && NumOfPhot == 1 && 
	NumOfEl == 1 && NumOfPos == 1)|| (m_MotherPDG == 111 && m_NumOfDaug == 4 && 
					  NumOfPhot == 0 && NumOfEl == 2 && NumOfPos == 2) )
    return  DalitzDec;  
  // Quark weak decay 
  if ( abs(m_MotherPDG)< 7 &&m_NumOfParents == 1&&m_NumOfDaug==3&&NumOfquark==1&&NumOfElNeut==1 ) return QuarkWeakDec;

  if( abs(m_MotherPDG) == 13&&NumOfNucFr!=0 )  return ElMagProc; 


  if( abs(m_MotherPDG)==6 )                    return top;  

  if( abs(m_MotherPDG)==24&&m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0 ) {
   
    const xAOD::TruthVertex* prodVert = m_MothOriVert;
    const xAOD::TruthParticle* ptrPart;
    do {
      ptrPart=prodVert->incomingParticle(0);
      prodVert=ptrPart->prodVtx();
    } while(abs(ptrPart->pdgId())==24&&prodVert!=0);
   
    if(prodVert&&prodVert->nIncomingParticles()==1&&abs(ptrPart->pdgId())==9900012)      return NuREle;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(ptrPart->pdgId())==9900014) return NuRMu;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(ptrPart->pdgId())==9900016) return NuRTau;
    return WBoson;  
  }else if( abs(m_MotherPDG)==24) return WBoson;  

  if( abs(m_MotherPDG)==23 ) return ZBoson;  
    
  //-- Exotics
  if( abs(m_MotherPDG)<7&&m_NumOfParents==2&&m_NumOfDaug==2&&NumOfEl==1&&NumOfPos==1&&
      m_partOriVert->barcode()==-5){
    int pdg1 = m_partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = m_partOriVert->incomingParticle(1)->pdgId();
    const xAOD::TruthVertex* TmpVert= m_partOriVert->outgoingParticle(0)->decayVtx();

    if(abs(pdg1)==abs(pdg2)&&TmpVert->nIncomingParticles()==2)   return ZorHeavyBoson; 
  }

  if(abs(m_MotherPDG)<7&&m_NumOfParents==2&&m_NumOfDaug==2&&(NumOfEl==1||NumOfPos==1)&&
     NumOfElNeut==1&&m_partOriVert->barcode()==-5)  {
    const xAOD::TruthVertex* TmpVert= m_partOriVert->outgoingParticle(0)->decayVtx();
    if(TmpVert->nIncomingParticles()==2) return HeavyBoson;
  }

  //MadGraphPythia ZWW*->lllnulnu
  if(m_NumOfParents==1&&m_NumOfDaug>4&&(abs(m_MotherPDG)<7||m_MotherPDG==21)){
 
    const xAOD::TruthParticle* thePartToCheck=thePriPart;
    const xAOD::TruthParticle* theMother=thePriPart->prodVtx()->incomingParticle(0);
    if( abs(theMother->pdgId())==11&&theMother->status()==2) thePartToCheck=theMother;

    bool isZboson=false;
    bool isWboson=false;
    bool skipnext=false;

    for (unsigned int ipOut=0; ipOut<m_partOriVert->nOutgoingParticles()-1; ++ipOut) {
      const xAOD::TruthParticle* theDaug=m_partOriVert->outgoingParticle(ipOut);
      const xAOD::TruthParticle* theNextDaug=m_partOriVert->outgoingParticle(ipOut+1);

      if(skipnext) {skipnext=false; continue;}
 
      if(abs(theDaug->pdgId())==11&&abs(theNextDaug->pdgId())==11 ) {
	//Zboson
	if(thePartToCheck==theDaug||thePartToCheck==theNextDaug) { isZboson=true; break;}
	skipnext=true;
      } else if(abs(theDaug->pdgId())==11&&abs(theNextDaug->pdgId())==12) {
	//WBoson       
	if(thePartToCheck==theDaug||thePartToCheck==theNextDaug) { isWboson=true; break;}
	skipnext=true;
      }
    }
    if(isWboson) return WBoson;
    if(isZboson) return ZBoson;
  }

  //--Sherpa Z->ee
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&NumOfEl==1&&NumOfPos==1) return ZBoson;


  //--Sherpa W->enu ??
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&(NumOfEl==1||NumOfPos==1)&&NumOfElNeut==1)  return WBoson;


  //--Sherpa ZZ,ZW
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==4&&
     (NumOfEl+NumOfPos+NumOfMuPl+NumOfMuMin+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==4) ) {
    int pdg1=m_partOriVert->incomingParticle(0)->pdgId();
    int pdg2=m_partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return DiBoson;
  }

  //-- McAtNLo 
  if( abs(m_MotherPDG)<7&&m_NumOfParents==2&&NumOfEl==1&&NumOfPos==1&&m_partOriVert->barcode()==-1){
    int pdg1=m_partOriVert->incomingParticle(0)->pdgId();
    int pdg2=m_partOriVert->incomingParticle(1)->pdgId();
    if(abs(pdg1)==abs(pdg2))                   return ZBoson; 
  }

  if( abs(m_MotherPDG)==25 )                   return Higgs;  

  if( abs(m_MotherPDG)==35||
      abs(m_MotherPDG)==36||
      abs(m_MotherPDG)==37)                    return HiggsMSSM; 
 
  if( abs(m_MotherPDG)==32||
      abs(m_MotherPDG)==33||
      abs(m_MotherPDG)==34)                    return HeavyBoson; 

  if( abs(m_MotherPDG)==13)                    return Mu;
  if( abs(m_MotherPDG)==15){
        
    ParticleOrigin  tauOrig= defOrigOfTau (m_mcTruthTES,m_Mother);
    ParticleType    tautype=defTypeOfTau(tauOrig);
    if(tautype==IsoTau) return tauOrig;
    else return TauLep;
  }

  if(abs(m_MotherPDG)==9900024)                return WBosonLRSM;  
  if(abs(m_MotherPDG)==9900012)                return NuREle;
  if(abs(m_MotherPDG)==9900014)                return NuRMu;
  if(abs(m_MotherPDG)==9900016)                return NuRTau;

  if (abs(m_MotherPDG) == 42 || NumOfLQ!=0 )  return LQ;  

  if( abs(m_MotherPDG)<2000040&&
      abs(m_MotherPDG)>1000001)                return SUSY;  

 
  ParticleType pType = defTypeOfHadron(m_MotherPDG);
  if( (pType==BBbarMesonPart || pType==CCbarMesonPart ) 
      && m_MothOriVert!=0&&isHardScatVrtx(m_MothOriVert))  m_isPrompt=true;

  //---- 
  return convHadronTypeToOrig(pType);

}

//-------------------------------------------------------------------------------
ParticleType MCTruthClassifier::defTypeOfMuon(ParticleOrigin MuOrig){
  //-------------------------------------------------------------------------------

 
  if( MuOrig == NonDefined)  return UnknownMuon;


  if (MuOrig == WBoson     || MuOrig == ZBoson     || MuOrig == top       ||
      MuOrig == SingleMuon || MuOrig == Higgs      || MuOrig == HiggsMSSM || 
      MuOrig == HeavyBoson || MuOrig == WBosonLRSM || MuOrig == NuREle    ||  
      MuOrig == NuRMu      || MuOrig == NuRTau     || MuOrig == LQ        || 
      MuOrig == SUSY       || MuOrig == DiBoson    || MuOrig == ZorHeavyBoson ||
      m_isPrompt)  
                                         
    return IsoMuon;

  if (MuOrig == JPsi          || MuOrig == BottomMeson  || 
      MuOrig == CharmedMeson  || MuOrig == BottomBaryon || 
      MuOrig == CharmedBaryon ||  MuOrig == TauLep      ||
      MuOrig == QuarkWeakDec)
    return  NonIsoMuon;

  //  if (MuOrig == Pion  || MuOrig == Kaon ) return  DecayMuon;
  return BkgMuon;

}

//2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defOrigOfMuon(const xAOD::TruthParticleContainer* m_mcTruthTES,
						const xAOD::TruthParticle  *thePart){
  //-------------------------------------------------------------------------------
 
  ATH_MSG_DEBUG( "Executing DefOrigOfMuon " );
 
  int PriBarcode     = thePart->barcode()%m_barcodeShift;
  const xAOD::TruthParticle* thePriPart =barcode_to_particle(m_mcTruthTES,PriBarcode);
  if(!thePriPart) return NonDefined;
  if(abs(thePriPart->pdgId())!=13) return NonDefined;
 
  const xAOD::TruthVertex*   m_partOriVert = thePriPart->prodVtx();


  //-- to define muon  outcome status
  m_ParticleOutCome = defOutComeOfMuon(thePriPart); 

  if(!m_partOriVert) return NonDefined;


  m_NumOfParents=m_partOriVert->nIncomingParticles();
  if(m_NumOfParents>1) 
    ATH_MSG_WARNING ( "DefOrigOfMuon:: muon  has more than one mother " ); 


  m_Mother      = getMother(thePriPart);
  if(!m_Mother) return NonDefined;
  m_MothOriVert   = m_Mother->prodVtx();
  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();

  // to exclude interactions mu(barcode<10^6)->mu(barcode10^6)+e
  // bool samePart=false;
  //for (HepMC::GenVertex::particles_out_const_iterator 
  //     itrDaug =m_partOriVert->particles_out_const_begin();
  //     itrDaug!=m_partOriVert->particles_out_const_end(); ++itrDaug){
  //  if( m_MotherPDG==(*itrDaug)->pdgId()&&m_MotherBarcode%m_barcodeShift==(*itrDaug)->barcode()%m_barcodeShift) samePart=true;
  // }


  if( ( abs(m_MotherPDG)==15||abs(m_MotherPDG)==24)&& m_MothOriVert!=0){  
    long pPDG(0);
    const xAOD::TruthParticle*  MotherParent(0);
    do { 
      pPDG=0;
      MotherParent=getMother(m_Mother);
      if(MotherParent) pPDG = MotherParent->pdgId();
      if(abs(pPDG)==13 || abs(pPDG)==15 || abs(pPDG)==24)  m_Mother = MotherParent;
 
    }  while ((abs(pPDG)==13 || abs(pPDG)==15 || abs(pPDG)==24) );


    if(abs(pPDG)==15 || abs(pPDG)==24 || abs(pPDG)==23 || abs(pPDG)==25 ||
       abs(pPDG)==35 || abs(pPDG)==36 || abs(pPDG)==37 || abs(pPDG)==32 ||
       abs(pPDG)==33 || abs(pPDG)==34 || abs(pPDG)==6  ||
       abs(pPDG)==9900024 || abs(pPDG)==9900012 || abs(pPDG)==9900014 || abs(pPDG)==9900016 ||      
       (abs(pPDG)<2000040&& abs(pPDG)>1000001))  m_Mother = MotherParent; 

  }

  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();
  m_MotherStatus  = m_Mother->status();
  m_MothOriVert   = m_Mother->prodVtx();
  m_partOriVert   = m_Mother->decayVtx();
  m_NumOfParents  = m_partOriVert->nIncomingParticles();
  m_NumOfDaug     = m_partOriVert->nOutgoingParticles();

  //---
  long DaugType(0);
  int  NumOfEl(0),NumOfPos(0);
  int  NumOfElNeut(0),NumOfMuNeut(0),NumOfLQ(0),NumOfquark(0),NumOfgluon(0);
  int  NumOfMuPl(0),NumOfMuMin(0);
  int  NumOfTau(0),NumOfTauNeut(0);
  for(unsigned int ipOut=0;ipOut<m_partOriVert->nOutgoingParticles();ipOut++){
    const xAOD::TruthParticle * theDaug=m_partOriVert->outgoingParticle(ipOut);
    DaugType  = theDaug->pdgId();
    if( DaugType      == 11 ) NumOfEl++;
    if( DaugType      ==-11 ) NumOfPos++;
    if( DaugType      == 13 ) NumOfMuMin++;
    if( DaugType      ==-13 ) NumOfMuPl++;
    if( abs(DaugType) == 12 ) NumOfElNeut++;
    if( abs(DaugType) == 14 ) NumOfMuNeut++;
    if( abs(DaugType) == 15 ) NumOfTau++;
    if( abs(DaugType) == 16 ) NumOfTauNeut++;
    if( abs(DaugType) == 42 ) NumOfLQ++;
    if( abs(DaugType)  < 7  ) NumOfquark++;
    if( abs(DaugType) == 21 ) NumOfgluon++;
  } // cycle itrDaug

 
  // if ( m_NumOfParents == 0 && m_NumOfDaug == 1 )   return  SingleMuon;   

  if ( abs(m_MotherPDG) == 211&&m_NumOfDaug == 2 && NumOfMuNeut == 1) 
    return   PionDecay;
 
  if ( abs(m_MotherPDG) == 321&&m_NumOfDaug == 2 && NumOfMuNeut == 1) 
    return   KaonDecay;

  if( abs(m_MotherPDG)==15 ) {                   
    ParticleOrigin  tauOrig= defOrigOfTau (m_mcTruthTES,m_Mother);
    ParticleType    tautype=defTypeOfTau(tauOrig);
    if(tautype==IsoTau) return tauOrig;
    else return TauLep;
  }

  if( abs(m_MotherPDG)==6  )                   return top;  

  // Quark weak decay 
  if ( abs(m_MotherPDG)< 7 &&m_NumOfParents == 1&&m_NumOfDaug==3&&NumOfquark==1&&NumOfMuNeut==1 ) return QuarkWeakDec;

  if( abs(m_MotherPDG)==24&&m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0 ) {
    const xAOD::TruthVertex* prodVert = m_MothOriVert;
    const xAOD::TruthParticle* itrP;
    do {
      itrP =prodVert->incomingParticle(0);
      prodVert= itrP->prodVtx();
    } while(abs(itrP->pdgId())==24&&prodVert!=0);

    if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900012)      return NuREle;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900014) return NuRMu;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900016) return NuRTau;
    return WBoson;  
  } else  if( abs(m_MotherPDG)==24) return WBoson;  

  if( abs(m_MotherPDG)==23 )                   return ZBoson;  
    
 

  //-- Exotics
  if( abs(m_MotherPDG)<7&&m_NumOfParents==2&&NumOfMuPl==1&&NumOfMuMin==1&&
      NumOfEl+NumOfPos==0&&NumOfTau==0&&m_partOriVert->barcode()==-5){
    int pdg1= m_partOriVert->incomingParticle(0)->pdgId();
    int pdg2= m_partOriVert->incomingParticle(1)->pdgId(); 
    const xAOD::TruthParticle* theDaug =m_partOriVert->outgoingParticle(0);
    const xAOD::TruthVertex* TmpVert=theDaug->decayVtx();
    if(abs(pdg1)==abs(pdg2)&&TmpVert->nIncomingParticles()==2)  return ZorHeavyBoson; 
  }

  if(abs(m_MotherPDG)<7&&m_NumOfParents==2&&(NumOfMuMin==1||NumOfMuPl==1)&&
     NumOfMuNeut==1&&NumOfEl+NumOfPos==0&&NumOfTau==0&&m_partOriVert->barcode()==-5) {
    const xAOD::TruthParticle* theDaug =m_partOriVert->outgoingParticle(0);
    const xAOD::TruthVertex* TmpVert=theDaug->decayVtx();
    if(TmpVert->nIncomingParticles()==2) return HeavyBoson;
  }
                                               
  //MadGraphPythia ZWW*->lllnulnu
 
  if(m_NumOfParents==1&&m_NumOfDaug>4&&(abs(m_MotherPDG)<7||m_MotherPDG==21)){
    bool isZboson=false;
    bool isWboson=false;
    bool skipnext=false;
    for(unsigned int ipOut=0;ipOut<m_partOriVert->nOutgoingParticles()-1;ipOut++){
      if(skipnext) {skipnext=false; continue;}
      const xAOD::TruthParticle* theDaug=m_partOriVert->outgoingParticle(ipOut); 
      const xAOD::TruthParticle* theNextDaug=m_partOriVert->outgoingParticle(ipOut+1);

      if(  abs(theDaug->pdgId())==13&&abs(theNextDaug->pdgId())==13) {
	//Zboson
	if(thePriPart==theDaug||thePriPart==theNextDaug) { isZboson=true; break;}
	skipnext=true;
      } else if(abs(theDaug->pdgId())==13&&abs(theNextDaug->pdgId())==14) {
	//WBoson       
	if(thePriPart==theDaug||thePriPart==theNextDaug) { isWboson=true; break;}
	skipnext=true;
      }
    }
    if(isWboson) return WBoson;
    if(isZboson) return ZBoson;
  }

  //--Sherpa Z->mumu
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&NumOfMuPl==1&&NumOfMuMin==1) return ZBoson;

  //--Sherpa W->munu ??
  // if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&(NumOfEl==1||NumOfPos==1)&&NumOfElNeut==1) return WBoson;
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&(NumOfMuPl==1||NumOfMuMin==1)&&NumOfMuNeut==1) return WBoson;
 

  //--Sherpa ZZ,ZW
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==4&&
     (NumOfEl+NumOfPos+NumOfMuPl+NumOfMuMin+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==4) ) {
    int pdg1=m_partOriVert->incomingParticle(0)->pdgId();
    int pdg2=m_partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return DiBoson;
  }


  //-- McAtNLo 
  if( abs(m_MotherPDG)<7&&m_NumOfParents==2&&NumOfMuPl==1&&NumOfMuMin==1&&m_partOriVert->barcode()==-1){
    int pdg1=m_partOriVert->incomingParticle(0)->pdgId();
    int pdg2=m_partOriVert->incomingParticle(0)->pdgId();
    if(abs(pdg1)==abs(pdg2))                   return ZBoson; 
  }


  if( abs(m_MotherPDG)==25 )                   return Higgs;  

  if( abs(m_MotherPDG)==35||
      abs(m_MotherPDG)==36||
      abs(m_MotherPDG)==37)                    return HiggsMSSM; 
 
  if( abs(m_MotherPDG)==32||
      abs(m_MotherPDG)==33||
      abs(m_MotherPDG)==34)                    return HeavyBoson; 

  if(abs(m_MotherPDG)==9900024)                return WBosonLRSM;  
  if(abs(m_MotherPDG)==9900012)                return NuREle;
  if(abs(m_MotherPDG)==9900014)                return NuRMu;
  if(abs(m_MotherPDG)==9900016)                return NuRTau;

  if (abs(m_MotherPDG) == 42 || NumOfLQ!=0  )  return LQ;  

  if( abs(m_MotherPDG)<2000040&&
      abs(m_MotherPDG)>1000001)                return SUSY;  

 
  ParticleType pType = defTypeOfHadron(m_MotherPDG);
  if( (pType==BBbarMesonPart || pType==CCbarMesonPart ) 
      && m_MothOriVert!=0&&isHardScatVrtx(m_MothOriVert))  m_isPrompt=true;

  return convHadronTypeToOrig(pType);

 
}
//-------------------------------------------------------------------------------
ParticleType MCTruthClassifier::defTypeOfTau(ParticleOrigin TauOrig){
  //-------------------------------------------------------------------------------

 
  if( TauOrig == NonDefined)  return UnknownTau;


  if (TauOrig == WBoson     || TauOrig == ZBoson     || TauOrig == top       ||
      TauOrig == SingleMuon || TauOrig == Higgs      || TauOrig == HiggsMSSM || 
      TauOrig == HeavyBoson || TauOrig == WBosonLRSM || TauOrig ==  NuREle   || 
      TauOrig == NuRMu      || TauOrig ==  NuRTau    || TauOrig == SUSY      ||
      TauOrig == DiBoson    || TauOrig == ZorHeavyBoson )  
    return IsoTau;

  if (TauOrig == JPsi          || TauOrig == BottomMeson  || 
      TauOrig == CharmedMeson  || TauOrig == BottomBaryon || 
      TauOrig == CharmedBaryon || TauOrig == QuarkWeakDec)
    return  NonIsoTau;

  return BkgTau;

  
}

//2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defOrigOfTau(const xAOD::TruthParticleContainer* m_mcTruthTES,
					       const xAOD::TruthParticle  *thePart){
  //-------------------------------------------------------------------------------
 
  ATH_MSG_DEBUG( "Executing DefOrigOfTau " );
 
 
  int PriBarcode     = thePart->barcode()%m_barcodeShift;
  const xAOD::TruthParticle* thePriPart = barcode_to_particle(m_mcTruthTES,PriBarcode);
  if(!thePriPart) return NonDefined;
  if(abs(thePriPart->pdgId())!=15) return NonDefined;
 
  const xAOD::TruthVertex*   m_partOriVert = thePriPart->prodVtx();


  //-- to define tau  outcome status
  m_ParticleOutCome = defOutComeOfTau(thePriPart); 

  if(!m_partOriVert) return NonDefined;

  m_NumOfParents=m_partOriVert->nIncomingParticles();
  if(m_NumOfParents>1) 
    ATH_MSG_WARNING( "DefOrigOfTau:: tau  has more than one mother " );

  m_Mother=getMother(thePriPart);
  if(!m_Mother) return NonDefined;
  m_MothOriVert = m_Mother->prodVtx();

  const xAOD::TruthParticle* MotherParent(0);
  if( abs(m_MotherPDG)==24 && m_MothOriVert!=0){  
    MotherParent=getMother(m_Mother);
    long pPDG(0);
    if(MotherParent) pPDG = MotherParent->pdgId();
    if(abs(pPDG)==6) m_Mother = MotherParent; 
  }

  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();
  m_MotherStatus  = m_Mother->status();
  m_MothOriVert   = m_Mother->prodVtx();
  m_partOriVert   = m_Mother->decayVtx();
  m_NumOfParents  = m_partOriVert->nIncomingParticles();
  m_NumOfDaug     = m_partOriVert->nOutgoingParticles();


  int  NumOfPhot(0),NumOfEl(0),NumOfPos(0);
  int  NumOfquark(0),NumOfgluon(0),NumOfElNeut(0);
  int  NumOfMuPl(0),NumOfMuMin(0),NumOfMuNeut(0);
  int  NumOfTau(0),NumOfTauNeut(0);
  long DaugType(0);

  for(unsigned int ipOut=0;ipOut<m_partOriVert->nOutgoingParticles(); ipOut++){
    DaugType = m_partOriVert->outgoingParticle(ipOut)->pdgId();
    if( abs(DaugType)  < 7  ) NumOfquark++;
    if( abs(DaugType) == 21 ) NumOfgluon++;
    if( abs(DaugType) == 12 ) NumOfElNeut++;
    if( abs(DaugType) == 14 ) NumOfMuNeut++;
    if( DaugType      == 22 ) NumOfPhot++;
    if( DaugType      == 11 ) NumOfEl++;
    if( DaugType      ==-11 ) NumOfPos++;
    if( DaugType      == 13 ) NumOfMuMin++;
    if( DaugType      ==-13 ) NumOfMuPl++;
    if( abs(DaugType) == 15 ) NumOfTau++;
    if( abs(DaugType) == 16 ) NumOfTauNeut++;
  }
 
  // if ( m_NumOfParents == 0 && m_NumOfDaug == 1 )   return  SingleTau;   

  if( abs(m_MotherPDG)==6  )                   return top;  

  if( abs(m_MotherPDG)==24&&m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0 ){
   
    const xAOD::TruthVertex* prodVert = m_MothOriVert;
    const xAOD::TruthParticle* itrP;
    do {
      itrP =prodVert->incomingParticle(0);
      prodVert= itrP->prodVtx();
    } while(abs(itrP->pdgId())==24&&prodVert!=0);

    if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900012)      return NuREle;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900014) return NuRMu;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900016) return NuRTau;
    return WBoson;  
  } else  if( abs(m_MotherPDG)==24) return WBoson;  

  if( abs(m_MotherPDG)==23 )                   return ZBoson;  
    
  if(m_NumOfParents==1&&m_NumOfDaug>4&&(abs(m_MotherPDG)<7||m_MotherPDG==21)){
    bool isZboson=false;
    bool isWboson=false;
    bool skipnext=false;
    for(unsigned int ipOut=0;m_partOriVert->nOutgoingParticles()-1;ipOut++){
      if(skipnext) {skipnext=false; continue;}
      const xAOD::TruthParticle* theDaug=m_partOriVert->outgoingParticle(ipOut);
      const xAOD::TruthParticle* theNextDaug=m_partOriVert->outgoingParticle(ipOut+1);
 
      if(  abs(theDaug->pdgId())==15&&abs(theNextDaug->pdgId())==15) {
	//Zboson
	if(thePriPart==theDaug||thePriPart==theNextDaug) { isZboson=true; break;}
	skipnext=true;
      } else if(abs(theDaug->pdgId())==15&&abs(theNextDaug->pdgId())==16) {
	//WBoson       
	if(thePriPart==theDaug||thePriPart==theNextDaug) { isWboson=true; break;}
	skipnext=true;
      }
    }
    if(isWboson) return WBoson;
    if(isZboson) return ZBoson;
  }
  //--Sherpa Z->tautau
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&NumOfTau==2){
    int pdg1=m_partOriVert->incomingParticle(0)->pdgId();
    int pdg2=m_partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return ZBoson;
  }
 
  //--Sherpa W->taunu  new
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&NumOfTau==1&&NumOfTauNeut==1) return WBoson;


  //--Sherpa ZZ,ZW
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==4&&
     (NumOfEl+NumOfPos+NumOfMuPl+NumOfMuMin+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==4) ) {
    int pdg1=m_partOriVert->incomingParticle(0)->pdgId();
    int pdg2=m_partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return DiBoson;
  }


  if( abs(m_MotherPDG)==25 )                   return Higgs;  

  if( abs(m_MotherPDG)==35||
      abs(m_MotherPDG)==36||
      abs(m_MotherPDG)==37)                    return HiggsMSSM; 
 
  if( abs(m_MotherPDG)==32||
      abs(m_MotherPDG)==33||
      abs(m_MotherPDG)==34)                    return HeavyBoson; 


  if(abs(m_MotherPDG)==9900024)                return WBosonLRSM;  
  if(abs(m_MotherPDG)==9900016)                return NuRTau;

  if( abs(m_MotherPDG)<2000040&&
      abs(m_MotherPDG)>1000001)                return SUSY;  


  if ( abs(m_MotherPDG) == 443 )               return JPsi;
 
  ParticleType pType = defTypeOfHadron(m_MotherPDG);
  return convHadronTypeToOrig(pType);

}


//-------------------------------------------------------------------------------
ParticleType MCTruthClassifier::defTypeOfPhoton(ParticleOrigin PhotOrig){
  //-------------------------------------------------------------------------------



  if (PhotOrig == NonDefined)   return UnknownPhoton;

  if (PhotOrig == WBoson     || PhotOrig == ZBoson  || PhotOrig == SinglePhot || 
      PhotOrig == Higgs      || PhotOrig == HiggsMSSM  || PhotOrig == HeavyBoson  ||
      PhotOrig == PromptPhot || PhotOrig == SUSY )   return IsoPhoton;

  if (PhotOrig == ISRPhot ||  PhotOrig == FSRPhot || PhotOrig == TauLep ||   
      PhotOrig == Mu      ||  PhotOrig == NuREle   || PhotOrig == NuRMu ||
      PhotOrig == NuRTau  )   
    return  NonIsoPhoton;
 

  return BkgPhoton;

 
}
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defOrigOfPhoton(const xAOD::TruthParticleContainer* m_mcTruthTES,
						  const xAOD::TruthParticle  *thePart)
//-------------------------------------------------------------------------------
{
 
  ATH_MSG_DEBUG( "Executing DefOrigOfPhoton " ); 

  m_Mother              = 0;
  m_MotherPDG           = 0;
  m_MotherBarcode       = 0;
  m_MotherStatus        = 0;
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherBarcode = 0;

  m_ParticleOutCome = UnknownOutCome;


  int PriBarcode = thePart->barcode()%m_barcodeShift;

  const xAOD::TruthParticle* thePriPart = barcode_to_particle(m_mcTruthTES,PriBarcode);
  if(!thePriPart) return NonDefined;
  if(abs(thePriPart->pdgId())!=22) return  NonDefined;

  const xAOD::TruthVertex*   m_partOriVert = thePriPart->prodVtx();

  //-- to define photon outcome status
  m_ParticleOutCome = defOutComeOfPhoton(thePriPart); 

  if(!m_partOriVert) return NonDefined;


  m_NumOfParents=m_partOriVert->nIncomingParticles();
  if(m_partOriVert->nIncomingParticles()>1) 
    ATH_MSG_WARNING( "DefOrigOfPhoton:: photon  has more than one mother " ); 

  m_Mother        = getMother(thePriPart);
  if(!m_Mother) return NonDefined;
  m_MotherPDG     = m_Mother->pdgId();
  m_MothOriVert   = m_Mother->prodVtx();
  m_MotherBarcode = m_Mother->barcode();
  m_MotherStatus  = m_Mother->status();
  m_partOriVert   = m_Mother->decayVtx();
  m_NumOfParents  = m_partOriVert->nIncomingParticles();
  m_NumOfDaug     = m_partOriVert->nOutgoingParticles();

  int  NumOfNucFr(0),NumOfEl(0),NumOfPos(0),NumOfMu(0),NumOfTau(0), NumOfPht(0),NumOfLQ(0);
  long DaugBarcode(0),DaugType(0), NumOfLep(0), NumOfNeut(0);
  for(unsigned int ipOut=0;ipOut<m_partOriVert->nOutgoingParticles();ipOut++){

    DaugType = m_partOriVert->outgoingParticle(ipOut)->pdgId();
    if(m_NumOfParents==1&&(m_MotherPDG==22||abs(m_MotherPDG)==11||abs(m_MotherPDG)==211)&&
       (DaugType > 1000000000 ||DaugType==0 ||DaugType == 2212||DaugType == 2112))  NumOfNucFr++;
    if(DaugType      == 22) NumOfPht++;
    if(DaugType      == 11) NumOfEl++;
    if(DaugType      ==-11) NumOfPos++;
    if(abs(DaugType) == 13) NumOfMu++;
    if(abs(DaugType) == 15) NumOfTau++;
    if(abs(DaugType) == 42) NumOfLQ++;
    if(abs(DaugType) == 11||abs(DaugType) == 13||abs(DaugType) == 15) NumOfLep++;
    if(abs(DaugType) == 12||abs(DaugType) == 14||abs(DaugType) == 16) NumOfNeut++;

    if(DaugType == m_MotherPDG ) DaugBarcode = m_partOriVert->outgoingParticle(ipOut)->barcode() ;
  } // cycle itrDaug

 

  bool foundISR=false;
  bool foundFSR=false;

  // if ( m_NumOfParents == 0 && m_NumOfDaug == 1 )    return  SinglePhot;   

 
  if ( m_NumOfParents == 1 && m_NumOfDaug == 2 && 
       DaugBarcode%m_barcodeShift==m_MotherBarcode%m_barcodeShift )   return  BremPhot;

  if ( m_NumOfParents == 1 && m_NumOfDaug == 2 && 
       abs(m_MotherPDG)==11&& NumOfPht==2 )   return  ElMagProc;

 
  // decay of W,Z and Higgs to lepton with FSR generated by Pythia 
  if(m_NumOfParents == 1 && m_NumOfDaug==2&&
     (abs(m_MotherPDG)==11||abs(m_MotherPDG)==13||abs(m_MotherPDG)==15)&&
     DaugBarcode%m_barcodeShift!=m_MotherBarcode%m_barcodeShift&& m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()==1){  
    int itr=0;
    long PartPDG=0;
    const xAOD::TruthVertex* prodVert = m_MothOriVert;
    const xAOD::TruthVertex*Vert=0;
    do { 
      Vert = prodVert;
      for(unsigned int ipIn=0;ipIn<Vert->nIncomingParticles();ipIn++){
	PartPDG   = abs(Vert->incomingParticle(ipIn)->pdgId()); 
	prodVert  = Vert->incomingParticle(ipIn)->prodVtx();
	if(PartPDG==23||PartPDG==24||PartPDG==25) foundFSR=true;
      }// cycle itrMother
      itr++;
      if(itr>100) {ATH_MSG_WARNING( "DefOrigOfPhoton:: infinite while" ); break;}
    }  while (prodVert!=0&&abs(m_MotherPDG)==PartPDG);

    if(foundFSR) return  FSRPhot;
     
  }

  // Nucl reaction 
  // gamma+Nuclear=>gamma+Nucl.Fr+Nuclons+pions
  // e+Nuclear=>e+gamma+Nucl.Fr+Nuclons+pions
  // pi+Nuclear=>gamma+Nucl.Fr+Nuclons+pions

  if ( ( m_NumOfParents == 1&&abs((m_MotherPDG)==22||abs(m_MotherPDG)==11)&&m_NumOfDaug>2&&NumOfNucFr!=0)||
       ( m_NumOfParents == 1&&abs(m_MotherPDG)==211&&m_NumOfDaug>10&&NumOfNucFr!=0) ||
       ( m_NumOfParents == 1&&m_MotherPDG==22&&m_NumOfDaug>10&&m_MotherStatus==1)||
       ( m_NumOfParents == 1&&m_MotherPDG>1000000000) 
       )     return  NucReact;

  if ( abs(m_MotherPDG)==13 &&  NumOfMu==0 )        return  Mu;   

  if ( abs(m_MotherPDG)==15 &&  NumOfTau==0 )       return  TauLep;   

  //-- to find prompt photon 
  if (abs(m_partOriVert->barcode())==5){
    int nqin(0),ngin(0),nqout(0),ngout(0),npartin(0),npartout(0),nphtout(0);
    if(m_partOriVert!=0){
      npartin = m_partOriVert->nIncomingParticles();
      for(unsigned int ipIn=0;ipIn<m_partOriVert->nIncomingParticles();ipIn++){
	if(abs(m_partOriVert->incomingParticle(ipIn)->pdgId())<7) nqin++; 
	if(abs(m_partOriVert->incomingParticle(ipIn)->pdgId())==21) ngin++; 
      }

      npartout = m_partOriVert->nOutgoingParticles();
      for(unsigned int ipOut=0;ipOut<m_partOriVert->nOutgoingParticles();ipOut++){
	if(abs(m_partOriVert->outgoingParticle(ipOut)->pdgId())<7) nqout++; 
	if(abs(m_partOriVert->outgoingParticle(ipOut)->pdgId())==21) ngout++; 
	if(abs(m_partOriVert->outgoingParticle(ipOut)->pdgId())==22) nphtout++; 

      }
    }

    if(npartout==2&&npartin==2&&
       ( ((nqin==2&&ngin==0)&&(nqout==0&&ngout==1&&nphtout==1)) ||
	 ((nqin==2&&ngin==0)&&(nqout==0&&ngout==0&&nphtout==2)) ||
	 ((nqin==1&&ngin==1)&&(nqout==1&&ngout==0&&nphtout==1)) ||
	 ((nqin==0&&ngin==2)&&(nqout==0&&ngout==0&&nphtout==2)) ||
	 ((nqin==0&&ngin==2)&&(nqout==0&&ngout==1&&nphtout==1)) 
	 )  ) return  PromptPhot; 
  }

  //-- to find initial and final state raiation and underline photons
  //-- dijets and min bias
  if(m_NumOfParents==1&&m_MotherStatus==3){

    //  const xAOD::TruthParticle* thePhotMoth = (*mcEventItr)->barcode_to_particle(m_MotherBarcode);
    //  const xAOD::TruthVertex*   MothOriVert = thePhotMoth->prodVtx();
    if(m_MothOriVert!=0&&abs(m_MothOriVert->barcode())==5) return FSRPhot;

    for(unsigned int ipOut=0;ipOut<m_partOriVert->nOutgoingParticles();ipOut++){
      if(m_partOriVert->outgoingParticle(ipOut)->status()!=3||
	 m_MotherPDG!=m_partOriVert->outgoingParticle(ipOut)->pdgId()) continue;
      const xAOD::TruthVertex* Vrtx=m_partOriVert->outgoingParticle(ipOut)->decayVtx();
      if(!Vrtx) continue;
      if(abs(Vrtx->barcode())==5) foundISR=true;
    }

    if( foundISR ) return ISRPhot; 
    else           return UndrPhot;

  }  
  
  //-- to find initial and final state raiation and underline photons
  //-- SUSY
  if(m_NumOfParents==1&&(abs(m_MotherPDG)<7||m_MotherPDG==21)){
    for(unsigned int ipOut=0;ipOut<m_partOriVert->nOutgoingParticles();ipOut++){
      if(m_MotherPDG!=m_partOriVert->outgoingParticle(ipOut)->pdgId()) continue;
      const xAOD::TruthVertex* Vrtx = m_partOriVert->outgoingParticle(ipOut)->decayVtx();
      if(!Vrtx) continue;
      if(Vrtx->nOutgoingParticles()!=1&&Vrtx->nIncomingParticles()==1) continue;
      if(Vrtx->outgoingParticle(0)->pdgId()==91) foundISR=true;
    }
    if( foundISR ) return ISRPhot; 
    else           return UndrPhot;

  }

  //-- to find final  state radiation 
  //-- Exotics
  if(m_NumOfParents==1&&abs(m_MotherPDG)==11&&m_MotherStatus==2&&m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0){
    const xAOD::TruthParticle*  itrP =m_MothOriVert->incomingParticle(0);
    if(m_MothOriVert->nIncomingParticles()==1&&abs(itrP->pdgId())==11&&itrP->prodVtx()!=0&&
       abs(itrP->prodVtx()->barcode())==5&&itrP->status()==3) return FSRPhot;
  } 


  // FSR  from Photos
  //-- Exotics- CompHep
  if(m_NumOfParents==2&&((abs(m_MotherPDG)==11&&NumOfEl==1&&NumOfPos==1)|| 
			 (abs(m_MotherPDG)==13&&NumOfMu==2)||(abs(m_MotherPDG)==15&&NumOfTau==2))) {
    int pdg1=m_partOriVert->incomingParticle(0)->pdgId();
    int pdg2=m_partOriVert->incomingParticle(1)->pdgId();
    if (abs(pdg1)==abs(pdg2))              return FSRPhot; 
  }


  if(m_NumOfParents==2&&NumOfLep==1&&NumOfNeut==1&&(abs(m_MotherPDG)==11||abs(m_MotherPDG)==12))  return FSRPhot;
  
 


  //-- Exotics - CompHep
  if (abs(m_MotherPDG)==11&& m_NumOfParents == 1 && m_NumOfDaug == 2 && (NumOfEl==1||NumOfPos==1)&&NumOfPht==1&&
      DaugBarcode%m_barcodeShift!=m_MotherBarcode%m_barcodeShift&&DaugBarcode<20000&&m_MotherBarcode<20000 )   return  FSRPhot;
 

  // FSR  from Photos
  if ( abs(m_MotherPDG)==23&&( (NumOfEl+NumOfPos==2 || NumOfEl+NumOfPos==4 )||
			       (NumOfMu==2||NumOfMu==4) ||
			       (NumOfTau==2||NumOfTau==4)) &&NumOfPht>0) 
    return  FSRPhot;

  if( abs(m_MotherPDG)==9900024&&NumOfPht>0) return  FSRPhot;

  if( abs(m_MotherPDG)==9900012&&NumOfPht>0) return  FSRPhot;
  if( abs(m_MotherPDG)==9900014&&NumOfPht>0) return  FSRPhot;
  if( abs(m_MotherPDG)==9900016&&NumOfPht>0) return  FSRPhot;
 
  if(m_NumOfParents==2&&NumOfLQ==1)          return  FSRPhot;


  //--- other process 
 

  if ( abs(m_MotherPDG)==23)                return  ZBoson;   


  if ( abs(m_MotherPDG)==24 ) {

    if(NumOfLep==1&&NumOfNeut==1&&m_NumOfDaug==NumOfLep+NumOfNeut+NumOfPht) return  FSRPhot;

    if(m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0){
      const xAOD::TruthVertex* prodVert = m_MothOriVert;
      const xAOD::TruthParticle*  itrP;
      do {
	itrP =prodVert->incomingParticle(0);
	prodVert= itrP->prodVtx();
      } while(abs(itrP->pdgId())==24&&prodVert!=0);

      if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==15)           return TauLep;
      else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==13)      return Mu;
      else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900012) return NuREle;
      else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900014) return NuRMu;
      else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900016) return NuRTau;

    } else  return  WBoson;
  }                 

  //MadGraphPythia ZWW*->lllnulnu
  if(m_NumOfParents==1&&m_NumOfDaug>4&&(abs(m_MotherPDG)<7||m_MotherPDG==21)){
    bool isZboson=false;
    bool isWboson=false;
    bool skipnext=false;
    //cout<<"check electrons "<<m_partOriVert->nOutgoingParticles()<<endl;
    //int iter=0;
    for(unsigned int ipOut=0;ipOut<m_partOriVert->nOutgoingParticles()-1;ipOut++){
      if(skipnext) {skipnext=false; continue;}
      const xAOD::TruthParticle* theDaug=m_partOriVert->outgoingParticle(ipOut);
      const xAOD::TruthParticle* theNextDaug=m_partOriVert->outgoingParticle(ipOut+1);
      //cout<<"not skipped  theDaug "<<theDaug<<" theNextDaug  "<<theNextDaug<<endl;

      if(abs(theDaug->pdgId())==15&&abs(theNextDaug->pdgId())==15 ){
	//Zboson
	//cout<<"Zboson "<<thePriPart<<"   "<<theDaug<<"   "<<theNextDaug<<endl;
	if(thePriPart==theDaug||thePriPart==theNextDaug) { isZboson=true; break;}
	skipnext=true;
      } else if( abs(theDaug->pdgId())==15&&abs(theNextDaug->pdgId())==16) {
	//WBoson       
	//cout<<"Wboson "<<thePriPart<<"   "<<theDaug<<"   "<<theNextDaug<<endl;
	if(thePriPart==theDaug||thePriPart==theNextDaug) { isWboson=true; break;}
	skipnext=true;
      }
      //cout<<"here  "<<endl;
    }
    //cout<<"=============== end of cycle  isWboson  "<<isWboson<<" isZboson "<<isZboson<<endl;
    if(isWboson) return WBoson;
    if(isZboson) return ZBoson;
  }
 
  //--Sherpa ZZ,ZW+FSR
  if(m_NumOfParents==4&&(m_NumOfDaug-NumOfPht)==4&&(NumOfLep+NumOfNeut==4)) {
    int pdg1=m_partOriVert->incomingParticle(0)->pdgId();
    int pdg2=m_partOriVert->incomingParticle(1)->pdgId();
    int pdg3=m_partOriVert->incomingParticle(2)->pdgId();
    int pdg4=m_partOriVert->incomingParticle(3)->pdgId();
    if(abs(pdg1)<17&&abs(pdg1)>10&&abs(pdg2)<17&&abs(pdg2)>10&&abs(pdg3)<17&&abs(pdg3)>10&&
       abs(pdg4)<17&&abs(pdg4)>10 ) return FSRPhot;
  }


  if ( abs(m_MotherPDG)==25 )               return  Higgs;   

  if ( abs(m_MotherPDG)==111 )              return  PiZero;   

  if( abs(m_MotherPDG)==35||
      abs(m_MotherPDG)==36||
      abs(m_MotherPDG)==37)                return HiggsMSSM; 
 
  if( abs(m_MotherPDG)==32||
      abs(m_MotherPDG)==33||
      abs(m_MotherPDG)==34||      
      abs(m_MotherPDG)==5100039 // KK graviton
    )                                       return HeavyBoson; 

  if( abs(m_MotherPDG)<2000040&&
      abs(m_MotherPDG)>1000001)             return SUSY; 
 
 

  ParticleType pType = defTypeOfHadron(m_MotherPDG);
  if( (pType==BBbarMesonPart || pType==CCbarMesonPart ) 
      && m_MothOriVert!=0&&isHardScatVrtx(m_MothOriVert))  m_isPrompt=true;
  return convHadronTypeToOrig(pType);



}
//-----------------------------------------------------------------------------------------
const xAOD::TruthParticle*
MCTruthClassifier::egammaClusMatch(const xAOD::CaloCluster* clus, bool isFwrdEle){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing egammaClusMatch " );

  const xAOD::TruthParticle* theMatchPart=0;

  //retrieve collection and get a pointer
 
  const xAOD::TruthParticleContainer  * xTruthParticleContainer;
  StatusCode sc = evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName);
  if (sc.isFailure()||!xTruthParticleContainer){
    ATH_MSG_WARNING( "No  xAODTruthParticleContainer "<<m_xaodTruthParticleContainerName<<" found" ); 
    return theMatchPart;
  }
  ATH_MSG_DEBUG( "xAODTruthParticleContainer  " << m_xaodTruthParticleContainerName<<" successfully retrieved " );

  const xAOD::TruthParticle* thePhoton(0);
  const xAOD::TruthParticle* theLeadingPartInCone(0);
  const xAOD::TruthParticle* theBestPartOutCone(0);
  const xAOD::TruthParticle* theBestPartdR(0);
  double LeadingPhtPT(0),LeadingPartPT(0);
  double LeadingPhtdR(999.),LeadingPartdR(999.),BestPartdR(999.);

  double etaClus = clus->etaBE(2);
  double phiClus = clus->phiBE(2);
  if (etaClus < -900) etaClus = clus->eta();
  if (phiClus < -900) phiClus = clus->phi();

  std::vector<const xAOD::TruthParticle*> tps;
  if( !m_truthInConeTool->particlesInCone(etaClus,phiClus,0.5,tps) ) {
    ATH_MSG_WARNING( "Truth Particle in Cone failed" ); 
    return theMatchPart;
  }

  for(const auto thePart : tps){
    // loop over the stable particle  
    if(thePart->status()!=1) continue;
    // excluding G4 particle 
    if(thePart->barcode()>=m_barcodeG4Shift) continue;
    long iParticlePDG = thePart->pdgId(); 
    // excluding neutrino
    if(abs(iParticlePDG)==12||abs(iParticlePDG)==14||abs(iParticlePDG)==16) continue;

    double pt = thePart->pt()/GeV;
    double q  = partCharge(thePart);
    // exclude charged particles with pT<1 GeV
    if(q!=0&&pt<m_pTChargePartCut)  continue;
    if(q==0&&pt<m_pTNeutralPartCut) continue;

    // eleptical cone  for extrapolations m_partExtrConePhi X m_partExtrConeEta 
    if(!isFwrdEle&&m_ROICone&& pow((detPhi(phiClus,thePart->phi())/m_partExtrConePhi),2)+
       pow((detEta(etaClus,thePart->eta())/m_partExtrConeEta),2)>1.0 ) continue;

    //Also check if the clus and true have different sign , i they need both to be <0 or >0 
    if(isFwrdEle && //It is forward and  
       (((etaClus<0) - (thePart->eta()<0) !=0) //The truth eta has different sign wrt to the fwd electron
	|| (fabs(thePart->eta())<m_FwdElectronTruthExtrEtaCut)  //or the truth is less than 2.4 (default cut)
	|| (fabs(thePart->eta()-etaClus)> m_FwdElectronTruthExtrEtaWindowCut) //or if the delta Eta between el and truth is  > 0.15
	) //then do no extrapolate this truth Particle for this fwd electron
       ) continue;

    double dR(-999.);
    bool isNCone=false;
    bool isExt = genPartToCalo(clus, thePart,isFwrdEle, dR, isNCone);
    if (!isExt) continue;

    m_egPartPtr.push_back(thePart);
    m_egPartdR.push_back(dR);

    theMatchPart = barcode_to_particle(xTruthParticleContainer,thePart->barcode()%m_barcodeShift);
    m_egPartClas.push_back(particleTruthClassifier(theMatchPart));

    // the leading photon  inside narrow eleptical cone  m_partExtrConePhi X m_partExtrConeEta 
    if(!isFwrdEle&&iParticlePDG==22&&isNCone&&pt>LeadingPhtPT)   { thePhoton = thePart; LeadingPhtPT=pt; LeadingPhtdR=dR;}
    // leading particle (excluding photon) inside narrow eleptic cone m_partExtrConePhi X m_partExtrConeEta 
    if(!isFwrdEle&&iParticlePDG!=22&&isNCone&&pt>LeadingPartPT)  { theLeadingPartInCone = thePart; LeadingPartPT = pt; LeadingPartdR = dR;};
    // the best dR matched particle outside  narrow eleptic cone m_partExtrConePhi X m_partExtrConeEta 
    if(!isFwrdEle&&!isNCone&&dR<BestPartdR)                      { theBestPartOutCone = thePart; BestPartdR = dR; };
    // for forward electrons
    if(isFwrdEle&&dR<BestPartdR)                                 { theBestPartdR = thePart; BestPartdR = dR; };

  } // end cycle for Gen particle 
  

  if(thePhoton!=0) {
    theMatchPart = barcode_to_particle(xTruthParticleContainer,thePhoton->barcode()%m_barcodeShift); m_deltaRMatch=LeadingPhtdR;
  } else if(theLeadingPartInCone!=0) {
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theLeadingPartInCone->barcode()%m_barcodeShift);  m_deltaRMatch=LeadingPartdR;  
  } else if(theBestPartOutCone!=0) {
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theBestPartOutCone->barcode()%m_barcodeShift); m_deltaRMatch=BestPartdR;
  } else if(isFwrdEle&&theBestPartdR!=0) {
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theBestPartdR->barcode()%m_barcodeShift); m_deltaRMatch=BestPartdR;
  } else theMatchPart = 0;

  if(isFwrdEle||theMatchPart!=0||!m_inclG4part)   {return theMatchPart;}

  // additional loop over G4 particles for unmatched egamma photons
  // requested by Photon's group people 
  for(const auto thePart : tps){
    // loop over the stable particle  
    if(thePart->status()!=1) continue;
    // only G4 particle including None Primary with barcode > 10^6 
    if(thePart->barcode()<m_barcodeG4Shift) continue;
    long iParticlePDG = thePart->pdgId(); 
    // exclude neutrino
    if(abs(iParticlePDG)==12||abs(iParticlePDG)==14||abs(iParticlePDG)==16) continue;
    // exclude particles interacting into the detector volume
    if(thePart->decayVtx()!=0) continue;

    if( pow((detPhi(phiClus,thePart->phi())/m_partExtrConePhi),2)+
	pow((detEta(etaClus,thePart->eta())/m_partExtrConeEta),2)>1.0 ) continue;

    double pt = thePart->pt()/GeV;
    double q  = partCharge(thePart);
    // exclude charged particles with pT<1 GeV
    if(q!=0&&pt<m_pTChargePartCut )  continue;
    if(q==0&&pt<m_pTNeutralPartCut)  continue;

    double dR(-999.);
    bool isNCone=false;
    bool isExt = genPartToCalo(clus, thePart, false , dR, isNCone);
    if(!isExt) continue;

    m_egPartPtr.push_back(thePart);
    m_egPartdR.push_back(dR);

    theMatchPart = barcode_to_particle(xTruthParticleContainer,thePart->barcode()%m_barcodeShift);
    m_egPartClas.push_back(particleTruthClassifier(theMatchPart));

    // the leading photon  inside narrow eleptical cone m_phtClasConePhi  X m_phtClasConeEta
    if(iParticlePDG==22&&isNCone&&pt>LeadingPhtPT)   { thePhoton = thePart; LeadingPhtPT=pt; LeadingPhtdR=dR;}
    // leading particle (excluding photon) inside narrow eleptic cone m_phtClasConePhi  X m_phtClasConeEta
    if(iParticlePDG!=22&&isNCone&&pt>LeadingPartPT)  { theLeadingPartInCone = thePart; LeadingPartPT = pt; LeadingPartdR = dR;};
    // the best dR matched particle outside  narrow eleptic cone cone m_phtClasConePhi  X m_phtClasConeEta
    if(!isNCone&&dR<BestPartdR)                      { theBestPartOutCone = thePart; BestPartdR = dR; };

  } // end cycle for G4 particle

  if( thePhoton!=0){
    theMatchPart = barcode_to_particle(xTruthParticleContainer,thePhoton->barcode()%m_barcodeShift); m_deltaRMatch=LeadingPhtdR;
  } else if( theLeadingPartInCone!=0) {
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theLeadingPartInCone->barcode()%m_barcodeShift); m_deltaRMatch=LeadingPartdR;
  } else if( theBestPartOutCone!=0) { 
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theBestPartOutCone->barcode()%m_barcodeShift); m_deltaRMatch=BestPartdR;
  } else theMatchPart = 0;

  ATH_MSG_DEBUG( "succeeded  egammaClusMatch ");
  return theMatchPart;
}
//--------------------------------------------------------------
bool MCTruthClassifier::genPartToCalo(const xAOD::CaloCluster* clus, 
				      const xAOD::TruthParticle* thePart,
				      bool isFwrdEle, 
				      double& dRmatch,
				      bool  & isNarrowCone)  {
  //--------------------------------------------------------------
  dRmatch      = -999.;
  isNarrowCone = false;

  if ( thePart == 0 ) return false ;

  const Trk::CaloExtension* caloExtension = 0;
  if( !m_caloExtensionTool->caloExtension(*thePart,caloExtension,m_useCaching) || caloExtension->caloLayerIntersections().empty() ){
    ATH_MSG_WARNING("extrapolation of Truth Particle with eta  " << thePart->eta() 
		    <<" and Pt " << thePart->pt() << " to calo failed");
    return false;
  }

  Trk::TrackParametersIdHelper parsIdHelper;
  double etaCalo= -99;
  double phiCalo= -99; 

   // define calo sample
  CaloCell_ID::CaloSample sample= CaloSampling::EMB2;
  if ( (clus->inBarrel()  && !clus->inEndcap()) ||
       (clus->inBarrel()  &&  clus->inEndcap() &&
        clus->eSample(CaloSampling::EMB2) >= clus->eSample(CaloSampling::EME2) ) ) {
    // Barrel
    sample = CaloSampling::EMB2;
    
  } else if( ( !clus->inBarrel()  && clus->inEndcap() && !isFwrdEle) ||
             (  clus->inBarrel()  && clus->inEndcap() && 
                clus->eSample(CaloSampling::EME2) > clus->eSample(CaloSampling::EMB2) ) ) {
    // End-cap
    sample = CaloSampling::EME2; 
  } else if( isFwrdEle && clus->inEndcap()) {            
    // FCAL
    sample= CaloSampling::FCAL2;
    
  } else return false ;

  // loop over calo layers
  for( auto cur = caloExtension->caloLayerIntersections().begin(); cur != caloExtension->caloLayerIntersections().end() ; ++cur ){
      
    // only use entry layer
    if( !parsIdHelper.isEntryToVolume((*cur)->cIdentifier()) ) continue;
      
    CaloSampling::CaloSample sampleEx = parsIdHelper.caloSample((*cur)->cIdentifier());
    if( sampleEx != CaloSampling::EMB2 && sampleEx != CaloSampling::EME2 && sampleEx != CaloSampling::FCAL2 ) continue;
    
    if( sampleEx == sample || etaCalo == -99 ){
      etaCalo = (*cur)->position().eta();
      phiCalo = (*cur)->position().phi();
      if( sampleEx == sample ) break;
    }
  }

  double phiClus = clus->phiBE(2);
  double etaClus = clus->etaBE(2);
  if (etaClus < -900) etaClus = clus->eta();
  if (phiClus < -900) phiClus = clus->phi();

  //--FixMe
  if(isFwrdEle||(etaClus==0.&&phiClus==0.)) {
    phiClus = clus->phi();
    etaClus = clus->eta();  
  }

  double dPhi    = detPhi(phiCalo,phiClus);
  double dEta    = detEta(etaCalo,etaClus);
  dRmatch        = rCone(dPhi,dEta);
  
  if((!isFwrdEle&&dRmatch>m_phtdRtoTrCut)||
     (isFwrdEle&&dRmatch>m_fwrdEledRtoTrCut))    return false ;

  if(!isFwrdEle&&pow(dPhi/m_phtClasConePhi,2)+pow(dEta/m_phtClasConeEta,2)<=1.0) isNarrowCone=true;

  return  true;
}

//---------------------------------------------------------------------------------
float MCTruthClassifier::detPhi(float x, float y)
//---------------------------------------------------------------------------------
{
  float det;
  det=x-y;
  if(det  >  M_PI) det=det-2.*M_PI; 
  if(det < - M_PI) det=det+2.*M_PI;
  return fabs(det);
}
//---------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::convHadronTypeToOrig(ParticleType pType){
  //---------------------------------------------------------------------------------
  if(pType==BBbarMesonPart&&
     abs(m_MotherPDG==443))         return JPsi;
  else if(pType==BBbarMesonPart)    return BBbarMeson;
  else if(pType==BottomMesonPart)   return BottomMeson;
  else if(pType==BottomBaryonPart)  return BottomBaryon;
  else if(pType==CCbarMesonPart)    return CCbarMeson;
  else if(pType==CharmedMesonPart)  return CharmedMeson;
  else if(pType==CharmedBaryonPart) return CharmedBaryon;
  else if(pType==StrangeBaryonPart) return StrangeBaryon;
  else if(pType==StrangeMesonPart)  return StrangeMeson;
  else if(pType==LightBaryonPart)   return LightBaryon;
  else if(pType==LightMesonPart)    return LightMeson;
  else                              return NonDefined;
}
//---------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defHadronType(long pdg){
  //---------------------------------------------------------------------------------

  if ( abs(pdg) == 443 )     return JPsi;

  int q1=(abs(pdg)/1000)%10;
  int q2=(abs(pdg)/100)%10;
  int q3=(abs(pdg)/10)%10;

  // di quark
  // if( q3 == 0 && q2 >=q3 )   cout<<"di quark"<<endl;
 
  if( q1 == 0 && q2 == 5 && q3 == 5 )            return BBbarMeson;  
  else if( q1 == 0 && q3<5 && q3>0 && q2 == 5 )  return BottomMeson;
  else if( q1 == 0 && q3 == 4 && q2 == 4 )       return CCbarMeson;
  else if(q1==0&&q3<4&&q3>0&&q2==4)              return CharmedMeson;
  else if( q1 == 5 )                             return BottomBaryon;
  else if( q1 == 4 )                             return CharmedBaryon;
  else if( q1 == 3 )                             return StrangeBaryon;
  else if( q1 == 2 || q1 == 1 )                  return LightBaryon;
  else if((q1==0&&q2==3&&q3<3&&q3>0)||abs(pdg)==130)  
    return StrangeMeson;
  else if( (q1==0&&(q3==1||q3==2) && (q2==1||q2==2)) || (q1==0&&q3==3&&q2==3))
    return  LightMeson;
  else                                           return  NonDefined; 
 
}

//---------------------------------------------------------------------------------
ParticleType MCTruthClassifier::defTypeOfHadron(long pdg){
  //---------------------------------------------------------------------------------

  int q1=(abs(pdg)/1000)%10;
  int q2=(abs(pdg)/100)%10;
  int q3=(abs(pdg)/10)%10;

  // di quark
  // if( q3 == 0 && q2 >=q3 )   cout<<"di quark"<<endl;
 
  if( q1 == 0 && q2 == 5 && q3 == 5 )            return BBbarMesonPart;  
  else if( q1 == 0 && q3<5 && q3>0 && q2 == 5 )  return BottomMesonPart;
  else if( q1 == 0 && q3 == 4 && q2 == 4 )       return CCbarMesonPart;
  else if(q1==0&&q3<4&&q3>0&&q2==4)              return CharmedMesonPart;
  else if( q1 == 5 )                             return BottomBaryonPart;
  else if( q1 == 4 )                             return CharmedBaryonPart;
  else if( q1 == 3 )                             return StrangeBaryonPart;
  else if( q1 == 2 || q1 == 1 )                  return LightBaryonPart;
  else if((q1==0&&q2==3&&q3<3&&q3>0)||abs(pdg)==130)  
    return StrangeMesonPart;
  else if( (q1==0&&(q3==1||q3==2) && (q2==1||q2==2)) || (q1==0&&q3==3&&q2==3))
    return  LightMesonPart;
  else                                           return  Unknown; 
 
}

//-------------------------------------------------------------------------------
bool  MCTruthClassifier::isHardScatVrtx(const xAOD::TruthVertex* pVert){
  //-------------------------------------------------------------------------------

  if(pVert==0) return false;

  const xAOD::TruthVertex* pV = pVert; 
  int numOfPartIn(0),pdg(0),NumOfParton(0);

  do { 
    pVert = pV;
    numOfPartIn = pVert->nIncomingParticles();
    pdg         = pVert->incomingParticle(0)->pdgId();
    pV          = pVert->incomingParticle(0)->prodVtx();
  }  while (numOfPartIn==1&&(abs(pdg)<81||abs(pdg)>100)&&pV!=0);

  if(numOfPartIn==2) { 
    for(unsigned int ipIn=0;ipIn<pVert->nIncomingParticles();ipIn++){
      if(abs(pVert->incomingParticle(ipIn)->pdgId())<7||
	 pVert->incomingParticle(ipIn)->pdgId()==21) NumOfParton++;
    }
    if(NumOfParton==2) return true;
  }

  return false; 
}

//---------------------------------------------------------------------------------
bool MCTruthClassifier::isHadron(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------
 
 
  bool isPartHadron = false;
  if(thePart!=0) { 
    long pdg=thePart->pdgId(); 
    ParticleType pType = defTypeOfHadron(pdg);
    // ParticleOrigin  partOrig=convHadronTypeToOrig(pType);
    //if(partOrig!=NonDefined) isPartHadron = true;
    if(pType!=Unknown) isPartHadron = true;
    //--exclude protons from beam 
    if(pdg==2212&&thePart->status()==3) isPartHadron=false;
  }
  return isPartHadron;
}

//---------------------------------------------------------------------------------
const xAOD::TruthParticle*  MCTruthClassifier::getMother(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------
  ATH_MSG_DEBUG( "executing getMother" ); 

  const xAOD::TruthVertex*   partOriVert = thePart->prodVtx();

  long partPDG       = thePart->pdgId();
  int  partBarcode   = thePart->barcode()%m_barcodeShift;
  long MotherPDG(0);

  const xAOD::TruthVertex*   MothOriVert(0);
  const xAOD::TruthParticle* theMoth(0);

  if(!partOriVert) return theMoth;

  int itr=0;
  do { 
    if(itr!=0)  partOriVert = MothOriVert; 
    for(unsigned int ipIn=0;ipIn<partOriVert->nIncomingParticles();ipIn++){
      theMoth=partOriVert->incomingParticle(ipIn);
      MotherPDG   = theMoth->pdgId();
      MothOriVert = theMoth->prodVtx();
      if(MotherPDG==partPDG) break;
    }
    itr++;
    if(itr>100) { ATH_MSG_WARNING( "getMother:: infinite while" );  break;}
  }  while (MothOriVert!=0&&MotherPDG==partPDG&&partBarcode<m_barcodeG4Shift);
 

  ATH_MSG_DEBUG( "succeded getMother" ); 
  return theMoth;
}

//---------------------------------------------------------------------------------
const xAOD::TruthVertex*  MCTruthClassifier::findEndVert(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------
  const xAOD::TruthVertex*  EndVert = thePart->decayVtx();
  const xAOD::TruthVertex* pVert(0);
  if(EndVert!=0){
    do { 
      bool samePart=false;
      pVert=0;
      for(unsigned int ipOut=0;ipOut<EndVert->nOutgoingParticles();ipOut++){
	const xAOD::TruthParticle * itrDaug=EndVert->outgoingParticle(ipOut);
	if(( (itrDaug->barcode()%m_barcodeShift==thePart->barcode()%m_barcodeShift)||
	     // brem on generator level for tau 
	     (EndVert->nOutgoingParticles()==1&&EndVert->nIncomingParticles()==1&&
	      itrDaug->barcode()<m_barcodeG4Shift&&thePart->barcode()<m_barcodeG4Shift) 
	     ) &&
	   itrDaug->pdgId() == thePart->pdgId()) {samePart=true; pVert=itrDaug ->decayVtx();}
      } // cycle itrDaug
      if(samePart) EndVert=pVert;
    }  while (pVert!=0);

  } // EndVert

  return EndVert;
}
//---------------------------------------------------------------------------------
ParticleOutCome MCTruthClassifier::defOutComeOfElectron(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------
 
  ParticleOutCome PartOutCome=UnknownOutCome;

  const xAOD::TruthVertex* EndVert = findEndVert(thePart);

  if(EndVert==0) return  NonInteract;

  int   NumOfElecDaug(0),ElecOutNumOfNucFr(0),ElecOutNumOfElec(0),NumOfHadr(0);
  long  EndDaugType(0);
  NumOfElecDaug=EndVert->nOutgoingParticles();
  for(unsigned int ipOut=0;ipOut<EndVert->nOutgoingParticles();ipOut++){
    EndDaugType    =  EndVert->outgoingParticle(ipOut)->pdgId();
    if(abs(EndDaugType)==11)  ElecOutNumOfElec++;
    if(isHadron(EndVert->outgoingParticle(ipOut))) NumOfHadr++;
    if(EndDaugType >  1000000000||EndDaugType==0||abs(EndDaugType) == 2212||abs(EndDaugType) == 2112) ElecOutNumOfNucFr++;
  } // cycle itrDaug

  if(ElecOutNumOfNucFr!=0||NumOfHadr!=0)     PartOutCome = NuclInteraction;
  if(ElecOutNumOfElec==1&&NumOfElecDaug==1)  PartOutCome = ElectrMagInter;

  return  PartOutCome;
}
//---------------------------------------------------------------------------------
ParticleOutCome MCTruthClassifier::defOutComeOfMuon(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------
 
  ParticleOutCome PartOutCome=UnknownOutCome;

  const xAOD::TruthVertex*   EndVert = findEndVert(thePart);
 
  if(EndVert==0) return  NonInteract;
  int   NumOfMuDaug(0),MuOutNumOfNucFr(0),NumOfHadr(0);
  int   NumOfEleNeutr(0),NumOfMuonNeutr(0),NumOfElec(0);
  long  EndDaugType(0);

  NumOfMuDaug=EndVert->nOutgoingParticles();
  for(unsigned int ipOut=0;ipOut<EndVert->nOutgoingParticles();ipOut++){
    EndDaugType    =  EndVert->outgoingParticle(ipOut)->pdgId();
    if(abs(EndDaugType)==11)  NumOfElec++;
    if(abs(EndDaugType)==12)  NumOfEleNeutr++;
    if(abs(EndDaugType)==14)  NumOfMuonNeutr++;
    if(isHadron(EndVert->outgoingParticle(ipOut))) NumOfHadr++;
    if(EndDaugType >  1000000000||EndDaugType==0||abs(EndDaugType) == 2212||abs(EndDaugType) == 2112) MuOutNumOfNucFr++;
  } // cycle itrDaug

  if(MuOutNumOfNucFr!=0||NumOfHadr!=0)                                   PartOutCome = NuclInteraction;
  if(NumOfMuDaug==3&&NumOfElec==1&&NumOfEleNeutr==1&&NumOfMuonNeutr==1)  PartOutCome = DecaytoElectron;

  return  PartOutCome;
}
//---------------------------------------------------------------------------------
ParticleOutCome MCTruthClassifier::defOutComeOfTau(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------
 
  ATH_MSG_DEBUG( "Executing defOutComeOfTau" ); 

  ParticleOutCome PartOutCome=UnknownOutCome;

  const xAOD::TruthVertex*   EndVert = findEndVert(thePart);

  if(EndVert==0) return  NonInteract;
  int   NumOfTauDaug(0),NumOfNucFr(0);
  int   NumOfElec(0),NumOfElecNeut(0),NumOfTauNeut(0);
  int   NumOfMuon(0),NumOfMuonNeut(0),NumOfPhot(0),NumOfPi(0),NumOfKaon(0);

  NumOfTauDaug=EndVert->nOutgoingParticles();

  m_tauFinalStatePart = findFinalStatePart(EndVert);

  for(int i=0;i<(int) m_tauFinalStatePart.size();i++){
    long pdg=m_tauFinalStatePart[i]->pdgId();
    if(abs(pdg)==11)       NumOfElec++;  
    else if(abs(pdg)==13)  NumOfMuon++;  
    else if(abs(pdg)==12)  NumOfElecNeut++;  
    else if(abs(pdg)==14)  NumOfMuonNeut++;  
    else if(abs(pdg)==16)  NumOfTauNeut++;  
    else if(pdg==22)       NumOfPhot++;  
    else if(abs(pdg)==211) NumOfPi++;
    else if(abs(pdg)==321) NumOfKaon++;
    else if(pdg > 1000000000||pdg==0) NumOfNucFr++;
  }


  if(NumOfNucFr!=0)                                         PartOutCome = NuclInteraction;
  if((NumOfTauDaug==3&&NumOfElec==1&&NumOfElecNeut==1)||
     (NumOfTauDaug==(3+NumOfPhot)&&NumOfElecNeut==1))       PartOutCome = DecaytoElectron;
  if((NumOfTauDaug== 3&&NumOfMuon==1&&NumOfMuonNeut==1)||
     (NumOfTauDaug==(3+NumOfPhot)&&NumOfMuonNeut==1))       PartOutCome = DecaytoMuon;

  if(NumOfPi==1||NumOfKaon==1)    PartOutCome = OneProng;
  if(NumOfPi+NumOfKaon==3)        PartOutCome = ThreeProng;
  if(NumOfPi+NumOfKaon==5)        PartOutCome = FiveProng;


  ATH_MSG_DEBUG( "defOutComeOfTau succeeded" );

  return  PartOutCome;

}
//---------------------------------------------------------------------------------
std::vector<const xAOD::TruthParticle*>  
MCTruthClassifier::findFinalStatePart(const xAOD::TruthVertex* EndVert){
  //---------------------------------------------------------------------------------
 

  //long  EndDaugType(0);
  std::vector<const xAOD::TruthParticle*> finalStatePart;

  if(EndVert==0) return finalStatePart ;

  for(unsigned int ipOut=0;ipOut<EndVert->nOutgoingParticles();ipOut++){

    const xAOD::TruthParticle* thePart=EndVert->outgoingParticle(ipOut);
 
    finalStatePart.push_back(thePart);
    if(thePart->status()!=1) {

      const xAOD::TruthVertex* pVert = findEndVert(thePart);
      std::vector<const xAOD::TruthParticle*> vecPart;
      if(pVert!=0) {
	vecPart  = findFinalStatePart(pVert);
	if(vecPart.size()!=0)  for(int i=0;i<(int)vecPart.size();i++) finalStatePart.push_back(vecPart[i]);
      } 
    } 

  } // cycle itrDaug

  return finalStatePart;
}
//---------------------------------------------------------------------------------
ParticleOutCome MCTruthClassifier::defOutComeOfPhoton(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------
 
  ParticleOutCome PartOutCome=UnknownOutCome;
  const xAOD::TruthVertex*   EndVert = findEndVert(thePart);
  if(EndVert==0) return  UnConverted;

  int  PhtOutNumOfNucFr(0),PhtOutNumOfEl(0),PhtOutNumOfPos(0);
  int  NumOfPhtDaug(0),PhtOutNumOfHadr(0);
  // int  PhtOutNumOfPi(0);
  long EndDaugType(0);

  NumOfPhtDaug=EndVert->nOutgoingParticles();
  for(unsigned int ipOut=0;ipOut<EndVert->nOutgoingParticles();ipOut++){
    EndDaugType    =  EndVert->outgoingParticle(ipOut)->pdgId();
    if(EndDaugType >  1000000000||EndDaugType==0||abs(EndDaugType) == 2212||abs(EndDaugType) == 2112)  PhtOutNumOfNucFr++;
    if(EndDaugType == 11) PhtOutNumOfEl++;
    if(EndDaugType ==-11) PhtOutNumOfPos++;
    // if(abs(EndDaugType)==211||EndDaugType==111) PhtOutNumOfPi++;
    if(isHadron( EndVert->outgoingParticle(ipOut))) PhtOutNumOfHadr++;
  } // cycle itrDaug

  if(PhtOutNumOfEl==1&&PhtOutNumOfPos==1&&NumOfPhtDaug==2)      PartOutCome = Converted;
  if((NumOfPhtDaug>1&&PhtOutNumOfNucFr!=0)||PhtOutNumOfHadr>0 ) PartOutCome = NuclInteraction;

  return  PartOutCome;

}
//---------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::checkOrigOfBkgElec(const xAOD::TruthParticle* theEle){
  //---------------------------------------------------------------------------------
 
  ATH_MSG_DEBUG( "executing CheckOrigOfBkgElec  " << theEle );

  m_BkgElecMother = 0;

  std::pair<ParticleType,ParticleOrigin> part;
  part.first  = Unknown;
  part.second = NonDefined;
 
  if(theEle==0) return part;

  const xAOD::TruthParticleContainer  * xTruthParticleContainer;
  StatusCode sc = evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName);
  if (sc.isFailure()||!xTruthParticleContainer){
    ATH_MSG_WARNING( "No  xAODTruthParticleContainer "<<m_xaodTruthParticleContainerName<<" found" ); 
    return part;
  }
  ATH_MSG_DEBUG( "xAODTruthParticleContainer  " << m_xaodTruthParticleContainerName<<" successfully retrieved " );


  part=particleTruthClassifier(theEle);
 
  if(part.first!=BkgElectron||part.second!=PhotonConv) return part;

  const xAOD::TruthParticle* thePart(0);

  if(abs(m_PhotonMotherPDG)==11||abs(m_PhotonMotherPDG)==13|| abs(m_PhotonMotherPDG)==15||isHadron(m_PhotonMother)){
    do {
      thePart = barcode_to_particle(xTruthParticleContainer,m_PhotonMotherBarcode%m_barcodeShift);
      if(thePart==0) break;
      part.first=Unknown; part.second=NonDefined;
      part=particleTruthClassifier(thePart);
    } while (part.first == BkgElectron&& part.second==PhotonConv&&
	     (abs(m_PhotonMotherPDG)==11||abs(m_PhotonMotherPDG)==13||
	      abs(m_PhotonMotherPDG)==15||isHadron(m_PhotonMother)));

 
    if(part.first == BkgElectron&& part.second==PhotonConv) {
      // in case of photon from gen particle  classify photon
      //part=particleTruthClassifier(m_Mother); 
      thePart = barcode_to_particle(xTruthParticleContainer,m_MotherBarcode%m_barcodeShift);
      if(thePart!=0) part=particleTruthClassifier(thePart);

    } else if(part.first == GenParticle&&isHadron(thePart)){
      // to fix Alpgen hadrons with status code !=1 (>100) 
      part.first=Hadron; part.second=NonDefined;
    }

  } else {
    // in case of photon from gen particle  classify photon
    thePart = barcode_to_particle(xTruthParticleContainer,m_MotherBarcode%m_barcodeShift);
    if(thePart!=0) part=particleTruthClassifier(thePart);
  }

  m_BkgElecMother = thePart;
  return part;

}
//------------------------------------------------------------------------
double MCTruthClassifier::partCharge(const xAOD::TruthParticle* thePart){
  //------------------------------------------------------------------------
  if ( thePart == 0 ) return 0. ;

  const HepPDT::ParticleData * pData=m_particleTable->particle(HepPDT::ParticleID(abs(thePart->pdgId())));
  double  pCharge=0.;
  if(pData) pCharge=pData->charge();
  if(pCharge!=0&&thePart->pdgId()/abs(thePart->pdgId())<0) pCharge=-pCharge;
  return pCharge;

}

//------------------------------------------------------------------------
const xAOD::TruthParticle* MCTruthClassifier::
barcode_to_particle(const xAOD::TruthParticleContainer* m_TruthTES,
		    int bc){
  //------------------------------------------------------------------------
  // temporary solution?
  const xAOD::TruthParticle* ptrPart=NULL;

  for(const auto truthParticle : *m_TruthTES){
    if(truthParticle->barcode()==bc) {ptrPart=truthParticle; break;}
  }

  return ptrPart;
}
