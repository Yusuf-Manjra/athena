/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONTRACKBUILDER_H
#define MUON_IMUONTRACKBUILDER_H

#include <memory>
#include <vector>

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonTrackBuilder("Muon::IMuonTrackBuilder", 1, 0);

namespace Trk {
    class Track;
    class MeasurementBase;
}  // namespace Trk

namespace Muon {

    class MuPatCandidateBase;
    class MuPatSegment;
    class MuPatTrack;
    class MuPatHit;

    /** @brief The IMuonTrackBuilder is a pure virtual interface for tools extending muon track candidates with
        segments in a given chamber

        The following interface is available.
        @code
           std::vector<MuPatTrack*>* find( MuPatCandidateBase& candidate, const std::vector<MuPatSegment*>& segments );
        @endcode

    */
    class IMuonTrackBuilder : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID();

        /** @brief interface for tools to find track in the muon system starting from a vector of segments
            @param segments a vector of input segments in a given chamber layer
            @return a pointer to a vector of MuPatTrack objects, zero if no tracks are found.
                    The ownership of the tracks is passed to the client calling the tool.

        */
        virtual std::vector<std::unique_ptr<MuPatTrack> > find(
            MuPatCandidateBase& candidate, const std::vector<MuPatSegment*>& segments,
            std::vector<std::unique_ptr<MuPatHit> >& hitsToBeDeleted,
            std::vector<std::unique_ptr<const Trk::MeasurementBase> >& measurementsToBeDeleted) const = 0;
    };

    inline const InterfaceID& IMuonTrackBuilder::interfaceID() { return IID_IMuonTrackBuilder; }
}  // namespace Muon

#endif  // IMuonTrackBuilder_H
