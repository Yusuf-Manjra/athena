# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetTileCorrection )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )

# Libraries in the package:
atlas_add_root_dictionary( JetTileCorrectionLib JetTileCorrectionDictSource
   ROOT_HEADERS JetTileCorrection/JetTileCorrectionTool.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Extra library/libraries when not in a standalone build.
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs AthAnalysisBaseCompsLib )
endif()

atlas_add_library( JetTileCorrectionLib
   JetTileCorrection/*.h Root/*.cxx ${JetTileCorrectionDictSource}
   PUBLIC_HEADERS JetTileCorrection
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools JetCPInterfaces
   xAODJet
   PRIVATE_LINK_LIBRARIES PATInterfaces PathResolver xAODEventInfo ${extra_libs} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetTileCorrection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthAnalysisBaseCompsLib AsgTools JetCPInterfaces xAODJet
      PathResolver GaudiKernel JetTileCorrectionLib )
endif()

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( JetTileCorrectionTester
      util/JetTileCorrectionTester.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODJet
      xAODCore AsgTools PATInterfaces JetTileCorrectionLib )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   # FIX ME: temporarily disabled as part of the migration of AnalysisBase
   #         to master

   # atlas_add_test( ut_JetTileCorrectionTool_data
   #    SOURCES test/ut_JetTileCorrectionTool_data.cxx )
   # atlas_add_test( ut_JetTileCorrectionTool_mc
   #    SOURCES test/ut_JetTileCorrectionTool_mc.cxx )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( data/* )
