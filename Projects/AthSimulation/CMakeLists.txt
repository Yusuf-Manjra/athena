# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Set up the project.
cmake_minimum_required( VERSION 3.6 )
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
project( AthSimulation VERSION ${_version} LANGUAGES C CXX Fortran )
unset( _version )

# HepMC3
set( HEPMC3_USE TRUE CACHE BOOL "Build with HepMC3" )

# Configure flake8:
set( ATLAS_FLAKE8 flake8_atlas --isolated --select ATL,F,E101,E7,E9,W6
                               --ignore E701,E702,E704,E741
                               --enable-extensions ATL902
   CACHE STRING "Default flake8 command" )
set( ATLAS_PYTHON_CHECKER ${ATLAS_FLAKE8}
   CACHE STRING "Python checker command to run during Python module compilation" )

# Find the ATLAS CMake code:
find_package( AtlasCMake QUIET )

# Find the base project(s):
find_package( AthSimulationExternals REQUIRED )
find_package( Gaudi REQUIRED )

# External(s) needed at build and runtime:
find_package( Frontier_Client )
find_package( PNG )
find_package( VDT )
find_package( TIFF )
find_package( libffi )
find_package( gl2ps )
find_package( autopep8 )

# Set the project into "SIMULATIONBASE mode".
set( SIMULATIONBASE TRUE CACHE BOOL
   "Flag specifying that this is a simulation release build" )
if( SIMULATIONBASE )
   add_definitions( -DSIMULATIONBASE )
endif()

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

# Set up CTest:
atlas_ctest_setup()

# Declare project name and version
atlas_project( USE AthSimulationExternals ${AthSimulationExternals_VERSION}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../ )

# Generate the environment setup for the externals, to be used during the build:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )

# Generate replacement rules for the installed paths:
set( _replacements )
if( NOT "$ENV{ATLAS_BUILD_DIR}" STREQUAL "" )
   list( APPEND _replacements "$ENV{ATLAS_BUILD_DIR}/install"
                              "\${AthSimulation_DIR}/../../../.." )
endif()

# Now generate and install the installed setup files:
lcg_generate_env(
   SH_FILE ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env_setup_install.sh
   REPLACE ${_replacements} )
install( FILES ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env_setup_install.sh
   DESTINATION . RENAME env_setup.sh )

# Configure and install the post-configuration file:
configure_file( ${CMAKE_SOURCE_DIR}/cmake/PreConfig.cmake.in
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PreConfig.cmake @ONLY )
configure_file( ${CMAKE_SOURCE_DIR}/cmake/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake @ONLY )
install( FILES
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PreConfig.cmake
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

# Install graphviz output if available:
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

# Package up the release using CPack:
atlas_cpack_setup()
