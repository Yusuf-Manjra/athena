/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     CalorimeterL1CaloMon.h
// PACKAGE:  TrigT1Monitoring
//
// AUTHOR:   Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//           Sky French (sfrench@hep.phy.cam.ac.uk)
//	     
//
// ********************************************************************
#ifndef TRIGT1MONITORING_CALORIMETERL1CALOMON_H
#define TRIGT1MONITORING_CALORIMETERL1CALOMON_H

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class StatusCode;
class TH1F_LW;
class TH2F_LW;
class TProfile2D_LW;
class TProfile_LW;

class CaloDetDescrElement;
// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class IL1TriggerTowerTool;
class IL1CaloMonitoringCaloTool;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramToolV1;
// ============================================================================

/** Monitoring of the Calo/L1Calo interface.
 *
 *  Compares Et of summed CaloCells (CaloTT) with TriggerTowers (TT)
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory
 *    </th><th> Contents                                                            </th></tr>
 *  <tr><td> @c LVL1_Interfaces/Calorimeter/CalorimeterData
 *    </td><td> CaloCell Et, eta and phi distributions                              <br>
 *              CaloCell eta/phi hitmap                                             </td></tr>
 *  <tr><td> @c LVL1_Interfaces/Calorimeter/CaloTriggerTowerHitMaps
 *    </td><td> Eta/phi hitmaps of CaloTT Et > various thresholds                   </td></tr>
 *  <tr><td> @c LVL1_Interfaces/Calorimeter/CaloTriggerTowerEnergyDistribution
 *    </td><td> CaloTT Et, eta and phi distributions                                </td></tr>
 *  <tr><td> @c LVL1_Interfaces/Calorimeter/ComparisonTT2CaloTT
 *    </td><td> Comparisons between CaloTT Et and TriggerTower Et:                  <br>
 *              Et comparisons and differences by tower, partition, eta, phi and Et <br>
 *              CaloTT Et zero and TT Et > 5 and vice-versa                         <br>
 *              Hitmap of most energetic CaloTT and TT by event
 *  </table>
 *
 *  <!--
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram               </th><th> Comment                     </th></tr>
 *  <tr><td>                         </td><td>                             </td></tr>
 *  </table>
 *
 *  <b>Custom Merges Used (Tier0):</b>
 *
 *  <table>
 *  <tr><th> Merge                   </th><th> Used For                    </th></tr>
 *  <tr><td>                         </td><td>                             </td></tr>
 *  </table>
 *  -->
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container               </th><th> Comment                </th></tr>
 *  <tr><td> @c CaloCellContainer    </td><td> CaloCell data          </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::TriggerTower> </td><td> PPM trigger tower data </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                               </th><th> Description          </th></tr>
 *  <tr><td> @c LVL1::IL1TriggerTowerTool       </td><td> @copydoc m_ttTool    </td></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool          </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramTool       </td><td> @copydoc m_histTool  </td></tr>
 *  <tr><td> @c LVL1::IL1CaloMonitoringCaloTool </td><td> @copydoc m_caloTool  </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                 </th><th> Description                          </th></tr>
 *  <tr><td> @c CaloCellContainer     </td><td> @copydoc m_caloCellContainerName     </td></tr>
 *  <tr><td> @c TriggerTowerContainer </td><td> @copydoc m_TriggerTowerContainerName </td></tr>
 *  <tr><td> @c CaloThreshold         </td><td> @copydoc m_CaloThreshold             </td></tr>
 *  <tr><td> @c Calo_HitMap_Thresh0   </td><td> @copydoc m_CaloTT_HitMap_Thresh0     </td></tr>
 *  <tr><td> @c Calo_HitMap_Thresh1   </td><td> @copydoc m_CaloTT_HitMap_Thresh1     </td></tr>
 *  <tr><td> @c Calo_HitMap_Thresh2   </td><td> @copydoc m_CaloTT_HitMap_Thresh2     </td></tr>
 *  <tr><td> @c MaxEnergyRange        </td><td> @copydoc m_MaxEnergyRange            </td></tr>
 *  <tr><td> @c PathInRootFile        </td><td> @copydoc m_PathInRootFile            </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Related Documentation:</b>
 *  -->
 *
 *  @authors Johanna Fleckner, Andrea Neusiedl, Sky French, Peter Faulkner
 *
 */

