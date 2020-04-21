/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCOMMONCALCVARS_H
#define TAUREC_TAUCOMMONCALCVARS_H

#include "tauRecTools/TauRecToolBase.h"


/**
 * @brief Calculate variables which rely on tracks and precalculated cell/cluster information.
 * 
 *  All variables here can be recalculated using AODs.
 * 
 * @author Stan Lai
 * @author Felix Friedrich
 */

class TauCommonCalcVars : public TauRecToolBase {
public:
    //-----------------------------------------------------------------
    // Constructor and destructor
    //-----------------------------------------------------------------
    TauCommonCalcVars(const std::string& name="TauCommonCalcVars");
    ASG_TOOL_CLASS2(TauCommonCalcVars, TauRecToolBase, ITauToolBase)
    ~TauCommonCalcVars();
    
    virtual StatusCode initialize() override;   
    virtual StatusCode execute(xAOD::TauJet& pTau) override;
    virtual StatusCode finalize() override;  

private:
    
    //if TauTrackClassifier is not run, wide&passTrkSelector==classifiedIsolation==modifiedIsolationTrack
    Gaudi::Property<int> m_isolationTrackType {this, "isolationTrackType", xAOD::TauJetParameters::modifiedIsolationTrack};
};

#endif // TAUREC_TAUCOMMONCALCVARS_H
