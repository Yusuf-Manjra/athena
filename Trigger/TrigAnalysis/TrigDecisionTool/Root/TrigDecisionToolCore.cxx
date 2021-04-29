/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : TrigDecisionTool
 *
 * @brief main tool
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 *
 ***********************************************************************************/

#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"

#include "TrigDecisionTool/TrigDecisionToolCore.h"


#ifndef XAOD_STANDALONE // AtheAnalysis or full Athena

Trig::TrigDecisionToolCore::TrigDecisionToolCore()
{
  SG::SlotSpecificObj<Trig::CacheGlobalMemory>* ptr = &m_cacheGlobalMemory;
  m_expertMethods=new ExpertMethods(ptr);
}

Trig::CacheGlobalMemory* Trig::TrigDecisionToolCore::cgm() const { 
  const Trig::CacheGlobalMemory* ptr = m_cacheGlobalMemory.get();
  // A consequence of placing the cache in a slot-specific wrapper
  Trig::CacheGlobalMemory* p = const_cast<Trig::CacheGlobalMemory*>(ptr);
  p->navigation (m_navigation);
  return p;
}

#else // AnalysisBase

Trig::TrigDecisionToolCore::TrigDecisionToolCore()
{
  Trig::CacheGlobalMemory* ptr = &m_cacheGlobalMemory;
  m_expertMethods=new ExpertMethods(ptr);
}

Trig::CacheGlobalMemory* Trig::TrigDecisionToolCore::cgm() const { 
  return const_cast<Trig::CacheGlobalMemory*>(&m_cacheGlobalMemory);
}

#endif


Trig::TrigDecisionToolCore::~TrigDecisionToolCore() {
  delete m_expertMethods;
}


StatusCode Trig::TrigDecisionToolCore::initialize() {
  ChainGroupInitialize();
  return StatusCode::SUCCESS;
}

StatusCode Trig::TrigDecisionToolCore::finalize() {
  return StatusCode::SUCCESS;
}


