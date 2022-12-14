/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigPhotonContainerCnv.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_p3.h"

//createPersistent
TrigPhotonContainer_PERS * TrigPhotonContainerCnv::createPersistent( TrigPhotonContainer *transObj)
{
  MsgStream mlog(msgSvc(), "TrigPhotonContainerConverter" );

  mlog << MSG::DEBUG << "TrigPhotonContainerCnv::createPersistent called" << endmsg;

  TrigPhotonContainer_PERS * p_PhotonCont = m_converter.createPersistent( transObj, mlog );
 
  return p_PhotonCont;
 
}//end of create persistent method
 

//createTransient
TrigPhotonContainer * TrigPhotonContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigPhotonContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigPhotonContainerCnv::createTransient called" << endmsg;

  static const pool::Guid p3_guid( "40192614-E7C5-4BAF-825F-CB9F3E023315" );
  static const pool::Guid tlp2_guid( "96487DD7-9CF7-4351-BF33-011E6CA515F0" );
  static const pool::Guid tlp1_guid( "7F4B4EF4-D7F6-4208-B522-4003A34EC664" );
  static const pool::Guid p0_guid2( "65F1CCA1-B672-4E26-B74E-397CE6C8F617" );
  static const pool::Guid p0_guid( "CB04DF3E-C363-49E3-9BE1-AD25230AB1EA" );

  if( compareClassGuid( p3_guid ) ){
         std::unique_ptr< TrigPhotonContainer_p3 > col_vect( poolReadObject< TrigPhotonContainer_p3 >() );
         //         std::cout << "Reading IMFC p3" << std::endl;
         return m_converter.createTransient( col_vect.get(), mlog ) ;

  } else if( compareClassGuid( tlp2_guid ) )     
    {
      std::unique_ptr< TrigPhotonContainer_tlp2 > col_vect( poolReadObject< TrigPhotonContainer_tlp2 >() );
      return m_converter_tlp2.createTransient( col_vect.get(), mlog );
    }   
  else if( compareClassGuid( tlp1_guid ) )     
    {
      std::unique_ptr< TrigPhotonContainer_tlp1 > col_vect( poolReadObject< TrigPhotonContainer_tlp1 >() );
      return m_converter_tlp1.createTransient( col_vect.get(), mlog );
    }
  else if( compareClassGuid( p0_guid ) || compareClassGuid( p0_guid2 ) ){

      return poolReadObject< TrigPhotonContainer >();
    
    }
  else  throw std::runtime_error( "Unsupported persistent version of TrigPhotonContainer" );
     
}//end of create transient method
 
