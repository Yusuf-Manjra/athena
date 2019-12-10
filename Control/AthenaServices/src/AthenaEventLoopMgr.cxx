/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#define  GAUDISVC_EVENTLOOPMGR_CPP

#include <cassert>
#include <ios>
#include <iostream>
#include <fstream> /* ofstream */
#include <iomanip>

#include "AthenaKernel/ITimeKeeper.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IAthenaEvtLoopPreSelectTool.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaKernel/EventContextClid.h"
#include "AthenaKernel/errorcheck.h"

#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/Algorithm.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

#include "xAODEventInfo/EventInfo.h"             
#include "xAODEventInfo/EventAuxInfo.h"          
#include "xAODEventInfo/EventInfoContainer.h"    
#include "xAODEventInfo/EventInfoAuxContainer.h" 
#include "EventInfoUtils/EventInfoFromxAOD.h"

#include "ClearStorePolicy.h"

#include "AthenaEventLoopMgr.h"
#include "PersistentDataModel/AthenaAttributeList.h"


//=========================================================================
// Standard Constructor
//=========================================================================
AthenaEventLoopMgr::AthenaEventLoopMgr(const std::string& nam, 
				       ISvcLocator* svcLoc)
  : MinimalEventLoopMgr(nam, svcLoc), 
    m_incidentSvc ( "IncidentSvc",  nam ), 
    m_eventStore( "StoreGateSvc", nam ), 
    m_evtSelector(nullptr), m_evtSelCtxt(nullptr),
    m_histoDataMgrSvc( "HistogramDataSvc",         nam ), 
    m_histoPersSvc   ( "HistogramPersistencySvc",  nam ), 
    m_activeStoreSvc ( "ActiveStoreSvc",           nam ),
    m_pITK(nullptr), 
    m_currentRun(0), m_firstRun(true), m_tools(this), m_useSecondaryEventNumber(false),
	m_nevt(0), m_writeHists(false),
    m_nev(0), m_proc(0), m_useTools(false), 
    m_chronoStatSvc( "ChronoStatSvc", nam ),
    m_conditionsCleaner( "Athena::ConditionsCleanerSvc", nam )
{
  declareProperty("EvtStore", m_eventStore, "The StoreGateSvc instance to interact with for event payload" );
  declareProperty("EvtSel", m_evtsel, 
		  "Name of Event Selector to use. If empty string (default) "
		  "take value from ApplicationMgr");
  declareProperty("HistogramPersistency", m_histPersName="",
		  "Histogram persistency technology to use: ROOT, HBOOK, NONE. "
		  "By default (empty string) get property value from "
		  "ApplicationMgr");
  declareProperty("TimeKeeper", m_timeKeeperName, 
		  "Name of TimeKeeper to use. NONE or empty string (default) "
		  "means no time limit control on event loop");
  m_timeKeeperName.declareUpdateHandler(&AthenaEventLoopMgr::setupTimeKeeper, this);
  declareProperty("HistWriteInterval",    m_writeInterval=0 ,
		  "histogram write/update interval");
  declareProperty("FailureMode",          m_failureMode=1 , 
		  "Controls behaviour of event loop depending on return code of"
		  " Algorithms. 0: all non-SUCCESSes terminate job. "
		  "1: RECOVERABLE skips to next event, FAILURE terminates job "
		  "(DEFAULT). 2: RECOVERABLE and FAILURE skip to next events");
  declareProperty("EventPrintoutInterval", m_eventPrintoutInterval=1,
                  "Print event heartbeat printouts every m_eventPrintoutInterval events");
  declareProperty("UseDetailChronoStat",m_doChrono=false);
  declareProperty("ClearStorePolicy",
		  m_clearStorePolicy = "EndEvent",
		  "Configure the policy wrt handling of when the "
		  "'clear-the-event-store' event shall happen: at EndEvent "
		  "(default as it is makes things easier for memory management"
		  ") or at BeginEvent (easier e.g. for interactive use)");
  declareProperty("PreSelectTools",m_tools,"AlgTools for event pre-selection")->
declareUpdateHandler( &AthenaEventLoopMgr::setupPreSelectTools, this );
  declareProperty("UseSecondaryEventNumber", m_useSecondaryEventNumber = false,
                  "In case of DoubleEventSelector use event number from secondary input");

  

}

//=========================================================================
// Standard Destructor
//=========================================================================
AthenaEventLoopMgr::~AthenaEventLoopMgr()   
{
}

//=========================================================================
// implementation of IAppMgrUI::initalize
//=========================================================================
StatusCode AthenaEventLoopMgr::initialize()    
{

  // configure our MsgStream
  info() << "Initializing " << name()
         << " - package version " << PACKAGE_VERSION << endmsg ;
 

  StatusCode sc = MinimalEventLoopMgr::initialize();
  if ( !sc.isSuccess() ) 
  {
    error() << "Failed to initialize base class MinimalEventLoopMgr"
            << endmsg;
    return sc;
  }

//-------------------------------------------------------------------------
// Setup access to event data services
//-------------------------------------------------------------------------

  sc = m_eventStore.retrieve();
  if( !sc.isSuccess() )  
  {
    fatal() << "Error retrieving pointer to StoreGateSvc"
            << endmsg;
    return sc;
  }

//--------------------------------------------------------------------------
// Get the references to the services that are needed by the ApplicationMgr 
// itself
//--------------------------------------------------------------------------
  sc = m_incidentSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    fatal() << "Error retrieving IncidentSvc." << endmsg;
    return sc;
  }

