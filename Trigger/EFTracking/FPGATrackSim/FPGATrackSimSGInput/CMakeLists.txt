# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FPGATrackSimSGInput )

# External dependencies:
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree Hist RIO )

# Component(s) in the package:
atlas_add_library(
  FPGATrackSimSGInputLib         src/*.cxx FPGATrackSimSGInput/*.h
  PUBLIC_HEADERS            FPGATrackSimSGInput
  INCLUDE_DIRS              ${ROOT_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS      ${HEPPDT_INCLUDE_DIRS}
  LINK_LIBRARIES            ${ROOT_LIBRARIES}
                            AthenaBaseComps GeneratorObjects InDetIdentifier InDetPrepRawData StoreGateLib FPGATrackSimObjectsLib TrkExInterfaces TrkToolInterfaces BeamSpotConditionsData InDetSimData AtlasHepMCLib
  PRIVATE_LINK_LIBRARIES    ${HEPPDT_LIBRARIES}
                            AtlasDetDescr EventInfo EventPrimitives GaudiKernel IdDict IdDictDetDescr Identifier InDetRawData InDetReadoutGeometry InDetRIO_OnTrack ReadoutGeometryBase TileIdentifier TrkFitterInterfaces TrkRIO_OnTrack TrkSpacePoint VxVertex 
)

atlas_add_component(
  FPGATrackSimSGInput            src/components/*.cxx
  INCLUDE_DIRS              ${HEPPDT_INCLUDE_DIRS}
  LINK_LIBRARIES            ${HEPPDT_LIBRARIES} FPGATrackSimSGInputLib
)

# Install files from the package:
atlas_install_joboptions( share/*jobOptions*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
