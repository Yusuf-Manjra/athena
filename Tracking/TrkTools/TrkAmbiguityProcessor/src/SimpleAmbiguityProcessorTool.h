/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMPLEAMBIGUITYPROCESSORTOOL_H
#define SIMPLEAMBIGUITYPROCESSORTOOL_H

// turn on debugging ? uncomment this
//#define SIMPLEAMBIGPROCDEBUGCODE

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"

#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"

//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackSeedMap.h"

#include <map>
#include <set>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>

#include "TrackPtr.h"

#if defined SIMPLEAMBIGPROCNTUPLECODE || defined SIMPLEAMBIGPROCDEBUGCODE
#define DEBUG_CODE(a) do { a; } while (false)
#else
#define DEBUG_CODE(a) do {  } while (false)
#endif

class AtlasDetectorID;
class PixelID;

namespace Trk {

  class ITrackScoringTool;
  class IPRD_AssociationTool;
  class ITruthToTrack;

  class SimpleAmbiguityProcessorTool : public AthAlgTool, virtual public ITrackAmbiguityProcessorTool 
    {
    public:

      // public types
      typedef std::multimap< TrackScore, TrackPtr > TrackScoreMap;
    
      typedef std::set<const PrepRawData*> PrdSignature;
      typedef std::set<PrdSignature> PrdSignatureSet;

      // default methods
      SimpleAmbiguityProcessorTool(const std::string&,const std::string&,const IInterface*);
      virtual ~SimpleAmbiguityProcessorTool ();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize  () override;

      /**Returns a processed TrackCollection from the passed 'tracks'
	 @param tracks collection of tracks which will have ambiguities resolved. Will not be 
	 modified.
	 The tracks will be refitted if no fitQuality is given at input.
         @param prd_to_track_map on optional prd-to-track map being filled by the processor.
	 @return new collections of tracks, with ambiguities resolved. Ownership is passed on 
	 (i.e. client handles deletion).

         If no prd-to-track map is given the processor will create
         one internally (exported to storegate).*/
      virtual TrackCollection*  process(const TrackCollection*, Trk::PRDtoTrackMap *prd_to_track_map) const override;
      virtual TrackCollection*  process(const TracksScores* scoredTracks) const override;

      /** statistics output to be called by algorithm during finalize. */
      virtual void statistics() override;
    private:

      TrackCollection*  process_vector(std::vector<const Track*> &tracks, Trk::PRDtoTrackMap *prd_to_track_map) const;

      /**Add passed TrackCollection, and Trk::PrepRawData from tracks to caches
	 @param tracks the TrackCollection is looped over, 
	 and each Trk::Track is added to the various caches. 
	 The Trk::PrepRawData from each Trk::Track are added to the IPRD_AssociationTool*/
      void addNewTracks(const std::vector<const Track*> &tracks,
                        TrackScoreMap& trackScoreTrackMap,
                        Trk::PRDtoTrackMap &prd_to_track_map) const;

      void addTrack(Track* track, const bool fitted,
                    TrackScoreMap &trackScoreMap,
                    Trk::PRDtoTrackMap &prd_to_track_map,
                    std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks) const;

      TrackCollection *solveTracks(TrackScoreMap &trackScoreTrackMap,
                                   Trk::PRDtoTrackMap &prd_to_track_map,
                                   std::vector<std::unique_ptr<const Trk::Track> > &cleanup_tracks) const;

      /** add subtrack to map */
      void addSubTrack( const std::vector<const TrackStateOnSurface*>& tsos) const;

      /** refit track */
      void refitTrack( const Trk::Track* track,
                       TrackScoreMap &trackScoreMap,
                       Trk::PRDtoTrackMap &prd_to_track_map,
                       std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks) const;

      /** refit PRDs */
      Track* refitPrds( const Track* track, Trk::PRDtoTrackMap &prd_to_track_map) const;

      /** refit ROTs corresponding to PRDs */
      Track* refitRots( const Track* track) const;

      /** print out tracks and their scores for debugging*/
      void dumpTracks(const TrackCollection& tracks) const;

      /** dump the accumulated statistics */
      void dumpStat(MsgStream &out) const;

      // private data members

      /** brem recovery mode with brem fit ? */
      bool  m_tryBremFit;
      bool  m_caloSeededBrem;
      float m_pTminBrem;

      /** by default drop double tracks before refit*/
      bool m_dropDouble;

      /** by default tracks at input get refitted */
      bool m_forceRefit;

      /** by default refit tracks from PRD */
      bool m_refitPrds;

      /** suppress Hole Search */ 
      bool m_suppressHoleSearch;

