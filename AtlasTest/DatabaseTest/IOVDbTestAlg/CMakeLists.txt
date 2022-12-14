# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IOVDbTestAlg )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( IOVDbTestAlg
                     src/IOVDbTestAlg.cxx
                     src/IOVDbTestCoolDCS.cxx
                     src/IOVDbTestAlg_entries.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} IOVDbTestConditions AthenaBaseComps AthenaKernel StoreGateLib GaudiKernel AthenaPoolUtilities RegistrationServicesLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

function (iovdbtestalg_run_test testName jo)
  cmake_parse_arguments( ARG "" "DEPENDS" "" ${ARGN} )

  atlas_add_test( ${testName}
                  SCRIPT "athena.py IOVDbTestAlg/${jo}.py"
                  LOG_SELECT_PATTERN "^IOVDbTestAlg"
                  PROPERTIES TIMEOUT 300
                  ENVIRONMENT "POOL_OUTMSG_LEVEL=4"
                  DEPENDS ${ARG_DEPENDS} )
endfunction (iovdbtestalg_run_test)

# Write out some simple objects and register them in IOVDb - COOL version:
iovdbtestalg_run_test( IOVDbTestWriteCool IOVDbTestAlgWriteCool )
# Read back the object using IOVDb - COOL version:
iovdbtestalg_run_test( IOVDbTestReadCool IOVDbTestAlgReadCool
                       DEPENDS IOVDbTestWriteCool )

# Write out some the same simple objects and register them with a later IOV in IOVDb - COOL version:
iovdbtestalg_run_test( IOVDbTestWriteCoolStep2 IOVDbTestAlgWriteCoolStep2
                       DEPENDS IOVDbTestReadCool )
# Read back the object using IOVDb - COOL version:
iovdbtestalg_run_test( IOVDbTestReadCool2 IOVDbTestAlgReadCool
                       DEPENDS IOVDbTestWriteCoolStep2 )

# Write out some the same simple objects and register them with a later IOV in IOVDb - COOL version:
iovdbtestalg_run_test( IOVDbTestWriteCoolStep3 IOVDbTestAlgWriteCoolStep3
                       DEPENDS IOVDbTestReadCool2 )
# Read back the object using IOVDb - COOL version:
iovdbtestalg_run_test( IOVDbTestReadCool3 IOVDbTestAlgReadCool
                       DEPENDS IOVDbTestWriteCoolStep3 )

# Write to file meta data
iovdbtestalg_run_test( IOVDbTestReadCoolWriteMD IOVDbTestAlgReadCoolWriteMD
                       DEPENDS IOVDbTestReadCool3 )
# Read back from file meta data
iovdbtestalg_run_test( IOVDbTestReadCoolFromMD IOVDbTestAlgReadCoolFromMetaData
                       DEPENDS IOVDbTestReadCoolWriteMD )

# TwoStep write/reg:

# Write out some the same simple objects BUT DO NOT register them
iovdbtestalg_run_test( IOVDbTestAlgWriteCoolNoReg IOVDbTestAlgWriteCoolNoReg
                       DEPENDS IOVDbTestReadCoolFromMD )
# Read back objects NOT registered
#iovdbtestalg_run_test( IOVDbTestAlgReadCoolNoReg IOVDbTestAlgReadCoolNoReg )

# Read back objects NOT registered and NOW register them
#iovdbtestalg_run_test( IOVDbTestAlgReadCoolAndReg IOVDbTestAlgReadCoolAndReg )

# Read back objects via IOVDB
iovdbtestalg_run_test( IOVDbTestAlgReadCoolAfterTwoStep IOVDbTestAlgReadCoolAfterTwoStep
                       DEPENDS IOVDbTestAlgWriteCoolNoReg )
