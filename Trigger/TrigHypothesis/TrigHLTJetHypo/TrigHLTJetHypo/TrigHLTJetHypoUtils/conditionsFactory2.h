/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHLTHYPO_CONDITIONSFACTORY2_H
#define TRIGJETHLTHYPO_CONDITIONSFACTORY2_H

// ********************************************************************
//
// NAME:     conditionsFactory2.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

Conditions conditionsFactoryEtaEt(const std::vector<double>& etasMin,
                                  const std::vector<double>& etasMax,
                                  const std::vector<double>& thresholds,
                                  const std::vector<int>& asymmetricEtas);

// for conditionsFactory2
Conditions conditionsFactoryDijetEtaMass(const std::vector<double>& etasMin,
                                         const std::vector<double>& etasMax,
                                         const std::vector<double>& etMins,
                                         const std::vector<double>& dEtaMin,
                                         const std::vector<double>& dEtaMax,
                                         const std::vector<double>& massesMin,
                                         const std::vector<double>& massesMax);

// for conditionsFactory
Conditions conditionsFactoryDijetEtaMass(double etasMin,
                                         double etasMax,
                                         double ystarsMin,
                                         double ystarsMax,
                                         double massesMin,
                                         double massesMax);


Conditions conditionsFactoryTLA(const std::vector<double>& etasMin,
                                const std::vector<double>& etasMax,
                                const std::vector<double>& ystarsMin,
                                const std::vector<double>& ystarsMax,
                                const std::vector<double>& massesMin,
                                const std::vector<double>& massesMax);

Conditions conditionsFactoryHT(double htMin);



#endif
