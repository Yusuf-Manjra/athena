################################################################################
# Package: G4DebuggingTools
################################################################################

# Declare the package name:
atlas_subdir( G4DebuggingTools )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( TBB )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( G4DebuggingHelperLib
                   src/helper/G4DebuggingHelper.cxx
                   OBJECT
                   PUBLIC_HEADERS G4DebuggingTools
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} )

atlas_add_library( G4DebuggingTools
                   src/*.cxx
                   src/components/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} GaudiKernel AthenaBaseComps StoreGateLib xAODEventInfo G4AtlasInterfaces G4AtlasToolsLib MCTruth MCTruthBaseLib SimHelpers G4DebuggingHelperLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

