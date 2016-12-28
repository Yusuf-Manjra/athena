/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloUtils/CaloClusterStoreHelper.h"

#include "CaloClusterSnapshot.h"
#include "AthenaKernel/errorcheck.h"
#include "CaloClusterMaker.h"

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
CaloClusterSnapshot::CaloClusterSnapshot(const std::string& type, 
				   const std::string& name,
				   const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_outputKey(""),
  m_finalContName(nullptr)
{ 
  declareInterface<CaloClusterCollectionProcessor> (this);
  declareProperty("OutputName",m_outputKey);
  declareProperty("SetCrossLinks",m_setCrossLinks=false);
  
}

// -------------------------------------------------------------
// Destructor 
// -------------------------------------------------------------
CaloClusterSnapshot::~CaloClusterSnapshot()
{ }

StatusCode CaloClusterSnapshot::initialize() {

  if (m_setCrossLinks) {
    const CaloClusterMaker* parentAlgo=dynamic_cast<const CaloClusterMaker*>(parent());
    if (!parentAlgo) {
      ATH_MSG_WARNING( "Configuration problem. Parent is not CaloClusterMaker. Can't set ElementLink to final cluster."  );
    }
    m_finalContName=&(parentAlgo->getOutputContainerName());
  }
  CHECK(m_outputKey.initialize());
  return StatusCode::SUCCESS;

}


StatusCode
CaloClusterSnapshot::execute(const EventContext& ctx,
                             xAOD::CaloClusterContainer* clusColl) const
{
  ATH_MSG_DEBUG("Executing CaloClusterSnapshot");
  
  SG::WriteHandle<xAOD::CaloClusterContainer>  outputColl(m_outputKey, ctx);
  
  CHECK(CaloClusterStoreHelper::AddContainerWriteHandle(&(*evtStore()),outputColl,msg()));
  
  CaloClusterStoreHelper::copyContainer(clusColl,outputColl.ptr());

  if (m_setCrossLinks) {
    const size_t nClusters=clusColl->size();
    if (nClusters!=outputColl->size()) {
      ATH_MSG_ERROR("After copy of container: Unequal number of entries! Original " << nClusters << ", copy " << outputColl->size());
      return StatusCode::FAILURE;
    }

    //final cluster to snapshot:
    typedef ElementLink<xAOD::CaloClusterContainer> ClusterLink_t;
    ClusterLink_t outputEL (outputColl.name(), 0, ctx);
    for (size_t i=0;i<nClusters;++i) {
      (*clusColl)[i]->setSisterClusterLink(ClusterLink_t(outputEL, i));
    }

    //From snapshot to final cluster
    if (m_finalContName) {
       ClusterLink_t finalEL (*m_finalContName, 0, ctx);
       for (size_t i=0;i<nClusters;++i) {
	 (*outputColl)[i]->setSisterClusterLink(ClusterLink_t(finalEL, i));
	 //	 std::cout << "Setting link to " << *m_finalContName << ", index "<< i <<std::endl;
       }
    }
    else
      ATH_MSG_DEBUG("Can't set element link from snapshot to final cluster, see warning above");
  }

  
  CHECK(CaloClusterStoreHelper::finalizeClusters(&(*evtStore()),outputColl.ptr(), outputColl.name(), msg()));

  
  return StatusCode::SUCCESS;
}

