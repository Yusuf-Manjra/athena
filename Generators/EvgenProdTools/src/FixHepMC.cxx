/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "EvgenProdTools/FixHepMC.h"
#include "TruthUtils/HepMCHelpers.h"


FixHepMC::FixHepMC(const std::string& name, ISvcLocator* pSvcLocator)
  : GenBase(name, pSvcLocator)
  , m_loopKilled(0)
  , m_pdg0Killed(0)
  , m_decayCleaned(0)
  , m_totalSeen(0)
{
  declareProperty("KillLoops", m_killLoops = true, "Remove particles in loops?");
  declareProperty("KillPDG0", m_killPDG0 = true, "Remove particles with PDG ID 0?");
  declareProperty("CleanDecays", m_cleanDecays = true, "Clean decay chains from non-propagating particles?");
  declareProperty("LoopsByBarcode", m_loopByBC = false, "Detect loops based on barcodes as well as vertices?");
}


StatusCode FixHepMC::execute() {
  for (McEventCollection::const_iterator ievt = events()->begin(); ievt != events()->end(); ++ievt) {
    // FIXME: const_cast
    HepMC::GenEvent* evt = const_cast<HepMC::GenEvent*>(*ievt);
#ifdef HEPMC3
    // Add a unit entry to the event weight vector if it's currently empty
    if (evt->weights().empty()) {
      ATH_MSG_DEBUG("Adding a unit weight to empty event weight vector");
      evt->weights().push_back(1);
    }
    // Set a (0,0,0) vertex to be the signal vertex if not already set
    if (!HepMC::signal_process_vertex(evt)) {
      const HepMC::FourVector nullpos;
      for (auto  iv: evt->vertices()) {
        if (iv->position() == nullpos) {
          ATH_MSG_DEBUG("Setting representative event position vertex");
          HepMC::set_signal_process_vertex(evt,iv);
          break;
        }
      }
    }

    // Some heuristics
    std::vector<HepMC::GenParticlePtr> tofix;
    std::vector<HepMC::GenParticlePtr> bad_pdg_id_particles;
    for (auto ip: evt->particles()) {
      // Skip this particle if (somehow) its pointer is null
      if (!ip) continue;
      bool particle_to_fix = false;
      /// Two types of bad particles: those w/o prod.vertex and with status other than 4.
      if ( (!ip->production_vertex() || ip->production_vertex()->id() == 0 ) &&  ip->end_vertex() && ip->status() != 4 ) particle_to_fix = true;
      /// Those w/o end vertex, but with bad status
      if (ip->production_vertex() && !ip->end_vertex() && ip->status() != 1 ) particle_to_fix = true;
      if (particle_to_fix) tofix.push_back(ip);
      int pdg_id = ip->pdg_id();
      if (pdg_id == 43 || pdg_id == 44 || pdg_id == -43 || pdg_id == -44 ) bad_pdg_id_particles.push_back(ip);
    }

    /// AV: In case we have 3 particles, we try to add a vertex that correspond to 1->2 and 1->1 splitting.
    if (tofix.size() == 3 || tofix.size() == 2) {
      int no_endv = 0;
      int no_prov = 0;
      HepMC::FourVector sum(0,0,0,0);
      for (auto part: tofix) if (!part->production_vertex() || !part->production_vertex()->id()) { no_prov++; sum += part->momentum();}  
      for (auto part: tofix) if (!part->end_vertex()) { no_endv++;  sum -= part->momentum(); }
      ATH_MSG_INFO("Heuristics: found " << tofix.size() << "particles to fix. The momenta sum is " << sum);
      if (no_endv == 1 && (no_prov == 2 || no_prov == 1) && std::abs(sum.px()) < 1e-2  && std::abs(sum.py()) < 1e-2  && std::abs(sum.pz()) < 1e-2 ) {
          ATH_MSG_INFO("Try " << no_endv << "->" << no_prov << " splitting.");
          auto v = HepMC::newGenVertexPtr();
          for (auto part: tofix) if (!part->production_vertex() || part->production_vertex()->id() == 0) v->add_particle_out(part);  
          for (auto part: tofix) if (!part->end_vertex()) v->add_particle_in(part);  
          evt->add_vertex(v);
      }
    }
    /// AV: Please note that this approach would distort some branching ratios.
    /// If some particle would have decay products with bad PDG ids, after the operation below
    /// the visible branching ratio of these decays would be zero.
    for (auto part: bad_pdg_id_particles) {
        if (!part->production_vertex()) continue;
        if (!part->end_vertex()) continue;
        for (auto p: part->end_vertex()->particles_out()) part->production_vertex()->add_particle_out(p);
        evt->remove_particle(part);
    }

    // Event particle content cleaning -- remove "bad" structures
    std::vector<HepMC::GenParticlePtr> toremove;
    long seenThisEvent = 0;
    for (auto ip: evt->particles()) {
      // Skip this particle if (somehow) its pointer is null
      if (!ip) continue;
      m_totalSeen += 1;
      seenThisEvent += 1;
      // Flag to declare if a particle should be removed
      bool bad_particle = false;
      // Check for loops
      if ( m_killLoops && isLoop(ip) ) {
        bad_particle = true;
        m_loopKilled += 1;
        ATH_MSG_DEBUG( "Found a looper : " );
        if ( msgLvl( MSG::DEBUG ) ) HepMC::Print::line(ip);
      }
      // Check on PDG ID 0
      if ( m_killPDG0 && isPID0(ip) ) {
        bad_particle = true;
        m_pdg0Killed += 1;
        ATH_MSG_DEBUG( "Found PDG ID 0 : " );
        if ( msgLvl( MSG::DEBUG ) )HepMC::Print::line(ip);
      }
      // Clean decays
      int abs_pdg_id = std::abs(ip->pdg_id());
      bool is_decayed_weak_boson =  ( abs_pdg_id == 23 || abs_pdg_id == 24 || abs_pdg_id == 25 ) && ip->end_vertex();
      if ( m_cleanDecays && isNonTransportableInDecayChain(ip) && !is_decayed_weak_boson ) {
        bad_particle = true;
        m_decayCleaned += 1;
        ATH_MSG_DEBUG( "Found a bad particle in a decay chain : " );
        if ( msgLvl( MSG::DEBUG ) ) HepMC::Print::line(ip);
      }
      // Only add to the toremove vector once, even if multiple tests match
      if (bad_particle) toremove.push_back(ip);
    }

    // Escape here if there's nothing more to do, otherwise do the cleaning
    if (toremove.empty()) continue;
    ATH_MSG_DEBUG("Cleaning event record of " << toremove.size() << " bad particles");
    // Properties before cleaning
    const int num_particles_orig = evt->particles().size();
    for (auto part: toremove) evt->remove_particle(part);
    const int num_particles_filt = evt->particles().size();
     // Write out the change in the number of particles
    ATH_MSG_INFO("Particles filtered: " << num_particles_orig << " -> " << num_particles_filt);
 #else

    // Add a unit entry to the event weight vector if it's currently empty
    if (evt->weights().empty()) {
      ATH_MSG_DEBUG("Adding a unit weight to empty event weight vector");
      evt->weights().push_back(1);
    }

    // Set a (0,0,0) vertex to be the signal vertex if not already set
    if (evt->signal_process_vertex() == NULL) {
      const HepMC::FourVector nullpos;
      for (HepMC::GenEvent::vertex_const_iterator iv = evt->vertices_begin(); iv != evt->vertices_end(); ++iv) {
        if ((*iv)->position() == nullpos) {
          ATH_MSG_DEBUG("Setting representative event position vertex");
          evt->set_signal_process_vertex(const_cast<HepMC::GenVertex*>(*iv));
          break;
        }
      }
    }

    // Event particle content cleaning -- remove "bad" structures
    std::vector<HepMC::GenParticlePtr> toremove; toremove.reserve(10);
    long seenThisEvent = 0;
    for (HepMC::GenEvent::particle_const_iterator ip = evt->particles_begin(); ip != evt->particles_end(); ++ip) {
      // Skip this particle if (somehow) its pointer is null
      if (*ip == NULL) continue;
      m_totalSeen += 1;
      seenThisEvent += 1;

      // Flag to declare if a particle should be removed
      bool bad_particle = false;

      // Check for loops
      if ( m_killLoops && isLoop(*ip) ) {
        bad_particle = true;
        m_loopKilled += 1;
        ATH_MSG_DEBUG( "Found a looper : " );
        if ( msgLvl( MSG::DEBUG ) ) (*ip)->print();
      }

      // Check on PDG ID 0
      if ( m_killPDG0 && isPID0(*ip) ) {
        bad_particle = true;
        m_pdg0Killed += 1;
        ATH_MSG_DEBUG( "Found PDG ID 0 : " );
        if ( msgLvl( MSG::DEBUG ) ) (*ip)->print();
      }

      // Clean decays
      int abs_pdg_id = std::abs((*ip)->pdg_id());
      bool is_decayed_weak_boson =  ( abs_pdg_id == 23 || abs_pdg_id == 24 || abs_pdg_id == 25 ) && (*ip)->end_vertex();
      if ( m_cleanDecays && isNonTransportableInDecayChain(*ip) && !is_decayed_weak_boson ) {
        bad_particle = true;
        m_decayCleaned += 1;
        ATH_MSG_DEBUG( "Found a bad particle in a decay chain : " );
        if ( msgLvl( MSG::DEBUG ) ) (*ip)->print();
      }

      // Only add to the toremove vector once, even if multiple tests match
      if (bad_particle) toremove.push_back(*ip);
    }

    // Escape here if there's nothing more to do, otherwise do the cleaning
    if (toremove.empty()) continue;
    ATH_MSG_DEBUG("Cleaning event record of " << toremove.size() << " bad particles");

    // Properties before cleaning
    const int num_particles_orig = evt->particles_size();
    const int num_orphan_vtxs_orig = MC::const_vertices_match(evt, MC::isDisconnected).size();
    const int num_noparent_vtxs_orig = MC::const_vertices_match(evt, MC::hasNoParents).size();
    const int num_nochild_vtxs_orig = MC::const_vertices_match(evt, MC::hasNoChildren).size();

    // Clean!
    int signal_vertex_bc = evt->signal_process_vertex() ? evt->signal_process_vertex()->barcode() : 0;
    MC::reduce(evt , toremove);
    if (evt->barcode_to_vertex (signal_vertex_bc) == nullptr) {
      evt->set_signal_process_vertex (nullptr);
    }

    // Properties after cleaning
    const int num_particles_filt = evt->particles_size();
    const int num_orphan_vtxs_filt = MC::const_vertices_match(evt, MC::isDisconnected).size();
    const int num_noparent_vtxs_filt = MC::const_vertices_match(evt, MC::hasNoParents).size();
    const int num_nochild_vtxs_filt = MC::const_vertices_match(evt, MC::hasNoChildren).size();

    // Write out the change in the number of particles
    ATH_MSG_INFO("Particles filtered: " << num_particles_orig << " -> " << num_particles_filt);
    // Warn if the numbers of "strange" vertices have changed
    if (num_orphan_vtxs_filt != num_orphan_vtxs_orig)
      ATH_MSG_WARNING("Change in orphaned vertices: " << num_orphan_vtxs_orig << " -> " << num_orphan_vtxs_filt);
    if (num_noparent_vtxs_filt != num_noparent_vtxs_orig)
      ATH_MSG_WARNING("Change in no-parent vertices: " << num_noparent_vtxs_orig << " -> " << num_noparent_vtxs_filt);
    if (num_nochild_vtxs_filt != num_nochild_vtxs_orig)
      ATH_MSG_WARNING("Change in no-parent vertices: " << num_nochild_vtxs_orig << " -> " << num_nochild_vtxs_filt);

#endif
  }
  return StatusCode::SUCCESS;
}


