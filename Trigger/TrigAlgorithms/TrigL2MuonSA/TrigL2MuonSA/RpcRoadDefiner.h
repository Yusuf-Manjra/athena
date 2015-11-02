/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPCROADDEFINER_H
#define  TRIGL2MUONSA_RPCROADDEFINER_H

#include <string>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigMuonBackExtrapolator/ITrigMuonBackExtrapolator.h"

#include "MuonIdHelpers/MdtIdHelper.h"

#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/RpcPatFinder.h"
#include "TrigL2MuonSA/MuonRoad.h"
#include "TrigL2MuonSA/MdtRegion.h"
#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/BarrelRoadData.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

#include "RegionSelector/IRegSelSvc.h"

class StoreGateSvc;

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class RpcRoadDefiner
{
 public:
  RpcRoadDefiner(MsgStream* msg);
  ~RpcRoadDefiner(void);
  
 public:
  inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }

  StatusCode defineRoad(const LVL1::RecMuonRoI*      p_roi,
			TrigL2MuonSA::MuonRoad&      muonRoad,
			TrigL2MuonSA::RpcHits&       rpcHits,
			TrigL2MuonSA::RpcPatFinder*  rpcPatFinder,
			TrigL2MuonSA::RpcFitResult&  rpcFitResult,
			double                       roiEtaMinLow,
			double                       roiEtaMaxLow,
			double                       roiEtaMinHigh,
			double                       roiEtaMaxHigh);

  void setMsgStream(MsgStream* msg) { m_msg = msg; };
  void setMdtGeometry(IRegSelSvc* regionSelector, const MdtIdHelper* mdtIdHelper);
  void setRoadWidthForFailure(double rWidth_RPC_Failed);
  void setRpcGeometry(bool use_rpc);

 private:
  inline MsgStream& msg() const { return *m_msg; }
  
 protected:
  float f(float x, float c0, float c1, float c2, float c3) const;
  float fp(float x, float c33, float c22, float c1) const;

 private:
  MsgStream* m_msg;
  const BarrelRoadData*  m_roadData;

  double m_rWidth_RPC_Failed;
  bool m_use_rpc;

  IRegSelSvc* m_regionSelector;
  const MdtIdHelper* m_mdtIdHelper;
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
 
 inline float TrigL2MuonSA::RpcRoadDefiner::f(float x, float c0, float c1, float c2, float c3) const
   {
     return c0 + x * (c1 + x * (c2 + x * c3)); // faster
   }
 
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
 inline float TrigL2MuonSA::RpcRoadDefiner::fp(float x, float c33, float c22, float c1) const
   {
     return c1 + x * (c22 + x * c33); // faster
   }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
}

#endif // TRIGL2MUONSA_RPCROADDEFINER_H
