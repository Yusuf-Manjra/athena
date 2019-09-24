/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACKSUMMARYTOOL_H
#define TRKTRACKSUMMARYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParameters/TrackParameters.h" 
#include "TrkTrack/Track.h"
#include "CxxUtils/checker_macros.h"
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"

class AtlasDetectorID;
class Identifier;
class ITRT_ToT_dEdx;

namespace Trk {
  class ITRT_ElectronPidTool;
  class ITrackHoleSearchTool;
  class IPixelToTPIDTool;

  /** @enum flag the methods for the probability calculation */

  enum TRT_ElectronPidProbability {
      Combined          = 0,
      HighThreshold     = 1,
      TimeOverThreshold = 2,
      Bremsstrahlung    = 3
    };


    class IExtendedTrackSummaryHelperTool;
    class RIO_OnTrack;
    class TrackStateOnSurface;
    class MeasurementBase;

    class TrackSummaryTool : public extends<AthAlgTool,IExtendedTrackSummaryTool>
    {

    public:
      TrackSummaryTool(const std::string&,const std::string&,const IInterface*);
      virtual ~TrackSummaryTool ();

      virtual StatusCode initialize() override;
      virtual StatusCode finalize  () override;

      /** create a summary object from passed Track. The summary object belongs to
      you, the user, and so you must take care of deletion of it.
      @param onlyUpdateTrack If false (default) then the summary is cloned and added to the track, 
      and a separate summary returned. If true, only update track and return 0*/
      virtual const Trk::TrackSummary* createSummary ATLAS_NOT_THREAD_SAFE ( const Track& track, bool onlyUpdateTrack=false ) const override;

      /** create a summary object of passed track without doing the tedious hole search.
      Same comments as for createSummary apply here, of course, too. */	   
      virtual const Trk::TrackSummary* createSummaryNoHoleSearch ATLAS_NOT_THREAD_SAFE( const Track& track ) const override;

      /** create a summary object from passed Track. The summary object belongs to
      you, the user, and so you must take care of deletion of it.
      @param onlyUpdateTrack If false (default) then the summary is cloned and added to the track, and a separate summary returned. If true, only update track and return 0*/
      virtual const Trk::TrackSummary* createSummary ATLAS_NOT_THREAD_SAFE ( const Track& track,
                                                                             const Trk::PRDtoTrackMap *prd_to_track_map,
                                                                             bool onlyUpdateTrack=false ) const override;

      /** create a summary object of passed track without doing the tedious hole search.
      Same comments as for createSummary apply here, of course, too. */
      virtual const Trk::TrackSummary* createSummaryNoHoleSearch ATLAS_NOT_THREAD_SAFE( const Track& track,
                                                                                        const Trk::PRDtoTrackMap *prd_to_track_map) const override;

      /** create a summary object from passed Track.*/
      virtual std::unique_ptr<Trk::TrackSummary> summary( const Track& track) const override;

      /** create a summary object of passed track without doing the tedious hole search. */
      virtual std::unique_ptr<Trk::TrackSummary> summaryNoHoleSearch( const Track& track) const override;

      /** use this method to update a track. this means a tracksummary is created for
      this track but not returned. the summary can then be obtained from the track.
      Because it is taken from the track the ownership stays with the track */
      virtual void updateTrack(Track& track, const Trk::PRDtoTrackMap *prd_to_track_map) const override;

      /** method which can be used to update the track and add a summary to it,without doing the tedious hole search.
      This can be used to add a summary to a track and then retrieve it from it without the need to clone. */
      virtual void updateTrackNoHoleSearch(Track& track, const Trk::PRDtoTrackMap *prd_to_track_map) const override;

      /** method to update the shared hit content only, this is optimised for track collection merging. */
      virtual void updateSharedHitCount(Track& track, const Trk::PRDtoTrackMap *prd_to_track_map) const override;

      /** method to update additional information (PID,shared hits, dEdX), this is optimised for track collection merging. */
      virtual void updateAdditionalInfo(Track& track, const Trk::PRDtoTrackMap *prd_to_track_map) const override;

      /** use this method to update a track. this means a tracksummary is created for
      this track but not returned. the summary can then be obtained from the track.
      Because it is taken from the track the ownership stays with the track */
      virtual void updateTrack(Track& track) const override {
        updateTrack(track,nullptr);
      }

      /** method which can be used to update the track and add a summary to it,without doing the tedious hole search.
          This can be used to add a summary to a track and then retrieve it from it without the need to clone. */
      virtual void updateTrackNoHoleSearch(Track& track) const override {
        updateTrackNoHoleSearch(track,nullptr);
      }

      /** method to update the shared hit content only, this is optimised for track collection merging. */
      virtual void updateSharedHitCount(Track& track) const override {
        updateSharedHitCount(track,nullptr);
      }

