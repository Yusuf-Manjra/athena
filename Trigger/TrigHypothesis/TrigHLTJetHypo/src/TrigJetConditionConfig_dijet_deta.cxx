/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for Dijet_Deta Conditions
 */
#include "TrigJetConditionConfig_dijet_deta.h"
#include "GaudiKernel/StatusCode.h"
#include "./DijetDEtaConditionMT.h"
#include "./ArgStrToDouble.h"

TrigJetConditionConfig_dijet_deta::TrigJetConditionConfig_dijet_deta(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


StatusCode TrigJetConditionConfig_dijet_deta::initialize() {
  CHECK(checkVals());

  return StatusCode::SUCCESS;
}


ConditionMT TrigJetConditionConfig_dijet_deta::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<DijetDEtaConditionMT>(a2d(m_min), a2d(m_max));
}


StatusCode TrigJetConditionConfig_dijet_deta::checkVals() const {
    auto a2d = ArgStrToDouble();
  if (a2d(m_min) > a2d(m_max)){
    ATH_MSG_ERROR("dEtaMin > dEtaMax");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

bool TrigJetConditionConfig_dijet_deta::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_dijet_deta::capacity() const {
  return getCondition()->capacity();
}
