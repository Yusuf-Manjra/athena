/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_IMAGINGSEEDFINDER_H
#define TRKVERTEXSEEDFINDERTOOLS_IMAGINGSEEDFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexSeedFinderUtils/VertexImage.h"

//class IBeamCondSvc; //Beam spot constraint from here

namespace Trk
{

  class Vertex;
  class Track;
  class IVertexImageMaker;
  class IVertexClusterFinder;
  class VertexCluster;

  // @author Matt Rudolph (matthew.scott.rudolph@cern.ch) Lars Egholm Pedersen (egholm...)
  // This class implements a multiseed finder using a filtering technique inspired by imaging algorithms
  // As a backup, the single findSeed methods are implemented using a cache of the result from findMultiSeed
  // @ATLAS software
  //

  class ImagingSeedFinder : public AthAlgTool, virtual public IVertexSeedFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    ImagingSeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~ImagingSeedFinder();

    // Interface for Tracks with starting seed/linearization point
    virtual Vertex findSeed(const std::vector<const Trk::Track*> & vectorTrk,const RecVertex * constraint=0);
    
    /** Interface for MeasuredPerigee with starting point */
    virtual Vertex findSeed(const std::vector<const Trk::TrackParameters*> &parametersList,const RecVertex * constraint=0);

    // Interface for finding vector of seeds from tracks
    virtual std::vector<Vertex> findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,const RecVertex * constraint=0);

    // Interface for finding vector of seeds from track parameters
    virtual std::vector<Vertex> findMultiSeeds(const std::vector<const Trk::TrackParameters*>& parametersList,const RecVertex * constraint=0);

  private:

    //Tool that actually makes the image to process
    ToolHandle< Trk::IVertexImageMaker > m_vertexImageMaker;

    //Clustering util
    ToolHandle< Trk::IVertexClusterFinder > m_VertexClusterFinder;
    
    //where the seeds to return are stored
    std::vector<Vertex> m_seeds;
    //cache control variables
    unsigned int m_cacheRunNumber; ///< cached results for given run/event number
    unsigned int m_cacheEventNumber; ///< cached results for given run/event number
    unsigned int m_currentSeedIdx; ///< keep track of what seeds we've given already

  };
}
#endif
