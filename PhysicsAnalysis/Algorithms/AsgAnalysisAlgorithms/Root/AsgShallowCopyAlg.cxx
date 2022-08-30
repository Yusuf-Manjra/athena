/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgAnalysisAlgorithms/AsgShallowCopyAlg.h>

#include <CxxUtils/fpcompare.h>
#include <xAODCore/AuxContainerBase.h>
#include <xAODEgamma/PhotonContainer.h>
#include <xAODEgamma/ElectronContainer.h>
#include <xAODJet/JetContainer.h>
#include <xAODMuon/MuonContainer.h>
#include <xAODTau/TauJetContainer.h>
#include <xAODTau/DiTauJetContainer.h>
#include <xAODTracking/TrackParticleContainer.h>
#include <xAODTruth/TruthParticleContainer.h>
#include <SystematicsHandles/CopyHelpers.h>

//
// method implementations
//

namespace CP
{
  template<typename Type> StatusCode AsgShallowCopyAlg ::
  executeTemplate (const CP::SystematicSet& sys)
  {
    const Type *input = nullptr;
    ANA_CHECK (evtStore()->retrieve (input, m_inputHandle.getName (sys)));

    const auto& name = m_outputHandle.getName(sys);
    Type *output = nullptr;
    ANA_CHECK (detail::ShallowCopy<Type>::getCopy
               (msg(), *evtStore(), output, input,
                name, name + "Aux."));

    // suppress compiler warning about unused variable
    (void) output;

    return StatusCode::SUCCESS;
  }




  StatusCode AsgShallowCopyAlg ::
  executeFindType (const CP::SystematicSet& sys)
  {
    const xAOD::IParticleContainer *input = nullptr;
    ANA_CHECK (m_inputHandle.retrieve (input, sys));

    if (dynamic_cast<const xAOD::ElectronContainer*> (input))
    {
      m_function =
        &AsgShallowCopyAlg::executeTemplate<xAOD::ElectronContainer>;
    }
    else if (dynamic_cast<const xAOD::PhotonContainer*> (input))
    {
      m_function =
        &AsgShallowCopyAlg::executeTemplate<xAOD::PhotonContainer>;
    }
    else if (dynamic_cast<const xAOD::JetContainer*> (input))
    {
      m_function =
        &AsgShallowCopyAlg::executeTemplate<xAOD::JetContainer>;
    }
    else if (dynamic_cast<const xAOD::MuonContainer*> (input)) {
      m_function =
        &AsgShallowCopyAlg::executeTemplate<xAOD::MuonContainer>;
    }
    else if (dynamic_cast<const xAOD::TauJetContainer*> (input))
    {
      m_function =
        &AsgShallowCopyAlg::executeTemplate<xAOD::TauJetContainer>;
    }
    else if (dynamic_cast<const xAOD::DiTauJetContainer*> (input))
    {
      m_function =
        &AsgShallowCopyAlg::executeTemplate<xAOD::DiTauJetContainer>;
    }
    else if (dynamic_cast<const xAOD::TrackParticleContainer*> (input))
    {
      m_function =
        &AsgShallowCopyAlg::executeTemplate<xAOD::TrackParticleContainer>;
    }
    else if (dynamic_cast<const xAOD::TruthParticleContainer*> (input))
    {
      m_function =
        &AsgShallowCopyAlg::executeTemplate<xAOD::TruthParticleContainer>;
    }
    else
    {
      ANA_MSG_ERROR ("unknown type contained in AsgShallowCopyAlg, please extend it");
      return StatusCode::FAILURE;
    }

    return (this->*m_function) (sys);
  }



  AsgShallowCopyAlg ::
  AsgShallowCopyAlg (const std::string& name,
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
  }



  StatusCode AsgShallowCopyAlg ::
  initialize ()
  {
    ANA_CHECK (m_systematicsList.service().registerCopy (m_inputHandle.getNamePattern(), m_outputHandle.getNamePattern()));
    ANA_CHECK (m_inputHandle.initialize (m_systematicsList));
    ANA_CHECK (m_outputHandle.initialize (m_systematicsList));
    ANA_CHECK (m_systematicsList.initialize());

    return StatusCode::SUCCESS;
  }



  StatusCode AsgShallowCopyAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      ANA_CHECK ((this->*m_function) (sys));
    }
    return StatusCode::SUCCESS;
  }
}
