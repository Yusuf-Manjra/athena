/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AlgC.h"

#include "CxxUtils/make_unique.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/ServiceHandle.h"
#include <thread>
#include <chrono>


AlgC::AlgC( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_rdh1("a2"),
  m_rch("X1")
{
  
  declareProperty("Key_R1",m_rdh1);
  declareProperty("Key_CH", m_rch);

}

//---------------------------------------------------------------------------

AlgC::~AlgC() {}

//---------------------------------------------------------------------------

StatusCode AlgC::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK(m_rdh1.initialize());
  ATH_CHECK(m_rch.initialize());

  ATH_MSG_INFO( "m_rdh1 id: " << m_rdh1.fullKey() );
  ATH_MSG_INFO( "m_rch id:  " << m_rch.fullKey() );


  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode AlgC::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode AlgC::execute() {
  ATH_MSG_DEBUG("execute " << name());

  SG::ReadHandle<HiveDataObj> rh1(m_rdh1);
  if (!rh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh1.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << rh1.key() << " = " << rh1->val() );

  EventIDBase t( getContext()->eventID() );
  
  //  const CondDataObj *cdo = m_rch.retrieve(t);    
  //  const CondDataObj *cdo = m_rch.retrieve();

  SG::ReadCondHandle<CondDataObj> rch( m_rch );
  const CondDataObj *cdo = *rch;
  if (cdo != 0) {
    ATH_MSG_INFO("  read CH: " << rch.key() << " = " << *cdo );
  } else {
    ATH_MSG_ERROR("  CDO ptr for " << rch.key() << " == zero");
  }
  
  return StatusCode::SUCCESS;

}

