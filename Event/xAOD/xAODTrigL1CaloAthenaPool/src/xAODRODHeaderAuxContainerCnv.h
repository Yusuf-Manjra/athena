/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODRODHEADERAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODRODHEADERAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODTrigL1Calo/RODHeaderAuxContainer.h"

/// Base class for the converter
typedef T_AthenaPoolCustomCnv< xAOD::RODHeaderAuxContainer,
                               xAOD::RODHeaderAuxContainer >
   xAODRODHeaderAuxContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::RODHeaderAuxContainer class
 *
 *         This is the converter doing the actual schema evolution
 *         of the package... The converter for xAOD::RODHeaderContainer
 *         doesn't do much, as the "interface classes" don't contain
 *         too much/any payload. Most of the payload is in the auxiliary
 *         containers like this one.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author John Morris <john.morris@cern.ch>
 *
 * $Revision: 576324 $
 * $Date: 2013-12-19 17:07:53 +0100 (Thu, 19 Dec 2013) $
 */
class xAODRODHeaderAuxContainerCnv :
   public xAODRODHeaderAuxContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODRODHeaderAuxContainerCnv >;

protected:
   /// Converter constructor
   xAODRODHeaderAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the container to be written out
   virtual xAOD::RODHeaderAuxContainer*
   createPersistent( xAOD::RODHeaderAuxContainer* trans );
   /// Function reading in the object from the input file
   virtual xAOD::RODHeaderAuxContainer* createTransient();

}; // class xAODRODHeaderAuxContainerCnv

#endif // XAODTRIGL1CALOATHENAPOOL_XAODRODHEADERAUXCONTAINERCNV_H
