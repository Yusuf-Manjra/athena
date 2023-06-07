/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaBaseComps/test/AthCommonDataStore_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2023
 * @brief Unit test for AthCommonDataStore (incomplete!).
 */


#undef NDEBUG
#include "AthenaBaseComps/AthCommonDataStore.h"
#include "AthenaBaseComps/AthCommonMsg.h"
#include "TestTools/initGaudi.h"
#include "Gaudi/Algorithm.h"
#include <vector>
#include <cassert>
#include <iostream>
#include <typeinfo>


class RenounceDS
  : public AthCommonMsg<Gaudi::Algorithm>
{
public:
  using Base = AthCommonMsg<Gaudi::Algorithm>;
  using Base::Base;
  virtual void renounce (Gaudi::DataHandle& dh) override {
    std::cout << "RenounceDS::renounce()\n";
    Base::renounce (dh);
  }
};


class TestDS
  : public AthCommonDataStore<RenounceDS>
{
public:
  using Base = AthCommonDataStore<RenounceDS>;
  using Base::Base;
  virtual StatusCode execute (const EventContext&) const override {
    return StatusCode::SUCCESS;
  }

  void test_renounce();
};


class DH1
  : public Gaudi::DataHandle
{
public:
  using Gaudi::DataHandle::DataHandle;
};


class DH2
  : public Gaudi::DataHandle
{
public:
  using Gaudi::DataHandle::DataHandle;
  void renounce() {
    std::cout << "DH2::renounce()\n";
  }
};


void TestDS::test_renounce()
{
  std::cout << "test_renounce\n";

  std::cout << "dh1\n";
  DH1 dh1 (DataObjID ("dh1"));
  declare (dh1);
  renounce (dh1);

  std::cout << "dh2\n";
  DH2 dh2 (DataObjID ("dh2"));
  declare (dh2);
  renounce (dh2);
}


int main()
{
  std::cout << "AthenaBaseComps/AthCommonDataStore_test\n";

  ISvcLocator* svcLoc = nullptr;
  if (!Athena_test::initGaudi (svcLoc))
    return 1;

  TestDS ds ("test_ds", svcLoc);
  ds.test_renounce();

  return 0;
}
