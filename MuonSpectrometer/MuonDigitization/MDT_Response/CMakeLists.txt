# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MDT_Response )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( MDT_Response
                   src/Amplifier.cxx
                   src/MDT_Response.cxx
                   PUBLIC_HEADERS MDT_Response
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_executable( MDT_ResponseTest
                      src/MDT_ResponseTest.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils MDT_Response )
