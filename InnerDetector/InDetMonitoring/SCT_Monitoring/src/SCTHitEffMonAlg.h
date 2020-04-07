// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCTHITEFFMONALG_H
#define SCTHITEFFMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#include "CommissionEvent/ComTime.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkTrack/Track.h" 
#include "TrkTrack/TrackCollection.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

//STL
#include <string>

class Identifier;
class ISCT_ConfigurationConditionsTool;
class PixelID;
class SCT_ID;
class TRT_ID;

class SCTHitEffMonAlg : public AthMonitorAlgorithm {
 public:
  SCTHitEffMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTHitEffMonAlg() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override final;

 private:
    // Method to cut on track or hit variables and automatize DEBUG statements 
  StatusCode failCut(bool value, std::string name) const;

  // Method to compute incident angle of track to wafer 
  StatusCode findAnglesToWaferSurface(const Amg::Vector3D& mom, const Identifier id,
                                       const InDetDD::SiDetectorElementCollection* elements,
                                       double& theta, double& phi) const;

  // Method to find the chip just before a given hit 
  int previousChip(double xl, int side, bool swap) const;

  // Computes residual of a hit to a track 
  double getResidual(const Identifier& surfaceID,
                      const Trk::TrackParameters* trkParam,
                      const InDet::SCT_ClusterContainer* p_sctclcontainer) const;

  //Convert a layer/disk number (0-21) to a bec index (0,1,2) according to position of that layer
   // Numbering is counter-intuitive, would expect C then B then A; in fact the original ordering was A, C, B
   // I have re-ordered this!!!! so now its C,B,A
   
  SCT_Monitoring::BecIndex layerIndex2becIndex(const int index) const;
  ///Convert a layer/disk number (0-21) to a layer number (0-8 for endcaps, 0-3 for barrel)
  int layerIndex2layer(const int index) const;
  int becIdxLayer2Index(const int becIdx, const int layer) const;

  std::string m_path;

  SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_sctContainerName{this, "SCT_ClusterContainer", "SCT_Clusters"};
  SG::ReadHandleKey<TrackCollection> m_TrackName{this, "TrackName", "CombinedInDetTracks"};
  SG::ReadHandleKey<ComTime> m_comTimeName{this, "ComTimeKey", "TRT_Phase"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};
  ToolHandle<Trig::IBunchCrossingTool> m_bunchCrossingTool{this, "BunchCrossingTool", "Trig::BunchCrossingTool/BunchCrossingTool"};
  
  ToolHandle<Trk::IResidualPullCalculator> m_residualPullCalculator{this, "ResPullCalc", "Trk::ResidualPullCalculator/ResidualPullCalculator"};
  ToolHandle<Trk::IRIO_OnTrackCreator> m_rotcreator{this, "ROTCreator", "InDet::SCT_ClusterOnTrackTool/SCT_ClusterOnTrackTool"};
  ToolHandle<Trk::ITrackHoleSearchTool> m_holeSearchTool{this, "HoleSearch", "InDet::InDetTrackHoleSearchTool"}; //HERE
  ToolHandle<ISCT_ConfigurationConditionsTool> m_configConditions{this, "ConfigConditions",
      "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};

  const PixelID* m_pixelId{nullptr};
  const SCT_ID* m_sctId{nullptr};
  const TRT_ID* m_trtId{nullptr};

  // If all 3 of these -1, defaults depend on cosmic/not
  IntegerProperty m_minSCTHits{this, "MinSCTHits", -1};
  IntegerProperty m_minTRTHits{this, "MinTRTHits", -1};
  IntegerProperty m_minOtherHits{this, "MinOtherHits", 6};

  IntegerProperty m_minPixelHits{this, "MinPixelHits", -1};

  FloatProperty m_maxPhiAngle{this, "MaxPhiAngle", 40., "Degrees, 100 implies no cut."};
  FloatProperty m_maxChi2{this, "MaxChi2", 3.};
  FloatProperty m_maxD0{this, "Maxd0", 10., "mm of D0"};
  FloatProperty m_minPt{this, "MinPt", 1000., "minimu pt in MeV/c"};
  FloatProperty m_effdistcut{this, "effDistanceCut", 2., "mm"};
  FloatProperty m_maxZ0sinTheta{this, "MaxZ0sinTheta", 0.};
  UnsignedIntegerProperty m_maxTracks{this, "MaxTracks", 500};

  BooleanProperty m_insideOutOnly{this, "InsideOutOnly", false};
  BooleanProperty m_isCosmic{this, "IsCosmic", false};
  BooleanProperty m_useTRTPhase{this, "UseTRTPhase", false};
  BooleanProperty m_useSCTorTRT{this, "UseSCTorTRT", false};
  BooleanProperty m_requireEnclosingHits{this, "RequireEnclosingHits", false};
  BooleanProperty m_requireOtherFace{this, "RequireOtherFace", false};
  BooleanProperty m_requireGuardRing{this, "RequireGuardRing", false, "should be returned to true"};
  BooleanProperty m_vetoBadChips{this, "VetoBadChips", true};
  BooleanProperty m_useIDGlobal{this, "useIDGlobal", false};  
};

#endif // SCTHITEFFMONALG_H
