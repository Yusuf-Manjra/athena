# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloDetMgrDetDescrCnv )

# Component(s) in the package:
atlas_add_component( CaloDetMgrDetDescrCnv
                     src/*.cxx
                     LINK_LIBRARIES DetDescrCnvSvcLib GaudiKernel CaloDetDescrLib CaloIdentifier CaloDetDescrUtils AthenaKernel AthenaBaseComps StoreGateLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

