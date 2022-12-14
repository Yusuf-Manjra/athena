// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonAuxContainerCnv_v1.h 619884 2014-10-03 15:00:40Z christos $
#ifndef XAODEGAMMAATHENAPOOL_XAODPHOTONAUXCONTAINERCNV_V1_H
#define XAODEGAMMAATHENAPOOL_XAODPHOTONAUXCONTAINERCNV_V1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODEgamma/versions/PhotonAuxContainer_v1.h"
#include "xAODEgamma/PhotonAuxContainer.h"

/// Converter class used for reading xAOD::PhotonAuxContainer_v1
///
/// This converter implements the conversion from xAOD::PhotonAuxContainer_v1
/// to the latest version of the class. In a way that makes this converter
/// usable both from the POOL converter, and the BS converter.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 619884 $
/// $Date: 2014-10-03 17:00:40 +0200 (Fri, 03 Oct 2014) $
///
class xAODPhotonAuxContainerCnv_v1 :
   public T_AthenaPoolTPCnvConstBase< xAOD::PhotonAuxContainer,
                                      xAOD::PhotonAuxContainer_v1 > {

public:
  using base_class::transToPers;
  using base_class::persToTrans;


   /// Default constructor
   xAODPhotonAuxContainerCnv_v1();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::PhotonAuxContainer_v1* oldObj,
                             xAOD::PhotonAuxContainer* newObj,
                             MsgStream& log ) const override;
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::PhotonAuxContainer*,
                             xAOD::PhotonAuxContainer_v1*,
                             MsgStream& log ) const override;

}; // class xAODPhotonAuxContainerCnv_v1

#endif // XAODEGAMMAATHENAPOOL_XAODPHOTONAUXCONTAINERCNV_V1_H
