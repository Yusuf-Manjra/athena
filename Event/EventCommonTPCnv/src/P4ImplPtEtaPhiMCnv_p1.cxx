///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4ImplPtEtaPhiMCnv_p1.cxx 
// Implementation file for class P4ImplPtEtaPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "FourMom/Lib/P4ImplPtEtaPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplPtEtaPhiMCnv_p1.h"


void 
P4ImplPtEtaPhiMCnv_p1::persToTrans( const P4ImplPtEtaPhiM_p1* persObj, 
				    P4ImplPtEtaPhiM* transObj, 
				    MsgStream &/*msg*/ ) const
{
  transObj->m_pt  = persObj->m_pt;
  transObj->m_eta = persObj->m_eta;
  transObj->m_phi = persObj->m_phi;
  transObj->m_m   = persObj->m_mass;
  return;
}

void 
P4ImplPtEtaPhiMCnv_p1::transToPers( const P4ImplPtEtaPhiM* transObj, 
				    P4ImplPtEtaPhiM_p1* persObj, 
				    MsgStream &/*msg*/ ) const
{
  persObj->m_pt   = transObj->m_pt;
  persObj->m_eta  = transObj->m_eta;
  persObj->m_phi  = transObj->m_phi;
  persObj->m_mass = transObj->m_m;
  return;
}
