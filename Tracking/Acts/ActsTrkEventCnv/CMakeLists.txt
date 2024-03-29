# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir(ActsTrkEventCnv)

# External dependencies:
find_package(Acts COMPONENTS Core)

# Component(s) in the package:
atlas_add_library(ActsTrkEventCnvLib
    ActsTrkEventCnv/*.h
    src/*.cxx
    PUBLIC_HEADERS ActsTrkEventCnv
    INTERFACE
    LINK_LIBRARIES
    ActsCore
    ActsGeometryLib
    GaudiKernel
    StoreGateLib
    TrkTrack
    TrkParameters
    xAODMeasurementBase
    xAODTracking
    ActsTrkEventLib
)

atlas_add_component(ActsTrkEventCnv
    src/ActsToTrkConverterTool.cxx
    src/components/*.cxx
    LINK_LIBRARIES
    ActsTrkEventCnvLib
    AthenaBaseComps
    AthenaKernel
    TrkGeometry
    TrkSurfaces
    TrkTrackSummary
    TrkExUtils
)

atlas_add_test(ActsTrkEventCnvTest
    SCRIPT test/testConversion.py
    PROPERTIES TIMEOUT 900
    POST_EXEC_SCRIPT noerror.sh)
