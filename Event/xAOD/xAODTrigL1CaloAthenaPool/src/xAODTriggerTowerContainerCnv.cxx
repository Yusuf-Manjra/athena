/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTriggerTowerContainerCnv.h"

xAODTriggerTowerContainerCnv::xAODTriggerTowerContainerCnv( ISvcLocator* svcLoc )
   : xAODTriggerTowerContainerCnvBase( svcLoc ) {
}

xAOD::TriggerTowerContainer*
xAODTriggerTowerContainerCnv::
createPersistent( xAOD::TriggerTowerContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TriggerTowerContainer* result =
      new xAOD::TriggerTowerContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::TriggerTowerContainer::iterator itr = result->begin();
   xAOD::TriggerTowerContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::TriggerTowerContainer* xAODTriggerTowerContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "DE2A7891-B5FE-4811-8178-EF106743312C" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      return poolReadObject< xAOD::TriggerTowerContainer >();
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TriggerTowerContainer found" );
   return 0;
}

void xAODTriggerTowerContainerCnv::
toPersistent( xAOD::TriggerTower* /*cluster*/ ) const {

   return;
}
