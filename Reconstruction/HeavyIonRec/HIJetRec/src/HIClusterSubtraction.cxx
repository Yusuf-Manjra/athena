/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIClusterSubtraction.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

class ISvcLocator;
//**********************************************************************

HIClusterSubtraction::HIClusterSubtraction(std::string name) : asg::AsgTool(name)//,
{
}

//**********************************************************************

StatusCode HIClusterSubtraction::initialize()
{
	//Keys initialization
	ATH_CHECK( m_eventShapeKey.initialize() );
	ATH_CHECK( m_clusterKey.initialize() );

  for (auto tool :  m_clusterCorrectionTools)
  {
    StatusCode sc = tool.retrieve();
    if (sc.isFailure()) ATH_MSG_ERROR("Failed to retrieve correction tool " << tool);
    else ATH_MSG_DEBUG("Successfully retrieved correction tool " << tool);
  }
  return StatusCode::SUCCESS;
}

int HIClusterSubtraction::execute() const
{

  //const jet::cellset_t & badcells = badCellMap.cells() ;
  //retrieve UE
	//From here on temporarily commented out code bc needs a dedicated treatment in MT to compile
	//In rel 21 we were updating a non-const CaloCalusterContainer. That approach is no more acceptable in Athena MT
	ATH_MSG_WARNING("HIClusterSubtraction being rebuilt to work in MT - upgrade not yet over! ");
	const xAOD::HIEventShapeContainer* shape = 0;
	SG::ReadHandle<xAOD::HIEventShapeContainer>  readHandleEvtShape ( m_eventShapeKey );
  shape = readHandleEvtShape.cptr();
  const HIEventShapeIndex* es_index = HIEventShapeMap::getIndex( m_eventShapeKey.key() );

  const xAOD::HIEventShape* eshape = nullptr;
  CHECK(m_modulatorTool->getShape(eshape), 1);

  //New implementation: make a shallow copy of original HIClusters and apply subtraction to clusters in the new container
	SG::ReadHandle<xAOD::CaloClusterContainer>  readHandleClusters ( m_clusterKey );

	ATH_MSG_DEBUG("Shallow-copying HIClusters");

  std::pair<xAOD::CaloClusterContainer*,xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*readHandleClusters);

  /// Set whether only the overriden parameters should be written out - default is true
  //shallowcopy.second->setShallowIO(m_shallowIO);

  // Make sure that memory is managed safely
  std::unique_ptr<xAOD::CaloClusterContainer> outCaloClus(shallowcopy.first);
  std::unique_ptr<xAOD::ShallowAuxContainer> shallowAux(shallowcopy.second);

  // Connect the copied clusters to their originals
  xAOD::setOriginalObjectLink(*readHandleClusters, *outCaloClus);

  if(m_updateMode)
  {
		const xAOD::CaloClusterContainer* ccl = shallowcopy.first;
		//ccl = readHandleClusters.cptr();
    std::unique_ptr<std::vector<float> > subtractedE(new std::vector<float>());
    subtractedE->reserve(ccl->size());
		//Decoration via SG::AuxElement::Decorator should still work in MT code (that seems from browsing the code)
    SG::AuxElement::Decorator< float > decorator("HISubtractedE");

    for(xAOD::CaloClusterContainer::const_iterator itr=ccl->begin(); itr!=ccl->end(); itr++)
    {
      const xAOD::CaloCluster* cl=*itr;
      xAOD::IParticle::FourMom_t p4;
      m_subtractorTool->subtract(p4,cl,shape,es_index,m_modulatorTool,eshape);
      subtractedE->push_back(p4.E());
      decorator(*cl)=p4.E();
    }
  }
  else
  {
		xAOD::CaloClusterContainer* ccl = shallowcopy.first;
    for(xAOD::CaloClusterContainer::iterator itr=ccl->begin(); itr!=ccl->end(); itr++)
    {
      xAOD::CaloCluster* cl=*itr;
      xAOD::IParticle::FourMom_t p4;
      if(m_setMoments) m_subtractorTool->subtractWithMoments(cl, shape, es_index, m_modulatorTool, eshape);
      else
      {
					m_subtractorTool->subtract(p4,cl,shape,es_index,m_modulatorTool,eshape);
					HIJetRec::setClusterP4(p4,cl,HIJetRec::subtractedClusterState());
      }
    }
    for(ToolHandleArray<CaloClusterCollectionProcessor>::const_iterator toolIt=m_clusterCorrectionTools.begin();
	      toolIt != m_clusterCorrectionTools.end(); toolIt++)
    {
      ATH_MSG_DEBUG(" Applying correction = " << (*toolIt)->name() );
			CHECK((*toolIt)->execute(Gaudi::Hive::currentContext(), ccl), 1);
    }//End loop over correction tools
  }
  return 0;
}
