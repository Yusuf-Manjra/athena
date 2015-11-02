#include "GaudiKernel/DeclareFactoryEntries.h"

#include "PixelDigitization/PixelDigitization.h"
#include "src/PixelDigitizationTool.h"
#include "src/SurfaceChargesTool.h"
#include "src/PixelNoisyCellGenerator.h"
#include "src/PixelGangedMerger.h"
#include "src/SpecialPixelGenerator.h"
#include "src/PixelCellDiscriminator.h"
#include "src/PixelChargeSmearer.h"
#include "src/PixelDiodeCrossTalkGenerator.h"
#include "src/PixelRandomDisabledCellGenerator.h"
#include "src/TimeSvc.h"
#include "src/SubChargesTool.h"
#include "src/PixelBarrelChargeTool.h"
#include "src/PixelECChargeTool.h"
#include "src/DBMChargeTool.h"
#include "src/IblPlanarChargeTool.h"
#include "src/Ibl3DChargeTool.h"
#include "src/SubChargesTool.h"
#include "src/ChargeCollProbSvc.h"
#include "src/CalibSvc.h"

DECLARE_ALGORITHM_FACTORY(PixelDigitization)
DECLARE_TOOL_FACTORY(PixelDigitizationTool)
DECLARE_TOOL_FACTORY(SurfaceChargesTool)
DECLARE_TOOL_FACTORY(PixelNoisyCellGenerator)
DECLARE_TOOL_FACTORY(PixelGangedMerger)
DECLARE_TOOL_FACTORY(SpecialPixelGenerator)
DECLARE_TOOL_FACTORY(PixelCellDiscriminator)
DECLARE_TOOL_FACTORY(PixelCellDiscriminator)
DECLARE_TOOL_FACTORY(PixelChargeSmearer)
DECLARE_TOOL_FACTORY(PixelDiodeCrossTalkGenerator)
DECLARE_TOOL_FACTORY(PixelRandomDisabledCellGenerator)
DECLARE_TOOL_FACTORY(PixelBarrelChargeTool)
DECLARE_TOOL_FACTORY(PixelECChargeTool)
DECLARE_TOOL_FACTORY(DBMChargeTool)
DECLARE_TOOL_FACTORY(IblPlanarChargeTool)
DECLARE_TOOL_FACTORY(Ibl3DChargeTool)
//DECLARE_TOOL_FACTORY(SubChargesTool)
DECLARE_SERVICE_FACTORY(ChargeCollProbSvc)
DECLARE_SERVICE_FACTORY(TimeSvc)
DECLARE_SERVICE_FACTORY(CalibSvc)

DECLARE_FACTORY_ENTRIES(PixelDigitization) {
  DECLARE_ALGORITHM(PixelDigitization)
  DECLARE_TOOL(PixelDigitizationTool)
  DECLARE_TOOL(SurfaceChargesTool)
  DECLARE_TOOL(PixelNoisyCellGenerator)
  DECLARE_TOOL(PixelGangedMerger)
  DECLARE_TOOL(SpecialPixelGenerator)
  DECLARE_TOOL(PixelCellDiscriminator)
  DECLARE_TOOL(PixelChargeSmearer)
  DECLARE_TOOL(PixelDiodeCrossTalkGenerator)
  DECLARE_TOOL(PixelRandomDisabledCellGenerator)
  DECLARE_TOOL(PixelBarrelChargeTool)
  DECLARE_TOOL(PixelECChargeTool)
  DECLARE_TOOL(DBMChargeTool)
  DECLARE_TOOL(IblPlanarChargeTool)
  DECLARE_TOOL(Ibl3DChargeTool)
 // DECLARE_TOOL(SubChargesTool)
  DECLARE_SERVICE(ChargeCollProbSvc)
  DECLARE_SERVICE(TimeSvc)
  DECLARE_SERVICE(CalibSvc)
}

