#ifndef __JustInitTDTAlgo_hxx__
#define __JustInitTDTAlgo_hxx__
/*
  JustInitTDTAlgo.hxx
*/
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TriggerMenuAnalysis/TrigAccessTool.h"

class JustInitTDTAlgo : public AthAlgorithm {
public:
  JustInitTDTAlgo(const std::string& name, ISvcLocator* svcloc);
  ~JustInitTDTAlgo();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ToolHandle<TrigAccessTool> mTrigAccessTool;
};

#endif // __JustInitTDTAlgo_hxx__
