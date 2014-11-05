// $Id: ElectronIsolationSelection_entries.cxx 605132 2014-07-07 12:58:34Z morrisj $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "ElectronIsolationSelection/ElectronIsolationSelectionTool.h"
#include "ElectronIsolationSelection/ShowerDepthTool.h"


DECLARE_NAMESPACE_TOOL_FACTORY( CP, ElectronIsolationSelectionTool )
DECLARE_NAMESPACE_TOOL_FACTORY( CP, ShowerDepthTool )


DECLARE_FACTORY_ENTRIES( egammaAnalysisUtils ) {

   DECLARE_NAMESPACE_TOOL( CP, ElectronIsolationSelectionTool )
   DECLARE_NAMESPACE_TOOL( CP, ShowerDepthTool )   

}
