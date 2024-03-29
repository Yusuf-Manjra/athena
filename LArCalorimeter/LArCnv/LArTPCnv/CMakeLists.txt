# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( LArTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS LArTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib CxxUtils Identifier LArRawEvent LArRecEvent
                         PRIVATE_LINK_LIBRARIES AthenaKernel CaloIdentifier GaudiKernel LArIdentifier )

atlas_add_dictionary( LArTPCnvDict
                      LArTPCnv/LArTPCnvDict.h
                      LArTPCnv/selection.xml
                      LINK_LIBRARIES LArTPCnv )

atlas_add_dictionary( OLD_LArTPCnvDict
                      LArTPCnv/LArTPCnvDict.h
                      LArTPCnv/OLD_selection.xml
                      LINK_LIBRARIES LArTPCnv )

atlas_add_test( LArDigitContainerCnv_p1_test
                SOURCES
                test/LArDigitContainerCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArTPCnv TestTools )

atlas_add_test( LArDigitContainerCnv_p2_test
		SOURCES
		test/LArDigitContainerCnv_p2_test.cxx
		LINK_LIBRARIES GaudiKernel LArTPCnv TestTools IdDictParser )

atlas_add_test( LArDigitContainerCnv_p3_test
		SOURCES
		test/LArDigitContainerCnv_p3_test.cxx
		LINK_LIBRARIES GaudiKernel LArTPCnv TestTools IdDictParser
		LOG_IGNORE_PATTERN "Welcome to ApplicationMgr|running on|getRegistryEntries|DEBUG Recorded object|in DataObject" )

atlas_add_test( LArFebErrorSummaryCnv_p1_test
                SOURCES
                test/LArFebErrorSummaryCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArTPCnv TestTools )

atlas_add_test( LArNoisyROSummaryCnv_p1_test
                SOURCES
                test/LArNoisyROSummaryCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArTPCnv TestTools )

atlas_add_test( LArNoisyROSummaryCnv_p2_test
                SOURCES
                test/LArNoisyROSummaryCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel LArTPCnv TestTools )

atlas_add_test( LArNoisyROSummaryCnv_p3_test
                SOURCES
                test/LArNoisyROSummaryCnv_p3_test.cxx
                LINK_LIBRARIES GaudiKernel LArTPCnv TestTools )

atlas_add_test( LArNoisyROSummaryCnv_p4_test
                SOURCES
                test/LArNoisyROSummaryCnv_p4_test.cxx
                LINK_LIBRARIES GaudiKernel LArTPCnv TestTools )

atlas_add_test( LArNoisyROSummaryCnv_p5_test
                SOURCES
                test/LArNoisyROSummaryCnv_p5_test.cxx
                LINK_LIBRARIES GaudiKernel LArTPCnv TestTools )

atlas_add_test( LArRawChannelCnv_p2_test
                SOURCES
                test/LArRawChannelCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel LArTPCnv TestTools )

atlas_add_test( LArRawChannelContainerCnv_p4_test
                SOURCES
                test/LArRawChannelContainerCnv_p4_test.cxx
                LINK_LIBRARIES GaudiKernel LArTPCnv TestTools )

atlas_add_test( LArLATOMEHeaderContainerCnv_p1_test
                SOURCES
                test/LArLATOMEHeaderContainerCnv_p1_test.cxx
                LINK_LIBRARIES AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArSCDigitContainerCnv_p1_test
                SOURCES
                test/LArSCDigitContainerCnv_p1_test.cxx
                LINK_LIBRARIES AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )

atlas_add_test( LArRawSCContainerCnv_p1_test
                SOURCES
                test/LArRawSCContainerCnv_p1_test.cxx
                LINK_LIBRARIES AthenaPoolCnvSvcLib LArRawEvent LArRecEvent TestTools CaloIdentifier AthenaKernel Identifier LArTPCnv )
