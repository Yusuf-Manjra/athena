/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <AthenaKernel/getMessageSvc.h>
#include <MuonPRDTest/PrdTesterModule.h>
#include <StoreGate/ReadCondHandle.h>
PrdTesterModule::PrdTesterModule(MuonTesterTree& tree, const std::string& grp_name, bool useCondGeo, MSG::Level msglvl) :
    MuonTesterBranch(tree, " prd module " + grp_name), AthMessaging(Athena::getMessageSvc(), grp_name), m_useCondDetMgr{useCondGeo} {
    setLevel(msglvl);
    m_idHelperSvc.retrieve().ignore();
}
const Muon::IMuonIdHelperSvc* PrdTesterModule::idHelperSvc() const { return m_idHelperSvc.get(); }
const MuonGM::MuonDetectorManager* PrdTesterModule::getDetMgr(const EventContext& ctx) const {
    if (!m_useCondDetMgr) return m_detMgr;
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> handle{m_detMgrKey, ctx};
    if (!handle.isValid()) {
        ATH_MSG_ERROR("Failed to retrieve MuonDetectorManager " << m_detMgrKey.fullKey());
        return nullptr;
    }
    return handle.cptr();
}
bool PrdTesterModule::init() {
    if (!m_useCondDetMgr && (!m_detStore.retrieve().isSuccess() || !m_detStore->retrieve(m_detMgr).isSuccess())) return false;
    if (!m_detMgrKey.initialize(m_useCondDetMgr).isSuccess()) return false;
    if (!m_idHelperSvc.retrieve().isSuccess()) return false;

    return declare_keys();
}
bool PrdTesterModule::declare_keys() { return true; }
