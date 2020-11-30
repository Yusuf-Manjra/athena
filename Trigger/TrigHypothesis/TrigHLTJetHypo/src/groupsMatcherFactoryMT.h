/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_GROUPSMATCHERFACTORYMT_H
#define TRIGHLTJETHYPO_GROUPSMATCHERFACTORYMT_H

#include "./IGroupsMatcherMT.h"
#include "./ConditionsDefsMT.h"
#include "./CapacityCheckedConditionsDefs.h"
#include <memory>

std::unique_ptr<IGroupsMatcherMT>
groupsMatcherFactoryMT_SingleCondition(ConditionsMT&&);

std::unique_ptr<IGroupsMatcherMT>
groupsMatcherFactoryMT_MaxBipartite(ConditionsMT&&);

std::unique_ptr<IGroupsMatcherMT>
groupsMatcherFactoryMT_Partitions(ConditionsMT&&);


std::unique_ptr<IGroupsMatcherMT>
groupsMatcherFactoryMT_FastReduction(ConditionPtrs&&,
				     const std::vector<std::size_t>& treeVec);
#endif
