/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHDECAYCOLLECTIONMAKER_H
#define DERIVATIONFRAMEWORK_TRUTHDECAYCOLLECTIONMAKER_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
// EDM -- typedefs so these are includes
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
// Handles and keys
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandle.h"
// Standard library includes
#include <vector>
#include <string>

namespace DerivationFramework {
 

  class TruthDecayCollectionMaker : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthDecayCollectionMaker(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthDecayCollectionMaker();
      StatusCode initialize();
      virtual StatusCode addBranches() const;

    private:
      std::vector<int> m_pdgIdsToKeep; //!< List of PDG IDs to build this collection from
      bool m_keepBHadrons; //!< Option to keep all b-hadrons (better than giving PDG IDs)
      bool m_keepCHadrons; //!< Option to keep all c-hadrons (better than giving PDG IDs)
      bool m_keepBSM; //!< Option to keep all BSM particles (better than giving PDG IDs)
      bool m_rejectHadronChildren; //!< Option to reject hadron descendants
      SG::ReadHandleKey<xAOD::TruthParticleContainer> m_particlesKey
         {this, "ParticlesKey", "TruthParticles", "ReadHandleKey for input TruthParticleContainer"};
      SG::WriteHandleKey<xAOD::TruthParticleContainer> m_outputParticlesKey
         {this, "NewParticleKey", "", "WriteHandleKey for new TruthParticleContainer"};
      SG::WriteHandleKey<xAOD::TruthVertexContainer> m_outputVerticesKey
         {this, "NewVertexKey", "", "WriteHandleKey for new TruthVertexContainer"};
      SG::WriteDecorHandleKey<xAOD::TruthParticleContainer> m_originDecoratorKey
         {this, "classifierParticleOrigin", "TruthParticles.classifierParticleOrigin","Name of the decoration which records the particle origin as determined by the MCTruthClassifier"};
      SG::WriteDecorHandleKey<xAOD::TruthParticleContainer> m_typeDecoratorKey
         {this, "classifierParticleType", "TruthParticles.classifierParticleType","Name of the decoration which records the particle type as determined by the MCTruthClassifier"};
      SG::WriteDecorHandleKey<xAOD::TruthParticleContainer> m_outcomeDecoratorKey
         {this, "classifierParticleOutCome", "TruthParticles.classifierParticleOutCome","Name of the decoration which records the particle outcome as determined by the MCTruthClassifier"};      
      SG::WriteDecorHandleKey<xAOD::TruthParticleContainer> m_classificationDecoratorKey
         {this, "Classification", "TruthParticles.Classification","Name of the decoration which records the particle outcome as determined by the MCTruthClassifier"};
      SG::WriteDecorHandleKey<xAOD::TruthParticleContainer> m_motherIDDecoratorKey
         {this, "motherID", "TruthParticles.motherID","Name of the decoration which records the ID of the particle's mother"};
      SG::WriteDecorHandleKey<xAOD::TruthParticleContainer> m_daughterIDDecoratorKey
         {this, "daughterID", "TruthParticles.daughterID","Name of the decoration which records the ID of the particle's daughter"};

      std::string m_collectionName; //!< Output collection name stem
      int m_generations; //!< Number of generations after the particle in question to keep
      // Helper functions for building up the decay product collections
      int addTruthParticle( const xAOD::TruthParticle& old_part, xAOD::TruthParticleContainer* part_cont,
                            xAOD::TruthVertexContainer* vert_cont, std::vector<int>& seen_particles,
                            const int generations=-1) const;
      int addTruthVertex( const xAOD::TruthVertex& old_vert, xAOD::TruthParticleContainer* part_cont,
                          xAOD::TruthVertexContainer* vert_cont, std::vector<int>& seen_particles,
                          const int generations=-1) const;
      bool id_ok( const xAOD::TruthParticle& part ) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHDECAYCOLLECTIONMAKER_H
