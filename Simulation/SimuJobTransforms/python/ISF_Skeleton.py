# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys
from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude

def defaultSimulationFlags(ConfigFlags, detectors):
    """Fill default simulation flags"""
    # TODO: how to autoconfigure those
    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep = ProductionStep.Simulation
    ConfigFlags.Sim.CalibrationRun = "Off" #"DeadLAr"
    ConfigFlags.Sim.RecordStepInfo = False
    ConfigFlags.Sim.CavernBG = "Signal"
    ConfigFlags.Sim.BeamPipeSimMode = 'FastSim'
    ConfigFlags.Sim.ReleaseGeoModel = False
    ConfigFlags.Sim.ISFRun = True
    ConfigFlags.GeoModel.Align.Dynamic = False

    #Frozen showers OFF = 0
    # ConfigFlags.Sim.LArParameterization = 2

    # Setup detector flags
    from SimuJobTransforms.SimulationTestHelpers import setupDetectorSimulateFlagsFromList
    setupDetectorSimulateFlagsFromList(ConfigFlags, detectors)


def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    log = logging.getLogger('Sim_tf')
    log.info('****************** STARTING Simulation *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    if hasattr(runArgs, 'detectors'):
        detectors = runArgs.detectors
    else:
        from SimuJobTransforms.SimulationTestHelpers import getDefaultDetectorsForRun
        detectors = getDefaultDetectorsForRun(ConfigFlags.GeoModel.Run)

    # Support switching on simulation of Forward Detectors
    if hasattr(runArgs, 'LucidOn'):
        detectors = detectors+['Lucid']
    if hasattr(runArgs, 'ZDCOn'):
        detectors = detectors+['ZDC']
    if hasattr(runArgs, 'AFPOn'):
        detectors = detectors+['AFP']
    if hasattr(runArgs, 'ALFAOn'):
        detectors = detectors+['ALFA']
    if hasattr(runArgs, 'FwdRegionOn'):
        detectors = detectors+['FwdRegion']
    #TODO here support switching on Cavern geometry
    #if hasattr(runArgs, 'CavernOn'):
    #    detectors = detectors+['Cavern']

    # Setup common simulation flags
    defaultSimulationFlags(ConfigFlags, detectors)

    if hasattr(runArgs, 'inputEVNTFile'):
        ConfigFlags.Input.Files = runArgs.inputEVNTFile
    else:
        raise RuntimeError('No input EVNT file defined')

    if hasattr(runArgs, 'outputHITSFile'):
        if runArgs.outputHITSFile == 'None':
            ConfigFlags.Output.HITSFileName = ''
        else:
            ConfigFlags.Output.HITSFileName  = runArgs.outputHITSFile
    else:
        raise RuntimeError('No outputHITSFile defined')

    if hasattr(runArgs, 'DataRunNumber'):
        ConfigFlags.Input.RunNumber = [runArgs.DataRunNumber] # is it updating?
        ConfigFlags.Input.OverrideRunNumber = True
        ConfigFlags.Input.LumiBlockNumber = [1] # dummy value

    if hasattr(runArgs, 'outputHITSFile'):
        ConfigFlags.Sim.PhysicsList = runArgs.physicsList

    if hasattr(runArgs, 'conditionsTag'):
        ConfigFlags.IOVDb.GlobalTag = runArgs.conditionsTag

    if hasattr(runArgs, 'simulator'):
        ConfigFlags.Sim.ISF.Simulator = runArgs.simulator

    if hasattr(runArgs, 'truthStrategy'):
        ConfigFlags.Sim.TruthStrategy = runArgs.truthStrategy

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # Lock flags
    ConfigFlags.lock()


    #run ISF
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(PoolWriteCfg(ConfigFlags))

    # add BeamEffectsAlg
    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    cfg.merge(BeamEffectsAlgCfg(ConfigFlags))

    # add the ISF_MainConfig
    from ISF_Config.ISF_MainConfigNew import Kernel_FullG4MTCfg
    cfg.merge(Kernel_FullG4MTCfg(ConfigFlags))

    from TileGeoG4SD.TileGeoG4SDToolConfig import TileGeoG4SDCalcCfg
    cfg.merge(TileGeoG4SDCalcCfg(ConfigFlags))

    #Add to item list
    #TODO - make a separate function (combine with G4AtlasAlg one?)
    ItemList = ["EventInfo#*",
                "McEventCollection#TruthEvent",
                "JetCollection#*"]

    if ConfigFlags.Sim.IncludeParentsInG4Event:
        ItemList += ["McEventCollection#GEN_EVENT"]

    ItemList += ["xAOD::JetContainer#*",
                 "xAOD::JetAuxContainer#*"]

    if ConfigFlags.Detector.SimulateID:
        ItemList += ["SiHitCollection#*",
                     "TRTUncompressedHitCollection#*",
                     "TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.SimulateITk:
        ItemList += ["SiHitCollection#*",
                     "TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.SimulateCalo:
        ItemList += ["CaloCalibrationHitContainer#*",
                     "LArHitContainer#*",
                     "TileHitVector#*",
                     "TrackRecordCollection#MuonEntryLayer"]

    if ConfigFlags.Detector.SimulateMuon:
        ItemList += ["RPCSimHitCollection#*",
                     "TGCSimHitCollection#*",
                     "MDTSimHitCollection#*",
                     "TrackRecordCollection#MuonExitLayer"]
        if ConfigFlags.Detector.GeometryCSC:
            ItemList += ["CSCSimHitCollection#*"]
        if ConfigFlags.Detector.GeometrysTGC:
            ItemList += ["sTGCSimHitCollection#*"]
        if ConfigFlags.Detector.GeometryMM:
            ItemList += ["MMSimHitCollection#*"]

    if ConfigFlags.Detector.SimulateLucid:
        ItemList += ["LUCID_SimHitCollection#*"]

    if ConfigFlags.Detector.SimulateFwdRegion:
        ItemList += ["SimulationHitCollection#*"]

    if ConfigFlags.Detector.SimulateZDC:
        ItemList += ["ZDC_SimPixelHit_Collection#*",
                     "ZDC_SimStripHit_Collection#*"]

    if ConfigFlags.Detector.SimulateALFA:
        ItemList += ["ALFA_HitCollection#*",
                     "ALFA_ODHitCollection#*"]

    if ConfigFlags.Detector.SimulateAFP:
        ItemList += ["AFP_TDSimHitCollection#*",
                     "AFP_SIDSimHitCollection#*"]

    # TimingAlg
    ItemList += ["RecoTimingObj#EVNTtoHITS_timings"]

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=ItemList, disableEventTag=True) )

    # FIXME hack because deduplication is broken
    PoolAttributes = ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
    PoolAttributes += ["DatabaseName = '" + ConfigFlags.Output.HITSFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'"]
    cfg.getService("AthenaPoolCnvSvc").PoolAttributes += PoolAttributes


    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)


    import time
    tic = time.time()
    # Run the final accumulator
    sc = cfg.run()
    log.info("Run ISF_MainConfigNew_Test in " + str(time.time()-tic) + " seconds")

    sys.exit(not sc.isSuccess())
