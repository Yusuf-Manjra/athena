/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************************
//
// NAME:     TrigCaloRatioHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigLongLivedParticlesHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
//
// ************************************************************

#ifndef TRIGCALORATIOHYPO_H
#define TRIGCALORATIOHYPO_H

#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;


class TrigCaloRatioHypo : public HLT::HypoAlgo {

 public:

  TrigCaloRatioHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigCaloRatioHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);


 private:

  /** @brief cut on eT. */
  double m_etCut;
  /** @brief cut on Log(H/E). */
  double m_logRatioCut;
  /** @brief cut on jet eta. */
  double m_etaCut;

  //Events accepted, rejected and with some error
  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.
  bool m_acceptAll;

  // Switch on Monitoring:
  bool m_doMonitoring;

  //Monitored Variables
  /** @brief variable for monitoring: E_T of the accepted jets. */
  std::vector<double> m_jetEt;
  /** @brief variable for monitoring: eta of the accepted jets. */
  std::vector<double> m_jetEta;
  /** @brief variable for monitoring: phi of the accepted jets. */
  std::vector<double> m_jetPhi;
  /** @brief variable for monitoring: cut counter. */
  int m_cutCounter;
  /** @brief variable for monitoring: log-ratio distribution. */
  double m_logRatio;

  StoreGateSvc*  m_storeGate;

};
#endif
