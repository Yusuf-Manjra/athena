# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHLTMonitoring )

# Component(s) in the package:
atlas_add_component( TrigHLTMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaMonitoringKernelLib AthenaMonitoringLib StoreGateLib TrigSteeringEvent xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

