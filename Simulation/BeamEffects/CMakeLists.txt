# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BeamEffects )

# External dependencies:
find_package( CLHEP )
find_package( GTest )
find_package( Eigen REQUIRED )
#find_package( GMock )

# Needed to suppress warnings from eigen when compiling the test.
include_directories(SYSTEM ${EIGEN_INCLUDE_DIRS})

atlas_add_test( BeamEffectsAlg_test
                SOURCES src/*.cxx test/BeamEffectsAlg_test.cxx
                INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${GTEST_INCLUDE_DIRS} #${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib GaudiKernel TestTools AthenaBaseComps StoreGateLib xAODEventInfo GeneratorObjects HepMC_InterfacesLib BeamSpotConditionsData CxxUtils ${GTEST_LIBRARIES} #${GMOCK_LIBRARIES}
                )

#Added test of new component accumulator syntax
atlas_add_test( BeamEffectsAlgConfig_test
                SCRIPT python/BeamEffectsAlgConfig.py
                POST_EXEC_SCRIPT noerror.sh )

atlas_add_component( BeamEffects
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib CxxUtils GaudiKernel AthenaBaseComps HepMC_InterfacesLib StoreGateLib xAODEventInfo GeneratorObjects BeamSpotConditionsData )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
