/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_WMULTILEPTONFILTER_H
#define GENERATORFILTERS_WMULTILEPTONFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Require the grandmother of a electron or muon to be either Top->W->e/mu or H->W->e/mu
/// @todo Safety?
/// @author Huaqiao Zhang
class WMultiLeptonFilter : public GenFilter {
public:

  WMultiLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  int m_NLeptonsH;
  int m_NLeptonsT;
  int m_H_WPlus_e, m_H_WMinus_e;
  int m_H_WPlus_m, m_H_WMinus_m;
  int m_T_WPlus_e, m_T_WMinus_e;
  int m_T_WPlus_m, m_T_WMinus_m;

};

#endif
