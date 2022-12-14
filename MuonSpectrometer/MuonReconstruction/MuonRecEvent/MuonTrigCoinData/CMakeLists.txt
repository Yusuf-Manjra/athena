################################################################################
# Package: MuonTrigCoinData
################################################################################

# Declare the package name:
atlas_subdir( MuonTrigCoinData )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonTrigCoinData
                   src/*.cxx
                   PUBLIC_HEADERS MuonTrigCoinData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel CxxUtils GeoPrimitives Identifier EventPrimitives GaudiKernel MuonReadoutGeometry MuonPrepRawData TrkSurfaces
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( MuonTrigCoinDataDict
                      MuonTrigCoinData/MuonCoinDataDict.h
                      MuonTrigCoinData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AthenaKernel CxxUtils GeoPrimitives Identifier EventPrimitives GaudiKernel MuonReadoutGeometry MuonPrepRawData TrkSurfaces MuonTrigCoinData )