//--------------------------------------------------------------------------
// Access Property Manager interface:
//--------------------------------------------------------------------------
  SmartIF<IProperty> prpMgr(serviceLocator());
  if ( !prpMgr.isValid() ) 
  {
    fatal() << "IProperty interface not found in ApplicationMgr." 
            << endmsg;
    return StatusCode::FAILURE;
  }


//--------------------------------------------------------------------------
// Set up the Histogram Service
//--------------------------------------------------------------------------
  sc = m_histoDataMgrSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    fatal() << "Error retrieving HistogramDataSvc" 
            << endmsg;
    return sc;
  }
    
  const std::string& histPersName(m_histPersName.value());
  if ( histPersName.length() == 0 )    
  {
    setProperty(prpMgr->getProperty("HistogramPersistency")).ignore();
  }

  if ( histPersName != "NONE" )   {

    m_histoPersSvc = IConversionSvc_t( "HistogramPersistencySvc", 
				       this->name() );

    if( !sc.isSuccess() )  {
      warning() << "Histograms cannot not be saved - though required." 
                << endmsg;
    } else {

      IService *is = nullptr;
      if (histPersName == "ROOT") {
	sc = serviceLocator()->service("RootHistSvc", is);
      } else if ( histPersName == "HBOOK" ) {
	sc = serviceLocator()->service("HbookHistSvc", is);
      }

      if (sc.isFailure()) {
	error() << "could not locate actual Histogram persistency service"
                << endmsg;
      } else {
	Service *s = dynamic_cast<Service*>(is);
	if (s == nullptr) {
	  error() << "Could not dcast HistPersSvc to a Service"
		<< endmsg;
	} else {
	  const Property &prop = s->getProperty("OutputFile");
	  std::string val;
	  try {
	    const StringProperty &sprop = dynamic_cast<const StringProperty&>( prop );

	    val = sprop.value();

	  } catch (...) {
            verbose() << "could not dcast OutputFile property to a StringProperty."
                      << " Need to fix Gaudi."
                      << endmsg;

	    val = prop.toString();

	    //	    val.erase(0,val.find(":")+1);
	    //	    val.erase(0,val.find("\"")+1);
	    //	    val.erase(val.find("\""),val.length());
	  }

	  if (val != "" && 
	      val != "UndefinedROOTOutputFileName" && 
	      val != "UndefinedHbookOutputFileName" ) {
	    m_writeHists = true;
	  }

	}
      }
    }
    

  }  else { if (msgLevel(MSG::DEBUG)) { debug() << "Histograms saving not required." 
                                                << endmsg; } }
//-------------------------------------------------------------------------
// Setup EventSelector service
//-------------------------------------------------------------------------
  const std::string& selName(m_evtsel.value());
  // the evt sel is usually specified as a property of ApplicationMgr
  if (selName.empty()) 
    sc = setProperty(prpMgr->getProperty("EvtSel"));
  if (sc.isFailure()) warning() << "Unable to set EvtSel property" << endmsg;

  // We do not expect a Event Selector necessarily being declared
  if( !selName.empty() && selName != "NONE") {
    IEvtSelector* theEvtSel(nullptr);
    StatusCode sc(serviceLocator()->service( selName, theEvtSel ));
    if( sc.isSuccess() && ( theEvtSel != m_evtSelector ) ) {
      // Event Selector changed (or setup for the first time)
      m_evtSelector = theEvtSel;
      
      // reset iterator
      if (m_evtSelector->createContext(m_evtSelCtxt).isFailure()) {
	fatal() << "Can not create the event selector Context." 
                << endmsg;
	return StatusCode::FAILURE;
      }
      if (msgLevel(MSG::INFO)) {
	INamedInterface* named (dynamic_cast< INamedInterface* >(theEvtSel));
	if (nullptr != named) {
	  info() << "Setup EventSelector service " << named->name( ) 
                 << endmsg;
	}
      }
    } else if (sc.isFailure()) {
      fatal() << "No valid event selector called " << selName 
              << endmsg;
      return StatusCode::FAILURE;
    }
  }  
//-------------------------------------------------------------------------
// Setup TimeKeeper service
//-------------------------------------------------------------------------
  // the time keeper may one day be specified as a property of ApplicationMgr
  //  setProperty(prpMgr->getProperty("TimeKeeper"));

//-------------------------------------------------------------------------
// Setup 'Clear-Store' policy
//-------------------------------------------------------------------------
  try {
    setClearStorePolicy( m_clearStorePolicy );
  } catch(...) {
    return StatusCode::FAILURE;
  }
