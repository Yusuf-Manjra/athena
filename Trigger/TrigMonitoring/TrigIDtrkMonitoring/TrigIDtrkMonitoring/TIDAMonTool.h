// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef __TIDAMONTOOL_H
#define __TIDAMONTOOL_H


#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysis/Efficiency.h"

#include "TrigInDetAnalysisUtils/Filter_Track.h"
#include "TrigInDetAnalysisUtils/Filter_RoiSelector.h"
#include "TrigInDetAnalysisUtils/T_AnalysisConfig.h"
#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"
#include "TrigInDetAnalysisUtils/TrackMatchDeltaR.h"
#include "TrigInDetAnalysisUtils/TrackMatchDeltaRCosmic.h"


#include "TrigInDetAnalysisExample/Analysis_Tier0.h"
#include "TrigInDetAnalysisExample/AnalysisConfig_Tier0.h"

#include "TrigInDetAnalysisExample/SigAnalysis.h"
#include "TrigInDetAnalysisExample/TrackEfficiency.h"


class TIDAMonTool : 
  public IHLTMonTool  {  
  
public:


  TIDAMonTool( const std::string & type, const std::string & name, const IInterface* parent); 
  
  ~TIDAMonTool();
  
  StatusCode init();
 
#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode book();
#else
  StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun); 
#endif

  StatusCode fill();

#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode proc();
#else
  StatusCode proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun);
#endif
  
  
  // histogram registration

  virtual void addHistogram( TH1* h ) { 
    ATH_MSG_DEBUG("TIDAMonTool::addHistogram() ");
    IHLTMonTool::addHistogram( h ); 
  }

  // track selector cuts

  // test tracks
  double  m_pTCut;
  double  m_etaCut;
  double  m_d0Cut;
  double  m_z0Cut;

  int  m_siHits; // total number of si hits
  int m_pixHits; // pixel hits
  int m_sctHits; // sct hits

  int   m_trtHits; // high threshold hits
  int m_strawHits; // total number of straws
 
  // reference tracks
  double  m_tauEtCutOffline;
  double  m_doTauThreeProng;
  double  m_pTCutOffline;
  double m_etaCutOffline;
  double  m_d0CutOffline;
  double  m_z0CutOffline;
 
  int  m_siHitsOffline; // total number of si hits
  int m_pixHitsOffline; // pixel hits
  int m_sctHitsOffline; // sct hits

  int   m_trtHitsOffline; // high threshold hits
  int m_strawHitsOffline; // total number of straws

  // roi size 
  double m_phiWidth;
  double m_etaWidth;
  double m_zedWidth;

  // matching parameters
  double m_matchR;   // for DeltaR matcher
  double m_matchPhi; // for DeltaPhi matcher

  ToolHandle<Trig::TrigDecisionTool> m_tdt;

  TIDARoiDescriptor m_roiInfo;
  
  std::vector<TrackFilter*>  m_filters;
  std::vector<TrackAssociator*>                 m_associators;

  std::vector<T_AnalysisConfig<IHLTMonTool>*>   m_sequences;

  std::vector<std::string> m_chainNames;
  std::vector<std::string> m_ntupleChainNames;

  bool m_buildNtuple;
  bool m_mcTruth;
  
  std::string m_analysis_config;
  std::string m_outputFileName;

  bool m_genericFlag;
  bool m_first_time;
};


#endif //  __TIDAMONTOOL_H
