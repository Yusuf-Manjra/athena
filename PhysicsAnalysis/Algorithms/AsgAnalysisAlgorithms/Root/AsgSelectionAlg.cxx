/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgAnalysisAlgorithms/AsgSelectionAlg.h>

#include <PATInterfaces/ISystematicsTool.h>

//
// method implementations
//

namespace CP
{
  AsgSelectionAlg ::
  AsgSelectionAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_selectionTool ("", this)
  {
    declareProperty ("selectionTool", m_selectionTool, "the selection tool we apply");
  }



  StatusCode AsgSelectionAlg ::
  initialize ()
  {
    ANA_CHECK (m_selectionTool.retrieve());
    m_systematicsTool = dynamic_cast<ISystematicsTool*>(&*m_selectionTool);
    if (m_systematicsTool)
      ANA_CHECK (m_systematicsList.addSystematics (*m_systematicsTool));
      
    ANA_CHECK (m_particlesHandle.initialize (m_systematicsList));
    ANA_CHECK (m_preselection.initialize (m_systematicsList, m_particlesHandle, SG::AllowEmpty));
    ANA_CHECK (m_selectionHandle.initialize (m_systematicsList, m_particlesHandle));
    ANA_CHECK (m_systematicsList.initialize());

    asg::AcceptData blankAccept (&m_selectionTool->getAcceptInfo());
    // Just in case this isn't initially set up as a failure clear it this one
    // time. This only calls reset on the bitset
    blankAccept.clear();
    m_setOnFail = selectionFromAccept(blankAccept);

    return StatusCode::SUCCESS;
  }



  StatusCode AsgSelectionAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      if (m_systematicsTool)
        ANA_CHECK (m_systematicsTool->applySystematicVariation (sys));

      const xAOD::IParticleContainer *particles = nullptr;
      ANA_CHECK (m_particlesHandle.getCopy (particles, sys));
      for (const xAOD::IParticle *particle : *particles)
      {
        if (m_preselection.getBool (*particle, sys))
        {
          m_selectionHandle.setBits
            (*particle, selectionFromAccept (m_selectionTool->accept (particle)), sys);
        }
        else
        {
          m_selectionHandle.setBits(*particle, m_setOnFail, sys);
        }
      }
    }

    return StatusCode::SUCCESS;
  }
}
