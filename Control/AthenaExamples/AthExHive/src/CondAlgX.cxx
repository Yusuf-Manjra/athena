/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CondAlgX.h"
#include "CxxUtils/make_unique.h"

#include "StoreGate/WriteCondHandle.h"

#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <thread>
#include <chrono>

CondAlgX::CondAlgX( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_evt("McEventInfo"),
  m_wchk("X2","X2"),
  m_cs("CondSvc",name)
{
  
  declareProperty("EvtInfo", m_evt);
  declareProperty("Key_CH", m_wchk);
  declareProperty("Key_DB", m_dbKey);

}

CondAlgX::~CondAlgX() {}

StatusCode CondAlgX::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  if (m_cs.retrieve().isFailure()) {
    ATH_MSG_ERROR("unable to retrieve CondSvc");
  }

  m_wchk.setDbKey(m_dbKey);

  if (m_wchk.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize WriteCondHandle with key" << m_wchk.key() );
    return StatusCode::FAILURE;
  }

  if (m_cs->regHandle(this, m_wchk, m_wchk.dbKey()).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_wchk.fullKey() 
                  << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CondAlgX::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode CondAlgX::execute() {
  ATH_MSG_DEBUG("execute " << name());

  if (!m_evt.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve EventInfo");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("   EventInfo:  r: " << m_evt->event_ID()->run_number()
                << " e: " << m_evt->event_ID()->event_number() );


  SG::WriteCondHandle<CondDataObj> wch(m_wchk);

  EventIDBase now(getContext()->eventID());

  if (m_evt->event_ID()->event_number() == 10) {
      std::this_thread::sleep_for(std::chrono::milliseconds( 500 ));
  }

  // do we have a valid m_wch for current time?
  if ( wch.isValid(now) ) {
    // in theory this should never be called
    wch.updateStore();
    ATH_MSG_DEBUG("CondHandle is already valid for " << now
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order."
		  << " Forcing update of Store contents");

  } else {

    ATH_MSG_DEBUG("  CondHandle " << wch.key() 
                  << " not valid. Getting new info for dbKey \"" 
                  << wch.dbKey() << "\" from CondDb");

    EventIDRange r;
    ICondSvc::dbData_t val;
    if (m_cs->getRange(wch.dbKey(), getContext(), r, val).isFailure()) {
      ATH_MSG_ERROR("  could not find dbKey \"" << wch.dbKey() 
		    << "\" in CondSvc registry");
      return StatusCode::FAILURE;
    }

    CondDataObj* cdo = new CondDataObj( val );
    if (wch.record(r, cdo).isFailure()) {
      ATH_MSG_ERROR("could not record CondDataObj " << wch.key() 
		    << " = " << *cdo
                    << " with EventRange " << r);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new CDO " << wch.key() << " = " << *cdo 
		 << " with range " << r);
  }



  return StatusCode::SUCCESS;

}

