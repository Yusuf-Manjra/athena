/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAHYPO_TRIGEGAMMAFASTPHOTONHYPOALGMT_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTPHOTONHYPOALGMT_H 1

#include <string>
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "TrigEgammaFastPhotonHypoToolInc.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "DecisionHandling/HypoBase.h"


/**
 * @class TrigEgammaFastPhotonHypoAlgoMT
 * @brief Implements Hypo selection on L2 photons
 **/

class TrigEgammaFastPhotonHypoAlgMT  :  public ::HypoBase 
{ 
  public: 

    TrigEgammaFastPhotonHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );
    virtual StatusCode  initialize() override;
    virtual StatusCode  execute(const EventContext& context) const override;

  private: 
    ToolHandleArray< TrigEgammaFastPhotonHypoToolInc > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};
    Gaudi::Property< bool > m_runInView { this, "RunInView", false , "Set input DH for running in views" };
    // internally used to getch from views
    SG::ReadHandleKey< xAOD::TrigPhotonContainer > m_photonsKey {this, "Photons", "L2PhotonContainer", "Input"};
  
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGEGAMMAFASTPHOTONHYPOALGMT_H                                                                                                                                                          
