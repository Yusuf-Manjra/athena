/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AthLinks/ElementLinkVector.h"
#include "TrigConfHLTUtils/HLTUtils.h"
#include "xAODTrigger/TrigPassBitsContainer.h"
#include "AthenaKernel/ClassID_traits.h"
#include "TrigNavStructure/TriggerElement.h"

#include "Run2ToRun3TrigNavConverterV2.h"

namespace TCU = TrigCompositeUtils;

// helper class 
ConvProxy::ConvProxy(const HLT::TriggerElement* t) : te{ t } {}

void ConvProxy::merge(ConvProxy* other) {
  if ( other == this ) {
    return;
  }
  // copy over chains
  runChains.insert(other->runChains.begin(), other->runChains.end());
  passChains.insert(other->passChains.begin(), other->passChains.end());
  /* the intention of the code below is following.
  Intial structure like is like this (the line is always bidirectional):
  P1 P2 P3 <- parents
   | | /
  T1 T2 <- "this" and "other"
   | |  
  C1 C2 <- children
  1) Lets assume that the first proxies we treat are B1 & B2 ath they are mergable. The resulting structure should look like this:
  P1 P2 P3 
   |/__/
  T1 T2
   |\   
  C1 C2

  */
  auto add = [](ConvProxy* toadd, std::vector<ConvProxy*>& coll) {
    if ( std::find(coll.begin(), coll.end(), toadd) == coll.end() )  {
      coll.push_back(toadd);
    }
  };

  auto remove = [](ConvProxy* torem, std::vector<ConvProxy*>& coll) {
    auto place = std::find(coll.begin(), coll.end(), torem);
    if ( place != coll.end() )  {
      coll.erase(place);
    }
  };


  // this is T <-> C connection
  for ( auto otherChild: other->children ) {
    add( otherChild, children);
    add( this, otherChild->parents);
  }
  // this is T <-> P connection rewiring
  for ( auto otherParent: other->parents ) {
    add(otherParent, parents);
    add(this, otherParent->children);
  }

  // now need to remove links back to the "other"
  for ( auto otherParent : other->parents ) {
    remove(other, otherParent->children);
  }

  for( auto otherChild: other->children ) {
    remove(other, otherChild->parents);
  }
  other->children.clear();
  other->parents.clear();
}





