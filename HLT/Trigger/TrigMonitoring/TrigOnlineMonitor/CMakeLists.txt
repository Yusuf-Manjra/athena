# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigOnlineMonitor )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )
find_package( tdaq-common COMPONENTS eformat )

# Component(s) in the package:
atlas_add_component( TrigOnlineMonitor
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaInterprocess AthenaKernel AthenaMonitoringKernelLib ByteStreamCnvSvcBaseLib ByteStreamData GaudiKernel LumiBlockData MagFieldConditions StoreGateLib TrigConfData TrigT1Result TrigT1MuctpiBits )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
