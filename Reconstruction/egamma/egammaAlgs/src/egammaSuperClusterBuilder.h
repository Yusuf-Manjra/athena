/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMAALGS_EGAMMASUPERCLUSTERBUILDER_H
#define EGAMMAALGS_EGAMMASUPERCLUSTERBUILDER_H

// INCLUDE HEADER FILES:
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgorithm.h"

//Fwd declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "CaloUtils/CaloCellDetPos.h"
#include "xAODEgamma/EgammaEnums.h"
#include "egammaInterfaces/IegammaSwTool.h"
#include "egammaInterfaces/IegammaMVASvc.h"

#include <memory>

/** Base class for electronSuperClusterBuilder and photonSuperClusterBuilder.
 *  The inheritance should be private. This class should never be instantiated
 *  by itself, and hence has no interface class
 **/
class egammaSuperClusterBuilder : public AthAlgorithm {

protected:
  /** Protected constructor since this class should not be instantiated by itself */
  egammaSuperClusterBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** should be called by the derived class in the initialize phase */
  StatusCode initialize() override;

  /** Is clus in window center around ref? */
  bool matchesInWindow(const xAOD::CaloCluster *ref,
		       const xAOD::CaloCluster *clus) const;

  /** Creates a new supercluster out of the input cluster */
  // not const because it calls CalibrateCluster
  std::unique_ptr<xAOD::CaloCluster> createNewCluster(const std::vector<const xAOD::CaloCluster*>& clusters,
						      xAOD::EgammaParameters::EgammaType egType);

  // some constants to use
  static constexpr float s_cellEtaSize = 0.025;
  static constexpr float s_cellPhiSize = M_PI/128.;

  /** @brief Seed selection requirements */
  Gaudi::Property<float> m_EtThresholdCut {this,
      "EtThresholdCut", 1.5*Gaudi::Units::GeV,
      "The minimum EM Et required of SEED clusters (not applied to secondaries)"};

  /** @brief Seed selection requirements */
  Gaudi::Property<float> m_EMFracThresholdCut {this,
      "EMFracThresholdCut", 0.65,
      "The minimum EMFrac required of SEED clusters (not applied to secondaries)"};


  // these are calculated search window values
  float m_searchWindowEtaBarrel; //!< half of search window size, converted to units of eta
  float m_searchWindowPhiBarrel; //!< half of search window size, converted to units of phi
  float m_searchWindowEtaEndcap; //!< half of search window size, converted to units of eta
  float m_searchWindowPhiEndcap; //!< half of search window size, converted to units of phi
  //
private:

  struct CentralPosition {
    float etaB = 999;
    float phiB = 999;
    float emaxB = -999*Gaudi::Units::GeV;
    float etaEC = 999;
    float phiEC = 999;
    float emaxEC = -999*Gaudi::Units::GeV;
  };
  
  /** Find the reference position (eta, phi) relative to which cells are restricted.
      The return value is whether it succeeded in finding a positive energy max value. 
      (If rv = false, the output variables are passed as arguments are not updated.) 
  */
  CentralPosition findCentralPosition(const std::vector<const xAOD::CaloCluster*>& clusters) const;

  struct PhiSize {
    float plusB = 0;
    float minusB = 0;
    float plusEC = 0;
    float minusEC = 0;
  };
  
  /** Find the size of the cluster */  
  PhiSize findPhiSize(const CentralPosition& cp0,
		      const xAOD::CaloCluster* cluster) const;

  
  /** Add the EM cells from reference cluster to self; eta and phi are the ones to use for limiting size. 
      This excludes L1 (which is done as a separate step). note, use raw eta and phi! */
  StatusCode addEMCellsToCluster(xAOD::CaloCluster* newCluster,
				 const xAOD::CaloCluster* ref,
				 const CentralPosition& cp0) const;

  /** Add the preshower and L1 EM cells from reference cluster to self; note, use raw eta and phi! */
  StatusCode addL0L1EMCellsToCluster(xAOD::CaloCluster* newCluster,
				     const xAOD::CaloCluster* ref,
				     const CentralPosition& cp0,
				     const PhiSize& phiSize) const;
  
  /** function to calibrate the new clusters energy */
  StatusCode calibrateCluster(xAOD::CaloCluster* newCluster,
			      const xAOD::EgammaParameters::EgammaType egType) ;
  // not const because it calls fillPositionsInCalo


  /** function to decorate the calo cluster with position variables */
  StatusCode fillPositionsInCalo(xAOD::CaloCluster* cluster); 
  // above can't be const because m_caloCellDetPos acceses are not const

