///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4ImplPtEtaPhiMCnv_p1.h 
// Header file for class P4ImplPtEtaPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4IMPLPTETAPHIMCNV_P1_H 
#define EVENTCOMMONTPCNV_P4IMPLPTETAPHIMCNV_P1_H 

// STL includes


// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// FourMom includes
#include "FourMom/Lib/P4ImplPtEtaPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"

// Forward declaration
class MsgStream;

// to get the same look'n feel (even if we are reusing the same _p1 class)
typedef P4PtEtaPhiM_p1 P4ImplPtEtaPhiM_p1;

class P4ImplPtEtaPhiMCnv_p1 : public T_AthenaPoolTPCnvConstBase<
                                              P4ImplPtEtaPhiM, 
                                              P4ImplPtEtaPhiM_p1
                                            >  
{ 
 public: 
  using base_class::transToPers; 
  using base_class::persToTrans;


  /** Default constructor: 
   */
  P4ImplPtEtaPhiMCnv_p1();

  /** Destructor: 
   */
  virtual ~P4ImplPtEtaPhiMCnv_p1();


  /** Method creating the transient representation of @c P4ImplPtEtaPhiM
   *  from its persistent representation @c P4ImplPtEtaPhiM_p1
   */
  virtual void persToTrans( const P4ImplPtEtaPhiM_p1* persObj, 
                            P4ImplPtEtaPhiM* transObj, 
                            MsgStream &msg ) const override;

  /** Method creating the persistent representation @c P4ImplPtEtaPhiM_p1
   *  from its transient representation @c P4ImplPtEtaPhiM
   */
  virtual void transToPers( const P4ImplPtEtaPhiM* transObj, 
                            P4ImplPtEtaPhiM_p1* persObj, 
                            MsgStream &msg ) const override;
}; 


inline P4ImplPtEtaPhiMCnv_p1::P4ImplPtEtaPhiMCnv_p1() 
{}

inline P4ImplPtEtaPhiMCnv_p1::~P4ImplPtEtaPhiMCnv_p1()
{}

#endif //> EVENTCOMMONTPCNV_P4IMPLPTETAPHIMCNV_P1_H
