/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaTLAPhotonHypoTool_H
#define TrigEgammaTLAPhotonHypoTool_H

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/PhotonContainer.h"

class TrigEgammaTLAPhotonHypoTool: public AthAlgTool{

  public:
    TrigEgammaTLAPhotonHypoTool(const std::string& type,
      const std::string& name,
      const IInterface* parent);
    ~TrigEgammaTLAPhotonHypoTool();
    StatusCode initialize();
    StatusCode finalize();

    //using PhotonDecision = std::pair<const xAOD::Photon*, TrigCompositeUtils::Decision*>;
   
    StatusCode decide(TrigCompositeUtils::DecisionContainer* decisionContainer) const;

    const HLT::Identifier& getId() const;

  private:
    HLT::Identifier m_decisionId;
    Gaudi::Property<std::string> m_chainName {
      this, "chain_name", {}, "chain name"
    };
};

#endif
