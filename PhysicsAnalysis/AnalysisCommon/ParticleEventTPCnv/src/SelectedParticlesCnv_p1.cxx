///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// SelectedParticlesCnv_p1.cxx 
// Implementation file for class SelectedParticlesCnv_p1
// Author: Serban Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 


// STL includes

// ParticleEvent includes
#include "ParticleEvent/SelectedParticles.h"
#include "ParticleEventTPCnv/SelectedParticlesCnv_p1.h"
#include "GaudiKernel/MsgStream.h"

void 
SelectedParticlesCnv_p1::persToTrans( const SelectedParticles_p1* pers,
			     SelectedParticles* trans, 
			     MsgStream& msg ) const
{
   msg << MSG::DEBUG << "Creating transient state of SelectedParticles"
       << endmsg;
   trans->Set (pers->m_bits);
  return;
}

void 
SelectedParticlesCnv_p1::transToPers( const SelectedParticles* trans, 
			     SelectedParticles_p1* pers, 
			     MsgStream& msg ) const
{
   msg << MSG::DEBUG << "Creating persistent state of SelectedParticles"
       << endmsg;
   pers->m_bits=trans->AllBits();
  return;
}
