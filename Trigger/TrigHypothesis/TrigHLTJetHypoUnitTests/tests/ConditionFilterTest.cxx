/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "gtest/gtest.h"
#include "TrigHLTJetHypo/../src/ConditionFilter.h"
#include "TrigHLTJetHypo/../src/ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/../src/EtaConditionAbsMT.h"
#include "TrigHLTJetHypo/../src/RepeatedCondition.h"


#include "../src/TLorentzVectorFactory.h"
#include "../src/TLorentzVectorAsIJet.h"
#include "../src/makeHypoJets.h"
#include "../src/makeRepeatedConditions.h"

#include <vector>
#include <memory>

ConditionPtrs cftMakeRepeatedConditions(){

  
  EtaPairs etas;
  etas.push_back(std::make_pair(0., 1.5));
		 
  return  makeRepeatedConditions(etas);

  /*
    auto el_condition = std::unique_ptr<IConditionMT>(nullptr);
  el_condition.reset(new EtaConditionAbsMT(0., 1.5));

  auto rep_condition = std::unique_ptr<IRepeatedCondition>(nullptr);

  std::size_t multiplicity{1u};
  rep_condition.reset(new RepeatedCondition(std::move(el_condition),
					    multiplicity));
  
  ConditionPtrs conditions;
  conditions.push_back(std::move(rep_condition));

  return conditions;
  */
}

TEST(ConditionFilterTester, construction_zeroconditions) { 
  EXPECT_NO_THROW(ConditionFilter());
}

TEST(ConditionFilterTester, construction_onecondition) { 

  ConditionPtrs conditionPtrs = cftMakeRepeatedConditions();
  EXPECT_NO_THROW(ConditionFilter(std::move(conditionPtrs)));
}

TEST(ConditionFilterTester, zerojets_zeroconditions) {
  auto rf = ConditionFilter();
  HypoJetVector tv{};

  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);
  
  auto pair = rf.filter(tv.begin(), tv.end(), deb);
  EXPECT_EQ((pair.second - pair.first), 0);

  EXPECT_EQ(pair.first, tv.begin());
  EXPECT_EQ(pair.second, tv.end());
}

TEST(ConditionFilterTester, zerojets_onecondition) {

  auto conditionPtrs = cftMakeRepeatedConditions();
  ConditionFilter filter(std::move(conditionPtrs));

  HypoJetVector tv{};

  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);
  
  auto pair = filter.filter(tv.begin(), tv.end(), deb);
  EXPECT_EQ((pair.second - pair.first), 0);
  
  EXPECT_EQ(pair.first, tv.begin());
  EXPECT_EQ(pair.second, tv.end());
}

TEST(ConditionFilterTester, twojets_zeroconditions) {

  ConditionFilter filter;

  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);
  std::vector<double> jet_eta{1.0, 0.5, 2.0, 4.0};
  HypoJetVector tv = makeHypoJets(jet_eta);
  
  EXPECT_EQ((tv.end() - tv.begin()),  4u);

  auto pair = filter.filter(tv.begin(), tv.end(), deb);
  EXPECT_EQ((pair.second - pair.first), 4u);

  const auto& fiter = pair.first;
  const auto& siter = pair.second;

  double f_eta = (*fiter)->eta();
  double s_eta = (*(siter-1))->eta();
  
  EXPECT_EQ( f_eta, (*(tv.begin()))->eta());
  EXPECT_EQ( s_eta, (*(tv.end()-1))->eta());

}


TEST(ConditionFilterTester, twojets_onecondition) {

  ConditionPtrs conditionPtrs = cftMakeRepeatedConditions();
  auto filter = ConditionFilter(std::move(conditionPtrs));
  std::unique_ptr<ITrigJetHypoInfoCollector> deb(nullptr);

  std::vector<double> jet_eta{1.0, 0.5, 2.0, 4.0};
  HypoJetVector tv = makeHypoJets(jet_eta);

  std::pair<HypoJetCIter, HypoJetCIter> iters =
    std::make_pair(tv.begin(), tv.end());
  
  iters = filter.filter(iters.first, iters.second, deb);
  EXPECT_EQ((iters.second - iters.first), 2);

  const auto& fiter = iters.first;
  const auto& siter = iters.second;

  auto etas = std::vector<double> {};
  etas.reserve(siter-fiter);
  std::transform(fiter,
		 siter,
		 std::back_inserter(etas),
		 [](const pHypoJet& hj){return hj->eta();});
  std::sort(etas.begin(), etas.end());

  EXPECT_EQ(etas.size(), 2);
  EXPECT_DOUBLE_EQ(etas[0], 0.5);
  EXPECT_DOUBLE_EQ(etas[1], 1.0);

}
