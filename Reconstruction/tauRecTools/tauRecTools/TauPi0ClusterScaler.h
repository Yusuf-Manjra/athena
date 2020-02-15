/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0CLUSTERSCALER_H
#define TAUREC_TAUPI0CLUSTERSCALER_H

#include <string>
#include "tauRecTools/TauRecToolBase.h"
#include "AsgTools/ToolHandle.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOAuxContainer.h"

/**
 * @brief scale cluster energy to take care of charged pion energy
 *
 * @author Stephanie Yuen <stephanie.yuen@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Will Davey <will.davey@cern.ch> 
 */

//namespace Trk {
//    class IParticleCaloExtensionTool;
//}
class TauPi0ClusterScaler : virtual public TauRecToolBase {
public:
    TauPi0ClusterScaler(const std::string& name);
    ASG_TOOL_CLASS2(TauPi0ClusterScaler, TauRecToolBase, ITauToolBase)
    virtual ~TauPi0ClusterScaler();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet& pTau, xAOD::PFOContainer& pNeutralPFOContainer, xAOD::PFOContainer& pChargedPFOContainer) override; 

private:

    /** @brief tool handles */
    //ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtensionTool;

    /** @brief reset neutral PFO kinematics (for AOD running) */
    void resetNeutralPFOs(xAOD::TauJet& pTau, xAOD::PFOContainer& pNeutralPFOContainer);

    /** @brief create charged PFOs */
    void createChargedPFOs(xAOD::TauJet& pTau, xAOD::PFOContainer& pChargedPFOContainer);

    /** @brief extrapolate charged PFO tracks to EM and HAD layers */
    //void extrapolateChargedPFOs(xAOD::TauJet& pTau);
    
    /** @brief associate hadronic PFOs to charged PFOs */
    void associateHadronicToChargedPFOs(xAOD::TauJet& pTau, xAOD::PFOContainer& pChargedPFOContainer);
    
    /** @brief associate charged PFOs to neutral PFOs */
    void associateChargedToNeutralPFOs(xAOD::TauJet& pTau, xAOD::PFOContainer& pNeutralPFOContainer);
    
    /** @brief associate charged PFOs to neutral PFOs */
    void subtractChargedEnergyFromNeutralPFOs(xAOD::PFOContainer& pNeutralPFOContainer);

    /** @brief sets of EM/Had samplings for track extrapolation */
    //std::set<CaloSampling::CaloSample> m_EMSamplings;
    //std::set<CaloSampling::CaloSample> m_HadSamplings;

    /** dodgy re-purposed PFOAttributes enums */
    //xAOD::PFODetails::PFOAttributes ETAECAL; 
    //xAOD::PFODetails::PFOAttributes PHIECAL;
    //xAOD::PFODetails::PFOAttributes ETAHCAL;
    //xAOD::PFODetails::PFOAttributes PHIHCAL;

};

#endif  /* TAUPI0CLUSTERSCALER_H */

