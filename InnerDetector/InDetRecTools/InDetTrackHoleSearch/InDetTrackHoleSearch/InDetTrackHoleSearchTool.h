/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetTrackHoleSearchTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETINDETTRACKHOLESEARCHTOOL_H
#define INDETINDETTRACKHOLESEARCHTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IBoundaryCheckTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include <atomic>
#include <vector>
#include <map>

class AtlasDetectorID;
class Identifier;
class AtlasID;
namespace InDet {class IInDetTestPixelLayerTool; }
class IGeoModelSvc;

namespace Trk { class RIO_OnTrack; class TrackStateOnSurface; class Track; }

namespace InDetDD { class SiDetectorElement; }

namespace InDet 
{

  /** @class InDetTrackHoleSearchTool 

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  Markus Elsing
  */  

  class InDetTrackHoleSearchTool : virtual public Trk::ITrackHoleSearchTool, public AthAlgTool
    {
    public:
      InDetTrackHoleSearchTool(const std::string&,const std::string&,const IInterface*);
      
      /** default destructor */
      virtual ~InDetTrackHoleSearchTool ();
      
      /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
      /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      /** Input : track, partHyp
	  Output: Changes in information
	  This method first calls the method getMapOfHits to isolate the relevant hits on the track before calling the method
	  performHoleSearchStepWise which then performs the actual hole search.
	  Additionally the Layers of the Pixel Detector which contribute measurements to the track are counted  
	  If problems occur, the information counters for Holes and PixelLayers are reset to -1 flagging them as not set.
      */
      virtual void countHoles(const Trk::Track& track, 
			      std::vector<int>& information ,
			      const Trk::ParticleHypothesis partHyp = Trk::pion) const ;
      
      /** Input : track, parthyp
	  Return: A DataVector containing pointers to TrackStateOnSurfaces which each represent an identified hole on the track.
	  The parthyp argument is relevant for the extrapolation steps in the hole search.
	  Attention: This is a factory, ownership of the return vector is passed to the calling method.
      */
      virtual const DataVector<const Trk::TrackStateOnSurface>* getHolesOnTrack(const Trk::Track& track, 
										const Trk::ParticleHypothesis partHyp = Trk::pion) const ;
      
      /** Input : track, parthyp
	  Return: A pointer to a new Trk::Track which containes the information of the input track plus the tsos of the identified holes
	  The parthyp argument is relevant for the extrapolation steps in the hole search.
	  Attention: This is a factory, ownership of the return track is passed to the calling method.
      */
      virtual const Trk::Track*  getTrackWithHoles(const Trk::Track& track, 
						   const Trk::ParticleHypothesis partHyp = Trk::pion) const ;
      
      
      /** Input : track, parthyp
	  Return: A pointer to a new Trk::Track which containes the information of the input track plus the tsos of the identified holes or outliers
	  The parthyp argument is relevant for the extrapolation steps in the hole search.
	  Attention: This is a factory, ownership of the return track is passed to the calling method.
      */
      virtual const Trk::Track* getTrackWithHolesAndOutliers(const Trk::Track& track, 
							     const Trk::ParticleHypothesis partHyp = Trk::pion) const;

    private:
      
      /** Input : track, parthyp
	  Return: Changes in information and/or listOfHoles
	  The interfacing method to the step wise hole search. Information and listOfHoles have to be given as pointers, zeros can be given in order to suppress the 
	  connected functionality (counting holes / producing hole TSOSs). This Method is not a member of the ITrackHoleSearchTool interface.
      */
      void searchForHoles(const Trk::Track& track, 
			  std::vector<int>* information ,
			  std::vector<const Trk::TrackStateOnSurface*>* listOfHoles,
			  const Trk::ParticleHypothesis partHyp = Trk::pion) const;
      
      /**ID pixel helper*/
      const AtlasDetectorID* m_atlasId;
      
      /** Pointer to Extrapolator AlgTool*/
      ToolHandle<Trk::IExtrapolator> m_extrapolator{
	this,
	"Extrapolator",
	"Trk::Extrapolator/InDetExtrapolator",
	"Extrapolator used to extrapolate to layers"
      };
      ToolHandle<Trk::IBoundaryCheckTool> m_boundaryCheckTool {
         this,
         "BoundaryCheckTool",
         "InDet::InDetBoundaryCheckTool",
         "Boundary checking tool for detector sensitivities"
      };

      /** Configure outwards hole search */
      bool m_extendedListOfHoles,m_cosmic;

      /** Min number of hits **/
      int m_minSiHits;

      /* searching for dead modules after the last measurement (needed for robustness of SW)*/
      bool m_countDeadModulesAfterLastHit;

      /** number of warnings printed when no track parameters available **/
      mutable std::atomic_int m_warning;

      /** Input: track
	  Output: changes in mapOfHits (filling it) and hasTRT
 	  Return value: True if filling was successful, false otherwise
	  This method distills the relevant hits out of a given track
      */
      bool getMapOfHits(
        const EventContext& ctx,
        const Trk::Track& track,
        const Trk::ParticleHypothesis partHyp,
        std::map<const Identifier, const Trk::TrackStateOnSurface*>& mapOfHits,
        std::map<const Identifier,std::pair<const Trk::TrackParameters*, const bool>>&
          mapOfPredictions) const;

      /** This method searches for holes in a track. It receives a list of
         detector responses sorted along the track. The TSOS in the list of hits
         should contain TrackParameters or at least MeasurementOnTrack. The
         method extrapolates along the tracks, using the hit information to
         update the extrapolation procedure. Surface without detector responses,
         which are crossed by the extrapolation are counted as holes. The number
         of holes found is returned through the information array. Input:
         mapOfHits,mapOfPredicitons Output: Changes in information,listOfHoles
      */
      void performHoleSearchStepWise(std::map<const Identifier, const Trk::TrackStateOnSurface*>& mapOfHits,
				     std::map<const Identifier, std::pair<const Trk::TrackParameters*,const bool> >& mapOfPredictions,
				     std::vector<int>* information,
				     std::vector<const Trk::TrackStateOnSurface*>* listOfHoles) const;
      
      /** This method creates a TSOS to represent a detected hole. I creates a new TP from the input and returns
	  a (pointer to a) new TSOS containing the TP and the typeset 'Hole'
      */
      static const Trk::TrackStateOnSurface* createHoleTSOS(const Trk::TrackParameters* trackPar) ;

      /** This Method creates a new Track from the TSOS of the input track combined with the TSOS from listOfHoles
       */
      const Trk::Track*  addHolesToTrack(const Trk::Track& oldTrack, 
					 std::vector<const Trk::TrackStateOnSurface*>* listOfHoles) const;
    };

} // end of namespace

#endif 