  /** functions to refine position in eta1*/
  StatusCode refineEta1Position(xAOD::CaloCluster* cluster) const ;
  StatusCode makeCorrection1(xAOD::CaloCluster* cluster,    
			     const CaloSampling::CaloSample sample) const ;
  
  /** functions to add all tile Gap 3 cells in a window*/
  StatusCode addTileGap3CellsinWindow(xAOD::CaloCluster *myCluster) const;

  // these are calculated window values for the windows in which cells of topoclusters are edded
  float m_addCellsWindowEtaBarrel; //!< half of addCells window size, converted to units of eta
  float m_addCellsWindowPhiBarrel; //!< half of addCells window size, converted to units of phi
  float m_addCellsWindowEtaEndcap; //!< half of addCells window size, converted to units of eta
  float m_addCellsWindowPhiEndcap; //!< half of addCells window size, converted to units of phi
  float m_extraL0L1PhiSize; //!< calculated value of cells to add in units of phi

  /** @brief Size of search window in eta for the barrel */
  Gaudi::Property<int> m_searchWindowEtaCellsBarrel {this,
      "SearchWindowEtaCellsBarrel", 3, 
      "Number of cells in eta of window in which to search for topoclusters"};

  /** @brief Size of search window in phi for the barrel */
  Gaudi::Property<int> m_searchWindowPhiCellsBarrel {this,
      "SearchWindowPhiCellsBarrel", 5,
      "Number of cells in phi of window in which to search for topoclusters"};

  /** @brief Size of search window in eta for the end-cap*/
  Gaudi::Property<int> m_searchWindowEtaCellsEndcap {this,
      "SearchWindowEtaCellsEndcap", 3, 
      "Number of cells in eta of window in which to search for topoclusters"};

  /** @brief Size of search window in phi for the end-cap*/
  Gaudi::Property<int> m_searchWindowPhiCellsEndcap {this,
      "SearchWindowPhiCellsEndcap", 5,
      "Number of cells in phi of window in which to search for topoclusters"};

  /** @brief Size of windows et eta in which cells of topoclusters are edded for the barrel */
  Gaudi::Property<int>   m_addCellsWindowEtaCellsBarrel {this,
      "AddCellsWindowEtaCellsBarrel", 3, 
      "Number of cells in eta of window around topocluster center to add cells"};

  /** @brief Size of windows et phi in which cells of topoclusters are edded for the barrel */
  Gaudi::Property<int>   m_addCellsWindowPhiCellsBarrel {this,
      "AddCellsWindowPhiCellsBarrel", 999 /*7 for SW*/,
      "Number of cells in phi of window around topocluster center to add cells"};

  /** @brief Size of windows et eta in which cells of topoclusters are edded for the endcap */
  Gaudi::Property<int>   m_addCellsWindowEtaCellsEndcap {this,
      "AddCellsWindowEtaCellsEndcap", 5, 
      "Number of cells in eta of window around topocluster center to add cells"};

  /** @brief Size of windows et phi in which cells of topoclusters are edded for the endcap */
  Gaudi::Property<int>   m_addCellsWindowPhiCellsEndcap {this,
      "AddCellsWindowPhiCellsEndcap", 999 /*5 for SW*/,
      "Number of cells in phi of window around topocluster center to add cells"};

  /** @brief "When adding L0 (PS) and L1 cells, how much wider than L2 is the acceptance */
  Gaudi::Property<int> m_extraL0L1PhiSizeCells {this,
      "ExtraL0L1PhiSize", 1,
      "When adding L0 (PS) and L1 cells, how much wider than L2 (in L2 cells) is the acceptance. Make large to remove limit"};
 
  /** @brief Threshold on minimum energy reconstructed in 2nd sampling */
  Gaudi::Property<double> m_thrE2min{this, "ThrE2min", 100.,
    "Threshold on minimum  energy reconstructed in 2nd sampling"};
  
  /** @brief Handle to the MVA calibration service **/
  ServiceHandle<IegammaMVASvc>  m_MVACalibSvc {this,
      "MVACalibSvc", "egammaMVASvc", "calibration service"};
  
  /** @brief Tool to handle cluster corrections */
  ToolHandle<IegammaSwTool>   m_clusterCorrectionTool {this,
      "ClusterCorrectionTool", "egammaSwTool/egammaswtool", 
      "tool that applies cluster corrections"};

  /** @brief Position in Calo frame**/  
  CaloCellDetPos m_caloCellDetPos;

};

#endif
