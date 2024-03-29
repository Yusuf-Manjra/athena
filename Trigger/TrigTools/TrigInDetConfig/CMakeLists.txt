# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL901 )
atlas_install_joboptions( share/*.py)

# Tests in the package:
atlas_add_test( trigInDetFastTrackingCfg
   SCRIPT python -m TrigInDetConfig.TrigInDetConfig
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigTrackingPassFlags
   SCRIPT python -m unittest -v TrigInDetConfig.TrigTrackingPassFlags
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigTrackingPassFlagsITk
   SCRIPT python -m unittest -v TrigInDetConfig.TrigTrackingPassFlagsITk
   POST_EXEC_SCRIPT nopost.sh )
