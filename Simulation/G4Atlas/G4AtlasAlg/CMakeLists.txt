# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( G4AtlasAlg )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( Eigen )

# G4AtlasAlgLib library

atlas_add_library( G4AtlasAlgLib
                     src/*.cxx
                     OBJECT
                     PUBLIC_HEADERS G4AtlasAlg
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}

                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel BarcodeInterfacesLib CxxUtils GaudiKernel G4AtlasInterfaces SGTools StoreGateLib EventInfo GeneratorObjects GeoModelInterfaces HepMC_InterfacesLib ISF_InterfacesLib MCTruthBaseLib )

# Component(s) in the package:
atlas_add_library( G4AtlasAlg
                   src/components/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel CxxUtils GaudiKernel G4AtlasInterfaces G4AtlasAlgLib SGTools StoreGateLib EventInfo GeneratorObjects MCTruthBaseLib )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/*.py )
