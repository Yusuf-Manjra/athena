/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventCount.cxx
 * @brief Implementation of class EventCount
 */
 
#include "EventCount.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/DataHeader.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IClassIDSvc.h"

//--------------- Utility Struct Constructors ----------------
EventCount::ObjSum::ObjSum() : num(-1)
{keys.clear();}

EventCount::ObjSum::ObjSum(int n) : num(n)
{keys.clear();}

//___________________________________________________________________________
EventCount::EventCount(const std::string& name, ISvcLocator* pSvcLocator) : 
   Algorithm(name, pSvcLocator), 
   m_dump(false),
   m_sGevent ( "StoreGateSvc", name ),
   m_pCLIDSvc( "ClassIDSvc",   name ),
   m_nev(0),
   m_first(-1),
   m_final(-1),
   m_firstlb(-1),
   m_finallb(-1),
   m_currentRun(-1)
{
   // Declare the properties
   declareProperty("Dump", m_dump,"Bool of whether to dump object summary");
}

EventCount::~EventCount()
{}

StatusCode EventCount::initialize() 
{
   // clear containers
   m_runs.clear();
   m_clids.clear();
   m_summaries.clear();
   //m_prov_keys.clear();
   m_guids.clear();

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // Locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = m_sGevent.retrieve();
   if (!sc.isSuccess() || 0 == m_sGevent) {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      sc = StatusCode::FAILURE;
   }
   return(sc);
}

