/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

/**
   Algorithm to test the validity of the MDT cabling
*/

#ifndef MUONMDT_CABLING_MDTCABLINGMEZZALG_H
#define MUONMDT_CABLING_MDTCABLINGMEZZALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"



class MdtCablMezzAlg : public AthAlgorithm {
public:
    MdtCablMezzAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~MdtCablMezzAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual unsigned int cardinality() const override final{return 1;}

    using CablingData = MuonMDT_CablingMap::CablingData;

private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    // MuonDetectorManager from the conditions store
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                                "Key of input MuonDetectorManager condition data"};

    SG::ReadCondHandleKey<MuonMDT_CablingMap> m_cablingKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of input MDT cabling map"};

    Gaudi::Property<std::string> m_summaryTxt{this, "SummaryFile", "SummaryFile.txt", "Summary of the extracted mapping"};
    Gaudi::Property<std::string> m_mezzJSON{this, "OutMezzanineJSON", "MezzMapping.json", "Mezzanine JSON"};
    Gaudi::Property<std::string> m_cablingJSON{this, "OutCablingJSON", "MdtCabling.json", "Cabling JSON"};
    
};

#endif
