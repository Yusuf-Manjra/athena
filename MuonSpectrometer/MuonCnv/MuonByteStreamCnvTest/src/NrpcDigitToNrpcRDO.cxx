/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamCnvTest/NrpcDigitToNrpcRDO.h"

#include <algorithm>
#include "GaudiKernel/PhysicalConstants.h"
#include "MuonDigitContainer/RpcDigit.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigT1RPClogic/ShowData.h"
#include "xAODMuonRDO/NRPCRDOAuxContainer.h"

namespace {
    constexpr unsigned int rpcRawHitWordLength = 7;
    constexpr double inverseSpeedOfLight = 1 / Gaudi::Units::c_light;  // need 1/299.792458
   
}  // namespace

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

NrpcDigitToNrpcRDO::NrpcDigitToNrpcRDO(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator){}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode NrpcDigitToNrpcRDO::initialize() {
    ATH_MSG_DEBUG(" in initialize()");
    
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK( m_NrpcContainerKey.initialize() );

    ATH_MSG_DEBUG("Tag info filled successfully");

    ATH_CHECK(m_cablingKey.initialize());
    ATH_MSG_DEBUG("Cabling info initialized");

    ATH_CHECK(m_digitContainerKey.initialize());
    ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_digitContainerKey);
    
    ATH_CHECK(m_muonManagerKey.initialize());
    for (const std::string& statName : m_convStat){
        m_selectedStations.insert(m_idHelperSvc->rpcIdHelper().stationNameIndex(statName));
    }    
    return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode NrpcDigitToNrpcRDO::execute(const EventContext& ctx) const {
    ATH_MSG_DEBUG("in execute()");

    SG::ReadHandle<RpcDigitContainer> container(m_digitContainerKey, ctx);
    if (!container.isValid()) {
        ATH_MSG_FATAL("Could not find RpcDigitContainer called " << container.name() << " in store " << container.store());
        return StatusCode::FAILURE;
    }
    SG::ReadCondHandle<MuonNRPC_CablingMap> readHandle_Cabling(m_cablingKey, ctx);
    if (!readHandle_Cabling.isValid()) {
        ATH_MSG_FATAL("Could not find MuonNRPC_CablingMap " );
        return StatusCode::FAILURE;
    }
    const MuonNRPC_CablingMap* cabling_ptr = readHandle_Cabling.cptr();
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> muonDetMgr{m_muonManagerKey, ctx};
    if (!muonDetMgr.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve the muon detector manager "<<m_muonManagerKey.fullKey());
        return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG("Found MuonNRPC_CablingMap ");

    /// Record the output container
    SG::WriteHandle<xAOD::NRPCRDOContainer> nrpcRdoHandle(m_NrpcContainerKey, ctx);
    ATH_CHECK(nrpcRdoHandle.record(std::make_unique<xAOD::NRPCRDOContainer>(), std::make_unique<xAOD::NRPCRDOAuxContainer>()));
    xAOD::NRPCRDOContainer* nrpcRdoData = nrpcRdoHandle.ptr();
    

    const IdContext rpcContext = m_idHelperSvc->rpcIdHelper().module_context();
    
    // loop over digit collections
    for (const RpcDigitCollection* rpcCollection : *container) {
        ATH_MSG_DEBUG("RPC Digit -> Pad loop :: digitCollection at " << rpcCollection);

        IdentifierHash moduleHash = rpcCollection->identifierHash();
        Identifier moduleId{0};

        if (m_idHelperSvc->rpcIdHelper().get_id(moduleHash, moduleId, &rpcContext)) {
            ATH_MSG_WARNING("Failed to translate the "<<moduleHash<<" to a valid identifier");
            continue;
        }
        if (!m_selectedStations.count(m_idHelperSvc->stationName(moduleId))) {
            ATH_MSG_DEBUG("Detector element "<<m_idHelperSvc->toString(moduleId)
                         <<" is not considered to be a small gap RPC");
            continue;
        }

        // loop over digit 
        for (const RpcDigit* rpcDigit : *rpcCollection) {
            const Identifier channelId = rpcDigit->identify();

            ATH_MSG_DEBUG("Convert RPC digit "<<m_idHelperSvc->toString(channelId));
            
            MuonNRPC_CablingMap::CablingData cabling_data{};
            /// Load the identifier into the cabling data
            if (!cabling_ptr->convert(channelId, cabling_data)) {
                ATH_MSG_FATAL("Found a non NRPC identifier " << m_idHelperSvc->toString(channelId));
                return StatusCode::FAILURE;
            }
            const MuonGM::RpcReadoutElement* descriptor = muonDetMgr->getRpcReadoutElement(channelId);
            if (!descriptor) {
                ATH_MSG_FATAL("No detector element associated to "<<m_idHelperSvc->toString(channelId));
                return StatusCode::FAILURE;
            }
            // Get the global position of RPC strip from MuonDetDesc
            const Amg::Vector3D pos = descriptor->stripPos(channelId);
            
            bool cabling = cabling_ptr->getOnlineId(cabling_data, msgStream());
            if (!cabling) {
                ATH_MSG_ERROR("Offline to Online Id conversion for NRPC chamber.");
                return StatusCode::FAILURE;
            }
            /// Correct for the time of flight
            const float rdo_time = m_patch_for_rpc_time ? rpcDigit->time() - pos.mag()* inverseSpeedOfLight 
                                                        : rpcDigit->time();

            const float the_timeoverthr = rpcDigit->ToT();
            uint32_t the_bcid= rdo_time /25.;

            xAOD::NRPCRDO* NrpcRdo = new xAOD::NRPCRDO();
            nrpcRdoData->push_back(NrpcRdo);			            
            NrpcRdo->setBcid(the_bcid);
            NrpcRdo->setTime(rdo_time);
            NrpcRdo->setSubdetector(cabling_data.subDetector);
            NrpcRdo->setTdcsector(cabling_data.tdcSector);
            NrpcRdo->setTdc(cabling_data.tdc);
            NrpcRdo->setChannel(cabling_data.channelId);
            NrpcRdo->setTimeoverthr(the_timeoverthr);
        }
    }
    return StatusCode::SUCCESS;
}