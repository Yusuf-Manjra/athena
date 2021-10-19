// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_TopoInputEvent
#define L1TopoEvent_TopoInputEvent

#include <iostream>
#include <vector>

#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/eEmTOBArray.h"
#include "L1TopoEvent/eTauTOBArray.h"
#include "L1TopoEvent/cTauTOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/jTauTOBArray.h"
#include "L1TopoEvent/jLargeRJetTOBArray.h"
#include "L1TopoEvent/jJetTOBArray.h"
#include "L1TopoEvent/LateMuonTOBArray.h"
#include "L1TopoEvent/MuonNextBCTOBArray.h"
#include "L1TopoEvent/MuonTOBArray.h"
#include "L1TopoEvent/MetTOBArray.h"
#include "L1TopoCommon/StatusCode.h"
#include "L1TopoCommon/Types.h"

#include "TrigConfBase/TrigConfMessaging.h"

namespace TCS {
   class TopoInputEvent;
}

namespace TCS {

  std::ostream & operator<<(std::ostream &, const TCS::TopoInputEvent &);

   class TopoInputEvent : public TrigConf::TrigConfMessaging {
   public:
      
      TopoInputEvent();
      ~TopoInputEvent();
      
      const ClusterTOBArray & clusters() const { return m_clusters; }
      const eEmTOBArray & eems() const { return m_eems; }
      const eTauTOBArray & etaus() const { return m_etaus; }
      const cTauTOBArray & ctaus() const { return m_ctaus; }
      const ClusterTOBArray & taus() const { return m_taus; }
      const MuonTOBArray & muons() const { return m_muons; }
      const LateMuonTOBArray & lateMuons() const { return m_lateMuons; }
      const MuonNextBCTOBArray & muonsNextBC() const { return m_muonsNextBC; }
      const JetTOBArray & jets() const { return m_jets; }
      const jTauTOBArray & jTaus() const { return m_jTaus; }
      const jLargeRJetTOBArray & jLrageRJets() const { return m_jLargeRJets; }
      const jJetTOBArray & jJets() const { return m_jJets; }
      const MetTOB & met() const { return m_met[0]; }
      uint32_t run_number()        const { return m_runNo; }
      uint32_t event_number()      const { return m_evtNo; }
      uint32_t lumi_block()        const { return m_lumiB; }
      uint32_t bunch_crossing_id() const { return m_BCID;  }
      /** @defgroup groupOverflowGetters
       *  @brief getter function for overflow bits
       *  @{
       */
      bool overflowFromEmtauInput () const { return m_overflowFromEmtauInput ; }
      bool overflowFromJetInput   () const { return m_overflowFromJetInput   ; }
      bool overflowFromjTauInput   () const { return m_overflowFromjTauInput   ; }
      bool overflowFromjLargeRJetInput   () const { return m_overflowFromjLargeRJetInput   ; }
      bool overflowFromjJetInput   () const { return m_overflowFromjJetInput   ; }
      bool overflowFromEnergyInput() const { return m_overflowFromEnergyInput; }
      bool overflowFromMuonInput  () const { return m_overflowFromMuonInput  ; }
      /** @} */ // end of groupOverflowGetters
      StatusCode addCluster(const ClusterTOB & cluster);
      StatusCode addeEm(const eEmTOB & eem);
      StatusCode addeTau(const eTauTOB & etau);
      StatusCode addcTau(const eTauTOB & etau);
      StatusCode addcTau(const jTauTOB & jtau);
      StatusCode addTau(const ClusterTOB & tau);
      StatusCode addMuon(const MuonTOB & muon);
      StatusCode addLateMuon(const LateMuonTOB & muon);
      StatusCode addMuonNextBC(const MuonNextBCTOB & muon);
      StatusCode addJet(const JetTOB & jet);
      StatusCode addjTau(const jTauTOB & tau);
      StatusCode addjLargeRJet(const jLargeRJetTOB & jet);
      StatusCode addjJet(const jJetTOB & jet);
      StatusCode setMET(const MetTOB & met);
      StatusCode setEventInfo(const uint32_t runNo, const uint32_t evtNo, const uint32_t lumiB, const uint32_t BCID);
      /** @defgroup groupOverflowSetters
       *  @brief setter function for overflow bits
       *  The bits are being propagated from the input providers.
       *  @{
       */
      void setOverflowFromEmtauInput (const bool &v);
      void setOverflowFromJetInput   (const bool &v);
      void setOverflowFromjTauInput   (const bool &v);
      void setOverflowFromjLargeRJetInput   (const bool &v);
      void setOverflowFromjJetInput   (const bool &v);
      void setOverflowFromEnergyInput(const bool &v);
      void setOverflowFromMuonInput  (const bool &v);
      /** @} */ // end of groupOverflowSetters
      // access all inputs by type
      const InputTOBArray * inputTOBs(TCS::inputTOBType_t) const;
      /// @brief whether there are input overflows from Mioct for muon and from CMX for calo
      bool hasInputOverflow(TCS::inputTOBType_t) const;

      // to be calles after each event
      StatusCode clear();

      // to enable dumping of the input after each event
      void enableInputDump( const std::string& filename, bool enable = true );

      // to trigger the writing of the event
      void dumpStart();
      void dump();
      void dumpFinish();

      void print() const;

   private:
      friend std::ostream & operator<<(std::ostream &, const TCS::TopoInputEvent &);

      // data is owned by the TopoInputEvent
      ClusterTOBArray   m_clusters;
      eEmTOBArray       m_eems;
      eTauTOBArray      m_etaus;
      cTauTOBArray      m_ctaus;
      ClusterTOBArray   m_taus;
      JetTOBArray       m_jets;
      jTauTOBArray      m_jTaus;
      jLargeRJetTOBArray      m_jLargeRJets;
      jJetTOBArray      m_jJets;
      MuonTOBArray      m_muons;
      LateMuonTOBArray  m_lateMuons;
      MuonNextBCTOBArray  m_muonsNextBC;
      MetTOBArray       m_met; // will have size 1

      uint32_t m_runNo {0};
      uint32_t m_evtNo {0};
      uint32_t m_lumiB {0};
      uint32_t m_BCID {0};

      bool m_dumpEnabled { false };
      bool m_overflowFromEmtauInput { false };
      bool m_overflowFromJetInput { false };
      bool m_overflowFromjTauInput { false };
      bool m_overflowFromjLargeRJetInput { false };
      bool m_overflowFromjJetInput { false };
      bool m_overflowFromEnergyInput { false };
      bool m_overflowFromMuonInput { false };
      std::string m_inputDumpFile { "" };

   };
   
}


#endif
