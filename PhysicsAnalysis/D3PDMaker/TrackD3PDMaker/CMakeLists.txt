# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackD3PDMaker )

# Component(s) in the package:
atlas_add_component( TrackD3PDMaker
                     src/components/*.cxx
                     LINK_LIBRARIES TrkParameters AthenaKernel CxxUtils Identifier EventPrimitives xAODBase xAODPrimitives xAODTracking GaudiKernel InDetIdentifier InDetReadoutGeometry InDetTestPixelLayerLib ParticleEvent D3PDMakerInterfaces D3PDMakerUtils Particle InDetRecToolInterfaces ITrackToVertex RecoToolInterfaces TrkEventPrimitives TrkParticleBase VxVertex BeamSpotConditionsData )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
