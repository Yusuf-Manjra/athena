/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTSTREAMINFOCNV_P2_H
#define EVENTATHENAPOOL_EVENTSTREAMINFOCNV_P2_H

/** @file EventStreamInfoCnv_p2.h
 *  @brief This file contains the class definition for the EventStreamInfoCnv_p2 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfoCnv_p2.h,v 1.1 2009-03-18 17:42:44 gemmeren Exp $
 **/

#include "EventInfo/EventStreamInfo.h"
#include "EventTPCnv/EventStreamInfo_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

/** @class EventStreamInfoCnv_p2
 *  @brief This class provides the converter to customize the saving of EventStreamInfo_p2.
 **/
class EventStreamInfoCnv_p2 : public T_AthenaPoolTPCnvConstBase<EventStreamInfo, EventStreamInfo_p2>  {
public:
   EventStreamInfoCnv_p2() {}
   using base_class::persToTrans;
   using base_class::transToPers;

   virtual void persToTrans(const EventStreamInfo_p2* persObj, EventStreamInfo* transObj, MsgStream &log) const override;
   virtual void transToPers(const EventStreamInfo* transObj, EventStreamInfo_p2* persObj, MsgStream &log) const override;
};

template<>
class T_TPCnv<EventStreamInfo, EventStreamInfo_p2> : public EventStreamInfoCnv_p2 {
public:
};

#endif
