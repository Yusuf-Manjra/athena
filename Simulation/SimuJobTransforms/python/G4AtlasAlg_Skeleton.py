# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys
from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude

def defaultSimulationFlags(ConfigFlags, detectors):
    """Fill default simulation flags"""
    # TODO: how to autoconfigure those
    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep = ProductionStep.Simulation
    # Writing out CalibrationHits only makes sense if we are running FullG4 simulation without frozen showers
    if (ConfigFlags.Sim.ISF.Simulator not in ('FullG4MT', 'FullG4MT_LongLived')) or ConfigFlags.Sim.LArParameterization!=0:
        ConfigFlags.Sim.CalibrationRun = "Off"

    ConfigFlags.Sim.RecordStepInfo = False
    ConfigFlags.Sim.ReleaseGeoModel = False
    ConfigFlags.Sim.ISFRun = False # Need to change this for G4AA and ISF!
    ConfigFlags.GeoModel.Align.Dynamic = False

    # Setup detector flags
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)


def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    log = logging.getLogger('AtlasG4_tf')
    log.info('****************** STARTING Simulation *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    if hasattr(runArgs, 'detectors'):
        detectors = runArgs.detectors
    else:
        from AthenaConfiguration.AutoConfigFlags import getDefaultDetectors
        detectors = getDefaultDetectors(ConfigFlags.GeoModel.AtlasVersion)

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

    # Beam Type
    if hasattr(runArgs,'beamType'):
        if runArgs.beamType == 'cosmics':
            ConfigFlags.Beam.Type = 'cosmics' # TODO would like to make these values into an enum to make it more robust
            ConfigFlags.Sim.CavernBG = 'Off'

    # Setup input: Three possible cases:
    # 1) inputEVNTFile (normal)
    # 2) inputEVNT_TRFile (TrackRecords from pre-simulated events,
    # used with TrackRecordGenerator)
    # 3) no input file (on-the-fly generation - typically ParticleGun
    # or CosmicGenerator)
    if hasattr(runArgs, 'inputEVNTFile'):
        ConfigFlags.Input.Files = runArgs.inputEVNTFile
    elif hasattr(runArgs, 'inputEVNT_TRFile'):
        ConfigFlags.Input.Files = runArgs.inputEVNT_TRFile
        ConfigFlags.Sim.ReadTR = True
        # Three common cases here:
        # 2a) Cosmics simulation
        # 2b) Stopped particle simulation
        # 2c) Cavern background simulation
        if ConfigFlags.Beam.Type == 'cosmics':
            ConfigFlags.Sim.CosmicFilterVolumeNames = ['Muon']
            ConfigFlags.Detector.GeometryCavern = True # simulate the cavern with a cosmic TR file
        elif hasattr(runArgs,"trackRecordType") and runArgs.trackRecordType=="stopped":
            log.error('Stopped Particle simulation is not supported yet')
        else:
            ConfigFlags.Detector.GeometryCavern = True # simulate the cavern
            ConfigFlags.Sim.CavernBG = 'Read'
    else:
        # Common cases
        # 3a) ParticleGun
        # 3b) CosmicGenerator
        ConfigFlags.Input.Files = ''
        ConfigFlags.Input.isMC = True
        log.info('No inputEVNTFile provided. Assuming that you are running a generator on the fly.')
        if ConfigFlags.Beam.Type == 'cosmics':
            ConfigFlags.Sim.CosmicFilterVolumeNames = [getattr(runArgs, "CosmicFilterVolume", "InnerDetector")]
            ConfigFlags.Sim.CosmicFilterVolumeNames += [getattr(runArgs, "CosmicFilterVolume2", "NONE")]
            ConfigFlags.Sim.CosmicPtSlice = getattr(runArgs, "CosmicPtSlice", 'NONE')
            ConfigFlags.Detector.GeometryCavern = True # simulate the cavern when generating cosmics on-the-fly
            log.debug('No inputEVNTFile provided. OK, as performing cosmics simulation.')

    if hasattr(runArgs, 'outputHITSFile'):
        if runArgs.outputHITSFile == 'None':
            ConfigFlags.Output.HITSFileName = ''
        else:
            ConfigFlags.Output.HITSFileName  = runArgs.outputHITSFile
    if hasattr(runArgs, "outputEVNT_TRFile"):
        # Three common cases
        # 1b) Write TrackRecords for Cavern background
        # 1c) Write TrackRecords for Stopped particles
        # 3b) CosmicGenerator
        ConfigFlags.Output.EVNT_TRFileName = runArgs.outputEVNT_TRFile
        if hasattr(runArgs,"trackRecordType") and runArgs.trackRecordType=="stopped":
            # Case 1c)
            log.error('Stopped Particle simulation not supported yet!')
        elif ConfigFlags.Beam.Type == 'cosmics':
            # Case 3b)
            pass
        else:
            #Case 1b) Cavern Background
            ConfigFlags.Detector.GeometryCavern = True # simulate the cavern
            ConfigFlags.Sim.CalibrationRun = "Off"
            ConfigFlags.Sim.CavernBG = "Write"
    if not (hasattr(runArgs, 'outputHITSFile') or hasattr(runArgs, "outputEVNT_TRFile")):
        raise RuntimeError('No outputHITSFile or outputEVNT_TRFile defined')

    if hasattr(runArgs, 'DataRunNumber'):
        ConfigFlags.Input.RunNumber = [runArgs.DataRunNumber]
        ConfigFlags.Input.OverrideRunNumber = True
        ConfigFlags.Input.LumiBlockNumber = [1] # dummy value

    if hasattr(runArgs, 'physicsList'):
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

    # Configure main services and input reading (if required)
    if ConfigFlags.Input.Files == '':
        # Cases 3a, 3b
        from AthenaConfiguration.MainServicesConfig import MainEvgenServicesCfg
        cfg = MainEvgenServicesCfg(ConfigFlags)
        from AthenaKernel.EventIdOverrideConfig import EvtIdModifierSvcCfg
        cfg.merge(EvtIdModifierSvcCfg(ConfigFlags))
        if ConfigFlags.Beam.Type == 'cosmics':
            # Case 3b: Configure the cosmic Generator
            from CosmicGenerator.CosmicGeneratorConfig import CosmicGeneratorCfg
            cfg.merge(CosmicGeneratorCfg(ConfigFlags))
        else:
            # Case 3a: Configure ParticleGun
            log.error("On-the-fly generation other than with CosmicGenerator is not supported yet!")
            pass
    else:
        # Cases 1, 2a, 2b, 2c
        from AthenaConfiguration.MainServicesConfig import MainServicesCfg
        cfg = MainServicesCfg(ConfigFlags)
        from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
        cfg.merge(PoolReadCfg(ConfigFlags))
        if ConfigFlags.Sim.ReadTR:
            # Cases 2a, 2b, 2c
            from TrackRecordGenerator.TrackRecordGeneratorConfigNew import Input_TrackRecordGeneratorCfg
            cfg.merge(Input_TrackRecordGeneratorCfg(ConfigFlags))

    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolWriteCfg(ConfigFlags))

    # add BeamEffectsAlg
    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    cfg.merge(BeamEffectsAlgCfg(ConfigFlags))

    #add the G4AtlasAlg
    from G4AtlasAlg.G4AtlasAlgConfigNew import G4AtlasAlgCfg
    cfg.merge(G4AtlasAlgCfg(ConfigFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from SimuJobTransforms.SimOutputConfig import getStreamHITS_ItemList
    cfg.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=getStreamHITS_ItemList(ConfigFlags), disableEventTag=True) )
    cfg.getEventAlgo("OutputStreamHITS").AcceptAlgs=['G4AtlasAlg']

    if len(ConfigFlags.Output.EVNT_TRFileName)>0:
        from SimuJobTransforms.SimOutputConfig import getStreamEVNT_TR_ItemList
        cfg.merge( OutputStreamCfg(ConfigFlags,"EVNT_TR", ItemList=getStreamEVNT_TR_ItemList(ConfigFlags), disableEventTag=True) )
        cfg.getEventAlgo("OutputStreamEVNT_TR").AcceptAlgs=['G4AtlasAlg']

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
    log.info("Run G4AtlasAlg in " + str(time.time()-tic) + " seconds")

    sys.exit(not sc.isSuccess())
