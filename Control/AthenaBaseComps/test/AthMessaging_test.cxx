/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */

/**
 * @file AthenaBaseComps/test/AthMessaging_test.cxx
 * @author Frank Winklmeier
 * @date Aug, 2022
 * @brief Test AthMessaging (run with --perf to measure performance)
 */

#include "AthenaBaseComps/AthMessaging.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "TestTools/initGaudi.h"

#include <chrono>
#include <iostream>


struct MyObj : public AthMessaging {
  /// Constructor using implicit MessageSvc retrieval
  MyObj()                    : AthMessaging("MyObj1")         {}
  /// Constructor with explicit MessageSvc
  MyObj(IMessageSvc* msgSvc) : AthMessaging(msgSvc, "MyObj2") {}

  void print()
  {
    ATH_MSG_WARNING("Hello");
    ATH_MSG_INFO("World");
  }
};


void test(IMessageSvc* msgSvc)
{
  MyObj obj1;
  obj1.print();

  MyObj obj2(msgSvc);
  obj2.print();
}


void perftest (IMessageSvc* msgSvc, unsigned int ntry)
{
  using namespace std::chrono;

  auto start = high_resolution_clock::now();
  for (unsigned int i=0; i < ntry; i++) {
    if (msgSvc) MyObj obj(msgSvc);
    else        MyObj obj;
  }
  auto stop = high_resolution_clock::now();
  auto elapsed = duration_cast<nanoseconds>(stop - start);

  std::cout << "--- " << ntry << " times: " << elapsed.count()/1000 << " us"
            << " (" << elapsed.count() / ntry << " ns per call)" << std::endl;
}


int main (int argc, char** argv)
{
  const unsigned int ntry = 100000;
  bool doPerf = false;
  if (argc >= 2 && strcmp (argv[1], "--perf") == 0) {
    doPerf = true;
  }

  // --------------------------------------------------------------------------------
  std::cout << "--- Test without MessageSvc" << std::endl;
  test(nullptr);

  if (doPerf) {
    Athena::getMessageSvcQuiet = true;
    perftest(nullptr, ntry);
    Athena::getMessageSvcQuiet = false;
  }

  // --------------------------------------------------------------------------------
  ISvcLocator* svcLoc = nullptr;
  IMessageSvc* msgSvc = nullptr;
  if (!Athena_test::initGaudi ("AthMessaging_test.txt", svcLoc)) return 1;
  if (svcLoc->service("MessageSvc", msgSvc).isFailure()) return 1;

  std::cout << "--- Test with MessageSvc" << std::endl;
  test(msgSvc);

  if (doPerf) {
    perftest(msgSvc, ntry);
  }

  return 0;
}
