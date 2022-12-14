# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsTrkPriVtxFinderTool )

# External dependencies:
find_package( Acts COMPONENTS Core )
find_package( Boost )

# Component(s) in the package:
atlas_add_component( ActsTrkPriVtxFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES}
                     ActsCore
                     ActsGeometryInterfacesLib
                     ActsGeometryLib
                     ActsInteropLib
                     AthenaBaseComps
                     BeamSpotConditionsData
                     GaudiKernel
                     InDetRecToolInterfaces
                     InDetTrackSelectionToolLib
                     TrkLinks
                     TrkParticleBase
                     TrkTrack
                     TrkTrackLink
                     xAODTracking )
