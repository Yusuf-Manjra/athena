/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


//Author: Doug Schaefer
//Email : schae@cern.ch
//Date  : May 2017
#ifndef XAOD_ANALYSIS
int main() {
  std::cout << "Examples are only defined in the analysis release." << std::endl;
  return 0;
}
#else

#ifdef XAOD_STANDALONE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#else
#include "POOLRootAccess/TEvent.h"
#include "StoreGate/StoreGateSvc.h"
#endif

// FrameWork includes
#include "AsgTools/MessageCheck.h"
#include "AsgTools/AnaToolHandle.h"

#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETContainer.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "JetInterface/IJetModifier.h"

#include <memory>
#undef NDEBUG
#include <cassert>
#include "TFile.h"
#include "TSystem.h"

#include "JetCalibTools/JetCalibrationTool.h"

#include "METInterface/IMETMaker.h"
#include "METInterface/IMETSignificance.h"
#include "METUtilities/CutsMETMaker.h"
#include "METUtilities/METHelpers.h"

#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

static const SG::AuxElement::Decorator< float > dec_uniqueFrac("UniqueFrac");
static const SG::AuxElement::Decorator< float > dec_METPtDiff("METPtDiff");

using namespace asg::msgUserCode;

int main( int argc, char* argv[] ){std::cout << __PRETTY_FUNCTION__ << std::endl;
#ifdef XAOD_STANDALONE
  StatusCode::enableFailure();
  xAOD::TReturnCode::enableFailure();
#else
  IAppMgrUI* app = POOL::Init(); //important to do this first!
#endif

  std::string jetType = "AntiKt4EMTopo";
  TString fileName = gSystem->Getenv("ASG_TEST_FILE_MC");
  size_t evtmax = 100;
  std::string jetAux = "";
  bool debug(false);
  for (int i=0; i<argc; ++i) {
    if (std::string(argv[i]) == "-filen" && i+1<argc) {
      fileName = argv[i+1];
    } else if (std::string(argv[i]) == "-jetcoll" && i+1<argc) {
      jetType = argv[i+1];
    } else if (std::string(argv[i]) == "-evtmax" && i+1<argc) {
      evtmax = atoi(argv[i+1]);
    }else if (std::string(argv[i]) == "-jetaux" && i+1<argc) {
      jetAux = argv[i+1];
    } else if (std::string(argv[i]) == "-debug") {
      debug = true;
    }
  }

  std::string config = "JES_MC16Recommendation_Consolidated_EMTopo_Apr2019_Rel21.config";
  std::string calibSeq = "JetArea_Residual_EtaJES_GSC";
  std::string calibArea = "00-04-82";
  if(jetType=="AntiKt4EMPFlow"){
    config = "JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21.config";
    calibSeq = "JetArea_Residual_EtaJES_GSC";
    calibArea = "00-04-82";
  }

  asg::AnaToolHandle<IJetCalibrationTool> jetCalibrationTool;
  ANA_CHECK( ASG_MAKE_ANA_TOOL( jetCalibrationTool, JetCalibrationTool ) );
  jetCalibrationTool.setName("jetCalibTool");
  ANA_CHECK( jetCalibrationTool.setProperty("JetCollection", jetType) );
  ANA_CHECK( jetCalibrationTool.setProperty("ConfigFile", config) );
  ANA_CHECK( jetCalibrationTool.setProperty("CalibSequence", calibSeq) );
  ANA_CHECK( jetCalibrationTool.setProperty("CalibArea", calibArea) );
  ANA_CHECK( jetCalibrationTool.setProperty("IsData", false) );
  ANA_CHECK( jetCalibrationTool.retrieve() );

  asg::AnaToolHandle<IJetModifier> m_jetFwdJvtTool;
  m_jetFwdJvtTool.setTypeAndName("JetForwardJvtTool/JetForwardJvtTool");
  ANA_CHECK( m_jetFwdJvtTool.setProperty("OutputDec", "passFJvt") ); //Output decoration
  // fJVT WPs depend on the MET WP, see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EtmissRecommendationsRel21p2#fJVT_and_MET
  ANA_CHECK( m_jetFwdJvtTool.setProperty("UseTightOP", true) ); // Tight
  ANA_CHECK( m_jetFwdJvtTool.setProperty("EtaThresh", 2.5) );   //Eta dividing central from forward jets
  ANA_CHECK( m_jetFwdJvtTool.setProperty("ForwardMaxPt", 120.0e3) ); //Max Pt to define fwdJets for JVT
  ANA_CHECK( m_jetFwdJvtTool.retrieve() );

  //this test file should work.  Feel free to contact me if there is a problem with the file.
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  assert( ifile.get() );

  // Create a TEvent object to read from file and a transient store in which to place items
#ifdef XAOD_STANDALONE
  std::unique_ptr<xAOD::TEvent> event(new xAOD::TEvent( xAOD::TEvent::kClassAccess ) );
  //std::unique_ptr<xAOD::TEvent> event(new xAOD::TEvent( xAOD::TEvent::kAthenaAccess ) );
  std::unique_ptr<xAOD::TStore> store(new xAOD::TStore());
#else // Athena "Store" is the same StoreGate used by the TEvent
  std::unique_ptr<POOL::TEvent> event(new POOL::TEvent( POOL::TEvent::kClassAccess ));
  //std::unique_ptr<POOL::TEvent> event(new POOL::TEvent( POOL::TEvent::kAthenaAccess ));
  ServiceHandle<StoreGateSvc>& store = event->evtStore();
#endif
  ANA_CHECK( event->readFrom( ifile.get() ) );

  // declare METSignificance
  
  asg::AnaToolHandle<IMETSignificance> metSignif;
  metSignif.setTypeAndName("met::METSignificance/metSignif");
  ANA_CHECK( metSignif.setProperty("SoftTermParam", met::Random) );
  ANA_CHECK( metSignif.setProperty("TreatPUJets",   true) );
  ANA_CHECK( metSignif.setProperty("DoPhiReso",     true) );
  ANA_CHECK( metSignif.setProperty("IsDataJet",     false) );
  ANA_CHECK( metSignif.setProperty("DoJerForEMu",   false) );
  ANA_CHECK( metSignif.setProperty("JetCollection", jetType) );
  if(jetAux!="")
    ANA_CHECK( metSignif.setProperty("JetResoAux", jetAux) );
  if(debug) ANA_CHECK( metSignif.setProperty("OutputLevel", MSG::VERBOSE) );
  ANA_CHECK( metSignif.retrieve() );
  
  // reconstruct the MET
  asg::AnaToolHandle<IMETMaker> metMaker;
  metMaker.setTypeAndName("met::METMaker/metMaker");
  ANA_CHECK( metMaker.setProperty("DoMuonEloss", false) );
  ANA_CHECK( metMaker.setProperty("DoRemoveMuonJets", true) );
  ANA_CHECK( metMaker.setProperty("DoSetMuonJetEMScale", true) );
  ANA_CHECK( metMaker.retrieve() );

  for(size_t ievent = 0;  ievent < std::min(size_t(event->getEntries()), evtmax); ++ievent){
    if(ievent % 10 == 0) std::cout << "event number: " << ievent << std::endl;
    ANA_CHECK( event->getEntry(ievent) >= 0 );

    const xAOD::EventInfo* eventinfo = 0;
    ANA_CHECK( event->retrieve( eventinfo, "EventInfo" ) );    

    //retrieve the original containers
    const xAOD::MissingETContainer* coreMet  = nullptr;
    std::string coreMetKey = "MET_Core_" + jetType;
    ANA_CHECK( event->retrieve(coreMet, coreMetKey) );
    if(debug) std::cout << "Using core MET " << coreMet << std::endl;

    const xAOD::ElectronContainer* electrons = nullptr;
    ANA_CHECK( event->retrieve(electrons, "Electrons") );

    const xAOD::MuonContainer* muons = nullptr;
    ANA_CHECK( event->retrieve(muons, "Muons") );

    const xAOD::PhotonContainer* photons = nullptr;
    ANA_CHECK( event->retrieve(photons, "Photons"));

    const xAOD::TauJetContainer* taus = nullptr;
    ANA_CHECK( event->retrieve(taus, "TauJets"));

    //this should probably be a calibrated jet container.  See the METUtilities twiki for more info
    const xAOD::JetContainer* jets = nullptr;
    ANA_CHECK( event->retrieve(jets, jetType+"Jets"));

    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > jets_shallowCopy = xAOD::shallowCopyContainer( *jets );
    ANA_CHECK(store->record( jets_shallowCopy.first,  "CalibJets"    ));
    ANA_CHECK(store->record( jets_shallowCopy.second, "CalibJetsAux."));
    //this is a non-const copy of the jet collection that you can calibrate.
    xAOD::JetContainer* calibJets = jets_shallowCopy.first;
    xAOD::setOriginalObjectLink(*jets,*calibJets);
    unsigned ij=0;
    for ( const auto& jet : *calibJets ) {
      //Shallow copy is needed (see links below)
      if(!jetCalibrationTool->applyCalibration(*jet))//apply the calibration	
	return 1;
      // setting 100% jet resolution for testing
      if(jetAux!="") jet->auxdata<float>(jetAux) = 1.0;
      //double pt_reso_dbl=0.0;
      //jetCalibrationTool->getNominalResolutionData(*jet, pt_reso_dbl);                                                              
      //std::cout << "pt_reso_dbl: " << pt_reso_dbl << std::endl;
      if(debug) std::cout << " jet: " << ij << " pt: " << jet->pt() << " eta: "<< jet->eta() << std::endl;
      ++ij;
    }
    m_jetFwdJvtTool->modify(*calibJets); //compute FwdJVT for all jets

    //retrieve the MET association map
    const xAOD::MissingETAssociationMap* metMap = nullptr;
    std::string metAssocKey = "METAssoc_" + jetType;
    ANA_CHECK( event->retrieve(metMap, metAssocKey) );
    metMap->resetObjSelectionFlags();

    xAOD::MissingETContainer*    newMetContainer    = new xAOD::MissingETContainer();
    xAOD::MissingETAuxContainer* newMetAuxContainer = new xAOD::MissingETAuxContainer();
    newMetContainer->setStore(newMetAuxContainer);

    // It is necessary to reset the selected objects before every MET calculation
    metMap->resetObjSelectionFlags();

    //here we apply some basic cuts and rebuild the met at each step
    if(!electrons->empty()){
      ConstDataVector<xAOD::ElectronContainer> metElectrons(SG::VIEW_ELEMENTS);
      for(const auto& el : *electrons) {
	if(CutsMETMaker::accept(el)){
	  metElectrons.push_back(el);
	}
      }

      ANA_CHECK(metMaker->rebuildMET("RefEle",
			       xAOD::Type::Electron,
			       newMetContainer,
			       metElectrons.asDataVector(),
			       metMap)
	   );
    }

    //Photons
    ConstDataVector<xAOD::PhotonContainer> metPhotons(SG::VIEW_ELEMENTS);
    for(const auto& ph : *photons) {
      if(CutsMETMaker::accept(ph)) metPhotons.push_back(ph);
    }
    ANA_CHECK(metMaker->rebuildMET("RefPhoton",
			       xAOD::Type::Photon,
			       newMetContainer,
			       metPhotons.asDataVector(),
			       metMap)
	   );
    //Taus
    ConstDataVector<xAOD::TauJetContainer> metTaus(SG::VIEW_ELEMENTS);
    for(const auto& tau : *taus) {
      if(CutsMETMaker::accept(tau)) metTaus.push_back(tau);
    }
    ANA_CHECK(metMaker->rebuildMET("RefTau",
			       xAOD::Type::Tau,
			       newMetContainer,
			       metTaus.asDataVector(),
			       metMap)
	   );
    
    //Muons
    ConstDataVector<xAOD::MuonContainer> metMuons(SG::VIEW_ELEMENTS);
    for(const auto& mu : *muons) {
      //      if(CutsMETMaker::accept(mu)) metMuons.push_back(mu);
      if(mu->muonType()==xAOD::Muon::Combined && mu->pt()>10e3) metMuons.push_back(mu);
    }

    ANA_CHECK(metMaker->rebuildMET("Muons",
			       xAOD::Type::Muon,
			       newMetContainer,
			       metMuons.asDataVector(),
			       metMap)
	   );
    
    met::addGhostMuonsToJets(*muons, *calibJets);

    //Now time to rebuild jetMet and get the soft term
    //these functions create an xAODMissingET object with the given names inside the container
    ANA_CHECK( metMaker->rebuildJetMET("RefJet",        //name of jet met
				    "SoftClus",      //name of soft cluster term met
				    "PVSoftTrk",     //name of soft track term met
				    newMetContainer, //adding to this new met container
				    calibJets,       //using this jet collection to calculate jet met
				    coreMet,         //core met container
				    metMap,          //with this association map
				    true             //apply jet jvt cut
				    )
	     );

    //this builds the final track and cluster met sums, using systematic varied container
    ANA_CHECK( metMaker->buildMETSum("FinalTrk" , newMetContainer, MissingETBase::Source::Track ) );
    ANA_CHECK( metMaker->buildMETSum("FinalClus", newMetContainer, MissingETBase::Source::LCTopo) );

    // Run MET significance    
    ANA_CHECK( metSignif->varianceMET(newMetContainer, eventinfo->averageInteractionsPerCrossing(), "RefJet", "PVSoftTrk","FinalTrk"));

    if(debug){
      if(newMetContainer->find("Muons")!=newMetContainer->end())
	std::cout << "Muon term: "  << static_cast<xAOD::MissingET*>(*(newMetContainer->find("Muons")))->met()
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("Muons")))->phi() 
		  << std::endl;
      if(newMetContainer->find("MuonEloss")!=newMetContainer->end())
	std::cout << "MuonEloss term: "  << static_cast<xAOD::MissingET*>(*(newMetContainer->find("MuonEloss")))->met()
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("MuonEloss")))->phi() 
		  << std::endl;
      if(newMetContainer->find("RefJet")!=newMetContainer->end())
	std::cout << "Jet term: "   << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefJet")))->met() 
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefJet")))->phi() 
		  << std::endl;
      
      if(newMetContainer->find("RefEle")!=newMetContainer->end())
	std::cout << "Ele term: "   << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefEle")))->met() 
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefEle")))->phi() 
		  << std::endl;
      if(newMetContainer->find("RefTau")!=newMetContainer->end())
	std::cout << "Tau term: "   << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefTau")))->met() 
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefTau")))->phi() 
		  << std::endl;
      if(newMetContainer->find("RefPhoton")!=newMetContainer->end())
	std::cout << "Gamma term: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefPhoton")))->met() 
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefPhoton")))->phi() 
		  << std::endl;
      if(newMetContainer->find("PVSoftTrk")!=newMetContainer->end())
	std::cout << "Soft term: "  << static_cast<xAOD::MissingET*>(*(newMetContainer->find("PVSoftTrk")))->met() 
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("PVSoftTrk")))->phi() 
		  << std::endl;

      // Print the METSignificance terms (e.g. jet, muon, ele, pho, etc)
      // NOTE::: these are not currently rotated if the MET is rotated
      std::cout << "  jet   VarL: " << metSignif->GetTermVarL(met::ResoJet) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoJet) << " GeV" << std::endl;
      std::cout << "  muon   VarL: " << metSignif->GetTermVarL(met::ResoMuo) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoMuo) << " GeV" << std::endl;
      std::cout << "  electron   VarL: " << metSignif->GetTermVarL(met::ResoEle) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoEle) << " GeV" << std::endl;
      std::cout << "  photon   VarL: " << metSignif->GetTermVarL(met::ResoPho) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoPho) << " GeV" << std::endl;
      std::cout << "  tau   VarL: " << metSignif->GetTermVarL(met::ResoTau) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoTau) << " GeV" << std::endl;
      std::cout << "  Soft term   VarL: " << metSignif->GetTermVarL(met::ResoSoft) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoSoft) << " GeV" << std::endl;
      std::cout << "  other/bug   VarL: " << metSignif->GetTermVarL(met::ResoNone) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoNone) << " GeV" << std::endl;

    }
      
    // extracting the MET significance
    std::cout << "MET significance: " << metSignif->GetSignificance() << std::endl;

    if(debug){
      // Try a rotation to a new lambda parameter
      std::cout << " Lambda Test Before: " << metSignif->GetSignificance() << " VarL: " << metSignif->GetVarL() << " VarT: " << metSignif->GetVarT() << std::endl;
      metSignif->SetLambda(0.0, 0.0);
      std::cout << " Lambda Test 0: " << metSignif->GetSignificance() << " VarL: " << metSignif->GetVarL() << " VarT: " << metSignif->GetVarT() << std::endl;
      metSignif->SetLambda(10.0, 10.0);
      std::cout << " Lambda Test 10: " << metSignif->GetSignificance() << " VarL: " << metSignif->GetVarL() << " VarT: " << metSignif->GetVarT() << std::endl;
    }

    ANA_CHECK(store->record( newMetContainer,    "FinalMETContainer"    ));
    ANA_CHECK(store->record( newMetAuxContainer, "FinalMETContainerAux."));

#ifdef XAOD_STANDALONE // POOL::TEvent should handle this when changing events
    //fill the containers stored in the event
    //to the output file and clear the transient store
    event->fill();
    store->clear();
#endif
  }

#ifndef XAOD_STANDALONE // POOL::TEvent should handle this when changing events
  app->finalize();
#endif

  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

  return 0;
 }

#endif
