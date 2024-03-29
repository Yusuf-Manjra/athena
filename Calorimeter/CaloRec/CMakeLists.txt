# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloRec )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( ROOT COMPONENTS Core MathCore MathMore Minuit Minuit2 Matrix )

# Component(s) in the package:

atlas_add_component( CaloRec
   src/components/*.cxx src/*.cxx  CaloRec/*.h
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloConditions CaloEvent CaloGeoHelpers
   CaloIdentifier AthenaBaseComps AthenaKernel CxxUtils AthenaPoolUtilities
   Identifier xAODCaloEvent GaudiKernel CaloDetDescrLib CaloUtilsLib
   StoreGateLib LumiBlockCompsLib AthenaMonitoringKernelLib
   ${ROOT_LIBRARIES} ${CORAL_LIBRARIES}
   ${EIGEN_LIBRARIES} AthAllocators IdDictParser CaloLumiConditions
   LArRawConditions FourMom LumiBlockData )



# Test(s) in the package:
atlas_add_test( CaloCellFastCopyTool_test
   SOURCES test/CaloCellFastCopyTool_test.cxx src/CaloCellFastCopyTool.cxx
   LINK_LIBRARIES CaloEvent CaloDetDescrLib CaloIdentifier CaloInterfaceLib CaloUtilsLib
   IdDictParser AthenaBaseComps StoreGateLib CxxUtils GaudiKernel
   LOG_IGNORE_PATTERN "@0x[0-9a-f]{4,}|Retrieved const pointer|Retrieved const handle|object not modifiable" )

atlas_add_test( CaloCellContainerFromClusterTool_test
   SOURCES test/CaloCellContainerFromClusterTool_test.cxx src/CaloCellContainerFromClusterTool.cxx src/Blob2ToolConstants.cxx
   LINK_LIBRARIES xAODCaloEvent CaloInterfaceLib CaloUtilsLib
   #LINK_LIBRARIES CaloRecLib 
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( CaloBCIDCoeffsCondAlg_test
   SOURCES test/CaloBCIDCoeffsCondAlg_test.cxx src/CaloBCIDCoeffsCondAlg.cxx
   LINK_LIBRARIES CaloLumiConditions LArRawConditions IdDictParser TestTools )

atlas_add_test( CaloBCIDLumiCondAlg_test
   SOURCES test/CaloBCIDLumiCondAlg_test.cxx src/CaloBCIDLumiCondAlg.cxx
   LINK_LIBRARIES CaloLumiConditions LArRawConditions IdDictParser TestTools LumiBlockData )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py share/*.txt )


atlas_add_test( CaloBCIDAvgAlgConfig_test
                SCRIPT python -m CaloRec.CaloBCIDAvgAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )


atlas_add_test( CaloBCIDCoeffsCondAlgConfig_test
                SCRIPT python -m CaloRec.CaloBCIDCoeffsCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )


atlas_add_test( CaloBCIDLumiCondAlgConfig_test
                SCRIPT python -m CaloRec.CaloBCIDLumiCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )


atlas_add_test( CaloTopoClusterConfig_test
                SCRIPT python -m CaloRec.CaloTopoClusterConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)


atlas_add_test( CaloCellContainerAliasAlg_test
                SCRIPT python -m CaloRec.CaloCellContainerAliasAlg_test
                PROPERTIES TIMEOUT 300
                LOG_SELECT_PATTERN "ERROR|error|WARNING [^U]|FATAL|processing" )


atlas_add_test( CaloThinCellsBySamplingAlg_test
                SCRIPT python -m CaloRec.CaloThinCellsBySamplingAlg_test
                PROPERTIES TIMEOUT 600
                LOG_SELECT_PATTERN "ERROR|error|WARNING [^U]|FATAL|processing" )


atlas_add_test( CaloThinCellsByClusterAlg_test
                SCRIPT python -m CaloRec.CaloThinCellsByClusterAlg_test
                PROPERTIES TIMEOUT 600
                LOG_SELECT_PATTERN "ERROR|error|WARNING [^U]|FATAL|processing" )


atlas_add_test( ToolConstantsCondAlg_test
                SCRIPT test/ToolConstantsCondAlg_test.py
                LOG_IGNORE_PATTERN "Cache alignment|Current filenames|Unable to locate catalog|peeking into|IOVDbSvc +INFO|^Py:MetaReader|CondProxyProvider +INFO|AthDictLoaderSvc +INFO" )


atlas_add_test( CaloCellContainerAliasAlgConfig_test
                SCRIPT python -m CaloRec.CaloCellContainerAliasAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator" )


atlas_add_test( CaloThinCellsByClusterAlgConfig_test
                SCRIPT python -m CaloRec.CaloThinCellsByClusterAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator" )


atlas_add_test( CaloThinCellsBySamplingAlgConfig_test
                SCRIPT python -m CaloRec.CaloThinCellsBySamplingAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator" )

atlas_add_test( ToolConstantsCondAlgConfig_test
                SCRIPT python -m CaloRec.ToolConstantsCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---" )

atlas_add_test( CaloCellMakerTest
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/CaloCellMakerConfig.py
                POST_EXEC_SCRIPT "/usr/bin/diff -u0 CaloCells.txt ${CMAKE_CURRENT_SOURCE_DIR}/share/CaloCells.txt.ref" )

atlas_add_test( CaloTowerStore_test
   SCRIPT athena.py CaloRec/CaloTowerStore_test.py
   LOG_IGNORE_PATTERN "Reading file|Unable to locate catalog|Cache alignment|MetaReader|AutoConfiguration|IOVDbSvc +INFO"
   PROPERTIES TIMEOUT 500 )
