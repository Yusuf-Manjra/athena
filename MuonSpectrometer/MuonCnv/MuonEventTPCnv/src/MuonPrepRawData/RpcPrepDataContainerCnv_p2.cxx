/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepData_p2.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p1.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepDataCnv_p2.h"
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepDataContainerCnv_p2.h"
#include "TrkEventCnvTools/ITrkEventCnvTool.h"
// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"



#include "AthAllocators/DataPool.h"

StatusCode Muon::RpcPrepDataContainerCnv_p2::initialize(MsgStream &log) {
   // Do not initialize again:
    m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
    StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
        log << MSG::FATAL << "StoreGate service not found !" << endmsg;
        return StatusCode::FAILURE;
    }

   // get DetectorStore service
    StoreGateSvc *detStore;
    sc = svcLocator->service("DetectorStore", detStore);
    if (sc.isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
    }

   // Get the pixel helper from the detector store
    sc = detStore->retrieve(m_RpcId);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get Rpc ID helper !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the Rpc ID helper." << endmsg;
    }

    if (m_eventCnvTool.retrieve().isFailure()) {
        log << MSG::FATAL << "Could not get DetectorDescription manager" << endmsg;
        return StatusCode::FAILURE;
    }

    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
    return StatusCode::SUCCESS;
}
const MuonGM::RpcReadoutElement* Muon::RpcPrepDataContainerCnv_p2::getReadOutElement(const Identifier& id ) const {
    const Trk::ITrkEventCnvTool* cnv_tool = m_eventCnvTool->getCnvTool(id);
    if (!cnv_tool) return nullptr; 
    return dynamic_cast<const MuonGM::RpcReadoutElement*>(cnv_tool->getDetectorElement(id));
}
void Muon::RpcPrepDataContainerCnv_p2::transToPers(const Muon::RpcPrepDataContainer* transCont,  Muon::MuonPRD_Container_p1* persCont, MsgStream &log) 
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all RDO
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the
    // collection. 

    typedef Muon::RpcPrepDataContainer TRANS;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

    RpcPrepDataCnv_p2  chanCnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex = 0;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    int numColl = transCont->numberOfCollections();

    persCont->m_collections.resize(numColl);    log << MSG::DEBUG  << " Preparing " << persCont->m_collections.size() << "Collections" << endmsg;

    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, ++it_Coll)  {
        // Add in new collection
        log << MSG::DEBUG  << " New collection" << endmsg;
        const Muon::RpcPrepDataCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        Muon::MuonPRD_Collection_p1& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().get_identifier32().get_compact();
        pcollection.m_hashId = (unsigned int) collection.identifyHash();
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
        // Add in channels
        persCont->m_PRD.resize(chanEnd);
        for (unsigned int i = 0; i < collection.size(); ++i) {
            const Muon::RpcPrepData* chan = collection[i];
            persCont->m_PRD[i + chanBegin] = toPersistent((CONV**)nullptr, chan, log );
        }
    }
    log << MSG::DEBUG  << " ***  Writing RpcPrepDataContainer ***" << endmsg;
}

void  Muon::RpcPrepDataContainerCnv_p2::persToTrans(const Muon::MuonPRD_Container_p1* persCont, Muon::RpcPrepDataContainer* transCont, MsgStream &log) 
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all channels
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and extract their channels
    // from the vector.


    Muon::RpcPrepDataCollection* coll = nullptr;

    RpcPrepDataCnv_p2  chanCnv;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

    log << MSG::DEBUG  << " Reading " << persCont->m_collections.size() << "Collections" << endmsg;
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - is NOT owner of RpcPrepData (SG::VIEW_ELEMENTS)
    // IDet collection don't have the Ownership policy c'tor
        const Muon::MuonPRD_Collection_p1& pcoll = persCont->m_collections[icoll];        
        Identifier collID(Identifier(pcoll.m_id));
        IdentifierHash collIDHash(IdentifierHash(pcoll.m_hashId));
        coll = new Muon::RpcPrepDataCollection(collIDHash);
        coll->setIdentifier(Identifier(pcoll.m_id));
        unsigned int nchans           = pcoll.m_end - pcoll.m_begin;
        coll->resize(nchans);
        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const TPObjRef pchan = persCont->m_PRD[ichan + pcoll.m_begin];
            Muon::RpcPrepData* chan = dynamic_cast<Muon::RpcPrepData*>(createTransFromPStore((CONV**)nullptr, pchan, log ) );
            if (!chan) {
               log << MSG::ERROR << "AthenaPoolTPCnvIDCont::persToTrans: Cannot get RpcPrepData!" << endmsg;
               continue;
            }
            const MuonGM::RpcReadoutElement * de = getReadOutElement(chan->identify());
            chan->m_detEl = de;
            (*coll)[ichan] = chan;
        }

        // register the rdo collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll, collIDHash);
        if (sc.isFailure()) {
            throw std::runtime_error("Failed to add collection to ID Container");
        }
        if (log.level() <= MSG::DEBUG) {
            log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " << 
                collID.get_compact() << ", added to Identifiable container." << endmsg;
        }
    }

    log << MSG::DEBUG  << " ***  Reading RpcPrepDataContainer" << endmsg;
}



//================================================================
Muon::RpcPrepDataContainer* Muon::RpcPrepDataContainerCnv_p2::createTransient(const Muon::MuonPRD_Container_p1* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize RpcPrepDataContainerCnv_p2 " << endmsg;
            return nullptr;
        } 
    }
    std::unique_ptr<Muon::RpcPrepDataContainer> trans(new Muon::RpcPrepDataContainer(m_RpcId->module_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


