#ifndef AthenaMonitoringKernel_test_mocks_MockAlgorithm_h
#define AthenaMonitoringKernel_test_mocks_MockAlgorithm_h

#include "AthenaBaseComps/AthAlgorithm.h"

#include "MockSvcLocator.h"

class MockAlgorithm : public AthAlgorithm {
  public:
    MockAlgorithm() : AthAlgorithm("MockAlgorithm", new MockSvcLocator(), "0.0.1") {}
    StatusCode execute() override { return StatusCode::SUCCESS; }
};

#endif /* AthenaMonitoringKernel_test_mocks_MockAlgorithm_h */