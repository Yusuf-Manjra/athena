/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthClassificationDecorator.cxx
// Decorates truth particles with the output of the MCTruthClassifier

#include "DerivationFrameworkMCTruth/TruthClassificationDecorator.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteDecorHandle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TruthClassificationDecorator::TruthClassificationDecorator(const std::string& t,
                                                                  const std::string& n,
                                                                  const IInterface* p ) :
AthAlgTool(t,n,p),
m_ntotpart(0),
m_classifier("MCTruthClassifier/MCTruthClassifier")
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("MCTruthClassifier", m_classifier);
}

// Destructor
DerivationFramework::TruthClassificationDecorator::~TruthClassificationDecorator() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthClassificationDecorator::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_CHECK(m_classifier.retrieve());

    ATH_CHECK( m_particlesKey.initialize() );
    ATH_MSG_INFO("Decorating " << m_particlesKey.key() << " with classification information");

    // Decorators
    ATH_CHECK(m_linkDecoratorKey.initialize());    
    ATH_CHECK(m_originDecoratorKey.initialize());
    ATH_CHECK(m_typeDecoratorKey.initialize());
    ATH_CHECK(m_outcomeDecoratorKey.initialize());
    ATH_CHECK(m_classificationDecoratorKey.initialize());

    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TruthClassificationDecorator::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed and decorated "<< m_ntotpart <<" truth particles");
    return StatusCode::SUCCESS;
}

// Selection and collection creation
StatusCode DerivationFramework::TruthClassificationDecorator::addBranches() const
{
    
    // Event context for multi-threading
    const EventContext& ctx = Gaudi::Hive::currentContext();

    // Retrieve truth collections
    SG::ReadHandle<xAOD::TruthParticleContainer> truthParticles(m_particlesKey,ctx);
    if (!truthParticles.isValid()) {
        ATH_MSG_ERROR("Couldn't retrieve TruthParticle collection with name " << m_particlesKey);
        return StatusCode::FAILURE;
    }
  
    unsigned int nParticles = truthParticles->size();
    m_ntotpart += nParticles;
    
    // Set up decorators
    SG::WriteDecorHandle<xAOD::TruthParticleContainer, ElementLink<xAOD::TruthParticleContainer> > linkDecorator(m_linkDecoratorKey, ctx);
    SG::WriteDecorHandle<xAOD::TruthParticleContainer, unsigned int> originDecorator(m_originDecoratorKey, ctx); 
    SG::WriteDecorHandle<xAOD::TruthParticleContainer, unsigned int> typeDecorator(m_typeDecoratorKey, ctx);  
    SG::WriteDecorHandle<xAOD::TruthParticleContainer, unsigned int> outcomeDecorator(m_outcomeDecoratorKey, ctx);
    SG::WriteDecorHandle<xAOD::TruthParticleContainer, unsigned int> classificationDecorator(m_classificationDecoratorKey, ctx);

    for (unsigned int i=0; i<nParticles; ++i) {
#ifdef MCTRUTHCLASSIFIER_CONST
        IMCTruthClassifier::Info info;
        std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> classification = 
          m_classifier->particleTruthClassifier((*truthParticles)[i], &info);
          unsigned int particleOutCome = info.particleOutCome;

	  unsigned int result = (unsigned int)m_classifier->classify((*truthParticles)[i]);
#else
        std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> classification = 
        m_classifier->particleTruthClassifier((*truthParticles)[i]);
        unsigned int particleOutCome = m_classifier->getParticleOutCome();

	unsigned int result = (unsigned int)m_classifier->classify((*truthParticles)[i]);
#endif
        unsigned int particleType = classification.first;
        unsigned int particleOrigin = classification.second;
        typeDecorator(*((*truthParticles)[i])) = particleType;
        originDecorator(*((*truthParticles)[i])) = particleOrigin;
        outcomeDecorator(*((*truthParticles)[i])) = particleOutCome;  

	classificationDecorator(*((*truthParticles)[i])) = result;
    }

    return StatusCode::SUCCESS;
}

