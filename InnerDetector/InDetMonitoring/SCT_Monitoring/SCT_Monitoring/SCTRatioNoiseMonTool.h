/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*    @file SCTRatioNoiseMonTool.h
 *   Class declaration for SCTRatioNoiseMonTool
 *
 *
 *
 *    @author Luca Fiorini, based on code from Shaun Roe, Manuel Diaz Gomez
 *    and Maria Jose Casta.
 *
 *
 *
 *
 */

#ifndef SCTRATIONOISEMONTOOL_H
#define SCTRATIONOISEMONTOOL_H

#include <string>
#include <vector>

// for Updator
#include "TrkToolInterfaces/IUpdator.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "SCT_Monitoring/SCTMotherTrigMonTool.h"
#include "SCT_Monitoring/SCT_MonitoringNumbers.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declarations
class IInterface;
class TH1I;
class TH1F;
class TH2I;
class TH2F;
class TProfile2D;
class TProfile;
class TH1F_LW;
class TH2F_LW;
class StatusCode;
class SCT_ID;
class IInDetConditionsSvc;

using namespace SCT_Monitoring;

#define DEBUG(x) ATH_MSG_DEBUG(x)
#define INFO(x) ATH_MSG_INFO(x)
#define WARNING(x) ATH_MSG_WARNING(x)
#define ERROR(x) ATH_MSG_ERROR(x)
#define VERBOSE(x) ATH_MSG_VERBOSE(x)

///Concrete monitoring tool derived from SCTMotherTrigMonTool
class SCTRatioNoiseMonTool : public SCTMotherTrigMonTool{
 public:
  SCTRatioNoiseMonTool(const std::string & type, const std::string & name, const IInterface* parent);
  virtual ~SCTRatioNoiseMonTool();
   /**    @name Book, fill & check (reimplemented from baseclass) */
//@{
  ///Book histograms in initialization
  virtual StatusCode bookHistogramsRecurrent();                                                     // hidetoshi 14.01.21
  virtual StatusCode bookHistograms();                                                              // hidetoshi 14.01.21
  ///Fill histograms in each loop
  virtual StatusCode fillHistograms() ;
  ///process histograms at the end (we only use 'isEndOfRun')
  virtual StatusCode procHistograms();                                                              // hidetoshi 14.01.21
  ///helper function used in procHistograms
  StatusCode checkHists(bool fromFinalize);
//@}


private:
  //@name typedefs centralised to enable easy changing of types
  //@{
  typedef TProfile * Prof_t;
  typedef TProfile2D * Prof2_t;
  typedef TH1F_LW * H1_t;
  typedef TH2F_LW * H2_t;
  typedef TH1I * H1I_t;
  typedef TH2I * H2I_t;
  typedef std::vector<Prof2_t> VecProf2_t;
  typedef std::vector<Prof_t> VecProf_t;
  typedef std::vector<H1_t> VecH1_t;
  typedef std::vector<H2_t> VecH2_t;
  typedef std::vector<H1I_t> VecH1I_t;
  //@}
  int eventID;
  int m_numberOfEvents;
  int nLink0[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];
  int nLink1[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];
  int nBoth[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];
  int nNoSidesBarrelModule[N_BARRELS];
  int nOneSideBarrelModule[N_BARRELS];
  int nTwoSideBarrelModule[N_BARRELS];
  int nNonGoodModulesBarrelModule[N_DISKS];
  int nNoSidesEndcapAModule[N_DISKS];
  int nOneSideEndcapAModule[N_DISKS];
  int nTwoSideEndcapAModule[N_DISKS];
  int nNonGoodModulesEndcapAModule[N_DISKS];
  int nNoSidesEndcapCModule[N_DISKS];
  int nOneSideEndcapCModule[N_DISKS];
  int nTwoSideEndcapCModule[N_DISKS];
  int nNonGoodModulesEndcapCModule[N_DISKS];
  
  bool goodModules[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];

  VecProf2_t m_pnoiseoccupancymapHistoVectorECC;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECCSide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECCSide1;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBar;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBarSide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBarSide1;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECA;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECASide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECASide1;

  int nOneSideModule;
  int nTwoSideModule;
  int nNoFiredModules;
  
  double d1;
  double n1;
  double n1Barrel[N_BARRELS];
  double n1EndcapA[N_DISKS];
  double n1EndcapC[N_DISKS];
  double d1Barrel[N_BARRELS];
  double d1EndcapA[N_DISKS];
  double d1EndcapC[N_DISKS];


  bool noSidesHit;
  bool oneSideHit;
  bool twoSidesHit;
  bool correct_TimeBin;

  int nZero[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];
  int nOne[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];
  int nOneSide0[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];
  int nOneSide1[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];
  int nLayer[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];
  int nEta[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];
  int nPhi[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];
  int nNonGoodModule[N_MOD_BARREL + 2 * N_MOD_ENDCAPS];

  int m_checkrecent;
  int m_current_lb;
  int m_last_reset_lb;
  
  int set_timebin;
  int tbin;
  int modNum;
  float ratio;
  float ratioside0;
  float ratioside1;
  
  static const long NBINS_LBs = 2000;

  //@name Histograms related members
  //@{

