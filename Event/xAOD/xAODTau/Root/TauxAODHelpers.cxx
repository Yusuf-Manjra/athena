/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTau/TauxAODHelpers.h"
#include "xAODTau/TauJet.h"

typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;

// ==================================================================
template<class T>
const T* xAOD::TauHelpers::getLink(const xAOD::IParticle* particle, std::string name, bool debug)
{
  if (!particle) return 0;
  typedef ElementLink< DataVector<T> > Link_t;
  
  if (!particle->isAvailable< Link_t >(name) ) 
  { 
    if (debug) std::cerr<< "Link not available" << std::endl; 
    return 0; 
  }  
  const Link_t link = particle->auxdata<Link_t>(name);
  if (!link.isValid()) 
  { 
    if (debug) std::cerr << "Invalid link" << std::endl; 
    return 0; 
  }
  return *link;
}

const xAOD::TruthParticle* xAOD::TauHelpers::getTruthParticle(const xAOD::IParticle* particle, bool debug)
{
  return getLink<xAOD::TruthParticle>(particle, "truthParticleLink", debug);
}
