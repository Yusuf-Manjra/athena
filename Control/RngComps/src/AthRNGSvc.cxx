/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthRNGSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include <functional>
#include "AtlasCLHEP_RandomGenerators/dSFMTEngine.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/Ranlux64Engine.h"
#include "CLHEP/Random/RanecuEngine.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ConcurrencyFlags.h"

AthRNGSvc::AthRNGSvc(const std::string& name, ISvcLocator* svc)
  : base_class(name, svc),
    m_RNGType("dSFMT"),
    m_numSlots(1),
    m_initialized(false)
{
  declareProperty("EngineType", m_RNGType, "CLHEP RandomEngine type");
  declareProperty("Seeds", m_seeds, "Ignored!");
}

StatusCode AthRNGSvc::initialize()
{
  ATH_CHECK( AthService::initialize() );
  if(m_seeds.size()){
    ATH_MSG_WARNING("Seeds are ignored. A hash composed of event number, " <<
                    "run number and stream/algorithm name is used as a seed");
  }
  if(m_RNGType == "dSFMT"){
    m_fact = [](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::dSFMTEngine();
    };
  }else if(m_RNGType == "Ranlux64"){
    m_fact = [](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::Ranlux64Engine();
    };
  }else if(m_RNGType == "Ranecu"){
    m_fact = [](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::RanecuEngine();
    };
  }else{
    ATH_MSG_WARNING("Supported Generator types are 'dSFMT', 'Ranlux64', 'Ranecu'");
    ATH_MSG_FATAL("Generator type \"" << m_RNGType << "\" is not known. Check Joboptions");
    return StatusCode::FAILURE;
  }
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
  if ( !incSvc.retrieve().isSuccess() ) {
    ATH_MSG_WARNING ("Unable to retrieve the IncidentSvc");
  }
  else {
    incSvc->addListener(this, IncidentType::BeginEvent);
  }

  return StatusCode::SUCCESS;
}

StatusCode AthRNGSvc::start()
{
  m_initialized = true;
  auto numSlots = Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents();
  numSlots = (numSlots < 1) ? 1 : numSlots;
  if(numSlots > 1000){
    ATH_MSG_WARNING("Num Slots are greater than 1000. Is this correct? numSlots=" <<
                    numSlots);
    numSlots = 1000;
    ATH_MSG_WARNING("Setting numSlots to " << numSlots);
  }
  m_numSlots = numSlots;
  return StatusCode::SUCCESS;
}

StatusCode AthRNGSvc::finalize()
{
  return StatusCode::SUCCESS;
}

ATHRNG::RNGWrapper* AthRNGSvc::getEngine(const INamedInterface* client,
                                         const std::string& streamName)
{
  if(!m_initialized){
    ATH_MSG_FATAL("RNGService is not initialized yet! " <<
                  "Number of slots is not known. This will create serious problems!");
  }
  const std::string rngName = client->name() + "/" + streamName;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = m_wrappers.find(rngName);
  if(it == m_wrappers.end()){
    ATH_MSG_INFO("Creating engine " << rngName);
    auto wrp = new ATHRNG::RNGWrapper(m_fact, m_numSlots);
    m_wrappers.insert( std::make_pair(rngName, wrp) );
    return wrp;
  }
  ATH_MSG_WARNING("Returning engine " << rngName);

  return it->second;
}

void AthRNGSvc::handle( const Incident& incident )
{
  const auto &currCtx = incident.context();
  auto currSlot = currCtx.slot();
  if(currSlot == EventContext::INVALID_CONTEXT_ID) currSlot = 0;
  if(incident.type() == IncidentType::BeginEvent){
    std::lock_guard<std::mutex> lock(m_mutex);
    auto evId = currCtx.eventID().event_number();
    auto run = currCtx.eventID().run_number();
    for(const auto &w :m_wrappers){
      w.second->setSeed(w.first, currSlot, evId, run);
    }
  }
}

AthRNGSvc::~AthRNGSvc()
{
  // Clean up the RNGWrappers and HepRandomEngines
  for(auto& wrapperPair : m_wrappers) {
    delete wrapperPair.second;
  }
}
