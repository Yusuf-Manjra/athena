/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// Based on CPToolTester.cxx (A. Kraznahorkay) 
/// and ut_ath_checkTrigger_test.cxx (W. Buttinger)
/// Contact: jmaurer@cern.ch
/*
 *    This example shows how to configure the tool for photon triggers. 
 *    Here, the minimal configuration is shown (one symmetric diphoton trigger)
 */

// ROOT include(s):
#include <TFile.h>
#include <TError.h>

// Infrastructure include(s):
#ifdef XAOD_STANDALONE
    #include "xAODRootAccess/Init.h"
    #include "xAODRootAccess/TEvent.h"
    #include "xAODRootAccess/TStore.h"
#else
    #include "AthAnalysisBaseComps/AthAnalysisHelper.h"
    #include "POOLRootAccess/TEvent.h"
#endif

// EDM include(s):
#include "AsgTools/AnaToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonEfficiencyCorrectionTool.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/PhotonContainer.h"
#include "PATCore/PATCoreEnums.h"

// stdlib include(s):
#include <random>
#include <vector>
#include <array>
using std::vector;
using std::string;

#define MSGSOURCE "Example 5a"

int main(int argc, char* argv[])
{
    const char* filename = nullptr;
    bool debug = false, cmdline_error = false, toys = false;
    for(int i=1;i<argc;++i)
    {
        if(string(argv[i]) == "--debug") debug = true;
        else if(string(argv[i]) == "--toys") toys = true;
        else if(!filename && *argv[i]!='-') filename = argv[i];
        else cmdline_error = true;
    }
    if(!filename || cmdline_error)
    {
        Error(MSGSOURCE, "No file name received!");
        Error(MSGSOURCE, "  Usage: %s [--debug] [--toys] [DxAOD file name]", argv[0]);
        return 1;
    }
    #ifdef XAOD_STANDALONE
        xAOD::Init(MSGSOURCE).ignore();
        TFile* file = TFile::Open(filename, "READ");
        if(!file)
        {
            Error(MSGSOURCE, "Unable to open file!");
            return 2;
        }
        xAOD::TEvent event(xAOD::TEvent::kClassAccess);
        xAOD::TStore store;
        StatusCode::enableFailure();
    #else
       IAppMgrUI* app = POOL::Init();
       POOL::TEvent event(POOL::TEvent::kClassAccess);
       TString file(filename);
    #endif
    event.readFrom(file).ignore();
    Long64_t entries = event.getEntries();
    Info(MSGSOURCE, "Number of events in the file: %lli", entries);

    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the photon CP tools");
    /// For property 'PhotonEfficiencyTools':
    ToolHandleArray<IAsgPhotonEfficiencyCorrectionTool> photonEffTools;
    /// For property 'PhotonScaleFactorTools':
    ToolHandleArray<IAsgPhotonEfficiencyCorrectionTool> photonSFTools; 

    /// RAII on-the-fly creation of photon CP tools:
    vector<asg::AnaToolHandle<IAsgPhotonEfficiencyCorrectionTool>> factory;

    const char* mapPath = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/"
            "PhotonEfficiencyCorrection/2015_2017/"
            "rel21.2/Winter2018_Prerec_v1/map1.txt";
    for(int j=0;j<2;++j) /// two instances: 0 -> MC efficiencies, 1 -> SFs
    {
        string name = "AsgPhotonEfficiencyCorrectionTool/" + std::string(j? "PhTrigEff" : "PhTrigSF");
        auto t = factory.emplace(factory.end(), name);
        t->setProperty("MapFilePath", mapPath).ignore();
        t->setProperty("TriggerKey", string(j?"":"Eff_") + "HLT_g22_tight_L1EM15VHI").ignore();
        t->setProperty("IsoKey", "Loose").ignore();
        t->setProperty("ForceDataType", (int)PATCore::ParticleDataType::Full).ignore();
        if(t->initialize() != StatusCode::SUCCESS)
        {
            Error(MSGSOURCE, "Unable to initialize the photon CP tool <%s>!",
                    t->name().c_str());
            return 3;
        }
        auto& handles = (j? photonSFTools : photonEffTools);
        handles.push_back(t->getHandle());
    }
    
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Configuring the global trigger SF tool");
    asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> myTool("TrigGlobalEfficiencyCorrectionTool/TrigGlobal");
    myTool.setProperty("PhotonEfficiencyTools", photonEffTools).ignore();
    myTool.setProperty("PhotonScaleFactorTools", photonSFTools).ignore();
    myTool.setProperty("TriggerCombination2017", "2g22_tight_L12EM15VHI").ignore();

    if(debug) myTool.setProperty("OutputLevel", MSG::DEBUG).ignore();
    if(toys) myTool.setProperty("NumberOfToys", 1000).ignore();
    if(myTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the TrigGlob tool!");
        return 3;
    }
    
    /* ********************************************************************** */
    
    Info(MSGSOURCE, "Starting the event loop");
    unsigned errors = 0;
    double nSuitableEvents = 0., sumW = 0.;
    for(Long64_t entry = 0; entry < entries; ++entry)
    {
        event.getEntry(entry);
        
        /// Get a random run number, and decorate the event info
        const xAOD::EventInfo* eventInfo = nullptr;
        event.retrieve(eventInfo,"EventInfo").ignore();
        unsigned runNumber = 332720;
        eventInfo->auxdecor<unsigned>("RandomRunNumber") = runNumber;

        vector<const xAOD::IParticle*> myTriggeringPhotons;
        const xAOD::PhotonContainer* photons = nullptr;
        event.retrieve(photons,"Photons").ignore();
        for(auto photon : *photons)
        {
            if(!photon->caloCluster()) continue;
            float eta = fabs(photon->caloCluster()->etaBE(2));
            float pt = photon->pt();
            if(pt<10e3f || eta>=2.37) continue;
            int t = photon->auxdata<int>("truthType");
            if(t!=14) continue;
            /// photon must be above trigger threshold:
            if(pt < 26e3f) continue;
            myTriggeringPhotons.push_back(photon);
        }

        /// Events must contain enough photons to trigger
        if(myTriggeringPhotons.size() < 2) continue;

        /// Finally retrieve the global trigger scale factor
        double sf = 1.;
        auto cc = myTool->getEfficiencyScaleFactor(myTriggeringPhotons, sf);
        if(cc==CP::CorrectionCode::Ok)
        {
            nSuitableEvents += 1;
            sumW += sf;
        }
        else
        {
            Warning(MSGSOURCE, "Scale factor evaluation failed");
            ++errors;
        }
        if(errors>10)
        {
            Error(MSGSOURCE, "Too many errors reported!");
            break;
        }
    }
    Info(MSGSOURCE, "Average scale factor: %f (over %ld events)",
            sumW / nSuitableEvents, long(nSuitableEvents));
    #ifndef XAOD_STANDALONE
        app->finalize();
    #endif
    return errors? 4 : 0;
}
