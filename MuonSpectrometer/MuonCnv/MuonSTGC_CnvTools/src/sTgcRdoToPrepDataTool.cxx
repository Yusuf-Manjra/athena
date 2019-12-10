/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// sTgcRdoToPrepDataTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "sTgcRdoToPrepDataTool.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"


Muon::sTgcRdoToPrepDataTool::sTgcRdoToPrepDataTool(const std::string& t,
						   const std::string& n,
						   const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  sTgcRdoToPrepDataToolCore(t,n,p)
{
}

Muon::sTgcRdoToPrepDataTool::~sTgcRdoToPrepDataTool()
{
}

StatusCode Muon::sTgcRdoToPrepDataTool::initialize()
{  
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( sTgcRdoToPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::sTgcRdoToPrepDataTool::finalize()
{
  return sTgcRdoToPrepDataToolCore::finalize();
}

Muon::sTgcRdoToPrepDataToolCore::SetupSTGC_PrepDataContainerStatus Muon::sTgcRdoToPrepDataTool::setupSTGC_PrepDataContainer() 
{

  if(!evtStore()->contains<Muon::sTgcPrepDataContainer>(m_stgcPrepDataContainerKey.key())){    
    m_fullEventDone=false;
    
    SG::WriteHandle< Muon::sTgcPrepDataContainer > handle(m_stgcPrepDataContainerKey);
    StatusCode status = handle.record(std::make_unique<Muon::sTgcPrepDataContainer>(m_muonIdHelperTool->stgcIdHelper().module_hash_max()));
    
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of STGC PrepData Container at " << m_stgcPrepDataContainerKey.key()); 
      return FAILED;
    }
    m_stgcPrepDataContainer = handle.ptr();
    return ADDED;
    
  }
  return ALREADYCONTAINED;
}
