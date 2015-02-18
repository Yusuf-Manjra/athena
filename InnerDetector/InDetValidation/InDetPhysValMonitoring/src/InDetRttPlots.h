/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETRTTPLOTS
#define INDETPHYSVALMONITORING_INDETRTTPLOTS
/**
 * @file InDetRttPlots.h
 * @author shaun roe
 * @date 29/3/2014
**/


//std includes
#include <string>

//local includes
#include "TrkValHistUtils/PlotBase.h"
#include "InDetPerfPlot_Pt.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/TruthInfoPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "InDetPerfPlot_res.h"
#include "InDetPerfPlot_pull.h"
#include "InDetPerfPlot_fakes.h"
#include "InDetPerfPlot_Eff.h"

#include "TrkValHistUtils/IDHitPlots.h"
#include "InDetPerfPlot_HitDetailed.h"
#include "InDetPerfPlot_Vertex.h"
#include "InDetPerfPlot_VertexContainer.h"

#include "InDetPerfPlot_TrkInJet.h"

#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"

///class holding all plots for Inner Detector RTT Validation and implementing fill methods
class InDetRttPlots:public PlotBase {
public:
	InDetRttPlots(PlotBase * pParent, const std::string & dirName);
  void SetFillExtraTIDEPlots( bool fillthem ) { m_moreJetPlots = fillthem; }
	///fill for things needing truth and track only
	void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle);
	///fill for things needing track only
	void fill(const xAOD::TrackParticle& particle);
	///fill for things needing truth only
	void fill(const xAOD::TruthParticle& particle);
	///fill for things needing all truth - not just the ones from the reco tracks
	void fillTruth(const xAOD::TruthParticle& particle);
	///fill reco-vertex related plots
	void fill(const xAOD::VertexContainer& vertexContainer);
	///fill reco-vertex related plots that need EventInfo 
	void fill(const xAOD::VertexContainer& vertexContainer, const xAOD::EventInfo& ei);

  ///fill track-jet related plots
  bool PassJetCuts( const xAOD::Jet& jet );
  void fillJetPlot(const xAOD::TrackParticle& particle, const xAOD::Jet& jet);
  void fillJetPlotCounter(const xAOD::Jet& jet);
  void fillJetTrkTruth(const xAOD::TruthParticle& truth, const xAOD::Jet& jet);
  void fillJetTrkTruthCounter(const xAOD::Jet& jet);

	virtual ~InDetRttPlots(){/**nop**/};
	///fill for Counters
	void fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory  counter);
	///fill for fakes
	void fillFakeRate(const xAOD::TrackParticle& particle, const bool match, const InDetPerfPlot_fakes::Category c = InDetPerfPlot_fakes::ALL);

	
private:
	InDetPerfPlot_Pt  m_ptPlot;
	Trk::ParamPlots m_PtEtaPlots;
	Trk::ImpactPlots m_IPPlots;
	Trk::RecoInfoPlots m_TrackRecoInfoPlots;
	Trk::TruthInfoPlots m_TrackTruthInfoPlots;
	InDetPerfPlot_nTracks m_nTracks;
	InDetPerfPlot_res m_resPlots;
	InDetPerfPlot_pull m_pullPlots;
	InDetPerfPlot_fakes m_fakePlots; //fakes vs eta etc, as per original RTT code
	Trk::IDHitPlots m_hitsPlots;
	Trk::IDHitPlots m_hitsMatchedTracksPlots;
	Trk::IDHitPlots m_hitsFakeTracksPlots;
	InDetPerfPlot_HitDetailed m_hitsDetailedPlots;
  InDetPerfPlot_Eff m_effPlots;

	InDetPerfPlot_VertexContainer m_verticesPlots;
	InDetPerfPlot_Vertex m_vertexPlots;
	InDetPerfPlot_Vertex m_hardScatterVertexPlots;	

  bool m_moreJetPlots;
	InDetPerfPlot_TrkInJet  m_trkInJetPlot;
	InDetPerfPlot_TrkInJet  m_trkInJetPlot_highPt;
	InDetPerfPlot_Pt        m_trkInJetPtPlot;
	Trk::ParamPlots         m_trkInJetPtEtaPlots;
	Trk::ImpactPlots        m_trkInJetIPPlots;
	Trk::RecoInfoPlots      m_trkInJetTrackRecoInfoPlots;
	Trk::IDHitPlots         m_trkInJetHitsPlots;
	InDetPerfPlot_HitDetailed m_trkInJetHitsDetailedPlots;
	InDetPerfPlot_fakes     m_trkInJetFakePlots; //fakes vs eta etc, as per original RTT code
	InDetPerfPlot_res       m_trkInJetResPlots;
	InDetPerfPlot_pull      m_trkInJetPullPlots;
	InDetPerfPlot_res*      m_trkInJetResPlotsDr0010;
	InDetPerfPlot_res*      m_trkInJetResPlotsDr1020;
	InDetPerfPlot_res*      m_trkInJetResPlotsDr2030;
	InDetPerfPlot_res       m_trkInJetHighPtResPlots;
	InDetPerfPlot_pull      m_trkInJetHighPtPullPlots;
	Trk::IDHitPlots         m_trkInJetHitsFakeTracksPlots;
	Trk::IDHitPlots         m_trkInJetHitsMatchedTracksPlots;
  InDetPerfPlot_Eff       m_trkInJetEffPlots;
	Trk::TruthInfoPlots     m_trkInJetTrackTruthInfoPlots;

	std::string m_trackParticleTruthProbKey;
	float m_truthProbThreshold;
	float m_truthProbLowThreshold;
	
};




#endif
