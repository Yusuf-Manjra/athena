/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MDTTESTCABLING_H
#define MUONMDT_CABLING_MDTTESTCABLING_H

/***********************************************
 *
 * @class MdtTestCabling
 *
 * Testing algorithm for the Muon MDT mapping class
 *
 * @author Stefano Rosati Stefano.Rosati@roma1.infn.it
 *
 ***********************************************/

#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "StoreGate/ReadCondHandleKey.h"


class MdtTestCabling : public AthAlgorithm {

 public:

  MdtTestCabling(const std::string& name, ISvcLocator* pSvcLocator);
  ~MdtTestCabling();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

 private:

  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};

  // test initialize function
  bool initTestMap();

  bool testMapTiming();

  bool testMap();

  // the chrono service
  ServiceHandle<IChronoStatSvc> m_chronoSvc;

  std::string m_chrono1;
  std::string m_chrono2;
  std::string m_chrono3;
  std::string m_chrono4;

  SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};

};


#endif     // MUONMDT_CABLING_MDTTESTCABLING_H
