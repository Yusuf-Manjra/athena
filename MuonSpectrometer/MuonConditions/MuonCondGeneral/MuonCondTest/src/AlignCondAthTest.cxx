/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
 
/////////////////////////////////////////////////////////
#include "MuonCondTest/AlignCondAthTest.h"
#include "MuonCondSvc/MuonAlignmentDbSvc.h"


AlignCondAthTest::AlignCondAthTest(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator){
   p_MuonAlignmentDbSvc              = 0; 

}
 
StatusCode AlignCondAthTest::initialize(){
//

   MsgStream log(messageService(), name());
   log << MSG::INFO << "in initialize()" << endreq;
   StatusCode sc ;
//

   sc = service("MuonCalib::MuonAlignmentDbSvc",p_MuonAlignmentDbSvc );
   if (!sc.isSuccess() || 0 == p_MuonAlignmentDbSvc) {
     log << MSG::ERROR
         << "::initialize "
         << "Could not find MuonAlignmentDbSvc" << endreq;
    return( StatusCode::FAILURE );
   }
//

  return StatusCode::SUCCESS;
//
}
 
StatusCode AlignCondAthTest::execute() {
//

   MsgStream log(messageService(), name());
   log << MSG::INFO << " AlignCondAthTest in execute()" << endreq;

  return StatusCode::SUCCESS;
//
}
 
StatusCode AlignCondAthTest::finalize() {
//

   MsgStream log(messageService(), name());
   log << MSG::INFO << "in finalize()" << endreq;
//
  return StatusCode::SUCCESS;
//
}
  
