/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgC.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/ServiceHandle.h"
#include <thread>
#include <chrono>

HiveAlgC::HiveAlgC( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator ),
  m_rdh1("a1"),
  m_wrh1("c1"),
  m_wrh2("c2")
{
  
  declareProperty("Key_R1",m_rdh1);
  declareProperty("Key_W1",m_wrh1);
  declareProperty("Key_W2",m_wrh2);

}

HiveAlgC::~HiveAlgC() {}

StatusCode HiveAlgC::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rdh1.initialize() );
  ATH_CHECK( m_wrh1.initialize() );
  ATH_CHECK( m_wrh2.initialize() );

  return HiveAlgBase::initialize();
}

StatusCode HiveAlgC::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgC::execute() {

  ATH_MSG_DEBUG("execute " << name());

  sleep();

  SG::ReadHandle<HiveDataObj> rdh1( m_rdh1 );
  if (!rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh1.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << rdh1.key() << " = " << rdh1->val() );
  
  SG::WriteHandle<HiveDataObj> wrh1( m_wrh1 );
  wrh1 = CxxUtils::make_unique< HiveDataObj >( HiveDataObj(30000) );

  SG::WriteHandle<HiveDataObj> wrh2( m_wrh2 );
  wrh2 = CxxUtils::make_unique< HiveDataObj >( HiveDataObj(30001) );
  
  ATH_MSG_INFO("  write: " << wrh1.key() << " = " << wrh1->val() );
  ATH_MSG_INFO("  write: " << wrh2.key() << " = " << wrh2->val() );

  return StatusCode::SUCCESS;

}

