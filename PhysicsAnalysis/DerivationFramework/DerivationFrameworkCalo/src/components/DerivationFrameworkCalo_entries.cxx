#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkCalo/CaloClusterThinning.h"
#include "DerivationFrameworkCalo/JetCaloClusterThinning.h"
#include "DerivationFrameworkCalo/CellsInConeThinning.h"
#include "DerivationFrameworkCalo/CellDecorator.h"
#include "DerivationFrameworkCalo/MaxCellDecorator.h"

using namespace DerivationFramework;
 
DECLARE_TOOL_FACTORY( CaloClusterThinning )
DECLARE_TOOL_FACTORY( JetCaloClusterThinning )
DECLARE_TOOL_FACTORY( CellsInConeThinning )
DECLARE_TOOL_FACTORY( CellDecorator )
DECLARE_TOOL_FACTORY( MaxCellDecorator )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkCalo ) {
   DECLARE_TOOL( CaloClusterThinning )
   DECLARE_TOOL( JetCaloClusterThinning )
   DECLARE_TOOL( CellsInConeThinning )
   DECLARE_TOOL( CellDecorator )
   DECLARE_TOOL( MaxCellDecorator )
}
 
