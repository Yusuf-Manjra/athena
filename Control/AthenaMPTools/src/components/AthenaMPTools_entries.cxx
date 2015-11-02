#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../SharedEvtQueueProvider.h"
#include "../SharedEvtQueueConsumer.h"
#include "../SharedReaderTool.h"
#include "../FileSchedulingTool.h"
#include "../TokenScatterer.h"
#include "../TokenProcessor.h"

DECLARE_TOOL_FACTORY( SharedEvtQueueProvider )
DECLARE_TOOL_FACTORY( SharedEvtQueueConsumer )
DECLARE_TOOL_FACTORY( SharedReaderTool )
DECLARE_TOOL_FACTORY( FileSchedulingTool )
DECLARE_TOOL_FACTORY( TokenScatterer )
DECLARE_TOOL_FACTORY( TokenProcessor )

// Much ugliness to only build SharedHiveEvtQueueConsumer for Hive builds

#ifdef ATHENAHIVE

#include "../SharedHiveEvtQueueConsumer.h"
DECLARE_TOOL_FACTORY( SharedHiveEvtQueueConsumer )

DECLARE_FACTORY_ENTRIES ( AthenaMPTools ) {
  DECLARE_TOOL( SharedEvtQueueProvider );
  DECLARE_TOOL( SharedEvtQueueConsumer );
  DECLARE_TOOL( SharedHiveEvtQueueConsumer );
  DECLARE_TOOL( SharedReaderTool );
  DECLARE_TOOL( FileSchedulingTool );
  DECLARE_TOOL( TokenScatterer );
  DECLARE_TOOL( TokenProcessor );
}

#else

DECLARE_FACTORY_ENTRIES ( AthenaMPTools ) {
  DECLARE_TOOL( SharedEvtQueueProvider );
  DECLARE_TOOL( SharedEvtQueueConsumer );
  DECLARE_TOOL( SharedReaderTool );
  DECLARE_TOOL( FileSchedulingTool );
  DECLARE_TOOL( TokenScatterer );
  DECLARE_TOOL( TokenProcessor );
}

#endif


