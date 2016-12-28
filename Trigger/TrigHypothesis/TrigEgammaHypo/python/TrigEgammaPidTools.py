# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# TrigEgammaPidTools
# Add all pid selectors to ToolSvc
# Using versioned configuration -- over rides the configuration defined in mapping
# Conf files should be moved to $CALIBPATH and seperated into directories
#
# Trigger specific interface methods have been removed 
# Behavior now set via properties
# Results is multiple instances of tools to apply calo-only and trigger threshold
###############################################################
# Release 21 Update
# Remove Run1 selectors
# Remove unused selectors
# 21.0.12 -- Use TrigEgammaFlags to control version
# Future plans:
# Move all tools into TrigEgammaRec 
# Ensure consistent initialize of Pid Tools in menu or in ToolFactories

from PATCore.HelperUtils import *
from AthenaCommon import CfgMgr

import sys
import cppyy
import logging
cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import LikeEnum
from ROOT import egammaPID

# Include electron menus for LH and Cut-based
from ElectronPhotonSelectorTools.ElectronLikelihoodToolMapping import electronLHmenu
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu
from ElectronPhotonSelectorTools.PhotonIsEMSelectorMapping import photonPIDmenu
from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool
from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefElectron
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefPhoton

mlog = logging.getLogger ('TrigEgammaPidTools')
# Path for versioned configuration
from TrigEgammaRec.TrigEgammaFlags import jobproperties
mlog.info("TrigEgammaPidTools version %s"%jobproperties.TrigEgammaFlags.pidVersion())
ConfigFilePath = jobproperties.TrigEgammaFlags.pidVersion() 

# Dictionaries for ToolNames
ElectronToolName = {'vloose':'AsgElectronIsEMVLooseSelector',
    'loose':'AsgElectronIsEMLooseSelector',
    'medium':'AsgElectronIsEMMediumSelector',
    'tight':'AsgElectronIsEMTightSelector',
    'mergedtight':'AsgElectronIsEMMergedTightSelector',
    'lhvloose':'AsgElectronLHVLooseSelector',
    'lhloose':'AsgElectronLHLooseSelector',
    'lhmedium':'AsgElectronLHMediumSelector',
    'lhtight':'AsgElectronLHTightSelector',}

ElectronCaloToolName = {'vloose':'AsgElectronIsEMVLooseCaloSelector',
    'loose':'AsgElectronIsEMLooseCaloSelector',
    'medium':'AsgElectronIsEMMediumCaloSelector',
    'tight':'AsgElectronIsEMTightCaloSelector',
    'mergedtight':'AsgElectronIsEMMergedTightCaloSelector',
    'lhvloose':'AsgElectronLHVLooseCaloSelector',
    'lhloose':'AsgElectronLHLooseCaloSelector',
    'lhmedium':'AsgElectronLHMediumCaloSelector',
    'lhtight':'AsgElectronLHTightCaloSelector',}

# Electron LH tools for alignment / commisioning
ElectronLHVLooseToolName = {'cutd0dphideta':'AsgElectronLHVeryLooseCutD0DphiDetaSelector',
        'nod0':'AsgElectronLHVeryLooseNoD0Selector',
        'nodeta':'AsgElectronLHVeryLooseNoDetaSelector',
        'nodphires':'AsgElectronLHVeryLooseNoDphiResSelector',}

ElectronLHVLooseToolConfigFile = {'cutd0dphideta':'ElectronLikelihoodVeryLooseTriggerConfig2015_CutD0DphiDeta.conf',
        'nod0':'ElectronLikelihoodVeryLooseTriggerConfig2015_NoD0.conf',
        'nodeta':'ElectronLikelihoodVeryLooseTriggerConfig2015_NoDeta.conf',
        'nodphires':'ElectronLikelihoodVeryLooseTriggerConfig2015_NoDphiRes.conf',}

ElectronLHLooseToolName = {'cutd0dphideta':'AsgElectronLHLooseCutD0DphiDetaSelector',
        'nod0':'AsgElectronLHLooseNoD0Selector',
        'nodeta':'AsgElectronLHLooseNoDetaSelector',
        'nodphires':'AsgElectronLHLooseNoDphiResSelector',}

