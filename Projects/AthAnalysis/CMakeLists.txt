# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Set up the project.
cmake_minimum_required( VERSION 3.6 )
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
project( AthAnalysis VERSION ${_version} LANGUAGES C CXX Fortran )
unset( _version )

# HepMC3
set( HEPMC3_USE TRUE CACHE BOOL "Build with HepMC3" )

# Configure flake8:
set( ATLAS_FLAKE8 true   # <- this is the shell builtin `true`
   CACHE STRING "Default flake8 command" )
set( ATLAS_PYTHON_CHECKER ""
   CACHE STRING "Python checker command to run during Python module compilation" )

# Find the ATLAS CMake code:
find_package( AtlasCMake QUIET )

# Find the base project(s):
find_package( AthAnalysisExternals ${_version} REQUIRED )
find_package( Gaudi REQUIRED )

# Additional externals needed for the build/runtime:
find_package( Xrootd )
find_package( GSL )
find_package( Davix )
find_package( PNG )
find_package( BLAS )
find_package( VDT )
find_package( requests )
find_package( libffi )
find_package( gl2ps )
find_package( autopep8 )

# Compile flag(s).
add_definitions( -DXAOD_ANALYSIS )

# CMake cache variable(s).
set( XAOD_ANALYSIS TRUE CACHE BOOL
   "Flag specifying that this is an analysis release" )

# Set up where to find the AthenaPoolUtilitiesTest CMake code.
set( AthenaPoolUtilitiesTest_DIR
   "${CMAKE_SOURCE_DIR}/../../Database/AthenaPOOL/AthenaPoolUtilities/cmake"
   CACHE PATH "Directory holding the AthenaPoolUtilititesTest module" )

# Set up where to find the xAODUtilities CMake code.
set( xAODUtilities_DIR
   "${CMAKE_SOURCE_DIR}/../../Event/xAOD/xAODCore/cmake"
   CACHE PATH "Directory holding the xAODUtilities module" )

# Make the local CMake files visible to AtlasCMake.
list( INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake )

# Set up CTest.
atlas_ctest_setup()

# Set up the ATLAS project.
atlas_project( USE AthAnalysisExternals ${AthAnalysisExternals_VERSION}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../ )

# Generate the environment setup for the externals, to be used during the build.
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )

# Generate replacement rules for the installed paths.
set( _replacements )
if( NOT "$ENV{ATLAS_BUILD_DIR}" STREQUAL "" )
   list( APPEND _replacements "$ENV{ATLAS_BUILD_DIR}/install"
                              "\${AthAnalysis_DIR}/../../../.." )
endif()

# Now generate and install the installed setup files.
lcg_generate_env(
   SH_FILE ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env_setup_install.sh
   REPLACE ${_replacements} )
install( FILES ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env_setup_install.sh
   DESTINATION . RENAME env_setup.sh )

# Configure and install the project configuration file(s).
configure_file( ${CMAKE_SOURCE_DIR}/cmake/PreConfig.cmake.in
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PreConfig.cmake @ONLY )
configure_file( ${CMAKE_SOURCE_DIR}/cmake/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake @ONLY )
install( FILES
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PreConfig.cmake
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

# Install graphviz output if available.
install( FILES ${CMAKE_BINARY_DIR}/packages.dot
   DESTINATION . OPTIONAL )

# Setup IDE integration:
set( ATLAS_ENABLE_IDE_HELPERS OFF CACHE BOOL "Enable IDE helpers" )
if( ATLAS_ENABLE_IDE_HELPERS )
   set( ATLAS_IDEHELPERSCRIPTS_SETUP
      "${CMAKE_SOURCE_DIR}/../../.vscode/IDEHelperScripts/Setup.cmake"
      CACHE FILEPATH "Setup file for the IDE / VS Code helpers" )
   include( "${ATLAS_IDEHELPERSCRIPTS_SETUP}" )
endif()

# Package up the release using CPack.
atlas_cpack_setup()
