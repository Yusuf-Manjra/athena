/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SharedEvtQueueConsumer.h"
#include "copy_file_icc_hack.h"
#include "AthenaInterprocess/ProcessGroup.h"

#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IEventShare.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IFileMgr.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdexcept>

SharedEvtQueueConsumer::SharedEvtQueueConsumer(const std::string& type
					       , const std::string& name
					       , const IInterface* parent)
  : AthenaMPToolBase(type,name,parent)
  , m_useSharedReader(false)
  , m_isPileup(false)
  , m_rankId(-1)
  , m_nEventsBeforeFork(0)
  , m_chronoStatSvc("ChronoStatSvc", name)
  , m_evtSeek(0)
  , m_evtShare(0)
  , m_shmemSegment()
  , m_sharedEventQueue(0)
  , m_sharedRankQueue(0)
{
  declareInterface<IAthenaMPTool>(this);

  declareProperty("UseSharedReader",m_useSharedReader);
  declareProperty("IsPileup",m_isPileup);
  declareProperty("EventsBeforeFork",m_nEventsBeforeFork);

  m_subprocDirPrefix = "worker_";
}

SharedEvtQueueConsumer::~SharedEvtQueueConsumer()
{
}

StatusCode SharedEvtQueueConsumer::initialize()
{
  msg(MSG::DEBUG) << "In initialize" << endreq;
  if(m_isPileup) {
    m_evtProcessor = ServiceHandle<IEventProcessor>("PileUpEventLoopMgr",name());
    msg(MSG::INFO) << "The job running in pileup mode" << endreq;
  }
  else {
    msg(MSG::INFO) << "The job running in non-pileup mode" << endreq;
  }

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess())
    return sc;

  // For pile-up jobs use event loop manager for seeking
  // otherwise use event selector
  if(m_isPileup) {
    m_evtSeek = dynamic_cast<IEventSeek*>(m_evtProcessor.operator->());
    if(!m_evtSeek) {
      msg(MSG::ERROR) << "Unable to dyn-cast PileUpEventLoopMgr to IEventSeek" << endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
    sc = serviceLocator()->service(m_evtSelName,m_evtSeek);
    if(sc.isFailure() || m_evtSeek==0) {
      msg(MSG::ERROR) << "Error retrieving IEventSeek" << endreq;
      return StatusCode::FAILURE;
    }
  }

  if(m_useSharedReader) {
    sc = serviceLocator()->service(m_evtSelName,m_evtShare);
    if(sc.isFailure() || m_evtShare==0) {
      msg(MSG::ERROR) << "Error retrieving IEventShare" << endreq;
      return StatusCode::FAILURE;
    }
  }

  sc = m_chronoStatSvc.retrieve();
  if (!sc.isSuccess()) {
    msg(MSG::ERROR) << "Cannot get ChronoStatSvc." << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode SharedEvtQueueConsumer::finalize()
{
  delete m_sharedRankQueue;
  return StatusCode::SUCCESS;
}

int SharedEvtQueueConsumer::makePool(int, int nprocs, const std::string& topdir)
{
  msg(MSG::DEBUG) << "In makePool " << getpid() << endreq;

  if(nprocs==0 || nprocs<-1) {
    msg(MSG::ERROR) << "Invalid value for the nprocs parameter: " << nprocs << endreq;
    return -1;
  }

  if(topdir.empty()) {
    msg(MSG::ERROR) << "Empty name for the top directory!" << endreq;
    return -1;
  }

  m_nprocs = (nprocs==-1?sysconf(_SC_NPROCESSORS_ONLN):nprocs);
  m_subprocTopDir = topdir;

  // Get the shared event queue
  msg(MSG::DEBUG) << "Event queue name " << "AthenaMPEventQueue_" << m_randStr << endreq;
  StatusCode sc = detStore()->retrieve(m_sharedEventQueue,"AthenaMPEventQueue_"+m_randStr);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve the pointer to Shared Event Queue" << endreq;
    return -1;
  }


  // Create rank queue and fill it
  m_sharedRankQueue = new AthenaInterprocess::SharedQueue("SharedEvtQueueConsumer_RankQueue_"+m_randStr,m_nprocs,sizeof(int));
  for(int i=0; i<m_nprocs; ++i)
    if(!m_sharedRankQueue->send_basic<int>(i)) {
      msg(MSG::ERROR) << "Unable to send int to the ranks queue!" << endreq;
      return -1;
    }

  // Create the process group and map_async bootstrap
  m_processGroup = new AthenaInterprocess::ProcessGroup(m_nprocs);
  msg(MSG::INFO) << "Created Pool of " << m_nprocs << " worker processes" << endreq;
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;
  msg(MSG::INFO) << "Workers bootstraped" << endreq; 

  return m_nprocs;
}

StatusCode SharedEvtQueueConsumer::exec()
{
  msg(MSG::DEBUG) << "In exec " << getpid() << endreq;

  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC))
    return StatusCode::FAILURE;
  msg(MSG::INFO) << "Workers started processing events" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode SharedEvtQueueConsumer::wait_once(int& numFinishedProc)
{
  StatusCode sc = AthenaMPToolBase::wait_once(numFinishedProc);
  AthenaInterprocess::ProcessResult* presult(0);
  if(sc.isFailure()) {
    // We are to stop waiting. Pull all available ProcessResults from the queue
    // Don't serialize finalizations
    do {
      presult = m_processGroup->pullOneResult();
      if(presult && (unsigned)(presult->output.size)>sizeof(int)) 
	decodeProcessResult(presult,false);
      if(presult) free(presult->output.data);
      delete presult;
    } while(presult);
  }
  else {
    // Pull one result and decode it if necessary
    presult = m_processGroup->pullOneResult();
    int res(0);
    if(presult && (unsigned)(presult->output.size)>sizeof(int)) 
      res = decodeProcessResult(presult,true);
    if(presult) free(presult->output.data);
    delete presult;
    if(res) return StatusCode::FAILURE;
  }
  return sc;
}

