/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPCDATAPREPARATOR_H
#define  TRIGL2MUONSA_RPCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "MuonRDO/RpcPadContainer.h"

#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/RpcPatFinder.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"
#include "RegionSelector/IRegSelSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "MuonRPC_Cabling/MuonRPC_CablingSvc.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"

#include "MuonRecToolInterfaces/IMuonCombinePatternTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

class StoreGateSvc;
class ActiveStoreSvc;
class RpcIdHelper;

namespace HLT {
  class TriggerElement;
  class Algo;
}

namespace MuonGM {
  class MuonDetectorManager;
  class RpcReadoutElement;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {

class RpcDataPreparator: public AthAlgTool
{
   public:
      
      static const InterfaceID& interfaceID();

   public:

      RpcDataPreparator(const std::string& type, 
			const std::string& name,
			const IInterface*  parent);
    
      ~RpcDataPreparator();
    
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();
    
      StatusCode prepareData(const TrigRoiDescriptor*    p_roids,
			     unsigned int roiWord,
			     TrigL2MuonSA::RpcHits&      rpcHits,
			     TrigL2MuonSA::RpcPatFinder* rpcPatFinder);

      inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }
      inline void setMsgLvl(const MSG::Level& level) { if(m_msg != 0) m_msg->setLevel(level); }
      // void setOptions(const TrigL2MuonSA::RpcDataPreparatorOptions& options) { m_options = options; };

      void setRoIBasedDataAccess(bool use_RoIBasedDataAccess);
      
 private:
      
      MsgStream* m_msg;
      inline MsgStream& msg() const { return *m_msg; }
      
 private:
		       
      // Reference to StoreGateSvc;
      ServiceHandle<StoreGateSvc>    m_storeGateSvc;
      ActiveStoreSvc* m_activeStore;
      
      // Region Selector
      IRegSelSvc*          m_regionSelector;
      // ROB DataProvider
      ROBDataProviderSvc*  m_robDataProvider;

      // RPC cabling service
      const IRPCcablingSvc* m_rpcCabling;
      const CablingRPCBase* m_rpcCablingSvc;
      
      const MuonGM::MuonDetectorManager* m_muonMgr;
      const MuonGM::RpcReadoutElement* m_rpcReadout;
      // Muon Id Helpers
      const RpcIdHelper* m_rpcIdHelper;

      // handles to the RoI driven data access
      ToolHandle<Muon::IMuonRdoToPrepDataTool> m_rpcPrepDataProvider;

      ToolHandle <Muon::MuonIdHelperTool>  m_idHelperTool;  //!< Pointer to concrete tool

      bool m_use_RoIBasedDataAccess;
};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_RPCDATAPREPARATOR_H
