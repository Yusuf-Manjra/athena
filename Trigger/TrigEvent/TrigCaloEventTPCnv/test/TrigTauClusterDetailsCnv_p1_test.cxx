/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigCaloEventTPCnv/test/TrigTauClusterDetailsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TrigTauClusterDetailsCnv_p1.
 */


#undef NDEBUG
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigTauClusterDetails& p1,
              const TrigTauClusterDetails& p2)
{
  for (int i=0; i < NUMEMSAMP; i++) {
    assert (p1.EMRadius(i) == p2.EMRadius(i));
    assert (p1.EMenergyWidth(i) == p2.EMenergyWidth(i));
    assert (p1.EMenergyWide(i) == p2.EMenergyWide(i));
    assert (p1.EMenergyMedium(i) == p2.EMenergyMedium(i));
    assert (p1.EMenergyNarrow(i) == p2.EMenergyNarrow(i));
  }
  for (int i=0; i < NUMHADSAMP; i++) {
    assert (p1.HADRadius(i) == p2.HADRadius(i));
    assert (p1.HADenergyWidth(i) == p2.HADenergyWidth(i));
    assert (p1.HADenergyWide(i) == p2.HADenergyWide(i));
    assert (p1.HADenergyMedium(i) == p2.HADenergyMedium(i));
    assert (p1.HADenergyNarrow(i) == p2.HADenergyNarrow(i));
  }
}


void testit (const TrigTauClusterDetails& trans1)
{
  MsgStream log (0, "test");
  TrigTauClusterDetailsCnv_p1 cnv;
  TrigTauClusterDetails_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigTauClusterDetails trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigTauClusterDetails trans1;
  for (int i=0; i < NUMEMSAMP; i++) {
    trans1.setEMRadius (i, i*100 + 1.5);
    trans1.setEMenergyWidth (i, i*100 + 2.5);
    trans1.setEMenergyWide (i, i*100 + 3.5);
    trans1.setEMenergyMedium (i, i*100 + 4.5);
    trans1.setEMenergyNarrow (i, i*100 + 5.5);
  }
  for (int i=0; i < NUMHADSAMP; i++) {
    trans1.setHADRadius (i, i*100 + 6.5);
    trans1.setHADenergyWidth (i, i*100 + 7.5);
    trans1.setHADenergyWide (i, i*100 + 8.5);
    trans1.setHADenergyMedium (i, i*100 + 9.5);
    trans1.setHADenergyNarrow (i, i*100 + 10.5);
  }
  
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