void SharedEvtQueueConsumer::reportSubprocessStatuses()
{
  msg(MSG::INFO) << "Statuses of event processors" << endreq;
  const std::vector<AthenaInterprocess::ProcessStatus>& statuses = m_processGroup->getStatuses();
  for(size_t i=0; i<statuses.size(); ++i) {
    // Get the number of events processed by this worker
    std::map<pid_t,int>::const_iterator it = m_nProcessedEvents.find(statuses[i].pid);
    msg(MSG::INFO) << "*** Process PID=" << statuses[i].pid 
		   << ". Status " << ((statuses[i].exitcode)?"FAILURE":"SUCCESS") 
		   << ". Number of events processed: ";
    if(it==m_nProcessedEvents.end())
      msg(MSG::INFO) << "N/A" << endreq;
    else
      msg(MSG::INFO) << it->second << endreq;
  }
}

void SharedEvtQueueConsumer::subProcessLogs(std::vector<std::string>& filenames)
{
  filenames.clear();
  for(int i=0; i<m_nprocs; ++i) {
    std::ostringstream workerIndex;
    workerIndex << i;
    boost::filesystem::path worker_rundir(m_subprocTopDir);
    worker_rundir /= boost::filesystem::path(m_subprocDirPrefix+workerIndex.str());
    filenames.push_back(worker_rundir.string()+std::string("/AthenaMP.log"));
  }
}

