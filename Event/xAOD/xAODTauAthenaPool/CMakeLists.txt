# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTauAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTauAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTau/TauJetContainer.h xAODTau/TauJetAuxContainer.h
         xAODTau/DiTauJetContainer.h xAODTau/DiTauJetAuxContainer.h
         xAODTau/TauTrackContainer.h xAODTau/TauTrackAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::TauJetContainer xAOD::TauJetAuxContainer
                        xAOD::DiTauJetContainer xAOD::DiTauJetAuxContainer
                        xAOD::TauTrackContainer xAOD::TauTrackAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel StoreGateLib AthenaPoolCnvSvcLib
                  AthenaPoolUtilities xAODTracking xAODTau GaudiKernel )

# Install files from the package.
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTAUATHENAPOOL_REFERENCE_TAG
       xAODTauAthenaPoolReference-01-01-00 )
  run_tpcnv_test( xAODTauAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODTauAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTAUATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( xAODTauAthenaPool_20.7.2.2   AOD-20.7.2.2-full
                   REQUIRED_LIBRARIES xAODTauAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTAUATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( xAODTauAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTauAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTAUATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
