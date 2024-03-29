# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package
atlas_subdir( TrigGepPerf )

# External package dependencies:
find_package( ROOT COMPONENTS Core Math Physics )

atlas_add_component( TrigGepPerf 
		     src/*.cxx
		     src/components/*.cxx
  		     LINK_LIBRARIES
		     AthenaBaseComps
		     AthContainers
		     CaloDetDescrLib
		     CaloIdentifier
		     GaudiKernel
		     TrigT1Interfaces
		     TrigEFMissingETLib
		     xAODEventInfo
		     xAODJet
		     xAODTrigger
)

# Install python modules, joboptions, and share content
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )



