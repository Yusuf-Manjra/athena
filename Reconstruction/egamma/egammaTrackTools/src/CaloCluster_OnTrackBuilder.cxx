/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCluster_OnTrackBuilder.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "CaloDetDescr/CaloDepthTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloUtils/CaloLayerCalculator.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"

#include "TrkCaloCluster_OnTrack/CaloCluster_OnTrack.h"

#include "StoreGate/ReadHandle.h"

CaloCluster_OnTrackBuilder::CaloCluster_OnTrackBuilder(const std::string& t,
                                                       const std::string& n,
                                                       const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_emid(nullptr)
{
  declareInterface<ICaloCluster_OnTrackBuilder>(this);
}

CaloCluster_OnTrackBuilder::~CaloCluster_OnTrackBuilder() {}

StatusCode
CaloCluster_OnTrackBuilder::initialize()
{

  ATH_MSG_INFO("Initializing CaloCluster_OnTrackBuilder");

  // Retrieve the updator CaloSurfaceBuilder
  if (m_calosurf.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve the instance " << m_calosurf.name()
                                                     << "... Exiting!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode
CaloCluster_OnTrackBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

Trk::CaloCluster_OnTrack*
CaloCluster_OnTrackBuilder::buildClusterOnTrack(
  const xAOD::CaloCluster* cluster,
  int charge) const
{

  ATH_MSG_DEBUG("Building Trk::CaloCluster_OnTrack");

  if (!m_useClusterPhi && !m_useClusterEta && !m_useClusterEnergy) {
    ATH_MSG_WARNING("CaloCluster_OnTrackBuilder is configured incorrectly");
    return nullptr;
  }

  if (!cluster)
    return nullptr;

  const Trk::Surface* surface = getCaloSurface(cluster);

  if (!surface)
    return nullptr;

  const Trk::LocalParameters* lp =
    getClusterLocalParameters(cluster, surface, charge);
  if (!lp) {
    delete surface;
    return nullptr;
  }

  const Amg::MatrixX* em = getClusterErrorMatrix(cluster, surface, charge);
  if (!em) {
    delete surface;
    delete lp;
    return nullptr;
  }

  Trk::CaloCluster_OnTrack* ccot =
    new Trk::CaloCluster_OnTrack(*lp, *em, *surface);
  delete surface;
  delete lp;
  delete em;

  if (ccot) {
    ATH_MSG_DEBUG("Successful build of Trk::CaloCluster_OnTrack");
  }

  return ccot;
}

const Trk::Surface*
CaloCluster_OnTrackBuilder::getCaloSurface(
  const xAOD::CaloCluster* cluster) const
{

  const Trk::Surface* destinationSurface = nullptr;

  // Determine if we want to extrapolate to the barrel or endcap.  If in the
  // crack choose the detector with largest amount of energy in the second
  // sampling layer
  if (xAOD::EgammaHelpers::isBarrel(cluster)) {
    destinationSurface =
      m_calosurf->CreateUserSurface(CaloCell_ID::EMB2, 0., cluster->eta());
  } else {
    destinationSurface =
      m_calosurf->CreateUserSurface(CaloCell_ID::EME2, 0., cluster->eta());
  }
  return destinationSurface;
}

const Trk::LocalParameters*
CaloCluster_OnTrackBuilder::getClusterLocalParameters(
  const xAOD::CaloCluster* cluster,
  const Trk::Surface* surf,
  int charge) const
{

  Amg::Vector3D surfRefPoint = surf->globalReferencePoint();

  double eta = cluster->eta();
  double theta = 2 * atan(exp(-eta)); //  -log(tan(theta/2));
  double tantheta = tan(theta);
  double phi = cluster->phi();

  double clusterQoverE =
    cluster->calE() != 0 ? (double)charge / cluster->calE() : 0;

  Trk::LocalParameters* newLocalParameters(nullptr);

  if (xAOD::EgammaHelpers::isBarrel(cluster)) {
    // Two corindate in a cyclinder are
    // Trk::locRPhi = 0 (ie phi)
    // Trk::locZ    = 1(ie z)
    double r = surfRefPoint.perp();
    double z = tantheta == 0 ? 0. : r / tantheta;
    Trk::DefinedParameter locRPhi(r * phi, Trk::locRPhi);
    Trk::DefinedParameter locZ(z, Trk::locZ);
    Trk::DefinedParameter qOverP(clusterQoverE, Trk::qOverP);
    std::vector<Trk::DefinedParameter> defPar;
    if (m_useClusterPhi)
      defPar.push_back(locRPhi);
    if (m_useClusterEta)
      defPar.push_back(locZ);
    if (m_useClusterEnergy)
      defPar.push_back(qOverP);
    newLocalParameters = new Trk::LocalParameters(defPar);
  } else {
    // Local paramters of a disk are
    // Trk::locR   = 0
    // Trk::locPhi = 1
    double z = surfRefPoint.z();
    double r = z * tantheta;
    Trk::DefinedParameter locR(r, Trk::locR);
    Trk::DefinedParameter locPhi(phi, Trk::locPhi);
    Trk::DefinedParameter qOverP(clusterQoverE, Trk::qOverP);
    std::vector<Trk::DefinedParameter> defPar;
    if (m_useClusterEta)
      defPar.push_back(locR);
    if (m_useClusterPhi)
      defPar.push_back(locPhi);
    if (m_useClusterEnergy)
      defPar.push_back(qOverP);

    newLocalParameters = new Trk::LocalParameters(defPar);
  }

  return newLocalParameters;
}

const Amg::MatrixX*
CaloCluster_OnTrackBuilder::getClusterErrorMatrix(
  const xAOD::CaloCluster* cluster,
  const Trk::Surface* surf,
  int) const
{

  double phierr = 0.1;
  phierr = phierr < 1.e-10 ? 0.1 : pow(phierr, 2);
  if (!m_useClusterPhi)
    phierr = 10;

  double etaerr =
    10; // 10mm large error as currently we dont want to use this measurement
  etaerr = etaerr < 1.e-10 ? 10. : pow(etaerr, 2);

  double energyerr = pow(0.10 * sqrt(cluster->calE() * 1e-3) * 1000, -4);
  Amg::MatrixX covMatrix;

  if (xAOD::EgammaHelpers::isBarrel(cluster)) {
    // Two corindate in a cyclinder are
    // Trk::locRPhi = 0 (ie phi)
    // Trk::locZ    = 1(ie z)
    Amg::Vector3D surfRefPoint = surf->globalReferencePoint();
    double r2 = pow(surfRefPoint[0], 2);

    int indexCount(0);

    if (m_useClusterPhi) {
      ++indexCount;
      covMatrix(indexCount, indexCount) = phierr * r2;
    }
    if (m_useClusterEta) {
      ++indexCount;
      covMatrix(indexCount, indexCount) = etaerr;
    }
    if (m_useClusterEnergy) {
      ++indexCount;
      covMatrix(indexCount, indexCount) = energyerr;
    }
  } else {
    // Local paramters of a disk are
    // Trk::locR   = 0
    // Trk::locPhi = 1

    int indexCount(0);

    if (m_useClusterEta) {
      ++indexCount;
      covMatrix(indexCount, indexCount) = etaerr;
    }
    if (m_useClusterPhi) {
      ++indexCount;
      covMatrix(indexCount, indexCount) = phierr;
    }
    if (m_useClusterEnergy) {
      ++indexCount;
      covMatrix(indexCount, indexCount) = energyerr;
    }
  }

  const Amg::MatrixX* result = new Amg::MatrixX(covMatrix);

  return result;
}

