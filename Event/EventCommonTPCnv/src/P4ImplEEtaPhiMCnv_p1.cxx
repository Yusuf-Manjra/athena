///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4ImplEEtaPhiMCnv_p1.cxx 
// Implementation file for class P4ImplEEtaPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "FourMom/Lib/P4ImplEEtaPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplEEtaPhiMCnv_p1.h"


void 
P4ImplEEtaPhiMCnv_p1::persToTrans( const P4ImplEEtaPhiM_p1* persObj, 
				   P4ImplEEtaPhiM* transObj, 
				   MsgStream &/*msg*/ ) const
{
  transObj->m_e   = persObj->m_e;
  transObj->m_eta = persObj->m_eta;
  transObj->m_phi = persObj->m_phi;
  transObj->m_m   = persObj->m_m;
  return;
}

void 
P4ImplEEtaPhiMCnv_p1::transToPers( const P4ImplEEtaPhiM* transObj, 
				   P4ImplEEtaPhiM_p1* persObj, 
				   MsgStream &/*msg*/ ) const
{
  persObj->m_e   = transObj->m_e;
  persObj->m_eta = transObj->m_eta;
  persObj->m_phi = transObj->m_phi;
  persObj->m_m   = transObj->m_m;
  return;
}
