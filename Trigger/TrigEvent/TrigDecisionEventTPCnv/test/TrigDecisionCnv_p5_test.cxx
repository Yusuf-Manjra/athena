/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigDecisionEventTPCnv/test/TrigDecisionCnv_p5_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigDecisionCnv_p5.
 */


#undef NDEBUG
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p5.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p5.h"
#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const LVL1CTP::Lvl1Result& p1,
              const LVL1CTP::Lvl1Result& p2)
{
  assert (p1.isConfigured() == p2.isConfigured());
  assert (p1.itemsBeforePrescale() == p2.itemsBeforePrescale());
  assert (p1.itemsAfterPrescale() == p2.itemsAfterPrescale());
  assert (p1.itemsAfterVeto() == p2.itemsAfterVeto());
}


void compare (const TrigDec::TrigDecision& p1,
              const TrigDec::TrigDecision& p2)
{
  assert (p1.masterKey() == p2.masterKey());
  assert (p1.BGCode() == p2.BGCode());
  assert (p1.getL2ResultLink() == p2.getL2ResultLink());
  assert (p1.getEFResultLink() == p2.getEFResultLink());
  assert (p1.getHLTResultLink() == p2.getHLTResultLink());
  compare (p1.getL1Result(), p2.getL1Result());
}


void testit (const TrigDec::TrigDecision& trans1)
{
  MsgStream log (0, "test");
  TrigDec::TrigDecisionCnv_p5 cnv;
  TrigDec::TrigDecision_p5 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigDec::TrigDecision trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  // Get proxies created outside of leak check.
  DataLink<HLT::HLTResult> l2link ("l2result");
  DataLink<HLT::HLTResult> eflink ("efresult");
  Athena_test::Leakcheck check;

  LVL1CTP::Lvl1Result l1result (true);
  l1result.itemsBeforePrescale().assign ({1, 2, 3});
  l1result.itemsAfterPrescale().assign ({4, 5});
  l1result.itemsAfterVeto().assign ({6, 7, 8});
  
  TrigDec::TrigDecision trans1 (l1result,
                                l2link, eflink,
                                12345,
                                10);
  
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
