# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEgammaRec )

# Component(s) in the package:
atlas_add_component( TrigEgammaRec
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib GaudiKernel RecoToolInterfaces StoreGateLib 
                     TrigSteeringEvent TrkCaloExtension xAODTracking xAODTrigCalo xAODTrigEgamma CaloDetDescrLib)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )

