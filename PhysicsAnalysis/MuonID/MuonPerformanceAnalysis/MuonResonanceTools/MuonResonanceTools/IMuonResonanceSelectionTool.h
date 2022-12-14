/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonResonanceSelectionTool_H
#define IMuonResonanceSelectionTool_H

#include "AsgTools/IAsgTool.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "TrigMuonMatching/ITrigMuonMatching.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "AsgMessaging/StatusCode.h"

class IMuonResonanceSelectionTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonResonanceSelectionTool)

    public:

  virtual std::pair<std::vector<const xAOD::Muon*>,std::vector<const xAOD::Muon*> > selectMuons(const xAOD::MuonContainer*, bool isMC, CP::SystematicSet sys) = 0;
};

#endif
