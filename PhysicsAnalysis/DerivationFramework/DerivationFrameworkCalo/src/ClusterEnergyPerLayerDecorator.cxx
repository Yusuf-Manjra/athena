/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ClusterEnergyPerLayerDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Bruno Lenzi

#include "DerivationFrameworkCalo/ClusterEnergyPerLayerDecorator.h"
#include "xAODEgamma/EgammaContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"

#include <TString.h>

// Constructor
DerivationFramework::ClusterEnergyPerLayerDecorator::ClusterEnergyPerLayerDecorator(const std::string& t,
						  const std::string& n,
						  const IInterface* p) :
  AthAlgTool(t,n,p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("SGKey_photons", m_SGKey_photons);
  declareProperty("SGKey_electrons", m_SGKey_electrons);
  declareProperty("neta", m_eta_size);
  declareProperty("nphi", m_phi_size);
  declareProperty("SGKey_caloCells", m_CellCollectionName="AllCalo", "Name of the CaloCellContainer");
  declareProperty("layers", m_layers = {0,1,2,3});  
}



// Destructor
DerivationFramework::ClusterEnergyPerLayerDecorator::~ClusterEnergyPerLayerDecorator() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::ClusterEnergyPerLayerDecorator::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  if(m_SGKey_photons.empty() and m_SGKey_electrons.empty()){
    ATH_MSG_FATAL("No e-gamma collection provided");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_caloFillRectangularTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::ClusterEnergyPerLayerDecorator::finalize()
{

  return StatusCode::SUCCESS;
}

// The decoration itself
StatusCode DerivationFramework::ClusterEnergyPerLayerDecorator::addBranches() const
{

  const EventContext& ctx=Gaudi::Hive::currentContext();
  for (const std::string& SGkey : {m_SGKey_photons, m_SGKey_electrons})
  {
    if (SGkey.empty()) continue;
    const xAOD::EgammaContainer *container(nullptr);
    ATH_CHECK( evtStore()->retrieve(container, SGkey) );
    for (const auto *egamma : *container){
      decorateObject(ctx, egamma);
    }
  }

  return StatusCode::SUCCESS;
}

void
DerivationFramework::ClusterEnergyPerLayerDecorator::decorateObject(
  const EventContext& ctx,
  const xAOD::Egamma* egamma) const
{

  if (not egamma or not egamma->caloCluster()) return;
  
  const CaloCellContainer* cellCont(nullptr);
  xAOD::CaloCluster* egcClone(nullptr);
  
  if (evtStore()->retrieve(cellCont,m_CellCollectionName).isFailure())
    ATH_MSG_WARNING(m_CellCollectionName<< " not found");
  else if (not egamma->author(xAOD::EgammaParameters::AuthorCaloTopo35) )
  {
    egcClone =
      CaloClusterStoreHelper::makeCluster(cellCont,
                                          egamma->caloCluster()->eta0(),
                                          egamma->caloCluster()->phi0(),
                                          egamma->caloCluster()->clusterSize());
    const CaloFillRectangularCluster* tool = dynamic_cast<const CaloFillRectangularCluster* >( &(*m_caloFillRectangularTool ));
    tool->makeCorrection(ctx, egcClone);
  }
  
  for (unsigned int layer: m_layers){
    egamma->auxdecor<float>(
      Form("E%dx%d_Lr%d", m_eta_size, m_phi_size, layer)) =
      egcClone ? egcClone->energyBE(layer) : 0.;
  }

  delete egcClone;
}
