// Framework
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local
#include "../DecoratePromptLepton.h"
#include "../DecoratePromptLeptonImproved.h"
#include "../DecoratePromptLeptonRNN.h"
#include "../NonPromptLeptonVertexingAlg.h"
#include "../PrimaryVertexReFitter.h"
#include "../VertexMergingTool.h"
#include "../VertexIterativeFitMergingTool.h"
#include "../RNNTool.h"
#include "../VertexFittingSvc.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Prompt, DecoratePromptLepton )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Prompt, NonPromptLeptonVertexingAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Prompt, PrimaryVertexReFitter )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Prompt, DecoratePromptLeptonImproved )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Prompt, DecoratePromptLeptonRNN )

DECLARE_NAMESPACE_TOOL_FACTORY   ( Prompt, VertexMergingTool )
DECLARE_NAMESPACE_TOOL_FACTORY   ( Prompt, VertexIterativeFitMergingTool )
DECLARE_NAMESPACE_TOOL_FACTORY   ( Prompt, RNNTool )
DECLARE_NAMESPACE_SERVICE_FACTORY( Prompt, VertexFittingSvc )

DECLARE_FACTORY_ENTRIES( LeptonTaggers )
{
  DECLARE_NAMESPACE_ALGORITHM( Prompt, DecoratePromptLepton )
  DECLARE_NAMESPACE_ALGORITHM( Prompt, NonPromptLeptonVertexingAlg )
  DECLARE_NAMESPACE_ALGORITHM( Prompt, PrimaryVertexReFitter )
  DECLARE_NAMESPACE_ALGORITHM( Prompt, DecoratePromptLeptonImproved )
  DECLARE_NAMESPACE_ALGORITHM( Prompt, DecoratePromptLeptonRNN )

  DECLARE_NAMESPACE_TOOL   ( Prompt, VertexMergingTool )
  DECLARE_NAMESPACE_TOOL   ( Prompt, VertexIterativeFitMergingTool )
  DECLARE_NAMESPACE_TOOL   ( Prompt, RNNTool )
  DECLARE_NAMESPACE_SERVICE( Prompt, VertexFittingSvc )
}

