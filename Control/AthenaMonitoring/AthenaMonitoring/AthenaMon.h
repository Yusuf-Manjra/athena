/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMON_H
#define ATHENAMON_H

#include "GaudiKernel/Algorithm.h"

#include "AthenaMonitoring/IMonitorToolBase.h"

class AthenaMon:public Algorithm {
 public:

  AthenaMon (const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~AthenaMon();
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
  virtual StatusCode beginRun();
  virtual StatusCode endRun();
  
 protected:
  
	std::vector<IMonitorToolBase*> m_monTools;
  
 private:
  
  std::vector<std::string>  m_monToolNames;
  
  std::vector<std::string>  m_THistSvc_streamnameMapping;
  
  long m_everyNevents;
  long m_eventCounter;
  
  bool m_bookHistsinInitialize;
  
};

#endif
