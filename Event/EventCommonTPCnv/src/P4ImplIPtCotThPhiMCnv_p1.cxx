///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4ImplIPtCotThPhiMCnv_p1.cxx 
// Implementation file for class P4ImplIPtCotThPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "FourMom/Lib/P4ImplIPtCotThPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplIPtCotThPhiMCnv_p1.h"


void 
P4ImplIPtCotThPhiMCnv_p1::persToTrans( const P4ImplIPtCotThPhiM_p1* persObj, 
				       P4ImplIPtCotThPhiM* transObj, 
				       MsgStream &/*msg*/ ) const
{
  transObj->m_iPt   = persObj->m_iPt;
  transObj->m_cotTh = persObj->m_cotTh;
  transObj->m_phi   = persObj->m_phi;
  transObj->m_m     = persObj->m_mass;
  return;
}

void 
P4ImplIPtCotThPhiMCnv_p1::transToPers( const P4ImplIPtCotThPhiM* transObj, 
				       P4ImplIPtCotThPhiM_p1* persObj, 
				       MsgStream &/*log*/ ) const
{
  persObj->m_iPt   = transObj->m_iPt;
  persObj->m_cotTh = transObj->m_cotTh;
  persObj->m_phi   = transObj->m_phi;
  persObj->m_mass  = transObj->m_m;
  return;
}
