# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( MuonAnalysisAlgorithms )

atlas_add_library( MuonAnalysisAlgorithmsLib
   MuonAnalysisAlgorithms/*.h MuonAnalysisAlgorithms/*.icc Root/*.cxx
   PUBLIC_HEADERS MuonAnalysisAlgorithms
   LINK_LIBRARIES xAODEventInfo xAODMuon SelectionHelpersLib
   SystematicsHandlesLib MuonAnalysisInterfacesLib IsolationSelectionLib
   AnaAlgorithmLib
   PRIVATE_LINK_LIBRARIES RootCoreUtils )

atlas_add_dictionary( MuonAnalysisAlgorithmsDict
   MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsDict.h
   MuonAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES MuonAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( MuonAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel MuonAnalysisAlgorithmsLib )
endif()

atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*_jobOptions.py )
atlas_install_scripts( share/*_eljob.py )

if( XAOD_STANDALONE )
   atlas_add_test( testJobData
      SCRIPT MuonAnalysisAlgorithmsTest_eljob.py --data-type data --unit-test
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSim
      SCRIPT MuonAnalysisAlgorithmsTest_eljob.py --data-type mc --unit-test
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSim
      SCRIPT MuonAnalysisAlgorithmsTest_eljob.py --data-type afii --unit-test
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
else()
   atlas_add_test( testJobData
      SCRIPT athena.py
      MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsTest_jobOptions.py --evtMax=500 - --data-type data
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSim
      SCRIPT athena.py
      MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsTest_jobOptions.py --evtMax=500 - --data-type mc
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSim
      SCRIPT athena.py
      MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsTest_jobOptions.py --evtMax=500 - --data-type afii
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
endif()

if( XAOD_STANDALONE )
   atlas_add_test( testJobDataConfig
      SCRIPT MuonAnalysisAlgorithmsConfigTest_eljob.py --data-type data --unit-test
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSimConfig
      SCRIPT MuonAnalysisAlgorithmsConfigTest_eljob.py --data-type mc --unit-test
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSimConfig
      SCRIPT MuonAnalysisAlgorithmsConfigTest_eljob.py --data-type afii --unit-test
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
else()
   atlas_add_test( testJobDataConfig
      SCRIPT athena.py
      MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsConfigTest_jobOptions.py --evtMax=500 - --data-type data
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFullSimConfig
      SCRIPT athena.py
      MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsConfigTest_jobOptions.py --evtMax=500 - --data-type mc
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( testJobFastSimConfig
      SCRIPT athena.py
      MuonAnalysisAlgorithms/MuonAnalysisAlgorithmsConfigTest_jobOptions.py --evtMax=500 - --data-type afii
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
endif()