StatusCode FixHepMC::finalize() {
  if (m_killLoops  ) ATH_MSG_INFO( "Removed " <<   m_loopKilled << " of " << m_totalSeen << " particles because of loops." );
  if (m_killPDG0   ) ATH_MSG_INFO( "Removed " <<   m_pdg0Killed << " of " << m_totalSeen << " particles because of PDG ID 0." );
  if (m_cleanDecays) ATH_MSG_INFO( "Removed " << m_decayCleaned << " of " << m_totalSeen << " particles while cleaning decay chains." );
  return StatusCode::SUCCESS;
}


/// @name Classifiers for identifying particles to be removed
//@{

// Identify PDG ID = 0 particles, usually from HEPEVT padding
bool FixHepMC::isPID0(HepMC::ConstGenParticlePtr p) {
  return p->pdg_id() == 0;
}

// Identify non-transportable stuff _after_ hadronisation
bool FixHepMC::isNonTransportableInDecayChain(HepMC::ConstGenParticlePtr p) {
  return !MC::PID::isTransportable(p->pdg_id()) && MC::fromDecay(p);
}

// Identify internal "loop" particles
bool FixHepMC::isLoop(HepMC::ConstGenParticlePtr p) {
  if (p->production_vertex() == p->end_vertex() && p->end_vertex() != NULL) return true;
  if (m_loopByBC && p->production_vertex()) {
    /// @todo Use new particle MC::parents(...) tool
    for (auto itrParent: *(p->production_vertex())) {
      if ( HepMC::barcode(itrParent) > HepMC::barcode(p) ) {
        ATH_MSG_VERBOSE("Found a loop (a la Sherpa sample) via barcode.");
        return true; // Cannot vectorize, but this is a pretty short loop
      } // Check on barcodes
    } // Loop over parent particles
  } // Has a production vertex
  return false;
}

//@}

#endif

