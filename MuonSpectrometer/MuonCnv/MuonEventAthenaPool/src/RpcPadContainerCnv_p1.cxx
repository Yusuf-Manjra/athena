/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonEventAthenaPool/RpcPad_p1.h"
#include "MuonEventAthenaPool/RpcPadContainer_p1.h"
#undef private
#undef protected

#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "RpcPadCnv_p1.h"
#include "RpcPadContainerCnv_p1.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "DataModel/DataPool.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCcablingInterface/RpcPadIdHash.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"

StatusCode RpcPadContainerCnv_p1::initialize(MsgStream &log) {
   // Do not initialize again:
    m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();


    // get RPC cablingSvc
    const IRPCcablingServerSvc* RpcCabGet = 0;
    StatusCode sc =  svcLocator->service("RPCcablingServerSvc", RpcCabGet);
    if (sc.isFailure()) {
        log<<MSG::FATAL << "Could not get RPCcablingServerSvc !" << endreq;
        return StatusCode::FAILURE;
    }
    else log <<MSG::VERBOSE << " RPCcablingServerSvc retrieved" << endreq;

    sc = RpcCabGet->giveCabling(m_rpcCabling);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get RPCcablingSvc from the Server !" << endreq;
        m_rpcCabling = 0;
        return StatusCode::FAILURE;
    }
    else {
        log <<MSG::VERBOSE << " RPCcablingSvc obtained " << endreq;
    }

    log << MSG::DEBUG << "Converter initialized." << endreq;
    return StatusCode::SUCCESS;
}

void RpcPadContainerCnv_p1::transToPers(const RpcPadContainer* transCont,  RpcPadContainer_p1* persCont, MsgStream &log) 
{

    typedef RpcPadContainer TRANS;

    RpcPadCnv_p1  cnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;

    int numColl = transCont->numberOfCollections();
    persCont->resize(numColl);    log << MSG::DEBUG  << " Preparing " << persCont->size() << "Collections" << endreq;

    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        //log << MSG::DEBUG  << " New collection" << endreq;
        RpcPad_p1* pcoll = &( (*persCont)[collIndex] );
        cnv.transToPers( &(**it_Coll), pcoll , log);
    }
    log << MSG::DEBUG  << " ***  Writing RpcPadContainer ***" << endreq;
}

void  RpcPadContainerCnv_p1::persToTrans(const RpcPadContainer_p1* persCont, RpcPadContainer* transCont, MsgStream &log) 
{


    RpcPadCnv_p1  cnv;

    log << MSG::DEBUG  << " Reading " << persCont->size() << "Collections" << endreq;
    for (unsigned int icoll = 0; icoll < persCont->size(); ++icoll) {

        // Identifier collID(Identifier(pcoll.m_id));
        // IdentifierHash collIDHash(IdentifierHash(persCont[icoll].m_hashId));
        RpcPad* coll = new RpcPad;
        cnv.persToTrans( &( (*persCont)[icoll]), coll, log);
         // register the rdo collection in IDC with hash - faster addCollection
// log<<MSG::INFO<<"coll has size "<<coll->size()<<endreq;
// log << MSG::INFO<<" coll has hash: "<<static_cast<unsigned int>(coll->identifyHash())<<endreq;
// log << MSG::INFO<<" coll has id: "<<static_cast<unsigned int>(coll->identify())<<endreq;

// hack to fix bug#56429 - always use identifier to get hash

        // sanity check
//        if (coll->identifyHash() > static_cast<unsigned int>(m_rpcCabling->padHashFunction()->max() ) ) {
//            m_errorCount++;

//            if (m_errorCount<m_maxNumberOfErrors) 
//               log<<MSG::WARNING<< "The persistent RDO had an IdentifierHash of "<<coll->identifyHash()
//                <<" which is larger than the hash max of "<<m_rpcCabling->padHashFunction()->max()
//                <<". Will attempt to correct from Identifier. This is message "<<m_errorCount<<" of "
//                <<m_maxNumberOfErrors<<" maximum."<<endreq;
//            if (m_errorCount==m_maxNumberOfErrors) log<<MSG::WARNING<<"Exceeding max errors. Further WARNINGs will be suppressed!"<<endreq;
            const RpcPadIdHash* pdHashFn = m_rpcCabling->padHashFunction();
            int temp = (*pdHashFn)(coll->identify());
            if (temp==-1) {
                coll->m_idHash = IdentifierHash(); 
                log<<MSG::WARNING<<"get_hash failed - nothing else we can try!"<<endreq;
            } else{
                coll->m_idHash = temp; 
//                if (m_errorCount<m_maxNumberOfErrors) log<<MSG::WARNING<<"New hash is: "<<coll->identifyHash()<<endreq;
            }
//        }
        
        // another check - see if already added
        RpcPadContainer::const_iterator it = transCont->indexFind(coll->identifyHash());
        if (it!=transCont->end() ) {
            log << MSG::WARNING<<"Collection with hash="<<coll->identifyHash()<<" already exists in container (with "<<(*it)->size()<<" elements). "
                << "Will therefore DISCARD this collection which has "<<coll->size()<<" elements)!"<<endreq;
            delete coll;
        } else {
            StatusCode sc = transCont->addCollection(coll, coll->identifyHash());
            if (sc.isFailure()) {
                log << MSG::WARNING<<"Could not add collection with hash="<<coll->identifyHash()
                    <<" to IDC which has hash max of "<<transCont->size()<<" (PadHashFunction gives "<<m_rpcCabling->padHashFunction()->max()<<")"<<endreq;
                throw std::runtime_error("Failed to add collection to ID Container. Hash = "+std::to_string(coll->identifyHash()));
            }
        }
        // if (log.level() <= MSG::DEBUG) {
        //     log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " << 
        //         persCont[icoll].m_id << ", added to Identifiable container." << endreq;
        // }
    }

    log << MSG::DEBUG  << " ***  Reading RpcPadContainer" << endreq;
}



//================================================================
RpcPadContainer* RpcPadContainerCnv_p1::createTransient(const RpcPadContainer_p1* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize RpcPadContainerCnv_p1 " << endreq;
            return 0;
        } 
    }
    // log<<MSG::INFO<<"creating new pad container with hashmax= "<<m_RpcId->module_hash_max()<<endreq;
    
    std::auto_ptr<RpcPadContainer> trans(new RpcPadContainer(m_rpcCabling->padHashFunction()->max() ));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


