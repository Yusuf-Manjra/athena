///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4ImplPtEtaPhiMCnv_p2.cxx 
// Implementation file for class P4ImplPtEtaPhiMCnv_p2
// Author: I. Vukotic <ivukotic@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#define private public
#define protected public
#include "FourMom/Lib/P4ImplPtEtaPhiM.h"
#undef protected
#undef private

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplPtEtaPhiMCnv_p2.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void 
P4ImplPtEtaPhiMCnv_p2::persToTrans( const P4PtEtaPhiMFloat_p2* persObj, 
				    P4ImplPtEtaPhiM* transObj, 
				    MsgStream &/*log*/ )
{
  transObj->m_pt  = (double) persObj->m_pt;
  transObj->m_eta = (double) persObj->m_eta;
  transObj->m_phi = (double) persObj->m_phi;
  transObj->m_m   = (double) persObj->m_mass;
  return;
}

void 
P4ImplPtEtaPhiMCnv_p2::transToPers( const P4ImplPtEtaPhiM* transObj, 
				    P4PtEtaPhiMFloat_p2* persObj, 
				    MsgStream &/*log*/ )
{
  persObj->m_pt   = (float) transObj->m_pt;
  persObj->m_eta  = (float) transObj->m_eta;
  persObj->m_phi  = (float) transObj->m_phi;
  persObj->m_mass = (float) transObj->m_m;
  return;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
