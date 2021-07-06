/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "MURoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "StoreGate/ReadDecorHandle.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
MURoIsUnpackingTool::MURoIsUnpackingTool( const std::string& type, 
            const std::string& name, 
            const IInterface* parent )
  : RoIsUnpackingToolBase  ( type, name, parent ),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name )
{
}


StatusCode MURoIsUnpackingTool::initialize() {

  CHECK( RoIsUnpackingToolBase::initialize() );
  CHECK( m_configSvc.retrieve() );
  CHECK( m_trigRoIsKey.initialize() );
  CHECK( m_recRoIsKey.initialize(SG::AllowEmpty) );
  CHECK( m_thresholdPatternsKey.initialize(!m_thresholdPatternsKey.empty()) );
  CHECK( m_recRpcRoITool.retrieve() );
  CHECK( m_recTgcRoITool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode MURoIsUnpackingTool::start() {
  ATH_CHECK( decodeMapping( [](const std::string& name ){ return name.find("MU") == 0 or name.find(getProbeThresholdName("MU")) == 0;  } ) );
  return StatusCode::SUCCESS;
}


StatusCode MURoIsUnpackingTool::updateConfiguration() {
  using namespace TrigConf;
  const ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
  for ( TriggerThreshold * th : thresholdConfig->getThresholdVector( L1DataDef::MUON ) ) {
    if ( th != nullptr ) {
      ATH_MSG_DEBUG( "Found threshold in the configuration: " << th->name() << " of ID: " << HLT::Identifier( th->name() ).numeric() ); 
      m_muonThresholds.push_back( th );    
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode MURoIsUnpackingTool::unpack( const EventContext& ctx,
                                        const ROIB::RoIBResult& roib,
                                        const HLT::IDSet& activeChains ) const {
  using namespace TrigCompositeUtils;
  // create and record the collections needed
  SG::WriteHandle<TrigRoiDescriptorCollection> handle1 = createAndStoreNoAux(m_trigRoIsKey, ctx ); 
  auto trigRoIs = handle1.ptr();
  SG::WriteHandle< DataVector<LVL1::RecMuonRoI> > handle2 = createAndStoreNoAux( m_recRoIsKey, ctx );
  auto recRoIs = handle2.ptr();
  SG::WriteHandle<DecisionContainer> handle3 = createAndStore(m_decisionsKey, ctx ); 
  auto decisionOutput = handle3.ptr();
  SG::WriteHandle<DecisionContainer> handle4 = createAndStore(m_decisionsKeyProbe, ctx ); 
  auto decisionOutputProbe = handle4.ptr();

  for ( auto& roi : roib.muCTPIResult().roIVec() ) {    
    const uint32_t roIWord = roi.roIWord();
    int thresholdNumber = roi.pt();
    ATH_MSG_DEBUG( "MUON RoI with the threshold number: " << thresholdNumber );
    if ( thresholdNumber < 1 or thresholdNumber > 6 ) {
      ATH_MSG_WARNING( "Incorrect threshold number, should be between 1 and 6 but is: "
           << thresholdNumber << ", force setting it to 1" );
      thresholdNumber = 1;
    }
    LVL1::RecMuonRoI* recRoI = new LVL1::RecMuonRoI( roIWord, m_recRpcRoITool.get(), m_recTgcRoITool.get(), &m_muonThresholds );
    recRoIs->push_back( recRoI );
    auto trigRoI = new TrigRoiDescriptor( roIWord, 0u ,0u,
            recRoI->eta(), recRoI->eta()-m_roIWidth, recRoI->eta()+m_roIWidth,
            recRoI->phi(), recRoI->phi()-m_roIWidth, recRoI->phi()+m_roIWidth );
    trigRoIs->push_back( trigRoI );
    
    ATH_MSG_DEBUG( "RoI word: 0x" << MSG::hex << std::setw( 8 ) << roIWord );
    
    // The l1DecoderNodeName() denotes an initial node with no parents
    Decision* decisionMain  = TrigCompositeUtils::newDecisionIn( decisionOutput, l1DecoderNodeName() ); 
    decisionMain->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
    decisionMain->setObjectLink( initialRecRoIString(), ElementLink<DataVector<LVL1::RecMuonRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );

    Decision* decisionProbe  = TrigCompositeUtils::newDecisionIn( decisionOutputProbe, l1DecoderNodeName() ); 
    decisionProbe->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
    decisionProbe->setObjectLink( initialRecRoIString(), ElementLink<DataVector<LVL1::RecMuonRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );
    
    for ( auto th: m_muonThresholds ) {
      if ( th->thresholdNumber() < thresholdNumber )  { 
      //th->thresholdNumber() is defined to be [0,5] and thresholdNumber [0,6]
      const std::string thresholdProbeName = getProbeThresholdName(th->name());
      ATH_MSG_DEBUG( "Passed Threshold names " << th->name() << " and " << thresholdProbeName);
      addChainsToDecision( HLT::Identifier( th->name() ), decisionMain, activeChains );
      addChainsToDecision( HLT::Identifier(thresholdProbeName ), decisionProbe, activeChains );
      }
    }

  }
  // monitoring
  {
    auto RoIsCount = Monitored::Scalar( "count", trigRoIs->size() );
    auto RoIsPhi   = Monitored::Collection( "phi", *trigRoIs, &TrigRoiDescriptor::phi );
    auto RoIsEta   = Monitored::Collection( "eta", *trigRoIs, &TrigRoiDescriptor::eta );
    Monitored::Group( m_monTool,  RoIsCount, RoIsEta, RoIsPhi );
  }
  return StatusCode::SUCCESS;
}


StatusCode MURoIsUnpackingTool::unpack(const EventContext& ctx,
                                       const xAOD::TrigComposite& l1TriggerResult,
                                       const HLT::IDSet& activeChains) const {
  // Retrieve the xAOD RoI container from L1TriggerResult
  if (!l1TriggerResult.hasObjectLink(m_muRoILinkName, ClassID_traits<xAOD::MuonRoIContainer>::ID())) {
    ATH_MSG_DEBUG("No muon RoIs in this event");
    return StatusCode::SUCCESS;
  }
  ElementLink<xAOD::MuonRoIContainer> roisLink = l1TriggerResult.objectLink<xAOD::MuonRoIContainer>(m_muRoILinkName);
  ATH_CHECK(roisLink.isValid());
  const xAOD::MuonRoIContainer& rois = roisLink.getStorableObjectRef();

  // Create threshold patterns decoration accessor
  auto thrPatternAcc = SG::makeHandle<uint64_t>(m_thresholdPatternsKey, ctx);

  // Create and record RoI descriptor and decision containers
  using namespace TrigCompositeUtils;
  SG::WriteHandle<TrigRoiDescriptorCollection> roiDescriptors = createAndStoreNoAux(m_trigRoIsKey, ctx);
  SG::WriteHandle<DecisionContainer> decisions = createAndStore(m_decisionsKey, ctx);

  size_t linkIndex{0};
  for (const xAOD::MuonRoI* roi : rois) {
    // Create new RoI descriptor
    roiDescriptors->push_back(std::make_unique<TrigRoiDescriptor>(
      roi->roiWord(), 0u, 0u,
      roi->eta(), roi->eta()-m_roIWidth, roi->eta()+m_roIWidth,
      roi->phi(), roi->phi()-m_roIWidth, roi->phi()+m_roIWidth
    ));

    // Create new decision and link the RoI objects
    Decision* decision = TrigCompositeUtils::newDecisionIn(decisions.ptr(), l1DecoderNodeName());
    decision->setObjectLink(initialRoIString(),
                            ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), linkIndex, ctx));
    decision->setObjectLink(initialRecRoIString(),
                            ElementLink<xAOD::MuonRoIContainer>(m_muRoILinkName, linkIndex, ctx));

    // Add positive decisions for chains above the threshold
    uint64_t thresholdPattern = thrPatternAcc(*roi);
    ATH_MSG_DEBUG("RoI #" << linkIndex << " threshold pattern: " << thresholdPattern);
    for (const TrigConf::TriggerThreshold* thr : m_muonThresholds) {
      if (not (thresholdPattern & (1 << thr->mapping()))) {continue;}
      ATH_MSG_DEBUG("RoI #" << linkIndex << " passed threshold number " << thr->thresholdNumber()
                    << " name " << thr->name());
      addChainsToDecision(HLT::Identifier(thr->name()), decision, activeChains);
      if (msgLvl(MSG::DEBUG)) {
        DecisionIDContainer ids;
        decisionIDs(decision, ids);
        ATH_MSG_DEBUG("Activated chains: " << std::vector<DecisionID>(ids.begin(), ids.end()));
      }
    }

    ++linkIndex;
  }
  return StatusCode::SUCCESS;
}
