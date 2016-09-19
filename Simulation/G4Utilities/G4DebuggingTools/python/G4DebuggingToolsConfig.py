# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr, Logging
from G4AtlasServices import G4AtlasServicesConfig

def getVerboseSelectorTool(name="G4UA::VerboseSelectorTool", **kwargs):
    from G4DebuggingTools.G4DebuggingToolsConf import G4UA__VerboseSelectorTool
    return G4UA__VerboseSelectorTool(name, **kwargs)

def addVerboseSelectorTool(name="G4UA::VerboseSelectorTool",system=False):
    G4AtlasServicesConfig.addAction(name,['BeginOfEvent','Step','BeginOfTracking','EndOfTracking'],system)

def getG4AtlantisDumperTool(name="G4UA::G4AtlantisDumperTool", **kwargs):
    from G4DebuggingTools.G4DebuggingToolsConf import G4UA__G4AtlantisDumperTool
    return G4UA__G4AtlantisDumperTool(name, **kwargs)

def addG4AtlantisDumperTool(name="G4UA::G4AtlantisDumperTool",system=False):
    G4AtlasServicesConfig.addAction(name,['EndOfEvent','Step','BeginOfEvent'],system)


def getEnergyConservationTestTool(name="G4UA::EnergyConservationTestTool", **kwargs):
    from G4DebuggingTools.G4DebuggingToolsConf import G4UA__EnergyConservationTestTool
    return G4UA__EnergyConservationTestTool(name, **kwargs)

def addEnergyConservationTestTool(name="G4UA::EnergyConservationTestTool",system=False):
    G4AtlasServicesConfig.addAction(name,['EndOfEvent','Step','BeginOfTracking','EndOfTracking'],system)

def getHyperspaceCatcherTool(name="G4UA::HyperspaceCatcherTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)

    from G4DebuggingTools.G4DebuggingToolsConf import G4UA__HyperspaceCatcherTool
    return G4UA__HyperspaceCatcherTool(name, **kwargs)

def addHyperspaceCatcherTool(name="G4UA::HyperspaceCatcherTool",system=False):
    G4AtlasServicesConfig.addAction(name,['BeginOfRun','Step'],system)

def getStepNtupleTool(name="G4UA::StepNtupleTool", **kwargs):
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() >1:
        log=Logging.logging.getLogger(name)
        log.fatal('Attempt to run '+name+' with more than one thread, which is not supported')
        #from AthenaCommon.AppMgr import theApp
        #theApp.exit(1)
        return False

    from G4DebuggingTools.G4DebuggingToolsConf import G4UA__StepNtupleTool
    return G4UA__StepNtupleTool(name, **kwargs)


def getVolumeDebuggerTool(name="G4UA::VolumeDebuggerTool", **kwargs):
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps

    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)

    from G4DebuggingTools.G4DebuggingToolsConf import G4UA__VolumeDebuggerTool
    return G4UA__VolumeDebuggerTool(name, **kwargs)
