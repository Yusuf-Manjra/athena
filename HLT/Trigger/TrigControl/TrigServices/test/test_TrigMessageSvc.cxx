#include "../src/TrigMessageSvc.h"
#include "TestTools/initGaudi.h"

#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"

#include <random>
#include <chrono>
#include <cassert>

#undef NDEBUG

constexpr size_t numSources = 1000;
constexpr size_t numMessages = 50000;

int main() {
  using namespace std;
  ISvcLocator* pSvcLoc;
  assert(Athena_test::initGaudi(pSvcLoc));
  
  //--------------------------------------------------
  // Create THistSvc and TrigMessageSvc
  //--------------------------------------------------
  IProperty* hsvcprop{nullptr};
  assert(pSvcLoc->service("THistSvc","THistSvc",hsvcprop,true).isSuccess());
  assert(hsvcprop->setProperty("Output","[\"EXPERT DATAFILE='expert-monitoring.root' OPT='RECREATE'\"]").isSuccess());

  IService* hsvc{nullptr};
  assert(pSvcLoc->service("THistSvc","THistSvc",hsvc,true).isSuccess());

  TrigMessageSvc msgsvc("MessageSvc", pSvcLoc);

  //--------------------------------------------------
  // Initialise and start the services
  //--------------------------------------------------
  assert(msgsvc.initialize().isSuccess());

  assert(hsvc->start().isSuccess());
  assert(msgsvc.start().isSuccess());

  //--------------------------------------------------
  // Prepare the benchmark
  //--------------------------------------------------
  std::vector<std::string> names;
  names.reserve(numSources);
  for (size_t i=1; i<=numSources; ++i) {
    names.push_back(std::string("TestMsgSource").append(std::to_string(i)));
  }

  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<size_t> distribution(0, numSources-1);

  //--------------------------------------------------
  // Run the benchmark
  //--------------------------------------------------
  auto t0 = std::chrono::high_resolution_clock::now();

  for (size_t i=0; i<numMessages; ++i) {
    size_t j = distribution(generator);
    msgsvc.reportMessage(names[j], 3, "This is a test message");
  }

  auto t1 = std::chrono::high_resolution_clock::now();
  auto td = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
  msgsvc.reportMessage("Benchmark", 3, std::string("Time: ").append(std::to_string(td.count()).append(" ms")));
  assert(td.count() < 3000);

  //--------------------------------------------------
  // Stop and finalise the services
  //--------------------------------------------------
  assert(msgsvc.stop().isSuccess());
  assert(hsvc->stop().isSuccess());

  assert(msgsvc.finalize().isSuccess());
  assert(hsvc->finalize().isSuccess());

  return 0;
}
