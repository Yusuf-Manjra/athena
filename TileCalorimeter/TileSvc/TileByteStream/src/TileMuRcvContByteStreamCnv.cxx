/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IToolSvc.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreClearedIncident.h"
#include "AthenaKernel/CLASS_DEF.h"

// Tile includes
#include "TileByteStream/TileMuRcvContByteStreamCnv.h"
#include "TileByteStream/TileMuRcvContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"


#include <string> 


TileMuRcvContByteStreamCnv::TileMuRcvContByteStreamCnv(ISvcLocator* svcloc)
  : Converter(ByteStream_StorageType, classID(),svcloc)
  , ::AthMessaging(msgSvc(), "TileMuRcvContByteStreamCnv")
  , m_name("TileMuRcvContByteStreamCnv")
  , m_tool("TileMuRcvContByteStreamTool")
  , m_byteStreamEventAccess("ByteStreamCnvSvc", m_name)
  , m_byteStreamCnvSvc(0)
  , m_storeGate("StoreGateSvc", m_name)
  , m_robSvc("ROBDataProviderSvc", m_name)
  , m_decoder("TileROD_Decoder")
{
}

const CLID& TileMuRcvContByteStreamCnv::classID(){
return ClassID_traits<TileMuonReceiverContainer>::ID() ;
}

StatusCode TileMuRcvContByteStreamCnv::initialize()
{
  CHECK(Converter::initialize());

  ATH_MSG_DEBUG(" initialize ");

  // Get ByteStreamCnvSvc
  CHECK( m_byteStreamEventAccess.retrieve() );
  m_byteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(&*m_byteStreamEventAccess);

  // retrieve Tool
  CHECK( m_decoder.retrieve() );

  CHECK( m_tool.retrieve() );

  CHECK( m_robSvc.retrieve() );

  CHECK( m_storeGate.retrieve() );

  return StatusCode::SUCCESS ;
}


StatusCode TileMuRcvContByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{

  ATH_MSG_DEBUG( " Executing createObj method" );

  ByteStreamAddress* pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr);
  if(!pRE_Addr) {
    ATH_MSG_ERROR( " Can not cast to ByteStreamAddress " );
    return StatusCode::FAILURE;
  }

  const RawEvent* re = m_robSvc->getEvent();
  if (!re) {
    ATH_MSG_ERROR( "Could not get raw event from ByteStreamInputSvc" );
    return StatusCode::FAILURE;
  }

  auto cont = std::make_unique<TileMuonReceiverContainer>();
  StatusCode sc=m_decoder->convertTMDBDecision(re,cont.get());
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Conversion tool returned an error. TileMuonReceiverContainer might be empty." );
  }

  pObj = SG::asStorable( std::move(cont) );

  return StatusCode::SUCCESS;
}

StatusCode TileMuRcvContByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
  ATH_MSG_DEBUG( " Executing createRep method" );

  // get Full Event Assembler
  FullEventAssembler<TileHid2RESrcID>* fea = 0;
  std::string key("Tile");

  CHECK( m_byteStreamCnvSvc->getFullEventAssembler(fea, key) );

  // create TileMuonReceiverContainer
  //
  TileMuonReceiverContainer* muRcvCont(0);
  SG::fromStorable(pObj, muRcvCont);
  if(!muRcvCont){
    ATH_MSG_ERROR( " Can not cast to TileMuonReceiverContainer " );
    return StatusCode::FAILURE;
  }

  std::string name = pObj->registry()->name();
  ByteStreamAddress* addr = new ByteStreamAddress(classID(), name, "");

  pAddr = addr;

  // call TileMuRcvContByteStreamTool
  CHECK( m_tool->convert(muRcvCont,fea) );

  return StatusCode::SUCCESS;
}

StatusCode TileMuRcvContByteStreamCnv::finalize()
{
  return Converter::finalize();
}