AthenaInterprocess::ScheduledWork* SharedEvtQueueConsumer::bootstrap_func()
{
  int* errcode = new int(1); // For now use 0 success, 1 failure
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = (void*)errcode;
  outwork->size = sizeof(int);
  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...

  // ________________________ Get RankID ________________________
  //
  if(!m_sharedRankQueue->receive_basic<int>(m_rankId)) {
    msg(MSG::ERROR) << "Unable to get rank ID!" << endreq;
    return outwork;
  }
  std::ostringstream workindex;
  workindex<<m_rankId;

  // ________________________ Worker dir: mkdir ________________________
  boost::filesystem::path worker_rundir(m_subprocTopDir);
  worker_rundir /= boost::filesystem::path(m_subprocDirPrefix+workindex.str());
  // TODO: this "worker_" can be made configurable too

  if(mkdir(worker_rundir.string().c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)==-1) {
    msg(MSG::ERROR) << "Unable to make worker run directory: " << worker_rundir.string() << ". " << strerror(errno) << endreq;
    return outwork;
  }

  // ________________________ Redirect logs ________________________
  if(redirectLog(worker_rundir.string()))
    return outwork;

  msg(MSG::INFO) << "Logs redirected in the AthenaMP event worker PID=" << getpid() << endreq;

  // ________________________ Update Io Registry ____________________________
  if(updateIoReg(worker_rundir.string()))
    return outwork;

  msg(MSG::INFO) << "Io registry updated in the AthenaMP event worker PID=" << getpid() << endreq;

  // ________________________ SimParams & DigiParams ____________________________
  boost::filesystem::path abs_worker_rundir = boost::filesystem::absolute(worker_rundir);
  if(boost::filesystem::is_regular_file("SimParams.db"))
    COPY_FILE_HACK("SimParams.db", abs_worker_rundir.string()+"/SimParams.db");
  if(boost::filesystem::is_regular_file("DigitParams.db"))
    COPY_FILE_HACK("DigitParams.db", abs_worker_rundir.string()+"/DigitParams.db");

  // _______________________ Handle saved PFC (if any) ______________________
  if(handleSavedPfc(abs_worker_rundir))
    return outwork;

  // ________________________  reopen descriptors ____________________________
  if(reopenFds())
    return outwork;

  msg(MSG::INFO) << "File descriptors re-opened in the AthenaMP event worker PID=" << getpid() << endreq;

  
  // ________________________ Make Shared RAW Reader Client ________________________
  if(m_useSharedReader) {
    if(!m_evtShare->makeClient(m_rankId).isSuccess()) {
      msg(MSG::ERROR) << "Failed to make the event selector a share client" << endreq;
      return outwork;
    }
    else {
      msg(MSG::DEBUG) << "Successfully made the event selector a share client" << endreq;
    }
  }

  // ________________________ I/O reinit ________________________
  if(!m_ioMgr->io_reinitialize().isSuccess()) {
    msg(MSG::ERROR) << "Failed to reinitialize I/O" << endreq;
    return outwork;
  } else {
    msg(MSG::DEBUG) << "Successfully reinitialized I/O" << endreq;
  }

  // ________________________ Event selector restart ________________________
  IService* evtSelSvc = dynamic_cast<IService*>(m_evtSelector);
  if(!evtSelSvc) {
    msg(MSG::ERROR) << "Failed to dyncast event selector to IService" << endreq;
    return outwork;
  }
  if(!evtSelSvc->start().isSuccess()) {
    msg(MSG::ERROR) << "Failed to restart the event selector" << endreq;
    return outwork;
  } else {
    msg(MSG::DEBUG) << "Successfully restarted the event selector" << endreq;
  }

  // ________________________ Restart background event selectors in pileup jobs ________________________
  if(m_isPileup) {
    const std::list<IService*>& service_list = serviceLocator()->getServices();
    std::list<IService*>::const_iterator itSvc = service_list.begin(),
      itSvcLast = service_list.end();
    for(;itSvc!=itSvcLast;++itSvc) {
      IEvtSelector* evtsel = dynamic_cast<IEvtSelector*>(*itSvc);
      if(evtsel && (evtsel != m_evtSelector)) {
	if((*itSvc)->start().isSuccess())
	  msg(MSG::DEBUG) << "Restarted event selector " << (*itSvc)->name() << endreq;
	else {
	  msg(MSG::ERROR) << "Failed to restart event selector " << (*itSvc)->name() << endreq;
	  return outwork;
	}
      }
    }
  }

  // ________________________ Worker dir: chdir ________________________
  if(chdir(worker_rundir.string().c_str())==-1) {
    msg(MSG::ERROR) << "Failed to chdir to " << worker_rundir.string() << endreq;
    return outwork;
  }

  // ________________________ Instantiate shared memory segment ________________________
  try {
    m_shmemSegment = boost::shared_ptr<yampl::SharedMemory>(new yampl::SharedMemory("/athmp-shmem-"+m_randStr,2*sizeof(int)));
  }
  catch(yampl::ErrnoException& ex) {
    msg(MSG::ERROR) << "yampl::ErrnoException caught: " << ex.what() << endreq;
    msg(MSG::ERROR) << "Unable to get shared memory" << endreq;
    return outwork;
  }

  // Declare success and return
  *errcode = 0;
  return outwork;
}

