/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// TRT_TrackTimingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetCosmicsEventPhase/TRT_TrackTimingTool.h"

#include "InDetIdentifier/TRT_ID.h"


#include "TRT_ConditionsData/RtRelation.h"
#include "TRT_ConditionsData/BasicRtRelation.h"


using CLHEP::GeV;

//================ Constructor =================================================

InDet::TRT_TrackTimingTool::TRT_TrackTimingTool(const std::string& t,
                                                const std::string& n,
                                                const IInterface* p) :
  AthAlgTool(t, n, p), m_ITrackFitter("Trk::GlobalChi2Fitter/InDetTrackFitter"), m_eventPhaseTool(), m_caldbtool(
    "TRT_CalDbTool", this), m_doEtaCorrection(true), m_debug(false) {
  declareInterface<ITRT_TrackTimingTool>(this);
  declareInterface<Trk::ITrackTimingTool>(this);

  // retrieve properties from job options
  declareProperty("FitterTool", m_ITrackFitter);
  declareProperty("EventPhaseTool", m_eventPhaseTool);
  declareProperty("TRTCalDbTool", m_caldbtool);
  declareProperty("DoEtaCorrection", m_doEtaCorrection);
  declareProperty("DebugMissingMeasurement", m_debug);
  declareProperty("EventInfoKey", m_EventInfoKey = "ByteStreamEventInfo");
}

//================ Destructor ==================================================

InDet::TRT_TrackTimingTool::~TRT_TrackTimingTool() {}


//================ Initialisation ==============================================

