/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKTOOLINTERFACES_ISEEDINGTOOL_H
#define ACTSTRKTOOLINTERFACES_ISEEDINGTOOL_H 1

// Athena 
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "Acts/MagneticField/MagneticFieldContext.hpp"

// ACTS EDM
#include "xAODInDetMeasurement/SpacePointContainer.h"
#include "xAODInDetMeasurement/SpacePointAuxContainer.h"
#include "ActsTrkEvent/Seed.h"

#include "Acts/Definitions/Algebra.hpp"

namespace ActsTrk {
  class ISeedingTool
    : virtual public IAlgTool {
  public:
    DeclareInterfaceID(ISeedingTool, 1, 0);
    
    virtual 
      StatusCode
      createSeeds(const EventContext& ctx,
                  const std::vector<const xAOD::SpacePoint*>& spContainer,
                  const Acts::Vector3& beamSpotPos,
                  const Acts::Vector3& bField,
                  ActsTrk::SeedContainer& seedContainer ) const = 0;
  };
  
} // namespace 

#endif 

