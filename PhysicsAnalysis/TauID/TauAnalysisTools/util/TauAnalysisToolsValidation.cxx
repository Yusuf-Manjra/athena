/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include <iostream>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TProfile.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE
#include "xAODCore/ShallowCopy.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/TauTruthTrackMatchingTool.h"
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"

// #include "PileupReweighting/PileupReweightingTool.h"

using namespace TauAnalysisTools;

std::map<std::string, TH2F*> mTH2F;
double ev_weight = 1;

#define CHECK( ARG )                    \
  do {                                                  \
    const bool result = ARG;                \
    if( ! result ) {                    \
      ::Error( "TauAnalysisToolsValidation", "Failed to execute: \"%s\"",  \
           #ARG );                  \
      return 1;                     \
    }                           \
  } while( false )

#define RETRIEVE( TYPE, CONTAINER , NAME )              \
  do {                                  \
  if (xEvent.contains<TYPE>(NAME))                  \
    CHECK( xEvent.retrieve( CONTAINER, NAME ) );            \
  else                                  \
    Warning("TauAnalysisToolsValidation","%s container is not available", NAME);       \
  } while(false)                            \
 


void fillHistograms(std::string sBaseName, const xAOD::TauJet& xTau, double val);
void initializeHistograms(std::string sEleOlrWP);


int main( int argc, char* argv[] )
{
  // argument 1: input file 
  // argument 2: maximum number of events (default: all events available)
  // argument 3: output file name (default: output.root)
  // argument 4: ID working point (default: loose, choose between: loose, medium or tight)
  // argument 5: electron veto working point (default: no e veto, choose between: TauEleOLR,
  //             eleBDTLoosePlusVeto, eleBDTLoose, eleBDTMediumPlusVeto or eleBDTMedium

  CP::SystematicCode::enableFailure();

  xAOD::TReturnCode::enableFailure();

  // Check if we received a file name:
  if( argc < 2 )
  {
    Error( "TauAnalysisToolsValidation", "No file name received!" );
    Error( "TauAnalysisToolsValidation", "  Usage: %s [xAOD file name]", "TauAnalysisToolsValidation" );
    return 1;
  }

  // Check for ID working point
  std::string sWP = "loose";
  if (argc>4)
    sWP = TString(argv[4]);
  if (sWP != "loose" && sWP != "medium" && sWP != "tight")
  {
    Error("TauAnalysisToolsValidation", "Given working point is unknown. Choose between loose, medium and tight");
    return 1;
  }
  // Check for electron rejection working point
  std::string sEleOlrWP = "";
  if (argc>5)
    sEleOlrWP = TString(argv[5]);
  if (sEleOlrWP != "" && sEleOlrWP != "TauEleOLR" && sEleOlrWP != "eleBDTLoosePlusVeto" && sEleOlrWP != "eleBDTLoose" && sEleOlrWP != "eleBDTMediumPlusVeto" && sEleOlrWP != "eleBDTMedium" )
  {
    Error("TauAnalysisToolsValidation", "Given working point is unknown. Choose between TauEleOLR, eleBDTLoosePlusVeto, eleBDTLoose, eleBDTMediumPlusVeto and eleBDTMedium");
    return 1;  
  }

  // Initialise the application:
  CHECK( xAOD::Init( "TauAnalysisToolsValidation" ) );

  // Open the input file:
  const TString sInputFileName = argv[ 1 ];
  Info( "TauAnalysisToolsValidation", "Opening input file: %s", sInputFileName.Data() );
  std::unique_ptr< TFile > fInputFile( TFile::Open( sInputFileName, "READ" ) );
  CHECK( fInputFile.get() );

  // Create the output file:
  TString sOutputFileName = "output.root";
  if (argc>3)
    sOutputFileName = TString(argv[3]);

  Info( "TauAnalysisToolsValidation", "Opening output file: %s", sOutputFileName.Data() );
  std::unique_ptr< TFile > fOutputFile( TFile::Open( sOutputFileName, "RECREATE" ) );
  CHECK( fOutputFile.get() );

  std::unique_ptr< TFile > fOutputAOD( TFile::Open( "AOD.root", "RECREATE" ) );
  CHECK( fOutputAOD.get() );

  // Create a TEvent object:
  // xAOD::TEvent xEvent( xAOD::TEvent::kClassAccess );
  xAOD::TEvent xEvent( xAOD::TEvent::kAthenaAccess );
  CHECK( xEvent.readFrom( fInputFile.get() ) );

  // Connect TEvent with output file :
  CHECK( xEvent.writeTo( fOutputAOD.get() ) );

  Info( "TauAnalysisToolsValidation", "Number of events in the file: %i",
        static_cast< int >( xEvent.getEntries() ) );

  // Decide how many events to run over:
  Long64_t iEntries = xEvent.getEntries();
  if( argc > 2 )
  {
    const Long64_t iMaxEntries = atoll( argv[ 2 ] );
    if( iMaxEntries>0 && iMaxEntries < iEntries )
    {
      iEntries = iMaxEntries;
    }
  }

  initializeHistograms(sEleOlrWP);

  // defining needed Container
  const xAOD::EventInfo* xEventInfo = 0;
  const xAOD::TauJetContainer* xTauJetContainer = 0;

  // ===========================================================================
  // TauSelectionTool
  // ===========================================================================
  TauAnalysisTools::TauSelectionTool* TauSelTool = new TauAnalysisTools::TauSelectionTool( "TauSelectionTool" );
  TauSelTool->msg().setLevel( MSG::DEBUG );
  TauSelTool->setOutFile( fOutputFile.get() );

  // configure cut on pt
  CHECK(TauSelTool->setProperty("PtMin", 20. ));
  CHECK(TauSelTool->setProperty("ConfigPath", "" ));

  // configure cut on ID working point 
  CHECK(TauSelTool->setProperty("CreateControlPlots", true ));
  if (sWP == "loose")
    CHECK(TauSelTool->setProperty("JetIDWP", int(JETIDBDTLOOSE) ));
  else if (sWP == "medium")
    CHECK(TauSelTool->setProperty("JetIDWP", int(JETIDBDTMEDIUM) ));
  else if (sWP == "tight")
    CHECK(TauSelTool->setProperty("JetIDWP", int(JETIDBDTTIGHT) ));
  // configure cut on electron removal working point 
  if (sEleOlrWP == "TauEleOLR" || sEleOlrWP == "eleBDTLoosePlusVeto" || sEleOlrWP == "eleBDTMediumPlusVeto" )
    CHECK(TauSelTool->setProperty("EleOLR", true ));
  if (sEleOlrWP == "eleBDTLoose" || sEleOlrWP == "eleBDTLoosePlusVeto" )
    CHECK(TauSelTool->setProperty("EleBDTWP", int(ELEIDBDTLOOSE) ));
  if (sEleOlrWP == "eleBDTMedium" || sEleOlrWP == "eleBDTMediumPlusVeto" )
    CHECK(TauSelTool->setProperty("EleBDTWP", int(ELEIDBDTMEDIUM) ));
  // set pt, ID and electron removal cuts
  if (sEleOlrWP == "")
    CHECK(TauSelTool->setProperty("SelectionCuts", int(CutPt|CutJetIDWP) ));
  else if (sEleOlrWP == "TauEleOLR")
    CHECK(TauSelTool->setProperty("SelectionCuts", int(CutPt|CutJetIDWP|CutEleOLR) ));
  else if (sEleOlrWP == "eleBDTLoosePlusVeto" || sEleOlrWP == "eleBDTMediumPlusVeto")
    CHECK(TauSelTool->setProperty("SelectionCuts", int(CutPt|CutJetIDWP|CutEleOLR|CutEleBDTWP) ));
  else if ( sEleOlrWP == "eleBDTLoose" || sEleOlrWP == "eleBDTMedium")
    CHECK(TauSelTool->setProperty("SelectionCuts", int(CutPt|CutJetIDWP|CutEleBDTWP) ));

  CHECK(TauSelTool->initialize());

  ToolHandle<TauAnalysisTools::ITauSelectionTool> TauSelToolHandle = TauSelTool;

  // ===========================================================================
  // TauSmearingTool
  // ===========================================================================
  TauAnalysisTools::TauSmearingTool TauSmeTool( "TauSmearingTool" );
  TauSmeTool.msg().setLevel( MSG::INFO );
  CHECK(TauSmeTool.setProperty("ApplyMVATES", false ));
  CHECK(TauSmeTool.initialize());

  // restructure all recommended systematic variations for smearing tool
  std::vector<CP::SystematicSet> vSmearingSystematicSet;
  for (auto SystematicsVariation : TauSmeTool.recommendedSystematics())
  {
    vSmearingSystematicSet.push_back(CP::SystematicSet());
    vSmearingSystematicSet.back().insert(SystematicsVariation);
  }

  // ===========================================================================
  // TauEfficiencyCorrectionsTool
  // ===========================================================================
  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffCorrTool( "TauEfficiencyCorrectionsTool" );
  TauEffCorrTool.msg().setLevel( MSG::INFO );
  CHECK(TauEffCorrTool.setProperty("TauSelectionTool", TauSelToolHandle));
  CHECK(TauEffCorrTool.initialize());

  // restructure all recommended systematic variations for efficiency tools
  std::map<std::string, CP::SystematicSet> mSysSet;
  std::vector<CP::SystematicSet> vEfficiencyCorrectionsSystematicSet;
  vEfficiencyCorrectionsSystematicSet.push_back(CP::SystematicSet());
  mSysSet["NOMINAL"] = CP::SystematicSet();
  for (auto SystematicsVariation : TauEffCorrTool.recommendedSystematics())
  {
    mSysSet[SystematicsVariation.name()] = CP::SystematicSet();
    mSysSet[SystematicsVariation.name()].insert(SystematicsVariation);
    vEfficiencyCorrectionsSystematicSet.push_back(CP::SystematicSet());
    vEfficiencyCorrectionsSystematicSet.back().insert(SystematicsVariation);
  }


  // ===========================================================================
  // TauTruthMatchingTool
  // ===========================================================================
  TauAnalysisTools::TauTruthMatchingTool T2MT( "TauTruthMatchingTool");
  T2MT.msg().setLevel( MSG::INFO );
  CHECK(T2MT.setProperty("WriteTruthTaus", true));
  CHECK(T2MT.setProperty("TruthElectronContainerName", "egammaTruthParticles"));
  CHECK(T2MT.initialize());


  // Loop over the events:
  for( Long64_t iEntry = 0; iEntry < iEntries; ++iEntry )
  {
    // Tell the object which entry to look at:
    xEvent.getEntry( iEntry );

    // Print some event information for fun:
    RETRIEVE(xAOD::EventInfo, xEventInfo, "EventInfo");
    if (xEventInfo)
      Info( "TauAnalysisToolsValidation",
            "===>>>  start processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( xEventInfo->eventNumber() ),
            static_cast< int >( xEventInfo->runNumber() ),
            static_cast< int >( iEntry ) );

    RETRIEVE(xAOD::TauJetContainer, xTauJetContainer, "TauJets");
    std::pair< xAOD::TauJetContainer*, xAOD::ShallowAuxContainer* >xTauShallowContainer = xAOD::shallowCopyContainer(*xTauJetContainer);

    // get event weight
    ev_weight = 1;
    const std::vector<float> weights = xEventInfo->mcEventWeights();
    if(weights.size()>0) ev_weight = weights[0];

    // Print tau properties, using the tools:
    for ( auto xTau : *xTauShallowContainer.first )
    {
      // perform truth matching
      auto xTruthTau = T2MT.getTruth(*xTau);

      // skip tau candidates that are not true hadronic decays
      bool isTrueHadTau = false;
      bool isTrueElectron = false;
      if (xTau->auxdata<char>("IsTruthMatched"))
      {
        if (xTruthTau->isTau())
          isTrueHadTau = xTruthTau->auxdata<char>("IsHadronicTau");
        isTrueElectron = xTruthTau->isElectron();
      }

      // Select "good" taus:
      if( ! TauSelTool->accept( *xTau ) ) continue;

      // Apply tau smearing systematics and fill validation plots
      if (isTrueHadTau)
      {
        double originalPt = xTau->pt();
        CHECK(TauSmeTool.applySystematicVariation(CP::SystematicSet())) ;
        CHECK(TauSmeTool.applyCorrection(*xTau));
        double sfNominalTES = xTau->pt() / originalPt;
        double sfVarTES = 0;
        fillHistograms("TESScaleFactorHadTau", *xTau, sfNominalTES);

        xTau->setP4( originalPt, xTau->eta(), xTau->phi(), xTau->m());

        for (auto sSystematicSet: vSmearingSystematicSet)
        {
          CHECK( TauSmeTool.applySystematicVariation(sSystematicSet)) ;
          CHECK( TauSmeTool.applyCorrection(*xTau) );
          sfVarTES = xTau->pt()/originalPt;
          fillHistograms(sSystematicSet.name().c_str(), *xTau, (sfVarTES-sfNominalTES)/sfNominalTES);
          // reset tau pt 
          xTau->setP4( originalPt, xTau->eta(), xTau->phi(), xTau->m());
        }
      }

      // Apply tau efficiency corrections and fill validation plots
      if (isTrueHadTau)
      {
        double sfNominalReco = 0;
        double sfNominalID = 0;
        double sfVar = 0;
        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["NOMINAL"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfNominalReco = xTau->auxdata<double>("TauScaleFactorReconstructionHadTau");
        sfNominalID = xTau->auxdata<double>("TauScaleFactorJetIDHadTau");
        fillHistograms("TauScaleFactorReconstructionHadTau", *xTau, sfNominalReco);
        fillHistograms("TauScaleFactorJetIDHadTau", *xTau, sfNominalID);
        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorReconstructionHadTau");
        fillHistograms("TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up", *xTau, (sfVar-sfNominalReco)/sfNominalReco);
        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorReconstructionHadTau");
        fillHistograms("TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up", *xTau, (sfVar-sfNominalReco)/sfNominalReco);

        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORR2025__1up"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
        fillHistograms("TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORR2025__1up", *xTau, (sfVar-sfNominalID)/sfNominalID);

        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORR2530__1up"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
        fillHistograms("TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORR2530__1up", *xTau, (sfVar-sfNominalID)/sfNominalID);

        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORR3040__1up"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
        fillHistograms("TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORR3040__1up", *xTau, (sfVar-sfNominalID)/sfNominalID);

        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORRGE40__1up"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
        fillHistograms("TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORRGE40__1up", *xTau, (sfVar-sfNominalID)/sfNominalID);

        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTUNCORR2030__1up"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
        fillHistograms("TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTUNCORR2030__1up", *xTau, (sfVar-sfNominalID)/sfNominalID);

        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTUNCORRGE30__1up"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
        fillHistograms("TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTUNCORRGE30__1up", *xTau, (sfVar-sfNominalID)/sfNominalID);

        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_SYST__1up"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
        fillHistograms("TAUS_TRUEHADTAU_EFF_JETID_SYST__1up", *xTau, (sfVar-sfNominalID)/sfNominalID);

        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up"]));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
        fillHistograms("TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up", *xTau, (sfVar-sfNominalID)/sfNominalID);
      }
      // Apply electron rejection systematics and fill validation plots
      if (isTrueElectron)
      {
        double sfNominal = 0;
        double sfVar = 0;
        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["NOMINAL"]) );
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfNominal = xTau->auxdata<double>("TauScaleFactorEleOLRElectron");
        fillHistograms("TauScaleFactorEleOLRElectron", *xTau, sfNominal);
        CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEELECTRON_EFF_ELEOLR_TOTAL__1up"]) );
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        sfVar = xTau->auxdata< double >("TauScaleFactorEleOLRElectron");
        fillHistograms("TAUS_TRUEELECTRON_EFF_ELEOLR_TOTAL__1up", *xTau, (sfVar-sfNominal)/sfNominal);
      }
    }

    if (xTauJetContainer->empty())
      CHECK(T2MT.retrieveTruthTaus());
    CHECK(xEvent.fill());
  }

  // Tau selection control histograms
  TauSelTool->writeControlHistograms();
  delete TauSelTool;
  fOutputFile.get()->mkdir(("EfficiencyCorrectionsTool_control"));
  fOutputFile.get()->cd(("EfficiencyCorrectionsTool_control"));

  // Tau efficiency control histograms
  for (auto& h: mTH2F) h.second->Write();
  for (auto& h: mTH2F) h.second->ProfileX((h.first+"_pfx").c_str(), 1, -1, "s")->Write();
  fOutputFile.get()->Close();
  Info( "TauAnalysisToolsValidation", "Finished writing to file: %s", sOutputFileName.Data() );


  // AOD output
  CHECK( xEvent.finishWritingTo(fOutputAOD.get()));
  fOutputAOD.get()->Close();
  Info( "TauAnalysisToolsValidation", "Finished writing to file: AOD.root" );

  return 0;
}


