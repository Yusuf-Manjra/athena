/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "../src/TLorentzVectorFactory.h"
#include "gtest/gtest.h"

TEST(TLorentzVectoryTest, positiveEta) {
  double eta = 2.1;
  double et = 101.;
  double eps = 0.00001;
  TLorentzVector tl = TLorentzVectorFactory().make(eta, et);
  EXPECT_NEAR(tl.Eta(), eta, eps);
  EXPECT_NEAR(tl.Et(), et, eps);
}

TEST(TLorentzVectoryTest, negativeEta) {
  double eta = -7.0;
  double et = 1.;
  double eps = 0.00001;
  TLorentzVector tl = TLorentzVectorFactory().make(eta, et);
  EXPECT_NEAR(tl.Eta(), eta, eps);
  EXPECT_NEAR(tl.Et(), et, eps);
}


TEST(TLorentzVectoryTest, vals0) {
  double eta = 0.5;
  double et = 100.;
  double eps = 0.00001;
  TLorentzVector tl = TLorentzVectorFactory().make(eta, et);
  EXPECT_NEAR(tl.Eta(), eta, eps);
  EXPECT_NEAR(tl.Et(), et, eps);
}

TEST(TLorentzVectoryTest, thowsOnBadEt) {
  double eta = -7.0;
  double et = 0.;
  EXPECT_THROW(TLorentzVectorFactory().make(eta, et), std::out_of_range);
  et = -1.;
  EXPECT_THROW(TLorentzVectorFactory().make(eta, et), std::out_of_range);
}

