#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ISF_Geant4UserActions/FastCaloSimParamAction.h"
#include "../TestBoundariesUserAction.h"

DECLARE_TOOL_FACTORY( FastCaloSimParamAction )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4, TestBoundariesUserAction )

DECLARE_FACTORY_ENTRIES( ISF_Geant4UserActions ) {
  DECLARE_TOOL( FastCaloSimParamAction )
    DECLARE_NAMESPACE_TOOL( iGeant4, TestBoundariesUserAction )

}
