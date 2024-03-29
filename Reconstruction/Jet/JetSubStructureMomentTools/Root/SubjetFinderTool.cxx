/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/SubjetFinderTool.h"
#include "JetEDM/FastJetUtils.h"
#include "JetSubStructureUtils/SubjetFinder.h"

SubjetFinderTool::SubjetFinderTool(const std::string& name) : 
  JetSubStructureMomentToolsBase(name) 
{
  declareProperty("JetAlgorithm", m_jetalg = "Kt");
  declareProperty("JetRadius", m_jetrad = 0.3);
  declareProperty("PtMin", m_ptmin = 5000);
  declareProperty("SubjetRecorder", m_subjetrecorder);
  declareProperty("ExclusiveNjets", m_exclusivenjets = -1);
}

int SubjetFinderTool::modifyJet(xAOD::Jet &jet) const {
  if(checkForConstituents(jet) == false) return 1;

  // Set up jet algorithm
  xAOD::JetAlgorithmType::ID ialg = xAOD::JetAlgorithmType::algId(m_jetalg);
  fastjet::JetAlgorithm fjalg = xAOD::JetAlgorithmType::fastJetDef(ialg);

  // Run it
  JetSubStructureUtils::SubjetFinder subjetFinder(fjalg, m_jetrad, m_ptmin, m_exclusivenjets);
  std::vector<fastjet::PseudoJet> subjets = subjetFinder.result(jet);

  // Record
  m_subjetrecorder->recordSubjets(subjets, jet);

  return 0;
}
