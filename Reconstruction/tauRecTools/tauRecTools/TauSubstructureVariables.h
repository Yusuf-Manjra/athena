/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUSUBSTRUCTUREBUILDER_H
#define TAUREC_TAUSUBSTRUCTUREBUILDER_H

#include "tauRecTools/TauRecToolBase.h"

/**
 * @brief Calculate variables from the tau substructure.
 * 
 * @author M. Trottier-McDonald
 * @author Felix Friedrich
 * 
 */

class TauSubstructureVariables : public TauRecToolBase
{
    public: 
	    ASG_TOOL_CLASS2(TauSubstructureVariables, TauRecToolBase, ITauToolBase)
        
        static const float DEFAULT;

        TauSubstructureVariables(const std::string& name="TauSubstructureVariables");

        ~TauSubstructureVariables();

        virtual StatusCode execute(xAOD::TauJet& pTau) const override;

    private:
        // enable cell origin correction
        // eta and phi of the cells are corrected wrt to the origin of the tau vertex
	bool m_doVertexCorrection;

	// use shower subtracted clusters with PFlow jet seeds
	bool m_incShowerSubtr;

};

#endif
