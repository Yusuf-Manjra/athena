/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgM.h"
#include "CxxUtils/make_unique.h"
#include <thread>
#include <chrono>

HiveAlgM::HiveAlgM( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator ),
  m_rdh1("a2"),
  m_rdh2("l1")
{
  
  declareProperty("Key_R1",m_rdh1);
  declareProperty("Key_R2",m_rdh2);
  declareProperty("Offset",m_off);

}

HiveAlgM::~HiveAlgM() {}

StatusCode HiveAlgM::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rdh1.initialize() );
  ATH_CHECK( m_rdh2.initialize() );

  return HiveAlgBase::initialize ();
}

StatusCode HiveAlgM::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgM::execute() {

  ATH_MSG_DEBUG("execute " << name());
 
  sleep();

  SG::ReadHandle<HiveDataObj> rdh1( m_rdh1 );
  if (!rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh1.key());
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<HiveDataObj> rdh2( m_rdh2 );
  if (!rdh2.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh2.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << rdh1.key() << " = " << rdh1->val() );
  ATH_MSG_INFO("  read: " << rdh2.key() << " = " << rdh2->val() );

  if ( rdh2->val() != (rdh1->val() + m_off) ) {
    ATH_MSG_ERROR (rdh2.key() << " != " << rdh1.key() << " + " << m_off);
  } else {
    ATH_MSG_INFO( "loop is ok");
  }

  return StatusCode::SUCCESS;

}

