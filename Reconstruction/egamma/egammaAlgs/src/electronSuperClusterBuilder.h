/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_ELECTRONSUPERCLUSTERBUILDER_H
#define EGAMMAALGS_ELECTRONSUPERCLUSTERBUILDER_H

// INCLUDE HEADER FILES:
#include "egammaSuperClusterBuilder.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

//Fwd declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"

class electronSuperClusterBuilder : public egammaSuperClusterBuilder {

 public:
  //Constructor/destructor.
  electronSuperClusterBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  //Tool standard routines.
  StatusCode initialize() override final;
  StatusCode finalize() override final;
  StatusCode execute() override final;

 private:

  bool matchSameTrack(const xAOD::TrackParticle& seedTrack,
                      const egammaRec& sec) const;

  const std::vector<std::size_t> searchForSecondaryClusters(const size_t i,
                                                            const EgammaRecContainer*,
                                                            const double EMEnergy,
                                                            std::vector<bool>& isUsed);
  
  bool passesSimpleBremSearch(const xAOD::CaloCluster& sec,
                              float perigeeExtrapEta,
                              float perigeeExtrapPhi) const;
  
  
  /////////////////////////////////////////////////////////////////////
  
  /** @brief Size of maximum search window in eta */
  Gaudi::Property<int> m_maxDelEtaCells {this, 
      "MaxWindowDelEtaCells", 5,
      "Size of maximum search window in eta"};
   /** @brief Size of maximum search window in phi */
  Gaudi::Property<int> m_maxDelPhiCells {this,
      "MaxWindowDelPhiCells", 12,
      "Size of maximum search window in phi"};
  
  float m_maxDelEta;
  float m_maxDelPhi;
  /** @brief Use Brem search when the seed E/P is less than thi value */ 
  Gaudi::Property<float> m_secEOverPCut {this,
      "BremSearchEOverPCut",  1.5,
      "Maximum E/P seed requirement for doing brem search"};
  
  /** @brief Delta Eta for matching a cluster to the extrapolated position of the a possible brem photon */ 
  Gaudi::Property<float> m_bremExtrapMatchDelEta {this,
      "BremExtrapDelEtaCut",  0.05,
      "maximum DelEta for brem search"};

  /** @brief Delta Phi for matching a cluster to the extrapolated position of the a possible brem photon */ 
  Gaudi::Property<float> m_bremExtrapMatchDelPhi {this,
      "BremExtrapDelPhiCut", 0.075,
      "maximum DelPhi for brem search"};

  Gaudi::Property<std::size_t> m_numberOfPixelHits {this, 
      "NumberOfReqPixelHits", 1, "Number of required pixel hits for electrons"};

  Gaudi::Property<std::size_t> m_numberOfSiHits {this, 
      "NumberOfReqSiHits", 7, "Number of required silicon hits for electrons"};

  /** @brief Key for input egammaRec container */
  SG::ReadHandleKey<EgammaRecContainer> m_inputEgammaRecContainerKey {this,
      "InputEgammaRecContainerName", "egammaRecCollection",
      "input egammaRec container"};
      
  /** @brief Key for output egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_electronSuperRecCollectionKey {this,
      "SuperElectronRecCollectionName", "ElectronSuperRecCollection",
      "output egammaRec container"};
  
  /** @brief Key for output clusters */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputElectronSuperClustersKey {this,
      "SuperClusterCollestionName", "ElectronSuperClusters",
      "output calo cluster container"};

  /** @brief Tool to perform track matching*/
  ToolHandle<IEMTrackMatchBuilder> m_trackMatchBuilder {this,
      "TrackMatchBuilderTool", "EMTrackMatchBuilder",
      "Tool that matches tracks to egammaRecs"};

  /** @brief private member flag to do the track matching */
  Gaudi::Property<bool> m_doTrackMatching {this, "doTrackMatching", true,
      "Boolean to do track matching"};

};

#endif
