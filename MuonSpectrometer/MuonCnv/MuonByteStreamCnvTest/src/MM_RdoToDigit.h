/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_MMRDOTOMMDIGIT_H
#define MUONBYTESTREAMCNVTEST_MMRDOTOMMDIGIT_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "MuonMM_CnvTools/IMM_RDO_Decoder.h"
#include "MuonRDO/MM_RawDataContainer.h"
#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class MM_RdoToDigit : public AthReentrantAlgorithm {

 public:

  MM_RdoToDigit(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MM_RdoToDigit() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

 private:

  StatusCode decodeMM( const Muon::MM_RawDataCollection *, MmDigitContainer *, MmDigitCollection*&, Identifier& ) const;

  ToolHandle<Muon::IMM_RDO_Decoder> m_mmRdoDecoderTool{this, "mmRdoDecoderTool", "Muon::MM_RDO_Decoder", ""};
  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
  SG::ReadHandleKey<Muon::MM_RawDataContainer> m_mmRdoKey{this, "MmRdoContainer", "MMRDO", "MM RDO Input"};
  SG::WriteHandleKey<MmDigitContainer> m_mmDigitKey{this, "MmDigitContainer", "MM_DIGITS", "MM Digit Output"};
};

#endif