StatusCode InDet::TRT_TrackTimingTool::initialize() {
  StatusCode sc = AlgTool::initialize();

  if (sc.isFailure()) return sc;

  // retrieve fitter
  if (m_ITrackFitter.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_ITrackFitter << endmsg;
    return StatusCode::FAILURE;
  } else msg(MSG::INFO) << "Retrieved tool " << m_ITrackFitter << endmsg;

  ATH_CHECK(m_EventInfoKey.initialize());


  ATH_MSG_INFO("InDet::TRT_TrackTimingTool::initialize() successful in " << name() << ", do eta correction: " <<
               (m_doEtaCorrection ? "true" : "false"));
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::TRT_TrackTimingTool::finalize() {
  StatusCode sc = AlgTool::finalize();

  return sc;
}

std::vector<Trk::SpaceTimePoint*> InDet::TRT_TrackTimingTool::timeMeasurements(const Trk::Track& track,
                                                                               const Trk::SegmentCollection* sgmColl)
const {
  if (sgmColl != nullptr) {
    ATH_MSG_INFO("TRT_TrackTimingTool::timeMeasurements called with SegmentCollection != 0");
  }

  std::vector<Trk::SpaceTimePoint*> timeMeasurementsVector;
  // need to use constructor: SpaceTimePoint(const GlobalPosition& position, const float& t, const float& t_error, const
  // float& weight);

  // Useless test; compiler can assume it's false and optimize it away.
  // Tests for null must be done on pointers, not references
  // (which by definition cannot be null).
  //if (&track==0) return timeMeasurementsVector; // this function should not have been called in the first place

  float time = 0.;
  Amg::Vector3D position(0., 0., 0.);

  // info about track
  int nTRTdriftCircles(0), nMissingRDOs(0), nMissingTrackParameters(0), nUsedHits(0);
  trackInformation(track, position, nTRTdriftCircles, nMissingRDOs, nMissingTrackParameters, nUsedHits);

  ATH_MSG_DEBUG(
    "trackInformation: " << nTRTdriftCircles << ", " << nMissingRDOs << ", " << nMissingTrackParameters << " slimmed? " <<
    (track.info().trackProperties(Trk::TrackInfo::SlimmedTrack)));

  if (!nTRTdriftCircles) return timeMeasurementsVector; // no TRT hits for this track, can not do anything

  Trk::Track* newtrack = nullptr;
  if (nMissingTrackParameters) { // need to re-fit the track to get the track parameters
    newtrack = (m_ITrackFitter->fit(Gaudi::Hive::currentContext(),track, 
                                    false, track.info().particleHypothesis())).release();

    if (newtrack == nullptr) {
      ATH_MSG_INFO("fit to unslim track has failed, giving up.");
      return timeMeasurementsVector;
    }
  }

  const Trk::Track* track_ptr = (newtrack != nullptr) ? newtrack : &track;

  // there are TRT hits, and track parameters, use the tool to calculate the time
  ATH_MSG_DEBUG("timeMeasurements(): diagnosed that track has all information already.");

  if (!nMissingRDOs) {
    time = m_eventPhaseTool->findPhase(track_ptr);
  } else {
    time = getTrackTimeFromDriftRadius(track_ptr, nUsedHits);
  }


  if (m_doEtaCorrection && time != 0.) time -= etaCorrection(*track_ptr);



// assign estimated constant conservative error for now
  float error = 1.; // units ns

// use default weight of 1.; 0. if the time measurement is invalid
  float weight = (time == 0.) ? 0. : 0.02 * nUsedHits;

  ATH_MSG_DEBUG("TRT_TrackTimingTool::timeMeasurements: TRT track with " << nTRTdriftCircles << " hits, track time = "
                                                                         << time << " ns, position: " << position << ", error: " << error << ", weight: " << weight <<
    ".");

  if (time != 0.) { // add measurement only if valid
    Trk::SpaceTimePoint* timeMeasurement = new Trk::SpaceTimePoint(position, time, error, weight, Trk::TrackState::TRT);

    timeMeasurementsVector.push_back(timeMeasurement);
  } else {
    ATH_MSG_INFO("TRT_TrackTimingTool::timeMeasurement not available");
    if (m_debug) debugMissingMeasurements(track);
  }


  if (newtrack) delete newtrack;

  return timeMeasurementsVector;
}

//===============================================================================

// The same functions but for TrackParticles
std::vector<Trk::SpaceTimePoint*> InDet::TRT_TrackTimingTool::timeMeasurements(const Trk::TrackParticleBase& tpb) const
{
  if (tpb.originalTrack() == nullptr) {
    std::vector<Trk::SpaceTimePoint*> v;
    return v;
  }
  return timeMeasurements(*tpb.originalTrack());
}

// a mock implementation for the segments, must be implemented for gaudi factories to work => returns empty vector
std::vector<Trk::SpaceTimePoint*> InDet::TRT_TrackTimingTool::timeMeasurements(std::vector<const Trk::Segment*> /*segments*/)
const {
  std::vector<Trk::SpaceTimePoint*> v;
  return v;
}

//===============================================================================

float InDet::TRT_TrackTimingTool::getTrackTimeFromDriftRadius(const Trk::Track* track, int& nHits) const {
  float time = 0.;

  nHits = 0;

  for (const Trk::TrackStateOnSurface* state : *track->trackStateOnSurfaces()) {
    Trk::MeasurementBase const* mesb = state->measurementOnTrack();
    if (!mesb || !state->type(Trk::TrackStateOnSurface::Measurement)) continue;

    InDet::TRT_DriftCircleOnTrack const* trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const*>(mesb);
    if (!trtcirc) continue;

    Trk::TrackParameters const* tparp = state->trackParameters();
    if (!tparp) continue;

    float driftR = trtcirc->localParameters()[Trk::driftRadius];
    if (std::abs(driftR) < 0.000001) continue; // driftR == 0 => measurement not available, could be anything
    if (std::abs(driftR) > 2.) continue;

    float trackR = tparp->parameters()[Trk::driftRadius];

    Identifier id = trtcirc->identify();
    const TRTCond::RtRelation* rtRelation = m_caldbtool->getRtRelation(id);
    if (not rtRelation) {
      ATH_MSG_WARNING("Rt relation pointer is null!");
      continue;
    }
    float driftT = rtRelation->drifttime(std::abs(driftR));
    float trackT = rtRelation->drifttime(std::abs(trackR));


    time += driftT - trackT;
    nHits++;
  }

  if (nHits > 0) time /= (float) nHits;

  return time;
}

//===============================================================================

float InDet::TRT_TrackTimingTool::etaCorrection(const Trk::Track& track) const {
  const float correctionLargeEta = -0.31; // ns

  const Trk::Perigee* trackPar = track.perigeeParameters();

  if (!trackPar) {
    ATH_MSG_WARNING("missing perigee parameters, can not make eta correction");
    return 0.;
  }

  double eta = trackPar->eta();
  float correction = (1.3 * eta * eta - 1.9) * eta * eta;
  if (std::abs(eta) > 1. && correction > correctionLargeEta) correction = correctionLargeEta;

  ATH_MSG_DEBUG("eta correction: " << correction << " ns at eta = " << eta);

  return correction;
}

//===============================================================================

void InDet::TRT_TrackTimingTool::trackInformation(const Trk::Track& track, Amg::Vector3D& position,
                                                  int& nTRTdriftCircles, int& nMissingRDOs,
                                                  int& nMissingTrackParameters, int& nUsedHits) const {
  // define position as center-of-gravity for all TRT hits on track
  Amg::Vector3D GlobalPositionSum(0., 0., 0.);

  for (const Trk::TrackStateOnSurface* state : *track.trackStateOnSurfaces()) {
    Trk::MeasurementBase const* mesb = state->measurementOnTrack();
    if (!mesb || !state->type(Trk::TrackStateOnSurface::Measurement)) continue;

    InDet::TRT_DriftCircleOnTrack const* trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const*>(mesb);
    if (!trtcirc) continue;

    nTRTdriftCircles++;

    GlobalPositionSum += trtcirc->globalPosition();

    if (!state->trackParameters()) nMissingTrackParameters++;

    InDet::TRT_DriftCircle const* rawhit = trtcirc->prepRawData();
    if (!rawhit) {
      nMissingRDOs++;
      continue;
    }
    if (!rawhit->driftTimeValid() || rawhit->firstBinHigh()) continue;
    nUsedHits++; // this is the N of hits that will potentially be used for the track time calculation

//		if ( !(state->trackParameters()) ) continue;
  }

  if (nTRTdriftCircles) GlobalPositionSum /= (double) nTRTdriftCircles;
  position = GlobalPositionSum;

  return;
}

//===============================================================================

void InDet::TRT_TrackTimingTool::debugMissingMeasurements(const Trk::Track& track) const {
  int checkNTRThits(0), missingMeasurement(0), noTRT_DriftCircle(0), missingPrepRawData(0), invalidHits(0),
  missingTrackPar(0);

  for (Trk::TrackStateOnSurface const* state : *track.trackStateOnSurfaces()) {
    Trk::MeasurementBase const* mesb = state->measurementOnTrack();
    if (!mesb || !state->type(Trk::TrackStateOnSurface::Measurement)) {
      missingMeasurement++;
      continue;
    }

    InDet::TRT_DriftCircleOnTrack const* trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const*>(mesb);
    if (!trtcirc) {
      noTRT_DriftCircle++;
      continue;
    }

    InDet::TRT_DriftCircle const* rawhit = trtcirc->prepRawData();
    if (!rawhit) {
      missingPrepRawData++;
      continue;
    }
    if (!rawhit->driftTimeValid() || rawhit->firstBinHigh()) {
      invalidHits++;
      continue;
    }
    Trk::TrackParameters const* tparp = state->trackParameters();
    if (!tparp) {
      missingTrackPar++;
      continue;
    }

    checkNTRThits++;

    ATH_MSG_INFO(
      "debugMissingMeasurements: TRT hit " << checkNTRThits << ", raw drift time: " << rawhit->rawDriftTime() << " ns, track R: " <<
        tparp->parameters()[Trk::driftRadius] << " mm.");
  }

  ATH_MSG_INFO("debugMissingMeasurements: N valid hits: " << checkNTRThits << ", missing MeasurementBase / Measurement: " << missingMeasurement
                                                          << ", missing TRT_DriftCircleOnTrack: " << noTRT_DriftCircle << ", missingPrepRawData: " << missingPrepRawData
                                                          << ", invalid LE or TE: " << invalidHits << ", missing track parameters: " <<
    missingTrackPar);

  return;
}
