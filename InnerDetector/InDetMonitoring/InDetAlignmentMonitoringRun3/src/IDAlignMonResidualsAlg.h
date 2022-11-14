/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDAlignMonResiduals.cxx
// AUTHORS: Beate Heinemann, Tobias Golling, Ben Cooper, John Alison, Pierfrancesco Butti
// Adapted to AthenaMT 2021-2022 by Per Johansson
// **********************************************************************

#ifndef IDAlignMonResidualsAlg_H
#define IDAlignMonResidualsAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "StoreGate/ReadHandleKey.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "InDetAlignGenTools/IInDetAlignHitQualSelTool.h"

#include <memory>
#include <string>
#include <vector>

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;

namespace InDetDD{
  class PixelDetectorManager;
  class SCT_DetectorManager;
}

namespace Trk {
  class Track;
  class TrackStateOnSurface;
}

class EventContext;
class ComTime;

class IDAlignMonResidualsAlg :  public AthMonitorAlgorithm {

 public:
  
  IDAlignMonResidualsAlg( const std::string & name, ISvcLocator* pSvcLocator ); 
  virtual ~IDAlignMonResidualsAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:
  void fillTRTHistograms(int barrel_ec, int layer_or_wheel, int phi_module, float predictR, float hitR, float residualR, float pullR, bool isTubeHit, float trketa) const;
  void fillTRTBarrelHistograms(int barrel_ec, int layer_or_wheel, int phi_module, float predictR, float hitR, float residualR, float pullR, bool LRcorrect, bool isTubeHit, float trketa) const;
  void fillTRTEndcapHistograms(int barrel_ec, int phi_module, float predictR, float hitR, float residualR, float pullR, bool LRcorrect, bool isTubeHit, float trketa) const;
  StatusCode setupTools();
	
  StatusCode getSiResiduals(const Trk::Track*, const Trk::TrackStateOnSurface*, bool, double*) const;
  std::unique_ptr <Trk::TrackParameters> getUnbiasedTrackParameters(const Trk::Track*, const Trk::TrackStateOnSurface*) const;

  bool trackRequiresRefit(const Trk::Track*) const;

  float m_maxPtEC{}; // threshold for low-pt EC distributions
  
  //tools
  const AtlasDetectorID*                m_idHelper{};
  const InDetDD::PixelDetectorManager*  m_PIX_Mgr{}; 
  const InDetDD::SCT_DetectorManager*   m_SCT_Mgr{};
  const PixelID*                        m_pixelID{};
  const SCT_ID*                         m_sctID{}; 
  const TRT_ID*                         m_trtID{}; 

  SG::ReadHandleKey<TrackCollection> m_tracksKey {this,"TrackName", "CombinedInDetTracks", "track data key"};
  SG::ReadHandleKey<TrackCollection> m_tracksName {this,"TrackName2","CombinedInDetTracks", "track data key"};

  ToolHandle<ITRT_CalDbTool> m_trtcaldbTool;
  ToolHandle<Trk::IUpdator>             m_iUpdator;
  ToolHandle<Trk::IPropagator>          m_propagator;
  ToolHandle<Trk::IResidualPullCalculator>    m_residualPullCalculator;   //!< The residual and pull calculator tool handle
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelection; // baseline
  ToolHandle<IInDetAlignHitQualSelTool>  m_hitQualityTool;

  std::string m_Pixel_Manager;
  std::string m_SCT_Manager;
  bool  m_extendedPlots;
  bool m_doHitQuality = false;
  int m_checkrate {};
  bool m_doPulls {};
  static const int m_nSiBlayers{4}; //
  static const int m_nPixEClayers{3}; //
  static const int m_nTRTBlayers{3}; //
  static const int m_nTRTEClayers{2}; //
  std::vector<int> m_pixResidualX;
  std::vector<int> m_pixResidualY;
  std::vector<int> m_pixPullX;
  std::vector<int> m_pixPullY;
  std::vector<int> m_pixResidualXvsEta;
  std::vector<int> m_pixResidualYvsEta;
  std::vector<int> m_pixResidualXvsPhi;
  std::vector<int> m_pixResidualYvsPhi;
  std::vector<int> m_pixECAResidualX;
  std::vector<int> m_pixECAResidualY;
  std::vector<int> m_pixECCResidualX;
  std::vector<int> m_pixECCResidualY;
  std::vector<int> m_sctResidualX;
  std::vector<int> m_sctPullX;
  std::vector<int> m_sctResidualXvsEta;
  std::vector<int> m_sctResidualXvsPhi;
  std::vector<int> m_trtBPredictedR;
  std::vector<int> m_trtBMeasuredR;
  std::vector<int> m_trtBResidualR;
  std::vector<int> m_trtBPullR;
  std::vector<int> m_trtBResidualRNoTube;
  std::vector<int> m_trtBPullRNoTube;
  std::vector<int> m_trtBLR;
  std::vector<std::vector<int>> m_trtBResVsEta;
  std::vector<std::vector<int>> m_trtBResVsPhiSec;
  std::vector<std::vector<int>> m_trtBLRVsPhiSec;
  std::vector<int> m_trtECPredictedR;
  std::vector<int> m_trtECMeasuredR;
  std::vector<int> m_trtECResidualR;
  std::vector<int> m_trtECPullR;
  std::vector<int> m_trtECResidualRNoTube;
  std::vector<int> m_trtECPullRNoTube;
  std::vector<int> m_trtECLR;
  std::vector<int> m_trtECResVsEta;
  std::vector<int> m_trtECResVsPhiSec;
  std::vector<int> m_trtECLRVsPhiSec;
};

#endif
