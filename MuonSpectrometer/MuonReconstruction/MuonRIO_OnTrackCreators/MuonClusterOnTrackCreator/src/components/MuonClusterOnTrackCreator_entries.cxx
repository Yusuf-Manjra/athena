#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonClusterOnTrackCreator.h"
#include "../CscClusterOnTrackCreator.h"
#include "../MMClusterOnTrackCreator.h"

using namespace Muon;

DECLARE_TOOL_FACTORY(MuonClusterOnTrackCreator)
DECLARE_TOOL_FACTORY(CscClusterOnTrackCreator)
DECLARE_TOOL_FACTORY(MMClusterOnTrackCreator)

DECLARE_FACTORY_ENTRIES(MuonClusterOnTrackCreator) {
  DECLARE_TOOL(MuonClusterOnTrackCreator)
  DECLARE_TOOL(CscClusterOnTrackCreator)
  DECLARE_TOOL(MMClusterOnTrackCreator)
}

