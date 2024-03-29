/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALIGNCONDATHTEST
#define ALIGNCONDATHTEST

#include <fstream>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class AlignCondAthTest : public AthAlgorithm {
public:
    AlignCondAthTest(const std::string& name, ISvcLocator* pSvcLocator);

public:
    StatusCode initialize() override;
    StatusCode execute() override;

private:
    const MuonGM::MuonDetectorManager* m_MuonDetMgrDS = nullptr;  // nominal MuonDetectorManager (no alignment) from the DetectorStore
                                                                  // (needed in this test alg to compare against the ConditionsObject)
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                            "Key of input MuonDetectorManager condition data"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    Gaudi::Property<bool> m_alinePrint{this, "ALinePrint", true};
    Gaudi::Property<bool> m_blinePrint{this, "BLinePrint", true};
    Gaudi::Property<bool> m_mdtPrint{this, "MdtPrint", true};
    Gaudi::Property<bool> m_rpcPrint{this, "RpcPrint", true};
    Gaudi::Property<bool> m_tgcPrint{this, "TgcPrint", true};
    Gaudi::Property<bool> m_cscPrint{this, "CscPrint", true};

    StatusCode checkALines(const MuonGM::MuonDetectorManager* manager, std::ofstream& fout);
    StatusCode checkBLines(const MuonGM::MuonDetectorManager* manager, std::ofstream& fout);
    StatusCode checkMdtGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream& fout);
    StatusCode checkRpcGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream& fout);
    StatusCode checkTgcGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream& fout);
    StatusCode checkCscGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream& fout);
};

#endif
