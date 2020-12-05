/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOutputHandling/HLTResultMTMaker.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include <sstream>

// Local helpers
namespace {
  /// Replaced with std::set::contains in C++20
  template<typename T> bool contains(const std::set<T>& set, const T& value) {
    return set.find(value) != set.cend();
  }
  bool isHLT(const uint32_t robid) {
    return eformat::helper::SourceIdentifier(robid).subdetector_id() == eformat::TDAQ_HLT;
  }
  bool isHLT(const eformat::SubDetector subdet) {
    return subdet == eformat::TDAQ_HLT;
  }
  /// Remove disabled IDs from @c requestedIDs and return the set of removed IDs
  template<typename T> std::set<T> removeDisabled(std::set<T>& requestedIds, const std::set<T>& enabledIds) {
    std::set<T> removedIds;
    for (auto it=requestedIds.begin(); it!=requestedIds.end(); ) {
      // Check if ROB/SubDet is disabled (skip HLT result ID)
      if (!isHLT(*it) && !contains(enabledIds, *it)) {
        removedIds.insert(*it);
        it = requestedIds.erase(it);
      }
      else ++it;
    }
    return removedIds;
  }
  /// Print helper for set<uint32_t>
  std::string format(const std::set<uint32_t>& set) {
    std::ostringstream ss;
    for (const uint32_t id : set)
      ss << "0x" << std::hex << std::setfill('0') << std::setw(8) << id << std::dec << " ";
    return ss.str();
  }
  /// Print helper for set<eformat::SubDetector>
  std::string format(const std::set<eformat::SubDetector>& set) {
    std::ostringstream ss;
    for (const eformat::SubDetector id : set) ss << eformat::helper::SubDetectorDictionary.string(id) << " ";
    return ss.str();
  }
}

// =============================================================================
// Standard constructor
// =============================================================================
HLTResultMTMaker::HLTResultMTMaker(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent) {}

