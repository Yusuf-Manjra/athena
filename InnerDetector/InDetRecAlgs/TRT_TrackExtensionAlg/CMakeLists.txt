# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_TrackExtensionAlg )

# Component(s) in the package:
atlas_add_component( TRT_TrackExtensionAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps CxxUtils GaudiKernel InDetRecToolInterfaces StoreGateLib TrkTrack)
