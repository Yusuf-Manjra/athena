/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETAuxComponentMapCnv.cxx 586838 2014-03-08 15:10:34Z khoo $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMissingETAuxComponentMapCnv.h"

xAODMissingETAuxComponentMapCnv::xAODMissingETAuxComponentMapCnv( ISvcLocator* svcLoc )
   : xAODMissingETAuxComponentMapCnvBase( svcLoc ) {

}

xAOD::MissingETAuxComponentMap*
xAODMissingETAuxComponentMapCnv::
createPersistent( xAOD::MissingETAuxComponentMap* trans ) {

   // Create a copy of the container:
   xAOD::MissingETAuxComponentMap* result =
      new xAOD::MissingETAuxComponentMap( *trans );

   return result;
}

xAOD::MissingETAuxComponentMap* xAODMissingETAuxComponentMapCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "96287268-4165-4288-8FC1-6227FD4AB95D" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::MissingETAuxComponentMap >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::MissingETAuxComponentMap found" );
   return 0;
}
