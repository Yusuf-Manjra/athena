# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsTrkSeedingTool )

# External dependencies:
find_package( Acts COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( ActsTrkSeedingTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES
                     GaudiKernel
                     ActsCore
                     ActsTrkEventLib
                     ActsTrkToolInterfacesLib
                     ActsInteropLib
                     CxxUtils
                     InDetRecToolInterfaces
                     SiSpacePoint
                     xAODInDetMeasurement
                     )



