/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_DECISIONUNPACKEREVENTINFO_H
#define TRIG_DECISIONUNPACKEREVENTINFO_H

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/Lvl1Item.h"

#include "TrigDecisionTool/IDecisionUnpacker.h"
#include "TrigDecisionTool/Logger.h"

#include "DecisionObjectHandleEventInfo.h"

#include <memory>

namespace HLT {
  class TrigNavStructure;
}

namespace LVL1CTP{
  class Lvl1Result;
}

namespace Trig{  
  

  class DecisionUnpackerEventInfo : public IDecisionUnpacker, public Logger {
  public:
    DecisionUnpackerEventInfo(SG::ReadHandleKey<EventInfo>* oldEventInfoKey);
    virtual ~DecisionUnpackerEventInfo();

    DecisionUnpackerEventInfo (const DecisionUnpackerEventInfo&) = delete;
    DecisionUnpackerEventInfo& operator= (const DecisionUnpackerEventInfo&) = delete;

    virtual StatusCode unpackDecision(std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>&,
				      std::map<CTPID, LVL1CTP::Lvl1Item>&,
				      std::unordered_map<std::string, const HLT::Chain*>&,
				      std::map<CHAIN_COUNTER, HLT::Chain>&,
				      std::unordered_map<std::string, const HLT::Chain*>&,
				      std::map<CHAIN_COUNTER, HLT::Chain>&,
				      char&,
				      bool
				    );
    virtual StatusCode unpackNavigation(HLT::TrigNavStructure*);
    virtual bool assert_handle();
    virtual void validate_handle();
    virtual void invalidate_handle();
  private:
    std::unique_ptr<DecisionObjectHandleEventInfo> m_handle;
    StatusCode unpackItems(const std::vector<uint32_t>& level1TriggerInfo,
			   std::map<CTPID, LVL1CTP::Lvl1Item>& itemsCache,
			   std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>& itemsByName);
    StatusCode unpackChains(const std::vector<uint32_t>& chainTriggerInfo,
			    std::map<unsigned, HLT::Chain>& cache,
			    std::unordered_map<std::string, const HLT::Chain*>& output);

  };


} //end of Trig namespace

#endif
