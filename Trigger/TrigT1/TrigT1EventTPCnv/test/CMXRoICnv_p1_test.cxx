/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigT1EventTPCnv/test/CMXRoICnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for CMXRoICnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/CMXRoICnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::CMXRoI& p1,
              const LVL1::CMXRoI& p2)
{
  for (int i = 0; i < 6; i++)
    assert (p1.roiWord(i) == p2.roiWord(i));
}


void testit (const LVL1::CMXRoI& trans1)
{
  MsgStream log (0, "test");
  CMXRoICnv_p1 cnv;
  CMXRoI_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::CMXRoI trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  LVL1::CMXRoI trans1 (1, 2, 3, 4, 5, 6);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