  //General Histograms
  H1_t m_NOEV;
  H1_t m_NOEV_Barrel[N_BARRELS];
  H1_t m_NOEV_EndcapC[N_DISKS];
  H1_t m_NOEV_EndcapA[N_DISKS];
  H2_t m_NOEV_RDO;
  H2_t m_NOEV_Eventnum;
  H1_t m_side;
  H1_t m_num_RDO;
  H1_t m_NZ1;
  H1_t m_N11;
  H1_t m_N21;

  H1_t m_NZ1BAR[N_BARRELS];
  H1_t m_N11BAR[N_BARRELS];
  H1_t m_N21BAR[N_BARRELS];
  H1_t m_NZ1BAR_vsLB[N_BARRELS];
  H1_t m_N11BAR_vsLB[N_BARRELS];

  H1_t m_NZ1ECC[N_DISKS];
  H1_t m_N11ECC[N_DISKS];
  H1_t m_N21ECC[N_DISKS];
  H1_t m_NZ1ECC_vsLB[N_DISKS];
  H1_t m_N11ECC_vsLB[N_DISKS];
  
  H1_t m_NZ1ECA[N_DISKS];
  H1_t m_N11ECA[N_DISKS];
  H1_t m_N21ECA[N_DISKS];
  H1_t m_NZ1ECA_vsLB[N_DISKS];
  H1_t m_N11ECA_vsLB[N_DISKS];

  H1_t m_NOb;
  H1_t m_NObSide;
  H1_t m_NO;
  H1_t m_NOSide;
  H1_t m_NOb_layer[N_BARRELS];
  H1_t m_NOb_layer_vsLB[N_BARRELS];
  H1_t m_NOECC_disk[N_DISKS];
  H1_t m_NOECC_disk_vsLB[N_DISKS];
  H1_t m_NOECA_disk[N_DISKS];
  H1_t m_NOECA_disk_vsLB[N_DISKS];
  H1_t m_NOEC;
  H1_t m_NOECSide;
  H1_t m_NOECASide;
  H1_t m_NOECCSide;
  H1_t m_NOEC_Outer;
  H1_t m_NOEC_ShortMiddle;
  H1_t m_NOEC_Inner;
  H1_t m_NOEC_Middle; 
  H1_t m_numberHitsinBarrel[N_BARRELS];

  H1_t m_NZ1_vs_modnum;
  H1_t m_N11_vs_modnum;


  std::string m_stream;
  std::string m_path;
  //@}
  /// Name of the Track collection to use
  std::string m_tracksName;

  std::string m_NOTrigger;

  //@name Service members
  //@{
  /// Kalman Updator for SCT Unbiased states in Residual calculation
//  ToolHandle<Trk::IUpdator> m_updator;
 /// Data object name: for the SCT this is "SCT_RDOs"
  std::string m_dataObjectName;
  ///SCT Helper class
  const SCT_ID* m_pSCTHelper;
  //SCT Detector Manager
  const InDetDD::SCT_DetectorManager* m_sctmgr;
  ServiceHandle<IInDetConditionsSvc>       m_pSummarySvc;
  bool                                     m_checkBadModules;
  bool                                     m_ignore_RDO_cut_online;
  /// For online monitoring
  int m_checkrate;
  //@}
  //@name  Histograms related methods
  //@{
  // Book Histograms
  //  StatusCode bookRatioNoiseHistos(bool isNewRun,bool isNewLumiBlock);  // hidetoshi 14.01.22
  StatusCode bookRatioNoiseHistos();                                       // hidetoshi 14.01.22
  //@}

  //@name Service methods
  //@{
  // Calculate the local angle of incidence
  int findAnglesToWaferSurface ( const double (&vec)[3], const double &sinAlpha, const Identifier &id, double &theta, double &phi );
  float calculateNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide);
  float calculateOneSideNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide);
  bool isBarrel(const int moduleNumber);
  bool isEndcap(const int moduleNumber);
  bool isEndcapA(const int moduleNumber);
  bool isEndcapC(const int moduleNumber);

  ///Factory + register for the 2D profiles, returns whether successfully registered
  Prof_t  pFactory(const std::string & name, const std::string & title, int nbinsx, double xlow, double xhigh, MonGroup & registry, int& iflag);
  ///Factory + register for the 1D histograms, returns whether successfully registered

  H1_t
    h1Factory(const std::string & name, const std::string & title, MonGroup & registry, VecH1_t & storageVector, const float lo, const float hi, const unsigned int nbins);
  H1_t
    h1Factory(const std::string & name, const std::string & title, MonGroup & registry, const float lo, const float hi, const unsigned int nbins);
  H2_t
    h2Factory(const std::string & name, const std::string & title, MonGroup & registry, const float lo_x, const float hi_x, const unsigned int nbins_x, const float lo_y, const float hi_y, const unsigned int nbins_y);
  H1I_t
    h1IFactory(const std::string & name, const std::string & title, MonGroup & registry, VecH1I_t & storageVector, const float lo, const float hi, const unsigned int nbins);
  H1I_t
    h1IFactory(const std::string & name, const std::string & title, MonGroup & registry, const float lo, const float hi, const unsigned int nbins);
  Prof2_t
    prof2Factory(const std::string & name, const std::string & title, const unsigned int&, MonGroup & registry, VecProf2_t & storageVector);
 
  //@}
};

#endif