ElectronLHLooseToolConfigFile = {'cutd0dphideta':'ElectronLikelihoodLooseTriggerConfig2015_CutD0DphiDeta.conf',
        'nod0':'ElectronLikelihoodLooseTriggerConfig2015_NoD0.conf',
        'nodeta':'ElectronLikelihoodLooseTriggerConfig2015_NoDeta.conf',
        'nodphires':'ElectronLikelihoodLooseTriggerConfig2015_NoDphiRes.conf',}

ElectronLHMediumToolName = {'cutd0dphideta':'AsgElectronLHMediumCutD0DphiDetaSelector',
        'nod0':'AsgElectronLHMediumNoD0Selector',
        'nodeta':'AsgElectronLHMediumNoDetaSelector',
        'nodphires':'AsgElectronLHMediumNoDphiResSelector',}

ElectronLHMediumToolConfigFile = {'cutd0dphideta':'ElectronLikelihoodMediumTriggerConfig2015_CutD0DphiDeta.conf',
        'nod0':'ElectronLikelihoodMediumTriggerConfig2015_NoD0.conf',
        'nodeta':'ElectronLikelihoodMediumTriggerConfig2015_NoDeta.conf',
        'nodphires':'ElectronLikelihoodMediumTriggerConfig2015_NoDphiRes.conf',}

ElectronLHTightToolName = {'cutd0dphideta':'AsgElectronLHTightCutD0DphiDetaSelector',
        'nod0':'AsgElectronLHTightNoD0Selector',
        'nodeta':'AsgElectronLHTightNoDetaSelector',
        'nodphires':'AsgElectronLHTightNoDphiResSelector',
        'smooth':'AsgElectronLHTightSmoothSelector'}

ElectronLHTightToolConfigFile = {'cutd0dphideta':'ElectronLikelihoodTightTriggerConfig2015_CutD0DphiDeta.conf',
        'nod0':'ElectronLikelihoodTightTriggerConfig2015_NoD0.conf',
        'nodeta':'ElectronLikelihoodTightTriggerConfig2015_NoDeta.conf',
        'nodphires':'ElectronLikelihoodTightTriggerConfig2015_NoDphiRes.conf',
        'smooth':'ElectronLikelihoodTightTriggerConfig2015_Smooth.conf'}

ElectronToolConfigFile = {'vloose':'ElectronIsEMVLooseSelectorCutDefs.conf', 
    'loose':'ElectronIsEMLooseSelectorCutDefs.conf',
    'medium':'ElectronIsEMMediumSelectorCutDefs.conf',
    'tight':'ElectronIsEMTightSelectorCutDefs.conf',
    'mergedtight':'ElectronIsEMMergedTightSelectorCutDefs.conf',
    'lhvloose':'ElectronLikelihoodVeryLooseTriggerConfig2015.conf',
    'lhloose':'ElectronLikelihoodLooseTriggerConfig2015.conf',
    'lhmedium':'ElectronLikelihoodMediumTriggerConfig2015.conf',
    'lhtight':'ElectronLikelihoodTightTriggerConfig2015.conf',}

ElectronCaloToolConfigFile = {'vloose':'ElectronIsEMVLooseSelectorCutDefs.conf', 
    'loose':'ElectronIsEMLooseSelectorCutDefs.conf',
    'medium':'ElectronIsEMMediumSelectorCutDefs.conf',
    'tight':'ElectronIsEMTightSelectorCutDefs.conf',
    'mergedtight':'ElectronIsEMMergedTightSelectorCutDefs.conf',
    'lhvloose':'ElectronLikelihoodEFCaloOnlyVeryLooseConfig2015.conf',
    'lhloose':'ElectronLikelihoodEFCaloOnlyLooseConfig2015.conf',
    'lhmedium':'ElectronLikelihoodEFCaloOnlyMediumConfig2015.conf',
    'lhtight':'ElectronLikelihoodEFCaloOnlyTightConfig2015.conf',}

ElectronIsEMBits = {'vloose':SelectionDefElectron.ElectronLooseHLT,
    'loose':SelectionDefElectron.ElectronLooseHLT,
    'medium':SelectionDefElectron.ElectronMediumHLT,
    'tight':SelectionDefElectron.ElectronTightHLT,
    'mergedtight':SelectionDefElectron.ElectronTightHLT, #Can define separate mask
    'lhvloose':SelectionDefElectron.ElectronLooseHLT,
    'lhloose':SelectionDefElectron.ElectronLooseHLT,
    'lhmedium':SelectionDefElectron.ElectronLooseHLT,
    'lhtight':SelectionDefElectron.ElectronLooseHLT,}

