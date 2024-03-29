/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_VERSIONS_SPACEPOINTAUXCONTAINER_V1_H
#define XAODINDETMEASUREMENT_VERSIONS_SPACEPOINTAUXCONTAINER_V1_H

#include <vector>
#include <array>

#include "xAODCore/AuxContainerBase.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"

namespace xAOD {
  /// Auxiliary store for space point
  ///
  class SpacePointAuxContainer_v1 : public AuxContainerBase {
  public:
    /// Default Constructor
    SpacePointAuxContainer_v1();

  private:
    /// @name Defining space point parameters
    /// @{

    std::vector < std::vector< IdentifierHash::value_type > > elementIdList;
    std::vector < std::array < float, 3 > > globalPosition;
    std::vector < float > radius;
    std::vector < float > varianceR;
    std::vector < float > varianceZ;
    std::vector < std::vector< std::size_t > > measurementIndexes;

    /// @}
  };

}

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::SpacePointAuxContainer_v1, xAOD::AuxContainerBase );
  
#endif
