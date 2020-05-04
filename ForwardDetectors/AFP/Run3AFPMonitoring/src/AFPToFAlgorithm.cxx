/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*
*
*	AFPToFAlgorithm
*
*
*/

#include "Run3AFPMonitoring/AFPToFAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODForward/AFPStationID.h"


AFPToFAlgorithm::AFPToFAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
, m_afpToFHitContainerKey("AFPToFHitContainer")

{
    declareProperty( "AFPToFHitContainer", m_afpToFHitContainerKey );
}


AFPToFAlgorithm::~AFPToFAlgorithm() {}


StatusCode AFPToFAlgorithm::initialize() {
    using namespace Monitored;
 
    // We must declare to the framework in initialize what SG objects we are going to use
    SG::ReadHandleKey<xAOD::AFPToFHitContainer> afpToFHitContainerKey("AFPToFHits");
    ATH_CHECK(m_afpToFHitContainerKey.initialize());
    
    return AthMonitorAlgorithm::initialize();
}


StatusCode AFPToFAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    auto lb = Monitored::Scalar<int>("lb", 0);
    auto nTofHits = Monitored::Scalar<int>("nTofHits", 1);
    
    lb = GetEventInfo(ctx)->lumiBlock();

 
    SG::ReadHandle<xAOD::AFPToFHitContainer> afpToFHitContainer(m_afpToFHitContainerKey, ctx);
    if(! afpToFHitContainer.isValid())
    {
	ATH_MSG_ERROR("evtStore() does not contain hits collection with name " << m_afpToFHitContainerKey);
	return StatusCode::FAILURE;
    }

    ATH_CHECK( afpToFHitContainer.initialize() );

    nTofHits = afpToFHitContainer->size();
    fill("AFPToFTool", lb, nTofHits);

/*	TO BE researched: difference between trainID and barInTrainID
    auto numberOfHit_S0 = Monitored::Scalar<int>("numberOfHit_S0", 0); 
    auto numberOfHit_S3 = Monitored::Scalar<int>("numberOfHit_S3", 0); 

    for(const xAOD::AFPTofHit *hitsIts: *afpToFHitContainer)
    {
	if(hitsIts->isSideA())
	{
	    fill("AFPToFTool", numberOfHit_S0);
	}
	else if(hitsIts->isSideC())
	{
	    fill("AFPToFTool", numberOfHit_S3);
	}
    }
*/


    return StatusCode::SUCCESS;
}

