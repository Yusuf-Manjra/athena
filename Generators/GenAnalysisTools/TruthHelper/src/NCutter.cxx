/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/NCutter.h"
#include "HepMCI/GenParticle.h"
#include <vector>

namespace TruthHelper {


  bool NCutter::operator()(const HepMC::ConstGenParticlePtr p ) const {
    for (std::vector<GenIMCselector*>::const_iterator i = m_selectors.begin(); i != m_selectors.end(); i++) {
      if ( !(*i)->operator()(p) ) return false;
    }
    return true;
  }


  GenIMCselector* NCutter::create() const {
    return new NCutter(*this);
  }


}
