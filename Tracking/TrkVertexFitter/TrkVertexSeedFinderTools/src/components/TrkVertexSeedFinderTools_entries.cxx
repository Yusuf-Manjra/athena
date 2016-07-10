#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkVertexSeedFinderTools/MCTrueSeedFinder.h"
#include "TrkVertexSeedFinderTools/CrossDistancesSeedFinder.h"
#include "TrkVertexSeedFinderTools/DummySeedFinder.h"
#include "TrkVertexSeedFinderTools/ZScanSeedFinder.h"
#include "TrkVertexSeedFinderTools/ImagingSeedFinder.h"

using namespace Trk ;

DECLARE_TOOL_FACTORY ( MCTrueSeedFinder )
DECLARE_TOOL_FACTORY ( CrossDistancesSeedFinder  )
DECLARE_TOOL_FACTORY ( DummySeedFinder )
DECLARE_TOOL_FACTORY ( ZScanSeedFinder )
DECLARE_TOOL_FACTORY ( ImagingSeedFinder )

DECLARE_FACTORY_ENTRIES( TrkVertexSeedFinderTools )
{
  DECLARE_TOOL( MCTrueSeedFinder );
  DECLARE_TOOL( CrossDistancesSeedFinder );
  DECLARE_TOOL( DummySeedFinder );
  DECLARE_TOOL( ZScanSeedFinder );
  DECLARE_TOOL( ImagingSeedFinder );
}
