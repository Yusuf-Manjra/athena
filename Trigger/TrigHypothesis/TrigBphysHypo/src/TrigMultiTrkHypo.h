/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigMultiTrkHypo.h
 **
 **   Description: multi-track hypothesis algorithm
 **
 **   Author: H. Russell
 **
 **************************************************************************/

#ifndef TRIG_TrigMultiTrkHypo_H
#define TRIG_TrigMultiTrkHypo_H

// standard stuff
#include <string>
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "xAODMuon/MuonContainer.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "DecisionHandling/TrigCompositeUtils.h"

#include "TrigMultiTrkHypoTool.h"
#include "TrigBphysHelperUtilsTool.h"

#include "DecisionHandling/HypoBase.h"

#include "Constants.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"


class TrigMultiTrkHypo: public ::HypoBase  {
  enum { MaxNumberTools = 20 };
  public:
    TrigMultiTrkHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigMultiTrkHypo();

    virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;
   virtual StatusCode  finalize() override;

  protected:
   //The vertexing tool is not reentrant so this algorithm cannot be used reentrantly
   bool isReEntrant() const override { return false; }

  private:

    TrigMultiTrkHypo();
    ToolHandleArray< TrigMultiTrkHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"};

	bool passNTracks(int nObjMin,
                 const std::vector<float> & ptObjMin,
  				 const std::vector<ElementLink<xAOD::TrackParticleContainer> > & inputTrkVec,
                 std::vector<ElementLink<xAOD::TrackParticleContainer> > & outputTrkVec,
                 float mindR) const;

    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleContainerKey{ this,
        "trackCollectionKey",         // property name
        "Tracks",                     // default value of StoreGate key
        "input TrackParticle container name"};

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonContainerKey{ this,
        "MuonCollectionKey",         // property name
        "CBCombinedMuon",                     // default value of StoreGate key
        "input EF Muon container name"};

    SG::WriteHandleKey<xAOD::TrigBphysContainer> m_bphysObjContKey{ this,
        "bphysCollectionKey",         // property name
        "TrigBphysContainer",         // default value of StoreGate key
        "Output TrigBphysContainer name"};

  static bool sortTracks(ElementLink<xAOD::TrackParticleContainer> &l1, ElementLink<xAOD::TrackParticleContainer> &l2)
  {
    return (*l1)->pt() > (*l2)->pt();
  }

  xAOD::TrigBphys* fit(const std::vector<ElementLink<xAOD::TrackParticleContainer>>& tracklist) const;

  ToolHandle<InDet::VertexPointEstimator> m_vertexPointEstimator{this, "VertexPointEstimator", "", "find starting point for the vertex fitter"};
  ToolHandle<Trk::TrkVKalVrtFitter> m_vertexFitter{this, "VertexFitter", "", "VKalVrtFitter tool to fit tracks into the common vertex"};
  std::unique_ptr<Trk::IVKalState> m_vertexFitterState;

  Gaudi::Property<int> m_nTrk { this, "nTrk",2,"Number of tracks in the vertex"};
  Gaudi::Property<int> m_nTrkQ { this, "nTrkCharge",-1,"sum of track charges"}; // if negative - no cut
  Gaudi::Property<std::vector<double>> m_trkMasses {this, "trackMasses", {105.6583745,105.6583745}, "track masses for vertex reco (one per track!)"};
  Gaudi::Property<std::vector<float>> m_ptTrkMin { this, "trkPtThresholds", {3650, 3650} ,"minimum track pTs (one per track, sorted descending!)"};

  Gaudi::Property<std::vector<float>> m_nTrkMassMin { this, "nTrackMassMin", {0},"list of mins for nTrk mass windows"}; // both min+max need to be defined
  Gaudi::Property<std::vector<float>> m_nTrkMassMax { this, "nTrackMassMax", {100000},"list of maxes for nTrk mass windows"};

  Gaudi::Property<float> m_mindR { this, "overlapdR",0.01 ,"Minimum dR between tracks (overlap removal)"};

  Gaudi::Property<int> m_particleType { this, "particleType", 0, "Types of particles to use. 0 = tracks, 1 = EF Muons, 2 = tbd..."};

  Gaudi::Property<std::string> m_trigLevelString { this, "trigLevelString", "", "Trigger Level to set for created TrigBphys objects."};

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

  xAOD::TrigBphys::levelType m_trigLevel;

};

#endif  // TRIG_TrigMultiTrkHypo_H
