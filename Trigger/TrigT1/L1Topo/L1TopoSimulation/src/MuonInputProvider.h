/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_MuonInputProvider
#define L1TopoSimulation_MuonInputProvider

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"
#include <vector>

class TH1I;
class TH2I;

class ITHistSvc;

namespace TrigConf {
   class ILVL1ConfigSvc;
   class TriggerThreshold;
}

namespace TCS {
   class MuonTOB;
   class LateMuonTOB;
}

namespace LVL1MUCTPI {
  class IMuctpiSimTool;
}

namespace LVL1 {

   class RecMuonRoiSvc;
   class MuCTPIL1TopoCandidate;

   class MuonInputProvider : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      MuonInputProvider(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~MuonInputProvider();

      virtual StatusCode initialize() override;

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const; 

      virtual void handle(const Incident&);

   private:

      TCS::MuonTOB createMuonTOB(uint32_t roiword) const;
      TCS::MuonTOB createMuonTOB(const MuCTPIL1TopoCandidate & roi) const;
      TCS::LateMuonTOB createLateMuonTOB(const MuCTPIL1TopoCandidate & roi) const;
      /**
         This is a hack to modify the unphysical muon pt values

         The muon TOB should be able to encode pt values only up to 10GeV.
         However, we observe TOBs with larger pt values, leading to
         hdw-sim mismatches for items using the INVM algorithm.
         This hack should be removed once the origin of the problem
         has been identified and fixed.
       */
      TCS::MuonTOB hackMuonTOB(const TCS::MuonTOB &mu) const;

      StringProperty m_roibLocation;

      ServiceHandle<ITHistSvc> m_histSvc;

      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

      ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc;
      ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc;

      ToolHandle<LVL1MUCTPI::IMuctpiSimTool> m_MuctpiSimTool;

      std::vector< TrigConf::TriggerThreshold* > m_MuonThresholds;

      StringProperty m_muonROILocation;    //!<  Muon ROIs SG key

      TH1I * m_hPt {nullptr};
      TH2I * m_hEtaPhi {nullptr};

     int m_MuonEncoding; //!< Use 0 for full granularity; 1 for MuCTPiToTopo muon granularity
     StringProperty m_MuCTPItoL1TopoLocation;
   };
}

#endif
