/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaMonitoring/IMonitorToolBase.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// root includes
#include "TTree.h"
// Local includes
#include "TrigT1NSW/NSWL1Simulation.h"
#include "MuonRDO/NSW_TrigRawDataContainer.h"
#include <vector>

namespace NSWL1 {

  NSWL1Simulation::NSWL1Simulation( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ),
      m_monitors(this),
      m_pad_tds("NSWL1::PadTdsOfflineTool",this),
      m_pad_trigger("NSWL1::PadTriggerLogicOfflineTool",this),
      m_pad_trigger_lookup("NSWL1::PadTriggerLookupTool",this),
      m_strip_tds("NSWL1::StripTdsOfflineTool",this),
      m_strip_cluster("NSWL1::StripClusterTool",this),
      m_strip_segment("NSWL1::StripSegmentTool",this),
      m_mmstrip_tds("NSWL1::MMStripTdsOfflineTool",this),
      m_mmtrigger("NSWL1::MMTriggerTool",this),
      m_tree(nullptr),
      m_current_run(-1),
      m_current_evt(-1)
  {
    m_counters.clear();

    // Services

    // Property setting general behaviour:
    declareProperty( "DoOffline",    m_doOffline    = false, "Steers the offline emulation of the LVL1 logic" );
     declareProperty( "UseLookup",   m_useLookup    = false, "Toggle Lookup mode on and off default is the otf(old) mode" );
    declareProperty( "DoNtuple",     m_doNtuple     = false, "Create an ntuple for data analysis" );
    declareProperty( "DoMM",         m_doMM         = false, "Run data analysis for MM" );
    declareProperty( "DosTGC",       m_dosTGC       = true, "Run data analysis for sTGCs" );
    
    // declare monitoring tools
    declareProperty( "AthenaMonTools",  m_monitors, "List of monitoring tools to be run with this instance, if incorrect then tool is silently skipped.");

    declareProperty( "PadTdsTool",      m_pad_tds,  "Tool that simulates the functionalities of the PAD TDS");
    //PadTriggerTool : in principle can be totally wuiped out. necesary for ntuples currently. Once you isolate ntuple making code and the trigger code you can abandon this method. Things ae still tangled a bit somewhow so keep it just in case
    declareProperty( "PadTriggerTool",  m_pad_trigger, "Tool that simulates the pad trigger logic");
    declareProperty( "PadTriggerLookupTool",  m_pad_trigger_lookup, "Tool that is used to lookup pad trigger patterns per execute against the same LUT as in trigger FPGA");
    declareProperty( "StripTdsTool",    m_strip_tds,  "Tool that simulates the functionalities of the Strip TDS");
    declareProperty( "StripClusterTool",m_strip_cluster,  "Tool that simulates the Strip Clustering");
    declareProperty( "StripSegmentTool",m_strip_segment,  "Tool that simulates the Segment finding");
    declareProperty( "MMStripTdsTool",  m_mmstrip_tds,  "Tool that simulates the functionalities of the MM STRIP TDS");
    declareProperty( "MMTriggerTool",   m_mmtrigger,    "Tool that simulates the MM Trigger");
    declareProperty("NSWTrigRDOContainerName", m_trigRdoContainer = "NSWTRGRDO"," Give a name to NSW trigger rdo container");
    declareProperty("NSWPadTrigRDOContainerName", m_padTriggerRdoKey = "NSWPADTRGRDO", "Name of the NSW Pad Trigger RDO container");
    // declare monitoring variables
    declareMonitoredStdContainer("COUNTERS", m_counters); // custom monitoring: number of processed events    
  }

  NSWL1Simulation::~NSWL1Simulation() {

  }


