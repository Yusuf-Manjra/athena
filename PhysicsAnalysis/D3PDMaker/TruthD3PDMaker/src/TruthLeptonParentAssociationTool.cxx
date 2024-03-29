/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TruthD3PDMaker/src/TruthLeptonParentAssociationTool.cxx
 * @author Zach Marshall <zach.marshall@cern.ch>
 * @date June, 2013
 * @brief Associate from a lepton to its parent (boson, tau, hadron)
 */

#include "TruthLeptonParentAssociationTool.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AthenaKernel/errorcheck.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TruthUtils/HepMCHelpers.h"
#include <algorithm>

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthLeptonParentAssociationTool::TruthLeptonParentAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
    , m_primary_is_tau(false)
    , m_i (0)
{
  m_barcode_trace.reserve(10);
}

/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode
TruthLeptonParentAssociationTool::reset (const TruthParticle& p)
{
  m_i = 0;
  m_parents.clear();
  m_parent_barcodes.clear();
  m_barcode_trace.clear();

  m_primary_is_tau = (15==abs(p.pdgId()));

  // Just add the daughters in... but have to find it in the full record first
  const McEventCollection* mcec{nullptr};
  if (evtStore()->retrieve<McEventCollection>(mcec,"GEN_EVENT").isSuccess()){ // Always run on EVGEN anyway...
      auto Part =p.genParticle();
      if (Part){
          // Found it!
          addLeptonParent( (Part) );
      }
  } // Successful retrieve

  const TruthParticleContainer* tpc{nullptr};
  if (evtStore()->retrieve<TruthParticleContainer>(tpc,"D3PDTruth").isSuccess()){
    for (unsigned int i=0;i<tpc->size();++i){
      if (! (*tpc)[i] ) continue;
      for (unsigned int j=0;j<m_parent_barcodes.size();++j){
        if ( (*tpc)[i]->barcode() == m_parent_barcodes[j] ){
          m_parents.push_back( (*tpc)[i] );
          break;
        } // End matching barcode
      } // End loop over barcodes
    } // End loop over particles
  } // End successful retrieve

  return StatusCode::SUCCESS;
}

void TruthLeptonParentAssociationTool::addLeptonParent(HepMC::ConstGenParticlePtr part) {

  auto begvx = part->production_vertex();
  if(!begvx){ // no parents
    return;
  }

  // Catch for crazy generators
  if (begvx==part->end_vertex()) return;

  // More complex loop catch
  if ( find(m_barcode_trace.begin(),m_barcode_trace.end(),HepMC::barcode(begvx)) != m_barcode_trace.end()){
    ATH_MSG_DEBUG( "Found a loop (a la Sherpa sample).  Backing out." );
    return;
  }
  m_barcode_trace.push_back(HepMC::barcode(begvx));

  // Loop over the parents of this particle.
#ifdef HEPMC3
  auto itrPar = begvx->particles_in().begin();
  auto endPar = begvx->particles_in().end();
#else  
  HepMC::GenVertex::particle_iterator itrPar = begvx->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endPar = begvx->particles_end(HepMC::parents);
#endif
  int n_iter=0;
  for(;itrPar!=endPar; ++itrPar){
    if ( !(*itrPar) ) continue;  // parent didn't exist
    n_iter++;
    if (n_iter>2) break; // No point in trying - this vertex does not have a quantum meaning...

    int pdg = std::abs((*itrPar)->pdg_id());

    if ( (31<pdg && pdg<38) || // BSM Higgs / W' / Z' / etc
         pdg==39 ||
         pdg==41 ||
         pdg==42 ||
         (1000000<pdg && pdg<1000040) || // left-handed SUSY
         (2000000<pdg && pdg<2000040) ||
         pdg == 22 || // Photon
         pdg == 23 || // Z
         pdg == 24 || // W
         pdg == 25 || // Higgs
         (pdg == 15 && !m_primary_is_tau) || // Tau
         MC::isHadron (pdg) // from a hadron!
        ){
      m_parent_barcodes.push_back( HepMC::barcode(*itrPar) ); 
    } else { // Will get to here if we are coming from the same lepton again
      addLeptonParent( *itrPar );
    } // End of catch on PDG ID
  } // End loop over parents

}

/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const TruthParticle* TruthLeptonParentAssociationTool::next()
{
  if (m_i < m_parents.size())
    return m_parents[m_i++];
  return 0;
}

} // namespace D3PD
