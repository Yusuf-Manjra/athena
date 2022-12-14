/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTrackInfoCnv.h"

TBTrackInfo_PERS* TBTrackInfoCnv::createPersistent(TBTrackInfo* transCont) {
    MsgStream log(msgSvc(), "TBTrackInfoConverter" );
    TBTrackInfo_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
    log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}
    
TBTrackInfo* TBTrackInfoCnv::createTransient() {
   MsgStream log(msgSvc(), "TBTrackInfoConverter" );
   static const pool::Guid   p1_guid("A49F674F-25AD-496B-BEBE-DE1EBEAFE990");  // GUID of the persistent object
   static const pool::Guid   p0_guid("5780AB02-D5D1-4537-B682-56CA95927BFB");  // GUID of the transient object
   if( compareClassGuid(p1_guid) ) {
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr< TBTrackInfo_p1 > col_vect( poolReadObject< TBTrackInfo_p1 >() );
      return m_TPConverter.createTransient( col_vect.get(), log );
   }
   else if( compareClassGuid(p0_guid) ) {
      // regular object from before TP separation, just return it
      return poolReadObject< TBTrackInfo >();
   } 
   throw std::runtime_error("Unsupported persistent version of Data Collection");
}

