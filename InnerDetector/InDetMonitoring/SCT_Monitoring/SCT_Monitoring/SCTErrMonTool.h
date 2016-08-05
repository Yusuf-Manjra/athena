/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTErrMonTool.h
 *   Class declaration for SCTErrMonTool
 *
 *    @author Martin White based on code by Luca Fiorini,Shaun Roe, Manuel Diaz Gomez and Maria Jose Casta.
 *    Major tidying/restructuring by Martin Flechl
 *
 */

#ifndef SCTERRMONTOOL_H
#define SCTERRMONTOOL_H
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>
#include <vector>
#include <map>

//for CondDB
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h" 
#include "SCT_ConditionsServices/ISCT_FlaggedConditionSvc.h"
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"
#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

/** Forward declarations*/
class IInterface;
class TH1F;
class TH2F;
class TH1I;
class TH2I;
class TProfile;
class TProfile2D;
class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class TProfile_LW;
class TProfile2D_LW;
class StatusCode;
class SCT_ID;
class SCT_ModuleStatistics;
class ISCT_ByteStreamErrorsSvc;
class TString;

///Concrete monitoring tool derived from MonitorToolBase
class SCTErrMonTool : public ManagedMonitorToolBase
{
 public:
  SCTErrMonTool(const std::string & type,const std::string & name,const IInterface* parent);
  virtual ~SCTErrMonTool();
  //
  /**    @name Methods reimplemented from baseclass */
  //@{
  //book
  virtual StatusCode bookHistograms() final;
  //fill
  virtual StatusCode fillHistograms() final;
  //post processing
  virtual StatusCode procHistograms() final;
  //@}

 private:
  static const int NREGIONS_INC_GENERAL=SCT_Monitoring::N_REGIONS+1;
  typedef TProfile2D_LW * Prof2_t;
  typedef std::vector<Prof2_t> VecProf2_t;
  StatusCode checkRateHists();
  StatusCode fillByteStreamErrors();
  StatusCode bookErrHistos();
  StatusCode bookPositiveEndCapErrHistos();
  StatusCode bookNegativeEndCapErrHistos();

 
  /// ---------------------------------------
  //@name Histograms related members
  //@{
  // Pointers to hit error histograms
  TH1F_LW * m_firstHit;
  TH1F_LW * m_firstHit_ECp;
  TH1F_LW * m_firstHit_ECm;
  TH1F_LW * m_secondHit;
  TH1F_LW * m_secondHit_ECp;
  TH1F_LW * m_secondHit_ECm;
  //@}
  
  enum ErrorTypes {ABCD=0, RAW, TIMEOUT, LVL1ID, BCID, PREAMBLE, FORMATTER, MASKEDLINKS, RODCLOCK, TRUNCATEDROD, ROBFRAG, BSPARSE, MISSINGLINK, MASKEDRODS, MASKEDLINKALL, SUMMARY, BADERR, LINKLEVEL, RODLEVEL, N_ERRTYPES};
  int errorsToGet(int errtype); // transfer [enum ErrorTypes] -> [SCT_ByteStreamErrors]
  TString errorsString(int errtype); // transfer [enum ErrorTypes] -> [TString ErrorName]
  
