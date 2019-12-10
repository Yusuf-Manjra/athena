/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAmbiguitySolver/TrkAmbiguitySolver.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityScoreProcessorTool.h"

Trk::TrkAmbiguitySolver::TrkAmbiguitySolver(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm (name, pSvcLocator),
  m_scoredTracksKey(""),
  m_resolvedTracksKey("Tracks"),
  m_ambiTool("Trk::SimpleAmbiguityProcessorTool/TrkAmbiguityProcessor", this),
  m_applySolve(true),
  m_trackInCount(0),
  m_trackOutCount(0)
{
  declareProperty("TrackInput"        , m_scoredTracksKey);
  declareProperty("TrackOutput"       , m_resolvedTracksKey);
  declareProperty("AmbiguityProcessor", m_ambiTool);
  declareProperty("ResolveTracks"     , m_applySolve);
}

//--------------------------------------------------------------------------
Trk::TrkAmbiguitySolver::~TrkAmbiguitySolver(void)
{}

//-----------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguitySolver::initialize()
{
  ATH_MSG_INFO( "TrkAmbiguitySolver::initialize(). " );

  if (m_applySolve) {
    ATH_CHECK(m_ambiTool.retrieve());
    ATH_MSG_INFO( "Retrieved tool " << m_ambiTool );
  } else {
    ATH_MSG_INFO( "ATTENTION: Resolving tracks turned off! " );
    m_ambiTool.disable();
  }

  ATH_CHECK(m_scoredTracksKey.initialize());
  ATH_CHECK(m_resolvedTracksKey.initialize());
  m_trackInCount=0;
  m_trackOutCount=0;
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguitySolver::execute()
{
  ATH_MSG_VERBOSE ("TrkAmbiguitySolver::execute()");
  SG::ReadHandle<TracksScores> scoredTracksHandle(m_scoredTracksKey);
  if ( !scoredTracksHandle.isValid() )  ATH_MSG_ERROR("Could not read scoredTracks.");
  m_trackInCount += scoredTracksHandle->size();
  std::unique_ptr<TrackCollection> resolvedTracks;
  if (m_applySolve){
     resolvedTracks.reset(m_ambiTool->process(scoredTracksHandle.cptr())); //note: take ownership and delete
  }
  else{
    resolvedTracks = std::make_unique<TrackCollection>(SG::VIEW_ELEMENTS);
    resolvedTracks->reserve(scoredTracksHandle->size());
    for(const std::pair< const Trk::Track *, float> &e: *scoredTracksHandle) {
       resolvedTracks->push_back(const_cast<Trk::Track *>(e.first));
    }
  }
  m_trackOutCount += resolvedTracks->size();
  SG::WriteHandle<TrackCollection> resolvedTracksHandle(m_resolvedTracksKey);
  ATH_CHECK(resolvedTracksHandle.record(std::move(resolvedTracks)));
  ATH_MSG_VERBOSE ("Saved "<<resolvedTracksHandle->size()<<" tracks");
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode
Trk::TrkAmbiguitySolver::finalize()
{
  if (m_ambiTool.isEnabled()) {
      m_ambiTool->statistics();
  }
  ATH_MSG_INFO( "Finalizing with "<< m_trackInCount << " tracks input, and "<< m_trackOutCount<< " output");
  return StatusCode::SUCCESS;
}
