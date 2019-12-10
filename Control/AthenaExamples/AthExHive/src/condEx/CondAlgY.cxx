/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CondAlgY.h"
#include "AthExHive/IASCIICondDbSvc.h"
#include "StoreGate/WriteCondHandle.h"

#include "GaudiKernel/ServiceHandle.h"

#include <thread>
#include <chrono>
#include <memory>

CondAlgY::CondAlgY( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_cds("ASCIICondDbSvc",name)
{
}

CondAlgY::~CondAlgY() {}

StatusCode CondAlgY::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  if (m_cds.retrieve().isFailure()) {
    ATH_MSG_ERROR("unable to retrieve ASCIICondDbSvc");
  }

  m_wch1.setDbKey(m_dbk1);
  m_wch2.setDbKey(m_dbk2);

  if (m_wch1.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize WriteHandleKey with key" << m_wch1.key() );
    return StatusCode::FAILURE;
  }

  if (m_wch2.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize WriteCondHandle with key" << m_wch2.key() );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CondAlgY::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode CondAlgY::execute() {
  ATH_MSG_DEBUG("execute " << name());

  EventIDBase now(getContext().eventID());

  SG::WriteCondHandle<CondDataObjY> wch1(m_wch1);
  SG::WriteCondHandle<CondDataObjY> wch2(m_wch2);

  // do we have a valid m_wch for current time?
  if ( !wch1.isValid(now) ) {

    ATH_MSG_DEBUG("  CondHandle " << wch1.key() 
                  << " not valid. Getting new info for dbKey \"" 
                  << wch1.dbKey() << "\" from CondDb");

    EventIDRange r;
    IASCIICondDbSvc::dbData_t val;
    if (m_cds->getRange(wch1.dbKey(), getContext(), r, val).isFailure()) {
      ATH_MSG_ERROR("  could not find dbKey \"" << wch1.dbKey() 
		    << "\" in CondSvc registry");
      return StatusCode::FAILURE;
    }

    CondDataObjY* cdo = new CondDataObjY( val );
    if (wch1.record(r, cdo).isFailure()) {
      ATH_MSG_ERROR("could not record CondDataObjY " << wch1.key() 
		    << " = " << *cdo
                    << " with EventRange " << r);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new CDO " << wch1.key() << " = " << *cdo 
		 << " with range " << r);
  }

  // do we have a valid wch for current time?
  if ( !wch2.isValid(now) ) {

    ATH_MSG_DEBUG("  CondHandle " << wch2.key() 
                  << " not valid. Getting new info for dbKey \"" 
                  << wch2.dbKey() << "\" from CondDb");

    EventIDRange r;
    IASCIICondDbSvc::dbData_t val;
    if (m_cds->getRange(wch2.dbKey(), getContext(), r, val).isFailure()) {
      ATH_MSG_ERROR("  could not find dbKey \"" << wch2.dbKey() 
		    << "\" in CondSvc registry");
      return StatusCode::FAILURE;
    }

    CondDataObjY* cdo = new CondDataObjY( val );
    if (wch2.record(r, cdo).isFailure()) {
      ATH_MSG_ERROR("could not record CondDataObjY " << wch2.key() 
		    << " = " << *cdo
                    << " with EventRange " << r);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new CDO " << wch2.key() << " = " << *cdo 
		 << " with range " << r);
  }

  return StatusCode::SUCCESS;

}

