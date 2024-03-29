# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PyJobTransforms )

# External dependencies:
find_package( cx_Oracle )
find_package( distro )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
atlas_install_runtime( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( scripts/HelloWorld_tf.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_generic( share/*.db
                       DESTINATION share
                       EXECUTABLE )

# Helper for test definition:
function( _add_test test type timeout )
   set( _testdir ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/unitTestRun/${test} )
   file( MAKE_DIRECTORY ${_testdir} )
   atlas_add_test( ${test}
      SCRIPT test/test_${test}.${type}
      PROPERTIES WORKING_DIRECTORY ${_testdir}
      PROPERTIES TIMEOUT ${timeout}
      POST_EXEC_SCRIPT nopost.sh )
endfunction( _add_test )

# Tests in the package:
foreach( test Echo_tf Sleep_tf
         transform TransformUtils trfAMI trfArgs trfDecorators trfEnv
         trfExceptions trfExe trfExitCodes trfFileUtils trfGraph trfJobOptions trfLogger
         trfMPTools trfReports trfReportsSignaledExit trfSignal trfSignatures
         trfUtils trfUtilsDBRelease trfUtilsParallelJobProcessor trfUtilsParallelJobProcessorData
         trfValidateRootFile trfValidation trfArgClasses )
      _add_test( ${test} py 300 )
endforeach()

# Tests with longer timeout (mostly for dbg build):
_add_test( LogFileScanner py 400 )

if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
   _add_test( RAWMerge_tf py 300 )
   _add_test( trfSubstepIntegration py 300 )
   _add_test( HelloWorldTrf sh 300 )
   _add_test( trfArgClassesATLAS py 300 )
endif()

#The following tests currently fail in master and should be revised
#AODMerge_tf
#AtlasG4_SimTTBar_tf
#ESDMerge_tf
#EVNTMerge_tf
#FullChain_tf
#HISTMerge_tf
#NTUPMerge_tf
#Reco_AthenaMP_tf
#Reco_EOS_tf
#Reco_MC_tf
#Reco_Tier0_tf
#Reco_q222_tf
#Reco_tf
#TAGMerge_tf