  StatusCode NSWL1Simulation::initialize() {
    ATH_MSG_INFO( "initialize " << name() );
    ATH_CHECK( m_trigRdoContainer.initialize() );    
    // Create an register the ntuple if requested, add branch for event and run number
    if ( m_doNtuple ) {
      ITHistSvc* tHistSvc;
      ATH_CHECK(service("THistSvc", tHistSvc));
      std::string ntuple_name{ name() + "Tree" };
      // create Ntuple and the branches
      m_tree = new TTree(ntuple_name.c_str(), "Ntuple of NSWL1Simulation");
      m_tree->Branch("runNumber",   &m_current_run, "runNumber/i");
      m_tree->Branch("eventNumber", &m_current_evt, "eventNumber/i");

      std::string tdir_name{ "/" + name() + "/" + ntuple_name };
      ATH_CHECK(tHistSvc->regTree(tdir_name,m_tree));
    }

    // retrieving the private tools implementing the simulation
    
    if(m_dosTGC){
      ATH_CHECK(m_pad_tds.retrieve());
      if(m_useLookup){
        ATH_CHECK(m_pad_trigger_lookup.retrieve());
      }
      else{
        ATH_CHECK(m_pad_trigger.retrieve());
      }
      ATH_CHECK(m_strip_tds.retrieve());
      ATH_CHECK(m_strip_cluster.retrieve());
      ATH_CHECK(m_strip_segment.retrieve());
      ATH_CHECK(m_padTriggerRdoKey.initialize());
    }
    
    if(m_doMM ){
      ATH_CHECK(m_mmtrigger.retrieve());
    }
    // Connect to Monitoring Service
    ATH_CHECK(m_monitors.retrieve());
    return StatusCode::SUCCESS;
  }


  StatusCode NSWL1Simulation::start() {
    ATH_MSG_INFO("start " << name() );

    ToolHandleArray<IMonitorToolBase>::iterator it;
    for ( const auto& mon : m_monitors ) {
      ATH_CHECK(mon->bookHists());
    }

    return StatusCode::SUCCESS;
  }


  StatusCode NSWL1Simulation::beginRun() {
    ATH_MSG_INFO( "beginRun " << name() );
    m_counters.clear();
    return StatusCode::SUCCESS;
  }


  StatusCode NSWL1Simulation::execute() {
    ATH_MSG_DEBUG( "execute" << name() );
    m_counters.clear();
    const DataHandle<EventInfo> pevt;
    ATH_CHECK( evtStore()->retrieve(pevt) );
    m_current_run = pevt->event_ID()->run_number();
    m_current_evt = pevt->event_ID()->event_number();


    m_counters.push_back(1.);
    std::vector<std::shared_ptr<PadData>> pads;  
    std::vector<std::unique_ptr<PadTrigger>> padTriggers;
    std::vector<std::unique_ptr<StripData>> strips;
    std::vector< std::unique_ptr<StripClusterData> > clusters;
    auto trgContainer=std::make_unique<Muon::NSW_TrigRawDataContainer>();
    auto padTriggerContainer = std::make_unique<Muon::NSW_PadTriggerDataContainer>();

    if(m_dosTGC){
      ATH_CHECK( m_pad_tds->gather_pad_data(pads) );
      if(m_useLookup){
        ATH_CHECK( m_pad_trigger_lookup->lookup_pad_triggers(pads, padTriggers) );
      }
      else{
          ATH_CHECK( m_pad_trigger->compute_pad_triggers(pads, padTriggers) );
      }
     
      // Fill RDO with data from simulated PadTriggers
      ATH_CHECK(PadTriggerRDOConverter::fillContainer(padTriggerContainer, padTriggers, m_current_evt));

      ATH_CHECK( m_strip_tds->gather_strip_data(strips,padTriggers) );
      ATH_CHECK( m_strip_cluster->cluster_strip_data(strips,clusters) );
      ATH_CHECK( m_strip_segment->find_segments(clusters,trgContainer) );
      
      auto rdohandle = SG::makeHandle( m_trigRdoContainer );
      ATH_CHECK( rdohandle.record( std::move(trgContainer)));

      SG::WriteHandle<Muon::NSW_PadTriggerDataContainer> padTriggerRdoHandle = SG::makeHandle(m_padTriggerRdoKey);
      ATH_CHECK(padTriggerRdoHandle.record(std::move(padTriggerContainer)));
    }

    //retrive the MM Strip hit data
    if(m_doMM){
      ATH_CHECK( m_mmtrigger->runTrigger() );
    }
    for ( const auto& mon : m_monitors) {
      ATH_CHECK(mon->fillHists());
    }
    if (m_tree) m_tree->Fill();

    return StatusCode::SUCCESS;
  }


  StatusCode NSWL1Simulation::finalize() {
    ATH_MSG_INFO( "finalize" << name() );
    for ( const auto& mon :  m_monitors ) {
      ATH_CHECK(mon->finalHists());
    }
    return StatusCode::SUCCESS;
  }

  int NSWL1Simulation::resultBuilder() const {
    return 0;
  }

}
