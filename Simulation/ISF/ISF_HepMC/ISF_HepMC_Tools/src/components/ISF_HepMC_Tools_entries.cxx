#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../GenEventValidityChecker.h"
#include "../GenEventVertexPositioner.h"
#include "../VertexBeamCondPositioner.h"
#include "../LongBeamspotVertexPositioner.h"
#include "../VertexPositionFromFile.h"
#include "../GenEventStackFiller.h"
#include "../GenParticleGenericFilter.h"
#include "../GenParticleFinalStateFilter.h"
#include "../GenParticlePositionFilter.h"
#include "../GenericTruthStrategy.h"
#include "../CylinderVolumeTruthStrategy.h"
#include "../ValidationTruthStrategy.h"
#include "../GenParticleSimWhiteList.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenEventValidityChecker      )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenEventVertexPositioner     )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , VertexBeamCondPositioner     )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , LongBeamspotVertexPositioner )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , VertexPositionFromFile       )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenEventStackFiller          )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleGenericFilter     )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleFinalStateFilter  )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticlePositionFilter    )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenericTruthStrategy         )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , CylinderVolumeTruthStrategy  )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , ValidationTruthStrategy      )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleSimWhiteList      )

DECLARE_FACTORY_ENTRIES( ISF_HepMC_Tools ) {
  DECLARE_NAMESPACE_TOOL( ISF ,  GenEventValidityChecker      )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenEventVertexPositioner     )
  DECLARE_NAMESPACE_TOOL( ISF ,  VertexBeamCondPositioner     )
  DECLARE_NAMESPACE_TOOL( ISF ,  LongBeamspotVertexPositioner )
  DECLARE_NAMESPACE_TOOL( ISF ,  VertexPositionFromFile       )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenEventStackFiller          )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenParticleGenericFilter     )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenParticleFinalStateFilter  )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenParticlePositionFilter    )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenericTruthStrategy         )
  DECLARE_NAMESPACE_TOOL( ISF ,  CylinderVolumeTruthStrategy  )
  DECLARE_NAMESPACE_TOOL( ISF ,  ValidationTruthStrategy      )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenParticleSimWhiteList      )
}
