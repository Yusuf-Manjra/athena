# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZLumiScripts )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_scripts( scripts/*.py scripts/Pandas_scripts/*py scripts/Pandas_scripts/plotting/dqt_csv_luminosity.py )
atlas_install_data( scripts/Pandas_scripts/plotting/AtlasStyle )