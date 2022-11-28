#include "PFlowUtils/WeightPFOTool.h"

#if !defined(XAOD_ANALYSIS)
#include "../CombinePFO.h"
#include "../PFlowCalibPFODecoratorAlgorithm.h"
#endif

DECLARE_COMPONENT( CP::WeightPFOTool )

#if !defined(XAOD_ANALYSIS)
DECLARE_COMPONENT( CombinePFO )
DECLARE_COMPONENT( PFlowCalibPFODecoratorAlgorithm )
#endif