  ///rate of errors
  TProfile2D_LW* m_allErrs[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///rate of errors per lumi-block
  TProfile2D_LW* m_allErrsPerLumi[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors
  TH2F_LW* m_pallErrs[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors
  TH2F_LW* m_pallErrsPerLumi[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  /// Recent error rate histograms
  TProfile2D_LW* m_summaryErrsRecent[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  /// Default histos to print per lumi block
  TH2F_LW* m_numErrorsPerLumi[NREGIONS_INC_GENERAL];
  TProfile2D_LW* m_rateErrorsPerLumi[NREGIONS_INC_GENERAL];

  TH1I *m_nErrors;
  TH1I *m_nLinksWithErrors;
  int *nErrors_buf;
  int *nLinksWithErrors_buf;
  int nErrors_pos;

  TH1I *m_MaskedAllLinks; // maskedLinks||maskedRODs

  //Count number of events
  int m_numberOfEventsLumi;
  int m_numberOfEvents;

  bool m_initialize;
  //max number of errors in lbs                                                                          
  unsigned int m_previous_lb;
  
  // Book noise map histograms
  StatusCode bookConfMaps();
  StatusCode bookPositiveEndCapConfMaps();
  StatusCode bookNegativeEndCapConfMaps();

  int m_nLink0[4088];
  int m_nLink1[4088];

  bool m_goodModules[4088];
  VecProf2_t m_pnoiseoccupancymapHistoVectorECC;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECCSide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECCSide1;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBar;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBarSide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBarSide1;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECA;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECASide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECASide1;

  bool m_noSidesHit;
  bool m_oneSideHit;
  int nZero[4088];
  int nOne[4088];
  int nOneSide0[4088];
  int nOneSide1[4088];
  int nLayer[4088];
  int nEta[4088];
  int nPhi[4088];
  int nNonGoodModule[4088];

  int tbin;
  int modNum;
  float ratio;
  float ratioside0;
  float ratioside1;

  //int fillByteStreamErrorsHelper(const std::set<IdentifierHash>* errors, TH2F_LW* histo[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2], bool lumi2DHist, int err_type);
  int fillByteStreamErrorsHelper(const std::set<IdentifierHash>* errors, TH2F_LW* histo[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2], bool lumi2DHist, int err_type, TH2F_LW* histo2[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]);
  void numByteStreamErrors(const std::set<IdentifierHash>* errors, int& ntot, int& nbar, int& neca, int& necc);
  StatusCode bookErrHistosHelper(MonGroup & mg, TString name, TString title, TString titlehitmap, TProfile2D_LW* &tprof, TH2F_LW* &th, const int layer, const bool barrel=true);

  StatusCode bookErrHistosHelper(MonGroup & mg, TString name, TString title, TProfile2D_LW* &tprof, const int layer, const bool barrel=true);

  std::vector<TH2F_LW *> m_p2DmapHistoVectorAll[NREGIONS_INC_GENERAL];

  /// "Magic numbers" for an SCT module
  //unsigned int m_nplanes; //to be determined from SCT Helper
  enum { Confbins = 6, ConfbinsDetailed = 5,n_lumiErrBins = 15, n_lumiBins = 3000 };
  // NOTE: The following is not the usual index order, which goes C, Barrel, A
  enum { iBARREL = 0, iECp = 1, iECm=2, iGEN=3 }; //iECp==EA, iECm==EC

  std::string m_stream;
  std::string m_path;
  /// CheckHists() frequency
  int m_checkrate;
  int m_checkrecent;
  int m_current_lb;
  int m_last_reset_lb;

  /// flag to run online
  bool m_runOnline;

  int m_evtsbins;
  bool m_doPositiveEndcap;
  bool m_doNegativeEndcap;
  // Use Summary database
  bool m_makeConfHisto;
  // Do lumi block 2D error histos
  bool m_doPerLumiErrors;
  bool m_doErr2DPerLumiHists;
  // Min stats per layer to use for number of inefficient modules
  float m_min_stat_ineff_mod;

  /// ---------------------------------------
  //@name Service members
  //@{
 
  /// Data object name: for the SCT this is "SCT_RDOs"
  std::string m_dataObjectName;

  ///SCT Helper class
  const SCT_ID* m_pSCTHelper;

  //@}

  /// ---------------------------------------
  //@name Service methods
  //@{

  //SCT_ModuleConditionsTool* m_ModuleConditions;
  ServiceHandle<ISCT_ConfigurationConditionsSvc> m_ConfigurationSvc;
  ServiceHandle<ISCT_FlaggedConditionSvc>   m_flaggedSvc;
  StatusCode fillCondDBMaps();
  StatusCode fillConfigurationDetails();
  StatusCode resetCondDBMaps();
  StatusCode resetConfigurationDetails();

  /// Pointer to 1D histogram of Number of SCT Clusters per LBs
  TProfile_LW * m_Conf[NREGIONS_INC_GENERAL];
  TProfile_LW * m_ConfRN[NREGIONS_INC_GENERAL];
  TProfile_LW * m_ConfOnline[NREGIONS_INC_GENERAL];

  TProfile_LW * m_ByteStreamVsLB[N_ERRTYPES][NREGIONS_INC_GENERAL];
  TProfile_LW * m_LinksWithAnyErrorsVsLB[NREGIONS_INC_GENERAL];
  TProfile_LW * m_LinksWithBadErrorsVsLB[NREGIONS_INC_GENERAL];
  TProfile_LW * m_LinksWithLnkErrorsVsLB[NREGIONS_INC_GENERAL];
  TProfile_LW * m_LinksWithRODErrorsVsLB[NREGIONS_INC_GENERAL];

  TProfile_LW * m_LinksWithAnyErrorsVsLBLayer[NREGIONS_INC_GENERAL][SCT_Monitoring::N_DISKSx2];
  TProfile_LW * m_LinksWithBadErrorsVsLBLayer[NREGIONS_INC_GENERAL][SCT_Monitoring::N_DISKSx2];
  TProfile_LW * m_LinksWithLnkErrorsVsLBLayer[NREGIONS_INC_GENERAL][SCT_Monitoring::N_DISKSx2];
  TProfile_LW * m_LinksWithRODErrorsVsLBLayer[NREGIONS_INC_GENERAL][SCT_Monitoring::N_DISKSx2];

  TH1F_LW * m_NumberOfSCTFlagErrorsVsLB;
  TH1F_LW * m_NumberOfEventsVsLB;

  TProfile * m_ConfEffOnline;
  TProfile_LW * m_ConfNoiseOnline;
  TProfile_LW * m_ConfNoiseOnlineRecent;
  TProfile_LW * m_DetailedConfiguration;


  /** a handle on the Hist/TTree registration service */
  ServiceHandle<ITHistSvc> m_thistSvc;
  ServiceHandle<ISCT_ByteStreamErrorsSvc> m_byteStreamErrSvc;
  //  ServiceHandle<IInDetConditionsSvc>       m_pSummarySvc;
  bool                                     m_checkBadModules;
  bool                                     m_ignore_RDO_cut_online;

  float m_errThreshold;
  float m_effThreshold;
  int m_noiseThreshold;
  bool getHisto(const int lyr, const int reg, const int type, TH2* histo[2]);
  bool getHistoRecent(const int lyr, const int reg, const int type, TH2* histo[2]);
  float calculateNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide);
  float calculateOneSideNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide);
  bool isBarrel(const int moduleNumber);
  bool isEndcap(const int moduleNumber);
  bool isEndcapA(const int moduleNumber);
  bool isEndcapC(const int moduleNumber);
  //@}

  Prof2_t
    prof2Factory(const std::string & name, const std::string & title, const unsigned int&, VecProf2_t & storageVector);
};

#endif
