# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

atlas_subdir( TrigMuonMonitoring )

atlas_add_component( TrigMuonMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringLib FourMomUtils GaudiKernel StoreGateLib TrigDecisionToolLib TrkExInterfaces TrkParameters xAODMuon xAODTracking xAODTrigMuon xAODTrigger MuonAnalysisInterfacesLib )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
