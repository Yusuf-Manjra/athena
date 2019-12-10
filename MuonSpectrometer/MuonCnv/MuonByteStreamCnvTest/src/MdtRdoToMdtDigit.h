/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_MDTRDOTOMDTDIGIT_H
#define MUONBYTESTREAMCNVTEST_MDTRDOTOMDTDIGIT_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "MuonMDT_CnvTools/IMDT_RDO_Decoder.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class MdtRdoToMdtDigit : public AthReentrantAlgorithm {

 public:

  MdtRdoToMdtDigit(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MdtRdoToMdtDigit() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

 private:

  StatusCode decodeMdt( const MdtCsm *, MdtDigitContainer *, MdtDigitCollection*&, Identifier& ) const;

  ToolHandle<Muon::IMDT_RDO_Decoder> m_mdtRdoDecoderTool{this, "mdtRdoDecoderTool", "Muon::MdtRDO_Decoder", ""};
  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
  SG::ReadHandleKey<MdtCsmContainer> m_mdtRdoKey{this, "MdtRdoContainer", "MDTCSM", "Mdt RDO Input"};
  SG::WriteHandleKey<MdtDigitContainer> m_mdtDigitKey{this, "MdtDigitContainer", "MDT_DIGITS", "Mdt Digit Output"};
};

#endif

