# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsTrkFittingAlgs )

# External dependencies:
find_package( Acts COMPONENTS Core )
find_package( ROOT COMPONENTS Core Tree RIO )

# Component(s) in the package:
atlas_add_component( ActsTrkFittingAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS  ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES  ${ROOT_LIBRARIES}
                     ActsCore
                     ActsGeometryInterfacesLib
                     ActsGeometryLib
                     ActsInteropLib
                     EventPrimitives
                     GeoPrimitives
                     AthenaBaseComps
                     AthenaKernel
                     CaloDetDescrLib
                     EventInfo
                     GaudiKernel
                     StoreGateLib
                     TRT_ReadoutGeometry
                     TrkGeometry
                     TrkExInterfaces
                     TrkParameters
                     TrkSurfaces
                     TrkFitterUtils
                     TrkTrackSummary
                     xAODTracking 
                     TrkToolInterfaces
                     TrkFitterInterfaces
                     ActsTrkEventCnvLib)

# Install files from the package:
atlas_install_joboptions( share/*.py )

