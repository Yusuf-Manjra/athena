// JetSubStructureMomentTools_entries.cxx

#include "JetSubStructureMomentTools/KtDeltaRTool.h"
#include "JetSubStructureMomentTools/NSubjettinessTool.h"
#include "JetSubStructureMomentTools/NSubjettinessRatiosTool.h"
#include "JetSubStructureMomentTools/KTSplittingScaleTool.h"
#include "JetSubStructureMomentTools/DipolarityTool.h"
#include "JetSubStructureMomentTools/AngularityTool.h"
#include "JetSubStructureMomentTools/KtMassDropTool.h"
#include "JetSubStructureMomentTools/PlanarFlowTool.h"
#include "JetSubStructureMomentTools/CenterOfMassShapesTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedRatiosTool.h"
#include "JetSubStructureMomentTools/JetPullTool.h"
#include "JetSubStructureMomentTools/JetChargeTool.h"
#include "JetSubStructureMomentTools/MultiplicitiesTool.h"
#include "JetSubStructureMomentTools/VolatilityTool.h"
#include "JetSubStructureMomentTools/ShowerDeconstructionTool.h"
#include "JetSubStructureMomentTools/SoftDropObservablesTool.h"
#include "JetSubStructureMomentTools/SubjetMakerTool.h"
#include "JetSubStructureMomentTools/SubjetFinderTool.h"
#include "JetSubStructureMomentTools/SubjetRecorderTool.h"
#include "JetSubStructureMomentTools/Validator.h"
#include "JetSubStructureMomentTools/QwTool.h"
#include "JetSubStructureMomentTools/VariableRPseudoJetCalTool.h"
#include "JetSubStructureMomentTools/HEPTopTaggerMomentsTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_TOOL_FACTORY(KtDeltaRTool)
DECLARE_TOOL_FACTORY(NSubjettinessTool)
DECLARE_TOOL_FACTORY(NSubjettinessRatiosTool)
DECLARE_TOOL_FACTORY(KTSplittingScaleTool)
DECLARE_TOOL_FACTORY(DipolarityTool)
DECLARE_TOOL_FACTORY(AngularityTool)
DECLARE_TOOL_FACTORY(KtMassDropTool)
DECLARE_TOOL_FACTORY(PlanarFlowTool)
DECLARE_TOOL_FACTORY(CenterOfMassShapesTool)
DECLARE_TOOL_FACTORY(EnergyCorrelatorTool)
DECLARE_TOOL_FACTORY(EnergyCorrelatorRatiosTool)
DECLARE_TOOL_FACTORY(EnergyCorrelatorGeneralizedTool)
DECLARE_TOOL_FACTORY(EnergyCorrelatorGeneralizedRatiosTool)
DECLARE_TOOL_FACTORY(MultiplicitiesTool)
DECLARE_TOOL_FACTORY(JetPullTool)
DECLARE_TOOL_FACTORY(JetChargeTool)
DECLARE_TOOL_FACTORY(VolatilityTool)
DECLARE_TOOL_FACTORY(ShowerDeconstructionTool)
DECLARE_TOOL_FACTORY(SoftDropObservablesTool)
DECLARE_TOOL_FACTORY(SubjetMakerTool)
DECLARE_TOOL_FACTORY(SubjetFinderTool)
DECLARE_TOOL_FACTORY(SubjetRecorderTool)
DECLARE_TOOL_FACTORY(Validator)
DECLARE_TOOL_FACTORY(QwTool)
DECLARE_TOOL_FACTORY(VariableRPseudoJetCalTool)
DECLARE_TOOL_FACTORY(HEPTopTaggerMomentsTool)

DECLARE_FACTORY_ENTRIES(JetSubStructureMomentTools) {
  DECLARE_TOOL(KtDeltaRTool)
  DECLARE_TOOL(NSubjettinessTool)
  DECLARE_TOOL(NSubjettinessRatiosTool)
  DECLARE_TOOL(KTSplittingScaleTool)
  DECLARE_TOOL(DipolarityTool)
  DECLARE_TOOL(AngularityTool)
  DECLARE_TOOL(KtMassDropTool)
  DECLARE_TOOL(PlanarFlowTool)
  DECLARE_TOOL(CenterOfMassShapesTool)
  DECLARE_TOOL(EnergyCorrelatorTool)
  DECLARE_TOOL(EnergyCorrelatorRatiosTool)
  DECLARE_TOOL(EnergyCorrelatorGeneralizedTool)
  DECLARE_TOOL(EnergyCorrelatorGeneralizedRatiosTool)
  DECLARE_TOOL(MultiplicitiesTool)
  DECLARE_TOOL(JetPullTool)
  DECLARE_TOOL(JetChargeTool)
  DECLARE_TOOL(VolatilityTool)
  DECLARE_TOOL(ShowerDeconstructionTool)
  DECLARE_TOOL(SoftDropObservablesTool)
  DECLARE_TOOL(SubjetMakerTool)
  DECLARE_TOOL(SubjetFinderTool)
  DECLARE_TOOL(SubjetRecorderTool)
  DECLARE_TOOL(Validator)
  DECLARE_TOOL(QwTool)
  DECLARE_TOOL(VariableRPseudoJetCalTool)
  DECLARE_TOOL(HEPTopTaggerMomentsTool)
}