AthenaInterprocess::ScheduledWork* SharedEvtQueueConsumer::exec_func()
{
  msg(MSG::INFO) << "Exec function in the AthenaMP worker PID=" << getpid() << endreq;

  bool all_ok(true);

  // ________________________ This is needed only for PileUp jobs __________________________________
  // **
  // If either EventsBeforeFork or SkipEvents is nonzero, first we need to advance the event selector
  // by EventsBeforeFork+SkipEvents and only after that start seeking on the PileUpEventLoopMgr
  // **
  if(m_isPileup) {
    IEventSeek* evtSeek(0);
    StatusCode sc = serviceLocator()->service(m_evtSelName,evtSeek);
    if(sc.isFailure() || evtSeek==0) {
      msg(MSG::ERROR) << "Error retrieving Event Selector with IEventSeek interface for PileUp job" << endreq;
      all_ok = false;
    }
    else {
      IProperty* propertyServer = dynamic_cast<IProperty*>(m_evtSelector);
      if(propertyServer==0) {
	msg(MSG::ERROR) << "Unable to cast event selector to IProperty" << endreq;
	all_ok = false;
      }
      else  {
	std::string propertyName("SkipEvents");
	int skipEvents(0);
	IntegerProperty skipEventsProp(propertyName,skipEvents);
	sc = propertyServer->getProperty(&skipEventsProp);
	if(sc.isFailure()) {
	  msg(MSG::INFO) << "Event Selector does not have SkipEvents property" << endreq;
	}
	else {
	  skipEvents = skipEventsProp.value();
	}
	
	if((m_nEventsBeforeFork+skipEvents)
	   && evtSeek->seek(m_nEventsBeforeFork+skipEvents).isFailure()) {
	  msg(MSG::ERROR) << "Unable to seek to " << m_nEventsBeforeFork+skipEvents << endreq;    
	  all_ok = false;
	}
      }
    }
  }
  // ________________________ This is needed only for PileUp jobs __________________________________


  int nEvt(1+m_nEventsBeforeFork);
  int nEventsProcessed(0);
  int evtnum(0);
  int* shmemCountedEvts = (int*)m_shmemSegment->getMemory();
  int* shmemCountFinal = shmemCountedEvts+1;
  msg(MSG::DEBUG) << "Counted events " << *shmemCountedEvts << " and the count is final " << *shmemCountFinal << endreq;

  m_chronoStatSvc->chronoStart("AthenaMP_getEvent");
  if(all_ok) {
    while(true) {
      if(!m_sharedEventQueue->try_receive_basic<int>(evtnum)) {
	// The event queue is empty, but we should check whether there are more events to come or not
	msg(MSG::DEBUG) << "Event queue is empty"; 
	if(*shmemCountFinal) {
	  msg(MSG::DEBUG) << " and no more events are expected" << endreq;
	  break;
	}
	else {
	  msg(MSG::DEBUG) << " but more events are expected" << endreq;
	  usleep(1);
	  continue;
	}
      }
      
      m_chronoStatSvc->chronoStop("AthenaMP_getEvent");
      msg(MSG::INFO) << "Received event num " << evtnum << endreq;
      StatusCode sc;
      if(m_useSharedReader) {
	sc = m_evtShare->share(evtnum);
	if(sc.isFailure()){
	  msg(MSG::ERROR) << "Unable to share " << evtnum << endreq;
	  all_ok=false;
	  break;
	}
	else {
	  msg(MSG::INFO) << "Share of " << evtnum << " succeeded" << endreq;
	}
      }
      else {
	m_chronoStatSvc->chronoStart("AthenaMP_seek");
	sc=m_evtSeek->seek(evtnum);
	if(sc.isFailure()){
	  msg(MSG::ERROR) << "Unable to seek to " << evtnum << endreq;
	  all_ok=false;
	  break;
	}
	else {
	  msg(MSG::INFO) << "Seek to " << evtnum << " succeeded" << endreq;
	}
	m_chronoStatSvc->chronoStop("AthenaMP_seek");
      }
      m_chronoStatSvc->chronoStart("AthenaMP_nextEvent");
      sc = m_evtProcessor->nextEvent(nEvt++);
      nEventsProcessed++;
      if(sc.isFailure()){
	msg(MSG::ERROR) << "Unable to process " << evtnum << endreq;
	all_ok=false;
	break;
      }
      m_chronoStatSvc->chronoStop("AthenaMP_nextEvent"); 
      m_chronoStatSvc->chronoStart("AthenaMP_getEvent");
    }
  }
  m_chronoStatSvc->chronoStop("AthenaMP_getEvent");

  if(all_ok) {
    if(m_evtProcessor->executeRun(0).isFailure()) {
      msg(MSG::ERROR) << "Could not finalize the Run" << endreq;
      all_ok=false;
    }
    else {
      msg(MSG::DEBUG) << *shmemCountedEvts << " is the max event counted" << endreq; 
      if(m_evtSeek->seek(*shmemCountedEvts).isFailure()) 
	msg(MSG::DEBUG) << "Seek past maxevt=" << *shmemCountedEvts << " returned failure. As expected..." << endreq;
    }
  }

  int errcode = (all_ok?0:1); // For now use 0 success, 1 failure
  AthenaMPToolBase::Func_Flag func = AthenaMPToolBase::FUNC_EXEC;
  // Return value: "ERRCODE|Func_Flag|NEvt"
  int outsize = 2*sizeof(int)+sizeof(AthenaMPToolBase::Func_Flag);
  void* outdata = malloc(outsize);
  memcpy(outdata,&errcode,sizeof(int));
  memcpy((char*)outdata+sizeof(int),&func,sizeof(func));
  memcpy((char*)outdata+sizeof(int)+sizeof(func),&nEventsProcessed,sizeof(int));
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = outdata;
  outwork->size = outsize;
  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...
  return outwork;
}

