#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkDetDescrTestTools/GeometryAsciiDumper.h"
#include "TrkDetDescrTestTools/GeometryTTreeDumper.h"
#include "TrkDetDescrTestTools/MaterialMapper.h"
#include "TrkDetDescrTestTools/LayerMaterialInspector.h"
#include "TrkDetDescrTestTools/LayerMaterialAnalyser.h"
#include "TrkDetDescrTestTools/RandomSurfaceBuilder.h"
#include "TrkDetDescrTestTools/RecursiveGeometryProcessor.h"
#include "TrkDetDescrTestTools/TrackingVolumeDisplayer.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( GeometryAsciiDumper )
DECLARE_TOOL_FACTORY( GeometryTTreeDumper )
DECLARE_TOOL_FACTORY( MaterialMapper )
DECLARE_TOOL_FACTORY( LayerMaterialInspector )
DECLARE_TOOL_FACTORY( LayerMaterialAnalyser )
DECLARE_TOOL_FACTORY( RandomSurfaceBuilder )
DECLARE_TOOL_FACTORY( RecursiveGeometryProcessor )
DECLARE_TOOL_FACTORY( TrackingVolumeDisplayer )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkDetDescrTestTools )
{
    DECLARE_TOOL( GeometryAsciiDumper )
    DECLARE_TOOL( GeometryTTreeDumper )
    DECLARE_TOOL( MaterialMapper )
    DECLARE_TOOL( LayerMaterialInspector )
    DECLARE_TOOL( LayerMaterialAnalyser )
    DECLARE_TOOL( RandomSurfaceBuilder )
    DECLARE_TOOL( RecursiveGeometryProcessor )    
    DECLARE_TOOL( TrackingVolumeDisplayer )
}


