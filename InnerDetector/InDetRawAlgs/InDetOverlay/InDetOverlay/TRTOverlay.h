/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETOVERLAY_TRTOVERLAY_H
#define INDETOVERLAY_TRTOVERLAY_H

#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <InDetRawData/TRT_RDO_Container.h>

#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "TRT_ElectronPidTools/ITRT_LocalOccupancy.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h"

class TRT_ID;

namespace CLHEP {
  class HepRandomEngine;
}

class TRTOverlay : public AthReentrantAlgorithm
{
public:

  TRTOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:

  StatusCode overlayContainer(const TRT_RDO_Container *bkgContainer,
                              const TRT_RDO_Container *signalContainer,
                              TRT_RDO_Container *outputContainer,
                              const InDetSimDataCollection *signalSDOCollection) const;

  void mergeCollections(TRT_RDO_Collection *bkgCollection,
                        TRT_RDO_Collection *signalCollection,
                        TRT_RDO_Collection *outputCollection,
                        double occupancy,
                        const InDetSimDataCollection *signalSDOCollection,
                        CLHEP::HepRandomEngine *rndmEngine) const;

  const TRT_ID *m_trtId{};

  SG::ReadHandleKey<TRT_RDO_Container> m_bkgInputKey{this, "BkgInputKey", "Bkg_TRT_RDOs"," ReadHandleKey for Background Input TRT_RDO_Container"};
  SG::ReadHandleKey<TRT_RDO_Container> m_signalInputKey{this, "SignalInputKey", "Sig_TRT_RDOs", "ReadHandleKey for Signal Input TRT_RDO_Container"};
  SG::WriteHandleKey<TRT_RDO_Container> m_outputKey{this, "OutputKey", "TRT_RDOs", "WriteHandleKey for Output TRT_RDO_Container"};
  SG::ReadHandleKey<InDetSimDataCollection> m_signalInputSDOKey{this, "SignalInputSDOKey", "Sig_TRT_SDO_Map", "ReadHandleKey for Signal Input InDetSimDataCollection for TRT"};

  BooleanProperty m_includeBkg { this, "includeBkg", true, "Include Background RDO Container" };

  // Following tools, services and configurables are there only for the correct of HT hits
  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", "Random Number Service"};      // Random number service

  double                                 m_HTOccupancyCorrectionB;
  double                                 m_HTOccupancyCorrectionEC;
  double                                 m_HTOccupancyCorrectionB_noE;
  double                                 m_HTOccupancyCorrectionEC_noE;
  ToolHandle<InDet::ITRT_LocalOccupancy> m_TRT_LocalOccupancyTool{this, "TRT_LocalOccupancyTool", "TRT_LocalOccupancy", ""};
  ToolHandle<ITRT_StrawStatusSummaryTool>   m_TRTStrawSummaryTool{this, "TRTStrawSummaryTool", "TRT_StrawStatusSummaryTool", ""};

};

#endif // INDETOVERLAY_TRTOVERLAY_H