void fillHistograms( std::string sBaseName, const xAOD::TauJet& xTau, double val )
{
  if (xTau.nTracks() == 1)
  {
    mTH2F[sBaseName+"__1p__pt"]->Fill(xTau.pt()/1000, val, ev_weight);
    mTH2F[sBaseName+"__1p__eta"]->Fill(xTau.eta(), val, ev_weight);
  }
  else if (xTau.nTracks() == 3)
  {
    mTH2F[sBaseName+"__3p__pt"]->Fill(xTau.pt()/1000, val, ev_weight);
    mTH2F[sBaseName+"__3p__eta"]->Fill(xTau.eta(), val, ev_weight);
  }
}


void initializeHistograms(std::string sEleOlrWP)
{
  std::string sNominalNames[4]     = {"TauScaleFactorReconstructionHadTau", 
                                      "TauScaleFactorJetIDHadTau", 
                                      "TESScaleFactorHadTau",
                                      "TauScaleFactorEleOLRElectron"};

  std::string sSystematicsNames[17] = {"TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up", 
                                      "TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up", 
                                      "TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORR2025__1up",
                                      "TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORR2530__1up",
                                      "TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORR3040__1up",
                                      "TAUS_TRUEHADTAU_EFF_JETID_1PRONGSTATSYSTUNCORRGE40__1up",
                                      "TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTUNCORR2030__1up",
                                      "TAUS_TRUEHADTAU_EFF_JETID_3PRONGSTATSYSTUNCORRGE30__1up",
                                      "TAUS_TRUEHADTAU_EFF_JETID_SYST__1up",
                                      "TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up",
                                      "TAUS_TRUEHADTAU_SME_TES_DETECTOR__1down", 
                                      "TAUS_TRUEHADTAU_SME_TES_DETECTOR__1up", 
                                      "TAUS_TRUEHADTAU_SME_TES_INSITU__1down",
                                      "TAUS_TRUEHADTAU_SME_TES_INSITU__1up",
                                      "TAUS_TRUEHADTAU_SME_TES_MODEL__1down",
                                      "TAUS_TRUEHADTAU_SME_TES_MODEL__1up",
                                      "TAUS_TRUEELECTRON_EFF_ELEOLR_TOTAL__1up"};

  if (sEleOlrWP == "")
  {
    double ptBins[8] = {20,25,30,40,60,100,300,1000};
    double etaBins[10] = {-2.5, -1.6, -1.3, -0.8, -0.3, 0.3, 0.8, 1.3, 1.6, 2.5};
    for (auto s: sNominalNames)
    {
      mTH2F[s+"__1p__pt"]  = new TH2F((s+"__1p__pt").c_str(),  (s+"__1p__pt").c_str(),  7, ptBins,  201, 0.8, 1.2);
      mTH2F[s+"__3p__pt"]  = new TH2F((s+"__3p__pt").c_str(),  (s+"__3p__pt").c_str(),  7, ptBins,  201, 0.8, 1.2);
      mTH2F[s+"__1p__eta"] = new TH2F((s+"__1p__eta").c_str(), (s+"__1p__eta").c_str(), 9, etaBins, 201, 0.8, 1.2);
      mTH2F[s+"__3p__eta"] = new TH2F((s+"__3p__eta").c_str(), (s+"__3p__eta").c_str(), 9, etaBins, 201, 0.8, 1.2); 
    }
    for (auto s: sSystematicsNames)
    {
      mTH2F[s+"__1p__pt"]  = new TH2F((s+"__1p__pt").c_str(),  (s+"__1p__pt").c_str(),  7, ptBins,  201, -0.2, 0.2);
      mTH2F[s+"__3p__pt"]  = new TH2F((s+"__3p__pt").c_str(),  (s+"__3p__pt").c_str(),  7, ptBins,  201, -0.2, 0.2);
      mTH2F[s+"__1p__eta"] = new TH2F((s+"__1p__eta").c_str(), (s+"__1p__eta").c_str(), 9, etaBins, 201, -0.2, 0.2);
      mTH2F[s+"__3p__eta"] = new TH2F((s+"__3p__eta").c_str(), (s+"__3p__eta").c_str(), 9, etaBins, 201, -0.2, 0.2);
    }
  }
  else
  {
    double ptBins[8] = {20,25,30,40,60,100,300,1000};
    double etaBins[14] = {-2.47, -2.37, -2.01, -1.52, -1.37, -0.80, -0.10, 0.10, 0.80, 1.37, 1.52, 2.01, 2.37, 2.47};
    for (auto s: sNominalNames)
    {
      mTH2F[s+"__1p__pt"]  = new TH2F((s+"__1p__pt").c_str(),  (s+"__1p__pt").c_str(),  7, ptBins,  3216, -3.2, 3.2);
      mTH2F[s+"__3p__pt"]  = new TH2F((s+"__3p__pt").c_str(),  (s+"__3p__pt").c_str(),  7, ptBins,  3216, -3.2, 3.2);
      mTH2F[s+"__1p__eta"] = new TH2F((s+"__1p__eta").c_str(), (s+"__1p__eta").c_str(), 13, etaBins, 3216, -3.2, 3.2);
      mTH2F[s+"__3p__eta"] = new TH2F((s+"__3p__eta").c_str(), (s+"__3p__eta").c_str(), 13, etaBins, 3216, -3.2, 3.2); 
    }
     
    for (auto s: sSystematicsNames)
    {
      mTH2F[s+"__1p__pt"]  = new TH2F((s+"__1p__pt").c_str(),  (s+"__1p__pt").c_str(),  7, ptBins,  201, -0.2, 0.2);
      mTH2F[s+"__3p__pt"]  = new TH2F((s+"__3p__pt").c_str(),  (s+"__3p__pt").c_str(),  7, ptBins,  201, -0.2, 0.2);
      mTH2F[s+"__1p__eta"] = new TH2F((s+"__1p__eta").c_str(), (s+"__1p__eta").c_str(), 13, etaBins, 201, -0.2, 0.2);
      mTH2F[s+"__3p__eta"] = new TH2F((s+"__3p__eta").c_str(), (s+"__3p__eta").c_str(), 13, etaBins, 201, -0.2, 0.2);
    }
  }
}
