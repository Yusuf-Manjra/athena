/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetClusterer.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/config.h"
#include "fastjet/AreaDefinition.hh"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "JetEDM/FastJetUtils.h"
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/ClusterSequence.h"



JetClusterer::JetClusterer(std::string name)
: AsgTool(name),
  m_jetFromPseudoJet("JetFromPseudojet") {

}


StatusCode JetClusterer::initialize() {

  ATH_MSG_DEBUG("Initializing...");
  xAOD::JetAlgorithmType::ID ialg = xAOD::JetAlgorithmType::algId(m_jetalg);
  m_fjalg = xAOD::JetAlgorithmType::fastJetDef(ialg);
  if ( m_fjalg == fastjet::undefined_jet_algorithm ) {
    ATH_MSG_ERROR("Invalid jet algorithm name: " << m_jetalg);
    ATH_MSG_ERROR("Allowed values are Kt, CamKt, AntiKt, etc.");
    return StatusCode::FAILURE;
  }
  if ( m_jetrad <=0 ) {
    ATH_MSG_ERROR("Invalid jet size parameter: " << m_jetrad);
    return StatusCode::FAILURE;
  }
  
  // buld an empty ClusterSequence, just for the fastjet splash screen to appear during initialization (?)
  fastjet::JetDefinition jetdef(m_fjalg, m_jetrad);
  PseudoJetVector empty;
  fastjet::ClusterSequence cs(empty, jetdef);
  cs.inclusive_jets(m_ptmin);
  
  // Input DataHandles
  ATH_CHECK( m_eventinfokey.initialize() );
  ATH_CHECK( m_inputPseudoJets.initialize() );
  return StatusCode::SUCCESS;
}


xAOD::JetContainer* JetClusterer::build() const {

  // Build the container to be returned 
  std::unique_ptr<xAOD::JetContainer>  jets = std::make_unique<xAOD::JetContainer>() ;
  jets->setStore(new xAOD::JetAuxContainer);  
  
  SG::ReadHandle<PseudoJetContainer> pjContHandle(m_inputPseudoJets);
  if(!pjContHandle.isValid()) {
    ATH_MSG_ERROR("No valid PseudoJetContainer with key "<< m_inputPseudoJets.key());
    return nullptr;
  }

  const PseudoJetVector* pseudoJetVector = pjContHandle->casVectorPseudoJet();

  fastjet::JetDefinition jetdef(m_fjalg, m_jetrad);

  fastjet::ClusterSequence *clSequence = nullptr;
  if ( m_ghostarea <= 0 ) {
    ATH_MSG_DEBUG("Creating input cluster sequence");
    clSequence = new fastjet::ClusterSequence(*pseudoJetVector, jetdef);    
  } else {
    // Prepare ghost area specifications -------------
    fastjet::GhostedAreaSpec gspec(5.0, 1, m_ghostarea);

    if ( m_ranopt == 1 ) {
      // Use run/event number as random number seeds.      
      auto evtInfoHandle = SG::makeHandle(m_eventinfokey);
      if (!evtInfoHandle.isValid()){
        ATH_MSG_ERROR("Unable to retrieve event info");
        return nullptr;
      }
      //const xAOD::EventInfo* pevinfo = handle.cptr();
      auto ievt = evtInfoHandle->eventNumber();
      auto irun = evtInfoHandle->runNumber();
      
      if ( evtInfoHandle->eventType(xAOD::EventInfo::IS_SIMULATION)) {
	// For MC, use the channel and MC event number
	ievt = evtInfoHandle->mcEventNumber();
	irun = evtInfoHandle->mcChannelNumber();
      }
      
      std::vector<int> inseeds;
      inseeds.push_back(ievt);
      inseeds.push_back(irun);

      gspec.set_random_status(inseeds);
    }

    
    ATH_MSG_DEBUG("Active area specs:");
    ATH_MSG_DEBUG("  Requested ghost area: " << m_ghostarea);
    ATH_MSG_DEBUG("     Actual ghost area: " << gspec.actual_ghost_area());
    ATH_MSG_DEBUG("               Max eta: " << gspec.ghost_etamax());
    ATH_MSG_DEBUG("              # ghosts: " << gspec.n_ghosts());
    ATH_MSG_DEBUG("       # rapidity bins: " << gspec.nrap());
    ATH_MSG_DEBUG("            # phi bins: " << gspec.nphi());
    std::vector<int> seeds;
    gspec.get_random_status(seeds);
    if ( seeds.size() == 2 ) {
      ATH_MSG_DEBUG("          Random seeds: " << seeds[0] << ", " << seeds[1]);
    } else {
      ATH_MSG_WARNING("Random generator size is not 2: " << seeds.size());
      ATH_MSG_DEBUG("          Random seeds: ");
      for ( auto seed : seeds ) ATH_MSG_DEBUG("                 " << seed);
    }

    fastjet::AreaDefinition adef(fastjet::active_area_explicit_ghosts, gspec);
    ATH_MSG_DEBUG("Creating input area cluster sequence");
    clSequence = new fastjet::ClusterSequenceArea(*pseudoJetVector, jetdef, adef);
  }

  
  // -----------------------
  // we have the cluster sequence .
  
  PseudoJetVector outs = fastjet::sorted_by_pt(clSequence->inclusive_jets(m_ptmin));
  ATH_MSG_DEBUG("Found jet count: " << outs.size());

  // Let fastjet deal with deletion of ClusterSequence, so we don't need to put it in the EventStore.
  clSequence->delete_self_when_unused();
  
  for (const fastjet::PseudoJet &  pj: outs ) {
    // create the xAOD::Jet from the PseudoJet, doing the signal & ghost constituents extraction
    xAOD::Jet* pjet = m_jetFromPseudoJet.add(pj, *pjContHandle, *jets, xAOD::JetInput::Type( (int) m_inputType));
    
    xAOD::JetAlgorithmType::ID ialg = xAOD::JetAlgorithmType::algId(m_fjalg);
    pjet->setAlgorithmType(ialg);
    pjet->setSizeParameter(m_jetrad);
    pjet->setAttribute("JetGhostArea", m_ghostarea);
  }
  ATH_MSG_DEBUG("Reconstructed jet count: " << jets->size() <<  "  clusterseq="<<clSequence);
  
  return jets.release();  
}
