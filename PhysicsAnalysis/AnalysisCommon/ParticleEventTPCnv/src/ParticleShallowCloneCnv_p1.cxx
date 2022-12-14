///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowCloneCnv_p1.cxx 
// Implementation file for class ParticleShallowCloneCnv_p1
// Author: Karsten Koeneke - February 2013
/////////////////////////////////////////////////////////////////// 


// STL includes

// ParticleEvent includes
#include "ParticleEvent/ParticleShallowClone.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PxPyPzECnv_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleShallowCloneCnv_p1.h"


typedef ElementLinkCnv_p3<MasterLink_t> MasterLinkCnv_t;
typedef ElementLinkCnv_p3< ElementLink< VxContainer > > OriginLinkCnv_t;


// pre-allocate converters
static const P4PxPyPzECnv_p1 s_momCnv;
static const MasterLinkCnv_t s_masterLinkCnv;
static const OriginLinkCnv_t s_originLinkCnv;


void ParticleShallowCloneCnv_p1::persToTrans( const ParticleShallowClone_p1* pers,
                                              Analysis::ParticleShallowClone* trans, 
                                              MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading ParticleShallowClone from persistent state..."
//       << endmsg;

  // convert the 4Momentum part
  s_momCnv.persToTrans( &pers->m_momentum,
                        trans,
                        msg );
  
  // Convert the ElementLink to the master particle
  s_masterLinkCnv.persToTrans( &pers->m_masterLink,
                               &trans->m_masterLink,
                               msg );
  
  // Convert the ElementLink to the vertex
  s_originLinkCnv.persToTrans( &pers->m_originLink,
                               &trans->m_originLink,
                               msg );
  
  // Convert the other attributes
  trans->m_hasCharge = pers->m_hasCharge;
  trans->m_charge    = static_cast<ChargeType>(pers->m_charge);

//   msg << MSG::DEBUG << "Loaded ParticleShallowClone from persistent state [OK]"
//       << endmsg;
  return;
}

void ParticleShallowCloneCnv_p1::transToPers( const Analysis::ParticleShallowClone* trans, 
                                              ParticleShallowClone_p1* pers, 
                                              MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of ParticleShallowClone..."
//       << endmsg;

  // convert the 4Momentum part
  s_momCnv.transToPers( &trans->momentumBase(),
                        &pers->m_momentum,
                        msg );

  // Convert the ElementLink to the master particle
  s_masterLinkCnv.transToPers( &trans->m_masterLink,
                               &pers->m_masterLink,
                               msg );
  
  // Convert the ElementLink to the vertex
  s_originLinkCnv.transToPers( &trans->m_originLink,
                               &pers->m_originLink,
                               msg );
  
  // Convert the other attributes
  pers->m_hasCharge = trans->m_hasCharge;
  pers->m_charge    = static_cast<float>(trans->m_charge);

//   msg << MSG::DEBUG << "Created persistent state of ParticleShallowClone [OK]"
//       << endmsg;
  return;
}