// =============================================================================
// Implementation of IStateful::initialize
// =============================================================================
StatusCode HLTResultMTMaker::initialize() {
  ATH_CHECK(m_hltResultWHKey.initialize());
  ATH_CHECK(m_streamTagMaker.retrieve(DisableTool{m_streamTagMaker.name().empty()}));
  ATH_CHECK(m_makerTools.retrieve());
  ATH_CHECK(m_monTool.retrieve());
  ATH_CHECK(m_jobOptionsSvc.retrieve());

  // Initialise the enabled ROBs/SubDets list from DataFlowConfig config and extra properties.
  // DataFlowConfig is a special object used online to hold DF properties passed from TDAQ to HLT as run parameters.
  Gaudi::Property<std::vector<uint32_t>> enabledROBsProp("EnabledROBs",{});
  if (enabledROBsProp.fromString(m_jobOptionsSvc->get("DataFlowConfig.DF_Enabled_ROB_IDs","[]")).isSuccess()) {
    m_enabledROBs.insert(enabledROBsProp.value().begin(), enabledROBsProp.value().end());
    ATH_MSG_DEBUG("Retrieved a list of " << m_enabledROBs.size()
                  << " ROBs from DataFlowConfig.DF_Enabled_ROB_IDs");
  }
  else {
    ATH_MSG_ERROR("Could not parse DataFlowConfig.DF_Enabled_ROB_IDs from JobOptionsSvc");
  }

  Gaudi::Property<std::vector<uint32_t>> enabledSubDetsProp("EnabledSubDets",{});
  if (enabledSubDetsProp.fromString(m_jobOptionsSvc->get("DataFlowConfig.DF_Enabled_SubDet_IDs","[]")).isSuccess()) {
    // Need to convert from uint32_t to eformat::SubDetector representable by uint8_t
    for (const uint32_t id : enabledSubDetsProp.value()) {
      m_enabledSubDets.insert( static_cast<eformat::SubDetector>(id & 0xFF) );
    }
    ATH_MSG_DEBUG("Retrieved a list of " << m_enabledSubDets.size()
                  << " SubDets from DataFlowConfig.DF_Enabled_SubDet_IDs");
  }
  else {
    ATH_MSG_ERROR("Could not parse DataFlowConfig.DF_Enabled_SubDet_IDs from JobOptionsSvc");
  }

  if (m_enabledROBs.empty() && m_enabledSubDets.empty()) {
    m_skipValidatePEBInfo = true;
    ATH_MSG_INFO("The list of enabled ROBs and SubDets was not provided by DataFlowConfig or is empty. "
                 << "StreamTag PEBInfo validation will be skipped.");
  }
  else {
    m_enabledROBs.insert(m_extraEnabledROBs.value().begin(), m_extraEnabledROBs.value().end());
    // Need to convert from uint32_t to eformat::SubDetector representable by uint8_t
    for (const uint32_t id : m_extraEnabledSubDets.value()) {
      m_enabledSubDets.insert( static_cast<eformat::SubDetector>(id & 0xFF) );
    }
    ATH_MSG_INFO("StreamTag PEBInfo will be validated against " << m_enabledROBs.size() << " enabled ROBs and "
                 << m_enabledSubDets.size() << " enabled SubDets");
  }
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IStateful::finalize
// =============================================================================
StatusCode HLTResultMTMaker::finalize() {
  ATH_CHECK(m_monTool.release());
  return StatusCode::SUCCESS;
}

// =============================================================================
// The main method of the tool
// =============================================================================
StatusCode HLTResultMTMaker::makeResult(const EventContext& eventContext) const {
  auto monTime =  Monitored::Timer<std::chrono::duration<float, std::milli>>("TIME_makeResult");

  // Create and record the HLTResultMT object
  auto hltResult = SG::makeHandle(m_hltResultWHKey,eventContext);
  ATH_CHECK( hltResult.record(std::make_unique<HLT::HLTResultMT>()) );
  ATH_MSG_DEBUG("Recorded HLTResultMT with key " << m_hltResultWHKey.key());

  // Fill the stream tags
  StatusCode finalStatus = StatusCode::SUCCESS;
  if (m_streamTagMaker.isEnabled() && m_streamTagMaker->fill(*hltResult, eventContext).isFailure()) {
    ATH_MSG_ERROR(m_streamTagMaker->name() << " failed");
    finalStatus = StatusCode::FAILURE;
  }

  // Fill the result using all other tools if the event was accepted
  if (hltResult->isAccepted()) {
    for (auto& maker: m_makerTools) {
      ATH_MSG_DEBUG("Calling " << maker->name() << " for accepted event");
      if (StatusCode sc = maker->fill(*hltResult, eventContext); sc.isFailure()) {
        ATH_MSG_ERROR(maker->name() << " failed");
        finalStatus = sc;
      }
    }

    if (!m_skipValidatePEBInfo) validatePEBInfo(*hltResult);
  }
  else {
    ATH_MSG_DEBUG("Rejected event, further result filling skipped after stream tag maker");
  }

  ATH_MSG_DEBUG(*hltResult);

  Monitored::Group(m_monTool, monTime);
  return finalStatus;
}

// =============================================================================
// Private method removing disabled ROBs/SubDets from StreamTags
// =============================================================================
void HLTResultMTMaker::validatePEBInfo(HLT::HLTResultMT& hltResult) const {
  for (eformat::helper::StreamTag& st : hltResult.getStreamTagsNonConst()) {
    std::set<uint32_t> removedROBs = removeDisabled(st.robs,m_enabledROBs);
    if (!removedROBs.empty())
      ATH_MSG_WARNING("StreamTag " << st.type << "_" << st.name << " requested disabled ROBs: " << format(removedROBs)
                      << " - these ROBs were removed from the StreamTag by " << name());
    else
      ATH_MSG_VERBOSE("No disabled ROBs were requested by StreamTag " << st.type << "_" << st.name);

    std::set<eformat::SubDetector> removedSubDets = removeDisabled(st.dets,m_enabledSubDets);
    if (!removedSubDets.empty())
      ATH_MSG_WARNING("StreamTag " << st.type << "_" << st.name << " requested disabled SubDets: " << format(removedSubDets)
                      << " - these SubDets were removed from the StreamTag by " << name());
    else
      ATH_MSG_VERBOSE("No disabled SubDets were requested by StreamTag " << st.type << "_" << st.name);
  }
}
