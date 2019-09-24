/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEXTENDEDTRACKSUMMARYHELPERTOOL
#define IEXTENDEDTRACKSUMMARYHELPERTOOL


#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include <bitset>

// @TODO remove once interface without PRDtoTrackMap argument is retired.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"

class Identifier;

namespace Trk {

  class PRDtoTrackMap;

  static const InterfaceID IID_IExtendedTrackSummaryHelperTool("Trk::IExtendedTrackSummaryHelperTool", 1, 0);

/** @class ITrackSummaryHelperTool
  @brief Interface for structuring the summary creation into sub-detector
    specific tools.

    This tool is aimed to be implemented once in the Inner Detector and
    once in the Muon Spectrometer, allowing each implementation to access
    detector-specific information while a master tool can decide at run-time
    which implementation to call, depending on the type of hit on the track.

    @author Edward Moyse, Martin Siebel <http://consult.cern.ch/xwho>
*/

  class IExtendedTrackSummaryHelperTool : virtual public ITrackSummaryHelperTool {
  public:
    static const InterfaceID& interfaceID();

  /** fill 'information' and 'hitpattern' using information from 'rot'. Should be overloaded by concrete
    TrackSummaryHelperTools. It is mandatory that the RIO_OnTrack* points to exactly the object contained
      inside the TrackStateOnSurface. This is to avoid that the RTTI from the TrackSummaryTool is done twice.
        */
      virtual void analyse(const Trk::Track& track,
                           const Trk::PRDtoTrackMap *prd_to_track_map,
                           const RIO_OnTrack* rot,
                           const TrackStateOnSurface* tsos,
                           std::vector<int>& information,
                           std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const = 0;

      virtual void analyse(const Trk::Track& track,
                           const Trk::PRDtoTrackMap *prd_to_track_map,
                           const CompetingRIOsOnTrack* crot,
                           const TrackStateOnSurface* tsos,
                           std::vector<int>& information,
                           std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const = 0;

      virtual void updateSharedHitCount(const Trk::Track&,
                                        const Trk::PRDtoTrackMap *prd_to_track_map,
                                        Trk::TrackSummary&) const  = 0 ;

    virtual void addDetailedTrackSummary(const Track& track, Trk::TrackSummary& summary ) const = 0;



  };
  inline const InterfaceID& Trk::IExtendedTrackSummaryHelperTool::interfaceID()
  {
    return IID_ITrackSummaryHelperTool;
  }


}
#pragma GCC diagnostic pop
#endif
