/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "tauMonitoring/tauMonitorAlgorithm.h"

tauMonitorAlgorithm::tauMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_doRandom(true)
{}


tauMonitorAlgorithm::~tauMonitorAlgorithm() {}


StatusCode tauMonitorAlgorithm::initialize() {
    using namespace Monitored;
    m_abGroups1 = buildToolMap<int>(m_tools,"tauMonitor",2);
    m_abGroups2 = buildToolMap<std::vector<int>>(m_tools,"tauMonitor",4,2);

    std::vector<std::string> layers = {"layer1","layer2"};
    std::vector<std::string> clusters = {"clusterX","clusterB"};
    m_cGroups1 = buildToolMap<int>(m_tools,"tauMonitor",layers);
    m_cGroups2 = buildToolMap<std::map<std::string,int>>(m_tools,"tauMonitor",layers,clusters);
    return AthMonitorAlgorithm::initialize();
}


StatusCode tauMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
    auto lb = Monitored::Scalar<int>("lb",0);
    auto run = Monitored::Scalar<int>("run",0);
    auto random = Monitored::Scalar<float>("random",0.0);
    auto testweight = Monitored::Scalar<float>("testweight",1.0);

    // Two variables (value and passed) needed for TEfficiency
    auto pT = Monitored::Scalar<float>("pT",0.0);
    auto pT_passed = Monitored::Scalar<bool>("pT_passed",false);

    // Set the values of the monitored variables for the event
    lumiPerBCID = lbAverageInteractionsPerCrossing(ctx);
    lb = GetEventInfo(ctx)->lumiBlock();
    run = GetEventInfo(ctx)->runNumber();
    testweight = 2.0;
    
    TRandom3 r(ctx.eventID().event_number());
    // Example of using flags
    if (m_doRandom) {
        random = r.Rndm();
    }

    // Fake efficiency calculator
    pT = r.Landau(15);
    pT_passed = pT>r.Poisson(15);

    // Fill. First argument is the tool name, all others are the variables to be saved.
    fill("tauMonitor",lumiPerBCID,lb,random,pT,pT_passed,testweight);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("tauMonitor");
    fill(tool,run);

    // Fill with a vector; useful in some circumstances.
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> varVec = {lumiPerBCID,pT};
    fill("tauMonitor",varVec);
    fill(tool,varVec);

    // Filling using a pre-defined array of groups.
    auto a = Scalar<float>("a",0.0);
    auto b = Scalar<float>("b",1.0);
    auto c = Scalar<float>("c",2.0);
    for ( auto iEta : {0,1} ) {
        // 1) Valid but inefficient fill
        fill("tauMonitor_"+std::to_string(iEta),a,b,c);
        // 2) Faster way to fill a vector of histograms
        fill(m_tools[m_abGroups1[iEta]],a,b,c);
        for ( auto iPhi : {0,1} ) {
            // Same efficient method for 2D array
            fill(m_tools[m_abGroups2[iEta][iPhi]],a,b);
        }
    }

    // Filling using a pre-defined map of groups.
    for ( auto& layer : std::vector<std::string>({"layer1","layer2"}) ) {
        fill(m_tools[m_cGroups1.at(layer)],c);
        for ( auto& cluster : std::vector<std::string>({"clusterX","clusterB"}) ) {
            // Same efficient method for 2D map
            fill(m_tools[m_cGroups2.at(layer).at(cluster)],c);
        }
    }

    return StatusCode::SUCCESS;
}
