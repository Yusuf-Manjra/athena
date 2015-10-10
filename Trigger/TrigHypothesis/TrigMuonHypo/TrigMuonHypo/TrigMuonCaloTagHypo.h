/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUCALOTAGHYPO_H
#define TRIG_TRIGMUCALOTAGHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuonCaloTagHypo : public HLT::HypoAlgo
{
public:
  TrigMuonCaloTagHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigMuonCaloTagHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  HLT::ErrorCode hltFinalize();

private:
  bool m_acceptAll;
  StoreGateSvc* m_storeGate;
  
  float m_ptMin;
  float m_etaMax;
  bool m_doTight;
  int m_maxMissedCells;

  std::string m_ctTrackContName;

};

#endif
