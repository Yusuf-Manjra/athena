/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODL2StandAloneMuonAuxContainerCnv.cxx 635805 2014-12-12 22:15:16Z ssnyder $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODL2StandAloneMuonAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODL2StandAloneMuonAuxContainerCnv::
xAODL2StandAloneMuonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODL2StandAloneMuonAuxContainerCnvBase( svcLoc ) {

}

xAOD::L2StandAloneMuonAuxContainer*
xAODL2StandAloneMuonAuxContainerCnv::
createPersistent( xAOD::L2StandAloneMuonAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::L2StandAloneMuonAuxContainer*
xAODL2StandAloneMuonAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "69F56941-D2B5-4C70-BFBD-605CB64A3DB8" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::L2StandAloneMuonAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::L2StandAloneMuonAuxContainer found" );
   return 0;
}
