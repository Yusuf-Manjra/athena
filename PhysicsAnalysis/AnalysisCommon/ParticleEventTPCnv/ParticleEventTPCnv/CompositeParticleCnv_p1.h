///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticleCnv_p1.h 
// Header file for class CompositeParticleCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_COMPOSITEPARTICLECNV_P1_H 
#define PARTICLEEVENTTPCNV_COMPOSITEPARTICLECNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/CompositeParticle_p1.h"

// Forward declaration
class MsgStream;
class CompositeParticle;

class CompositeParticleCnv_p1 
  : public T_AthenaPoolTPCnvConstBase<CompositeParticle, CompositeParticle_p1>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  CompositeParticleCnv_p1();


  /** Method creating the transient representation of @c CompositeParticle
   *  from its persistent representation @c CompositeParticle_p1
   */
  virtual void persToTrans( const CompositeParticle_p1* persObj, 
                            CompositeParticle* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c CompositeParticle_p1
   *  from its transient representation @c CompositeParticle
   */
  virtual void transToPers( const CompositeParticle* transObj, 
                            CompositeParticle_p1* persObj, 
                            MsgStream& msg ) const override;
}; 


inline CompositeParticleCnv_p1::CompositeParticleCnv_p1()
{}

#endif //> PARTICLEEVENTTPCNV_COMPOSITEPARTICLECNV_P1_H