class CalorimeterL1CaloMon: public ManagedMonitorToolBase
{

 public:
  
  CalorimeterL1CaloMon(const std::string & type, const std::string & name,
		       const IInterface* parent);
    

  virtual ~CalorimeterL1CaloMon();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  /// Return true if Calo cell is used in TT
  bool usedInTT(const CaloDetDescrElement* caloDDE);

  /// Tool for identifiers and disabled channels
  ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool;
  /// Event veto tool
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper
  ToolHandle<TrigT1CaloLWHistogramToolV1> m_histTool;
  /// Tool for CaloTT energies
  ToolHandle<LVL1::IL1CaloMonitoringCaloTool> m_caloTool;

  /// CaloCellContainer StoreGate key
  std::string m_caloCellContainerName;
  /// TriggerTower collection StoreGate key
  std::string m_TriggerTowerContainerName;

  /// Hitmaps per threshold first value
  int m_CaloTT_HitMap_Thresh0;
  /// Hitmaps per threshold second value
  int m_CaloTT_HitMap_Thresh1;
  /// Hitmaps per threshold third value
  int m_CaloTT_HitMap_Thresh2;
  /// Maximim energy in plots
  int m_MaxEnergyRange;
  /// Minimum CaloCell energy in GeV
  double m_CaloThreshold;
  
  /// Root directory
  std::string m_PathInRootFile;

  /// Histograms booked flag
  bool m_histBooked;

  //CaloCell 
  TH1F_LW* m_h_CaloCell_tile_phi;                   ///< phi - Distribution of Tile CaloCells
  TH1F_LW* m_h_CaloCell_tile_eta;                   ///< eta - Distribution of Tile CaloCells
  TH2F_LW* m_h_CaloCell_tile_HitMap;                ///< eta - phi Map of Tile CaloCells
  TH1F_LW* m_h_CaloCell_tile_et;                    ///< Distribution of Tile CaloCell Energy

  TH1F_LW* m_h_CaloCell_lar_phi;                    ///< phi - Distribution of LAr CaloCells
  TH1F_LW* m_h_CaloCell_lar_eta;                    ///< eta - Distribution of LAr CaloCells
  TH2F_LW* m_h_CaloCell_lar_HitMap;                 ///< eta - phi Map of LAr CaloCells
  TH1F_LW* m_h_CaloCell_lar_et;                     ///< Distribution of LAr CaloCell Energy

  //LUT Hitmaps per threshold
  TH2F_LW* m_h_CaloTT_HitMap_emLUT_Thresh0;         ///< eta - phi Map of Calo EM Et > Thresh0
  TH2F_LW* m_h_CaloTT_HitMap_emLUT_Thresh1;         ///< eta - phi Map of Calo EM Et > Thresh1
  TH2F_LW* m_h_CaloTT_HitMap_emLUT_Thresh2;         ///< eta - phi Map of Calo EM Et > Thresh2

  TH2F_LW* m_h_CaloTT_HitMap_hadLUT_Thresh0;        ///< eta - phi Map of Calo Had Et > Thresh0
  TH2F_LW* m_h_CaloTT_HitMap_hadLUT_Thresh1;        ///< eta - phi Map of Calo Had Et > Thresh1
  TH2F_LW* m_h_CaloTT_HitMap_hadLUT_Thresh2;        ///< eta - phi Map of Calo Had Et > Thresh2

  //distribution of LUT peak per detector region
  TH1F_LW* m_h_CaloTT_emLUT;                        ///< Calo EM Et Distribution
  TH1F_LW* m_h_CaloTT_emLUT_eta;                    ///< Calo EM eta Distribution
  TH1F_LW* m_h_CaloTT_emLUT_phi;                    ///< Calo EM phi Distribution

  TH1F_LW* m_h_CaloTT_hadLUT;                       ///< Calo HAD Et Distribution
  TH1F_LW* m_h_CaloTT_hadLUT_eta;                   ///< Calo HAD eta Distribution
  TH1F_LW* m_h_CaloTT_hadLUT_phi;                   ///< Calo HAD phi Distribution

