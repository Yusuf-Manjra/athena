/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetTrackTruthMapCnv.h"
#include "RootConversions/TConverterRegistry.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_old_cnv.h"
#include "TROOT.h"
#include <iostream>

//constructor
TrigInDetTrackTruthMapCnv::TrigInDetTrackTruthMapCnv( ISvcLocator* svcloc ) :
  TrigInDetTrackTruthMapCnvBase( svcloc ) {
}

//destructor
TrigInDetTrackTruthMapCnv::~TrigInDetTrackTruthMapCnv() {}


//create persistent
TrigInDetTrackTruthMap_PERS* TrigInDetTrackTruthMapCnv::createPersistent(TrigInDetTrackTruthMap* transObj) {
    ATH_MSG_DEBUG( "TrigInDetTrackTruthMapCnv::createPersistent"  );

    TrigInDetTrackTruthMap_PERS* persObj = m_trigInDetTrackTruthMapCnv_tlp2.createPersistent( transObj, msg() );

    return persObj;
}

//create transient
TrigInDetTrackTruthMap* TrigInDetTrackTruthMapCnv::createTransient() {
  
  ATH_MSG_DEBUG( "TrigInDetTrackTruthMapCnv::createTransient "  );
  
  const static pool::Guid tlp1_guid("08892FEB-5706-4938-9226-F45C0AA662E7");
  const static pool::Guid tlp2_guid("02074F47-F290-4A48-B503-4DCAB4181B3D");
  const static pool::Guid tlp3_guid("81BFA8A7-89A8-4072-B6E2-7124300CA9EB");
  const static pool::Guid p0_guid("41581666-F06D-44AE-93B9-D7E912A27AA1");
  
  TrigInDetTrackTruthMap  *transObj = 0;
  
  
  if( compareClassGuid(tlp3_guid) ) {

    ATH_MSG_DEBUG( "TrigInDetTrackTruthMapCnv::reading tlp3 persistent object"  );
    poolReadObject< TrigInDetTrackTruthMap_tlp3 >( m_trigInDetTrackTruthMapCnv_tlp3 );
    transObj = m_trigInDetTrackTruthMapCnv_tlp3.createTransient( msg() );

  }
  else if( compareClassGuid(tlp2_guid) ) {
    
    ATH_MSG_DEBUG( "TrigInDetTrackTruthMapCnv::reading tlp2 persistent object"  );
    poolReadObject< TrigInDetTrackTruthMap_tlp2 >( m_trigInDetTrackTruthMapCnv_tlp2 );
    transObj = m_trigInDetTrackTruthMapCnv_tlp2.createTransient( msg() );
    
  }
  else if( compareClassGuid(tlp1_guid) ) {    
    ATH_MSG_DEBUG( "TrigInDetTrackTruthMapCnv::reading tlp1 persistent object"  );
    TrigInDetTrackTruthMapCnv_tlp1  tlp1_Converter;
    poolReadObject< TrigInDetTrackTruthMap_tlp1 >(tlp1_Converter);
    transObj = tlp1_Converter.createTransient( msg() );
  }
  else if( compareClassGuid(p0_guid) ) {
    
    ATH_MSG_DEBUG( "TrigInDetTrackTruthMapCnv::reading p0 persistent object"  );
    // old version from before TP separation, just return it
    transObj = this->poolReadObject<TrigInDetTrackTruthMap>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version of TrigInDetTrackTruthMap");
  }
  
  return transObj;
}


namespace {
  // Helper to be able to call thread-unsafe code during initialize as we
  // currently cannot mark Converter::initialize() as thread-unsafe.
  StatusCode loadConverter ATLAS_NOT_THREAD_SAFE() {
    gROOT->GetClass("TrigInDetTrackTruthMap_old");
    static TrigInDetTrackTruthMap_old_cnv cnv;
    TConverterRegistry::Instance()->AddConverter (&cnv);
    return StatusCode::SUCCESS;
  }
}

StatusCode TrigInDetTrackTruthMapCnv::initialize()
{
  [[maybe_unused]] static const bool did_rootcnv = []{
    StatusCode sc ATLAS_THREAD_SAFE = loadConverter();
    return sc.isSuccess();
  }();
  return TrigInDetTrackTruthMapCnvBase::initialize();
}

