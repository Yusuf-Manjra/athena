/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloSuperCellAlignCondAlg.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescrUtils/CaloSuperCellUtils.h"
#include "AthenaKernel/getMessageSvc.h"

#include <memory>

StatusCode CaloSuperCellAlignCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());
  
  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_scidTool.retrieve());
  ATH_CHECK(m_readCaloMgrKey.initialize());
  ATH_CHECK(m_writeCaloSuperCellMgrKey.initialize());
  // Register Write Cond Handle
  if(m_condSvc->regHandle(this, m_writeCaloSuperCellMgrKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeCaloSuperCellMgrKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode CaloSuperCellAlignCondAlg::execute()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<CaloSuperCellDetDescrManager> writeCaloSuperCellMgrHandle{m_writeCaloSuperCellMgrKey,ctx};
  if (writeCaloSuperCellMgrHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }
  
  // ____________ Get Read Cond Object ____________
  SG::ReadCondHandle<CaloDetDescrManager> readCaloMgrHandle{m_readCaloMgrKey,ctx};
  ATH_CHECK(readCaloMgrHandle.isValid());
  ATH_MSG_DEBUG("Retrieved CaloDetDescrManager object form the Condition Store");
  writeCaloSuperCellMgrHandle.addDependency(readCaloMgrHandle);
  
  // ____________ Build new CaloSuperCellDetDescrManager _________________
  std::unique_ptr<CaloSuperCellDetDescrManager> mgr = std::make_unique<CaloSuperCellDetDescrManager>();

  const CaloIdManager* caloId_mgr{nullptr};
  if(detStore()->retrieve(caloId_mgr, "CaloIdManager")!=StatusCode::SUCCESS)
    throw std::runtime_error("... failed to acquire a pointer to CaloIdManager helper");
 
  mgr->set_helper(caloId_mgr->getCaloCell_SuperCell_ID());
  mgr->set_helper(caloId_mgr);
  mgr->initialize();
 
  createDescriptors(mgr.get());
  createElements(mgr.get());

  // ____________ Apply Alignment Corrections ____________________________
  ATH_CHECK(updateElements(mgr.get(),*readCaloMgrHandle,m_scidTool.get()));
  updateDescriptors(mgr.get(),*readCaloMgrHandle,m_scidTool.get());

  ATH_CHECK(writeCaloSuperCellMgrHandle.record(std::move(mgr)));
  ATH_MSG_INFO("recorded new CaloSuperCellDetDescr Manager condition object with key " << writeCaloSuperCellMgrHandle.key() 
	       << " and range " << writeCaloSuperCellMgrHandle.getRange());
  
  return StatusCode::SUCCESS;
}