//-------------------------------------------------------------------------
// Make sure the ActiveStoreSvc is initialized.
// We don't use this, but want to be sure that it gets created
// during initialization, to avoid heap fragmentation.
//-------------------------------------------------------------------------
  sc = m_activeStoreSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    fatal() << "Error retrieving ActiveStoreSvc." << endmsg;
    return sc;
  }

  // Get the AlgExecStateSvc
  m_aess = serviceLocator()->service("AlgExecStateSvc");
  if( !m_aess.isValid() ) {
    fatal() << "Error retrieving AlgExecStateSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  // Listen to the BeforeFork incident
  m_incidentSvc->addListener(this,"BeforeFork",0);

  CHECK( m_conditionsCleaner.retrieve() );

  // Print if we override the event number using the one from secondary event
  if (m_useSecondaryEventNumber)
  {
    info() << "Using secondary event number." << endmsg;
  }

  return sc;
}

//=========================================================================
// property handlers
//=========================================================================
void 
AthenaEventLoopMgr::setupTimeKeeper(Property&) {
  const std::string& tkName(m_timeKeeperName.value());
  // We do not expect a TimeKeeper necessarily being declared  
  if( tkName != "NONE" && tkName.length() != 0) {
    if (!(serviceLocator()->service( tkName, m_pITK, true)).isSuccess()) 
      error() << "TimeKeeper not found." << endmsg;
    else info() << "No TimeKeeper selected. "
                << "No time limit control on event loop." 
                << endmsg;
  }
}

void 
AthenaEventLoopMgr::setClearStorePolicy(Property&) {
  const std::string& policyName = m_clearStorePolicy.value();

  if ( policyName != "BeginEvent" &&
       policyName != "EndEvent" ) {

    fatal() << "Unknown policy [" << policyName 
            << "] for the 'ClearStore-policy !"
            << endmsg
            << "Valid values are: BeginEvent, EndEvent"
            << endmsg;
    throw GaudiException("Can not setup 'ClearStore'-policy",
			 name(),
			 StatusCode::FAILURE);
  }

  return;
}

void
AthenaEventLoopMgr::setupPreSelectTools(Property&) {

  m_toolInvoke.clear();
  m_toolReject.clear();
  m_toolAccept.clear();

  m_tools.retrieve().ignore();
  if(m_tools.size() > 0) {
    m_useTools=true;
    m_toolInvoke.resize(m_tools.size());
    m_toolReject.resize(m_tools.size());
    m_toolAccept.resize(m_tools.size());

    tool_iterator firstTool = m_tools.begin();
    tool_iterator lastTool  = m_tools.end();
    unsigned int toolCtr = 0;
    for ( ; firstTool != lastTool; firstTool++ )
      {
	// reset statistics
	m_toolInvoke[toolCtr] = 0;
	m_toolReject[toolCtr] = 0;
	m_toolAccept[toolCtr] = 0;
        toolCtr++;
      }
  }

  return;

}

//=========================================================================
// implementation of IAppMgrUI::finalize
//=========================================================================
StatusCode AthenaEventLoopMgr::finalize()    
{

  StatusCode sc = MinimalEventLoopMgr::finalize();
  if (sc.isFailure()) 
  {
    error() 
	  << "Error in Algorithm Finalize" 
	  << endmsg;
  }

  StatusCode sc2 = writeHistograms(true);
  if (sc2.isFailure()) 
  {
    error() 
	  << "Error in writing Histograms"
	  << endmsg;
  }

  // Release all interfaces (ignore StatusCodes)
  m_histoDataMgrSvc.release().ignore();
  m_histoPersSvc.release().ignore();
  m_activeStoreSvc.release().ignore();

  m_evtSelector   = releaseInterface(m_evtSelector);
  m_incidentSvc.release().ignore();

  delete m_evtSelCtxt; m_evtSelCtxt = nullptr;

  if(m_useTools) {
    tool_iterator firstTool = m_tools.begin();
    tool_iterator lastTool  = m_tools.end();
    unsigned int toolCtr = 0;
    info() << "Summary of AthenaEvtLoopPreSelectTool invocation: (invoked/success/failure)" << endmsg;
    info() << "-----------------------------------------------------" << endmsg;

    for ( ; firstTool != lastTool; firstTool++ ) {
      info() << std::setw(2)     << std::setiosflags(std::ios_base::right)
             << toolCtr+1 << ".) " << std::resetiosflags(std::ios_base::right)
             << std::setw(48) << std::setfill('.')
             << std::setiosflags(std::ios_base::left)
             << (*firstTool)->name() << std::resetiosflags(std::ios_base::left)
             << std::setfill(' ')
             << " ("
             << std::setw(6) << std::setiosflags(std::ios_base::right)
             << m_toolInvoke[toolCtr]
             << "/"
             << m_toolAccept[toolCtr]
             << "/"
             << m_toolReject[toolCtr]
             << ")"
             << endmsg;
      toolCtr++;
    }
  }
  return ( sc.isFailure() || sc2.isFailure() ) ? StatusCode::FAILURE :
    StatusCode::SUCCESS;

}

