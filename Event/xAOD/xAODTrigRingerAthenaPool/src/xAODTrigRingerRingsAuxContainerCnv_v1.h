/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGERATHENAPOOL_XAODTRIGRINGERRINGSAUXCONTAINERCNV_V1_H
#define XAODTRIGRINGERATHENAPOOL_XAODTRIGRINGERRINGSAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v1.h"
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"

/// Converter class used for reading xAOD::EmTauRoIAuxContainer_v1
///
class xAODTrigRingerRingsAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvBase< xAOD::TrigRingerRingsAuxContainer,
                                 xAOD::TrigRingerRingsAuxContainer_v1 > {

public:
   /// Default constructor
   xAODTrigRingerRingsAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::TrigRingerRingsAuxContainer_v1* oldObj,
                                   xAOD::TrigRingerRingsAuxContainer* newObj,
                                   MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::TrigRingerRingsAuxContainer*,
                                   xAOD::TrigRingerRingsAuxContainer_v1*,
                                   MsgStream& log );

}; // class
#endif 
