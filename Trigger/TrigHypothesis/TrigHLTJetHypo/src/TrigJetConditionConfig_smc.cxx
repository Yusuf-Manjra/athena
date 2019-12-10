/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetConditionConfig_smc.h"

#include "GaudiKernel/StatusCode.h"
#include "./SMCConditionMT.h"

TrigJetConditionConfig_smc::TrigJetConditionConfig_smc(const std::string& type,
						       const std::string& name,
						       const IInterface* parent) :
  base_class(type, name, parent){

}


StatusCode TrigJetConditionConfig_smc::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}


ConditionMT TrigJetConditionConfig_smc::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<SMCConditionMT>(a2d(m_min), a2d(m_max));
}


StatusCode TrigJetConditionConfig_smc::checkVals() const {

  auto a2d = ArgStrToDouble();
  if (a2d(m_min) > a2d(m_max)){
    ATH_MSG_ERROR(" min smc >  max smc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


bool TrigJetConditionConfig_smc::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_smc::capacity() const {
  return getCondition()->capacity();
}