//=========================================================================
// write out the histograms
//=========================================================================
StatusCode AthenaEventLoopMgr::writeHistograms(bool force) {

  StatusCode sc (StatusCode::SUCCESS);
  
  if ( 0 != m_histoPersSvc && m_writeHists ) {
    std::vector<DataObject*> objects;
    sc = m_histoDataMgrSvc->traverseTree( [&objects]( IRegistry* reg, int ) {
        DataObject* obj = reg->object();
        if ( !obj || obj->clID() == CLID_StatisticsFile ) return false;
        objects.push_back( obj );
        return true;
      } );

    if ( !sc.isSuccess() ) {
      error() << "Error while traversing Histogram data store" << endmsg;
      return sc;
    }

    if ( objects.size() > 0) {
      int writeInterval(m_writeInterval.value());

      if ( m_nevt == 1 || force || 
           (writeInterval != 0 && m_nevt%writeInterval == 0) ) {

        // skip /stat entry!
        sc = std::accumulate( begin( objects ), end( objects ), sc, [&]( StatusCode isc, auto& i ) {
            IOpaqueAddress* pAddr = nullptr;
            StatusCode      iret  = m_histoPersSvc->createRep( i, pAddr );
            if ( iret.isFailure() ) return iret;
            i->registry()->setAddress( pAddr );
            return isc;
          } );
        sc = std::accumulate( begin( objects ), end( objects ), sc, [&]( StatusCode isc, auto& i ) {
            IRegistry* reg  = i->registry();
            StatusCode iret = m_histoPersSvc->fillRepRefs( reg->address(), i );
            return iret.isFailure() ? iret : isc;
          } );
        if ( ! sc.isSuccess() ) {
          error() << "Error while saving Histograms." << endmsg;
        }
      }

      if (force || (writeInterval != 0 && m_nevt%writeInterval == 0) ) {
        if (msgLevel(MSG::DEBUG)) { debug() << "committing Histograms" << endmsg; }
        m_histoPersSvc->conversionSvc()->commitOutput("",true).ignore();
      }
    }
          
  }
  
  return sc;
}

