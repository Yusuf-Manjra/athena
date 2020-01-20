/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTTOOL_H
#define TESTTOOL_H

#include "MuonPrdSelector/IMuonIdCutTool.h"


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "Identifier/Identifier.h"
#include <vector>
#include <set>
#include "MuonCalibTools/IdToFixedIdTool.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class MsgStream;
class MuonStationIntersectSvc;


  /** 
      Implementation of an IMuonIdCutTool. 

      For more details look at the mainpage of this package.
  */
class MuonIdCutTool :  virtual public IMuonIdCutTool, public AthAlgTool   {
  

  public:
    /** default AlgTool constructor */
    MuonIdCutTool(const std::string&, const std::string&, const IInterface*);
    
    /** destructor */
    virtual ~MuonIdCutTool();
    
    /** initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** finialize method, method taken from bass-class AlgTool */
    StatusCode finalize();
    
    virtual bool isCut(Identifier ID) const;
    virtual bool isCut(MuonCalib::MuonFixedId id) const;
    
    bool m_cutStationRegion;
    bool m_cutStationName;
    bool m_cutSubstation;
    bool m_cutEE;
    bool m_cutEta;
    bool m_cutPhi;

    std::vector<std::string> m_EELList; //list of EEL chambers to keep
    std::vector<int>    m_mdtRegionList;
    std::vector<int>    m_cscRegionList;
    std::vector<int>    m_rpcRegionList;
    std::vector<int>    m_tgcRegionList;

    std::vector<int>    m_mdtStationNameList;
    std::vector<int>    m_cscStationNameList;
    std::vector<int>    m_rpcStationNameList;
    std::vector<int>    m_tgcStationNameList;

    std::vector<int>    m_mdtMultilayerList;
    std::vector<int>    m_rpcDoubletRList;
    std::vector<int>    m_rpcGasGapList;
    std::vector<int>    m_tgcGasGapList;

    std::vector<int>    m_mdtBarEtaList;
    std::vector<int>    m_mdtEndEtaList;
    std::vector<int>    m_mdtSectorList;
    std::vector<int>    m_rpcEtaList;
    std::vector<int>    m_rpcSectorList; 
    std::vector<int>    m_cscEtaList;
    std::vector<int>    m_cscSectorList;    

    std::vector<int>    m_tgcEtaList;
    std::vector<int>    m_tgcEndPhiList;    
    std::vector<int>    m_tgcForPhiList;       

  private:
 
    int FindSector(unsigned int staName, unsigned int staPhi) const;



    ToolHandle<MuonCalib::IIdToFixedIdTool>        m_idToFixedIdTool;     //<! tool to assist with Identifiers

    ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
      "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};

    std::vector<int> m_EELeta;
    std::vector<int> m_EELsector;


  };





#endif
