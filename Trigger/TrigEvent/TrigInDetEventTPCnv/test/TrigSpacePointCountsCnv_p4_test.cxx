/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigInDetEventTPCnv/test/TrigSpacePointCountsCnv_p4_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigSpacePointCountsCnv_p4.
 */


#undef NDEBUG
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p4.h"
#include "TestTools/random.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


using Athena_test::randf;


void compare (const TrigHisto& p1,
              const TrigHisto& p2)
{
  assert (p1.contents() == p2.contents());
  assert (p1.nbins_x() == p2.nbins_x());
  assert (p1.min_x() == p2.min_x());
  assert (p1.max_x() == p2.max_x());
}


void compare (const TrigHisto2D& p1,
              const TrigHisto2D& p2)
{
  compare (static_cast<const TrigHisto&>(p1),
           static_cast<const TrigHisto&>(p2));
  
  assert (p1.nbins_y() == p2.nbins_y());
  assert (p1.min_y() == p2.min_y());
  assert (p1.max_y() == p2.max_y());

  assert (p1.sumEntries (5, 4, TrigHistoCutType::BELOW_X_BELOW_Y) ==
          p2.sumEntries (5, 4, TrigHistoCutType::BELOW_X_BELOW_Y));
  assert (p1.sumEntries (5, 4, TrigHistoCutType::ABOVE_X_BELOW_Y) ==
          p2.sumEntries (5, 4, TrigHistoCutType::ABOVE_X_BELOW_Y));
  assert (p1.sumEntries (5, 4, TrigHistoCutType::BELOW_X_ABOVE_Y) ==
          p2.sumEntries (5, 4, TrigHistoCutType::BELOW_X_ABOVE_Y));
  assert (p1.sumEntries (5, 4, TrigHistoCutType::ABOVE_X_ABOVE_Y) ==
          p2.sumEntries (5, 4, TrigHistoCutType::ABOVE_X_ABOVE_Y));
}


void compare (const TrigSpacePointCounts& p1,
              const TrigSpacePointCounts& p2)
{
  compare (p1.pixelClusEndcapC(), p2.pixelClusEndcapC());
  compare (p1.pixelClusEndcapA(), p2.pixelClusEndcapA());
  compare (p1.pixelClusBarrel(), p2.pixelClusBarrel());
  assert (p1.droppedPixelModules() == p2.droppedPixelModules());
  assert (p1.droppedSctModules() == p2.droppedSctModules());
  assert (p1.sctSpEndcapC() == p2.sctSpEndcapC());
  assert (p1.sctSpEndcapA() == p2.sctSpEndcapA());
  assert (p1.sctSpBarrel() == p2.sctSpBarrel());
}


void testit (const TrigSpacePointCounts& trans1)
{
  MsgStream log (0, "test");
  TrigSpacePointCountsCnv_p4 cnv;
  TrigSpacePointCounts_p4 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigSpacePointCounts trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigHisto2D pixelClusEndcapC (20, 0, 10, 20, 0, 10);
  for (int i=0; i < 1000; i++)
    pixelClusEndcapC.fill (randf(10), randf(10), randf(3));
  TrigHisto2D pixelClusEndcapA (20, 0, 10, 20, 0, 10);
  for (int i=0; i < 1000; i++)
    pixelClusEndcapA.fill (randf(10), randf(10), randf(3));
  TrigHisto2D pixelClusBarrel (20, 0, 10, 20, 0, 10);
  for (int i=0; i < 1000; i++)
    pixelClusBarrel.fill (randf(10), randf(10), randf(3));

  std::vector<Identifier> droppedPixelModules {Identifier(123),
                                               Identifier(234)};
  std::vector<Identifier> droppedSctModules {Identifier(345),
                                             Identifier(456),
                                             Identifier(567)};
  TrigSpacePointCounts trans1 (pixelClusEndcapC,
                               pixelClusBarrel,
                               pixelClusEndcapA,
                               droppedPixelModules,
                               1, 2, 3,
                               droppedSctModules);

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
