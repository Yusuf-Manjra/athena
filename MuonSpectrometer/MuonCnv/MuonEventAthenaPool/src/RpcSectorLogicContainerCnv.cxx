/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonRDO/RpcSectorLogicContainer.h"
#undef private
#undef protected
#include "RpcSectorLogicContainerCnv.h"
#include "MuonEventAthenaPool/RpcSectorLogicContainer_p1.h"
#include "RpcSectorLogicContainerCnv_p1.h"

static RpcSectorLogicContainerCnv_p1   TPconverter_p1;

RpcSectorLogicContainer_p1* RpcSectorLogicContainerCnv::createPersistent(RpcSectorLogicContainer* transObj) {
    MsgStream log(messageService(), "MuonRpcSectorLogicContainerConverter" );
    RpcSectorLogicContainer_p1 *persObj = TPconverter_p1.createPersistent( transObj, log );
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "RpcSectorLogicContainer write Success" << endreq;
    return persObj;
}
   
RpcSectorLogicContainer* RpcSectorLogicContainerCnv::createTransient() {
    static pool::Guid   p1_guid("93035F54-0FA9-4A56-98E0-A808DD23C089");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< RpcSectorLogicContainer_p1 > col_vect( poolReadObject< RpcSectorLogicContainer_p1 >() );
        MsgStream log(messageService(), "RpcSectorLogicContainerCnv_p1" );
        //log << MSG::DEBUG << "Reading RpcSectorLogicContainer_p1" << endreq;
        return TPconverter_p1.createTransient( col_vect.get(), log );
    } 
    throw std::runtime_error("Unsupported persistent version of RpcSectorLogicContainer");
}
