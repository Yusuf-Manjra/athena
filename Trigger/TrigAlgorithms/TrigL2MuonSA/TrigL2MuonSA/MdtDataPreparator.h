/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MDTDATAPREPARATOR_H
#define  TRIGL2MUONSA_MDTDATAPREPARATOR_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TGCgeometry/TGCgeometrySvc.h" // necessary to avoid compiler warning
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "MDTcabling/IMDTcablingSvc.h"
#include "RegionSelector/IRegSelSvc.h"
#include "Identifier/IdentifierHash.h"
#include "MuonRDO/MdtCsmContainer.h"

#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/MdtData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"
#include "TrigL2MuonSA/MdtRegionDefiner.h"

#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/TgcFitResult.h"

#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"

class StoreGateSvc;
class MdtIdHelper;
namespace MuonGM{
     class MuonDetectorManager;
     class MdtReadoutElement;
     class MuonStation;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {
  
  class MdtDataPreparator: public AlgTool
  {
  public:
    
    static const InterfaceID& interfaceID();
    
  public:
    
    MdtDataPreparator(const std::string& type, 
		      const std::string& name,
		      const IInterface*  parent);
    
    ~MdtDataPreparator();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();
    
  public:
    
    StatusCode prepareData(const LVL1::RecMuonRoI*           p_roi,
			   const TrigL2MuonSA::RpcFitResult& rpcFitResult,
			   TrigL2MuonSA::MuonRoad&           muonRoad,
			   TrigL2MuonSA::MdtRegion&          mdtRegion,
			   TrigL2MuonSA::MdtHits&            mdtHits_normal,
			   TrigL2MuonSA::MdtHits&            mdtHits_overlap);

    StatusCode prepareData(const LVL1::RecMuonRoI*           p_roi,
			   const TrigL2MuonSA::TgcFitResult& tgcFitResult,
			   TrigL2MuonSA::MuonRoad&           muonRoad,
			   TrigL2MuonSA::MdtRegion&          mdtRegion,
			   TrigL2MuonSA::MdtHits&            mdtHits_normal,
			   TrigL2MuonSA::MdtHits&            mdtHits_overlap);

    void setGeometry(bool use_new_geometry);
    void setRpcGeometry(bool use_rpc);

  private:
    
    MsgStream* m_msg;
    inline MsgStream& msg() const { return *m_msg; }
    
    StatusCode getMdtHits(const LVL1::RecMuonRoI* p_roi,
			  const TrigL2MuonSA::MdtRegion& mdtRegion,
			  TrigL2MuonSA::MuonRoad& muonRoad,
			  TrigL2MuonSA::MdtHits& mdtHits_normal,
			  TrigL2MuonSA::MdtHits& mdtHits_overlap);

    void getMdtIdHashesBarrel(const TrigL2MuonSA::MdtRegion& mdtRegion,
			std::vector<IdentifierHash>& mdtIdHashes_normal,
			std::vector<IdentifierHash>& mdtIdHashes_overlap);
    
    void getMdtIdHashesEndcap(const TrigL2MuonSA::MdtRegion& mdtRegion,
			std::vector<IdentifierHash>& mdtIdHashes_normal,
			std::vector<IdentifierHash>& mdtIdHashes_overlap);
    
    StatusCode getMdtCsm(const MdtCsmContainer* pMdtCsmContainer,
			 const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& v_robFragments,
			 const std::vector<IdentifierHash>& v_idHash,
			 std::vector<const MdtCsm*>& v_mdtCsms);
    
    bool decodeMdtCsm(const MdtCsm* csm, TrigL2MuonSA::MdtHits& mdtHits, const TrigL2MuonSA::MuonRoad& muonRoad);
    uint32_t get_system_id (unsigned short int SubsystemId) const;
    
  public:
    
    inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }
    
  private:

    // Reference to StoreGateSvc;
    ServiceHandle<StoreGateSvc>    m_storeGateSvc;
    
    // Tools for the Raw data conversion
    ToolHandle<Muon::IMuonRawDataProviderTool>  m_mdtRawDataProvider;
    
    // Cabling
    ServiceHandle<IMDTcablingSvc>  m_mdtCablingSvcOld;
    // Cabling (new)
    MuonMDT_CablingSvc* m_mdtCabling;
    
    // Geometry Services
    const MuonGM::MuonDetectorManager* m_muonMgr;
    const MuonGM::MdtReadoutElement* m_mdtReadout;
    const MuonGM::MuonStation* m_muonStation;
    const MdtIdHelper* m_mdtIdHelper;
    IdentifierHash hash_id;
    
    // Region Selector
    IRegSelSvc*          m_regionSelector;
    
    // ROB DataProvider
    ROBDataProviderSvc*  m_robDataProvider;
    
    // Utils
    TrigL2MuonSA::RecMuonRoIUtils m_recMuonRoIUtils;
   
    BooleanProperty m_use_new_geometry;

    //
    TrigL2MuonSA::MdtRegionDefiner*  m_mdtRegionDefiner;
  };
  
} // namespace TrigL2MuonSA

#endif  // 
