# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude


def fromRunArgs(runArgs):
    from AthenaCommon.Logging import logging
    log = logging.getLogger('RAWtoALL')
    log.info('****************** STARTING Reconstruction (RAWtoALL) *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    import time
    timeStart = time.time()

    log.info('**** executing ROOT6Setup')
    from PyUtils.Helpers import ROOT6Setup
    ROOT6Setup(batch=True)

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)
    from RecJobTransforms.RecoConfigFlags import recoRunArgsToFlags
    recoRunArgsToFlags(runArgs, ConfigFlags)

    # Autoconfigure enabled subdetectors
    if hasattr(runArgs, 'detectors'):
        detectors = runArgs.detectors
    else:
        detectors = None

    # TODO: event service?

    ## Inputs
    # BS
    inputsDRAW = [prop for prop in dir(runArgs) if prop.startswith('inputDRAW') and prop.endswith('File')]
    if hasattr(runArgs, 'inputBSFile'):
        if inputsDRAW:
            raise RuntimeError('Impossible to run RAWtoALL with input BS and DRAW files (one input type only!)')
        ConfigFlags.Input.Files = runArgs.inputBSFile
    if len(inputsDRAW) == 1:
        ConfigFlags.Input.Files = getattr(runArgs, inputsDRAW[0])
    elif len(inputsDRAW) > 1:
        raise RuntimeError('Impossible to run RAWtoALL with multiple input DRAW files (viz.: {0})'.format(inputsDRAW))

    # RDO
    if hasattr(runArgs, 'inputRDOFile'):
        ConfigFlags.Input.Files = runArgs.inputRDOFile
    if hasattr(runArgs, 'inputRDO_TRIGFile'):
        ConfigFlags.Input.Files = runArgs.inputRDO_TRIGFile
        # TODO: trigger setup

    # TODO: EVNT?

    ## Outputs
    if hasattr(runArgs, 'outputESDFile'):
        ConfigFlags.Output.ESDFileName = runArgs.outputESDFile
        log.info("---------- Configured ESD output")

    if hasattr(runArgs, 'outputAODFile'):
        ConfigFlags.Output.AODFileName = runArgs.outputAODFile
        log.info("---------- Configured AOD output")

    if hasattr(runArgs, 'outputHISTFile'):
        ConfigFlags.Output.HISTFileName = runArgs.outputHISTFile
        ConfigFlags.DQ.doMonitoring = True
        log.info("---------- Configured HIST output")

    if hasattr(runArgs, 'outputHIST_R2AFile'):
        ConfigFlags.Output.HISTFileName = runArgs.outputHIST_R2AFile
        ConfigFlags.DQ.doMonitoring = True
        log.info("---------- Configured HIST_R2A output")

    if hasattr(runArgs, 'outputDAOD_IDTIDEFile'):
        flagString = 'Output.DAOD_IDTIDEFileName'
        ConfigFlags.addFlag(flagString, runArgs.outputDAOD_IDTIDEFile)
        ConfigFlags.Output.doWriteDAOD = True
        log.info("---------- Configured DAOD_IDTIDE output")

    if hasattr(runArgs, 'outputDESDM_MCPFile'):
        flagString = 'Output.DESDM_MCPFileName'
        ConfigFlags.addFlag(flagString, runArgs.outputDESDM_MCPFile)
        ConfigFlags.Output.doWriteDAOD = True
        log.info("---------- Configured DESDM_MCP output")
    
    if hasattr(runArgs, 'outputDRAW_ZMUMUFile'):
        flagString = 'Output.DRAW_ZmumuFileName'
        ConfigFlags.addFlag(flagString, runArgs.outputDRAW_ZMUMUFile)
        ConfigFlags.Output.doWriteBS = True
        log.info("---------- Configured DRAW ZMUMU output")

    if hasattr(runArgs, 'outputDRAW_EGZFile'):
        flagString = 'Output.DRAW_EGZFileName'
        ConfigFlags.addFlag(flagString, runArgs.outputDRAW_EGZFile)
        ConfigFlags.Output.doWriteBS = True
        log.info("---------- Configured DRAW_EGZ output")

    if hasattr(runArgs, 'outputDAOD_L1CALO1File'):
        flagString = 'Output.DAOD_L1CALO1FileName'
        ConfigFlags.addFlag(flagString, runArgs.outputDAOD_L1CALO1File)
        ConfigFlags.Output.doWriteDAOD = True
        log.info("---------- Configured DAOD_L1CALO1 output")

    if hasattr(runArgs, 'outputDAOD_L1CALO2File'):
        flagString = 'Output.DAOD_L1CALO2FileName'
        ConfigFlags.addFlag(flagString, runArgs.outputDAOD_L1CALO2File)
        ConfigFlags.Output.doWriteDAOD = True
        log.info("---------- Configured DAOD_L1CALO2 output")

    if hasattr(runArgs, 'outputDESDM_PHOJETFile'):
        flagString = 'Output.DESDM_PHOJETFileName'
        ConfigFlags.addFlag(flagString, runArgs.outputDESDM_PHOJETFile)
        ConfigFlags.Output.doWriteDAOD = True
        log.info("---------- Configured DESDM_PHOJET output")

    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep=ProductionStep.Reconstruction

    # Setup detector flags
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorFlags
    setupDetectorFlags(ConfigFlags, detectors, use_metadata=True, toggle_geometry=True, keep_beampipe=True)
    # Print reco domain status
    from RecJobTransforms.RecoConfigFlags import printRecoFlags
    printRecoFlags(ConfigFlags)

    # TODO: DESD, DAOD and DRAW

    # outputHIST_R2AFile
    # outputTAG_COMMFile
    # outputTXT_FTKIPFile
    # outputNTUP_MUONCALIBFile
    # outputTXT_JIVEXMLTGZFile

    # Setup perfmon flags from runargs
    from PerfMonComps.PerfMonConfigHelpers import setPerfmonFlagsFromRunArgs
    setPerfmonFlagsFromRunArgs(ConfigFlags, runArgs)

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # Lock flags
    ConfigFlags.lock()

    log.info("Configuring according to flag values listed below")
    ConfigFlags.dump()

    # Main reconstruction steering
    from RecJobTransforms.RecoSteering import RecoSteering
    cfg = RecoSteering(ConfigFlags)

    # Performance DPDs 
    cfg.flagPerfmonDomain('PerfDPD')
    # IDTIDE
    for flag in [key for key in ConfigFlags._flagdict.keys() if ("Output.DAOD_IDTIDEFileName" in key)]:
        from DerivationFrameworkInDet.IDTIDE import IDTIDECfg
        cfg.merge(IDTIDECfg(ConfigFlags))
        log.info("---------- Configured IDTIDE perfDPD")

    # DESDM_MCP
    for flag in [key for key in ConfigFlags._flagdict.keys() if ("Output.DESDM_MCPFileName" in key)]:
        from PrimaryDPDMaker.DESDM_MCP import DESDM_MCPCfg
        cfg.merge(DESDM_MCPCfg(ConfigFlags))
        log.info("---------- Configured DESDM_MCP perfDPD")

    # DRAW ZMUMU
    for flag in [key for key in ConfigFlags._flagdict.keys() if ("Output.DRAW_ZmumuFileName" in key)]:
        from PrimaryDPDMaker.DRAW_ZMUMU import DRAW_ZmumuCfg
        cfg.merge(DRAW_ZmumuCfg(ConfigFlags))
        log.info("---------- Configured DRAW_ZMUMU perfDPD")

    #DRAW_EGZ
    for flag in [key for key in ConfigFlags._flagdict.keys() if ("Output.DRAW_EGZFileName" in key)]:
        from PrimaryDPDMaker.DRAW_EGZ import DRAW_EGZCfg
        cfg.merge(DRAW_EGZCfg(ConfigFlags))
        log.info("---------- Configured DRAW_EGZ perfDPD")

    # L1CALO1/2
    for flag in [key for key in ConfigFlags._flagdict.keys() if ("Output.DAOD_L1CALO1FileName" in key)]:
        from DerivationFrameworkL1Calo.L1CALO1 import L1CALO1Cfg
        cfg.merge(L1CALO1Cfg(ConfigFlags))
        log.info("---------- Configured L1CALO1 perfDPD")

    for flag in [key for key in ConfigFlags._flagdict.keys() if ("Output.DAOD_L1CALO2FileName" in key)]:
        from DerivationFrameworkL1Calo.L1CALO2 import L1CALO2Cfg
        cfg.merge(L1CALO2Cfg(ConfigFlags))
        log.info("---------- Configured L1CALO2 perfDPD")

    # DESDM PHOJET
    for flag in [key for key in ConfigFlags._flagdict.keys() if ("Output.DESDM_PHOJETFileName" in key)]:
        from PrimaryDPDMaker.DESDM_PHOJET import DESDM_PHOJETCfg
        cfg.merge(DESDM_PHOJETCfg(ConfigFlags))
        log.info("---------- Configured PHOJET perfDPD")

    # Special message service configuration
    from Digitization.DigitizationSteering import DigitizationMessageSvcCfg
    cfg.merge(DigitizationMessageSvcCfg(ConfigFlags))

    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)

    from AthenaConfiguration.Utils import setupLoggingLevels
    setupLoggingLevels(ConfigFlags, cfg)

    # Write some metadata into TagInfo
    from EventInfoMgt.TagInfoMgrConfig import TagInfoMgrCfg
    cfg.merge(
        TagInfoMgrCfg(
            ConfigFlags,
            tagValuePairs={
                "beam_type": ConfigFlags.Beam.Type.value,
                "beam_energy": str(ConfigFlags.Beam.Energy),
                "triggerStreamOfFile": ""
                if ConfigFlags.Input.isMC
                else ConfigFlags.Input.TriggerStream,
                "project_name": "IS_SIMULATION"
                if ConfigFlags.Input.isMC
                else ConfigFlags.Input.ProjectName,
                f"AtlasRelease_{runArgs.trfSubstepName}": ConfigFlags.Input.Release or "n/a",
            },
        )
    )

    # Write AMI tag into in-file metadata
    from PyUtils.AMITagHelperConfig import AMITagCfg
    cfg.merge(AMITagCfg(ConfigFlags, runArgs))

    # Print PerfMon domain information when running detailed monitoring
    if ConfigFlags.PerfMon.doFullMonMT:
        cfg.printPerfmonDomains()

    timeConfig = time.time()
    log.info("configured in %d seconds", timeConfig - timeStart)

    # Print sum information about AccumulatorCache performance
    from AthenaConfiguration.AccumulatorCache import AccumulatorDecorator
    AccumulatorDecorator.printStats() 

    # Run the final accumulator
    sc = cfg.run()
    timeFinal = time.time()
    log.info("Run RAWtoALL_skeleton in %d seconds (running %d seconds)", timeFinal - timeStart, timeFinal - timeConfig)

    import sys
    sys.exit(not sc.isSuccess())
