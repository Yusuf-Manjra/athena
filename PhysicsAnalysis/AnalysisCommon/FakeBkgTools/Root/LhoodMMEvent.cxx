/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#define LhoodMMEvent_cxx
#include "FakeBkgTools/LhoodMMEvent.h"

#include <stdexcept>
#include <iostream>
 
void LhoodMMEvent::Init(int nlep, const std::vector<double>& realEff, const std::vector<double>& fakeEff, const std::vector<bool>& isTight, const std::vector<int>& charge, float weight, double aux, double aux2) {

  m_nlep = nlep;
  m_realEff = realEff;
  m_fakeEff = fakeEff;

  if (m_realEff.size() < m_nlep) {
    throw std::invalid_argument("LhoodMMEvent::Init:  Expected " + std::to_string(m_nlep) + " real efficiency values, but only " + std::to_string(m_realEff.size()) + " provided in efficiency vector");
  }
  if (m_fakeEff.size() < m_nlep) {
    throw std::invalid_argument("ERROR in LhoodMMEvent::Init:  Expected " + std::to_string(m_nlep) + " fake efficiency values, but " + std::to_string(m_fakeEff.size()) + " provided in efficiency vector");
  }
  m_isTight = isTight;
  m_charge = charge;

  m_weight = weight;
  m_aux = aux;
  m_aux2 = aux2;
}

LhoodMMEvent::LhoodMMEvent(int nlep, const std::vector<double>& realEff, const std::vector<double>& fakeEff, const std::vector<bool>& isTight, const std::vector<int>& charge, double weight, double aux, double aux2 ) {

  Init(nlep, realEff, fakeEff, isTight, charge, weight, aux, aux2);
}

LhoodMMEvent::LhoodMMEvent(int nlep, const std::vector<float>& realEff, const std::vector<float>& fakeEff, const std::vector<bool>& isTight, const std::vector<int>& charge, float weight, float aux, float aux2) {
 
  std::vector<double> realEff_d, fakeEff_d;
  std::vector<float>::const_iterator effIter;
  for (effIter = realEff.begin(); effIter != realEff.end(); effIter++) {
    realEff_d.push_back(*effIter);
  }
  for (effIter = fakeEff.begin(); effIter != fakeEff.end(); effIter++) {
    fakeEff_d.push_back(*effIter);
  }

  double aux_d = aux;
  double aux2_d = aux2;

  Init(nlep, realEff_d, fakeEff_d, isTight, charge, weight, aux_d, aux2_d);

}

LhoodMMEvent::LhoodMMEvent(const LhoodMMEvent& mmevt) {

  Init( mmevt.m_nlep, mmevt.m_realEff, mmevt.m_fakeEff, mmevt.m_isTight, 
       mmevt.m_charge, mmevt.m_weight, mmevt.m_aux, mmevt.m_aux2);
}

double LhoodMMEvent::realEff(unsigned ilep) const {
  if (ilep <= m_nlep) {
    return m_realEff[ilep];
  } else {
    throw std::out_of_range(" LhoodMMEvent::realEff: invalid lepton index " + std::to_string(ilep) + "; must be less than " + std::to_string(m_nlep));
  }

}

double LhoodMMEvent::fakeEff(unsigned ilep) const {
  if (ilep <= m_nlep) {
    return m_fakeEff[ilep];
  } else {

    throw std::out_of_range(" LhoodMMEvent::fakeEff: invalid lepton index " + std::to_string(ilep) + "; must be less than " + std::to_string(m_nlep));
  }

}

bool LhoodMMEvent::isTight(unsigned ilep) const {
  if (ilep < m_nlep) {
    return m_isTight[ilep];
  } else {
    throw std::out_of_range(" LhoodMMEvent::isTight: invalid lepton index " + std::to_string(ilep) + "; must be less than " + std::to_string(m_nlep));
  }

}

int LhoodMMEvent::charge(unsigned ilep) const {
  if (ilep > m_nlep) {

    throw std::out_of_range(" LhoodMMEvent::charge: invalid lepton index " + std::to_string(ilep) + "; must be less than " + std::to_string(m_nlep));
  }
  // user may input an empty charge vector if the selection does not care about 
  // lepton charges. Just return 0 without an error in this case
  if (unsigned(ilep) > m_charge.size() ) {
    return 0;
  } else {
    return m_charge[ilep];
  }
}




