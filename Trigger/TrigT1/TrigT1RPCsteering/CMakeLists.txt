# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1RPCsteering )

# Component(s) in the package:
atlas_add_component( TrigT1RPCsteering
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AtlasHepMCLib GaudiKernel GeneratorObjects MuonDigitContainer MuonIdHelpersLib MuonRDO MuonReadoutGeometry RPC_CondCablingLib TrigT1RPChardwareLib StoreGateLib TrigT1Interfaces TrigT1RPClogicLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