StatusCode EventCount::execute() 
{
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;

   // Get the event header, print out event and run number
   const DataHandle<EventInfo> evt;
   sc = m_sGevent->retrieve(evt);
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
   }
   if (!evt.isValid()) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
   }
   ++m_nev;
   if (m_nev==1) {
      m_first = evt->event_ID()->event_number(); 
      m_firstlb= evt->event_ID()->lumi_block();
   }
   // track final event number
   m_final = evt->event_ID()->event_number();
   m_finallb= evt->event_ID()->lumi_block();
   // keep list of runs
   if (m_currentRun!=static_cast<int>(evt->event_ID()->run_number())) {
      m_currentRun =static_cast<int>(evt->event_ID()->run_number());
      m_runs.push_back(static_cast<int>(evt->event_ID()->run_number()));
   }
   // keep list of event types
   if (evt->event_type()->test(EventType::IS_SIMULATION)) {
      m_types.insert("Simulation");
   }
   else {m_types.insert("Data");}
   if (evt->event_type()->test(EventType::IS_TESTBEAM)) {
      m_types.insert("Testbeam");
   }
   else {m_types.insert("Detector");}
   if (evt->event_type()->test(EventType::IS_CALIBRATION)) {
      m_types.insert("Calibration");
   }
   else {m_types.insert("Physics");}

   // Keep track of streams
   std::vector<TriggerInfo::StreamTag>::const_iterator STit  = evt->trigger_info()->streamTags().begin();
   std::vector<TriggerInfo::StreamTag>::const_iterator STend = evt->trigger_info()->streamTags().end();
   while (STit != STend) {
     // check if stream is in list, if not add it, otherwise increment it
     if (m_streams.find(STit->name()) == m_streams.end()) {
       m_streams.insert(make_pair(STit->name(),1));
     }
     else {
       m_streams[STit->name()] += 1;
     }
     ++STit;
   }
   

   log << MSG::DEBUG << "EventInfo event: " << evt->event_ID()->event_number() 
	            << " run: " << evt->event_ID()->run_number() << endreq;
   // 
   // Now check what objects are in storegate
   //
   //if (m_dump) {
       const DataHandle<DataHeader> beg; 
       const DataHandle<DataHeader> ending; 
       StatusCode status = m_sGevent->retrieve(beg,ending);
       if (status.isFailure() || beg==ending) {
     	 log << MSG::DEBUG << "No DataHeaders present in StoreGate"
	     << endreq;
       }
       else {
         // Find input data header
         for (; beg != ending; ++beg) {
             if (beg->isInput()) {
	         DataHeader doh = *beg;
	         log << MSG::DEBUG << "DataHeader with key " 
	  	     << beg.key() << " is input" << endreq;

                 std::vector<DataHeaderElement>::const_iterator it = doh.begin();
                 std::vector<DataHeaderElement>::const_iterator last = doh.end();
                 std::vector<DataHeaderElement>::const_iterator it_pr = doh.beginProvenance();
                 std::vector<DataHeaderElement>::const_iterator last_pr = doh.endProvenance();

	         // Loop over DataHeaderElements
	         log << MSG::DEBUG << " About to loop over DH elements" << endreq;
                 int ccnt = 0;
	         for (; it!=last; ++it) {
	             std::set<CLID> list = it->getClassIDs();
	             std::set<CLID>::iterator c = list.begin();
	             // Loop over clids in dhe
                     int dcnt = 0;
	             while (c!=list.end()) {
  	                 log << MSG::DEBUG << ccnt << " " << dcnt << " Inserting CLID " << *c 
                                           << " " << it->getKey() << endreq;
                         // Look for clid in current list
		         std::map<int,int>::iterator cpos = m_clids.find(*c);
		         if (cpos==m_clids.end()) {
		             // if it doesn't exist, make it and set count to 1
		 	     m_clids.insert(std::make_pair(*c,1));
		         }
		         else {
		             // if it does exist, increment counter
			     cpos->second++;
		         }
                         // For this clid look for key in summary
		         std::map<int,ObjSum>::iterator spos = m_summaries.find(*c);
		         if (spos==m_summaries.end()) {
		             // if it doesn't exist, make it and set count to 1
			     ObjSum sum(1);
			     sum.keys.insert(it->getKey());
			     m_summaries.insert(std::make_pair(*c,sum));
		         }
		         else {
		             // if it does exist, increment counter
			     ObjSum sum = spos->second;
			     sum.num++;
			     sum.keys.insert(it->getKey());
			     spos->second=sum;
		         }
	                 ++c;
                         ++dcnt;
	             }
                     ++ccnt;
	         }
	         
                 // Now Loop over Provenance records
	         for (; it_pr!=last_pr; ++it_pr) {
                    // For this clid look for key in summary
		    //std::set<std::string>::iterator spos = m_prov_keys.find(it_pr->getKey());
		    //if (spos==m_prov_keys.end()) {
		        // if it doesn't exist, add key to list
/*
                        std::pair<std::set<std::string>::iterator,bool> ins_stat; 
		        ins_stat = m_prov_keys.insert(it_pr->getKey());
                        if (ins_stat.second) {
  	                    log << MSG::DEBUG << "Provenance key " 
                                              << it_pr->getKey() << endreq;
                        }
*/
                        // Add token to list, and associate it with the key
                        const Token* tk = it_pr->getToken();
                        std::pair<std::map<Guid,std::string>::iterator,bool> ins_stat2; 
                        ins_stat2 = m_guids.insert(std::make_pair(tk != 0 ? tk->dbID() : Guid::null(),it_pr->getKey()));
                        if (ins_stat2.second) {
  	                    log << MSG::DEBUG << "Provenance key " 
                                              << it_pr->getKey() << endreq;
  	                    log << MSG::DEBUG << "New provenance token found " 
                                              << (tk != 0 ? tk->toString() : "") << endreq;
                        }
/*
                        else {
                            log << MSG::INFO << "Provenance key " << it_pr->getKey() 
                                             << " appears multiple times" << endreq;
                        }
*/
    	            //}
	         }
 	     }  // input check if, else
	     else {
	         log << MSG::DEBUG << "Not input header" << endreq;
	     }
          }  // loop over data headers
       }  
   //}  // end of dump section
   
   return(StatusCode::SUCCESS);
}

