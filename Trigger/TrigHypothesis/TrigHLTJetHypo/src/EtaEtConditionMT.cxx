/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./EtaEtConditionMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

EtaEtConditionMT::EtaEtConditionMT(double etaMin,
                                   double etaMax,
                                   double threshold):

m_etaMin(etaMin), m_etaMax(etaMax), m_threshold(threshold){
}


bool EtaEtConditionMT::isSatisfied(const pHypoJet& ip,
                                   const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  auto abseta = std::abs(ip->eta());
  auto et = ip->et();
  bool pass =
    m_etaMin <= abseta and
    m_etaMax > abseta and
    m_threshold <= et;

  
  if(collector){
    const void* address = static_cast<const void*>(this);

    std::stringstream ss0;
    ss0 << "EtaEtCondition: (" << address << ") " 
        << " eta[" << m_etaMin << ", " << m_etaMax << "]" 
        << " et thresh " << m_threshold
        << " pass: "  << std::boolalpha << pass << '\n';

    auto j_addr = static_cast<const void*>(ip);
    std::stringstream ss1;
    ss1 <<  "     jet : ("<< j_addr << ")"
        " abseta " << abseta << " et " << et << '\n';

    collector->collect(ss0.str(), ss1.str());

  }
  return pass;
}


bool 
EtaEtConditionMT::isSatisfied(const HypoJetVector& ips,
                              const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const {
  auto result =  isSatisfied(ips[0], c);
  return result;
}


std::string EtaEtConditionMT::toString() const noexcept {
  std::stringstream ss;
  ss << "EtaEtConditionMT (" << this << ") etaMin "
     <<  m_etaMin 
     << " etaMax " 
     << m_etaMax 
     << " Et threshold: " 
     << m_threshold
     <<'\n';

  return ss.str();
}
