/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_CHAINCNV_P1_H
#define TRIGCONF_CHAINCNV_P1_H

// Framework
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Transient data
#include "TrigMonitoringEvent/TrigConfChain.h"

// Persistent data and converters
#include "TrigMonitoringEventTPCnv/TrigConfChain_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfSigCnv_p1.h"

class MsgStream;

class TrigConfChainCnv_p1 : public T_AthenaPoolTPCnvConstBase<TrigConfChain, TrigConfChain_p1>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigConfChainCnv_p1() {}
  virtual ~TrigConfChainCnv_p1() {}

  virtual
  void persToTrans(const TrigConfChain_p1* persObj,
		   TrigConfChain* transObj,
		   MsgStream &log) const override;

  virtual
  void transToPers(const TrigConfChain* transObj,
		   TrigConfChain_p1* persObj,
		   MsgStream &log) const override;
  
 private:

  T_AthenaPoolTPCnvStdVectorConst<std::vector<TrigConfSig>, 
                                  std::vector<TrigConfSig_p1>, TrigConfSigCnv_p1> m_sigCnv;
};

#endif

