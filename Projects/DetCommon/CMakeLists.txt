# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Set up the project.
cmake_minimum_required( VERSION 3.10 )
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
project( DetCommon VERSION ${_version} LANGUAGES C CXX )
unset( _version )

# Set the version of tdaq-common to use for the build:
set( TDAQ-COMMON_VERSION "10-00-00" CACHE STRING
   "The version of tdaq-common to use for the build" )
set( TDAQ-COMMON_ATROOT
   "$ENV{TDAQ_RELEASE_BASE}/tdaq-common/tdaq-common-${TDAQ-COMMON_VERSION}"
   CACHE PATH "The directory to pick up tdaq-common from" )

# Find the ATLAS CMake code:
find_package( AtlasCMake QUIET )

# Build the project against LCG:
set( LCG_VERSION_POSTFIX "b"   # Needs to match tdaq release (NO layer, ATLINFR-3977)
   CACHE STRING "Version postfix for the LCG release to use" )
set( LCG_VERSION_NUMBER 102
   CACHE STRING "Version number for the LCG release to use" )
# Reduced set of LCG components (i.e. no generators):
set( LCG_COMPONENTS "externals"
   CACHE STRING "LCG components to set up" )

find_package( LCG ${LCG_VERSION_NUMBER} EXACT REQUIRED )

# Set up tdaq-common explicitly.
find_package( tdaq-common REQUIRED )

# Find some auxiliary packages:
find_package( XercesC )
find_package( libffi )
find_package( gl2ps )

# Set up the build/runtime environment:
set( DetCommonReleaseEnvironment_DIR ${CMAKE_SOURCE_DIR}/cmake CACHE PATH
   "Path to DetCommonReleaseEnvironmentConfig.cmake" )
find_package( DetCommonReleaseEnvironment REQUIRED )

# Make the local CMake files visible to AtlasCMake.
list( INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake )

# Set up CTest:
atlas_ctest_setup()

# Set up a work directory project:
atlas_project( PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../ )

# Disable flake8:
set( ATLAS_FLAKE8 true   # <- this is the shell builtin `true`
   CACHE STRING "Default flake8 command" )
set( ATLAS_PYTHON_CHECKER ""
   CACHE STRING "Python checker command to run during Python module compilation" )

# Generate the environment setup for the externals, to be used during the build:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )

# Generate replacement rules for the installed paths:
set( _replacements )
if( NOT "$ENV{ATLAS_BUILD_DIR}" STREQUAL "" )
   list( APPEND _replacements "$ENV{ATLAS_BUILD_DIR}/install"
                              "\${DetCommon_DIR}/../../../.." )
endif()
if( NOT "$ENV{TDAQ_RELEASE_BASE}" STREQUAL "" )
   list( APPEND _replacements $ENV{TDAQ_RELEASE_BASE}
      "\${TDAQ_RELEASE_BASE}" )
endif()

# Now generate and install the installed setup files:
lcg_generate_env(
   SH_FILE ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env_setup_install.sh
   REPLACE ${_replacements} )
install( FILES ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env_setup_install.sh
   DESTINATION . RENAME env_setup.sh )

# Configure and install the pre- and post-configuration files:
string( REPLACE "$ENV{TDAQ_RELEASE_BASE}" "\$ENV{TDAQ_RELEASE_BASE}"
   TDAQ-COMMON_ATROOT "${TDAQ-COMMON_ATROOT}" )
string( REPLACE "${TDAQ-COMMON_VERSION}" "\${TDAQ-COMMON_VERSION}"
   TDAQ-COMMON_ATROOT "${TDAQ-COMMON_ATROOT}" )

configure_file( ${CMAKE_SOURCE_DIR}/cmake/PreConfig.cmake.in
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PreConfig.cmake @ONLY )
configure_file( ${CMAKE_SOURCE_DIR}/cmake/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake @ONLY )
install( FILES
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PreConfig.cmake
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

# Setup IDE integration:
set( ATLAS_ENABLE_IDE_HELPERS OFF CACHE BOOL "Enable IDE helpers" )
if( ATLAS_ENABLE_IDE_HELPERS )
   set( ATLAS_IDEHELPERSCRIPTS_SETUP
      "${CMAKE_SOURCE_DIR}/../../.vscode/IDEHelperScripts/Setup.cmake"
      CACHE FILEPATH "Setup file for the IDE / VS Code helpers" )
   include( "${ATLAS_IDEHELPERSCRIPTS_SETUP}" )
endif()

# Package up the release using CPack:
atlas_cpack_setup()
