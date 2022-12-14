# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1TGC )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_library( TrigT1TGCLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1TGC
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel Identifier MuonCondSvcLib MuonDigitContainer MuonRDO StoreGateLib TileEvent TrigT1Interfaces TGCTriggerCondSvcLib xAODMuon
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities MuonCondInterface PathResolver TGCcablingInterfaceLib TrigConfL1Data )

atlas_add_component( TrigT1TGC
                     src/components/*.cxx
                     LINK_LIBRARIES TrigT1TGCLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/TrigT1TGC_jobOptions.py )
atlas_install_runtime( share/TrigT1TGCMaskedChannel*.db )
