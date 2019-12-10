/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of RpcROD_Decoder class 

#include "RpcROD_Decoder.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include <algorithm> 

#include <deque>
#include <vector>
#include <utility>
#include <algorithm>
//Gaudi
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ListItem.h"

//niko
//#include "RpcByteStreamAccess/IRPC_ByteStreamErrorSvc.h"


//Athena
#include "eformat/Version.h"
#include "ByteStreamData/RawEvent.h"
#include "AthenaBaseComps/AthAlgTool.h"

Muon::RpcROD_Decoder::RpcROD_Decoder ( const std::string& type, const std::string& name,const IInterface* parent ) :  AthAlgTool(type,name,parent),
														      //m_hashfunc(0),
														      //niko
														      m_decodeSL(false),
														      //m_byteStreamErrSvc("RPC_ByteStreamErrorSvc",name),
														      m_cabling(0)
														      //,m_bench("RpcROD_Decoder"),
{
    declareInterface< IRpcROD_Decoder  >( this );
    declareProperty("SpecialROBNumber",m_specialROBNumber=-1);
    declareProperty("Sector13Data",m_sector13Data=false);
    declareProperty("DataErrorPrintLimit",m_maxprinterror=1000);
    //declareProperty("RpcByteStreamErrorContainer",m_bsErrContainerName="myRpcByteStreamErrorContainer");
    
}


Muon::RpcROD_Decoder::~RpcROD_Decoder() {

}

StatusCode Muon::RpcROD_Decoder::initialize() {

  ServiceHandle<IRPCcablingServerSvc> rpc_server("RPCcablingServerSvc", name());
  if (rpc_server.retrieve().isFailure()) {
    ATH_MSG_FATAL("Can't get RPCcablingServerSvc" );
    return StatusCode::FAILURE;
  }
  
  if (StatusCode::SUCCESS != rpc_server->giveCabling(m_cabling)) {
    ATH_MSG_FATAL("Can't get RPCcablingSvc from Server");
    return StatusCode::FAILURE; 
  }

  ATH_CHECK( m_muonIdHelperTool.retrieve() );

  if (m_specialROBNumber>0) {
    ATH_MSG_DEBUG("Setting the special ROB Number to: 0x" << MSG::hex << m_specialROBNumber << MSG::dec );
  }

  //==LBTAG initialize vector and variables for format failure check
  for(int i=0; i<13; i++) m_RPCcheckfail[i]=0;
  m_printerror=0;

  return StatusCode::SUCCESS;
}



StatusCode Muon::RpcROD_Decoder::finalize() {
  //==LBTAG print format failure final statistics
  printcheckformat();
  return StatusCode::SUCCESS;
} 


#include "RpcROD_Decoder_v302.C"
