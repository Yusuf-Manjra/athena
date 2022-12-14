/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileTPCnv/test/TileCosmicMuonCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TileCosmicMuonCnv_p2.
 */


#undef NDEBUG
#include "TileTPCnv/TileCosmicMuonCnv_p2.h"
#include "TestTools/initGaudi.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TileCosmicMuon& p1,
              const TileCosmicMuon& p2)
{
  assert (p1.GetTime() == p2.GetTime());
  assert (p1.GetPositionX() == p2.GetPositionX());
  assert (p1.GetPositionY() == p2.GetPositionY());
  assert (p1.GetPositionZ() == p2.GetPositionZ());
  assert (p1.GetDirectionPhi() == p2.GetDirectionPhi());
  assert (p1.GetDirectionTheta() == p2.GetDirectionTheta());
  assert (p1.GetFitQuality() == p2.GetFitQuality());
  assert (p1.GetFitNCells() == p2.GetFitNCells());
  assert (p1.GetNSamples() == p2.GetNSamples());
  for (int i=0; i < p1.GetNSamples(); i++) {
    assert (p1.GetPathTop(i) == p2.GetPathTop(i));
    assert (p1.GetPathBottom(i) == p2.GetPathBottom(i));
    assert (p1.GetEnergyTop(i) == p2.GetEnergyTop(i));
    assert (p1.GetEnergyBottom(i) == p2.GetEnergyBottom(i));
  }
  assert (p1.GetTrackNCells() == p2.GetTrackNCells());
  for (int i = 0; i < p1.GetTrackNCells(); i++) {
    assert (p1.GetTrackCellHash(i) == p2.GetTrackCellHash(i));
  }
  assert (p1.GetNSegments() == p2.GetNSegments());
  for (int i = 0; i < p1.GetNSegments(); i++) {
    assert (p1.GetSegmentPath(i) == p2.GetSegmentPath(i));
    assert (p1.GetSegmentPartition(i) == p2.GetSegmentPartition(i));
    assert (p1.GetSegmentModule(i) == p2.GetSegmentModule(i));
    assert (p1.GetSegmentSampling(i) == p2.GetSegmentSampling(i));
  }
}



void testit (const TileCosmicMuon& trans1)
{
  MsgStream log (0, "test");
  TileCosmicMuonCnv_p2 cnv;
  TileCosmicMuon_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileCosmicMuon trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TileCosmicMuon trans1;
  trans1.SetTime (1.5);
  trans1.SetPositionX (2.5);
  trans1.SetPositionY (3.5);
  trans1.SetPositionZ (4.5);
  trans1.SetDirectionPhi (5.5);
  trans1.SetDirectionTheta (6.5);
  trans1.SetFitQuality (7.5);
  trans1.SetFitNCells (8);

  trans1.SetPathTop      (std::vector<double> { 10.5, 11.5, 12.5 });
  trans1.SetPathBottom   (std::vector<double> { 20.5, 21.5, 22.5 });
  trans1.SetEnergyTop    (std::vector<double> { 30.5, 31.5, 32.5 });
  trans1.SetEnergyBottom (std::vector<double> { 40.5, 41.5, 42.5 });

  trans1.SetTrackCellHash (std::vector<IdentifierHash> { 11, 12, 13, 14 });

  trans1.SetSegmentPath (std::vector<double> { 50.5, 51.5 } );
  trans1.SetSegmentPartition (std::vector<int> { 60, 61 } );
  trans1.SetSegmentModule    (std::vector<int> { 70, 71 } );
  trans1.SetSegmentSampling  (std::vector<int> { 80, 81 } );

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
