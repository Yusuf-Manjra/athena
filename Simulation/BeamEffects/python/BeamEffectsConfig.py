# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configurations for classes in BeamEffects package
"""

from AthenaCommon import CfgMgr
from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *

#--------------------------------------------------------------------------------------------------
## LorentzVectorGenerators

def getVertexPositionFromFile(name="VertexPositionFromFile", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    vtxPosFile = "vtx-pos.txt"
    if simFlags.VertexOverrideFile.statusOn:
        vtxPosFile = str(simFlags.VertexOverrideEventFile())
    kwargs.setdefault("VertexPositionsFile"         , vtxPosFile)
    runAndEventNosFile ="run-evt-nums.txt"
    if simFlags.VertexOverrideEventFile.statusOn:
        runAndEventNosFile = str(simFlags.VertexOverrideEventFile())
    kwargs.setdefault("RunAndEventNumbersFile"      , runAndEventNosFile)
    return CfgMgr.Simulation__VertexPositionFromFile(name, **kwargs)

def getVertexBeamCondPositioner(name="VertexBeamCondPositioner", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    #simFlags.VertexTimeOffset.get_Value()
    kwargs.setdefault('RandomSvc'               , simFlags.RandomSvc.get_Value())
    if not simFlags.RandomSeedList.checkForExistingSeed("VERTEX"):
        simFlags.RandomSeedList.addSeed( "VERTEX", 2040160768, 443921183 )
    return CfgMgr.Simulation__VertexBeamCondPositioner(name, **kwargs)

def getLongBeamspotVertexPositioner(name="LongBeamspotVertexPositioner", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    #simFlags.VertexTimeOffset.get_Value()
    kwargs.setdefault('LParameter'              , 150.0)
    kwargs.setdefault('RandomSvc'               , simFlags.RandomSvc.get_Value())
    if not simFlags.RandomSeedList.checkForExistingSeed("VERTEX"):
        simFlags.RandomSeedList.addSeed( "VERTEX", 2040160768, 443921183 )
    return CfgMgr.Simulation__LongBeamspotVertexPositioner(name, **kwargs)

def getCrabKissingVertexPositioner(name="CrabKissingVertexPositioner", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('BunchLength'             , 75.0)
    kwargs.setdefault('RandomSvc'               , simFlags.RandomSvc.get_Value())
    kwargs.setdefault('BunchShape'              , "GAUSS")
    if not simFlags.RandomSeedList.checkForExistingSeed("VERTEX"):
        simFlags.RandomSeedList.addSeed( "VERTEX", 2040160768, 443921183 )
    return CfgMgr.Simulation__CrabKissingVertexPositioner(name, **kwargs)

#--------------------------------------------------------------------------------------------------
## GenEventManipulators

def getGenEventValidityChecker(name="GenEventValidityChecker", **kwargs):
    return CfgMgr.Simulation__GenEventValidityChecker(name, **kwargs)

def getGenEventVertexPositioner(name="GenEventVertexPositioner", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    readVtxPosFromFile = simFlags.VertexOverrideFile.statusOn or simFlags.VertexOverrideEventFile.statusOn
    if simFlags.ISFRun():
        from  ISF_Config.ISF_jobProperties import ISF_Flags
        readVtxPosFromFile = readVtxPosFromFile or ISF_Flags.VertexPositionFromFile()
    #FIXME we can probably drop this ISF_Flag as it seems redundant
    if readVtxPosFromFile:
        kwargs.setdefault("VertexShifters"          , [ 'VertexPositionFromFile' ])
    elif simFlags.VertexFromCondDB():
        # TODO At this point there should be the option of using the
        # LongBeamspotVertexPositioner too.
        kwargs.setdefault("VertexShifters"          , [ 'VertexBeamCondPositioner' ])
    return CfgMgr.Simulation__GenEventVertexPositioner(name, **kwargs)

def getGenEventBeamEffectBooster(name="GenEventBeamEffectBooster", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed("BEAM"):
        simFlags.RandomSeedList.addSeed( "BEAM", 3499598793, 7345291 )
    for opt in simFlags.BeamEffectOptions.get_Value().keys():
        kwargs.setdefault(opt, float(simFlags.BeamEffectOptions.get_Value()[opt]) )#TODO check syntax
    return CfgMgr.Simulation__GenEventBeamEffectBooster(name, **kwargs)

def getGenEventRotator(name="GenEventRotator", **kwargs):
    kwargs.setdefault("xAngle", 0.0)
    kwargs.setdefault("yAngle", 0.0)
    kwargs.setdefault("zAngle", 0.0)
    return CfgMgr.CfgMgr.Simulation__GenEventRotator(name, **kwargs)

#--------------------------------------------------------------------------------------------------
## Algorithms
def getBeamEffectsAlg(name="BeamEffectsAlg", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("ISFRun", simFlags.ISFRun()) #FIXME Temporary property so that we don't change the output in the initial switch to this code.
    manipulatorList = ['GenEventValidityChecker']
    if hasattr(simFlags, 'Eta') or hasattr(simFlags, 'LArTB_H1TableYPos'): #FIXME Ugly hack
        # TestBeam do nothing else for now
        pass
    else:
        if simFlags.VertexTimeOffset.statusOn and simFlags.VertexTimeOffset.get_Value() != 0:
            raise RuntimeError( 'Vertex time offset should not be used!' )
        from AthenaCommon.BeamFlags import jobproperties
        if not (simFlags.CavernBG.get_Value()=='Read' or jobproperties.Beam.beamType() == "cosmics"):
            manipulatorList += ["GenEventVertexPositioner"]
        if simFlags.BeamEffectOptions.statusOn:
            manipulatorList += ["GenEventBeamEffectBooster"]
    kwargs.setdefault("GenEventManipulators", manipulatorList)
    return CfgMgr.Simulation__BeamEffectsAlg(name, **kwargs)
