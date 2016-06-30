/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_CONDALGX_H
#define CONDALGS_CONDALGX_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "AthExHive/CondDataObj.h"

#include "EventInfo/EventInfo.h"
#include "GaudiKernel/ICondSvc.h"

#include <string>

class CondAlgX  :  public AthAlgorithm {
  
public:
    
  CondAlgX (const std::string& name, ISvcLocator* pSvcLocator);
  ~CondAlgX();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandle<EventInfo> m_evt;

  SG::WriteCondHandleKey<CondDataObj> m_wchk;

  ServiceHandle<ICondSvc> m_cs;

  std::string m_dbKey;

};

#endif
