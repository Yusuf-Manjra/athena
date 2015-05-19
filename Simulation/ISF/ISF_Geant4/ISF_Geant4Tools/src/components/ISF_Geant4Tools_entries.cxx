#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TransportTool.h"
#include "../TrackProcessorUserAction.h"
#include "../SDActivateUserAction.h"
#include "../MCTruthUserAction.h"
#include "../PhysicsValidationUserAction.h"
#include "../G4RunManagerHelper.h"

//From Gaudi v25r3 onwards it is very important that the
//G4TransportTool is first in the list of tools to be created when
//arranged alphabetically, as this now determines the order in which
//genConf is run.
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4 , G4TransportTool            )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4 , TrackProcessorUserAction )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4 , SDActivateUserAction )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4 , MCTruthUserAction        )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4 , PhysicsValidationUserAction        )
DECLARE_NAMESPACE_TOOL_FACTORY( iGeant4 , G4RunManagerHelper )

DECLARE_FACTORY_ENTRIES( ISF_Geant4Tools ) {
  DECLARE_NAMESPACE_TOOL ( iGeant4 , G4TransportTool            )
  DECLARE_NAMESPACE_TOOL ( iGeant4 , TrackProcessorUserAction )
  DECLARE_NAMESPACE_TOOL ( iGeant4 , SDActivateUserAction )
  DECLARE_NAMESPACE_TOOL ( iGeant4 , MCTruthUserAction        )
  DECLARE_NAMESPACE_TOOL ( iGeant4 , PhysicsValidationUserAction        )
  DECLARE_NAMESPACE_TOOL ( iGeant4 , G4RunManagerHelper )
}
