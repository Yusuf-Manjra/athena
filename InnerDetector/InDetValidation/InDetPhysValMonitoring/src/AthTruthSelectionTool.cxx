/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthTruthSelectionTool.cxx
 * implementation file for truth selection in this package
 * @author shaun roe
 * @date 10 October 2016
 **/

#include "AthTruthSelectionTool.h"
#include "xAODTruth/TruthVertex.h"

#include <vector>
#include <cmath>

#include "TrkParameters/TrackParameters.h"

namespace {
  constexpr int electronId(11);
  constexpr int gammaId(22);
}


AthTruthSelectionTool::AthTruthSelectionTool(const std::string& type, const std::string& name,
                                             const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_counters{},
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator") 
{
  // declare interface from base class
  declareInterface<IAthSelectionTool>(this);
  // declareProperty( "Property", m_nProperty ); set defaults
  declareProperty("minEta", m_minEta = -1);
  declareProperty("maxEta", m_maxEta = 2.5);
  declareProperty("minPt", m_minPt = 400);
  declareProperty("maxPt", m_maxPt = -1);
  declareProperty("maxD0", m_maxD0 = -1);
  declareProperty("maxZ0", m_maxZ0 = -1);
  declareProperty("maxBarcode", m_maxBarcode = 200e3);
  declareProperty("requireCharged", m_requireCharged = true);
  declareProperty("requireStatus1", m_requireStatus1 = true);
  declareProperty("maxProdVertRadius", m_maxProdVertRadius = 110.);
  declareProperty("pdgId", m_pdgId = -1);
  declareProperty("hasNoGrandparent", m_grandparent = false);
  declareProperty("poselectronfromgamma", m_poselectronfromgamma = false);
  declareProperty("radiusCylinder", m_radiusCylinder=-1, "Select truth particle based on extrapolated position on cylinder placed at this radius. Enabled if greater than 0.");
  declareProperty("minZCylinder", m_minZCylinder=0.0, "Minimum |Z| on cylinder for accepting extrapolated truth particle to surface.");
  declareProperty("maxZCylinder", m_maxZCylinder=0.0, "Maximum |Z| on cylinder for accepting extrapolated truth particle to surface.");
  declareProperty("zDisc", m_zDisc=-1.0, "Select truth particle based on extrapolated position on disks placed at +/- z positions. Enabled if greater than 0.");
  declareProperty("minRadiusDisc", m_minRadiusDisc=0.0, "Minimum radius on disk for accepting extrapolated truth particle to surface.");
  declareProperty("maxRadiusDisc", m_maxRadiusDisc=0.0, "Maximum radius on disk for accepting extrapolated truth particle to surface.");

  //reset cache
  cylinder = 0;
  disc1 = 0;
  disc2 = 0;
}

StatusCode
AthTruthSelectionTool::initialize() {
  // can set cut properties now
  typedef xAOD::TruthParticle P_t;
  typedef Accept<P_t> Accept_t;
  typedef Accept_t::func_type F_t;
  //
  const std::vector<Accept_t> filters = {
    // if p.pt=0, TVector3 generates an error when querying p.eta(); a limit of 1e-7 was not found to be enough to
    // prevent this
    // the following also vetoes the case where the p.pt()=NaN, as any inequality with NaN evaluates to false
    Accept_t([this](const P_t& p) -> bool {
      return((p.pt() > 0.1) ? (std::abs(p.eta()) < m_maxEta) : false);
    }, std::string("eta")),
    Accept_t([this](const P_t& p) -> bool {
      return(p.pt() > m_minPt);
    }, std::string("min_pt")),
    Accept_t([this](const P_t& p) -> bool {
      return((not (p.hasProdVtx()))or(p.prodVtx()->perp() < m_maxProdVertRadius));
    }, "decay_before_" + std::to_string(m_maxProdVertRadius))
  };
  //
  m_cutFlow = CutFlow<P_t>(filters);
  if (m_maxPt > 0) {
    m_cutFlow.add(Accept_t([this](const P_t& p) {
      return(p.pt() < m_maxPt);
    }, "max_pt"));
  }
  if (m_minEta > 0) {
    m_cutFlow.add(Accept_t([this](const P_t& p) {
      return((p.pt() > 0.1) ? (std::abs(p.eta()) > m_minEta) : false);
    }, "min_eta"));
  }
  if (m_maxD0 > 0) {
    m_cutFlow.add(Accept_t([this](const P_t& p) {
      return(p.isAvailable<float>("d0") && std::fabs(p.auxdata<float>("d0")) < m_maxD0);
    }, "max_d0"));
  }
  if (m_maxZ0 > 0) {
    m_cutFlow.add(Accept_t([this](const P_t& p) {
      return(p.isAvailable<float>("z0") && std::fabs(p.auxdata<float>("z0")) < m_maxZ0);
    }, "max_z0"));
  }
  if (m_maxBarcode > -1) {
    m_cutFlow.add(Accept_t([this](const P_t& p) {
      return(p.barcode() < m_maxBarcode);
    }, "barcode < " + std::to_string(m_maxBarcode)));
  }
  if (m_requireCharged) {
    m_cutFlow.add(Accept_t([](const P_t& p) {
      return(not (p.isNeutral()));
    }, "charged"));
  }
  if (m_requireStatus1) {
    m_cutFlow.add(Accept_t([](const P_t& p) {
      return(p.status() == 1);
    }, "status1"));
  }
  if (m_pdgId > 0) {
    m_cutFlow.add(Accept_t([this](const P_t& p) {
      return(std::abs(p.pdgId()) == m_pdgId);
    }, "pdgId"));
  }
  if (m_grandparent) {
    m_cutFlow.add(Accept_t([](const P_t& p) {
      return((p.nParents() == 0) || ((p.nParents() == 1)and((p.parent(0))->nParents() == 0)));
    }, "hasNoGrandparent"));
  }
  if (m_poselectronfromgamma) {
    m_cutFlow.add(Accept_t([](const P_t& p) {
      return((p.absPdgId() == electronId)and(p.nParents() >= 1) and(p.parent(0)) and(p.parent(0)->pdgId() == gammaId));
    }, "poselectronfromgamma"));
  }
  if (m_radiusCylinder > 0) {
    //    m_cutFlow.add(Accept_t(acceptExtrapolatedTPToSurface, "SelectCylinder"));
    m_cutFlow.add(Accept_t([this](const P_t& p) -> bool {
	  ATH_MSG_VERBOSE("Checking particle for intersection with cylinder of radius " << m_radiusCylinder);
	  //create surface we extrapolate to and cache it
	  if (cylinder == 0) {
	    ATH_MSG_VERBOSE("Creating and caching cylinder surface");
	    Amg::Transform3D *trnsf = new Amg::Transform3D();
	    trnsf->setIdentity();
	    cylinder = new Trk::CylinderSurface( trnsf, m_radiusCylinder, 20000.);
	  }
	  const xAOD::TruthVertex* ptruthVertex = p.prodVtx();
	  if (ptruthVertex == 0) {
	    //cannot derive production vertex, reject track
	    ATH_MSG_VERBOSE("Rejecting particle without production vertex.");
	    return false;
	  }
	  const auto xPos = ptruthVertex->x();
	  const auto yPos = ptruthVertex->y();
	  const auto z_truth = ptruthVertex->z();
	  const Amg::Vector3D position(xPos, yPos, z_truth);    
	  const Amg::Vector3D momentum(p.px(), p.py(), p.pz());
	  const Trk::CurvilinearParameters cParameters(position, momentum, p.charge());
	  const Trk::TrackParameters *exParameters = m_extrapolator->extrapolate(cParameters, *cylinder, Trk::anyDirection, false, Trk::pion);
	  if (!exParameters) {
	    ATH_MSG_VERBOSE("Failed extrapolation. Rejecting track.");
	    return false;
	  }
	  ATH_MSG_VERBOSE("Extrapolated parameters to cylinder: " << *exParameters);
	  const float ex_abs_z = fabs(exParameters->position().z());
	  if ( (ex_abs_z > m_minZCylinder) and (ex_abs_z < m_maxZCylinder) ) {
	    ATH_MSG_VERBOSE("Particle accepted.");
	    return true;
	  }
	  //else..
	  ATH_MSG_VERBOSE("Particle rejected");
	  return false;	  
	}, "SelectCylinder"));
  } else if (m_zDisc > 0) {
    //m_cutFlow.add(Accept_t(acceptExtrapolatedTPToSurface, "SelectDisc"));
    m_cutFlow.add(Accept_t([this](const P_t& p) -> bool {	  
	  ATH_MSG_VERBOSE("Checking particle for intersection with discs of |z| " << m_zDisc);
	  //create surface we extrapolate to and cache it
	  if (disc1 == 0) { //disc2 == 0 implied
	    ATH_MSG_VERBOSE("Creating and caching disc surface");
	    Amg::Transform3D *trnsf_shiftZ = new Amg::Transform3D();
	    (*trnsf_shiftZ) = Amg::Translation3D(0.,0.,m_zDisc);
	    disc1 = new Trk::DiscSurface( trnsf_shiftZ, m_minRadiusDisc, m_maxRadiusDisc);
	    (*trnsf_shiftZ) = Amg::Translation3D(0.,0.,-m_zDisc);
	    disc2 = new Trk::DiscSurface( trnsf_shiftZ, m_minRadiusDisc, m_maxRadiusDisc);
	  }
	  const xAOD::TruthVertex* ptruthVertex = p.prodVtx();
	  if (ptruthVertex == 0) {
	    //cannot derive production vertex, reject track
	    ATH_MSG_VERBOSE("Rejecting particle without production vertex.");
	    return false;
	  }
	  const auto xPos = ptruthVertex->x();
	  const auto yPos = ptruthVertex->y();
	  const auto z_truth = ptruthVertex->z();
	  const Amg::Vector3D position(xPos, yPos, z_truth);    
	  const Amg::Vector3D momentum(p.px(), p.py(), p.pz());
	  const Trk::CurvilinearParameters cParameters(position, momentum, p.charge());
	  const Trk::TrackParameters *exParameters = m_extrapolator->extrapolate(cParameters, *disc1, Trk::anyDirection, true, Trk::pion);
	  if (exParameters) {
	    //since boundary check is true, should be enough to say we've hit the disk..
	    ATH_MSG_VERBOSE("Successfully extrapolated track to disk at +" << m_zDisc << ": " << *exParameters);
	    float ex_radius = sqrt(pow(exParameters->position().x(),2)+pow(exParameters->position().y(),2));
	    ATH_MSG_VERBOSE("radial position at surface: " << ex_radius);
	    if ((ex_radius > m_minRadiusDisc) and (ex_radius < m_maxRadiusDisc)) {
	      ATH_MSG_VERBOSE("Confirmed within the disk. Accepting particle");
	      return true;
	    }
	    //else...
	    ATH_MSG_VERBOSE("Strange, extrapolation succeeded but extrapolated position not within disc radius! Test next disc");
	  }
	  exParameters = m_extrapolator->extrapolate(cParameters, *disc2, Trk::anyDirection, true, Trk::pion);
	  if (exParameters) {
	    //since boundary check is true, should be enough to say we've hit the disk..
	    ATH_MSG_VERBOSE("Successfully extrapolated track to disk at -" << m_zDisc << ": " << *exParameters);
	    float ex_radius = sqrt(pow(exParameters->position().x(),2)+pow(exParameters->position().y(),2));
	    ATH_MSG_VERBOSE("radial position at surface: " << ex_radius);
	    if ((ex_radius > m_minRadiusDisc) and (ex_radius < m_maxRadiusDisc)) {
	      ATH_MSG_VERBOSE("Confirmed within the disk. Accepting particle");
	      return true;
	    }
	    //else...
	    ATH_MSG_VERBOSE("Strange, extrapolation succeeded but extrapolated position not within disc radius! Rejecting");
	  }
	  //else..
	  ATH_MSG_VERBOSE("Particle rejected");
	  return false;        
	}, "SelectDisc"));
  } //m_zDisc > 0

  m_counters = std::vector<unsigned int>(m_cutFlow.size(), 0);
  std::string msg = std::to_string(m_cutFlow.size()) + " truth acceptance cuts are used:\n";
  for (const auto& i:m_cutFlow.names()) {
    msg += i + "\n";
  }
  ATH_MSG_INFO(msg);
  clearCounters();

  ATH_CHECK(m_extrapolator.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode
AthTruthSelectionTool::finalize() {
  // nop
  return StatusCode::SUCCESS;
}

void
AthTruthSelectionTool::clearCounters() {
  m_cutFlow.clear();
  m_counters = m_cutFlow.counters();
}

std::vector<unsigned int>
AthTruthSelectionTool::counters() const {
  return m_cutFlow.counters();
}

std::vector<std::string>
AthTruthSelectionTool::names() const {
  return m_cutFlow.names();
}

bool
AthTruthSelectionTool::accept(const xAOD::IParticle* particle) {
  const xAOD::TruthParticle* pTruth = dynamic_cast<const xAOD::TruthParticle*>(particle);

  if (not pTruth) {
    return false;
  }
  return m_cutFlow.accept(*pTruth);
}

std::string
AthTruthSelectionTool::str() const {
  return m_cutFlow.report();
}
