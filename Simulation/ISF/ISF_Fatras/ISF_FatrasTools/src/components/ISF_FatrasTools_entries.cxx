#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_FatrasTools/TransportTool.h"
#include "ISF_FatrasTools/McMaterialEffectsUpdator.h"
#include "ISF_FatrasTools/HadIntProcessorParametric.h"
#include "ISF_FatrasTools/McEnergyLossUpdator.h"
#include "ISF_FatrasTools/McBetheHeitlerEnergyLossUpdator.h"
#include "ISF_FatrasTools/PhotonConversionTool.h"
#include "ISF_FatrasTools/ProcessSamplingTool.h"
#include "ISF_FatrasTools/PhysicsValidationTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , TransportTool )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , McMaterialEffectsUpdator )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , HadIntProcessorParametric )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , McEnergyLossUpdator )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , McBetheHeitlerEnergyLossUpdator )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , PhotonConversionTool )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , ProcessSamplingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , PhysicsValidationTool )

DECLARE_FACTORY_ENTRIES( ISF_FatrasTools ) {
  DECLARE_NAMESPACE_TOOL( iFatras ,  TransportTool )
}