AthenaInterprocess::ScheduledWork* SharedEvtQueueConsumer::fin_func()
{
  msg(MSG::INFO) << "Fin function in the AthenaMP worker PID=" << getpid() << endreq;

  bool all_ok(true);

  if(m_appMgr->stop().isFailure()) {
    msg(MSG::ERROR) << "Unable to stop AppMgr" << endreq; 
    all_ok=false;
  }
  else { 
    if(m_appMgr->finalize().isFailure()) {
      msg(MSG::ERROR) << "Unable to finalize AppMgr" << endreq;
      all_ok=false;
    }
  }

  int errcode = (all_ok?0:1); // For now use 0 success, 1 failure
  int nEvt = -1;
  AthenaMPToolBase::Func_Flag func = AthenaMPToolBase::FUNC_FIN;
  // Return value: "ERRCODE|Func_Flag|NEvt"  (Here NEvt=-1)
  int outsize = 2*sizeof(int)+sizeof(AthenaMPToolBase::Func_Flag);
  void* outdata = malloc(outsize);
  memcpy(outdata,&errcode,sizeof(int));
  memcpy((char*)outdata+sizeof(int),&func,sizeof(func));
  memcpy((char*)outdata+sizeof(int)+sizeof(func),&nEvt,sizeof(int));
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = outdata;
  outwork->size = outsize;

  return outwork;
}

int SharedEvtQueueConsumer::decodeProcessResult(const AthenaInterprocess::ProcessResult* presult, bool doFinalize)
{
  if(!presult) return 0;
  const AthenaInterprocess::ScheduledWork& output = presult->output;
  msg(MSG::DEBUG) << "Decoding the output of PID=" << presult->pid << " with the size=" << output.size << endreq;
  if(output.size!=2*sizeof(int)+sizeof(AthenaMPToolBase::Func_Flag)) return 0;
  
  AthenaMPToolBase::Func_Flag func;
  memcpy(&func,(char*)output.data+sizeof(int),sizeof(func));
  if(func==AthenaMPToolBase::FUNC_EXEC) {
    // Store the number of processed events
    int nevt(0);
    memcpy(&nevt,(char*)output.data+sizeof(int)+sizeof(func),sizeof(int));
    m_nProcessedEvents[presult->pid]=nevt;
    msg(MSG::DEBUG) << "PID=" << presult->pid << " processed " << nevt << " events" << endreq;

    if(doFinalize) {
      // Add PID to the finalization queue
      m_finQueue.push(presult->pid);
      msg(MSG::DEBUG) << "Added PID=" << presult->pid << " to the finalization queue" << endreq;

      // If this is the only element in the queue then start its finalization
      // Otherwise it has to wait its turn until all previous processes have been finalized
      if(m_finQueue.size()==1) {
	if(mapAsyncFlag(AthenaMPToolBase::FUNC_FIN,presult->pid)
	   || m_processGroup->map_async(0,0,presult->pid)) {
	  msg(MSG::ERROR) << "Problem scheduling finalization on PID=" << presult->pid << endreq;
	  return 1;
	}
	else {
	  msg(MSG::DEBUG) << "Scheduled finalization of PID=" << presult->pid << endreq;
	}
      }
    }
  }
  else if(doFinalize && func==AthenaMPToolBase::FUNC_FIN) {
    msg(MSG::DEBUG) << "Finished finalization of PID=" << presult->pid << endreq;
    pid_t pid = m_finQueue.front();
    if(pid==presult->pid) {
      // pid received as expected. Remove it from the queue
      m_finQueue.pop();
      msg(MSG::DEBUG) << "PID=" << presult->pid << " removed from the queue" << endreq;
      // Schedule finalization of the next processe in the queue
      if(m_finQueue.size()) {
	if(mapAsyncFlag(AthenaMPToolBase::FUNC_FIN,m_finQueue.front())
           || m_processGroup->map_async(0,0,m_finQueue.front())) {
	  msg(MSG::ERROR) << "Problem scheduling finalization on PID=" << m_finQueue.front() << endreq;
          return 1;
	}
	else  {
	  msg(MSG::DEBUG) << "Scheduled finalization of PID=" << m_finQueue.front() << endreq;
	}
      }
    }
    else {
      // Error: unexpected pid received from presult
      msg(MSG::ERROR) << "Finalized PID=" << presult->pid << " while PID=" << pid << " was expected" << endreq;
      return 1;
    }
  }

  return 0;
}
