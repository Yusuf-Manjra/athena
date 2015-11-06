/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AlgTestHistoDefSvc.cxx
 * Implementation file for  HistogramDefinitionSvc service test algorithm
 * @author Shaun Roe
 * @date 5 September, 2015
 **/
 
//STL
//to access environment


//Gaudi
#include "GaudiKernel/StatusCode.h"
#include "InDetPhysValMonitoring/HistogramDefinitionSvc.h"
//Athena
//Package
#include "InDetPhysValMonitoring/AlgTestHistoDefSvc.h"



using namespace std;
 
AlgTestHistoDefSvc::AlgTestHistoDefSvc (const std::string& name, ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator),
  m_histoDefSvc("HistogramDefinitionSvc",name)
{  //nop
}

AlgTestHistoDefSvc::~AlgTestHistoDefSvc(){
  //nop
}

StatusCode
AlgTestHistoDefSvc::initialize(){
  ATH_CHECK(m_histoDefSvc.retrieve());
  ATH_MSG_INFO("Test algorithm for HistogramDefinitionSvc");
  ATH_MSG_INFO("Printing histogram definition for 002_Al_N_01");
  
  std::string definition = m_histoDefSvc->definition("test","default").str();
  ATH_MSG_INFO(definition);
  ATH_MSG_INFO("Printing histogram definition for testProfile");

  definition = m_histoDefSvc->definition("testProfile","default").str();
  ATH_MSG_INFO(definition);

  return StatusCode::SUCCESS;
}


StatusCode
AlgTestHistoDefSvc::execute(){
  
  return StatusCode::SUCCESS;
}


StatusCode
AlgTestHistoDefSvc::finalize(){
  msg(MSG::INFO)<<"finalize()"<<endreq;
  return StatusCode::SUCCESS;
}

