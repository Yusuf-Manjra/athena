/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONGEOMODEL_MUONDETECTORCONDALG_H
#define MUONGEOMODEL_MUONDETECTORCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "GeoModelInterfaces/IGeoModelTool.h"
#include "MuonAlignmentData/CorrContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

class MuonDetectorCondAlg : public AthAlgorithm {

public:

  // Standard Constructor
  MuonDetectorCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

  // Standard Destructor
  virtual ~MuonDetectorCondAlg() = default;

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;


private:
  
  ServiceHandle<ICondSvc> m_condSvc;
  ToolHandle<IGeoModelTool> m_iGeoModelTool {this, "SelectionTool", "MuonDetectorTool", "The selection tool"};

  bool m_applyCscIntAlignment = false;
  bool m_applyMdtAsBuiltParams = false;

// Read Handles
  SG::ReadCondHandleKey<ALineMapContainer> m_readALineKey                {this, "ReadALineKey", 
                                                                                "ALineMapContainer", 
                                                                                "Key of input muon alignment ALine condition data"};    
  SG::ReadCondHandleKey<BLineMapContainer> m_readBLineKey                {this, "ReadBLineKey", 
                                                                                "BLineMapContainer", 
                                                                                "Key of input muon alignment BLine condition data"};    
  SG::ReadCondHandleKey<CscInternalAlignmentMapContainer> m_readILineKey {this, "ReadILineKey",
                                                                                "CscInternalAlignmentMapContainer",
                                                                                "Key of input muon alignment CSC/ILine condition data"};
  SG::ReadCondHandleKey<MdtAsBuiltMapContainer> m_readAsBuiltKey         {this, "ReadAsBuiltKey",
                                                                                "MdtAsBuiltMapContainer",
                                                                                "Key of output muon alignment MDT/AsBuilt condition data"};

// Write Handle
  SG::WriteCondHandleKey<MuonGM::MuonDetectorManager> m_writeDetectorManagerKey {this, "WriteDetectorManagerKey",
      "MuonDetectorManager",
      "Key of output MuonDetectorManager condition data"};
  
};

#endif
