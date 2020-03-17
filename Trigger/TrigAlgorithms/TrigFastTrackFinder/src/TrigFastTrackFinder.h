/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: TrigFastTrackFinder.h
// 
// Description: a part of L2+EF HLT ID tracking
// 
// date: 16/04/2013
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIGFASTTRACKFINDER_TRIGFASTTRACKFINDER_H
#define TRIGFASTTRACKFINDER_TRIGFASTTRACKFINDER_H

#include<string>
#include<vector>
#include<map>
//#include<algorithm>

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"

#include "AthenaMonitoringKernel/Monitored.h"

class ITrigL2LayerNumberTool;
class ITrigL2LayerSetPredictorTool;
class ITrigSpacePointConversionTool;
class ITrigL2ResidualCalculator;
class ITrigInDetTrackFitter;
class ITrigZFinder;
class IRegSelSvc;
class TrigRoiDescriptor;
class TrigSiSpacePointBase;
class Identifier;
namespace InDet { 
  class SiSpacePointsSeed;
  class ISiTrackMaker; 
  class SiTrackMakerEventData_xk;
}

namespace Trk {
  class ITrackSummaryTool;
  class SpacePoint;
}

class TrigL2LayerSetLUT;
class TrigSpacePointStorage;
class TrigInDetTriplet;
class PixelID;
class SCT_ID;
class AtlasDetectorID;

class TrigFastTrackFinder : public HLT::FexAlgo {

 public:
  
  TrigFastTrackFinder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigFastTrackFinder();
  virtual HLT::ErrorCode hltInitialize() override;
  virtual HLT::ErrorCode hltFinalize() override;
  virtual HLT::ErrorCode hltStart() override;

  virtual StatusCode execute() override;
  virtual
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
			    HLT::TriggerElement* outputTE) override;

  StatusCode findTracks(InDet::SiTrackMakerEventData_xk &event_data,
                        const TrigRoiDescriptor& roi,
                        TrackCollection& outputTracks) const;

  double trackQuality(const Trk::Track* Tr) const;
  void filterSharedTracks(std::vector<std::tuple<bool, double, Trk::Track*>>& QT) const;

  virtual bool isClonable() const override { return true; }
  virtual unsigned int cardinality() const override { return 0; }//Mark as re-entrant

protected: 

  void updateClusterMap(long int, const Trk::Track*, std::map<Identifier, std::vector<long int> >&) const;
  void extractClusterIds(const Trk::SpacePoint*, std::vector<Identifier>&) const;
  bool usedByAnyTrack(const std::vector<Identifier>&, std::map<Identifier, std::vector<long int> >&) const;

 private:

  // AlgTools and Services

  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;
  ToolHandle<ITrigSpacePointConversionTool> m_spacePointTool;
  ToolHandle<ITrigL2ResidualCalculator> m_trigL2ResidualCalculator;
  ToolHandle<InDet::ISiTrackMaker> m_trackMaker;   // Track maker 
  ToolHandle<ITrigInDetTrackFitter> m_trigInDetTrackFitter;
  ToolHandle<ITrigZFinder> m_trigZFinder;
  ToolHandle< Trk::ITrackSummaryTool > m_trackSummaryTool;
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

  //DataHandles
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
  SG::WriteHandleKey<TrackCollection> m_outputTracksKey;

  SG::ReadHandleKey<Trk::PRDtoTrackMap>       m_prdToTrackMap
     {this,"PRDtoTrackMap",""};
 
  // Control flags

  bool m_doCloneRemoval;
  bool m_useBeamSpot; 

  bool m_vertexSeededMode;

  bool m_doZFinder;
  bool m_doZFinderOnly;

  bool m_storeZFinderVertices;

  bool m_doFastZVseeding;

  bool m_doResMonitoring;

  // Cuts and settings
  TrigCombinatorialSettings m_tcs;

  int  m_minHits;

  int                     m_nSeeds;       //!< Number seeds 
  int                     m_nfreeCut;     // Min number free clusters 


  float m_tripletMinPtFrac;
  float m_pTmin;
  float m_initialD0Max;
  float m_Z0Max;                        
  bool m_checkSeedRedundancy;

  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

  // Monitoring member functions 

  void fillMon(const TrackCollection& tracks, const TrigVertexCollection& vertices, const TrigRoiDescriptor& roi) const;
  void runResidualMonitoring(const Trk::Track& track) const;

  //Setup functions
  void getBeamSpot(float&, float&) const;
  HLT::ErrorCode getRoI(const HLT::TriggerElement* inputTE, const IRoiDescriptor*& roi);

  // Internal bookkeeping

  std::string m_instanceName, m_attachedFeatureName, m_outputCollectionSuffix;

  ATLAS_THREAD_SAFE mutable unsigned int m_countTotalRoI;
  ATLAS_THREAD_SAFE mutable unsigned int m_countRoIwithEnoughHits;
  ATLAS_THREAD_SAFE mutable unsigned int m_countRoIwithTracks;

  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  const AtlasDetectorID* m_idHelper;


  Trk::ParticleHypothesis m_particleHypothesis;//particle hypothesis to attach to each track - usually pion, can be set to other values

  bool m_useNewLayerNumberScheme;

};

#endif // not TRIGFASTTRACKFINDER_TRIGFASTTRACKFINDER_H
