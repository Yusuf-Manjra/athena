/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSIMHITCOLLECTION_P4_H
#define MDTSIMHITCOLLECTION_P4_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p3.h"

namespace Muon {
class MDTSimHitCollection_p4   
{
 public:
  std::vector<int>      m_MDTid;
  std::vector<float>    m_globalTime;
  std::vector<float>    m_driftRadius;
  std::vector<float>    m_stX, m_stY, m_stZ; //!< Represents m_localPosition in transient class
  //std::vector<float>    m_meanTime;
  std::vector<HepMcParticleLink_p3>  m_partLink;
  std::vector<float>    m_energyDeposit;
  std::vector<int>      m_particleEncoding;
  std::vector<float>    m_kineticEnergy;
  std::vector<float>    m_stepLength;
};
}

#endif

