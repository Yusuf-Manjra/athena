/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TrigTileLookForMuAlg.h
// Author   : G Usai 
// Created  : Jun 2003
//
//****************************************************************************

#ifndef  TRIGTILEMUID_TRIGTILELOOKFORMUALG_H
#define  TRIGTILEMUID_TRIGTILELOOKFORMUALG_H 

// includes 
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/ToolHandle.h"
#include "TileEvent/TileCell.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "TileByteStream/TileCellCont.h" 
#include "TrigT2CaloCommon/ITrigDataAccess.h"

// C++ STL includes
#include <vector> 
#include <string>
#include <math.h>

namespace HLT {
  class TriggerElement;
}
class Identifier;
class TileID;

class TrigTileLookForMuAlg : public HLT::AllTEAlgo{

 public:

  TrigTileLookForMuAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~TrigTileLookForMuAlg();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*input*/, unsigned int output);

 private:

  /** Error associated to data access */
  uint32_t m_error;

  /** counter for conversion error */
  unsigned int m_conversionError;

  /** counter for algorithm error */
  unsigned int m_algorithmError;

  /** Pointer to TileID */
  const TileID * m_tileID;

  /** Pointer to ITrigDataAccess */
  ITrigDataAccess * m_data;

  /** Pointer to TrigTimer */
  TrigTimer *m_timerTotal, *m_timerLink, *m_timerCandSearch, *m_timerLoopOnCand, *m_timerLoopOnCellCollections;

  TileCellCollection::const_iterator m_itBegin;
  TileCellCollection::const_iterator m_itEnd;
  TileCellCollection::const_iterator m_itT;

  /** Eta for D cells */
  double m_eta2[13]; 

  /** Eta for BC cells */
  double m_eta1[30];

  /** Eta for A cells */
  double m_eta0[30]; 

  /** DeltaEta */
  double m_deta2;
  double m_deta1;

  /** Lower energy thresholds */
  double m_th0d;
  double m_th1d;
  double m_th2d;
  double m_thitcd;

  /** Higher energy thresholds */
  std::vector<double> m_thres2;
  std::vector<double> m_thres1;
  std::vector<double> m_thres0;

  /** Pattern search from 3rd to 2nd layer */
  std::vector<int> m_cell32;

  /** Pattern search from 2nd to 1st layer */
  std::vector<int> m_cell21; 

  /** Properties */
  bool m_LooseSelection;
  bool m_ReadRoIsFromL1;
  double m_DelEta_ForRoIs;
  double m_DelPhi_ForRoIs;

  /** Monitored variables */
  int m_allNMuon;  
  int m_NCellsAccess;
  float m_EffEvtswRoI;

  /** Monitored containers */
  std::vector<float> m_allEta;
  std::vector<float> m_allPhi;
  std::vector<float> m_allEne;
  std::vector<float> m_allEta_LS;
  std::vector<float> m_allPhi_LS;
  std::vector<float> m_allEne_LS;
  std::vector<float> m_EffEvtswRoI_LimitedEta;
  std::vector<float> m_EtaTileMu_wRoI;
  std::vector<float> m_EtaRoI_wTileMu;
  std::vector<float> m_PhiTileMu_wRoI;
  std::vector<float> m_PhiRoI_wTileMu;
  std::vector<float> m_EtaRoI_NoTileMu;
  std::vector<float> m_PhiRoI_NoTileMu;
};

#endif // TRIGTILEMUID_TRIGTILELOOKFORMUALG_H
