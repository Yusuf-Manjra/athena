/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBHitPlaneContCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "AthenaKernel/StorableConversions.h"


// Constructor - call base constructor and initialize local attributes
TBHitPlaneContCnv::TBHitPlaneContCnv(ISvcLocator* svcloc)
  :
  // Base class constructor
  TBHitPlaneContCnvBase::T_AthenaPoolCnv(svcloc)
{}

// Destructor
TBHitPlaneContCnv::~TBHitPlaneContCnv()
{}

StatusCode TBHitPlaneContCnv::initialize()
{
  // Call base clase initialize
  AthenaPoolConverter::initialize();

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBHitPlaneContCnv");
  log << MSG::INFO << "initialize()" << endmsg;

  return StatusCode::SUCCESS;
}


StatusCode TBHitPlaneContCnv::PoolToDataObject(DataObject*& pObj, const Token* token)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(msgSvc(), "TBHitPlaneContCnv::PoolToDataObject" );
   
  StatusCode sc = TBHitPlaneContCnvBase::PoolToDataObject(pObj, token);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endmsg;
  }
    
  // Convert DataObject pointer to TBHitPlaneCont*
  TBHitPlaneCont* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBHitPlaneCont " << endmsg ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}

StatusCode TBHitPlaneContCnv::DataObjectToPool(DataObject* pObj, const std::string &tname) 
{
  MsgStream log(msgSvc(),"TBHitPlaneContCnv::DataObjectToPool" );
  
  return TBHitPlaneContCnvBase::DataObjectToPool( pObj, tname) ;
}
