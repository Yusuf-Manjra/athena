/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGONLINEMONITOR_TRIGL1TOPOROBMONITOR_H
#define TRIGONLINEMONITOR_TRIGL1TOPOROBMONITOR_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/HistoProperty.h"
#include <vector>
#include <map>

class ITHistSvc;
class IROBDataProviderSvc;
class ITrigROBDataProviderSvc;
class TH1F;    /// for monitoring purposes
class TH2F;    /// for monitoring purposes
class TProfile;/// for monitoring purposes

namespace L1Topo {
  class Header;
  class L1TopoTOB;
}

namespace HLT {
  class IScaler;
}

namespace TrigConf {
  class IL1TopoConfigSvc;
}

class TrigL1TopoROBMonitor:public AthAlgorithm {
 public:
  TrigL1TopoROBMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode beginRun();  
  StatusCode endRun();
 private:
  StatusCode doCnvMon(bool);
  StatusCode doRawMon(bool);
  StatusCode doSimMon(bool);
  StatusCode monitorROBs(const std::vector<uint32_t>&, bool);
  StatusCode monitorBlock(uint32_t sourceID, L1Topo::Header& header, std::vector<uint32_t>& vFibreSizes, std::vector<uint32_t>& vFibreStatus, std::vector<L1Topo::L1TopoTOB>& daqTobs);
  StatusCode bookAndRegisterHist(ServiceHandle<ITHistSvc>&, TH1F*& , const Histo1DProperty& prop, std::string extraName, std::string extraTitle);
  StatusCode bookAndRegisterHist(ServiceHandle<ITHistSvc>&, TH1F*& , std::string hName, std::string hTitle, int bins, float lowEdge, float highEdge);
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc;
  ServiceHandle<TrigConf::IL1TopoConfigSvc> m_l1topoConfigSvc;
  BooleanProperty m_doRawMon;
  BooleanProperty m_doCnvMon;
  BooleanProperty m_doSimMon;
  UnsignedIntegerArrayProperty m_vDAQROBIDs;
  UnsignedIntegerArrayProperty m_vROIROBIDs;
  UnsignedIntegerProperty m_prescaleForDAQROBAccess;
  StringProperty m_simTopoCTPLocation;
  HLT::IScaler* m_scaler; //! prescale decision tool
  BooleanProperty m_useDetMask;
  std::map<unsigned int,unsigned int> m_allSIDLabelsToInts;
  //Histo1DProperty m_histPropNoBins; // obsolete?
  TH1F* m_histSIDsViaConverters;
  TH1F* m_histSIDsDirectFromROBs;
  TH1F* m_histCTPSignalPartFromROIConv;
  TH1F* m_histOverflowBitsFromROIConv;
  TH1F* m_histTriggerBitsFromROIConv;
  TH1F* m_histPayloadCRCFromDAQConv;
  TH1F* m_histFibreStatusFlagsFromDAQConv;
  TH1F* m_histTOBCountsFromROIROB;
  TH1F* m_histTOBCountsFromDAQROB;
  TH1F* m_histPayloadSizeDAQROB;
  TH1F* m_histPayloadSizeROIROB;
  TH1F* m_histBCNsFromDAQConv;
  TH1F* m_histTopoSimHdwStatComparison;
  TH1F* m_histTopoSimHdwEventComparison;
  TH1F* m_histTopoSimResult;
  TH1F* m_histTopoHdwResult;
  TH1F* m_histTopoProblems;
  static const unsigned int m_nTopoCTPOutputs = 128; //! Number of CTP outputs, used for histogram ranges and loops
  std::bitset<m_nTopoCTPOutputs> m_triggerBits; //! trigger bits sent to CTP
  std::bitset<m_nTopoCTPOutputs> m_overflowBits; //! overflow bits corresponding to CTP output
  std::bitset<m_nTopoCTPOutputs> m_topoSimResult; //! simulation of CTP output
  enum class Problems {
    ROI_NO_RDO=0,
    ROI_CNV_ERR,
    ROI_PAYLOAD_EMPTY,
    ROI_BAD_TOB,
    DAQ_NO_RDO,
    DAQ_COLL_EMPTY,
    DAQ_CNV_ERR,
    DAQ_PAYLOAD_EMPTY,
    DAQ_BAD_TOB,
    FIBRE_OVERFLOW,
    FIBRE_CRC
  }; //! used to assign an integer ID and histogram bin to various types of problem that arise and should be reported.
  const std::vector<std::string> m_problems = {
    "ROI no RDO",
    "ROI converter error",
    "ROI payload empty",
    "ROI invalid TOB type",
    "DAQ no RDO",
    "DAQ collection empty",
    "DAQ converter error",
    "DAQ payload empty",
    "DAQ invalid TOB type",
    "Fibre overflow",
    "Fibre CRC"
  }; //! bin labels for problem histograms
};

#endif // TRIGONLINEMONITOR
