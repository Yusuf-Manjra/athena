# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonMM_Cabling )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_component( MuonMM_Cabling
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib MuonIdHelpersLib 
                                    MuonCablingData AthenaPoolUtilities MuonCondSvcLib PathResolver MuonReadoutGeometry SGTools )


# Install files from the package:
atlas_install_python_modules( python/*.py )