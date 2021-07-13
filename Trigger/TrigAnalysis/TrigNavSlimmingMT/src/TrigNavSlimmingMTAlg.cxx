
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavSlimmingMTAlg.h"

#include "TrigTimeAlgs/TrigTimeStamp.h"

using namespace TrigCompositeUtils;

TrigNavSlimmingMTAlg::TrigNavSlimmingMTAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}


StatusCode TrigNavSlimmingMTAlg::initialize() {
  ATH_CHECK( m_primaryInputCollection.initialize() );
  ATH_CHECK( m_outputCollection.initialize() );
  if (not m_trigDec.empty()) {
    ATH_CHECK( m_trigDec.retrieve() );
    m_trigDec->ExperimentalAndExpertMethods()->enable();
  }
  for (const std::string& output : m_allOutputContainers) {
    if (output == m_primaryInputCollection.key()) {
      continue; // We do want to search for failed nodes in the primary input (it may already be merged)
    }
    // We don't want to search for failed nodes in other possible summary keys, we might read in the 
    // summary collection from another running instance (e.g. an AODSlim alg reading in the output of
    // ESDSlim in a RAWtoALL job).
    m_allOutputContainersSet.insert(output);
  }
  m_allOutputContainersSet.insert(m_outputCollection.key());
  msg() << MSG::INFO << "Initialized. Will *not* inspect the following SG Keys: ";
  for (const std::string& key : m_allOutputContainersSet) {
    msg() << key << " ";
  }
  msg() << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::execute(const EventContext& ctx) const {

  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(m_outputCollection, ctx);

  SG::ReadHandle<DecisionContainer> primaryInputHandle = SG::ReadHandle(m_primaryInputCollection, ctx);
  ATH_CHECK(primaryInputHandle.isValid());

  const Decision* terminusNode = TrigCompositeUtils::getTerminusNode(primaryInputHandle);
  if (!terminusNode) {
    ATH_MSG_ERROR("Unable to locate the HLTPassRaw from the primary input navigation collection, size:" << primaryInputHandle->size());
    return StatusCode::FAILURE;
  }

  // Stage 1. Build a transient representation of the navigation graph.
  TrigTimeStamp stage1;
  NavGraph transientNavGraph;

  // We can optionally only keep data for a given set of chains. An empty set means to keep for all chains.
  DecisionIDContainer chainIDs = {};
  if (not m_chainsFilter.empty()) {
    ATH_CHECK(fillChainIDs(chainIDs));
    ATH_MSG_DEBUG("Supplied " << m_chainsFilter.size() << " chain patterns. This converts to " << chainIDs.size() << " DecisionIDs to be preserved.");
  }
  if (chainIDs.size() == 0) {
    ATH_MSG_DEBUG("chainIDs size is zero. No HLT-chain based filtering of the navigation graph will be performed.");
  }

  std::set<const Decision*> fullyExploredFrom;
  // Note: We use the "internal" version of this call such that we maintain our own cache, 
  // as we may need to call this more than once if keepFailedBranches is ture
  TrigCompositeUtils::recursiveGetDecisionsInternal(terminusNode, 
    nullptr, // 'Coming from' is nullptr for the first call of the recursive function
    transientNavGraph, 
    fullyExploredFrom, 
    chainIDs, 
    /*enforce chainIDs on terminus node*/ true);

  ATH_MSG_DEBUG("Collated nodes from passing paths, now have " << transientNavGraph.nodes() << " nodes with " << transientNavGraph.edges() << "edges");

  // Stage 2. We can optionally include branches through the graph which were never accepted by any chain.
  TrigTimeStamp stage2;
  // These branches do not connect to the terminusNode, so we have to go hunting them explicitly.
  // We need to pass in the evtStore as these nodes can be spread out over numerous collections.
  // Like with the terminus node, we can restrict this search to only nodes which were rejected by certain chains.
  // We also want to restrict the search to exclue the output collections of any other TrigNavSlimminMTAlg instances.\\s
  if (m_keepFailedBranches) {
    std::vector<const Decision*> rejectedNodes = TrigCompositeUtils::getRejectedDecisionNodes(&*evtStore(), chainIDs, m_allOutputContainersSet);
    for (const Decision* rejectedNode : rejectedNodes) {
      // We do *not* enfoce that a member of chainIDs must be present in the starting node (rejectedNode)
      // specifically because we know that at least one of chainIDs was _rejected_ here, but is active in the rejected
      // node's seeds.
      TrigCompositeUtils::recursiveGetDecisionsInternal(rejectedNode, 
        nullptr, // 'Coming from' is nullptr for the first call of the recursive function
        transientNavGraph, 
        fullyExploredFrom, 
        chainIDs, 
        /*enforce chainIDs on terminus node*/ false);
    }
    ATH_MSG_DEBUG("Collated nodes from failing paths, now have " << transientNavGraph.nodes() << " nodes with " << transientNavGraph.edges() << "edges");
  }

  // Stage 3. Walk all paths through the graph. Flag for thinning.
  TrigTimeStamp stage3;
  // Final nodes includes the terminus node, plus any rejected nodes (if these were collated).
  TrigCompositeUtils::recursiveFlagForThinning(transientNavGraph, m_keepOnlyFinalFeatures, m_nodesToDrop);

  // Stage 4. Do the thinning. Re-wire removed nodes as we go.
  TrigTimeStamp stage4;
  const size_t nodesBefore = transientNavGraph.nodes();
  const size_t edgesBefore = transientNavGraph.edges();
  std::set<const Decision*> thinnedInputNodes = transientNavGraph.thin();

  // TODO - thinnedInputNodes will be dropped, these may link to "features", "roi", or other objects in other containers.
  // Need to let the slimming svc know that we no longer need the objects pointed to here, and hence they can be thinned.

  ATH_MSG_DEBUG("Trigger navigation graph thinning going from " << nodesBefore << " nodes with " << edgesBefore << " edges, to "
    << transientNavGraph.nodes() << " nodes with " << transientNavGraph.edges() << " edges");

  if (msg().level() <= MSG::VERBOSE) {
    ATH_MSG_VERBOSE("The navigation graph has been slimmed to the following paths");
    transientNavGraph.printAllPaths(msg(), MSG::VERBOSE);
  }

  // Stage 5. Fill the transientNavGraph structure (with NavGraphNode* nodes) back into an xAOD::DecisionContainer (with xAOD::Decision* nodes).
  TrigTimeStamp stage5;
  IOCacheMap cache; // Used to keep a one-to-one relationship between the const input Decision* and the mutable output Decision*
  // Do the terminus node first - such that it ends up at index 0 of the outputHandle (fast to locate in the future)
  Decision* terminusNodeOut = nullptr;
  ATH_CHECK(inputToOutput(terminusNode, terminusNodeOut, cache, outputHandle, chainIDs, ctx));
  // Don't have to walk the graph here, just iterate through the set of (thinned) nodes.
  // We won't end up with two terminus nodes because of this (it checks that the node hasn't already been processed)
  const std::set<NavGraphNode*> allNodes = transientNavGraph.allNodes();
  for (const NavGraphNode* inputNode : allNodes) {
    Decision* outputNode = nullptr;
    ATH_CHECK(inputToOutput(inputNode->node(), outputNode, cache, outputHandle, chainIDs, ctx));
    // TODO - anything else to do here with outputNode? We cannot hook up its seeding yet, we may not yet have output nodes for all of its seeds.
  }
  // Now we have all of the new nodes in the output collection, can link them all up with their slimmed seeding relationships.
  for (const NavGraphNode* inputNode : allNodes) {
    ATH_CHECK(propagateSeedingRelation(inputNode, cache, ctx));
  }

  if (msg().level() <= MSG::DEBUG) {
    ATH_MSG_DEBUG("Navigation slimming and thinning timings:");
    ATH_MSG_DEBUG("  1. Transient Graph of Passed Nodes = " << stage1.millisecondsDifference(stage2) << " ms");
    ATH_MSG_DEBUG("  2. Transient Graph of Failed Nodes = " << stage2.millisecondsDifference(stage3) << " ms");
    ATH_MSG_DEBUG("  3. Flag Transient Graph For Thinning = " << stage3.millisecondsDifference(stage4) << " ms");
    ATH_MSG_DEBUG("  4. Perform Transient Graph Thinning = " << stage4.millisecondsDifference(stage5) << " ms");
    ATH_MSG_DEBUG("  5. Write xAOD Graph = " << stage5.millisecondsSince() << " ms");
  }

  return StatusCode::SUCCESS;  
}


StatusCode TrigNavSlimmingMTAlg::fillChainIDs(DecisionIDContainer& chainIDs) const {
  for (const std::string& filter : m_chainsFilter) {
    // We do this as filter->chains stage as filter could be a regexp matching a large number of chains
    const Trig::ChainGroup* cg = m_trigDec->getChainGroup(filter);
    std::vector<std::string> chains = cg->getListOfTriggers();
    for (const std::string& chain : chains) {
      const TrigConf::HLTChain* hltChain = m_trigDec->ExperimentalAndExpertMethods()->getChainConfigurationDetails(chain);
      const HLT::Identifier chainID( hltChain->chain_name() );
      chainIDs.insert( chainID.numeric() );
      const std::vector<size_t> legMultiplicites = hltChain->leg_multiplicities();
      if (legMultiplicites.size() == 0) {
        ATH_MSG_ERROR("chain " << chainID << " has invalid configuration, no multiplicity data.");
      } else if (legMultiplicites.size() > 1) {
        // For multi-leg chains, the DecisionIDs are handled per leg.
        // We don't care here exactly how many objects are required per leg, just that there are two-or-more legs
        for (size_t legNumeral = 0; legNumeral < legMultiplicites.size(); ++legNumeral) {
          const HLT::Identifier legID = TrigCompositeUtils::createLegName(chainID, legNumeral);
          chainIDs.insert( legID.numeric() );
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::inputToOutput(
  const TrigCompositeUtils::Decision* input, 
  TrigCompositeUtils::Decision* output,
  IOCacheMap& cache, 
  SG::WriteHandle<DecisionContainer>& outputHandle,
  const DecisionIDContainer& chainIDs,
  const EventContext& ctx) const
{
  IOCacheMap::const_iterator it = cache.find(input);
  if (it != cache.end()) {
    output = it->second;
  } else {
    output = newDecisionIn(outputHandle.ptr(), input, input->name(), ctx);
    ATH_CHECK(propagateLinks(input, output));
    ATH_CHECK(propagateDecisionIDs(input, output, chainIDs));
    cache[input] = output;
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::propagateSeedingRelation(  
  const TrigCompositeUtils::NavGraphNode* inputNode, 
  IOCacheMap& cache,
  const EventContext& ctx) const
{
  const Decision* inputDecision = inputNode->node(); // The incoming Decision objects, with links into the old graph 
  Decision* outputDecision = nullptr; // The outgoing Decision object, without graph links so far
  {
    IOCacheMap::const_iterator it = cache.find(inputDecision);
    ATH_CHECK( it != cache.end() );
    outputDecision = it->second;
  }
  for (const NavGraphNode* seed : inputNode->seeds()) {
    const Decision* inputSeedDecision = seed->node(); // A Decision object the incoming Decision object links to (old graph)
    const Decision* outputSeedDecision = nullptr; // The equivalent Decision Object in the slimmed outgoing graph
    {
      IOCacheMap::const_iterator it = cache.find(inputSeedDecision);
      ATH_CHECK( it != cache.end() );
      outputSeedDecision = it->second;
    }
    // Perform the linking only using nodes from the slimmed output graph
    TrigCompositeUtils::linkToPrevious(outputDecision, outputSeedDecision, ctx);
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::propagateLinks(  
  const TrigCompositeUtils::Decision* input, 
  TrigCompositeUtils::Decision* output) const
{
  // ElementLinks form the edges in the graph

  output->copyAllLinksFrom( input );
  for (const std::string& toRemove : m_edgesToDrop) {
    output->removeObjectLink(toRemove);
    output->removeObjectCollectionLinks(toRemove);
    // TODO - let the slimming svc know that we no longer need these objects
  }

  // Do not propagate "seed" links - TrigNavSlimmingMTAlg will 
  // propagate these following additional logic
  output->removeObjectCollectionLinks( seedString() );

  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::propagateDecisionIDs(  
  const TrigCompositeUtils::Decision* input, 
  TrigCompositeUtils::Decision* output,
  const DecisionIDContainer& chainIDs) const
{

  // Get all DecisionIDs from the const input Decision*
  DecisionIDContainer fromInput;
  decisionIDs(input, fromInput);

  DecisionIDContainer toOutput;

  if (chainIDs.size()) {
    // Applying ChainsFilter to the set of DecisionIDs
    std::set_intersection(fromInput.begin(), fromInput.end(), chainIDs.begin(), chainIDs.end(),
      std::inserter(toOutput, toOutput.begin()));
  } else {
    // Copying all DecisionIDs from input to output
    toOutput.insert(fromInput.begin(), fromInput.end());
  }

  // Set the DecisionIDs into the mutable output Decision* 
  insertDecisionIDs(toOutput, output);

  return StatusCode::SUCCESS;
}
