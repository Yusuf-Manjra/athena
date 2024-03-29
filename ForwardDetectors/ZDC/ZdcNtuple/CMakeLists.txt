atlas_subdir( ZdcNtuple )


atlas_add_library (ZdcNtupleLib
                   ZdcNtuple/*.h Root/*.cxx
		   PUBLIC_HEADERS ZdcNtuple
		   LINK_LIBRARIES AnaAlgorithmLib AsgAnalysisInterfaces
                   AsgTools PMGAnalysisInterfacesLib PMGToolsLib
                   TrigConfInterfaces TrigDecisionToolLib
                   ZdcAnalysisLib xAODCaloEvent xAODEventInfo
                   xAODForward xAODHIEvent xAODTracking
                   xAODTrigL1Calo xAODTrigMinBias xAODTrigger xAODTruth TrigConfxAODLib
                   PRIVATE_LINK_LIBRARIES xAODCore xAODRootAccess)

if (XAOD_STANDALONE)
atlas_add_dictionary (ZdcNtupleDict
			ZdcNtuple/ZdcNtupleDict.h
			ZdcNtuple/selection.xml
			LINK_LIBRARIES ZdcNtupleLib)
endif ()

if (NOT XAOD_STANDALONE)
  # Add a component library for AthAnalysis only:
atlas_add_component (ZdcNtuple
    src/components/*.cxx
    LINK_LIBRARIES ZdcNtupleLib)
endif ()


# Install files from the package:
atlas_install_joboptions( share/*_jobOptions.py )