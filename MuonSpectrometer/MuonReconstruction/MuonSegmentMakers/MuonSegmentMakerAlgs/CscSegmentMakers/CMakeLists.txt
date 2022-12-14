################################################################################
# Package: CscSegmentMakers
################################################################################

# Declare the package name:
atlas_subdir( CscSegmentMakers )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( CscSegmentMakersLib
                   CscSegmentMakers/*.h
                   INTERFACE
                   PUBLIC_HEADERS CscSegmentMakers
                   LINK_LIBRARIES GaudiKernel MuonRecToolInterfaces MuonSegment MuonPrepRawData CscSegmentMakersLib GeoPrimitives )

# Component(s) in the package:
atlas_add_component( CscSegmentMakers
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CscSegmentMakersLib AthenaBaseComps StoreGateLib Identifier EventPrimitives xAODEventInfo MuonReadoutGeometry MuonIdHelpersLib CscClusterizationLib MuonRIO_OnTrack MuonRecHelperToolsLib MuonCondData TrkSurfaces TrkEventPrimitives TrkRoad TrkSegment  )


