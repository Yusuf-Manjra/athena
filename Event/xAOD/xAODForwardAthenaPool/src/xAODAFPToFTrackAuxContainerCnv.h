/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPTOFTRACKAUXCONTAINERCNV_H
#define AFPTTOFRACKAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "xAODForward/AFPToFTrackAuxContainer.h"

typedef T_AthenaPoolCustomCnv<xAOD::AFPToFTrackAuxContainer, xAOD::AFPToFTrackAuxContainer> xAODAFPToFTrackAuxContainerCnvBase;


class xAODAFPToFTrackAuxContainerCnv :
  public xAODAFPToFTrackAuxContainerCnvBase {
 
  // Declare the factory as our friend:
  friend class CnvFactory< xAODAFPToFTrackAuxContainerCnv >;
 
protected:
  /// Converter constructor
  xAODAFPToFTrackAuxContainerCnv( ISvcLocator* svcLoc );
 
  /// Function preparing the container to be written out
  virtual xAOD::AFPToFTrackAuxContainer*
  createPersistent( xAOD::AFPToFTrackAuxContainer* trans );
  
  /// Function reading in the object from the input file
  virtual xAOD::AFPToFTrackAuxContainer* createTransient();
 
};
 
#endif
