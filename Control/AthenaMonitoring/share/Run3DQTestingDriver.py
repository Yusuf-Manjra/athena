#!/usr/bin/env python
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file DQTestingDriver.py
@author C. D. Burton
@author P. Onyisi
@date 2019-06-20
@brief Driver script to run DQ with new-style configuration on an ESD/AOD
'''

if __name__=='__main__':
    import sys
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--preExec', help='Code to execute before locking configs')
    parser.add_argument('--postExec', help='Code to execute after setup')
    parser.add_argument('--dqOffByDefault', action='store_true',
                        help='Set all DQ steering flags to False, user must then switch them on again explicitly')
    parser.add_argument('--maxEvents', type=int,
                        help='Maximum number of events to process')
    parser.add_argument('--loglevel', type=int, default=3,
                        help='Verbosity level')
    parser.add_argument('flags', nargs='*', help='Config flag overrides')
    args = parser.parse_args()

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import *
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.AutoConfigFlags import GetFileMD
    
    ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q431/21.0/myESD.pool.root']
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'
    if args.dqOffByDefault:
        from AthenaMonitoring.DQConfigFlags import allSteeringFlagsOff
        allSteeringFlagsOff()
    ConfigFlags.fillFromArgs(args.flags)
    isReadingRaw = (GetFileMD(ConfigFlags.Input.Files).get('file_type', 'POOL') == 'BS')
    if isReadingRaw:
        if ConfigFlags.DQ.Environment not in ('tier0', 'tier0Raw', 'online'):
            log.warning('Reading RAW file, but DQ.Environment set to %s',
                        ConfigFlags.DQ.Environment)
            log.warning('Will proceed but best guess is this is an error')
        log.info('Will schedule reconstruction, as best we know')
    else:
        if ConfigFlags.DQ.Environment in ('tier0', 'tier0Raw', 'online'):
            log.warning('Reading POOL file, but DQ.Environment set to %s',
                        ConfigFlags.DQ.Environment)
            log.warning('Will proceed but best guess is this is an error')

    if args.preExec:
        # bring things into scope
        from AthenaMonitoring.DQConfigFlags import allSteeringFlagsOff
        log.info('Executing preExec: %s', args.preExec)
        exec(args.preExec)

    if hasattr(ConfigFlags, "DQ") and hasattr(ConfigFlags.DQ, "Steering") and hasattr(ConfigFlags, "Detector"):
        if hasattr(ConfigFlags.DQ.Steering, "InDet"):
            if ((ConfigFlags.DQ.Steering.InDet, "doAlignMon") and ConfigFlags.DQ.Steering.InDet.doAlignMon) or \
               ((ConfigFlags.DQ.Steering.InDet, "doGlobalMon") and ConfigFlags.DQ.Steering.InDet.doGlobalMon) or \
               ((ConfigFlags.DQ.Steering.InDet, "doPerfMon") and ConfigFlags.DQ.Steering.InDet.doPerfMon):
                ConfigFlags.Detector.GeometryID = True
        if hasattr(ConfigFlags.DQ.Steering, "doPixelMon") and ConfigFlags.DQ.Steering.doPixelMon:
            ConfigFlags.Detector.GeometryPixel = True
        if hasattr(ConfigFlags.DQ.Steering, "doSCTMon") and ConfigFlags.DQ.Steering.doSCTMon:
            ConfigFlags.Detector.GeometrySCT = True
        if hasattr(ConfigFlags.DQ.Steering, "doTRTMon") and ConfigFlags.DQ.Steering.doTRTMon:
            ConfigFlags.Detector.GeometryTRT = True
            
    log.info('FINAL CONFIG FLAGS SETTINGS FOLLOW')
    ConfigFlags.dump()
        
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg, MainServicesThreadedCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    if ConfigFlags.Concurrency.NumThreads == 0:
        cfg = MainServicesSerialCfg()
    else:
        cfg = MainServicesThreadedCfg(ConfigFlags)
    if isReadingRaw:
        # attempt to start setting up reco ...
        from CaloRec.CaloRecoConfig import CaloRecoCfg
        cfg.merge(CaloRecoCfg(ConfigFlags))
    else:
        cfg.merge(PoolReadCfg(ConfigFlags))

    # load DQ
    from AthenaMonitoring.AthenaMonitoringCfg import AthenaMonitoringCfg
    dq = AthenaMonitoringCfg(ConfigFlags)
    cfg.merge(dq)

    # Force loading of conditions in MT mode
    if ConfigFlags.Concurrency.NumThreads > 0:
        from AthenaConfiguration.ComponentFactory import CompFactory
        if len([_ for _ in cfg._conditionsAlgs if _.name=="PixelDetectorElementCondAlg"]) > 0:
            beginseq = cfg.getSequence("AthBeginSeq")
            beginseq.Members.append(CompFactory.ForceIDConditionsAlg("ForceIDConditionsAlg"))
        if len([_ for _ in cfg._conditionsAlgs if _.name=="MuonAlignmentCondAlg"]) > 0:
            beginseq = cfg.getSequence("AthBeginSeq")
            beginseq.Members.append(CompFactory.ForceMSConditionsAlg("ForceMSConditionsAlg"))
    
    # any last things to do?
    if args.postExec:
        log.info('Executing postExec: %s', args.postExec)
        exec(args.postExec)

    # If you want to turn on more detailed messages ...
    # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    sc = cfg.run(args.maxEvents, args.loglevel)
    sys.exit(0 if sc.isSuccess() else 1)
