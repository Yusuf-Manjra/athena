/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @version: $Id: T2TrackClusterer.h 793164 2017-01-20 03:59:26Z ssnyder $
 *
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2TrackClusterer
 *
 * @brief Auxiliary class that clusters tracks in a TrackCollection
 *        in Z around a seed track
 *
 * @author Rainer Bartoldus, SLAC, <bartoldu@slac.stanford.edu>
 * @author David W. Miller, SLAC/Stanford University, <David.W.Miller@cern.ch>
 *
 **********************************************************************************/

#ifndef TRIGT2BEAMSPOT_T2TRACKCLUSTERER_H
#define TRIGT2BEAMSPOT_T2TRACKCLUSTERER_H

/// Externals
#include "AthContainers/ConstDataVector.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "GaudiKernel/SystemOfUnits.h"
//using Gaudi::Units::GeV;
//using Gaudi::Units::mm;

#include <string>
#include <vector>
#include <cmath>

namespace PESA
{

  class T2TrackClusterer
  {
  public:

    // Constructor
    T2TrackClusterer( double deltaZ = 10.*Gaudi::Units::mm, double minPT = 1.*Gaudi::Units::GeV, bool weightedZ = true, unsigned maxSize = 10000. )
      : m_deltaZ    ( deltaZ    )
      , m_minPT     ( minPT     )
      , m_weightedZ ( weightedZ )
      , m_maxSize   ( maxSize   )
      , m_seedZ0    ( 0. )
      , m_totalZ0Err( 0. )
      {}

    // Accessors
    double     seedZ0() const { return m_seedZ0    ; }
    double totalZ0Err() const { return m_totalZ0Err; }

    const TrackCollection&               cluster() const { return *m_cluster.asDataVector();      }
    const TrackCollection&          unusedTracks() const { return *m_unusedTracks.asDataVector(); }

    // Methods
    double trackWeight( const Trk::Track& track ) const;

    const TrackCollection& cluster( const TrackCollection& tracks );

  private:
    // Data members
    const double   m_deltaZ;
    const double   m_minPT;
    const bool     m_weightedZ;
    const unsigned m_maxSize;

    double m_seedZ0;
    double m_totalZ0Err;

    ConstDataVector<TrackCollection> m_cluster;
    ConstDataVector<TrackCollection> m_unusedTracks;
  };

} // end namespace

#endif
