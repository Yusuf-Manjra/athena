/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigParticleTPCnv/test/TrigEFBphysCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigEFBphysCnv_p1.
 */


#undef NDEBUG
#include "TrigParticleTPCnv/TrigEFBphysCnv_p1.h"
#include "TrigParticle/TrigEFBphysContainer.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigEFBphys& p1,
              const TrigEFBphys& p2)
{
  assert (p1.roiId() == p2.roiId());
  assert (p1.particleType() == p2.particleType());
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.mass() == p2.mass());
  //assert (p1.fitmass() == p2.fitmass());
  assert (-99 == p2.fitmass());
  //assert (p1.fitchi2() == p2.fitchi2());
  assert (-99 == p2.fitchi2());
  //assert (p1.fitndof() == p2.fitndof());
  assert (-99 == p2.fitndof());
  //assert (p1.fitx() == p2.fitx());
  assert (-99 == p2.fitx());
  //assert (p1.fity() == p2.fity());
  assert (-99 == p2.fity());
  //assert (p1.fitz() == p2.fitz());
  assert (-99 == p2.fitz());
  //assert (p1.secondaryDecayLink() == p2.secondaryDecayLink());
  assert (p2.secondaryDecayLink().isDefault());
  //assert (p1.trackVector() == p2.trackVector());
  assert (p2.trackVector().empty());
}


void testit (const TrigEFBphys& trans1)
{
  MsgStream log (0, "test");
  TrigEFBphysCnv_p1 cnv;
  TrigEFBphys_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigEFBphys trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  // Get proxies created outside of leak check.
  ElementLink<TrigEFBphysContainer> foo ("foo", 10);
  ElementLink<Rec::TrackParticleContainer> bar1 ("bar1", 1);
  ElementLink<Rec::TrackParticleContainer> bar2 ("bar2", 2);
  Athena_test::Leakcheck check;

  TrigEFBphys trans1 (123, 2.5, 1.5,
                      TrigEFBphys::BMUMU,
                      3.5,
                      ElementLink<TrigEFBphysContainer> ("foo", 10));
  trans1.fitmass (4.5);
  trans1.fitchi2 (5.5);
  trans1.fitndof (6);
  trans1.fitx (6.5);
  trans1.fity (7.5);
  trans1.fitz (8.5);

  trans1.addTrack (ElementLink<Rec::TrackParticleContainer> ("bar1", 1));
  trans1.addTrack (ElementLink<Rec::TrackParticleContainer> ("bar2", 2));
    
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
