/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMuonEventTPCnv/test/CombinedMuonFeatureCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for CombinedMuonFeatureCnv_p2.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p2.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>
#include <sstream>


void compare (const CombinedMuonFeature& p1,
              const CombinedMuonFeature& p2)
{
  assert (p1.pt() == p2.pt());
  //assert (p1.charge() == p2.charge());
  assert (1 == p2.charge());
  //assert (p1.ptq() == p2.ptq());
  assert (p1.sigma_pt_raw() == p2.sigma_pt_raw());
  //assert (p1.getFlag() == p2.getFlag());
  assert (0 == p2.getFlag());
  assert (p1.muFastTrackLink() == p2.muFastTrackLink());
  assert (p1.IDTrackLink() == p2.IDTrackLink());
}


void testit (const CombinedMuonFeature& trans1)
{
  MsgStream log (0, "test");
  CombinedMuonFeatureCnv_p2 cnv;
  CombinedMuonFeature_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  CombinedMuonFeature trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxies created outside of leak checking.
  ElementLink<MuonFeatureContainer> foo ("foo", 1);
  ElementLink<TrigInDetTrackCollection> bar ("bar", 2);
  Athena_test::Leakcheck check;

  CombinedMuonFeature trans1 (80000, 8000, 800, 3, 4, 5,
                              ElementLink<MuonFeatureContainer> ("foo", 1),
                              ElementLink<TrigInDetTrackCollection> ("bar", 2));
    
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
