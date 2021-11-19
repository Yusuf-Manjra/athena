/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "eFexTauRoIsUnpackingTool.h"
#include "StoreGate/ReadDecorHandle.h"

// =============================================================================
eFexTauRoIsUnpackingTool::eFexTauRoIsUnpackingTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
  : RoIsUnpackingToolBase(type, name, parent) {}

// =============================================================================
StatusCode eFexTauRoIsUnpackingTool::initialize() {
  ATH_CHECK(RoIsUnpackingToolBase::initialize());
  ATH_CHECK(m_l1MenuKey.initialize());
  ATH_CHECK(m_trigRoIsKey.initialize());
  ATH_CHECK(m_thresholdPatternsKey.initialize());
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode eFexTauRoIsUnpackingTool::start() {
  ATH_CHECK(decodeMapping([](const std::string& name){
    return name.find("eTAU")==0 or name.find(getProbeThresholdName("eTAU"))==0;
  }));
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode eFexTauRoIsUnpackingTool::unpack(const EventContext& ctx,
                                            const xAOD::TrigComposite& l1TriggerResult,
                                            const HLT::IDSet& activeChains) const {
  using namespace TrigCompositeUtils;

  // Create and record RoI descriptor and decision containers
  SG::WriteHandle<TrigRoiDescriptorCollection> roiDescriptors = createAndStoreNoAux(m_trigRoIsKey, ctx);
  SG::WriteHandle<DecisionContainer> decisionsMain = createAndStore(m_decisionsKey, ctx);
  SG::WriteHandle<DecisionContainer> decisionsProbe = createAndStore(m_decisionsKeyProbe, ctx);

  // Retrieve the xAOD RoI container from L1TriggerResult
  if (!l1TriggerResult.hasObjectLink(m_eFexTauRoILinkName, ClassID_traits<xAOD::eFexTauRoIContainer>::ID())) {
    ATH_MSG_DEBUG("No eFex Tau RoIs in this event");
    return StatusCode::SUCCESS;
  }
  ElementLink<xAOD::eFexTauRoIContainer> roisLink = l1TriggerResult.objectLink<xAOD::eFexTauRoIContainer>(m_eFexTauRoILinkName);
  ATH_CHECK(roisLink.isValid());
  const xAOD::eFexTauRoIContainer& rois = roisLink.getStorableObjectRef();

  // Create threshold patterns decoration accessor
  auto thrPatternAcc = SG::makeHandle<uint64_t>(m_thresholdPatternsKey, ctx);
  ATH_CHECK(thrPatternAcc.isPresent());

  // Retrieve the L1 menu configuration
  SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey, ctx);
  ATH_CHECK(l1Menu.isValid());
  std::optional<ThrVecRef> thresholds;
  ATH_CHECK(getL1Thresholds(*l1Menu, "eTAU", thresholds));

  size_t linkIndex{0};
  for (const xAOD::eFexTauRoI* roi : rois) {
    // Create new RoI descriptor
    roiDescriptors->push_back(std::make_unique<TrigRoiDescriptor>(
      roi->eta(), roi->eta()-m_roiWidthEta, roi->eta()+m_roiWidthEta,
      roi->phi(), roi->phi()-m_roiWidthPhi, roi->phi()+m_roiWidthPhi
    ));

    // Create new decision and link the RoI objects
    Decision* decisionMain = TrigCompositeUtils::newDecisionIn(decisionsMain.ptr(), hltSeedingNodeName());
    decisionMain->setObjectLink(initialRoIString(),
                                ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), linkIndex, ctx));
    decisionMain->setObjectLink(initialRecRoIString(),
                                ElementLink<xAOD::eFexTauRoIContainer>(m_eFexTauRoILinkName, linkIndex, ctx));

    Decision* decisionProbe = TrigCompositeUtils::newDecisionIn(decisionsProbe.ptr(), hltSeedingNodeName());
    decisionProbe->setObjectLink(initialRoIString(),
                                 ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), linkIndex, ctx));
    decisionProbe->setObjectLink(initialRecRoIString(),
                                 ElementLink<xAOD::eFexTauRoIContainer>(m_eFexTauRoILinkName, linkIndex, ctx));

    // Add positive decisions for chains to be activated by this RoI object
    uint64_t thresholdPattern = thrPatternAcc(*roi);
    ATH_MSG_DEBUG("RoI #" << linkIndex << " threshold pattern: " << thresholdPattern);
    for (const std::shared_ptr<TrigConf::L1Threshold>& thr : thresholds.value().get()) {
      if ((thresholdPattern & (1 << thr->mapping())) == 0u) {continue;}
      const std::string thresholdProbeName = getProbeThresholdName(thr->name());
      ATH_MSG_DEBUG("RoI #" << linkIndex << " passed threshold number " << thr->mapping()
                    << " names " << thr->name() << " and " << thresholdProbeName);
      addChainsToDecision(HLT::Identifier(thr->name()), decisionMain, activeChains);
      addChainsToDecision(HLT::Identifier(thresholdProbeName), decisionProbe, activeChains);
    }

    ++linkIndex;
  }

  return StatusCode::SUCCESS;
}
