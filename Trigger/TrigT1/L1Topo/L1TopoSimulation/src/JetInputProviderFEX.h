/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// Based on handling of gFEX objects implemeted in Trigger/TrigT1/TrigT1CTP/src/CTPEmulation by Joerg Stelzer.  


#ifndef L1TopoSimulation_JetInputProviderFEX
#define L1TopoSimulation_JetInputProviderFEX

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"

#include "xAODTrigger/JetRoIContainer.h" //jets from gFEX

class TH1I;
class TH2I;

class ITHistSvc;

namespace LVL1 {

   class JetInputProviderFEX : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      JetInputProviderFEX(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~JetInputProviderFEX();

      virtual StatusCode initialize() override final;
      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const override final; 

      virtual void handle(const Incident&) override final;

   private:

      ServiceHandle<ITHistSvc> m_histSvc;

      StringProperty m_gFEXJetLoc {""};

      TH1I * m_hPt1 {nullptr};
      TH1I * m_hPt2 {nullptr};
      TH2I * m_hEtaPhi {nullptr};
   };
}

#endif
