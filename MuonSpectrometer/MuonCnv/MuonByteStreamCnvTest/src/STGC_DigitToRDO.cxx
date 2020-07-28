/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "STGC_DigitToRDO.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

STGC_DigitToRDO::STGC_DigitToRDO(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_idHelper(0)
{
}

StatusCode STGC_DigitToRDO::initialize()
{
  declareProperty("RDOContainerName", m_rdoContainer = "sTGCRDO");
  declareProperty("DigitContainerName", m_digitContainer = "sTGC_DIGITS");
  
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_rdoContainer.initialize() );
  ATH_CHECK( m_digitContainer.initialize() );  
  ATH_CHECK ( detStore()->retrieve(m_idHelper, "STGCIDHELPER") );
  
  return StatusCode::SUCCESS;
}

StatusCode STGC_DigitToRDO::execute()
{  
  using namespace Muon;
  ATH_MSG_DEBUG( "in execute()"  );
  SG::WriteHandle<STGC_RawDataContainer> rdos (m_rdoContainer);
  SG::ReadHandle<sTgcDigitContainer> digits (m_digitContainer);
  ATH_CHECK( rdos.record(std::unique_ptr<STGC_RawDataContainer>(new STGC_RawDataContainer(m_idHelper->module_hash_max())) ) );

  if (digits.isValid() ){
    for (const sTgcDigitCollection* digitColl : *digits ){

      // Transform the hash id ( digit collection use detector element ID hash, RDO's use 
      // module Id hash
      Identifier digitId = digitColl->identify();
      IdentifierHash hash;
      int getRdoCollHash = m_idHelper->get_module_hash(digitId,hash);
      if ( getRdoCollHash !=0 ) {
	ATH_MSG_ERROR("Could not get the module hash Id");
	return StatusCode::FAILURE;
      } 

      STGC_RawDataCollection* coll = new STGC_RawDataCollection(hash);
      if (rdos->addCollection(coll,hash).isFailure() ){
        ATH_MSG_WARNING("Failed to add collection with hash " << (int)hash );
        delete coll;
        continue;
      }
    
      for (const sTgcDigit* digit : *digitColl ){
        Identifier id = digit->identify();
	uint16_t bcTag = digit->bcTag();
	// keep the time as a float for now, but it should also become an int
	float time   = digit->time();
	//uint16_t charge = (uint16_t) digit->charge_10bit();
	uint16_t charge = (uint16_t) digit->charge(); // 10bit charge conversion to PDO is done else where as a quick fix for now; correct version incoming in the next few days
	bool isDead = digit->isDead();
        STGC_RawData* rdo = new STGC_RawData(id, bcTag, time, charge, isDead);
        coll->push_back(rdo);
      }
    }
  } else {
    ATH_MSG_WARNING("Unable to find STGC digits");
  }
    
  ATH_MSG_DEBUG( "done execute()"  );
  return StatusCode::SUCCESS;
}

StatusCode STGC_DigitToRDO::finalize() {
  return StatusCode::SUCCESS;
}
