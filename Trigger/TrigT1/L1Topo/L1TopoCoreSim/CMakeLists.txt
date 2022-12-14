# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoCoreSim )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core RIO Hist )

# Component(s) in the package:
atlas_disable_as_needed()
atlas_add_library( L1TopoCoreSim
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoCoreSim
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CxxUtils L1TopoCommon L1TopoConfig L1TopoEvent L1TopoInterfaces TrigConfBase TrigConfData
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} L1TopoHardware L1TopoAlgorithms )

atlas_add_executable( TrigConfTopoStandAlone
                      src/test/TopoStandAlone.cxx
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils L1TopoConfig L1TopoCoreSim L1TopoEvent L1TopoInterfaces TrigConfBase TrigConfData TrigConfIO )

atlas_add_executable( TrigConfTopoTestSteeringConfig
                      src/test/TopoTestSteeringConfig.cxx
                      LINK_LIBRARIES CxxUtils L1TopoConfig L1TopoCoreSim TrigConfIO TrigConfData)
