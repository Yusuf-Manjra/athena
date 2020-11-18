/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MuonIDCovMatrixDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch) 
// Decorates muon objects with the covariance matrix (std::vector<float>) 
// from the corresponding inner detector track particle
#include "DerivationFrameworkMuons/MuonIDCovMatrixDecorator.h"
#include "AthenaKernel/errorcheck.h"
#include <vector>
#include <string>
#include <memory>
#include "xAODMuon/MuonContainer.h"

// Constructor
DerivationFramework::MuonIDCovMatrixDecorator::MuonIDCovMatrixDecorator(const std::string& t,
							    const std::string& n,
							    const IInterface* p):
  AthAlgTool(t, n, p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("MuonContainerKey", m_muonSGKey="Muons");
}
  
// Destructor
DerivationFramework::MuonIDCovMatrixDecorator::~MuonIDCovMatrixDecorator() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::MuonIDCovMatrixDecorator::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MuonIDCovMatrixDecorator::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MuonIDCovMatrixDecorator::addBranches() const
{
 
  // Retrieve main TrackParticle collection
  const xAOD::MuonContainer* muons;
  if (evtStore()->retrieve(muons,m_muonSGKey).isFailure()) {
    ATH_MSG_ERROR("No Muon collection with name " << m_muonSGKey << " found in StoreGate!");
    return StatusCode::FAILURE;
  }

  // Set up the decorator
  SG::AuxElement::Decorator< std::vector<float>  > decorator("inDet_definingParametersCovarianceMatrix"); 

  // Loop over the muons and apply the decoration
  std::vector<float> defaultCovarianceMatrix(15,0.0); // in case the ID track doesn't exist
  for (auto mu : *muons) {
    auto trackParticle = mu->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle);
    if (trackParticle==nullptr) {
      decorator(*mu) = defaultCovarianceMatrix;            
    } else {
     auto covarianceMatrix = trackParticle->definingParametersCovMatrixVec();
     decorator(*mu) = covarianceMatrix;      
    }
  }

  return StatusCode::SUCCESS;
}