      /** method to update additional information (PID,shared hits, dEdX), this is optimised for track collection merging. */
      virtual void updateAdditionalInfo(Track& track) const override {
        updateAdditionalInfo(track,nullptr);
      }

    private:

      Trk::TrackSummary *createSummaryAndUpdateTrack ATLAS_NOT_THREAD_SAFE ( const Track& track,
                                                                             const Trk::PRDtoTrackMap *prd_to_track_map,
                                                                             bool onlyUpdateTrack,
                                                                             bool doHolesInDet,
                                                                             bool doHolesMuon) const {
        Trk::TrackSummary* ts= createSummary(track, prd_to_track_map, doHolesInDet, doHolesMuon).release();
        Trk::Track& nonConstTrack = const_cast<Trk::Track&>(track);
        delete nonConstTrack.m_trackSummary;
        nonConstTrack.m_trackSummary = ts;
        return onlyUpdateTrack ? nullptr : new Trk::TrackSummary(*ts);
      }

      std::unique_ptr<Trk::TrackSummary>  createSummary( const Track& track,
                                                         const Trk::PRDtoTrackMap *prd_to_track_map,
                                                         bool doHolesInDet,
                                                         bool doHolesMuon ) const;

      /** Return the correct tool, matching the passed Identifier*/
      Trk::IExtendedTrackSummaryHelperTool*  getTool(const Identifier& id);
      const Trk::IExtendedTrackSummaryHelperTool*  getTool(const Identifier& id) const;
      
  /** controls whether holes on track are produced
      Turning this on will (slightly) increase processing time.*/
      bool m_doHolesMuon;
  /** For the InDet it is switched on automatically as soon as the HoleSearchTool is given */
      bool m_doHolesInDet;
      
  /** controls whether the detailed summary is added for the indet */
      bool m_addInDetDetailedSummary;

  /** controls whether the detailed summary is added for the muons */
      bool m_addMuonDetailedSummary;

  /** controls whether shared hits in Pix+SCT are produced
      Turning this on will increase processing time.*/
      bool m_doSharedHits;
	
  /**atlas id helper*/
      const AtlasDetectorID* m_detID;

  /**tool to decipher ID RoTs*/
      ToolHandle< IExtendedTrackSummaryHelperTool > m_idTool;

  /**tool to calculate electron probabilities*/
  // Troels.Petersen@cern.ch:
      ToolHandle< ITRT_ElectronPidTool > m_eProbabilityTool;
      /** tool to calculate the TRT_ToT_dEdx.
       */
      ToolHandle<ITRT_ToT_dEdx>          m_trt_dEdxTool;


  /**tool to calculate dE/dx using pixel clusters*/
      ToolHandle< IPixelToTPIDTool > m_dedxtool;

  /**tool to decipher muon RoTs*/
      ToolHandle< IExtendedTrackSummaryHelperTool > m_muonTool;

      /** Only compute TRT dE/dx if there are at least this number of TRT hits or outliers.
       */
      int m_minTRThitsForTRTdEdx;

  /** switch to deactivate Pixel info init */
      bool m_pixelExists;

      /** Parameters for the TRT dE/dx compution see @ref ITRT_ToT_dEdx for details.*/
      bool m_TRTdEdx_DivideByL;
      /** Parameters for the TRT dE/dx compution see @ref ITRT_ToT_dEdx for details.*/
      bool m_TRTdEdx_useHThits;
      /** Parameters for the TRT dE/dx compution see @ref ITRT_ToT_dEdx for details.*/
      bool m_TRTdEdx_corrected;

  /**loops over TrackStatesOnSurface and uses this to produce the summary information
      Fills 'information', 'eProbability', and 'hitPattern'*/
      void processTrackStates(const Track& track,
                              const Trk::PRDtoTrackMap *prd_to_track_map,
			      const DataVector<const TrackStateOnSurface>* tsos,
			      std::vector<int>& information,
			      std::bitset<numberOfDetectorTypes>& hitPattern,
                              bool doHolesInDet,
                              bool doHolesMuon) const;

      void processMeasurement(const Track& track,
                              const Trk::PRDtoTrackMap *prd_to_track_map,
			      const Trk::MeasurementBase* meas,
			      const Trk::TrackStateOnSurface* tsos,
			      std::vector<int>& information,
			      std::bitset<numberOfDetectorTypes>& hitPattern) const;

  /** Extrapolation is performed from one hit to the next, it is checked if surfaces in between
      the extrapolation are left out. The trackParameters of the destination hit (instead of the
        trackParameters of the extrapolation step) are then used as starting point for the next
        extrapolation step. */
      void searchHolesStepWise( const Trk::Track& track,
                                std::vector<int>& information,
                                bool doHolesInDet,
                                bool doHolesMuon) const;

    };

}
#pragma GCC diagnostic pop
#endif
