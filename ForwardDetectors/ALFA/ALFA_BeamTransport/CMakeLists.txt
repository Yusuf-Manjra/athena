# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_BeamTransport )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ALFA_BeamTransport
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS  ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES  ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps FPTracker GaudiKernel StoreGateLib xAODEventInfo GeneratorObjects FPTracker )

# Install files from the package:
atlas_install_joboptions( share/*.py )
