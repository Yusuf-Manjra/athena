/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCSimHitCollectionCnv.h"

// Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEventTPCnv/CSCSimHitCollection_p1.h"
#include "MuonSimEventTPCnv/CSCSimHitCollection_p2.h"
#include "HitManagement/AthenaHitsVector.h" //for back-compatibility


CSCSimHitCollectionCnv::CSCSimHitCollectionCnv(ISvcLocator* svcloc) :
    CSCSimHitCollectionCnvBase(svcloc)
{
}

CSCSimHitCollectionCnv::~CSCSimHitCollectionCnv() {
}

CSCSimHitCollection_PERS*    CSCSimHitCollectionCnv::createPersistent (CSCSimHitCollection* transCont) {
    MsgStream log(messageService(), "CSCSimHitCollectionCnv" );
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createPersistent(): main converter"<<endreq;
    CSCSimHitCollection_PERS *pixdc_p= m_TPConverter_p2.createPersistent( transCont, log );
    return pixdc_p;
}

CSCSimHitCollection* CSCSimHitCollectionCnv::createTransient() {
    MsgStream log(messageService(), "CSCSimHitCollectionCnv" );
    static pool::Guid   p0_guid("DC744F9B-078A-4C61-B03F-D035D885B559"); // before t/p split
    static pool::Guid   p1_guid("E45CD293-4599-41BE-8B65-945A18F41AB7"); 
    static pool::Guid   p2_guid("7E5A120C-E64F-4519-A24B-B485A58724BF"); 
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): main converter"<<endreq;
    CSCSimHitCollection* p_collection(0);
    if( compareClassGuid(p2_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 2 detected"<<endreq;
        // poolReadObject< CSCSimHitCollection_PERS >( m_TPConverter );
        // p_collection = m_TPConverter.createTransient( log );
        std::auto_ptr< CSCSimHitCollection_PERS >   col_vect( this->poolReadObject< CSCSimHitCollection_PERS >() );
        p_collection = m_TPConverter_p2.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p1_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): T/P version 1 detected"<<endreq;
        // poolReadObject< CSCSimHitCollection_PERS >( m_TPConverter );
        // p_collection = m_TPConverter.createTransient( log );
        std::auto_ptr< Muon::CSCSimHitCollection_p1 >   col_vect( this->poolReadObject< Muon::CSCSimHitCollection_p1 >() );
        p_collection = m_TPConverter.createTransient( col_vect.get(), log );
    }
  //----------------------------------------------------------------
    else if( compareClassGuid(p0_guid) ) {
        if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<"createTransient(): Old input file"<<std::endl;
        AthenaHitsVector<CSCSimHit>* oldColl = this->poolReadObject< AthenaHitsVector<CSCSimHit> >();
        size_t size = oldColl->size();
        p_collection=new CSCSimHitCollection("DefaultCollectionName",size);
        //p_collection->reserve(size);
	//do the copy
        AthenaHitsVector<CSCSimHit>::const_iterator it = oldColl->begin(), itEnd=oldColl->end();
        for (;it!=itEnd;it++)  {
            p_collection->push_back(**it); 
        }
        delete oldColl;
   }
    else {
        throw std::runtime_error("Unsupported persistent version of CSCSimHitCollection");
    }
    return p_collection;
}