// the algorithm
Run2ToRun3TrigNavConverterV2::Run2ToRun3TrigNavConverterV2(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

Run2ToRun3TrigNavConverterV2::~Run2ToRun3TrigNavConverterV2()
{
}

StatusCode Run2ToRun3TrigNavConverterV2::initialize()
{
  ATH_CHECK(m_trigOutputNavKey.initialize());
  ATH_CHECK(m_tdt.empty() != m_trigNavKey.key().empty()); //either of the two has to be enabled but not both
  if (!m_tdt.empty()) {
    ATH_CHECK(m_tdt.retrieve());
    ATH_MSG_INFO("Will use Trigger Navigation from TrigDecisionTool");
  }
  else {
    ATH_CHECK(m_trigNavKey.initialize(SG::AllowEmpty));
    ATH_MSG_INFO("Will use Trigger Navigation decoded from TrigNavigation object");
  }

  ATH_CHECK(m_configSvc.retrieve());
  ATH_CHECK(m_clidSvc.retrieve());

  // configured collections can be either just type name, or type#key
  // decoding takes this into account, if only the type is configured then empty string is places in the decoded lookup map
  // else CLID + a name is placed
  for (const auto& name : m_collectionsToSave) {
    std::string typeName = name;
    std::string collName;
    size_t delimeterIndex = name.find('#');
    if (delimeterIndex != std::string::npos) {
      ATH_MSG_ERROR("Not support for specific collection name yet");
      return StatusCode::FAILURE;
      typeName = name.substr(0, delimeterIndex);
      collName = name.substr(delimeterIndex+1);
    }
    CLID id{ 0 };
    ATH_CHECK(m_clidSvc->getIDOfTypeName(typeName, id));
    ATH_MSG_DEBUG("Will be linking collection type " << typeName << " name (empty==all) " << collName);
    m_collectionsToSaveDecoded[id].insert(collName);

  }
  // sanity check, i.e. if there is at least one entry w/o the coll name no other enties are needed for a given clid
  for ( auto [clid, keysSet]: m_collectionsToSaveDecoded ) {
    if ( keysSet.size() > 1 and keysSet.count("") != 0 ) {
      ATH_MSG_ERROR("Bad configuration for CLID " << clid << " reuested saving of all (empty coll name configures) collections, yet there are also specific keys");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::execute(const EventContext& context) const
{
  // need to check if this step works in start() or initialize, if so we need to it once
  // and remember this mapping 
  TEIdToChainsMap_t allTEIdsToChains, finalTEIdsToChains;
  ATH_CHECK(extractTECtoChainMapping(allTEIdsToChains, finalTEIdsToChains));

  ConvProxySet_t convProxies;
  HLT::StandaloneNavigation standaloneNav; // needed to keep TEs around
  ATH_CHECK(mirrorTEsStructure(convProxies, standaloneNav, context));

  if (m_doSelfValidation)
    ATH_CHECK(allProxiesConnected(convProxies));

  ATH_CHECK(associateChainsToProxies(convProxies, allTEIdsToChains));
  ATH_CHECK(cureUnassociatedProxies(convProxies));
  ATH_MSG_DEBUG("Proxies to chains mapping done");

  if (not m_chainsToSave.empty()) {
    ATH_CHECK(removeUnassociatedProxies(convProxies));
    ATH_MSG_DEBUG("Removed proxies to chains that are not converted, remaining number of elements " << convProxies.size());
  }
  if (m_doSelfValidation) {
    ATH_CHECK(allProxiesHaveChain(convProxies));
  }
  if (m_doCompression) {
    ATH_CHECK(doCompression(convProxies));
  }
  if (m_doLinkFeatures) {
    ATH_CHECK(fillRelevantFeatures(convProxies));
    ATH_MSG_DEBUG("Features to link found");
  }
  SG::WriteHandle<TrigCompositeUtils::DecisionContainer> outputNavigation = TrigCompositeUtils::createAndStore(m_trigOutputNavKey, context);
  auto decisionOutput = outputNavigation.ptr();
  TrigCompositeUtils::newDecisionIn(decisionOutput, "HLTPassRaw"); // we rely on the fact that the 1st element is the top

  ATH_CHECK(createIMHNodes(convProxies, *decisionOutput));
  if (m_doSelfValidation) {
    ATH_CHECK(numberOfHNodesPerProxyNotExcessive(convProxies));
  }
  ATH_CHECK(createFSNodes(convProxies, *decisionOutput, finalTEIdsToChains));
  ATH_CHECK(linkTopNode(*decisionOutput));
  ATH_MSG_DEBUG("Conversion done, from " << convProxies.size() << " elements to " << decisionOutput->size() << " elements");

  // dispose temporaries
  for (auto proxy : convProxies) {
    delete proxy;
  }

  return StatusCode::SUCCESS;
}




StatusCode Run2ToRun3TrigNavConverterV2::extractTECtoChainMapping(TEIdToChainsMap_t& allTEs, TEIdToChainsMap_t& finalTEs) const {
  // port chains iteration code from previous version
  ATH_MSG_DEBUG("Recognised " << allTEs.size() << " kinds of TEs and among them " << finalTEs.size() << " final types");

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::mirrorTEsStructure(ConvProxySet_t& convProxies, HLT::StandaloneNavigation& standaloneNav, const EventContext& context) const {
  const HLT::TrigNavStructure* run2NavigationPtr = nullptr;

  if (!m_trigNavKey.key().empty()) {
    SG::ReadHandle navReadHandle(m_trigNavKey, context);
    ATH_CHECK(navReadHandle.isValid());
    standaloneNav.deserialize(navReadHandle->serialized());
    run2NavigationPtr = &standaloneNav;
  }
  else {
    run2NavigationPtr = m_tdt->ExperimentalAndExpertMethods().getNavigation();
  }
  const HLT::TrigNavStructure& run2Navigation = *run2NavigationPtr;

  // iterate over the TEs, for each make the ConvProxy and build connections
  std::map<const HLT::TriggerElement*, ConvProxy*> teToProxy;
  ATH_MSG_DEBUG("TrigNavStructure with " << run2Navigation.getAllTEs().size() << " TEs acquired");
  for (auto te : run2Navigation.getAllTEs()) {
    // skip event seed node
    if (HLT::TrigNavStructure::isInitialNode(te)) continue;
    auto proxy = new ConvProxy(te);
    convProxies.insert(proxy);
    // add linking 
    for (auto predecessor : HLT::TrigNavStructure::getDirectPredecessors(te)) {
      ConvProxy* predecessorProxy = teToProxy[predecessor];
      if (predecessorProxy != nullptr) { // because we skip some
        proxy->parents.push_back(predecessorProxy);
        predecessorProxy->children.push_back(proxy);
      }
    }
  }
  ATH_MSG_DEBUG("Created " << convProxies.size() << " proxy objects");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::associateChainsToProxies(ConvProxySet_t&, const TEIdToChainsMap_t&) const {
  // using map chain IDs mapping 

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::cureUnassociatedProxies(ConvProxySet_t& convProxies) const {
  // propagate up (towards L1) chain IDs if they are not in proxies
  // technically each proxy looks at the children proxies and inserts from it all unseen chains
  // procedure is repeated until, no single proxy needs an update (tedious - we may be smarter in future)

  while (true) {
    size_t numberOfUpdates = 0;
    for (auto p : convProxies) {
      for (auto child : p->children) {
        size_t startSize = p->runChains.size();
        p->runChains.insert(std::begin(child->runChains), std::end(child->runChains));

        if (startSize != p->runChains.size()) { // some chain needed to be inserted
          numberOfUpdates++;
          // if update was need, it means set of chains that passed need update as well
          p->passChains.insert(std::begin(child->runChains), std::end(child->runChains));
        }
      }
    }
    ATH_MSG_DEBUG("Needed to propagate chains from " << numberOfUpdates << " child(ren)");
    if (numberOfUpdates == 0) {
      break;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::removeUnassociatedProxies(ConvProxySet_t& convProxies) const {
  // remove proxies that have no chains
  for (auto i = std::begin(convProxies); i != std::end(convProxies);) {
    if ((*i)->runChains.empty()) {
      // TODO we may need to deregister it from it's children & parents
      delete* i;
      i = convProxies.erase(i);
    }
    else {
      ++i;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::doCompression(ConvProxySet_t& convProxies) const {
  ATH_CHECK(fillFEAHashes(convProxies));
  { // new scope in order not to leak the fatures map beyond the moment when the pointers in it are invalidated
    FEAToConvProxySet_t sharedFeaturesMap;
    ATH_CHECK(findSharedFEAHashes(convProxies, sharedFeaturesMap));
    if (m_doSelfValidation) {
      ATH_CHECK(allFEAHashesAreUnique(sharedFeaturesMap));
    }
    ATH_CHECK(collapseConvProxies(convProxies, sharedFeaturesMap));
  }
  ATH_CHECK(collapseFeaturelessProxies(convProxies));
  if (m_doSelfValidation) {
    ATH_CHECK(allProxiesHaveChain(convProxies));
    ATH_CHECK(allProxiesConnected(convProxies));
  }
  ATH_MSG_DEBUG("Compression done");

  return StatusCode::SUCCESS;
}



StatusCode Run2ToRun3TrigNavConverterV2::fillFEAHashes(ConvProxySet_t& convProxies) const {
  // calculate fea hash for each vector<FEA> from te associated to proxy
  for ( auto proxy: convProxies ) {
    proxy->feaHash = feaToHash(proxy->te->getFeatureAccessHelpers());
    ATH_MSG_VERBOSE("TE " << TrigConf::HLTUtils::hash2string(proxy->te->getId()) << " FEA hash " << proxy->feaHash);    
    for ( auto fea : proxy->te->getFeatureAccessHelpers()) {
      ATH_MSG_VERBOSE( "FEA: " << fea);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::findSharedFEAHashes(const ConvProxySet_t& convProxies, FEAToConvProxySet_t& feaToProxyMap) const {
  // ceate feahash -> [te1, te2...] mapping, these tes are candidates for merging/compression
  for ( auto proxy: convProxies ) {
    feaToProxyMap[proxy->feaHash].insert(proxy);
  }
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::collapseConvProxies(ConvProxySet_t& convProxies, FEAToConvProxySet_t& feaToProxyMap) const {
  // for all entries in the FEA to TES map, use the first TE as a "primary" and merge all others to it
  const size_t beforeCount = convProxies.size();
  std::vector<ConvProxy*> todelete;
  for ( auto & [hash, proxies]: feaToProxyMap ) {
    if (proxies.size() > 1 ) {
      for ( auto p: proxies ) {
        if ( p != *(proxies.begin())) {
          (*proxies.begin())->merge(p);
          todelete.push_back(p);
        }
      }
    }
  }
  for ( auto proxy: todelete) {
    convProxies.erase(proxy);
    delete proxy;
  }
  ATH_MSG_DEBUG("Proxies with features collapsing reduces size from " << beforeCount << " to " << convProxies.size());
  // remove from proxies set all elements that are now unassociated (remember to delete after)
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::collapseFeaturelessProxies(ConvProxySet_t&) const {
  // heuristically compres proxies that mirror TEs w/o any feature
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::fillRelevantFeatures(ConvProxySet_t&) const {
  // from all FEAs of the associated TE pick those objects that are to be linked
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::createIMHNodes(ConvProxySet_t&, xAOD::TrigCompositeContainer& decisions) const {
  // create nodes of ne navigation for relevant features
  ATH_MSG_DEBUG("IM & H nodes made, output nav elements " << decisions.size());
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::createFSNodes(const ConvProxySet_t&, xAOD::TrigCompositeContainer& decisions, const TEIdToChainsMap_t&) const {
  // associate terminal nodes to filter nodes, 
  // associate all nodes designated as final one with the filter nodes
  ATH_MSG_DEBUG("FS nodes made, output nav elements " << decisions.size());
  return StatusCode::SUCCESS;
}
StatusCode Run2ToRun3TrigNavConverterV2::linkTopNode(xAOD::TrigCompositeContainer& decisions) const {
  // simply link all filter nodes to the HLTPassRaw (the 1st element)
  ATH_CHECK((*decisions.begin())->name() == "HLTPassRaw");
  return StatusCode::SUCCESS;
}

// does not need to be a method, so kept as local
bool feaToSkip( const HLT::TriggerElement::FeatureAccessHelper& fea) {
  CLID thePassBitsCLID = ClassID_traits<xAOD::TrigPassBits>::ID();
  CLID thePassBitsContCLID = ClassID_traits<xAOD::TrigPassBitsContainer>::ID();
  return fea.getCLID() == thePassBitsCLID or fea.getCLID() == thePassBitsContCLID;
}

uint64_t Run2ToRun3TrigNavConverterV2::feaToHash(const std::vector<HLT::TriggerElement::FeatureAccessHelper>& feaVector) const {
  // FEA vectors hashing

  uint64_t hash = 0;
  for ( auto fea:  feaVector) {
    if ( feaToSkip(fea) ) {
      ATH_MSG_VERBOSE("Skipping TrigPassBits in FEA hash calculation");
      continue;
    }
    uint64_t repr64 = static_cast<uint64_t>(fea.getCLID()) << 32 | static_cast<uint64_t>(fea.getIndex().subTypeIndex())<<24 | (fea.getIndex().objectsBegin() << 16 ^ fea.getIndex().objectsEnd());
    hash ^= repr64;
  }
  return hash;
}

bool Run2ToRun3TrigNavConverterV2::feaEqual(const std::vector<HLT::TriggerElement::FeatureAccessHelper>& a, 
                                            const std::vector<HLT::TriggerElement::FeatureAccessHelper>& b ) const {
  if ( a.size() != b.size() ) return false;
  
  for ( size_t i = 0; i < a.size(); ++i ) {
    if ( feaToSkip(a[i]) and feaToSkip(b[i])) {
        ATH_MSG_VERBOSE("Skipping TrigPassBits in FEA comparison");
        continue;
    }
    if ( not (a[i] == b[i]) ) return false;
  }
  return true;
}


bool Run2ToRun3TrigNavConverterV2::feaToSave(const HLT::TriggerElement::FeatureAccessHelper& fea) const { 
  auto iter = m_collectionsToSaveDecoded.find(fea.getCLID());
  if ( iter == m_collectionsToSaveDecoded.end() ) return false;
  if ( iter->second.count("") ) return true; // recording of all collections of a type is requested
  // TODO complete this implementation with precise collection match (requires access to the NavStructure)
  //
  // the logic would be to call: HLTNavDetails::formatSGKey("HLT", conainerTypeName, *(getHolder(fea)->label());
  //
  return false; 
}

StatusCode Run2ToRun3TrigNavConverterV2::allProxiesHaveChain(const ConvProxySet_t& proxies) const {
  for (auto p : proxies) {
    if (p->runChains.empty()) {
      ATH_MSG_ERROR("Proxy with no chains");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("CHECK OK, no proxies w/o a chain");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::allProxiesConnected(const ConvProxySet_t& proxies) const {
  for (auto p : proxies) {
    if (p->children.empty() and p->parents.empty()) {
      ATH_MSG_ERROR("Orphanted proxy");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("CHECK OK, no orphanted proxies");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::allFEAHashesAreUnique(const FEAToConvProxySet_t& feaToProxies) const {
  for (auto [feaHash, proxies] : feaToProxies) {
    auto first = *proxies.begin();
    for (auto p : proxies) {
      if ( not feaEqual(p->te->getFeatureAccessHelpers(), first->te->getFeatureAccessHelpers())) {
        ATH_MSG_ERROR("Proxies grouped by FEA hash have actually distinct features (specific FEAs are different)");
        return StatusCode::FAILURE;
      }
    }
  }
  ATH_MSG_DEBUG("CHECK OK, FE hashes unique");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::numberOfHNodesPerProxyNotExcessive(const ConvProxySet_t& proxies) const {
  for (auto p : proxies) {
    if (p->hNodes.size() > m_hNodesPerProxyThreshold) {
      ATH_MSG_ERROR("Too many H nodes per proxy");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("CHECK OK, no excessive number of H nodes per proxy");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::noUnconnectedHNodes(const xAOD::TrigCompositeContainer& decisions)const {
  // build map of all links to H nodes from IMs and FS
  std::set<const TrigCompositeUtils::Decision*> linkedHNodes;
  for (auto d : decisions) {
    if (d->name() == "IM" or d->name() == "FS") {
      for (auto el : TCU::getLinkToPrevious(d)) {
        linkedHNodes.insert(*el); // dereferences to bare pointer
      }
    }
  }
  for (auto d : decisions) {
    if (d->name() == "H") {
      if (linkedHNodes.count(d) == 0) {
        ATH_MSG_ERROR("Orphanted H node");
        return StatusCode::FAILURE;
      }
    }
  }
  ATH_MSG_DEBUG("CHECK OK, all H modes are connected");

  return StatusCode::SUCCESS;
}