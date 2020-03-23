#!/usr/bin/env python
"""Overlay test helpers

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from argparse import ArgumentParser

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def JobOptsDumperCfg(flags):
    """Configure event loop for overlay"""
    JobOptsDumperAlg = CompFactory.JobOptsDumperAlg
    acc = ComponentAccumulator()
    acc.addEventAlgo(JobOptsDumperAlg(FileName="OverlayTestConfig.txt"))
    return acc


def TestMessageSvcCfg(flags):
    """MessageSvc for overlay"""
    MessageSvc = CompFactory.MessageSvc
    acc = ComponentAccumulator()
    acc.addService(MessageSvc(setError=["HepMcParticleLink"]))
    return acc


def CommonTestArgumentParser(prog):
    """Common overlay test argument parser"""
    parser = ArgumentParser(prog=prog)
    parser.add_argument("-d", "--data", default=False,
                        action="store_true", help="Run data overlay")
    parser.add_argument("-n", "--maxEvents", default=3, type=int,
                        help="The number of events to run. 0 skips execution")
    parser.add_argument("-t", "--threads", default=1, type=int,
                        help="The number of concurrent threads to run. 0 uses serial Athena.")
    parser.add_argument("-V", "--verboseAccumulators", default=False, action="store_true",
                        help="Print full details of the AlgSequence for each accumulator")
    parser.add_argument("-S", "--verboseStoreGate", default=False, action="store_true",
                        help="Dump the StoreGate(s) each event iteration")
    parser.add_argument("-o", "--output", default='', type=str,
                        help="Output RDO file")
    parser.add_argument("-s", "--outputSig", default='', type=str,
                        help="Output RDO_SGNL file")
    return parser


def setupOverlayTestDetectorFlags(configFlags, detectors):
    """Setup Overlay detector flags"""
    if not detectors or 'BCM' in detectors or 'ID' in detectors:
        configFlags.Detector.OverlayBCM = True
    if not detectors or 'DBM' in detectors or 'ID' in detectors:
        configFlags.Detector.OverlayDBM = True
    if not detectors or 'Pixel' in detectors or 'ID' in detectors:
        configFlags.Detector.OverlayPixel = True
    if not detectors or 'SCT' in detectors or 'ID' in detectors:
        configFlags.Detector.OverlaySCT = True
    if not detectors or 'TRT' in detectors or 'ID' in detectors:
        configFlags.Detector.OverlayTRT = True
    if not detectors or 'LAr' in detectors or 'Calo' in detectors or 'L1Calo' in detectors:
        configFlags.Detector.OverlayLAr = True
    if not detectors or 'Tile' in detectors or 'Calo' in detectors or 'L1Calo' in detectors:
        configFlags.Detector.OverlayTile = True
    if not detectors or 'L1Calo' in detectors:
        configFlags.Detector.OverlayL1Calo = not configFlags.Overlay.DataOverlay
    if not detectors or 'CSC' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlayCSC = True
    if not detectors or 'MDT' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlayMDT = True
    if not detectors or 'RPC' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlayRPC = True
    if not detectors or 'TGC' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlayTGC = True
    if not detectors or 'sTGC' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlaysTGC = True
    if not detectors or 'MM' in detectors or 'Muon' in detectors:
        configFlags.Detector.OverlayMM = True


def defaultTestFlags(configFlags, args):
    """Fill default overlay flags for testing"""
    configFlags.GeoModel.Align.Dynamic = False
    configFlags.Digitization.DoCaloNoise = False
    configFlags.Digitization.DoInnerDetectorNoise = False
    configFlags.Digitization.DoDigiTruth = False
    configFlags.LAr.OFCShapeFolder = "4samples1phase"
    configFlags.LAr.ROD.DoOFCPileupOptimization = True
    configFlags.LAr.ROD.nSamples = 4
    configFlags.LAr.ROD.NumberOfCollisions = 20
    configFlags.LAr.ROD.UseHighestGainAutoCorr = True
    configFlags.Tile.BestPhaseFromCOOL = False
    configFlags.Tile.correctTime = False
    configFlags.Tile.zeroAmplitudeWithoutDigits = False

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    if args.data:
        configFlags.Input.isMC = False  # TODO: this one should be autodetected
        configFlags.Input.Files = defaultTestFiles.HITS_DATA_OVERLAY
        configFlags.Input.SecondaryFiles = defaultTestFiles.RAW_BKG
        configFlags.Output.RDOFileName = "dataOverlayRDO.pool.root"
        configFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2016-12"
        configFlags.IOVDb.DatabaseInstance = "CONDBR2"
        configFlags.Overlay.DataOverlay = True
    else:
        configFlags.Input.Files = defaultTestFiles.RDO_BKG
        configFlags.Input.SecondaryFiles = defaultTestFiles.HITS
        configFlags.Output.RDOFileName = "mcOverlayRDO.pool.root"
        configFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-20"
        configFlags.Overlay.DataOverlay = False

    if args.output:
        if args.output == 'None':
            configFlags.Output.RDOFileName = ''
        else:
            configFlags.Output.RDOFileName = args.output

    if args.outputSig:
        configFlags.Output.RDO_SGNLFileName = args.outputSig

    setupOverlayTestDetectorFlags(configFlags, args.detectors if 'detectors' in args else None)


def postprocessAndLockFlags(configFlags, args):
    """Postprocess and lock config flags for overlay"""

    # Flags relating to multithreaded execution
    configFlags.Concurrency.NumThreads = args.threads
    if args.threads > 0:
        configFlags.Scheduler.ShowDataDeps = True
        configFlags.Scheduler.ShowDataFlow = True
        configFlags.Scheduler.ShowControlFlow = True
        configFlags.Concurrency.NumConcurrentEvents = args.threads

    configFlags.lock()


def printAndRun(accessor, configFlags, args):
    """Common debugging and execution for overlay tests"""
    # Dump config
    if args.verboseAccumulators:
        accessor.printConfig(withDetails=True)
    if args.verboseStoreGate:
        accessor.getService("StoreGateSvc").Dump = True
    configFlags.dump()

    # Execute and finish
    sc = accessor.run(maxEvents=args.maxEvents)

    # Dump config summary
    accessor.printConfig(withDetails=False)

    # Success should be 0
    return not sc.isSuccess()
