/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICombinedMuonTrackBuilder
//  interface to build and fit a combined muon from input track(s)
//  and/or MeasurementSet, gathering material effects along the
//  track (in particular for the calorimeter).
//
//  (c) ATLAS Combined Muon software
///////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_ICOMBINEDMUONTRACKBUILDER_H
#define MUIDINTERFACES_ICOMBINEDMUONTRACKBUILDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk
{
    class Track;
    class Vertex;
}
 
namespace Rec
{
  
/** Interface ID for ICombinedMuonTrackBuilder*/  
static const InterfaceID IID_ICombinedMuonTrackBuilder("ICombinedMuonTrackBuilder", 1, 0);
  
/**@class ICombinedMuonTrackBuilder

Base class for CombinedMuonTrackBuilder AlgTool
     
@author Alan.Poppleton@cern.ch
*/
    class ICombinedMuonTrackBuilder : virtual public IAlgTool,
				      virtual public Trk::ITrackFitter
{
public:

    /**Virtual destructor*/
    virtual ~ICombinedMuonTrackBuilder(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ICombinedMuonTrackBuilder; }

    /**ICombinedMuonTrackBuilder interface: build and fit combined ID/Calo/MS track */
    virtual Trk::Track* 	combinedFit	(const Trk::Track&	indetTrack,
						 const Trk::Track&	extrapolatedTrack,
						 const Trk::Track&	spectrometerTrack) const = 0;

    /**ICombinedMuonTrackBuilder interface:
       build and fit indet track extended to include MS Measurement set.
       Adds material effects as appropriate plus calo energy-loss treatment */
    virtual Trk::Track* 	indetExtension	(const Trk::Track&	     indetTrack,
						 const Trk::MeasurementSet&  spectrometerMeasurements,
						 const Trk::TrackParameters* innerParameters = 0,
						 const Trk::TrackParameters* middleParameters = 0,
						 const Trk::TrackParameters* outerParameters=0) const=0;

    /**ICombinedMuonTrackBuilder interface:
       propagate to perigee adding calo energy-loss and material to MS track */
    virtual Trk::Track*		standaloneFit	(const Trk::Track&	spectrometerTrack,
						 const Trk::Vertex*	vertex = 0, 
                                                 float bs_x = 0., float bs_y = 0., float bs_z = 0.) const = 0;

    /**ICombinedMuonTrackBuilder interface:
       refit a track removing any indet measurements with optional addition of pseudoMeasurements */
    virtual Trk::Track*		standaloneRefit	(const Trk::Track&	combinedTrack, 
                                                 float bs_x = 0., float bs_y = 0., float bs_z = 0.) const = 0;


     /*
      * Default implement the ITrackFitter context aware method to just call
      * the no-context ones
      */
      using Trk::ITrackFitter::fit;
      virtual std::unique_ptr<Trk::Track> fit(
        const EventContext&,
        const Trk::Track& track,
        const Trk::RunOutlierRemoval runOutlier = false,
        const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const
      {
        return std::unique_ptr<Trk::Track>(fit(track, runOutlier, matEffects));
      }
      virtual std::unique_ptr<Trk::Track> fit(
        const EventContext&,
        const Trk::Track& track,
        const Trk::PrepRawDataSet& prepRawSet,
        const Trk::RunOutlierRemoval runOutlier = false,
        const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const
      {
        return std::unique_ptr<Trk::Track>(fit(track, prepRawSet, runOutlier, matEffects));
      }
      virtual std::unique_ptr<Trk::Track> fit(
        const EventContext&,
        const Trk::PrepRawDataSet& prepRawSet,
        const Trk::TrackParameters& params,
        const Trk::RunOutlierRemoval runOutlier = false,
        const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const
      {
        return std::unique_ptr<Trk::Track>(fit(prepRawSet, params, runOutlier, matEffects));
      }
      virtual std::unique_ptr<Trk::Track> fit(
        const EventContext&,
        const Trk::Track& track,
        const Trk::MeasurementSet& measSet,
        const Trk::RunOutlierRemoval runOutlier = false,
        const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const
      {
         return std::unique_ptr<Trk::Track>(fit(track, measSet, runOutlier, matEffects));
      }
      virtual std::unique_ptr<Trk::Track> fit(
        const EventContext&,
        const Trk::MeasurementSet& measSet,
        const Trk::TrackParameters& params,
        const Trk::RunOutlierRemoval runOutlier = false,
        const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const
      {
        return std::unique_ptr<Trk::Track>(fit(measSet, params, runOutlier, matEffects));
      }
      virtual std::unique_ptr<Trk::Track> fit(
        const EventContext&,
        const Trk::Track& track1,
        const Trk::Track& track2,
        const Trk::RunOutlierRemoval runOutlier = false,
        const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const
      {

        return std::unique_ptr<Trk::Track>(fit(track1, track2, runOutlier, matEffects));
      }
    //End of default implementing of context aware methods 


};
 
}	// end of namespace

#endif // MUIDINTERFACES_ICOMBINEDMUONTRACKBUILDER_H


