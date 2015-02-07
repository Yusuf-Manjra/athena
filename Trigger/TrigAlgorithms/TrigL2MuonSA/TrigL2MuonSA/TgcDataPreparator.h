/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCDATAPREPARATOR_H
#define  TRIGL2MUONSA_TGCDATAPREPARATOR_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TGCgeometry/TGCgeometrySvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "TrigL2MuonSA/TgcDataPreparatorOptions.h"
#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"
#include "RegionSelector/IRegSelSvc.h"

#include "MuonTGC_Cabling/MuonTGC_CablingSvc.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

class StoreGateSvc;
class TgcIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
  class TgcReadoutElement;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {

class TgcDataPreparator: public AlgTool
{
 public:
  struct tgcRawData
  {
    unsigned short int bcTag;
    unsigned short int sswId;
    unsigned short int slbId;
    unsigned short int bitpos;
  };

  public:
      
      static const InterfaceID& interfaceID();

   public:

      TgcDataPreparator(const std::string& type, 
			const std::string& name,
			const IInterface*  parent);
    
      ~TgcDataPreparator();
    
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();
    
      void setGeometry(bool use_new_geometry);

      StatusCode prepareData(const LVL1::RecMuonRoI*  p_roi,
			     TrigL2MuonSA::TgcHits&   tgcHits);

      inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }
      inline void setMsgLvl(const MSG::Level& level) { if(m_msg != 0) m_msg->setLevel(level); }
      void setOptions(const TrigL2MuonSA::TgcDataPreparatorOptions& options) { m_options = options; };

   private:

      MsgStream* m_msg;
      inline MsgStream& msg() const { return *m_msg; }

      // internal methods
      StatusCode getTgcRdo(unsigned int robId, float eta, float phi, const TgcRdo*& p_rdo);
      StatusCode decodeTgcRdo(const TgcRdo* p_rdo, double roiEta, double roiPhi, bool isLowPt,
			      TrigL2MuonSA::TgcHits& tgcHits);

   private:

      // Reference to StoreGateSvc;
      ServiceHandle<StoreGateSvc>    m_storeGateSvc;

      const MuonGM::MuonDetectorManager* m_muonMgr;
      const MuonGM::TgcReadoutElement* m_tgcReadout;
      const TgcIdHelper* m_tgcIdHelper;
      ActiveStoreSvc* m_activeStore;
      // Tool for Rdo to Prep Data conversion
      ToolHandle<Muon::IMuonRdoToPrepDataTool> m_tgcPrepDataProvider;
	
      // Cabling (new)
      MuonTGC_CablingSvc* m_tgcCabling;	

      // Tools for the Raw data conversion
      ToolHandle<Muon::IMuonRawDataProviderTool>  m_tgcRawDataProvider;

      // TGC raw data
      std::vector<tgcRawData> m_tgcRawData;

      // L2-specific Geometry Svc
      ServiceHandle<TGCgeometrySvc>  m_tgcGeometrySvc;

      // Region Selector
      IRegSelSvc*          m_regionSelector;

      // ROB DataProvider
      ROBDataProviderSvc*  m_robDataProvider;

      // Switch to use new geometry
      BooleanProperty m_use_new_geometry;

      // option
      TrigL2MuonSA::TgcDataPreparatorOptions m_options;

      // utils
      TrigL2MuonSA::RecMuonRoIUtils m_recMuonRoIUtils;
};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_TGCDATAPREPARATOR_H
