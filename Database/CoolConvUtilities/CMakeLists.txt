# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CoolConvUtilities )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Hist Tree RIO Core MathCore pthread )

# Component(s) in the package:
atlas_add_executable( AtlCoolCopy
                      src/AtlCoolCopy.cxx
                      src/ReplicaSorter.cxx
                      src/CoolTagInfo.cxx
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} FileCatalog PersistencySvc CoraCool CxxUtils )
set_target_properties( AtlCoolCopy PROPERTIES ENABLE_EXPORTS True )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/coolDumpFolderTags.py )
atlas_install_scripts( share/AtlCoolConsole.py share/coolIOVToTimeStamp.py share/coolTimeStampLocalToIOV.py share/coolTimeStampUTCToIOV.py share/RunList.py share/AtlCoolCheckFiles.py share/AtlCoolTopTag.py share/AtlCoolMakeReplica.py share/coolMagneticField.py share/AtlCoolCopy_test.sh )

atlas_add_test( AtlCoolCopy_test
                SCRIPT share/AtlCoolCopy_test.sh
                LOG_IGNORE_PATTERN "Cannot find plugin for connection string .oracle"
                PROPERTIES TIMEOUT 300 )
