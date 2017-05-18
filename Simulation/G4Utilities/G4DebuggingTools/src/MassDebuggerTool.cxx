/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "MassDebuggerTool.h"
namespace G4UA {
MassDebuggerTool::MassDebuggerTool(const std::string &type, const std::string &name, const IInterface *parent) :
    ActionToolBase<MassDebugger>(type, name, parent), m_config() {
    declareProperty("TargetLogicalVolume", m_config.targetLogicalVolume);
    declareProperty("nStat", m_config.nStat);
}

std::unique_ptr<MassDebugger>  MassDebuggerTool::makeAction() {
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<MassDebugger>(m_config);
    return std::move(action);
}

StatusCode MassDebuggerTool::queryInterface(const InterfaceID &riid, void * *ppvIf) {
    if (riid == IBeginRunActionTool::interfaceID()) {
        *ppvIf = (IBeginRunActionTool *) this;
        addRef();
        return StatusCode::SUCCESS;
    }
    return ActionToolBase<MassDebugger>::queryInterface(riid, ppvIf);
}
} // namespace G4UA
