/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCandidateTool
//  AlgTool performing pre-selection on MS tracks, extrapolation and creation
//  of MuonCandidate collection.
//
//////////////////////////////////////////////////////////////////////////////

#include "MuonCandidateTool.h"

#include "TrkTrackSummary/MuonTrackSummary.h"

namespace MuonCombined {

    MuonCandidateTool::MuonCandidateTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonCandidateTool>(this);
    }

    StatusCode MuonCandidateTool::initialize() {
        ATH_CHECK(m_printer.retrieve());
        if (!m_trackBuilder.empty())
            ATH_CHECK(m_trackBuilder.retrieve());
        else
            m_trackBuilder.disable();
        if (!m_trackExtrapolationTool.empty())
            ATH_CHECK(m_trackExtrapolationTool.retrieve());
        else
            m_trackExtrapolationTool.disable();
        ATH_CHECK(m_ambiguityProcessor.retrieve());
        ATH_CHECK(m_trackSummaryTool.retrieve());
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_beamSpotKey.initialize());
        return StatusCode::SUCCESS;
    }

    void MuonCandidateTool::create(const xAOD::TrackParticleContainer& tracks, MuonCandidateCollection& outputCollection,
                                   TrackCollection& outputTracks) {
        create(tracks, outputCollection, outputTracks, Gaudi::Hive::currentContext());
    }
    void MuonCandidateTool::create(const xAOD::TrackParticleContainer& tracks, MuonCandidateCollection& outputCollection,
                                   TrackCollection& outputTracks, const EventContext& ctx) {
        ATH_MSG_DEBUG("Producing MuonCandidates for " << tracks.size());
        unsigned int ntracks = 0;

        SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{m_beamSpotKey, ctx};
        if (!beamSpotHandle.isValid()) {
            ATH_MSG_ERROR("Could not retrieve the BeamSpot data from key " << m_beamSpotKey.objKey());
            return;
        }
        float beamSpotX = beamSpotHandle->beamPos()[Amg::x];
        float beamSpotY = beamSpotHandle->beamPos()[Amg::y];
        float beamSpotZ = beamSpotHandle->beamPos()[Amg::z];

        ATH_MSG_DEBUG("Beamspot position bs_x=" << beamSpotX << ", bs_y=" << beamSpotY << ", bs_z=" << beamSpotZ);

        // Temporary collection for extrapolated tracks and links with correspondent MS tracks
        std::map<const Trk::Track*, std::pair<ElementLink<xAOD::TrackParticleContainer>, Trk::Track*> > trackLinks;
        std::unique_ptr<TrackCollection> extrapTracks(new TrackCollection(SG::VIEW_ELEMENTS));

        std::set<const Trk::Track*> tracksToBeDeleted;

        unsigned int index = 0;
        // Loop over MS tracks
        for (const auto* track : tracks) {
            if (!track->trackLink().isValid() || track->track() == nullptr) {
                ATH_MSG_WARNING("MuonStandalone track particle without Trk::Track");
                continue;
            }
            ElementLink<xAOD::TrackParticleContainer> trackLink(tracks, index++);

            const Trk::Track& msTrack = *track->track();

            ATH_MSG_VERBOSE("Re-Fitting track " << std::endl
                                                << m_printer->print(msTrack) << std::endl
                                                << m_printer->printStations(msTrack));
            Trk::Track* standaloneTrack = nullptr;
            const Trk::Vertex* vertex = nullptr;
            if (m_extrapolationStrategy == 0u) {
                standaloneTrack = m_trackBuilder->standaloneFit(msTrack, ctx, vertex, beamSpotX, beamSpotY, beamSpotZ);
            } else {
                standaloneTrack = m_trackExtrapolationTool->extrapolate(msTrack);
            }
            if (standaloneTrack) {
                // Reject the track if its fit quality is much (much much) worse than that of the non-extrapolated track
                if (standaloneTrack->fitQuality()->doubleNumberDoF() == 0) {
                    tracksToBeDeleted.emplace(standaloneTrack);
                    standaloneTrack = nullptr;
                    ATH_MSG_DEBUG("extrapolated track has no DOF, don't use it");
                } else {
                    double mschi2 = 2.5;  // a default we should hopefully never have to use (taken from CombinedMuonTrackBuilder)
                    if (msTrack.fitQuality()->doubleNumberDoF() > 0)
                        mschi2 = msTrack.fitQuality()->chiSquared() / msTrack.fitQuality()->doubleNumberDoF();
                    // choice of 1000 is slightly arbitrary, the point is that the fit should be really be terrible
                    if (standaloneTrack->fitQuality()->chiSquared() / standaloneTrack->fitQuality()->doubleNumberDoF() > 1000 * mschi2) {
                        tracksToBeDeleted.emplace(standaloneTrack);
                        standaloneTrack = nullptr;
                        ATH_MSG_DEBUG("extrapolated track has a degraded fit, don't use it");
                    }
                }
            }
            if (standaloneTrack) {
                standaloneTrack->info().setParticleHypothesis(Trk::muon);
                standaloneTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidStandAlone);
                ATH_MSG_VERBOSE("Extrapolated track " << std::endl
                                                      << m_printer->print(*standaloneTrack) << std::endl
                                                      << m_printer->printStations(*standaloneTrack));
                ++ntracks;
                if (!standaloneTrack->perigeeParameters())
                    ATH_MSG_WARNING(" Track without perigee " << standaloneTrack);
                else if (!standaloneTrack->perigeeParameters()->covariance())
                    ATH_MSG_WARNING(" Track with perigee without covariance " << standaloneTrack);
                trackLinks[standaloneTrack] = std::make_pair(trackLink, standaloneTrack);
            } else {
                // We can create tracks from EM segments+TGC hits
                // If these are not successfully extrapolated, they are too low quality to be useful
                // So only make candidates from un-extrapolated tracks if they are not EM-only
                bool skipTrack = true;
                const Trk::MuonTrackSummary* msMuonTrackSummary = nullptr;
                // If reading from an ESD, the track will not have a track summary yet
                if (!msTrack.trackSummary()) {
                    std::unique_ptr<Trk::TrackSummary> msTrackSummary = m_trackSummaryTool->summary(msTrack, nullptr);
                    msMuonTrackSummary = msTrackSummary->muonTrackSummary();
                } else
                    msMuonTrackSummary = msTrack.trackSummary()->muonTrackSummary();
                for (const auto& chs : msMuonTrackSummary->chamberHitSummary()) {
                    if (chs.isMdt() && m_idHelperSvc->stationIndex(chs.chamberId()) != Muon::MuonStationIndex::EM) {
                        skipTrack = false;
                        break;
                    }
                }
                if (!skipTrack) {
                    tracksToBeDeleted.emplace(standaloneTrack);
                    standaloneTrack = new Trk::Track(msTrack);
                    trackLinks[standaloneTrack] = std::make_pair(trackLink, nullptr);
                }
            }
            if (standaloneTrack) {
                extrapTracks->push_back(standaloneTrack);
                tracksToBeDeleted.emplace(standaloneTrack);  // insert track for deletion
            }
        }
        ATH_MSG_DEBUG("Finished back-tracking, total number of successfull fits " << ntracks);

        // Resolve ambiguity between extrapolated tracks (where available)
        std::unique_ptr<const TrackCollection> resolvedTracks(m_ambiguityProcessor->process(extrapTracks.get()));

        ATH_MSG_DEBUG("Finished ambiguity solving: " << extrapTracks->size() << " track(s) in -> " << resolvedTracks->size()
                                                     << " track(s) out");

        // Loop over resolved tracks and build MuonCondidate collection
        int nfailed = 0;
        for (const Trk::Track* track : *resolvedTracks) {
            auto tLink = trackLinks.find(track);
            if (tLink == trackLinks.end()) {
                ATH_MSG_WARNING("Unable to find internal link between MS and SA tracks!");
                continue;
            }
            auto tpair = tLink->second;

            if (tpair.second) {
                outputTracks.push_back(tpair.second);
                ElementLink<TrackCollection> saLink(outputTracks, outputTracks.size() - 1);
                outputCollection.push_back(new MuonCandidate(tpair.first, saLink));
                // remove track from set so it is not deleted
                tracksToBeDeleted.erase(tpair.second);
            } else {
                // in this case the extrapolation failed
                outputCollection.push_back(new MuonCandidate(tpair.first, ElementLink<TrackCollection>()));
                nfailed++;
            }
        }

        // note that we made a copy of the failed track above, this copy will always be deleted
        if (extrapTracks->size() != resolvedTracks->size() + tracksToBeDeleted.size() - nfailed)
            ATH_MSG_WARNING(" inconsistent number of tracks: in " << extrapTracks->size() << " resolved " << resolvedTracks->size()
                                                                  << " remaining " << tracksToBeDeleted.size()
                                                                  << "failed tracks to be deleted" << nfailed);

        for (auto& trk : tracksToBeDeleted) delete trk;
    }

}  // namespace MuonCombined
