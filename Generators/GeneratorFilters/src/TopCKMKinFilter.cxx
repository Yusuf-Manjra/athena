/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TopCKMKinFilter.h"
#include <cmath>

TopCKMKinFilter::TopCKMKinFilter(const std::string& fname, ISvcLocator* pSvcLocator)
  : GenFilter(fname, pSvcLocator)
{
  static const int chs[] = {3, 5};
  static const std::vector<int> vchs(chs, chs + sizeof(chs) / sizeof(int));

  declareProperty("PDGChild", m_PDGChild=vchs);
  declareProperty("PtMinChild", m_PtMinChild=0.);
  declareProperty("EtaRangeChild", m_EtaRangeChild=10.0);
}


StatusCode TopCKMKinFilter::filterInitialize() {
 if (m_PDGChild.empty()) ATH_MSG_ERROR( "PDGChild[] not set ");

 for (size_t i = 0; i < m_PDGChild.size(); ++i) {
   ATH_MSG_INFO(  "PDGChild[" << i << "] = " << m_PDGChild[i] );
 }
 ATH_MSG_INFO( "PtMinChild = " << m_PtMinChild );
 ATH_MSG_INFO( "EtaRangeChild = " << m_EtaRangeChild );
 return StatusCode::SUCCESS;
}


StatusCode TopCKMKinFilter::filterEvent() {
  for (McEventCollection::const_iterator itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent * genEvt = (*itr);
    HepMC::GenEvent::particle_const_iterator part1 = genEvt->particles_begin();
    HepMC::GenEvent::particle_const_iterator partItr = part1;
    HepMC::GenEvent::particle_const_iterator partE = genEvt->particles_end();

    // Get the number of parents on the event (ttbar --> 2 tops!)
    int nParents = 0;
    for (partItr = part1; partItr != partE; ++partItr){
      if (std::abs((*partItr)->pdg_id()) == 6) ++nParents;
    }

    // Get pointers to both parents
    if (nParents == 2) {

      const HepMC::GenParticlePtr firstParent = nullptr;
      for (partItr = part1; partItr != partE; ++partItr){
        if (std::abs((*partItr)->pdg_id()) == 6) firstParent = (*partItr);
      }

      const HepMC::GenParticlePtr secondParent = nullptr;
      for (partItr = part1; partItr != partE; ++partItr){
        if (std::abs((*partItr)->pdg_id()) == 6 && (*partItr) != firstParent) secondParent = (*partItr);
      }

      // Now check that the first and second parents have the corresponding childs
      if (firstParent && secondParent) {
        HepMC::GenVertexPtr firstVertex = firstParent->end_vertex();
        HepMC::GenVertexPtr secondVertex = secondParent->end_vertex();

        HepMC::GenVertex::particle_iterator firstChild1 = firstVertex->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator firstChildItr = firstChild1;
        HepMC::GenVertex::particle_iterator firstChildE = firstVertex->particles_end(HepMC::children);

        HepMC::GenVertex::particle_iterator secondChild1 = secondVertex->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator secondChildItr = secondChild1;
        HepMC::GenVertex::particle_iterator secondChildE = secondVertex->particles_end(HepMC::children);

        // Check the children
        bool isOK1 = 0;
        bool isOK2 = 0;
        HepMC::GenParticlePtr quark1 = nullptr;
        HepMC::GenParticlePtr quark2 = nullptr;
        HepMC::GenParticlePtr wBoson1 = nullptr;
        HepMC::GenParticlePtr wBoson2 = nullptr;

        for (firstChildItr = firstChild1; firstChildItr != firstChildE; ++firstChildItr){
          if ((std::abs((*firstChildItr)->pdg_id()) == m_PDGChild[0] || std::abs((*firstChildItr)->pdg_id()) == m_PDGChild[1])
              && (*firstChildItr)->momentum().perp() > m_PtMinChild
              && fabs((*firstChildItr)->momentum().eta()) < m_EtaRangeChild){
            isOK1 = 1;
            quark1 = (*firstChildItr);
          }
          if (std::abs((*firstChildItr)->pdg_id()) == 24) wBoson1 = (*firstChildItr);
        }

        for (secondChildItr = secondChild1; secondChildItr != secondChildE; ++secondChildItr){
          if ((std::abs((*secondChildItr)->pdg_id()) == m_PDGChild[0] || std::abs((*secondChildItr)->pdg_id()) == m_PDGChild[1])
              && (*secondChildItr)->momentum().perp() > m_PtMinChild
              && fabs((*secondChildItr)->momentum().eta()) < m_EtaRangeChild){
            isOK2 = 1;
            quark2 = (*secondChildItr);
          }
          if (std::abs((*secondChildItr)->pdg_id()) == 24) wBoson2 = (*secondChildItr);
        }

        // Now avoid the decay W --> tau nu and impose kinematic cuts for leptons
        bool isTau1 = 0;
        bool isTau2 = 0;
        bool isKinLep1 = 0;
        bool isKinLep2 = 0;

        HepMC::GenVertexPtr wVertex1 = wBoson1->end_vertex();
        HepMC::GenVertexPtr wVertex2 = wBoson2->end_vertex();

        HepMC::GenVertex::particle_iterator w1Son1, w1SonItr, w1SonE;
        HepMC::GenVertex::particle_iterator w2Son1, w2SonItr, w2SonE;

        w1Son1 = wVertex1->particles_begin(HepMC::children);
        w1SonE = wVertex1->particles_end(HepMC::children);
        w2Son1 = wVertex2->particles_begin(HepMC::children);
        w2SonE = wVertex2->particles_end(HepMC::children);

        for (w1SonItr = w1Son1; w1SonItr != w1SonE; ++w1SonItr){
          if (std::abs((*w1SonItr)->pdg_id()) == 15) isTau1 = 1;
          if (std::abs((*w1SonItr)->pdg_id()) == 11 || std::abs((*w1SonItr)->pdg_id()) == 13){
            if ((*w1SonItr)->momentum().perp() > m_PtMinChild && fabs((*w1SonItr)->momentum().eta()) < m_EtaRangeChild) isKinLep1 = 1;
          }
        }

        for (w2SonItr = w2Son1; w2SonItr != w2SonE; ++w2SonItr){
          if (std::abs((*w2SonItr)->pdg_id()) == 15) isTau2 = 1;
          if (std::abs((*w2SonItr)->pdg_id()) == 11 || std::abs((*w2SonItr)->pdg_id()) == 13){
            if ((*w2SonItr)->momentum().perp() > m_PtMinChild && fabs((*w2SonItr)->momentum().eta()) < m_EtaRangeChild) isKinLep2 = 1;
          }
        }

        if (isKinLep1 && isKinLep2 && isOK1 && isOK2 && !isTau1 && !isTau2 && std::abs(quark1->pdg_id()) != std::abs(quark2->pdg_id())){
          return StatusCode::SUCCESS;
        }

      }
    }
  }

  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
