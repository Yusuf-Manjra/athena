/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthParticleFakerTool.cxx
 * @author Zach Marshall <zmarshal@caltech.edu>
 * @date April, 2011
 * @brief Block filler tool for TruthParticle's.
 * Fakes the filling of branches from "real" reco
 */

#include "TruthParticleFakerTool.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/SimpleVector.h"
#include "AtlasHepMC/MagicNumbers.h"
#include "AthenaKernel/errorcheck.h"

#include "GaudiKernel/SystemOfUnits.h"

namespace D3PD {

using Gaudi::Units::GeV;

TruthParticleFakerTool::TruthParticleFakerTool (const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent)
  : BlockFillerTool<HepMC::GenParticle> (type, name, parent)

{
  // Avoid coverity warnings
  m_do_E = true;
  m_do_p = true;
  m_do_pt = true;
  m_do_m = true;
  m_do_pn = true;
  m_do_chg = true;
  m_do_Et = true;
  book().ignore();

  declareProperty ("PDG_ID" ,       m_filterID    = 11 );
  declareProperty ("WriteE",        m_do_E        = false);
  declareProperty ("WriteP",        m_do_p        = false);
  declareProperty ("WritePt",       m_do_pt       = true);
  declareProperty ("WriteM",        m_do_m        = true);
  declareProperty ("WritePn",       m_do_pn       = false);
  declareProperty ("WriteCharge",   m_do_chg      = true);
  declareProperty ("WriteEt",       m_do_Et       = false);
  declareProperty ("MinPt",         m_minPt       = -1*GeV );
}

StatusCode TruthParticleFakerTool::book()
{
  if (m_do_E)        CHECK( addVariable ("E",     m_E)  );
  if (m_do_p)        CHECK( addVariable ("p",     m_p)  );
  if (m_do_pt)       CHECK( addVariable ("pt",    m_pt)  );
  if (m_do_m)        CHECK( addVariable ("m",     m_m)  );
  if (m_do_pn){
    CHECK( addVariable ("px", m_px) );
    CHECK( addVariable ("py", m_py) );
    CHECK( addVariable ("pz", m_pz) );
  }
  CHECK( addVariable ("eta", m_eta) );
  CHECK( addVariable ("phi", m_phi) );
  CHECK( addVariable ("status",        m_status) );
  CHECK( addVariable ("barcode",       m_barcode) );
  if (m_do_chg)     CHECK( addVariable ("charge", m_charge) );

  return StatusCode::SUCCESS;
}

StatusCode TruthParticleFakerTool::fill (const HepMC::GenParticle& p)
{
  if ( std::abs(p.pdg_id())!=m_filterID || p.momentum().perp()<m_minPt ) return StatusCode(IBlockFillerTool::EMPTY);
  auto endvertex=p.end_vertex();
  bool last = std::abs(p.pdg_id())==15;
  if ( std::abs(p.pdg_id())==15 && p.status()!=1 && endvertex ){
    // Special handling for taus - take the ones that are last in the tau chain
    for (auto pit: *endvertex){
      if (!pit || std::abs(pit->pdg_id())!=15) continue;
      last=false;
      break;
    }
    if (!last) return StatusCode(IBlockFillerTool::EMPTY);
  }

  if ( !last &&
       p.status()%1000 != 1 &&
       !(p.status()%1000 == 2 && p.status()>1000) &&
       !(p.status()==2 && (!endvertex || HepMC::is_simulation_vertex(endvertex)) ) ) {
    return StatusCode(IBlockFillerTool::EMPTY);
  }

  HepMC::FourVector v = p.momentum();
  if (m_do_E)        *m_E     = static_cast<float> (v.e());
#ifdef HEPMC3
  if (m_do_p)        *m_p     = static_cast<float> (v.length());
#else
  if (m_do_p)        *m_p     = static_cast<float> (v.rho());
#endif
  if (m_do_pt)       *m_pt    = static_cast<float> (v.perp());
  if (m_do_m)        *m_m     = static_cast<float> (v.m());
  if (m_do_pn){
                     *m_px    = static_cast<float> (v.px());
                     *m_py    = static_cast<float> (v.py());
                     *m_pz    = static_cast<float> (v.pz());
  }
  *m_eta = static_cast<float> (v.eta());
  *m_phi = static_cast<float> (v.phi());

  *m_status = p.status();
  *m_barcode = HepMC::barcode(p);
  if (m_do_chg)     *m_charge = p.pdg_id()<0?1:-1;

  return StatusCode::SUCCESS;
}

} // namespace D3PD

