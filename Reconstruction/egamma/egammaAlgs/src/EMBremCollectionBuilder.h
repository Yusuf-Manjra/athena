/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EGAMMAALGS_EMBREMCOLLECTIONBUILDER_H
#define EGAMMAALGS_EMBREMCOLLECTIONBUILDER_H
/**
  @class EMBremCollectionBuilder
  Algorithm which creates new brem-refitted tracks
  */

#include "egammaInterfaces/IegammaTrkRefitterTool.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkTrack/TrackCollection.h"

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "TrkTrack/Track.h"      
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include <memory>
class EMBremCollectionBuilder : public AthAlgorithm 
{

public:
  /** @brief Default constructor*/
  EMBremCollectionBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute() override final;

  struct TrackWithIndex{

    TrackWithIndex(std::unique_ptr<Trk::Track>intrack,
                   size_t index):track(std::move(intrack)), origIndex(index){}
    
    TrackWithIndex(const TrackWithIndex&) = delete;
    TrackWithIndex(TrackWithIndex&&) = default;
    TrackWithIndex& operator=(const TrackWithIndex&) = delete;
    TrackWithIndex& operator=(TrackWithIndex&&) = default;
    ~TrackWithIndex() = default;

    std::unique_ptr<Trk::Track> track;
    size_t origIndex;
  };

private:

  StatusCode refitTracks(const EventContext& ctx,
                         const std::vector<const xAOD::TrackParticle*>& input,                           
                         std::vector<TrackWithIndex>& refitted,                                  
                         std::vector<TrackWithIndex>& failedfit)const;

  StatusCode createCollections(const std::vector<TrackWithIndex>& refitted,                            
                               const std::vector<TrackWithIndex>& failedfit,                           
                               const std::vector<TrackWithIndex>& trtAlone,                            
                               TrackCollection* finalTracks,                                     
                               xAOD::TrackParticleContainer* finalTrkPartContainer,              
                               const xAOD::TrackParticleContainer* AllTracks)const;                     

  StatusCode createNew(const TrackWithIndex& Info,                                               
                       TrackCollection* finalTracks,                                             
                       xAOD::TrackParticleContainer* finalTrkPartContainer,                      
                       const xAOD::TrackParticleContainer* AllTracks) const;

  void updateGSFTrack(const TrackWithIndex& Info, 
                      const xAOD::TrackParticleContainer* AllTracks) const;

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_trkRefitTool {this,
    "TrackRefitTool", "ElectronRefitterTool", "Track refitter tool"};

  /** @brief Tool to create track particle */
  ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool {this,
    "TrackParticleCreatorTool", 
    "TrackParticleCreatorTool", 
    "TrackParticle creator tool"};

  /** @brief Tool to slim tracks  */
  ToolHandle<Trk::ITrackSlimmingTool>  m_slimTool {this,
    "TrackSlimmingTool", "TrkTrackSlimmingTool", "Track slimming tool"};

  /** @brief Tool for Track summary  */
  ToolHandle<Trk::ITrackSummaryTool>   m_summaryTool {this,
    "TrackSummaryTool", "InDetTrackSummaryTool", "Track summary tool"};

  /** @brief Tool for extrapolation */
  ToolHandle<IEMExtrapolationTools> m_extrapolationTool {this,
    "ExtrapolationTool", "EMExtrapolationTools", "Extrapolation tool"};

  /** @brief Option to do truth*/
  Gaudi::Property<bool> m_doTruth {this, "DoTruth", false, "do truth"};

  /** @brief Option to do SCT holes estimation*/
  Gaudi::Property<bool> m_doSCT {this, "useSCT", false, "do SCT"};

  /** @brief Option to do truth*/
  Gaudi::Property<bool> m_doPix {this, "usePixel", false, "do Pix"};

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleContainerKey {this,
    "TrackParticleContainerName", "InDetTrackParticles", "Input InDet TrackParticles"};

  /** @brief Names of input output collections */
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_selectedTrackParticleContainerKey {this,
    "SelectedTrackParticleContainerName", "egammaSelectedTrackParticles", "Input of Selected TrackParticles"};

  SG::WriteHandleKey<xAOD::TrackParticleContainer> m_OutputTrkPartContainerKey {this,
    "OutputTrkPartContainerName", "GSFTrackParticles", "Output GSF TrackParticles"};

  SG::WriteHandleKey<TrackCollection> m_OutputTrackContainerKey {this,
    "OutputTrackContainerName", "GSFTracks", "Output GSF Trk::Tracks"};

  /** @Cut on minimum silicon hits*/
  Gaudi::Property<int> m_MinNoSiHits {this, "minNoSiHits", 4, 
    "Minimum number of silicon hits on track before it is allowed to be refitted"};

  // For P->T converters of ID tracks with Pixel
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, 
    "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  // For P->T converters of ID tracks with SCT
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, 
    "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  //counters
  mutable std::atomic_uint m_FailedFitTracks{0};
  mutable std::atomic_uint m_RefittedTracks{0};
  mutable std::atomic_uint m_FailedSiliconRequirFit{0};  

};
#endif //
