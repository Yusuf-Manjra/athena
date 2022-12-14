# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1AODSystems )

# External dependencies:
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate UI files automatically:
# Note: add the "Widgets" component to "find_package( Qt5 ...)" if you have UI
# files, otherwise UIC, even if CMAKE_AUTOUIC is set to ON, is not run
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

if(NOT BUILDVP1LIGHT)
    # Component(s) in the package:
    atlas_add_library( VP1AODSystems
       VP1AODSystems/*.h src/*.h src/*.cxx
       PUBLIC_HEADERS VP1AODSystems
       PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}
       LINK_LIBRARIES xAODMuon xAODEgamma xAODTracking xAODBTagging GaudiKernel VP1Base StoreGateLib
       Qt5::Core Qt5::Gui
       PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} GeoPrimitives EventPrimitives
       xAODBase xAODCaloEvent xAODJet xAODMissingET PathResolver TrkExInterfaces
       VP1HEPVis VP1Utils Qt5::Gui )
else()
    # Component(s) in the package:
    atlas_add_library( VP1AODSystems
       src/*.h VP1AODSystems/*.h src/*cxx
       PUBLIC_HEADERS VP1AODSystems
       PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}
       LINK_LIBRARIES xAODMuon xAODEgamma xAODTracking xAODBTagging VP1Base
       Qt5::Core Qt5::Gui
       PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} GeoPrimitives EventPrimitives
       xAODBase xAODCaloEvent xAODJet xAODMissingET PathResolver TrkExInterfaces xAODEventInfo
       VP1HEPVis VP1Utils xAODRootAccess Qt5::Gui )
endif()

# Install files from the package:
atlas_install_runtime( textures/*.jpg )
