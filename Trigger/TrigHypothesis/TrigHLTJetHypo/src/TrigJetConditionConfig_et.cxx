/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for ET Condition
 */
#include "TrigJetConditionConfig_et.h"
#include "GaudiKernel/StatusCode.h"
#include "./EtConditionMT.h"
#include "./ArgStrToDouble.h"


TrigJetConditionConfig_et::TrigJetConditionConfig_et(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_et::initialize() {
  return StatusCode::SUCCESS;
}


ConditionMT TrigJetConditionConfig_et::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<EtConditionMT>(a2d(m_min));
}
				     

StatusCode TrigJetConditionConfig_et::checkVals() const {
  return StatusCode::SUCCESS;
}

bool TrigJetConditionConfig_et::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_et::capacity() const {
  return getCondition()->capacity();
}
