/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMMCPHitsContainerCnv.h"

namespace {
  /// Helper function setting up the container's link to its auxiliary store
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    
    // The link to set up:
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
    
    // Give it to the container:
    cont->setStore( link );

    return;
  }   
} // private namespace


xAODCMMCPHitsContainerCnv::xAODCMMCPHitsContainerCnv( ISvcLocator* svcLoc )
   : xAODCMMCPHitsContainerCnvBase( svcLoc ) {
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODCMMCPHitsContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::CMMCPHitsContainer*
xAODCMMCPHitsContainerCnv::
createPersistent( xAOD::CMMCPHitsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::CMMCPHitsContainer* result =
      new xAOD::CMMCPHitsContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::CMMCPHitsContainer::iterator itr = result->begin();
   xAOD::CMMCPHitsContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::CMMCPHitsContainer* xAODCMMCPHitsContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "8826CC48-CD08-4FD7-9AC4-358A4888271F" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
     xAOD::CMMCPHitsContainer* c = poolReadObject< xAOD::CMMCPHitsContainer >();
     setStoreLink( c , m_key );
     return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CMMCPHitsContainer found" );
   return 0;
}

void xAODCMMCPHitsContainerCnv::
toPersistent( xAOD::CMMCPHits* /*cluster*/ ) const {

   return;
}
