/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H
#define TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H 1

#include <string>
#include <mutex>
#include <memory>
#include <TH2.h>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/LockedHandle.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigger/versions/TrigComposite_v1.h"
#include "DecisionCollectorTool.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/DataStructure.h"

#include "TimeDivider.h"
#include "AthenaKernel/AlgorithmTimer.h"

// Forward declarations
class IIncidentSvc;

/**
 * @class TrigSignatureMoniMT
 * @brief Algorithm implementing monitoring of the HLT decision in the MT framework
 **/
class TrigSignatureMoniMT : public extends<AthReentrantAlgorithm, IIncidentListener>
{ 
 public: 

  TrigSignatureMoniMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  start() override;
  virtual StatusCode  execute( const EventContext& context ) const override;
  virtual StatusCode  stop() override;
  virtual void handle( const Incident& incident ) override;

 private:
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_l1DecisionsKey{ this, "L1Decisions", "L1DecoderSummary", "Chains activated after the L1" };
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_finalDecisionKey{ this, "FinalDecisionKey", "HLTNav_Summary", "Final stage of all decisions" };
  SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{ this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu" };
  SG::ReadHandleKey<TrigConf::L1Menu> m_L1MenuKey{ this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu", "L1 Menu" };

  std::map<unsigned int, int> m_chainIDToBinMap;
  std::map<std::string, int> m_nameToBinMap;
  std::map<unsigned int, std::set<std::string>> m_chainIDToBunchMap;
  std::map<std::string, TrigCompositeUtils::DecisionIDContainer> m_groupToChainMap;
  std::map<std::string, TrigCompositeUtils::DecisionIDContainer> m_streamToChainMap;
  
  ServiceHandle<IIncidentSvc> m_incidentSvc{ this, "IncidentSvc", "IncidentSvc", "Incident service"};
  ServiceHandle<ITHistSvc> m_histSvc{ this, "THistSvc", "THistSvc/THistSvc", "Histogramming svc" };
  Gaudi::Property<std::string> m_bookingPath{ this, "HistPath", "/EXPERT/HLTFramework", "Booking path for the histogram"};

  mutable std::mutex m_bufferMutex;
  mutable LockedHandle<TH2> m_passHistogram;
  mutable LockedHandle<TH2> m_countHistogram;
  mutable LockedHandle<TH2> m_rateBufferHistogram;
  mutable LockedHandle<TH2> m_rateHistogram;
  mutable LockedHandle<TH2> m_bunchHistogram;

  std::unique_ptr<Athena::AlgorithmTimer> m_timer;

  //helper to know when to switch to new interval  
  std::unique_ptr<TimeDivider> m_timeDivider;

  //necessary for asynchronous calling callback function
  Gaudi::Property<unsigned int> m_duration {this, "RateIntegrationDuration", 10, "Integration time for the rate histogram in seconds"};
  Gaudi::Property<unsigned int> m_intervals {this, "RateIntegrationIntervals", 6, "Number of the rate histogram publications"};

  ToolHandleArray<DecisionCollectorTool> m_collectorTools{ this, "CollectorTools", {}, "Tools that collect decisions for steps" };
  
  int nBinsX(SG::ReadHandle<TrigConf::HLTMenu>& ) const;
  int nBunchBinsX(SG::ReadHandle<TrigConf::HLTMenu>& ) const;
  int nBinsY() const;
  int nRateBinsY() const;
  int nBunchBinsY(SG::ReadHandle<TrigConf::L1Menu>& ) const;
  void callback() const;
  void updatePublished(unsigned int duration) const;
  StatusCode initHist(std::unique_ptr<TH2>&, SG::ReadHandle<TrigConf::HLTMenu>&, bool = true);
  StatusCode initBunchHist(std::unique_ptr<TH2>&, SG::ReadHandle<TrigConf::HLTMenu>&, SG::ReadHandle<TrigConf::L1Menu>&);
  StatusCode fillDecisionCount(const std::vector<TrigCompositeUtils::DecisionID>& dc, int row) const;
  StatusCode fillPassEvents(const TrigCompositeUtils::DecisionIDContainer& dc, int row, LockedHandle<TH2>& histogram) const;
  StatusCode fillRate(const TrigCompositeUtils::DecisionIDContainer& dc, int row) const;
  StatusCode fillBunchGroups(const TrigCompositeUtils::DecisionIDContainer&) const;
  StatusCode fillStreamsAndGroups(const std::map<std::string, TrigCompositeUtils::DecisionIDContainer>& map, const TrigCompositeUtils::DecisionIDContainer& dc) const;
};

#endif //> !TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H
