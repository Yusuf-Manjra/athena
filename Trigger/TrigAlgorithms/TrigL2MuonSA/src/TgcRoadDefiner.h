/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCROADDEFINER_H
#define  TRIGL2MUONSA_TGCROADDEFINER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigMuonToolInterfaces/ITrigMuonBackExtrapolator.h"
#include "PtEndcapLUTSvc.h"
#include "PtEndcapLUT.h"
#include "TgcFit.h"
#include "TgcData.h"
#include "MuonRoad.h"
#include "MdtRegion.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "IRegionSelector/IRegSelTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <string>

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TgcRoadDefiner: public AthAlgTool
{
 public:
  TgcRoadDefiner(const std::string& type, 
		 const std::string& name,
		 const IInterface*  parent);
  
  virtual StatusCode initialize() override;

  StatusCode defineRoad(const LVL1::RecMuonRoI*      p_roi,
                        const bool                   insideOut,
                        const TrigL2MuonSA::TgcHits& tgcHits,
                        TrigL2MuonSA::MuonRoad&      muonRoad,
                        TrigL2MuonSA::TgcFitResult&  tgcFitResult) const;

  void setPtLUT(const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc) { m_ptEndcapLUT = ptEndcapLUTSvc->ptEndcapLUT(); };
  void setRoadWidthForFailure(double rWidth_TGC_Failed) { m_rWidth_TGC_Failed = rWidth_TGC_Failed; };
  void setExtrapolatorTool(ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator) { m_backExtrapolatorTool = backExtrapolator; };

  bool prepareTgcPoints(const TrigL2MuonSA::TgcHits& tgcHits,
                        TrigL2MuonSA::TgcFit::PointArray& tgcStripInnPoints,
                        TrigL2MuonSA::TgcFit::PointArray& tgcWireInnPoints,
                        TrigL2MuonSA::TgcFit::PointArray& tgcStripMidPoints,
                        TrigL2MuonSA::TgcFit::PointArray& tgcWireMidPoints) const;

 private:
  // setted in MuFastSteering::hltInitialize, setExtrapolatorTool
  ToolHandle<ITrigMuonBackExtrapolator>* m_backExtrapolatorTool {nullptr};
  // setted in MuFastSteering::hltInitialize, setMCFlag  
  const ToolHandle<PtEndcapLUT>*         m_ptEndcapLUT {nullptr};

  ToolHandle<TgcFit>                     m_tgcFit {"TrigL2MuonSA::TgcFit"};

  double m_rWidth_TGC_Failed {0};
  
  ToolHandle<IRegSelTool> m_regionSelector{this, "RegionSelectionTool", "RegSelTool/RegSelTool_MDT", "MDT Region Selector Tool"};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
}

#endif // TRIGL2MUONSA_TGCROADDEFINER_H
