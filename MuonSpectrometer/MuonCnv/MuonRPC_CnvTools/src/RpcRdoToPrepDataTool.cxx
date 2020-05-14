/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "RpcRdoToPrepDataTool.h"

#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"


Muon::RpcRdoToPrepDataTool::RpcRdoToPrepDataTool( const std::string& type, const std::string& name,
						  const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    RpcRdoToPrepDataToolCore( type, name, parent )
{
}

Muon::RpcRdoToPrepDataTool::~RpcRdoToPrepDataTool()
{
}

StatusCode Muon::RpcRdoToPrepDataTool::initialize() 
{
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( RpcRdoToPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataTool::finalize()
{   
  return RpcRdoToPrepDataToolCore::finalize();
}

StatusCode Muon::RpcRdoToPrepDataTool::manageOutputContainers(bool& firstTimeInTheEvent)
{
  SG::WriteHandle< Muon::RpcPrepDataContainer > rpcPrepDataHandle(m_rpcPrepDataContainerKey);
  if(!rpcPrepDataHandle.isPresent()) {
    firstTimeInTheEvent = true;

    StatusCode status = rpcPrepDataHandle.record(std::make_unique<Muon::RpcPrepDataContainer>(m_idHelperSvc->rpcIdHelper().module_hash_max()));

    if (status.isFailure() || !rpcPrepDataHandle.isValid() ) 	{
      ATH_MSG_FATAL("Could not record container of RPC PrepData Container at " << m_rpcPrepDataContainerKey.key());
      return status;
    } else {
      m_rpcPrepDataContainer = rpcPrepDataHandle.ptr();
      ATH_MSG_DEBUG("RPC PrepData Container recorded in StoreGate with key " << m_rpcPrepDataContainerKey.key() << ", " << rpcPrepDataHandle.key());
    }

    if (m_producePRDfromTriggerWords){
      /// create an empty RPC trigger hit container for filling
      SG::WriteHandle< Muon::RpcCoinDataContainer > rpcCoinDataHandle(m_rpcCoinDataContainerKey);
      status = rpcCoinDataHandle.record(std::make_unique<Muon::RpcCoinDataContainer>(m_idHelperSvc->rpcIdHelper().module_hash_max()));

      if (status.isFailure() || !rpcCoinDataHandle.isValid() ) 	{
        ATH_MSG_FATAL("Could not record container of RPC TrigCoinData Container at " << m_rpcCoinDataContainerKey.key());
        return status;
      } else {
        m_rpcCoinDataContainer = rpcCoinDataHandle.ptr();
	ATH_MSG_DEBUG("RPC TrigCoinData Container recorded in StoreGate with key " << m_rpcCoinDataContainerKey.key());
      }      
      ATH_MSG_VERBOSE(" RpcCoinDataContainer created");
    } 
    m_decodedOfflineHashIds.clear();
    m_decodedRobIds.clear();

  }
  else{
    const Muon::RpcPrepDataContainer* rpcPrepDataContainer_c;
    ATH_CHECK( evtStore()->retrieve (rpcPrepDataContainer_c, m_rpcPrepDataContainerKey.key()) );
    m_rpcPrepDataContainer = const_cast<Muon::RpcPrepDataContainer*> (rpcPrepDataContainer_c);
    ATH_MSG_DEBUG("RPC PrepData Container is already in StoreGate ");
    if (m_producePRDfromTriggerWords){
      SG::WriteHandle< Muon::RpcCoinDataContainer > rpcCoinDataHandle(m_rpcCoinDataContainerKey);
      if (!rpcCoinDataHandle.isPresent()) {
        ATH_MSG_FATAL("Muon::RpcPrepDataContainer found while Muon::RpcCoinDataContainer not found in Event Store");
        return StatusCode::FAILURE;
      }
      const Muon::RpcCoinDataContainer* rpcCoinDataContainer_c;
      ATH_CHECK( evtStore()->retrieve (rpcCoinDataContainer_c, m_rpcCoinDataContainerKey.key()) );
      m_rpcCoinDataContainer = const_cast<Muon::RpcCoinDataContainer*> (rpcCoinDataContainer_c);
      ATH_MSG_DEBUG("RPC CoinData Container is already in StoreGate ");
    }
  }
  return StatusCode::SUCCESS;
}
