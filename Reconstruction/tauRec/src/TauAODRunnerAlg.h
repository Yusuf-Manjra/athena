/*
	Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUAODRUNNERALG_H
#define TAUREC_TAUAODRUNNERALG_H

#include "tauRecTools/ITauToolBase.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODParticleEvent/ParticleAuxContainer.h"
#include "xAODTau/TauTrackContainer.h"

#include "GaudiKernel/ToolHandle.h"

/**
 * @brief       Main class for tau candidate processing.
 */


class TauAODRunnerAlg: public AthAlgorithm
{
    public:
        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauAODRunnerAlg( const std::string &name, ISvcLocator *  );
        ~TauAODRunnerAlg();

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode initialize() override;
        virtual StatusCode execute() override;
        virtual StatusCode finalize() override;

    private:
		ToolHandleArray<ITauToolBase>  m_tools_mod{this, "Tools_mod", {}, "Modified Tools for taus"};
		ToolHandleArray<ITauToolBase>  m_tools_after{this, "Tools_after", {}, "Tools for taus After the modification"};

		SG::ReadHandleKey<xAOD::TauJetContainer>        m_tauContainer{this,"Key_tauContainer","TauJets","input tau key"};
		SG::ReadHandleKey<xAOD::CaloClusterContainer>   m_pi0ClusterInputContainer{this,"Key_Pi0ClusterInputContainer", "TauPi0Clusters", "input pi0 cluster"};
		SG::ReadHandleKey<xAOD::TauTrackContainer>      m_tauTrackInputContainer{this, "Key_TauTrackInputContainer", "TauTracks", "input tau track cluster"};

		SG::WriteHandleKey<xAOD::TauJetContainer>       m_tauOutContainer           {this, "Key_tauOutputContainer",            "TauJets_Fix", "output tau key"};
		SG::WriteHandleKey<xAOD::ParticleContainer>     m_pi0Container              {this, "Key_pi0OutputContainer",            "TauFinalPi0s_Fix", "tau final pi0s output"};
		SG::WriteHandleKey<xAOD::PFOContainer>          m_neutralPFOOutputContainer {this, "Key_neutralPFOOutputContainer",     "TauNeutralPFOs_Fix", "tau neutral pfo out key"};
		SG::WriteHandleKey<xAOD::PFOContainer>          m_chargedPFOOutputContainer {this, "Key_chargedPFOOutputContainer",     "TauChargedPFOs_Fix", "tau charged pfo out key"};
		SG::WriteHandleKey<xAOD::PFOContainer>          m_hadronicPFOOutputContainer{this, "Key_hadronicPFOOutputContainer",    "TauHadronicPFOs_Fix", "tau hadronic pfo out key"};
		SG::WriteHandleKey<xAOD::TauTrackContainer>     m_tauTrackOutputContainer   {this, "Key_tauTrackOutputContainer",       "TauTracks_Fix", "output tau track key"};

		virtual bool isTauModified(const xAOD::TauJet* oritaus, const xAOD::TauJet* modtaus);
};

#endif // TAUREC_TAUAODRUNNERALG_H
