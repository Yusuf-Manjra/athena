/**
 **   @file   MuonRegSelCondAlg.cxx         
 **            
 **           conditions algorithm to create the Si detector 
 **           lookup tables    
 **            
 **   @author sutt
 **   @date   Sun 22 Sep 2019 10:21:50 BST
 **
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/


#include "GaudiKernel/EventIDRange.h"
#include "StoreGate/WriteCondHandle.h"

#include <iostream>
#include <fstream>
#include <string>


#include "MuonCablingData/MuonMDT_CablingMap.h"

#include "MDT_RegSelCondAlg.h"



MuonRegSelCondAlg::MuonRegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm( name, pSvcLocator ),
  m_managerName(""),
  m_printTable(false)
{ 
  ATH_MSG_DEBUG( "MuonRegSelCondAlg::MuonRegSelCondAlg() " << name );
  declareProperty( "PrintTable",  m_printTable=false );  
  declareProperty( "ManagerName", m_managerName );  
}




StatusCode MuonRegSelCondAlg::initialize()
{
  ATH_MSG_DEBUG("MuonRegSelCondAlg::initialize() ");
  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_tableKey.initialize());
  ATH_MSG_INFO("MuonRegSelCondAlg::initialize() " << m_tableKey );
  return StatusCode::SUCCESS;
}



StatusCode MuonRegSelCondAlg::execute(const EventContext& ctx)  const
{
  ATH_MSG_DEBUG("MuonRegSelCondAlg::execute() -- enter -- ");
  
  /// do stuff here ...  
  ATH_MSG_DEBUG( "Creating region selector table " << m_tableKey );
 

  SG::ReadCondHandle<MuonMDT_CablingMap> cabling( m_cablingKey, ctx );

  EventIDRange id_range;
  
  if( !cabling.range( id_range ) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << cabling.key());
    return StatusCode::FAILURE;
  }   

  /// create the new lookuo table

  std::unique_ptr<RegSelSiLUT> rd = createTable( *cabling );

  if ( !rd ) return StatusCode::FAILURE;

  ATH_MSG_DEBUG( "Initialising new map " );;

  // write out new new LUT to a file if need be
  
  if ( m_printTable ) rd->write( name()+".map" );

  /// create the conditions data for storage 

  RegSelLUTCondData* rcd = new RegSelLUTCondData( std::move(rd) );
  
  try { 
    SG::WriteCondHandle<RegSelLUTCondData> lutCondData( m_tableKey, ctx );
    if( lutCondData.record( id_range, rcd ).isFailure() ) {
      ATH_MSG_ERROR( "Could not record " << m_tableKey 
		     << " " << lutCondData.key()
		     << " with range " << id_range );  
      return StatusCode::FAILURE;   
    } 
    ATH_MSG_INFO( "RegSelCondAlg LUT recorded: " << m_tableKey);
  }
  catch (...) { 
    ATH_MSG_ERROR("MuonRegSelCondAlg::execute() failed to record table: " << m_tableKey);
    return StatusCode::FAILURE;   
  }   
  
  
  ATH_MSG_DEBUG("MuonRegSelCondAlg::execute() -- exit -- ");

  return StatusCode::SUCCESS;
}




