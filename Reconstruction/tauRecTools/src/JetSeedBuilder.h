/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_JETSEEDBUILDER_H
#define	TAUREC_JETSEEDBUILDER_H

#include "tauRecTools/TauRecToolBase.h"


/**
 * @brief Class to build tauRec seeds from topojets.
 * 
 *  Sets the jet ElementLink and basic kinematic variables in TauJet by searching a matching jet to the tau direction.
 *  Also the mass of the tau is set to 0.
 *  With tauRec4 the JetSeedBuilder method is the only one to search for a tau candidates. 
 *  The author of the tau candidate is set 1 (former calo-only seeded) and 3 (former calo+track-seeded) to keep backwards compatibility.
 *  
 * @author N.Meyer <nicom@cern.ch>
 * @author Felix Friedrich
*/

class JetSeedBuilder : public TauRecToolBase {
public:
    JetSeedBuilder(const std::string& name);
    ASG_TOOL_CLASS2( JetSeedBuilder, TauRecToolBase, ITauToolBase )

    virtual ~JetSeedBuilder();

    virtual StatusCode initialize() override;
    virtual StatusCode execute(xAOD::TauJet& pTau) const override;
    virtual StatusCode finalize() override;

};

#endif	/* JETSEEDBUILDER_H */

