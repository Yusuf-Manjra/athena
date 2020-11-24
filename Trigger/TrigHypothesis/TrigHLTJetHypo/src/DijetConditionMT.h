/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIJETCONDITIONMT_H
#define TRIGHLTJETHYPO_DIJETCONDITIONMT_H

/********************************************************************
 *
 * NAME:     DijetConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * Cuts on pairs of jets makling up a dijet
 *
 * AUTHOR:   P. Sherwood
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./IConditionMT.h"
#include <vector>
#include <string>

class ITrigJetHypoInfoCollector;

class DijetConditionMT: public IConditionMT{
 public:
  DijetConditionMT(
                   double massMin,
                   double massMax,
                   double detaMin,
                   double detaMax,
                   double dphiMin,
                   double dphiMax);

  ~DijetConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  std::string toString() const override;

  virtual unsigned int capacity() const override{return s_capacity;}

 private:

  bool passJetCuts(pHypoJet, pHypoJet) const;
  bool passDijetCuts(pHypoJet, pHypoJet) const;

  // cuts on sum of jets
  double m_massMin;
  double m_massMax;

  // cuts on the two jets
  double m_detaMin;
  double m_detaMax;
  
  double m_dphiMin;
  double m_dphiMax;

  const static unsigned int s_capacity{2};

};

#endif
