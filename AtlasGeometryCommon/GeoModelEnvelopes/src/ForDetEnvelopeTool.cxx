/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelEnvelopes/ForDetEnvelopeTool.h"
#include "GeoModelEnvelopes/ForDetEnvelopeFactory.h" 
#include "GeoModelEnvelopes/ForDetEnvelopeManager.h" 
#include "GeoModelUtilities/GeoModelExperiment.h"

#include "StoreGate/StoreGateSvc.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

ForDetEnvelopeTool::ForDetEnvelopeTool(const std::string& type, 
				       const std::string& name, 
				       const IInterface* parent): 
  GeoModelTool(type,name,parent),
  m_manager(0)
{
}

ForDetEnvelopeTool::~ForDetEnvelopeTool()
{
}


StatusCode ForDetEnvelopeTool::create(StoreGateSvc* detStore)
{ 
  ATH_MSG_INFO("Building Forward Detectors Envelope");

  GeoModelExperiment* theExpt; 
  if (StatusCode::SUCCESS != detStore->retrieve(theExpt,"ATLAS")) { 
    ATH_MSG_ERROR("Could not find GeoModelExperiment ATLAS");
    return StatusCode::FAILURE; 
  } 

  if(0==m_manager) {
    GeoPhysVol *world=&*theExpt->getPhysVol();

    ForDetEnvelopeFactory theFactory(detStore);
    theFactory.create(world);

    m_manager = theFactory.getDetectorManager();
    theExpt->addManager(m_manager);
    StatusCode sc = detStore->record(m_manager,
			  m_manager->getName());
    
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not register ForDetEnvelope detector manager");
      return StatusCode::FAILURE; 
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode ForDetEnvelopeTool::clear(StoreGateSvc* detStore)
{
  SG::DataProxy* _proxy = detStore->proxy(ClassID_traits<ForDetEnvelopeManager>::ID(),m_manager->getName());
  if(_proxy) {
    _proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}
