# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMultiVarHypo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_library( TrigMultiVarHypoLib
                   src/*.cxx
                   src/tools/*.cxx
                   src/tools/procedures/*.cxx
                   src/tools/common/*.cxx
                   PUBLIC_HEADERS TrigMultiVarHypo
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib AthenaMonitoringKernelLib GaudiKernel PathResolver )