//=========================================================================
// Call sysInitialize() on all algorithms and output streams
//=========================================================================
StatusCode AthenaEventLoopMgr::initializeAlgorithms() {

  // Initialize the list of Algorithms. Note that existing Algorithms
  // are protected against multiple initialization attempts.
  ListAlg::iterator ita;
  for ( ita = m_topAlgList.begin(); ita != m_topAlgList.end(); ++ita )
    {
      StatusCode sc = (*ita)->sysInitialize();
      if( sc.isFailure() )
	{
	  error()
		<< "Unable to initialize Algorithm: "
		<< (*ita)->name()
		<< endmsg;
	  return sc;
	}
    }

  // Initialize the list of Output Streams. Note that existing Output Streams
  // are protected against multiple initialization attempts.
  for (ita = m_outStreamList.begin(); ita != m_outStreamList.end(); ++ita )
    {
      StatusCode sc = (*ita)->sysInitialize();
      if( sc.isFailure() ) {
	error()
	      << "Unable to initialize Output Stream: "
	      << (*ita)->name()
	      << endmsg;
	return sc;
      }

    }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Run the algorithms for the current event
//=========================================================================
StatusCode AthenaEventLoopMgr::executeAlgorithms(const EventContext& ctx) {

  // Call the execute() method of all top algorithms 
  for ( ListAlg::iterator ita = m_topAlgList.begin(); 
        ita != m_topAlgList.end();
        ita++ ) 
  {
    const StatusCode& sc = (*ita)->sysExecute(ctx); 
    // this duplicates what is already done in Algorithm::sysExecute, which
    // calls Algorithm::setExecuted, but eventually we plan to remove that 
    // function
    m_aess->algExecState(*ita,ctx).setState(AlgExecState::State::Done, sc);
    if ( !sc.isSuccess() ) {
      info()  << "Execution of algorithm "
              << (*ita)->name() << " failed with StatusCode::" << sc
              << endmsg;
      return sc;
    }
  }

  return StatusCode::SUCCESS;
}


//=========================================================================
// Execute one event
//=========================================================================
StatusCode AthenaEventLoopMgr::executeEvent(EventContext&& ctx)    
{

  const EventInfo* pEvent(nullptr);
  std::unique_ptr<EventInfo> pEventPtr;
  unsigned int conditionsRun = EventIDBase::UNDEFNUM;
  EventID eventID;
  if ( m_evtSelCtxt )
  { // Deal with the case when an EventSelector is provided
    // Retrieve the Event object
    const AthenaAttributeList* pAttrList = eventStore()->tryConstRetrieve<AthenaAttributeList>("Input");
    if ( pAttrList != nullptr && pAttrList->size() > 6 ) { // Try making EventID-only EventInfo object from in-file TAG
      try {
        unsigned int runNumber = (*pAttrList)["RunNumber"].data<unsigned int>();
        unsigned long long eventNumber = (*pAttrList)["EventNumber"].data<unsigned long long>();
        unsigned int eventTime = (*pAttrList)["EventTime"].data<unsigned int>();
        unsigned int eventTimeNS = (*pAttrList)["EventTimeNanoSec"].data<unsigned int>();
        unsigned int lumiBlock = (*pAttrList)["LumiBlockN"].data<unsigned int>();
        unsigned int bunchId = (*pAttrList)["BunchId"].data<unsigned int>();
    
        // an option to override primary eventNumber with the secondary one in case of DoubleEventSelector
        if ( m_useSecondaryEventNumber ) {
            unsigned long long eventNumberSecondary{};
            if ( !(pAttrList->exists("hasSecondaryInput") && (*pAttrList)["hasSecondaryInput"].data<bool>()) ) {
                fatal() << "Secondary EventNumber requested, but secondary input does not exist!" << endmsg;
                return StatusCode::FAILURE;
            }
            if ( pAttrList->exists("EventNumber_secondary") ) {
                eventNumberSecondary = (*pAttrList)["EventNumber_secondary"].data<unsigned long long>();
            }
            else {
                // try legacy EventInfo if secondary input did not have attribute list
                // primary input should not have this EventInfo type
                const EventInfo* pEventSecondary = eventStore()->tryConstRetrieve<EventInfo>();
                if (pEventSecondary) {
                    eventNumberSecondary = pEventSecondary->event_ID()->event_number();
                }
                else {
                    fatal() << "Secondary EventNumber requested, but it does not exist!" << endmsg;
                    return StatusCode::FAILURE;
                }
            }
            if (eventNumberSecondary != 0) {
                bool doEvtHeartbeat(m_eventPrintoutInterval.value() > 0 && 
                                    0 == (m_nev % m_eventPrintoutInterval.value()));
                if (doEvtHeartbeat) {
                    info() << "  ===>>>  using secondary event #" << eventNumberSecondary << " instead of #" << eventNumber << "<<<===" << endmsg;
                }
                eventNumber = eventNumberSecondary;
            }
        }
    
        pEventPtr = std::make_unique<EventInfo>
          (new EventID(runNumber, eventNumber, eventTime, eventTimeNS, lumiBlock, bunchId), (EventType*)nullptr);
        pEvent = pEventPtr.get();

        if (pAttrList->exists ("ConditionsRun"))
          conditionsRun = (*pAttrList)["ConditionsRun"].data<unsigned int>();
        else
          conditionsRun = runNumber;
        
	eventID=*(pEvent->event_ID());
      } catch (...) {
      }
/* FIXME: PvG, not currently written
      if ( pEvent != 0 && pAttrList->size() > 7 ) { // Try adding EventType information
        try {
          float eventWeight = (*pAttrList)["EventWeight"].data<float>();
          const EventType* pType = new EventType();
          pEvent->setEventType(pType);
          pEvent->event_type()->set_mc_event_weight(eventWeight);
        } catch (...) {
          pEvent->setEventType(0);
        }
      }
*/
    }
    if ( pEvent == nullptr ) { //Try getting EventInfo from old-style object
      pEvent=eventStore()->tryConstRetrieve<EventInfo>();
      if (pEvent) {
	eventID=(*pEvent->event_ID());
      }
      else { //Try getting xAOD::EventInfo object
	const xAOD::EventInfo* xAODEvent=
	  eventStore()->tryConstRetrieve<xAOD::EventInfo>(); 
	if (xAODEvent==nullptr) {
	  error() << "Failed to get EventID from input. Tried old-style and xAOD::EventInfo" <<endmsg;
	  return StatusCode::FAILURE;
	}
	// Record the old-style object for those clients that still need it
	pEventPtr = std::make_unique<EventInfo>(new EventID(eventIDFromxAOD(xAODEvent)), new EventType(eventTypeFromxAOD(xAODEvent)));
	pEvent = pEventPtr.get();
	eventID=*(pEvent->event_ID());
	StatusCode sc = eventStore()->record(std::move(pEventPtr),"");
	if( !sc.isSuccess() )  {
	  error() << "Error declaring event data object" << endmsg;
	  return StatusCode::FAILURE;
	}
      }
    }
  }
  else 
  {
    // With no iterator it's up to us to create an EventInfo
    pEventPtr = std::make_unique<EventInfo>
      (new EventID(1,m_nevt,0), new EventType());
    pEvent = pEventPtr.get();
    pEventPtr->event_ID()->set_lumi_block( m_nevt );
    eventID=*(pEvent->event_ID());
    StatusCode sc = eventStore()->record(std::move(pEventPtr),"");
    if( !sc.isSuccess() )  {
      error() 
	    << "Error declaring event data object" << endmsg;
      return (StatusCode::FAILURE);
    } 
  }

  if (installEventContext (ctx, eventID, conditionsRun).isFailure())
  {
    error() 
          << "Error installing event context object" << endmsg;
    return (StatusCode::FAILURE);
  }

  /// Fire begin-Run incident if new run:
  if (m_firstRun || (m_currentRun != eventID.run_number()) ) {
    // Fire EndRun incident unless this is the first run
    if (!m_firstRun) {
      m_incidentSvc->fireIncident(Incident(name(),IncidentType::EndRun));
    }
    m_firstRun=false;
    m_currentRun = eventID.run_number();

    info() << "  ===>>>  start of run " << m_currentRun << "    <<<==="
           << endmsg;

    m_incidentSvc->fireIncident(Incident(name(),IncidentType::BeginRun,ctx));
  }

  bool toolsPassed=true;
  bool eventFailed = false;
  // Call any attached tools to reject events early
  unsigned int toolCtr=0;
  if(m_useTools) {
    if (pEvent == nullptr) {
      error() << "Tools for event selection work only with old-style EventInfo!"  << endmsg;
      return StatusCode::FAILURE;
    }

    tool_store::iterator theTool = m_tools.begin();
    tool_store::iterator lastTool  = m_tools.end();
    while(toolsPassed && theTool!=lastTool ) 
      {
        toolsPassed = (*theTool)->passEvent(ctx.eventID()); 
	m_toolInvoke[toolCtr]++;
        {toolsPassed ? m_toolAccept[toolCtr]++ : m_toolReject[toolCtr]++;}
        toolCtr++;
        theTool++;
      }
  }


  uint64_t evtNumber = eventID.event_number();
  bool doEvtHeartbeat(m_eventPrintoutInterval.value() > 0 && 
                      0 == (m_nev % m_eventPrintoutInterval.value()));
  if (doEvtHeartbeat)  {
    if(!m_useTools) info() << "  ===>>>  start processing event #" << evtNumber << ", run #" << m_currentRun 
	<< " " << m_nev << " events processed so far  <<<===" << endmsg;
   else info() << "  ===>>>  start processing event #" << evtNumber << ", run #" << m_currentRun 
	<< " " << m_nev << " events read and " << m_proc 
        << " events processed so far  <<<===" << endmsg;   
  }

  // Reset the timeout singleton
  resetTimeout(Athena::Timeout::instance(ctx));
  if(toolsPassed) {
  // Fire BeginEvent "Incident"
  //m_incidentSvc->fireIncident(Incident(*pEvent, name(),"BeginEvent"));

  // An incident may schedule a stop, in which case is better to exit before the actual execution.
  if ( m_scheduledStop ) {
    always() << "A stopRun was requested by an incidentListener. "
             << "Do not process this event." 
             << endmsg;
    return (StatusCode::SUCCESS);
  }

  CHECK( m_conditionsCleaner->event (ctx, false) );

  // Call the execute() method of all top algorithms 
  StatusCode sc = executeAlgorithms(ctx);

  if(!sc.isSuccess()) {
    eventFailed = true; 
    m_aess->setEventStatus( EventStatus::AlgFail, ctx );

 /// m_failureMode 1, 
 ///    RECOVERABLE: skip algorithms, but do not terminate job
 ///    FAILURE: terminate job 
    if (m_failureMode == 1 && sc.isRecoverable() ) {
      warning() << "RECOVERABLE error returned by algorithm. "
                << "Skipping remaining algorithms." << std::endl
                << "\tNo output will be written for this event, "
                << "but job will continue to next event"
                << endmsg;
      eventFailed = false;
    }

 /// m_failureMode 2: skip algorithms, but do not terminate job
    if (m_failureMode >= 2) {
      info() << "Skipping remaining algorithms." << std::endl
             << "\tNo output will be written for this event, "
             << "but job will continue to next event"
             << endmsg;
      eventFailed = false;
    }

  }  else {

    m_aess->setEventStatus( EventStatus::Success, ctx );

    // Call the execute() method of all output streams 
    for (ListAlg::iterator ito = m_outStreamList.begin(); 
	 ito != m_outStreamList.end(); ito++ ) {
      sc = (*ito)->sysExecute(ctx); 
      if( !sc.isSuccess() ) {
	eventFailed = true; 
      } 
    }
    
    writeHistograms().ignore();

  }

  // Fire EndEvent "Incident"
  //m_incidentSvc->fireIncident(Incident(*pEvent, name(),"EndEvent"));
  ++m_proc;
  }  // end of toolsPassed test
  ++m_nev;
  if (doEvtHeartbeat) {
    if(!m_useTools) info() << "  ===>>>  done processing event #" << evtNumber << ", run #" << m_currentRun 
                           << " " << m_nev << " events processed so far  <<<===" << endmsg;
    else info()	<< "  ===>>>  done processing event #" << evtNumber << ", run #" << m_currentRun 
                << " " << m_nev << " events read and " << m_proc 
                << " events processed so far <<<===" << endmsg;
   std::ofstream outfile( "eventLoopHeartBeat.txt");
   if ( !outfile ) {
     error() << " unable to open: eventLoopHeartBeat.txt" << endmsg;
   } else {
     outfile << "  done processing event #" << evtNumber << ", run #" << m_currentRun 
	     << " " << m_nev << " events read so far  <<<===" << std::endl;
     outfile.close();
   }  

  }

  //------------------------------------------------------------------------
  // Check if there was an error processing current event
  //------------------------------------------------------------------------
  return eventFailed?StatusCode::FAILURE:StatusCode::SUCCESS;
  
}

//=========================================================================
// implementation of IEventProcessor::executeRun
//=========================================================================
StatusCode AthenaEventLoopMgr::executeRun(int maxevt)
{
  if (!(this->nextEvent(maxevt)).isSuccess()) return StatusCode::FAILURE;
  m_incidentSvc->fireIncident(Incident(name(),"EndEvtLoop"));
  m_incidentSvc->fireIncident(Incident(name(),IncidentType::EndRun));

  return StatusCode::SUCCESS;
}

//=========================================================================
// implementation of IAppMgrUI::nextEvent
//=========================================================================
StatusCode AthenaEventLoopMgr::nextEvent(int maxevt)   
{
  // make nextEvent(0) a dummy call
  if (0 == maxevt) return StatusCode::SUCCESS;

  static int        total_nevt = 0;

  // the current 'clear-store' policy
  static const ClearStorePolicy::Type s_clearStore = 
    clearStorePolicy( m_clearStorePolicy.value(), msgStream() );


  // These following two initialization loops are performed here in case new
  // Top level Algorithms or Output Streams have been created interactively
  // at run-time instead of configuration time. Note also that it is possible
  // that some newly created Algorithms are still not initialized as a result
  // of these loops (e.g. where the membership of an existing Sequencer is 
  // changed at run-time. In this case, the Algorithm::sysExecute() function 
  // will ensure that the Algorithm is correctly initialized. This mechanism 
  // actually makes loops redundant, but they do  provide a well defined 
  // location for the initialization to take place in the non-interactive case.

  StatusCode sc = initializeAlgorithms();
  if(sc.isFailure())
    return sc;

  // loop over events if the maxevt (received as input) is different from -1.
  // if evtmax is -1 it means infinite loop (till time limit that is)
  //  int nevt(0);
  bool noTimeLimit(false);

  while((maxevt == -1 || m_nevt < maxevt) && 
	(noTimeLimit = (m_pITK == nullptr || m_pITK->nextIter()) ) ) {

   if(m_doChrono && total_nevt>0) m_chronoStatSvc->chronoStart("EventLoopMgr"); //start after first event
   if(m_doChrono && total_nevt>0) m_chronoStatSvc->chronoStart("EventLoopMgr_preexec");

    // Check if there is a scheduled stop issued by some algorithm/sevice
    if ( m_scheduledStop ) {
      m_scheduledStop = false;
      always() << "A stopRun was requested. Terminating event loop." << endmsg;
      break;
    }

   

    ++m_nevt; ++total_nevt; 
    //-----------------------------------------------------------------------
    // Clear the event store, if used in the event loop
    //-----------------------------------------------------------------------
    if( s_clearStore == ClearStorePolicy::BeginEvent &&
	0 != total_nevt ) {
      sc = eventStore()->clearStore();
      if( !sc.isSuccess() ) {
	error()
	      << "Clear of Event data store failed" << endmsg;
        m_incidentSvc->fireIncident(Incident(name(),"EndEvtLoop"));
	return sc;
      }
    }



    //-----------------------------------------------------------------------
    // we need an EventInfo Object to fire the incidents. 
    //-----------------------------------------------------------------------
    if ( m_evtSelCtxt )
    {   // Deal with the case when an EventSelector is provided

      IOpaqueAddress* addr = nullptr;

      sc = m_evtSelector->next(*m_evtSelCtxt);

      if ( !sc.isSuccess() )
      {
        // This is the end of the loop. No more events in the selection
        info() << "No more events in event selection " << endmsg;
	sc = StatusCode::SUCCESS;
        break;
      }

      if (m_evtSelector->createAddress(*m_evtSelCtxt, addr).isFailure()) {
        error()
	      << "Could not create an IOpaqueAddress" << endmsg;
        break;
      }
      

      // Most iterators provide the IOA of an event header (EventInfo, DataHeader)
      if (nullptr != addr) {
	//create its proxy
	sc = eventStore()->recordAddress(addr);
	if( !sc.isSuccess() ) {
	  warning() << "Error declaring Event object" << endmsg;
	  continue;
	}
      } 
      if ((sc=eventStore()->loadEventProxies()).isFailure()) {
	error() << "Error loading Event proxies" << endmsg;
	continue;
      } 
    }
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStop("EventLoopMgr_preexec");
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStart("EventLoopMgr_execute");
    // create an empty EventContext that will get set inside executeEvent
    EventContext ctx;
    // Execute event for all required algorithms
    sc = executeEvent( std::move( ctx ) );
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStop("EventLoopMgr_execute");
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStart("EventLoopMgr_postexec");
    if( !sc.isSuccess() )
    {
      error() 
	    << "Terminating event processing loop due to errors" << endmsg;
      break;
    }

    //-----------------------------------------------------------------------
    // Clear the event store, if used in the event loop
    //-----------------------------------------------------------------------
    if ( s_clearStore == ClearStorePolicy::EndEvent ) {
      sc = eventStore()->clearStore();
      if( !sc.isSuccess() ) {
        error() 
	      << "Clear of Event data store failed" << endmsg;
	break;
      }
    }
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStop("EventLoopMgr_postexec");
    if(m_doChrono && total_nevt>1) m_chronoStatSvc->chronoStop("EventLoopMgr");
  } //event loop

   

  return (noTimeLimit ? sc : StatusCode::FAILURE);

}


//=========================================================================
// Seek to a given event.
// The event selector must support the IEvtSelectorSeek interface for this to work.
//=========================================================================
StatusCode AthenaEventLoopMgr::seek (int evt)
{
  IEvtSelectorSeek* is = dynamic_cast<IEvtSelectorSeek*> (m_evtSelector);
  if (is == nullptr) {
    error() << "Seek failed; unsupported by event selector"
	  <<endmsg;
    return StatusCode::FAILURE;
  }

  if (!m_evtSelCtxt) {
    if (m_evtSelector->createContext(m_evtSelCtxt).isFailure()) {
      fatal() << "Can not create the event selector Context."
              << endmsg;
      return StatusCode::FAILURE;
    }
  }

  StatusCode sc = is->seek (*m_evtSelCtxt, evt);
  if (sc.isSuccess()) {
    m_nevt = evt;
  }
  else {
    error() << "Seek failed." << endmsg;
  }
  return sc;
}


//=========================================================================
// Return the current event count.
//=========================================================================
int AthenaEventLoopMgr::curEvent() const
{
  return m_nevt;
}

//=========================================================================
// Return the collection size
//=========================================================================
int AthenaEventLoopMgr::size()
{
  IEvtSelectorSeek* cs = dynamic_cast<IEvtSelectorSeek*> (m_evtSelector);
  if (cs == nullptr) {
    error() << "Collection size unsupported by event selector"
	  <<endmsg;
    return -1;
  }

  if (!m_evtSelCtxt) {
    if (m_evtSelector->createContext(m_evtSelCtxt).isFailure()) {
      fatal() << "Can not create the event selector Context."
              << endmsg;
      return -1;
    }
  }

  return cs->size (*m_evtSelCtxt);
}

//=========================================================================
// Handle incidents
//=========================================================================
void AthenaEventLoopMgr::handle(const Incident& inc)
{
  if(inc.type()!="BeforeFork")
    return;

  if(!m_evtSelCtxt || !m_firstRun) {
    warning() << "Skipping BeforeFork handler. Either no event selector is provided or begin run has already passed" << endmsg;
  }

  // Initialize Algorithms and Output Streams
  StatusCode sc = initializeAlgorithms();
  if(sc.isFailure()) {
    error() << "Failed to initialize Algorithms" << endmsg;
    return; 
  }

  // Construct EventInfo
  IOpaqueAddress* addr = nullptr;
  sc = m_evtSelector->next(*m_evtSelCtxt);
  if(!sc.isSuccess()) {
    info() << "No more events in event selection " << endmsg;
    return;
  }
  sc = m_evtSelector->createAddress(*m_evtSelCtxt, addr);
  if (sc.isFailure()) {
    error() << "Could not create an IOpaqueAddress" << endmsg;
    return; 
  }
  if (nullptr != addr) {
    //create its proxy
    sc = eventStore()->recordAddress(addr);
    if(!sc.isSuccess()) {
      error() << "Error declaring Event object" << endmsg;
      return;
    }
  } 
  
  if(eventStore()->loadEventProxies().isFailure()) {
    warning() << "Error loading Event proxies" << endmsg;
    return;
  }

  EventID eventID;
  const EventInfo* pEvent=eventStore()->tryConstRetrieve<EventInfo>(); //Try getting EventInfo from old-style object
  if (pEvent) {
    eventID=*(pEvent->event_ID());
  }
  else { //Try getting xAOD::EventInfo object
    const xAOD::EventInfo* xAODEvent=eventStore()->tryConstRetrieve<xAOD::EventInfo>();
    if (xAODEvent==nullptr) {
      error() << "Failed to get EventID from input. Tried old-style and xAOD::EventInfo" <<endmsg;
      return;
    }
    eventID=eventIDFromxAOD(xAODEvent);
  }

  // Need to make sure we have a valid EventContext in place before
  // doing anything that could fire incidents.
  unsigned int conditionsRun = eventID.run_number();
  {
    const AthenaAttributeList* pAttrList = eventStore()->tryConstRetrieve<AthenaAttributeList>();
    if ( pAttrList != nullptr ) {
      if (pAttrList->exists ("ConditionsRun"))
        conditionsRun = (*pAttrList)["ConditionsRun"].data<unsigned int>();
    }
  }
  EventContext ctx;
  if (installEventContext (ctx, eventID, conditionsRun).isFailure()) {
    error() << "Unable to install EventContext object" << endmsg;
    return;
  }

  m_incidentSvc->fireIncident(Incident(name(),IncidentType::BeginRun,ctx));
  m_firstRun=false;
  m_currentRun = eventID.run_number();

  // Clear Store
  const ClearStorePolicy::Type s_clearStore = clearStorePolicy( m_clearStorePolicy.value(), msgStream() );
  if(s_clearStore==ClearStorePolicy::EndEvent) {
    sc = eventStore()->clearStore();
    if(!sc.isSuccess()) {
      error() << "Clear of Event data store failed" << endmsg;
    }
  }
}

//=========================================================================
// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
//=========================================================================
StatusCode 
AthenaEventLoopMgr::queryInterface(const InterfaceID& riid, 
				   void** ppvInterface) 
{
  if ( IEventSeek::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IEventSeek*>(this);
  }
  else if ( ICollectionSize::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ICollectionSize*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return MinimalEventLoopMgr::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
StoreGateSvc* 
AthenaEventLoopMgr::eventStore() const {
  return m_eventStore.get();
}


//=========================================================================
// Fill in our EventContext object and make it current.
//=========================================================================
StatusCode AthenaEventLoopMgr::installEventContext (EventContext& ctx,
                                                    const EventID& pEvent,
                                                    unsigned int conditionsRun)
{
  ctx.setEventID(pEvent);
  ctx.set(m_nev,0);

  Atlas::setExtendedEventContext(ctx,
                                 Atlas::ExtendedEventContext( eventStore()->hiveProxyDict(),
                                                              conditionsRun) );
  Gaudi::Hive::setCurrentContext( ctx );

  m_aess->reset( ctx );
  if (eventStore()->record(std::make_unique<EventContext> ( ctx ),
                           "EventContext").isFailure())
  {
    error() << "Error recording event context object" << endmsg;
    return (StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}
