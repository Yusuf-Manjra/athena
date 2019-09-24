/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include <mutex>
#include <algorithm>

#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/HistogramDef.h"

#include "HistogramFiller/HistogramFillerFactory.h"

using namespace Monitored;

GenericMonitoringTool::GenericMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent)
  : AthAlgTool(type, name, parent) { }

GenericMonitoringTool::~GenericMonitoringTool() { }

StatusCode GenericMonitoringTool::initialize() {
  ATH_CHECK(m_histSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode GenericMonitoringTool::start() {
  if ( not m_explicitBooking ) {
    ATH_MSG_DEBUG("Proceeding to histogram booking");
    return book();
  }
  return StatusCode::SUCCESS;
}

StatusCode GenericMonitoringTool::stop() {
  m_fillers.clear();
  return StatusCode::SUCCESS;
}

StatusCode GenericMonitoringTool::book() {

  // If no histogram path given use parent or our own name
  if (m_histoPath.empty()) {
    auto named = dynamic_cast<const INamedInterface*>(parent());
    m_histoPath = named ? named->name() : name();
  }

  // Replace dot (e.g. MyAlg.MyTool) with slash to create sub-directory
  std::replace( m_histoPath.begin(), m_histoPath.end(), '.', '/' );

  ATH_MSG_DEBUG("Booking histograms in path: " << m_histoPath.value());

  HistogramFillerFactory factory(this, m_histoPath);

  m_fillers.reserve(m_histograms.size());
  for (const std::string& item : m_histograms) {
    if (item.empty()) {
      ATH_MSG_DEBUG( "Skipping empty histogram definition" );
      continue;
    }
    ATH_MSG_DEBUG( "Configuring monitoring for: " << item );
    HistogramDef def = HistogramDef::parse(item);

    if (def.ok) {
      std::shared_ptr<HistogramFiller> filler(factory.create(def));

      if (filler) {
        m_fillers.push_back(filler);
      } else {
        ATH_MSG_WARNING( "The histogram filler cannot be instantiated for: " << def.name );
      }
    } else {
      ATH_MSG_ERROR( "Unparsable histogram definition: " << item );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Monitoring for variable " << def.name << " prepared" );
  }

  if ( m_fillers.empty() ) {
    std::string hists;
    for (const auto &h : m_histograms) hists += (h+",");
    ATH_MSG_ERROR("No monitored variables created based on histogram definition: [" << hists <<
                  "] Remove this monitoring tool or check its configuration.");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

namespace Monitored {
    std::ostream& operator<< ( std::ostream& os, const std::reference_wrapper<Monitored::IMonitoredVariable>& rmv ) {
        std::string s = rmv.get().name();
        return os << s;
    }
}

std::vector<std::shared_ptr<HistogramFiller>> GenericMonitoringTool::getHistogramsFillers(std::vector<std::reference_wrapper<IMonitoredVariable>> monitoredVariables) const {
  std::vector<std::shared_ptr<HistogramFiller>> result;

  for (auto filler : m_fillers) {
    // Find the associated monitored variable for each histogram's variable(s)
    auto fillerVariables = filler->histogramVariablesNames();

    std::vector<std::reference_wrapper<IMonitoredVariable>> variables;

    for (auto fillerVariable : fillerVariables) {
      for (auto monValue : monitoredVariables) {
        if (fillerVariable.compare(monValue.get().name()) == 0) {
          variables.push_back(monValue);
          break;
        }
      }
    }

    // Find the weight variable in the list of monitored variables
    auto fillerWeight = filler->histogramWeightName();
    Monitored::IMonitoredVariable* weight = nullptr;
    if ( fillerWeight != "" ) {
      for (auto monValue : monitoredVariables) {
        if (fillerWeight.compare(monValue.get().name()) == 0) {
          weight = &monValue.get();
          break;
        }
      }
    }

    if (fillerVariables.size() != variables.size()) {
      ATH_MSG_DEBUG("Filler has different variables than monitoredVariables");
      ATH_MSG_DEBUG("Filler variables            : " << fillerVariables);
      ATH_MSG_DEBUG("Asked to fill from mon. vars: " << monitoredVariables);
      ATH_MSG_DEBUG("Selected monitored variables: " << variables);
      continue;
    }

    std::shared_ptr<HistogramFiller> fillerCopy(filler->clone());
    fillerCopy->setMonitoredVariables(variables);
    fillerCopy->setMonitoredWeight(weight);
    result.push_back(fillerCopy);
  }

  return result;
}

uint32_t GenericMonitoringTool::runNumber() {
  return Gaudi::Hive::currentContext().eventID().run_number();
}

uint32_t GenericMonitoringTool::lumiBlock() {
  return Gaudi::Hive::currentContext().eventID().lumi_block();
}
