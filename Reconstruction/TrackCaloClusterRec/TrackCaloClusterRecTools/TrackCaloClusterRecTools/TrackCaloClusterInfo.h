// this file is -*- C++ -*-
/*
  Copyright (C) 2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TRACKCALOCLUSTERINFO_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TRACKCALOCLUSTERINFO_H

#include <map>
#include "AsgTools/CLASS_DEF.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"

struct TrackCaloClusterInfo
{

  /// Definition of the 4-momentum type.
  typedef xAOD::IParticle::FourMom_t FourMom_t;
  
  const xAOD::TrackParticleClusterAssociationContainer* assocContainer =  nullptr;
  std::map <const xAOD::TrackParticle*, FourMom_t> trackTotalClusterPt; 
  std::map <const xAOD::IParticle*, FourMom_t> clusterToTracksWeightMap ;

  const xAOD::TrackParticleContainer * allTracks =  nullptr;
  const xAOD::CaloClusterContainer * allClusters =  nullptr;
  const xAOD::Vertex * pv0 =  nullptr;
  
  
};
CLASS_DEF( TrackCaloClusterInfo , 111448884 , 1 )

#endif