ElectronPidName = {'vloose':egammaPID.ElectronIDLooseHLT,
    'loose':egammaPID.ElectronIDLooseHLT,
    'medium':egammaPID.ElectronIDMediumHLT,
    'tight':egammaPID.ElectronIDTightHLT,
    'mergedtight':egammaPID.ElectronIDTightHLT,
    }

# Dictionaries for ToolNames
PhotonToolName = {'loose':'AsgPhotonIsEMLooseSelector',
    'medium':'AsgPhotonIsEMMediumSelector',
    'tight':'AsgPhotonIsEMTightSelector',}

PhotonToolConfigFile = {'loose':"PhotonIsEMLooseSelectorCutDefs.conf",
    'medium':"PhotonIsEMMediumSelectorCutDefs.conf",
    'tight':"PhotonIsEMTightSelectorCutDefs.conf",}

PhotonIsEMBits = {'loose':SelectionDefPhoton.PhotonLoose,
    'medium':SelectionDefPhoton.PhotonMedium,
    'tight':SelectionDefPhoton.PhotonTight,}

# Add function to add to ToolSvc, same as in Factories
def addToToolSvc( tool ):
    "addToToolSvc( tool ) --> add tool to ToolSvc"
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,tool.getName()):
        ToolSvc += tool
        #print tool
    return tool

def ElectronPidTools():
    # Versioned selectors for Run2
    #print '=========== Run2 PID ============'
    for key in ElectronToolName:
        if('lh' in key):
            tool=CfgMgr.AsgElectronLikelihoodTool(ElectronToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronToolConfigFile[key]
            tool.usePVContainer = False
            addToToolSvc( tool )
        else:
            tool=CfgMgr.AsgElectronIsEMSelector(ElectronToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronToolConfigFile[key]
            tool.isEMMask = ElectronIsEMBits[key]
            addToToolSvc( tool )
    
    # Calo-only selectors
    #print '=========== Run2 PID Calo============'
    for key in ElectronCaloToolName:
        if('lh' in key):
            tool=CfgMgr.AsgElectronLikelihoodTool(ElectronCaloToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronCaloToolConfigFile[key]
            tool.usePVContainer = False
            tool.caloOnly = True
            addToToolSvc( tool )
        else: 
            tool=CfgMgr.AsgElectronIsEMSelector(ElectronCaloToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronCaloToolConfigFile[key]
            tool.isEMMask = ElectronIsEMBits[key]
            tool.caloOnly = True
            addToToolSvc( tool )
    
   
    # Special LH triggers for alignment / commisioning
    for key in ElectronLHVLooseToolName:
        tool=CfgMgr.AsgElectronLikelihoodTool(ElectronLHVLooseToolName[key])
        tool.ConfigFile = ConfigFilePath + ElectronLHVLooseToolConfigFile[key]
        tool.usePVContainer = False
        addToToolSvc( tool )
        
    for key in ElectronLHLooseToolName:
        tool=CfgMgr.AsgElectronLikelihoodTool(ElectronLHLooseToolName[key])
        tool.ConfigFile = ConfigFilePath + ElectronLHLooseToolConfigFile[key]
        tool.usePVContainer = False
        addToToolSvc( tool )
        
    for key in ElectronLHMediumToolName:
        tool=CfgMgr.AsgElectronLikelihoodTool(ElectronLHMediumToolName[key])
        tool.ConfigFile = ConfigFilePath + ElectronLHMediumToolConfigFile[key]
        tool.usePVContainer = False
        addToToolSvc( tool )
        
    for key in ElectronLHTightToolName:
        tool=CfgMgr.AsgElectronLikelihoodTool(ElectronLHTightToolName[key])
        tool.ConfigFile = ConfigFilePath + ElectronLHTightToolConfigFile[key]
        tool.usePVContainer = False
        addToToolSvc( tool )
    
def PhotonPidTools():
    from AthenaCommon.AppMgr import ToolSvc
    # Run2 Photon trigger
    for key in PhotonToolName:
        tool=CfgMgr.AsgPhotonIsEMSelector(PhotonToolName[key])
        tool.ConfigFile = ConfigFilePath + PhotonToolConfigFile[key]
        tool.isEMMask = PhotonIsEMBits[key]
        tool.ForceConvertedPhotonPID = True
        addToToolSvc( tool )
del mlog
