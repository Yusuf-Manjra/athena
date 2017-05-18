/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_MASSDEBUGGER_H
#define G4DEBUGGINGTOOLS_MASSDEBUGGER_H

#include <string>
#include <vector>

#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA {
/// @class MassDebugger
/// @brief a user action to get masses of G4 logical volumes
///
/// It looks for the logical volume specified by the user in the TargetLogicalVolume parameter.
/// (If it does not find it, it prints the list of possibilities, and exits).
/// If found, it prints information about the logical volume including the G4 estimate of its mass.
/// Some volumes, such as booleans, use Monte Carlo integration to estimate the volume. For these, precision/CPU time can 
/// be controlled with the nStat parameter which is the number of points to be sampled in the integration.
///

class MassDebugger: public AthMessaging, public IBeginRunAction {
public:
    struct Config {
        std::vector<std::string> targetLogicalVolume;
        int nStat = 1000000; // Cannot find how to use this
    };

    MassDebugger(const Config &config);
    /// hook for G4 UA functionality
    virtual void beginOfRun(const G4Run *) override;
private:
    Config m_config;
};

}

#endif // G4DEBUGGINGTOOLS_MASSDEBUGGER_H
