///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCollectionCnv_p1.h 
// Header file for class JetCollectionCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_JETCOLLECTIONCNV_P1_H 
#define RECTPCNV_JETCOLLECTIONCNV_P1_H 

// STL includes

// Gaudi includes

// JetEvent includes
//#include "JetEvent/JetCollection.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
//#include "JetEventTPCnv/JetCollection_p1.h"
//#include "JetEventTPCnv/JetCnv_p1.h"

// forward declares
class JetCollection;
class JetCollection_p1;

class JetCollectionCnv_p1 : public T_AthenaPoolTPCnvConstBase<
                                       JetCollection, 
                                       JetCollection_p1
                                   >  
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetCollectionCnv_p1();


  /** Method creating the transient representation of @c JetCollection
   *  from its persistent representation @c JetCollection_p1
   */
  virtual void persToTrans( const JetCollection_p1* pers, 
                            JetCollection* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c JetCollection_p1
   *  from its transient representation @c JetCollection
   */
  virtual void transToPers( const JetCollection* trans, 
                            JetCollection_p1* pers, 
                            MsgStream& msg ) const override;
}; 


inline JetCollectionCnv_p1::JetCollectionCnv_p1()
{}

#endif //> RECTPCNV_JETCOLLECTIONCNV_P1_H
