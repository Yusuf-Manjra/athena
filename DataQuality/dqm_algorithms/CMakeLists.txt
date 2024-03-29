# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( dqm_algorithms )

# External dependencies:
find_package( Boost COMPONENTS program_options filesystem thread system )
find_package( ROOT COMPONENTS MathCore Hist Core Tree RIO pthread )
find_package( tdaq-common COMPONENTS dqm_core_io dqm_core dqm_dummy_io dqm_dummy )
find_package( COOL COMPONENTS CoolKernel CoolApplication )

# Component(s) in the package:
atlas_add_library( dqm_algorithms
                   src/*.cxx
                   tools/AlgorithmHelper.cxx
                   tools/SimpleAlgorithmConfig.cxx
                   PUBLIC_HEADERS dqm_algorithms
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${COOL_LIBRARIES} CxxUtils
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_library( dqm_tools
                   tools/DumpConfig.cxx
                   PUBLIC_HEADERS dqm_algorithms
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_library( dqm_summaries
                   summary/*.cxx
                   PUBLIC_HEADERS dqm_algorithms
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms dqm_tools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( dqm_algorithmsDict
                      dqm_algorithms/dqm_algorithmsDict.h
                      dqm_algorithms/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms dqm_tools dqm_summaries )

atlas_add_dictionary( dqm_toolsDict
                      dqm_algorithms/tools/dqm_toolsDict.h
                      dqm_algorithms/tools/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms dqm_tools dqm_summaries )

atlas_add_executable( dqm_print_algorithms
                      test/print_algorithms.cpp
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms dqm_tools dqm_summaries )

atlas_add_executable( dqm_print_summarymakers
                      test/print_summarymakers.cpp
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms dqm_tools dqm_summaries )

