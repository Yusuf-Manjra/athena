/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/Photon.h"

#include<cmath>

// ==================================================================

bool xAOD::EgammaHelpers::isConvertedPhoton(const xAOD::Photon *ph){
  return (ph && (ph->nVertices()>0) );
}

xAOD::EgammaParameters::ConversionType xAOD::EgammaHelpers::conversionType(const xAOD::Photon *ph){
  if (!ph) return xAOD::EgammaParameters::NumberOfVertexConversionTypes;
  return conversionType(ph->vertex());
}

xAOD::EgammaParameters::ConversionType xAOD::EgammaHelpers::conversionType(const xAOD::Vertex *vx){
  if (!vx) return xAOD::EgammaParameters::unconverted;

  const TrackParticle *trk1 = ( vx->nTrackParticles() ? vx->trackParticle(0) : 0 );
  const TrackParticle *trk2 = ( vx->nTrackParticles() > 1 ? vx->trackParticle(1) : 0 );
  uint8_t nSiHits1 = numberOfSiHits(trk1);
  uint8_t nSiHits2 = numberOfSiHits(trk2);

  if (!trk1) return xAOD::EgammaParameters::unconverted;

  if (!trk2)
    return nSiHits1 ? xAOD::EgammaParameters::singleSi : xAOD::EgammaParameters::singleTRT;
  
  if (nSiHits1 && nSiHits2)
    return xAOD::EgammaParameters::doubleSi;
  else if (nSiHits1 || nSiHits2)
    return xAOD::EgammaParameters::doubleSiTRT;  
  else
    return xAOD::EgammaParameters::doubleTRT;
}

// ==================================================================

std::size_t xAOD::EgammaHelpers::numberOfSiTracks(const xAOD::Photon *ph){
  if (!ph || !ph->vertex()) return 0;
  return numberOfSiTracks(ph->vertex());
}

std::size_t xAOD::EgammaHelpers::numberOfSiTracks(const xAOD::Vertex *vx)
{
  if (!vx) return 0;
  return numberOfSiTracks(conversionType(vx));
}

std::size_t xAOD::EgammaHelpers::numberOfSiTracks(const xAOD::EgammaParameters::ConversionType convType)
{
  if (convType == xAOD::EgammaParameters::doubleSi) return 2;
  if (convType == xAOD::EgammaParameters::singleSi ||
      convType == xAOD::EgammaParameters::doubleSiTRT) return 1;
  return 0;
}

// ==================================================================

float xAOD::EgammaHelpers::conversionRadius(const xAOD::Vertex* vx)
{
  if (!vx) return 9999.;
  return sqrt( vx->x()*vx->x() + vx->y()*vx->y() );
}

float xAOD::EgammaHelpers::conversionRadius(const xAOD::Photon* ph)
{
  if (!ph || !ph->vertex()) return 9999.;
  return conversionRadius(ph->vertex());
}

// ==================================================================

Amg::Vector3D xAOD::EgammaHelpers::momentumAtVertex(const xAOD::Photon *photon, bool debug)
{
  if (!photon || !photon->vertex()) return Amg::Vector3D(0., 0., 0.);
  return momentumAtVertex(*photon->vertex(), debug);
}
  

Amg::Vector3D xAOD::EgammaHelpers::momentumAtVertex(const xAOD::Vertex& vertex, bool debug)
{  
  if (isAvailable(vertex, "px") && 
      isAvailable(vertex, "py") && 
      isAvailable(vertex, "pz") )
  {
    return Amg::Vector3D( vertex.auxdata<float>("px"),
                          vertex.auxdata<float>("py"),
                          vertex.auxdata<float>("pz") );
  }
  else if (debug)
    std::cout << "Vertex not decorated with momentum" << std::endl;
  
  return Amg::Vector3D(0., 0., 0.);
}

// ==================================================================
const std::set<const xAOD::TrackParticle*> xAOD::EgammaHelpers::getTrackParticles(const xAOD::Photon* ph,
  bool useBremAssoc /* = true */)
{
  std::set<const xAOD::TrackParticle*> tps;
  if (!ph) return tps;
  const xAOD::Vertex* vx = ph->vertex();
  for (unsigned int i=0; vx && i < vx->nTrackParticles(); ++i)
  {
    const xAOD::TrackParticle *tp = vx->trackParticle(i);
    tps.insert( useBremAssoc ? xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(tp) : tp );
  }
  return tps;
}



bool xAOD::EgammaHelpers::isAvailable(const xAOD::Vertex& vertex, std::string name)
{
  SG::AuxElement::Accessor<float> acc(name, "");
  return acc.isAvailable(vertex); 
}
