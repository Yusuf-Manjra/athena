#include "GaudiKernel/DeclareFactoryEntries.h"
#include "BoostedJetTaggers/HEPTopTagger.h"
#include "BoostedJetTaggers/BoostedXbbTagger.h"
#include "BoostedJetTaggers/SmoothedTopTagger.h"
#include "BoostedJetTaggers/SmoothedWZTagger.h"
#include "BoostedJetTaggers/JetQGTagger.h"
#include "BoostedJetTaggers/JetQGTaggerBDT.h"
#include "BoostedJetTaggers/JSSWTopTaggerBDT.h"
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"
#include "BoostedJetTaggers/JSSWTopTaggerANN.h"
#include "BoostedJetTaggers/TopoclusterTopTagger.h"
#include "BoostedJetTaggers/HbbTaggerDNN.h"
#include "BoostedJetTaggers/MassDecoXbbTagger.h"

DECLARE_TOOL_FACTORY(SmoothedWZTagger)
DECLARE_TOOL_FACTORY(SmoothedTopTagger)
DECLARE_TOOL_FACTORY(BoostedXbbTagger)
DECLARE_TOOL_FACTORY(JSSWTopTaggerBDT)
DECLARE_TOOL_FACTORY(JSSWTopTaggerDNN)
DECLARE_TOOL_FACTORY(JSSWTopTaggerANN)
DECLARE_TOOL_FACTORY(TopoclusterTopTagger)
DECLARE_TOOL_FACTORY(HbbTaggerDNN)
DECLARE_TOOL_FACTORY(MassDecoXbbTagger)
DECLARE_NAMESPACE_TOOL_FACTORY(HTTxAOD, HEPTopTagger)
DECLARE_NAMESPACE_TOOL_FACTORY(CP, JetQGTagger)
DECLARE_NAMESPACE_TOOL_FACTORY(CP, JetQGTaggerBDT)

DECLARE_FACTORY_ENTRIES(BoostedJetTaggers) {
  DECLARE_TOOL(SmoothedWZTagger)
  DECLARE_TOOL(SmoothedTopTagger)
  DECLARE_TOOL(BoostedXbbTagger)
  DECLARE_TOOL(JSSWTopTaggerBDT)
  DECLARE_TOOL(JSSWTopTaggerDNN)
  DECLARE_TOOL(JSSWTopTaggerANN)
  DECLARE_TOOL(TopoclusterTopTagger)
  DECLARE_TOOL(HbbTaggerDNN)
  DECLARE_TOOL(MassDecoXbbTagger)
  DECLARE_NAMESPACE_TOOL(HTTxAOD, HEPTopTagger)
  DECLARE_NAMESPACE_TOOL(CP, JetQGTagger)
  DECLARE_NAMESPACE_TOOL(CP, JetQGTaggerBDT)

}
