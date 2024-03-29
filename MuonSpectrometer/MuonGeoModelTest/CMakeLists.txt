################################################################################
# Package: MuonGeoModelTest
################################################################################

# Declare the package name:
atlas_subdir( MuonGeoModelTest )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonGeoModelTestLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonGeoModelTest
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel MuonCalibITools MuonReadoutGeometry MuonCondData
                                  MuonPrepRawData MuonSimEvent MuonIdHelpersLib StoreGateLib RegionSelectorLib MuonTesterTreeLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} GeoAdaptors GeoPrimitives GeneratorObjects 
                                          MuonAlignmentData MuonDigitContainer TrkDistortedSurfaces TrkSurfaces )

atlas_add_component( MuonGeoModelTest
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthenaBaseComps Identifier GaudiKernel 
                                    MuonCalibITools MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData MuonSimEvent
                                    StoreGateLib GeoAdaptors GeoPrimitives RegionSelectorLib GeneratorObjects MuonCondData
                                    MuonAlignmentData MuonDigitContainer TrkDistortedSurfaces TrkSurfaces MuonGeoModelTestLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.txt share/*.py )