  //comparison between CaloTT and TT
  TH2F_LW* m_h_emEnergy_Match_CaloTT_TT_LArEMB;     ///< LArEMB Match between Calo and L1Calo em Et
  TH2F_LW* m_h_emEnergy_Match_CaloTT_TT_LArOverlap; ///< LArOverlap Match between Calo and L1Calo em Et
  TH2F_LW* m_h_emEnergy_Match_CaloTT_TT_LArEMEC;    ///< LArEMEC Match between Calo and L1Calo em Et
  TH2F_LW* m_h_emEnergy_Match_CaloTT_TT_LArFCAL1;   ///< LArFCAL1 Match between Calo and L1Calo em Et
  TH2F_LW* m_h_hadEnergy_Match_CaloTT_TT_TileLB;    ///< TileLB Match between Calo and L1Calo had Et
  TH2F_LW* m_h_hadEnergy_Match_CaloTT_TT_TileEB;    ///< TileEB Match between Calo and L1Calo had Et
  TH2F_LW* m_h_hadEnergy_Match_CaloTT_TT_LArHEC;    ///< LArHEC Match between Calo and L1Calo had Et
  TH2F_LW* m_h_hadEnergy_Match_CaloTT_TT_LArFCAL23; ///< LArFCAL23 Match between Calo and L1Calo had Et

  TProfile_LW* m_h_em_profile_Match;                ///< Difference between L1Calo - Calo em Et
  TProfile_LW* m_h_had_profile_Match;               ///< Difference between L1Calo - Calo had Et
 
  TH2F_LW* m_h_em_Mismatch_etaphi_alt;              ///< eta-phi map of em mismatches with L1Calo Et > 5 and zero Calo Et
  TH2F_LW* m_h_em_Mismatch_etaphi;                  ///< eta-phi map of em mismatches with zero L1Calo Et and Calo Et > 5
  TH2F_LW* m_h_had_Mismatch_etaphi_alt;             ///< eta-phi map of had mismatches with L1Calo Et > 5 and zero Calo Et
  TH2F_LW* m_h_had_Mismatch_etaphi;                 ///< eta-phi map of had mismatches with zero L1Calo Et and Calo Et > 5
  
  TH1F_LW* m_h_em_1D_Match_LArEMB;                  ///< LArEMB Difference between L1Calo and Calo em Et
  TH1F_LW* m_h_em_1D_Match_LArOverlap;              ///< LArOverlap Difference between L1Calo and Calo em Et
  TH1F_LW* m_h_em_1D_Match_LArEMEC;                 ///< LArEMEC Difference between L1Calo and Calo em Et
  TH1F_LW* m_h_em_1D_Match_LArFCAL1;                ///< LArFCAL1 Difference between L1Calo and Calo em Et
  TH1F_LW* m_h_had_1D_Match_TileLB;                 ///< TileLB Difference between L1Calo and Calo had Et
  TH1F_LW* m_h_had_1D_Match_TileEB;                 ///< TileEB Difference between L1Calo and Calo had Et
  TH1F_LW* m_h_had_1D_Match_LArHEC;                 ///< LArHEC Difference between L1Calo and Calo had Et
  TH1F_LW* m_h_had_1D_Match_LArFCAL23;              ///< LArFCAL23 Difference between L1Calo and Calo had Et

  TProfile_LW* m_h_em_profile_etaRegion;            ///< Difference in eta: L1Calo - Calo em Et
  TProfile_LW* m_h_had_profile_etaRegion;           ///< Difference in eta: L1Calo - Calo had Et
  
  TProfile_LW* m_h_had_profile_phiRegion;           ///< Difference in phi: L1Calo - Calo had Et
  TProfile_LW* m_h_em_profile_phiRegion;            ///< Difference in phi: L1Calo - Calo em Et
  
  TProfile2D_LW* m_h_average_emDE_map;              ///< eta-phi map of average L1Calo - Calo em Et difference
  TProfile2D_LW* m_h_average_hadDE_map;             ///< eta-phi map of average L1Calo - Calo had Et difference
   
  //Most Energetic towers
  TH2F_LW* m_h_emTTME_etaphi;                       ///< eta-phi map of em most energetic TT
  TH2F_LW* m_h_hadTTME_etaphi;                      ///< eta-phi map of had most energetic TT
  
  TH2F_LW* m_h_emcaloME_etaphi;                     ///< eta-phi map of em most energetic caloTT
  TH2F_LW* m_h_hadcaloME_etaphi;                    ///< eta-phi map of had most energetic caloTT

};
// ============================================================================
} // end namespace
// ============================================================================

#endif
