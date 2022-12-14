/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArMphysOverMcalMCCnv.cxx
 * @brief AthenaPool converter LArMphysOverMcalMC
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * $Id: LArMphysOverMcalMCCnv.cxx,v 1.2 2008-04-10 14:54:35 wlampl Exp $
 */

#include "LArMphysOverMcalMCCnv.h"
#include "LArCondTPCnv/LArMphysOverMcalSubsetCnv_p1.h"
#include "LArSingleFloatUpdater.h"

static const LArMphysOverMcalSubsetCnv_p1 TPconverter;

LArConditionsSubset<LArSingleFloatP>*
LArMphysOverMcalMCCnv::createTransient ()
{
  static const pool::Guid   p0_guid("9C53AC43-3FD6-470F-A6FF-1DF80E85ACBF");
  static const pool::Guid   p1_guid("60FB956A-0B7F-450E-BF6A-2A0B8ED55204");
  // first try the single-float converter
  LArConditionsSubset<LArSingleFloatP>* p=createTransientSingleFloat();
  if (p) 
    return p;
  else if( compareClassGuid(p1_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr< LArMphysOverMcalSubset_p1 > col_vect( poolReadObject< LArMphysOverMcalSubset_p1 >() );
    MsgStream log(msgSvc(), "LArMphysOverMcalMCCnv" ); 
    //log << MSG::INFO << "Reading LArMphysOverMcalSubset_p1" << endmsg; 
    return TPconverter.createTransientConst( col_vect.get(), log );
  }
  else if( compareClassGuid(p0_guid) ) {
    // subset from before TP separation    
    MsgStream log(msgSvc(), "LArMphysOverMcalMCCnv" ); 
    log << MSG::DEBUG << "Reading LArMphysOverMcalSubset (original)" << endmsg; 
    
    std::unique_ptr< LArConditionsSubset<LArMphysOverMcalP> > subset ( poolReadObject< LArConditionsSubset<LArMphysOverMcalP> >() );
    // Here we must convert from LArMphysOverMcalP to LArMphysOverMcalP1
    return (createTransient(subset.get()));
    
  } 
  throw std::runtime_error("Unsupported persistent version of LArMphysOverMcalMCCnv");
}

LArConditionsSubset<LArSingleFloatP>* 
LArMphysOverMcalMCCnv::createTransient(LArConditionsSubset<LArMphysOverMcalP>* orig){
    MsgStream log(msgSvc(), "LArMphysOverMcalMCCnv" ); 
    log << MSG::DEBUG << "LArMphysOverMcalMCCnv::createTransient orig " << orig << endmsg; 

    LArConditionsSubset<LArSingleFloatP>* result = new LArConditionsSubset<LArSingleFloatP>();
    LArSingleFloatUpdater copier;
    copier.update(orig, result);

    return (result);
}
