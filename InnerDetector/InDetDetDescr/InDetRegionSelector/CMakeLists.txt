# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetRegionSelector )

# Component(s) in the package:
atlas_add_component( InDetRegionSelector
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps RegSelLUT GaudiKernel PixelConditionsData SCT_CablingLib Identifier InDetIdentifier InDetReadoutGeometry IRegionSelector StoreGateLib TRT_ReadoutGeometry TRT_CablingLib )