      /** suppress Track Fit */ 
      bool m_suppressTrackFit;

      /** control material effects (0=non-interacting, 1=pion, 2=electron, 3=muon, 4=pion) read in as an integer 
	  read in as an integer and convert to particle hypothesis */
      int m_matEffects;
      Trk::ParticleHypothesis m_particleHypothesis;
   
      /**Scoring tool
	 This tool is used to 'score' the tracks, i.e. to quantify what a good track is.
	 @todo The actual tool that is used should be configured through job options*/
      ToolHandle<ITrackScoringTool> m_scoringTool;


      /** refitting tool - used to refit tracks once shared hits are removed. 
	  Refitting tool used is configured via jobOptions.*/
      ToolHandle<ITrackFitter> m_fitterTool;

      ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool
         {this, "AssociationTool", "Trk::PRDtoTrackMapTool" };

      ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool
        {this, "TrackSummaryTool", "InDetTrackSummaryToolNoHoleSearch"};

      /** selection tool - here the decision which hits remain on a track and
	  which are removed are made
      */
      ToolHandle<IAmbiTrackSelectionTool> m_selectionTool;

      /** monitoring statistics */
      mutable std::atomic<int> m_Nevents;
      mutable std::mutex m_statMutex;
      mutable std::vector<int> m_Ncandidates, m_NcandScoreZero, m_NcandDouble,
	m_NscoreOk,m_NscoreZeroBremRefit,m_NscoreZeroBremRefitFailed,m_NscoreZeroBremRefitScoreZero,m_NscoreZero,
	m_Naccepted,m_NsubTrack,m_NnoSubTrack,m_NacceptedBrem,
	m_NbremFits,m_Nfits,m_NrecoveryBremFits,m_NgoodFits,m_NfailedFits;
      /** internal monitoring: categories for counting different types of extension results*/
      enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3, iDBM = 4};
      std::vector<float>  m_etabounds;           //!< eta intervals for internal monitoring

      /** helper for monitoring and validation: does success/failure counting */
      void increment_by_eta(std::vector<int>&,const Track*,bool=true) const;

//==================================================================================================
//
//
//   FROM HERE EVERYTHING IS DEBUGGING CODE !!!
//
//==================================================================================================

#if defined SIMPLEAMBIGPROCNTUPLECODE || defined SIMPLEAMBIGPROCDEBUGCODE
       virtual void ntupleReset() {}
       virtual void fillEventData(std::vector<const Track*> &tracks)  { (void) tracks; };
       virtual void findTrueTracks(std::vector<const Track*> &recTracks)  { (void) recTracks; }; 
       virtual void fillValidationTree(const Trk::Track* track) const { (void) track; }
       virtual void resetTrackOutliers() {}
       virtual void setBarcodeStats(const Trk::Track *a_track, TrackScore score) { (void) a_track; (void) score;}
       virtual void fillBadTrack(const Trk::Track *a_track, const Trk::PRDtoTrackMap &prd_to_track_map)
         { (void) a_track; (void) prd_to_track_map; }

       virtual void fillDuplicateTrack(const Trk::Track *a_track) { (void) a_track; }
       virtual void associateToOrig(const Trk::Track*new_track, const Trk::Track* orig_track) { (void) new_track; (void) orig_track; }
       virtual void keepTrackOfTracks(const Trk::Track* oldTrack, const Trk::Track* newTrack) { (void) oldTrack; (void) newTrack; }
       virtual void countTrueTracksInMap(const TrackScoreMap &trackScoreTrackMap) { (void) trackScoreTrackMap; }
       virtual void rejectedTrack(const Trk::Track*track, const Trk::PRDtoTrackMap &prd_to_track_map)
         { (void) track; (void) prd_to_track_map; }

       virtual void fitStatReset() {}
       virtual void keepFittedInputTrack(const Trk::Track *a_track, TrackScore ascore)
         { (void) a_track; (void) ascore; }

       virtual void acceptedTrack(const Trk::Track*track) { (void) track; }
       virtual void memoriseOutliers(const Trk::Track*track) { (void) track; }
       virtual void newCleanedTrack(const Trk::Track*new_track, const Trk::Track* orig_track) { (void) new_track; (void) orig_track; }
       virtual void eventSummary(const TrackCollection *final_tracks) override {(void) final_tracks; }
       virtual void fillFailedFit(const Trk::Track *a_track, const Trk::PRDtoTrackMap &prd_to_track_map)
         { (void) a_track; (void) prd_to_track_map; }

#endif // DebugCode
    };

} //end ns

#endif // TrackAmbiguityProcessorTool_H

