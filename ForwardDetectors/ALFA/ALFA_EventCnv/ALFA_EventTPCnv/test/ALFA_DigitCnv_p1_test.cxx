/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_DigitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_DigitCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_DigitCnv_p1.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


void compare (const ALFA_Digit& p1,
              const ALFA_Digit& p2)
{
  assert (p1.getStation() == p2.getStation());
  assert (p1.getPlate()   == p2.getPlate());
  assert (p1.getFiber()   == p2.getFiber());
}


void testit (const ALFA_Digit& trans1)
{
  MsgStream log (0, "test");
  ALFA_DigitCnv_p1 cnv;
  ALFA_Digit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_Digit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  ALFA_Digit trans1 (123, 234, 345);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
