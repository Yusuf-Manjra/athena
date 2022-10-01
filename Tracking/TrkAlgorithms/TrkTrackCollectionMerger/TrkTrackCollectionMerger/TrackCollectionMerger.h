/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackCollectionMerger.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TrackCollectionMerger_H
#define TrackCollectionMerger_H

#include "AthContainers/ConstDataVector.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkTrack/TrackCollection.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"

#include <string>
#include <map>


namespace Trk {

  /** @brief Class-algorithm for track collection merging and removalof potential duplicate tracks. */
  class TrackCollectionMerger final: public AthAlgorithm
    {
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** @brief Standard Algotithm methods:                           */
      ///////////////////////////////////////////////////////////////////

      TrackCollectionMerger(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TrackCollectionMerger() {}
      virtual StatusCode initialize() override final;
      virtual StatusCode execute() override final;
      virtual StatusCode finalize() override final;

    protected:

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected data:                                       */
      ///////////////////////////////////////////////////////////////////
      
      /** Vector of track collections to be merged. */  
      SG::ReadHandleKeyArray<TrackCollection>
        m_tracklocation;
      /**overlay track collection, if track overlay is on; 
       * default is emptystring, i.e. track overlay is off */
      SG::ReadHandleKey<TrackCollection> m_overlayTrackLocation{
        this,
        "OverlayTracksLocation",
        "",
        "Pileup Track Collection"
      }; 
     // note that these handles are not used directly, they are needed here
      // just to ensure that these collections get loaded into SG
      /** pileup TRT PRDs, only retrieved if track overlay is on */
      SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_pileupTRT{
        this,
        "OverlayTRTClusters",
        "Bkg_TRT_DriftCircles",
        "Pileup Drift Circles"
      }; 
       /** pileup pixel PRDs, only retrieved if track overlay is on*/
      SG::ReadHandleKey<InDet::PixelClusterContainer> m_pileupPixel{
        this,
        "OverlayPixelClusters",
        "Bkg_PixelClusters",
        "Pileup Pixel Clusters"
      };
      /** pileup SCT PRDs, only retrieved if track overlay is on */
      SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_pileupSCT{
        this,
        "OverlaySCTClusters",
        "Bkg_SCT_Clusters",
        "Pileup SCT Clusters"
      }; 
      /** Combined track collection.   */
      SG::WriteHandleKey<ConstDataVector<TrackCollection>>
        m_outtracklocation;

      ///< the key given to the newly created association map
      SG::WriteHandleKey<Trk::PRDtoTrackMap> m_assoMapName{
        this,
        "AssociationMapName",
        ""
      }; ///< the key given to the newly created association map

      ToolHandle<Trk::IPRDtoTrackMapTool> m_assoTool{
        this,
        "AssociationTool",
        "InDet::InDetPRDtoTrackMapToolGangedPixels"
      };

      /** summary tool with shared hits enabled */
      ToolHandle<Trk::IExtendedTrackSummaryTool> m_trkSummaryTool;

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected methods:                                    */
      ///////////////////////////////////////////////////////////////////

      /** @brief A routine that merges the track collections. */
      StatusCode mergeTrack(const TrackCollection* trackCol,
                            Trk::PRDtoTrackMap* pPrdToTrackMap,
                            ConstDataVector<TrackCollection>* outputCol);

    private:
     
      bool m_updateSharedHits; //!< do not create the track summary again, but only update shared hits
      bool  m_updateAdditionalInfo;     //!< do not create the track summary again, but only update necessary things
      bool m_doTrackOverlay; //doing track overlay: needed to initialize the background PRD containers


    };
    
}
#endif // TrackCollectionMerger_H
