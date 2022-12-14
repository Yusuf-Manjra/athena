# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This module is used to set up the environment for Superchic
# 
#

# Set the environment variable(s):
find_package( Superchic )

if( SUPERCHIC_FOUND )
  set( SUPERCHICENVIRONMENT_ENVIRONMENT 
        FORCESET SUPERCHICVER ${SUPERCHIC_LCGVERSION} 
        FORCESET SUPERCHICPATH ${SUPERCHIC_LCGROOT} 
        APPEND LHAPATH
              /cvmfs/atlas.cern.ch/repo/sw/Generators/lhapdfsets/current
        APPEND LHAPATH ${LHAPDF_DATA_PATH}
        APPEND LHAPDF_DATA_PATH
              /cvmfs/atlas.cern.ch/repo/sw/Generators/lhapdfsets/current
        APPEND LHAPDF_DATA_PATH ${LHAPDF_DATA_PATH} )
endif()


# Silently declare the module found:
set( SUPERCHICENVIRONMENT_FOUND TRUE )


