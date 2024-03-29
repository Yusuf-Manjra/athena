# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ISF_FastCaloSimParametrization )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( HepPDT )
find_package( ROOT COMPONENTS TreePlayer Core Tree MathCore Hist RIO Physics Graf Gpad )

# Component(s) in the package:
atlas_add_root_dictionary( ISF_FastCaloSimParametrizationLib
                           ISF_FastCaloSimParametrizationLibDictSource
                           ROOT_HEADERS
                           ISF_FastCaloSimParametrization/MeanAndRMS.h
                           ISF_FastCaloSimParametrization/TFCSNNLateralShapeParametrization.h
                           ISF_FastCaloSimParametrization/TFCSSimpleLateralShapeParametrization.h
                           ISF_FastCaloSimParametrization/TreeReader.h
                           ISF_FastCaloSimParametrization/FCS_Cell.h
                           ISF_FastCaloSimParametrization/CaloGeometry.h
                           ISF_FastCaloSimParametrization/CaloGeometryLookup.h
                           Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT HepPDT CLHEP Geant4 )

atlas_add_library( ISF_FastCaloSimParametrizationLib
                   Root/*.cxx src/lib/CaloGeometryFromCaloDDM.cxx src/lib/CaloGeometryLookup.cxx src/lib/CaloGeometry.cxx
                   src/lib/CaloCellContainerSD.cxx
                   ${ISF_FastCaloSimParametrizationLibDictSource}
                   OBJECT
                   PUBLIC_HEADERS ISF_FastCaloSimParametrization
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AtlasHepMCLib BarcodeEventLib CaloDetDescrLib CaloInterfaceLib CxxUtils GaudiKernel ISF_FastCaloSimEvent ISF_InterfacesLib LArElecCalib LArReadoutGeometry StoreGateLib TrkEventPrimitives TrkExInterfaces TrkParameters
                   PRIVATE_LINK_LIBRARIES CaloGeoHelpers xAODCaloEvent FastCaloSimLib FastCaloSimHitLib G4AtlasToolsLib MCTruth)

atlas_add_library( ISF_FastCaloSimParametrization
                   src/*.cxx
                   src/components/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEANT4_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaPoolUtilities AtlasHepMCLib CaloDetDescrLib CaloEvent CaloGeoHelpers CaloIdentifier CaloDetDescrUtils CxxUtils GaudiKernel GeneratorObjects GeoModelInterfaces GeoPrimitives ISF_Event ISF_FastCaloSimEvent ISF_FastCaloSimInterfaces ISF_FastCaloSimParametrizationLib ISF_InterfacesLib LArSimEvent NavFourMom StoreGateLib TileConditionsLib TileDetDescr TileSimEvent TrackRecordLib TrkExInterfaces TrkGeometry TrkMaterialOnTrack TrkParameters TrkSurfaces xAODCaloEvent FastCaloSimLib FastCaloSimHitLib G4AtlasToolsLib MCTruth)

# Code in this file makes heavy use of eigen and runs orders of magnitude
# more slowly without optimization.  So force this to be optimized even
# in debug builds.  If you need to debug it you might want to change this.
# Specifying optimization via an attribute on the particular
# function didn't work, because that still didn't allow inlining.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     ${CMAKE_CURRENT_SOURCE_DIR}/src/FastCaloSimCaloExtrapolation.cxx
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/*.py )
