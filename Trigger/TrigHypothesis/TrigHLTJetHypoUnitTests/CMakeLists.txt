# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHLTJetHypoUnitTests )

# External dependencies:
find_package( ROOT COMPONENTS Core Physics )
find_package( GMock )
find_package( GTest )

# Componetns in the package:
atlas_add_library( TrigHLTJetHypoUnitTestsLib
                   src/*.cxx
                   exerciser/*.cxx
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrigHLTJetHypoLib )

atlas_add_component( TrigHLTJetHypoUnitTests
                     exerciser/components/*.cxx
                     LINK_LIBRARIES TrigHLTJetHypoLib TrigHLTJetHypoUnitTestsLib)

# Test(s) in the package:
atlas_add_test( TrigHLTJetHypoUnitTests
   SOURCES tests/all_tests.cxx
   tests/CombinationsGeneratorTest.cxx
   tests/ProductGenTest.cxx
   tests/JetGroupProductTest.cxx
   tests/JetGroupUnionTest.cxx
   tests/JetGroupReducerTest.cxx
   tests/JetGroupSingleCliqueTest.cxx
   tests/JetGroupRegisterTest.cxx
   tests/TLorentzVectorFactoryTest.cxx
   tests/xAODJetCollectorTest.cxx
   tests/JetTrigTimerTest.cxx
   tests/RangeFilterTest.cxx
   tests/PassThroughFilterTest.cxx
   tests/ConditionFilterTest.cxx
   tests/MultiFilterTest.cxx
   tests/JetStreamerTest.cxx
   tests/make_jetstreamTest.cxx
   tests/TreeTest.cxx
   tests/JetGroupProductFactoryTest.cxx	   
   INCLUDE_DIRS ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES} ${ROOT_LIBRARIES} TrigHLTJetHypoLib TrigHLTJetHypoUnitTestsLib xAODJet
   POST_EXEC_SCRIPT nopost.sh )
   
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
