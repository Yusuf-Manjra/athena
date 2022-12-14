/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "EventShapeStoreCnv.h"
#include "EventShapeTPCnv/EventShapeStoreCnv_p1.h"
#include "EventShapeTPCnv/EventShapeStore_p1.h"

EventShapeStore_PERS* EventShapeStoreCnv::createPersistent(EventShapeStore* transObj)
{
  MsgStream report( msgSvc(), "EventShapeStoreConverter" );
  report << MSG::INFO << "Writing EventShapeStore_p1" << endmsg; 
  EventShapeStoreCnv_p1   TPconverter;
  EventShapeStore_PERS *persObj = TPconverter.createPersistent( transObj, report );
  
  return persObj;
}

EventShapeStore* EventShapeStoreCnv::createTransient()
{
  static const pool::Guid   p1_guid("730C49D9-47C2-45D4-A755-1313F4A2ACB1");
  if( compareClassGuid(p1_guid) )
    {
      EventShapeStoreCnv_p1   TPconverter;
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr<EventShapeStore_p1> shapestore( poolReadObject< EventShapeStore_p1 >() );
      MsgStream report( msgSvc(), "EventShapeStoreCnv" );
      report << MSG::INFO << "Reading EventShapeStore_p1" << endmsg; 
      return TPconverter.createTransient( shapestore.get(), report );
    }
  throw std::runtime_error("Unsupported persistent version of EventShapeStore");
}
