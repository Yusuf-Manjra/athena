/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsGenerator.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/MagicNumbers.h"

namespace TruthHelper {


bool IsGenerator::operator()(HepMC::ConstGenParticlePtr p ) const {
    return (!HepMC::is_simulation_particle(p)) &&
           (p->status() < 200 ||
            p->status() % 1000 == 1 || p->status() % 1000 == 2 ||
            p->status() == 10902);
}


GenIMCselector* IsGenerator::create() const {
    return new IsGenerator(*this);
}


}
