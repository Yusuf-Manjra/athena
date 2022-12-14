# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Build configuration for package AtlasAuthentication.
#

# Declare the package's name:
atlas_subdir( AtlasAuthentication )

# Install the local file(s) of the package:
atlas_install_xmls( data/*.xml )
atlas_install_runtime( data/dbreplica.config )
atlas_install_scripts( share/setupLocalDBReplica_CERN.sh )

# Find the ATLAS externals area:
if( NOT SITEROOT )
   if( NOT "$ENV{SITEROOT}" STREQUAL "" )
      set( SITEROOT "$ENV{SITEROOT}" )
   else()
      set( SITEROOT "/afs/cern.ch" )
   endif()
endif()
if( NOT ATLAS_EXTERNAL )
   if( NOT "$ENV{ATLAS_EXTERNAL}" STREQUAL "" )
      set( ATLAS_EXTERNAL "$ENV{ATLAS_EXTERNAL}" )
   else()
      set( ATLAS_EXTERNAL "${SITEROOT}/atlas/offline/external" )
   endif()
endif()

# Pick up authentication.xml:
set( AtlasAuthentication_native_version "v21" )
set( AtlasAuthentication_home
   "${ATLAS_EXTERNAL}/AtlasAuth/${AtlasAuthentication_native_version}" )
if (EXISTS ${AtlasAuthentication_home}/authentication.xml )
   atlas_install_xmls( ${AtlasAuthentication_home}/authentication.xml )
else()
   message(WARNING "The required authentication file ${AtlasAuthentication_home}/authentication.xml does not exist!")
endif()
# Configure the environment setup module:
configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/AtlasAuthenticationEnvironmentConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/AtlasAuthenticationEnvironmentConfig.cmake
   @ONLY )

# And now "find" it:
set( AtlasAuthenticationEnvironment_DIR ${CMAKE_CURRENT_BINARY_DIR}
   CACHE PATH "Location of AtlasAuthenticationEnvironmentConfig.cmake" )
find_package( AtlasAuthenticationEnvironment )
