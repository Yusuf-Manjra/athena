# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

DetectorGeometrySvc, G4AtlasSvc, G4GeometryNotifierSvc, PhysicsListSvc=CompFactory.getComps("DetectorGeometrySvc","G4AtlasSvc","G4GeometryNotifierSvc","PhysicsListSvc",)
from G4AtlasTools.G4GeometryToolConfig import G4AtlasDetectorConstructionToolCfg
from G4StepLimitation.G4StepLimitationConfigNew import G4StepLimitationToolCfg
from ExtraParticles.ExtraParticlesConfigNew import ExtraParticlesPhysicsToolCfg
from G4ExtraProcesses.G4ExtraProcessesConfigNew import G4EMProcessesPhysicsToolCfg
from TRT_TR_Process.TRT_TR_ProcessConfigNew import TRTPhysicsToolCfg

def DetectorGeometrySvcCfg(ConfigFlags, name="DetectorGeometrySvc", **kwargs):
    result = ComponentAccumulator()
    detConstTool = result.popToolsAndMerge(G4AtlasDetectorConstructionToolCfg(ConfigFlags))
    result.addPublicTool(detConstTool)
    kwargs.setdefault("DetectorConstruction", result.getPublicTool(detConstTool.name))

    result.addService(DetectorGeometrySvc(name, **kwargs))
    return result


def G4AtlasSvcCfg(ConfigFlags, name="G4AtlasSvc", **kwargs):
    if ConfigFlags.Concurrency.NumThreads > 0:
        is_hive = True
    else:
        is_hive = False
    kwargs.setdefault("isMT", is_hive)
    kwargs.setdefault("DetectorGeometrySvc", 'DetectorGeometrySvc')
    return G4AtlasSvc(name, **kwargs)


def G4GeometryNotifierSvcCfg(ConfigFlags, name="G4GeometryNotifierSvc", **kwargs):
    kwargs.setdefault("ActivateLVNotifier", True)
    kwargs.setdefault("ActivatePVNotifier", False)
    return G4GeometryNotifierSvc(name, **kwargs)


def PhysicsListSvcCfg(ConfigFlags, name="PhysicsListSvc", **kwargs):
    result = ComponentAccumulator()
    PhysOptionList = [ result.popToolsAndMerge(G4StepLimitationToolCfg(ConfigFlags)) ]
    if 'QS' in ConfigFlags.Sim.ISF.Simulator or 'LongLived' in ConfigFlags.Sim.ISF.Simulator:
        #Quasi stable particle simulation
        PhysOptionList += [ result.popToolsAndMerge(ExtraParticlesPhysicsToolCfg(ConfigFlags)) ] # FIXME more configuration required in this method
        PhysOptionList += [ result.popToolsAndMerge(G4EMProcessesPhysicsToolCfg(ConfigFlags)) ]
    #PhysOptionList += ConfigFlags.Sim.PhysicsOptions # FIXME Missing functionality
    if ConfigFlags.Detector.GeometryTRT:
        PhysOptionList +=[ result.popToolsAndMerge(TRTPhysicsToolCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.GeometryLucid or ConfigFlags.Detector.GeometryAFP:
        LucidPhysicsTool = CompFactory.LucidPhysicsTool
        PhysOptionList +=[LucidPhysicsTool("LucidPhysicsTool")]
    kwargs.setdefault("PhysOption", PhysOptionList)
    PhysDecaysList = []
    kwargs.setdefault("PhysicsDecay", PhysDecaysList)
    kwargs.setdefault("PhysicsList", ConfigFlags.Sim.PhysicsList)
    if 'PhysicsList' in kwargs:
        if kwargs['PhysicsList'].endswith('_EMV') or kwargs['PhysicsList'].endswith('_EMX'):
            raise RuntimeError( 'PhysicsList not allowed: '+kwargs['PhysicsList'] )

    kwargs.setdefault("GeneralCut", 1.)
    if ConfigFlags.Sim.CavernBG not in ["Read","Write"]:
        kwargs.setdefault("NeutronTimeCut", ConfigFlags.Sim.NeutronTimeCut)
    kwargs.setdefault("NeutronEnergyCut", ConfigFlags.Sim.NeutronEnergyCut)
    kwargs.setdefault("ApplyEMCuts", ConfigFlags.Sim.ApplyEMCuts)
    ## from AthenaCommon.SystemOfUnits import eV, TeV
    ## kwargs.setdefault("EMMaxEnergy"     , 7*TeV)
    ## kwargs.setdefault("EMMinEnergy"     , 100*eV)
    """ --- ATLASSIM-3967 ---
        these two options are replaced by SetNumberOfBinsPerDecade
        which now controls both values.
    """
    ## kwargs.setdefault("EMDEDXBinning"   , 77)
    ## kwargs.setdefault("EMLambdaBinning" , 77)
    result.addService(PhysicsListSvc(name, **kwargs))
    return result
