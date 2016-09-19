# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("ISF_Geant4UserActions.ISF_Geant4UserActionsConf.FastCaloSimParamAction", "FastCaloSimParamAction")
addTool("ISF_Geant4UserActions.ISF_Geant4UserActionsConfig.getFastCaloSimParamActionTool", "G4UA::FastCaloSimParamActionTool")

addTool("ISF_Geant4UserActions.ISF_Geant4UserActionsConf.iGeant4__TestBoundariesUserAction", "iGeant4__TestBoundariesUserAction")
addTool("ISF_Geant4UserActions.ISF_Geant4UserActionsConfig.getTestBoundariesUserActionTool", "G4UA::iGeant4::TestBoundariesUserActionTool")
