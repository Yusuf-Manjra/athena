/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ALGA_H
#define ATHEXHIVE_ALGA_H 1

#include "HiveAlgBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include "EventInfo/EventInfo.h"

#include <string>

class HiveAlgA  :  public HiveAlgBase {
  
public:
  
  // Standard Algorithm Constructor:
  
  HiveAlgA (const std::string& name, ISvcLocator* pSvcLocator);
  ~HiveAlgA ();
  
  // Define the initialize, execute and finalize methods:
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  SG::ReadHandleKey<EventInfo> m_evt;

  SG::WriteHandleKey<HiveDataObj> m_wrh1;
  SG::WriteHandleKey<HiveDataObj> m_wrh2;

  std::atomic<int> m_i;
  
};
#endif
