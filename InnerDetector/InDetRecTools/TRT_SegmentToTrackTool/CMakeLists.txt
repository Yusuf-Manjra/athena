# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_SegmentToTrackTool )

# Component(s) in the package:
atlas_add_component( TRT_SegmentToTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces TrkEventPrimitives InDetIdentifier InDetRIO_OnTrack TrkPseudoMeasurementOnTrack TrkExInterfaces TrkFitterInterfaces TrkSurfaces TrkToolInterfaces MagFieldElements MagFieldConditions)