StatusCode EventCount::finalize() 
{
   MsgStream log(msgSvc(), name());
   //
   // Present summary information regardless of dump setting
   //
   // Show beginning and ending event numbers
   //
   log << MSG::DEBUG << "in m_finalize()" << endreq;
   log << MSG::INFO << "---------- INPUT FILE SUMMARY ----------" << endreq;
   log << MSG::INFO << "Input contained: " << m_nev << " events" << endreq;
   log << MSG::INFO << " -- Event Range ( " << m_first
	            << " .. " <<  m_final << " )" << endreq;
   log << MSG::INFO << " -- Lumiblock Range ( " << m_firstlb
	            << " .. " <<  m_finallb << " )" << endreq;
   log << MSG::INFO << "Input contained: " << m_runs.size() << " runs" << endreq;
   log << MSG::INFO << " --";
   // List all the runs
   std::vector<int>::iterator it = m_runs.begin();
   while (it!=m_runs.end()) {
      log << " " << *it; ++it;
   }
   log << endreq;
   // List all the event types
   log << MSG::INFO << "Input contained the following Event Types" << endreq;
   std::set<std::string>::iterator itype=m_types.begin();
   while (itype != m_types.end()) {
      log << MSG::INFO << " -- " << *itype << endreq; 
      ++itype;
   }
   // List all the event streams
   log << MSG::INFO << "Input contained the following Streams" << endreq;
   std::map<std::string,int>::iterator istr=m_streams.begin();
   while (istr != m_streams.end()) {
      log << MSG::INFO << " -- " << istr->first << " (" << istr->second <<")" << endreq; 
      ++istr;
   }
/*
   log << MSG::INFO << "Input contained the following Provenance Tags" << endreq;
   std::set<std::string>::iterator ipr = m_prov_keys.begin();
   while (ipr!=m_prov_keys.end()) {
      log << MSG::INFO << " -- " << *ipr << endreq;
      ++ipr;
   }
*/
   log << MSG::INFO << "Input contained references to the following File GUID's" << endreq;
   std::string fkey = "";
   std::map<Guid,std::string>::iterator ifr = m_guids.begin();
   while (ifr!=m_guids.end()) {
      if (ifr->second != fkey) {
          fkey = ifr->second;
          log << MSG::INFO << " -> " << fkey << endreq;
      }
      log << MSG::INFO << "      - " << ifr->first << endreq;
      ++ifr;
   }
   //
   // If dump flag set, then show full contents
   //
   if (m_dump) {
      log << MSG::INFO << "Input contained the following CLIDs and Keys" << endreq;
      // Get pointer to classid service
      bool clidTran = false;
      if (m_pCLIDSvc.retrieve().isSuccess()) {
	  clidTran = true;
      }
      else {
          log << MSG::WARNING 
	      << "Could not locate ClassIDSvc" 
	      << endreq;
      }
      //log << MSG::INFO << "--------- D A T A - O B J ---------" << endreq;
      std::map<int,int>::iterator i = m_clids.begin();
      std::string classname = "NONE";
      StatusCode clidsvc_sc;
      while (i!=m_clids.end()) {
          classname = "Unknown";
          if(clidTran) clidsvc_sc = m_pCLIDSvc->getTypeNameOfID(i->first,classname);
          if (!clidsvc_sc.isSuccess())
              log << MSG::DEBUG << "CLIDSvc bad return code" << endmsg;
          log << MSG::INFO << " -> " << i->first << " " 
                           << classname
		           << " (" << i->second << ") " 
		           << endreq;
	  // Now check for key list
	  std::map<int,ObjSum>::iterator keyloc = m_summaries.find(i->first);
	  if (keyloc != m_summaries.end()) {
	      ObjSum out = keyloc->second;
	      std::set<std::string>::iterator ot = out.keys.begin();
	      while(ot != out.keys.end()) {
		  log << MSG::INFO << "      - "
		                   << *ot << endreq;
		  ++ot;
	      }
	  } 
          ++i;
      }
      log << MSG::INFO << "----------------------------------------" << endreq;
   }
   return(StatusCode::SUCCESS);
}
