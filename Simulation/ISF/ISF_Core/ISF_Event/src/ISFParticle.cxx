/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFParticle.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "ISF_Event/ISFParticle.h"

ISF::ISFParticle::ISFParticle(
  const Amg::Vector3D& pos,
  const Amg::Vector3D& mom,
  double mass,
  double charge,
  int pdgCode,
  double time,
  const ISFParticle &p,
  Barcode::ParticleBarcode barcode,
  TruthBinding* truth):
 m_position(pos),
 m_momentum(mom),
 m_mass(mass),
 m_charge(charge),
 m_pdgCode(pdgCode),
 m_tstamp(time),
 m_history(p.history()),
 m_barcode(barcode),
 m_extraBarcode(Barcode::fUndefinedBarcode),
 m_truth(truth),
 m_order(ISF::DefaultParticleOrder),
 m_userInfo(nullptr)
{

}

ISF::ISFParticle::ISFParticle(
  const HepGeom::Point3D<double>& pos,
  const HepGeom::Vector3D<double>& mom,
  double mass,
  double charge,
  int pdgCode,
  double time,
  const ISFParticle &p,
  Barcode::ParticleBarcode barcode,
  TruthBinding* truth):
 m_position( pos.x(), pos.y(), pos.z()),
 m_momentum( mom.x(), mom.y(), mom.z()),
 m_mass(mass),
 m_charge(charge),
 m_pdgCode(pdgCode),
 m_tstamp(time),
 m_history(p.history()),
 m_barcode(barcode),
 m_extraBarcode(Barcode::fUndefinedBarcode),
 m_truth(truth),
 m_order(ISF::DefaultParticleOrder),
 m_userInfo(nullptr)
{

}

ISF::ISFParticle::ISFParticle(
  const Amg::Vector3D& pos,
  const Amg::Vector3D& mom,
  double mass,
  double charge,
  int pdgCode,
  double time,
  const DetRegionSvcIDPair &origin,
  Barcode::ParticleBarcode barcode,
  TruthBinding* truth):
 m_position(pos),
 m_momentum(mom),
 m_mass(mass),
 m_charge(charge),
 m_pdgCode(pdgCode),
 m_tstamp(time),
 m_history(1, origin),
 m_barcode(barcode),
 m_extraBarcode(Barcode::fUndefinedBarcode),
 m_truth(truth),
 m_order(ISF::DefaultParticleOrder),
 m_userInfo(nullptr)
{

}

ISF::ISFParticle::ISFParticle(
  const HepGeom::Point3D<double>& pos,
  const HepGeom::Vector3D<double>& mom,
  double mass,
  double charge,
  int pdgCode,
  double time,
  const DetRegionSvcIDPair &origin,
  Barcode::ParticleBarcode barcode,
  TruthBinding* truth):
 m_position( pos.x(), pos.y(), pos.z() ),
 m_momentum( mom.x(), mom.y(), mom.z() ),
 m_mass(mass),
 m_charge(charge),
 m_pdgCode(pdgCode),
 m_tstamp(time),
 m_history(1, origin),
 m_barcode(barcode),
 m_extraBarcode(Barcode::fUndefinedBarcode),
 m_truth(truth),
 m_order(ISF::DefaultParticleOrder),
 m_userInfo(nullptr)
{

}

ISF::ISFParticle::ISFParticle(const ISFParticle& isfp):
  m_position(isfp.position()),
  m_momentum(isfp.momentum()),
  m_mass(isfp.mass()),
  m_charge(isfp.charge()),
  m_pdgCode(isfp.pdgCode()),
  m_tstamp(isfp.timeStamp()),
  m_history(isfp.history()),
  m_barcode(isfp.barcode()),
  m_extraBarcode(Barcode::fUndefinedBarcode),
  m_truth(nullptr),
  m_order(ISF::DefaultParticleOrder),
  m_userInfo(nullptr)
{
  TruthBinding *truth = isfp.getTruthBinding();
  if (truth) {
      m_truth = new TruthBinding(*truth);
  }
}

ISF::ISFParticle::~ISFParticle() {
  delete m_truth;
  delete m_userInfo;
}

ISF::ISFParticle& ISF::ISFParticle::operator=(const ISF::ISFParticle& rhs)
{

  if (this != &rhs) {
    m_position     = rhs.position();
    m_momentum     = rhs.momentum();
    m_mass         = rhs.mass();
    m_charge       = rhs.charge();
    m_pdgCode      = rhs.pdgCode();
    m_tstamp       = rhs.timeStamp();
    m_history      = rhs.history();
    m_barcode      = rhs.barcode();
    m_extraBarcode = rhs.getExtraBC();

    delete m_truth;
    m_truth=nullptr;

    TruthBinding *rhsTruth = rhs.getTruthBinding();
    if (rhsTruth) {
        m_truth = new TruthBinding(*rhsTruth);
    }
  }

  return *this;
}
