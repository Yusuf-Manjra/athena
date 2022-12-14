/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************

  TRTOccupancyInclude.cxx : Simple code to include Occupancy inside event info xAOD 

* ***************************************************************** */

#include "TRT_CalibAlgs/TRTOccupancyInclude.h"
#include "xAODEventInfo/EventInfo.h"

#include "StoreGate/ReadHandle.h"
#include "CxxUtils/checker_macros.h"

TRTOccupancyInclude::TRTOccupancyInclude(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm   (name, pSvcLocator),
  m_LocalOccTool()
{
  declareProperty("TRT_LocalOccupancyTool", m_LocalOccTool);
}

//---------------------------------------------------------------------

TRTOccupancyInclude::~TRTOccupancyInclude(void)
{}

//--------------------------------------------------------------------------

StatusCode TRTOccupancyInclude::initialize()
{
  if ( m_LocalOccTool.retrieve().isFailure() ){
    ATH_MSG_ERROR(" Failed to retrieve TRT Local Occupancy tool: " << m_LocalOccTool );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_INFO("Retrieved tool " << m_LocalOccTool);
  }

  ATH_CHECK(m_eventInfoKey.initialize());

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------

StatusCode TRTOccupancyInclude::execute()
{
  msg(MSG::DEBUG) << "execute()" << endmsg;

  const EventContext& ctx = Gaudi::Hive::currentContext();

  SG::ReadHandle<xAOD::EventInfo> evtInfo(m_eventInfoKey, ctx);
  if (not evtInfo.isValid()) {
    ATH_MSG_WARNING(" Cannot access to event info.");
    return StatusCode::SUCCESS;
  } 
  
  const xAOD::EventInfo* eventInfo = evtInfo.cptr();

  std::vector<float> TRTOccu = m_LocalOccTool->GlobalOccupancy(ctx);
  if (TRTOccu.size() > 6) {

  static const SG::AuxElement::Decorator< float >  decEventInfo_occupancy0("TRTOccGlobal"); 
  decEventInfo_occupancy0( *eventInfo ) = TRTOccu.at(0); 

  static const SG::AuxElement::Decorator< float >  decEventInfo_occupancy1("TRTOccBarrelC"); 
  decEventInfo_occupancy1( *eventInfo ) = TRTOccu.at(1); 

  static const SG::AuxElement::Decorator< float >  decEventInfo_occupancy2("TRTOccEndcapAC"); 
  decEventInfo_occupancy2( *eventInfo ) = TRTOccu.at(2); 

  static const SG::AuxElement::Decorator< float >  decEventInfo_occupancy3("TRTOccEndcapBC"); 
  decEventInfo_occupancy3( *eventInfo ) = TRTOccu.at(3); 

  static const SG::AuxElement::Decorator< float >  decEventInfo_occupancy4("TRTOccBarrelA"); 
  decEventInfo_occupancy4( *eventInfo ) = TRTOccu.at(4); 

  static const SG::AuxElement::Decorator< float >  decEventInfo_occupancy5("TRTOccEndcapAA"); 
  decEventInfo_occupancy5( *eventInfo ) = TRTOccu.at(5); 

  static const SG::AuxElement::Decorator< float >  decEventInfo_occupancy6("TRTOccEndcapBA"); 
  decEventInfo_occupancy6( *eventInfo ) = TRTOccu.at(6); 
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------

StatusCode TRTOccupancyInclude::finalize()
{
  msg(MSG::INFO) << "finalise()" << endmsg;
  return StatusCode::SUCCESS;
}
