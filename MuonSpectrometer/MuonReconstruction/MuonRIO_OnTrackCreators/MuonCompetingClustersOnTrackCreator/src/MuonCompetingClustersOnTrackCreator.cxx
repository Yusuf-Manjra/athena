/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class MuonCompetingClustersOnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for MuonCompetingClustersOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 18/07/2004
///////////////////////////////////////////////////////////////////

#include "MuonCompetingClustersOnTrackCreator.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include <list>
#include <vector>


namespace Muon {

  MuonCompetingClustersOnTrackCreator::MuonCompetingClustersOnTrackCreator
  (const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa)
  {
    // algtool interface - necessary!
    declareInterface<IMuonCompetingClustersOnTrackCreator>(this);
  }

  StatusCode MuonCompetingClustersOnTrackCreator::initialize()
  {

    ATH_MSG_VERBOSE("MuonCompetingClustersOnTrackCreator::Initializing");
    ATH_CHECK( m_clusterCreator.retrieve() );

    return StatusCode::SUCCESS;
  }
  
  std::unique_ptr<const CompetingMuonClustersOnTrack>
  MuonCompetingClustersOnTrackCreator::createBroadCluster(const std::list< const Trk::PrepRawData * > & prds, const double errorScaleFactor ) const
  {
    if (prds.empty()) return nullptr;

// implement cluster formation
    auto rios = std::vector <const Muon::MuonClusterOnTrack* >() ;
    auto assocProbs = std::vector < double >();
    std::list< const Trk::PrepRawData* >::const_iterator  it = prds.begin();
    std::list< const Trk::PrepRawData* >::const_iterator  it_end = prds.end();
    const double prob = 1./(errorScaleFactor*errorScaleFactor);
    for( ;it!=it_end;++it ){
      Identifier id = (*it)->identify();
      const Trk::TrkDetElementBase* detEl = (*it)->detectorElement();
      const Amg::Vector3D gHitPos = detEl->center(id);
      const Muon::MuonClusterOnTrack* cluster = m_clusterCreator->createRIO_OnTrack( **it, gHitPos ); 
      rios.push_back( cluster );
      assocProbs.push_back( prob );
    }
    return std::make_unique<const CompetingMuonClustersOnTrack>( std::move(rios), std::move(assocProbs) );
  }
}
