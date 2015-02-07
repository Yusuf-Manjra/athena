// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetTrackAccessor.cxx
// implementations of accessor objects

#include "InDetTrackSelectionTool/InDetTrackAccessor.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#ifndef XAOD_ANALYSIS
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
// includes for top/bottom Si hits (for cosmic selection)
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#endif

// ---------------- TrackAccessor ----------------
InDet::TrackAccessor::TrackAccessor(const asg::IAsgTool* tool)
  : asg::AsgMessaging::AsgMessaging(tool)
  , m_valid(false)
{
}
InDet::TrackAccessor::~TrackAccessor()
{
}

// ---------------- SummaryAccessor ----------------
InDet::SummaryAccessor::SummaryAccessor(const asg::IAsgTool* tool)
  : TrackAccessor::TrackAccessor(tool)
  , m_summaryValue(0)
  , m_summaryType(xAOD::numberOfTrackSummaryTypes)
{
}

void InDet::SummaryAccessor::setSummaryType( xAOD::SummaryType sumType )
{
  m_summaryType = sumType;
  return;
}

StatusCode InDet::SummaryAccessor::access( const xAOD::TrackParticle& track,
					   const xAOD::Vertex* )
{
  m_valid = false;
  if (!track.summaryValue(m_summaryValue, m_summaryType)) {
    ATH_MSG_DEBUG( "Failed to get SummaryType " << std::to_string(m_summaryType) << " from xAOD::TrackParticle summary. A value of zero will be used instead." );
    m_summaryValue = 0;
  }
  m_valid = true;
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::SummaryAccessor::access ( const Trk::Track&,
					    const Trk::TrackParameters*,
					    const Trk::TrackSummary* summary )
{
  m_valid = false;
  if (!summary) {
    ATH_MSG_ERROR( "Recieved null pointer to track summary." );
    m_summaryValue = 0;
    return StatusCode::FAILURE;
  }
  // previously casting the xAOD::SummaryType directly to a Trk::SummaryType.
  // now casting to an int in between to try to remove coverity warning.
  Int_t summaryTypeInt = static_cast<Int_t>(m_summaryType);
  Int_t checkSummaryValue = summary->get( static_cast<Trk::SummaryType>(summaryTypeInt) );
  if (checkSummaryValue < 0) {
    // Trk::TrackSummary::get() will return -1 if the data cannot be retrieved
    ATH_MSG_DEBUG( "Recieved " << checkSummaryValue << " for SummaryType " << std::to_string(m_summaryType) << " from Trk::TrackSummary. A value of zero will be used instead." );
    checkSummaryValue = 0;
  }
  m_summaryValue = checkSummaryValue;
  m_valid = true;
  return StatusCode::SUCCESS;
}
#endif

// ---------------- ParamAccessor ----------------
InDet::ParamAccessor::ParamAccessor(const asg::IAsgTool* tool)
  : InDet::TrackAccessor(tool)
  , m_index(-1)
  , m_paramValue(0)
{
}

void InDet::ParamAccessor::setIndex( Int_t index )
{
  if (index < 0 || index > 4) {
    ATH_MSG_ERROR( "Invalid index in ParamAccessor." );
    return;
  }
  m_index = index;
  return;
}

StatusCode InDet::ParamAccessor::access( const xAOD::TrackParticle& track,
					 const xAOD::Vertex* vertex )
{
  m_valid = false;
  m_paramValue = track.definingParameters()[m_index];
  if (m_index == 1 && vertex) {
    // if this is a z-accessor and a vertex is provided we need to cut w.r.t. the vertex
    m_paramValue += track.vz() - vertex->z();
  }
  m_valid = true;
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::ParamAccessor::access( const Trk::Track&,
					 const Trk::TrackParameters* perigee,
					 const Trk::TrackSummary* )
{
  m_valid = false;
  if (!perigee) {
    ATH_MSG_ERROR( "Zero pointer to perigee." );
    m_paramValue = std::nan("");
    return StatusCode::FAILURE;
  }
  m_paramValue = perigee->parameters()[m_index];
  m_valid = true;
  return StatusCode::SUCCESS;
}
#endif

// ---------------- ParamCovAccessor ----------------
InDet::ParamCovAccessor::ParamCovAccessor(const asg::IAsgTool* tool)
  : InDet::TrackAccessor(tool)
  , m_index1(-1)
  , m_index2(-1)
  , m_paramCovValue(0)
{
}

void InDet::ParamCovAccessor::setIndices(Int_t index1, Int_t index2)
{
  if (index1 < 0 || index1 > 4) {
    ATH_MSG_ERROR( "Invalid index." );
    return;
  }
  m_index1 = index1;
  if (index2 < 0 || index2 > 4) {
    ATH_MSG_ERROR( "Invalid index." );
    return;
  }
  m_index2 = index2;
  return;
}

StatusCode InDet::ParamCovAccessor::access( const xAOD::TrackParticle& track,
					    const xAOD::Vertex* )
{
  m_valid = false;
  m_paramCovValue = track.definingParametersCovMatrix()(m_index1, m_index2);
  m_valid = true;
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::ParamCovAccessor::access( const Trk::Track&,
					    const Trk::TrackParameters* perigee,
					    const Trk::TrackSummary* )
{
  m_valid = false;
  if (!perigee) {
    ATH_MSG_ERROR( "Recieved zero pointer to perigee." );
    m_paramCovValue = std::nan("");
    return StatusCode::FAILURE;
  }
  m_paramCovValue = (*perigee->covariance())(m_index1, m_index2);
  m_valid = true;
  return StatusCode::SUCCESS;
}
#endif

// ---------------- FitQualityAccessor ----------------
InDet::FitQualityAccessor::FitQualityAccessor(const asg::IAsgTool* tool)
  : TrackAccessor(tool)
  , m_chiSq(0)
  , m_nDoF(0)
{
}

StatusCode InDet::FitQualityAccessor::access( const xAOD::TrackParticle& track,
					      const xAOD::Vertex* )
{
  m_valid = false;
  m_chiSq = track.chiSquared();
  m_nDoF = track.numberDoF();
  m_valid = true;
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::FitQualityAccessor::access( const Trk::Track& track,
					      const Trk::TrackParameters*,
					      const Trk::TrackSummary* )
{
  m_valid = false;
  if (!track.fitQuality()) {
    ATH_MSG_WARNING( "Zero pointer to fit quality recieved." );
    m_chiSq = std::nan("");
    m_nDoF = std::nan("");
    return StatusCode::FAILURE;
  }
  m_chiSq = track.fitQuality()->chiSquared();
  m_nDoF = track.fitQuality()->doubleNumberDoF();
  m_valid = true;
  return StatusCode::SUCCESS;
}
#endif


// ---------------- PtAccessor ----------------
InDet::PtAccessor::PtAccessor(const asg::IAsgTool* tool)
  : TrackAccessor(tool)
  , m_pt(0)
{
}

StatusCode InDet::PtAccessor::access( const xAOD::TrackParticle& track,
				      const xAOD::Vertex* )
{
  m_valid = false;
  m_pt = track.pt();
  m_valid = true;
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::PtAccessor::access( const Trk::Track&,
				      const Trk::TrackParameters* perigee,
				      const Trk::TrackSummary* )
{
  m_valid = false;
  if (!perigee) {
    ATH_MSG_WARNING( "Zero pointer to perigee recieved." );
    m_pt = std::nan("");
    return StatusCode::FAILURE;
  }
  m_pt = perigee->momentum().perp();
  m_valid = true;
  return StatusCode::SUCCESS;
}
#endif

// ---------------- EtaAccessor ----------------
InDet::EtaAccessor::EtaAccessor(const asg::IAsgTool* tool)
  : TrackAccessor(tool)
  , m_eta(0)
{
}

StatusCode InDet::EtaAccessor::access( const xAOD::TrackParticle& track,
				       const xAOD::Vertex* )
{
  m_valid = false;
  m_eta = track.eta();
  m_valid = true;
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::EtaAccessor::access( const Trk::Track&,
				       const Trk::TrackParameters* perigee,
				       const Trk::TrackSummary* )
{
  m_valid = false;
  if (!perigee) {
    ATH_MSG_WARNING( "Zero pointer to perigee recieved." );
    m_eta = std::nan("");
    return StatusCode::FAILURE;
  }
  m_eta = perigee->momentum().eta();
  m_valid = true;
  return StatusCode::SUCCESS;
}
#endif


// ---------------- PAccessor ----------------
InDet::PAccessor::PAccessor(const asg::IAsgTool* tool)
  : TrackAccessor(tool)
  , m_p(0)
{
}

StatusCode InDet::PAccessor::access( const xAOD::TrackParticle& track,
				     const xAOD::Vertex* )
{
  m_valid = false;
  if (track.qOverP() == 0) {
    ATH_MSG_WARNING( "Track qOverP is zero." );
    m_p = std::nan("");
    return StatusCode::FAILURE;
  }
  m_p = 1.0/std::fabs(track.qOverP());
  m_valid = true;
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::PAccessor::access( const Trk::Track&,
				     const Trk::TrackParameters* perigee,
				     const Trk::TrackSummary* )
{
  m_valid = false;
  if (!perigee) {
    ATH_MSG_WARNING( "Zero pointer to perigee recieved." );
    m_p = std::nan("");
    return StatusCode::FAILURE;
  }
  m_p = perigee->momentum().mag();
  m_valid = true;
  return StatusCode::SUCCESS;
}
#endif

// ---------------- UsedHitsdEdxAccessor ----------------
InDet::UsedHitsdEdxAccessor::UsedHitsdEdxAccessor(const asg::IAsgTool* tool)
  : TrackAccessor::TrackAccessor(tool)
  , m_n(0)
{
}

StatusCode InDet::UsedHitsdEdxAccessor::access( const xAOD::TrackParticle& track,
						const xAOD::Vertex* )
{
  m_valid = false;
  m_n = track.numberOfUsedHitsdEdx();
  m_valid = true;
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::UsedHitsdEdxAccessor::access ( const Trk::Track&,
						 const Trk::TrackParameters*,
						 const Trk::TrackSummary* summary )
{
  m_valid = false;
  if (!summary) {
    ATH_MSG_ERROR( "Recieved null pointer to track summary." );
    m_n = 0;
    return StatusCode::FAILURE;
  }
  Int_t checkSummaryValue = summary->numberOfUsedHitsdEdx();
  if (checkSummaryValue < 0) {
    // Trk::TrackSummary::numberOfUsedHitsdEdx() will return -1 if the data cannot be retrieved
    ATH_MSG_DEBUG( "Recieved " << checkSummaryValue << " for numberOfUsedHitsdEdx from Trk::TrackSummary. A value of zero will be used instead." );
    checkSummaryValue = 0;
  }
  m_n = checkSummaryValue;
  m_valid = true;
  return StatusCode::SUCCESS;
}
#endif

// ---------------- UsedHitsdEdxAccessor ----------------
InDet::OverflowHitsdEdxAccessor::OverflowHitsdEdxAccessor(const asg::IAsgTool* tool)
  : TrackAccessor::TrackAccessor(tool)
  , m_n(0)
{
}

StatusCode InDet::OverflowHitsdEdxAccessor::access( const xAOD::TrackParticle& track,
						    const xAOD::Vertex* )
{
  m_valid = false;
  m_n = track.numberOfIBLOverflowsdEdx();
  m_valid = true;
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::OverflowHitsdEdxAccessor::access ( const Trk::Track&,
						     const Trk::TrackParameters*,
						     const Trk::TrackSummary* summary )
{
  m_valid = false;
  if (!summary) {
    ATH_MSG_ERROR( "Recieved null pointer to track summary." );
    m_n = 0;
    return StatusCode::FAILURE;
  }
  Int_t checkSummaryValue = summary->numberOfOverflowHitsdEdx();
  if (checkSummaryValue < 0) {
    // Trk::TrackSummary::numberOfOverflowHitsdEdx() will return -1 if the data cannot be retrieved
    ATH_MSG_DEBUG( "Recieved " << checkSummaryValue << " for numberOfOverflowHitsdEdx from Trk::TrackSummary. A value of zero will be used instead." );
    checkSummaryValue = 0;
  }
  m_n = checkSummaryValue;
  m_valid = true;
  return StatusCode::SUCCESS;
}
#endif

// the top/bottom is likely only available in athena
#ifndef XAOD_ANALYSIS
// ---------------- SiHitsTopBottomAccessor ----------------
InDet::SiHitsTopBottomAccessor::SiHitsTopBottomAccessor(const asg::IAsgTool* tool)
  : TrackAccessor(tool)
  , m_top(0)
  , m_bottom(0)
{
}

StatusCode InDet::SiHitsTopBottomAccessor::access( const xAOD::TrackParticle& track,
						   const xAOD::Vertex* )
{
  const Trk::Track* trkTrack = track.track();
  if (!trkTrack) {
    ATH_MSG_WARNING( "xAOD::TrackParticle has a null pointer to a Trk::Track." );
    ATH_MSG_WARNING( "Unable to access top/bottom Si hits." );
    return StatusCode::FAILURE;
  }
  return access(*trkTrack);
}

StatusCode InDet::SiHitsTopBottomAccessor::access( const Trk::Track& track,
						   const Trk::TrackParameters*,
						   const Trk::TrackSummary* )
{
  m_valid = false;
  m_top = 0;
  m_bottom = 0;

  const DataVector<const Trk::MeasurementBase>* trkMeasurements = track.measurementsOnTrack();
  if (!trkMeasurements) {
    ATH_MSG_WARNING( "Null pointer to Trk::MeasurementBase vector." );
    return StatusCode::FAILURE;
  }
  // code in here is mostly adapted from InDetCosmicTrackSelectorTool
  for (const Trk::MeasurementBase* measurement : *trkMeasurements) {
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
    if (!rot)
      continue;
    const InDet::SiClusterOnTrack* siclus
      = dynamic_cast<const InDet::SiClusterOnTrack*>(rot);
    if (!siclus)
      continue;

    float ypos = siclus->globalPosition().y();
    if (ypos == 0)
      continue; // neither top nor bottom

    const InDet::SCT_ClusterOnTrack* sctclus
      = dynamic_cast<const SCT_ClusterOnTrack*>(siclus);
    if (!sctclus) {
      // Pixel hit
      if (ypos > 0)
	m_top+=2;
      else
	m_bottom+=2;
    } else {
      // SCT hit
      if (ypos > 0)
	m_top++;
      else
	m_bottom++;
    }   
  }

  m_valid = true;
  return StatusCode::SUCCESS;
}
#endif
