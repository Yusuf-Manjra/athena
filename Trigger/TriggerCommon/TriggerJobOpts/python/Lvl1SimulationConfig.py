# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

## @brief this function sets up the top L1 simulation sequence 
##
## it covers the two cases of running L1 in the MC simulation and for rerunning on data 

# Note on the New JO migration
#    from AthenaCommon.ComponentAccumulator import ComponentAccumulator
#    acc = ComponentAccumulator()
# all tools then should be added to the acc, cond folders as well.
# L1ConfigSvc CA has to be imported and merged
# at the end the sequence added to the CA

from AthenaCommon.Logging import logging
from AthenaCommon.CFElements import seqAND
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon import CfgMgr

def Lvl1SimulationSequence_Common( ConfigFlags ):
    """ 
    Configure L1 simulation for Athena MT data jobs on MC or data (data only with rerun L1)
    """

    log = logging.getLogger('TriggerJobOpts.Lvl1SimulationConfig')

    if not ConfigFlags.Input.isMC:
        from AthenaCommon.DetFlags import DetFlags
        DetFlags.detdescr.ALFA_setOff()

    ##################################################
    # Calo rerun on data
    ##################################################
    l1CaloSimSeq = seqAND('L1CaloSimSeq',[])
    if ConfigFlags.Trigger.enableL1CaloLegacy:
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper

        if not ConfigFlags.Input.isMC:
            from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
            CAtoGlobalWrapper(LVL1CaloRun2ReadBSCfg, ConfigFlags)

        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMSim
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMJetSim
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMEnergySim
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMX
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetCMX
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyCMX
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__RoIROD


        l1CaloSimSeq = seqAND('L1CaloLegacySimSeq',[
        ])

        if ConfigFlags.Input.isMC:
            from TrigT1CaloSim.TrigT1CaloSimRun2Config import Run2TriggerTowerMaker
            caloTowerMaker              = Run2TriggerTowerMaker("Run2TriggerTowerMaker25ns")
            caloTowerMaker.ZeroSuppress = True
            caloTowerMaker.CellType     = 3
            l1CaloSimSeq += caloTowerMaker
            from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2CPMTowerMaker
            l1CaloSimSeq += LVL1__Run2CPMTowerMaker( 'CPMTowerMaker')
            from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2JetElementMaker
            l1CaloSimSeq += LVL1__Run2JetElementMaker( 'JetElementMaker')

            from IOVDbSvc.CondDB import conddb
            L1CaloFolderList = []
            L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib"]
            L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Conditions/DisabledTowers"]
            L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels"]
            L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"]
    
            for l1calofolder in L1CaloFolderList:
                conddb.addFolder( "TRIGGER_OFL", l1calofolder )

        l1CaloSimSeq += [
          LVL1__CPMSim( 'CPMSim' ) ,
          LVL1__JEMJetSim( 'JEMJetSim' ) ,
          LVL1__JEMEnergySim( 'JEMEnergySim' ) ,
          LVL1__CPCMX( 'CPCMX' ) ,
          LVL1__JetCMX( 'JetCMX' ),
          LVL1__EnergyCMX( 'EnergyCMX' ) ,
          LVL1__RoIROD( 'RoIROD' ),
        ]

        if ConfigFlags.Input.isMC:
            from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
            from TrigT1ZDC.TrigT1ZDCConf import LVL1__TrigT1ZDC
            l1CaloSimSeq += [
              LVL1__TrigT1MBTS(UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON),
              LVL1__TrigT1ZDC(UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON)
            ]

    if ConfigFlags.Trigger.enableL1CaloPhase1:
        #Adding the floating point simulation for now. 
        from TrigT1CaloFexPerf.Rel22L1PerfSequence import setupRun3L1CaloPerfSequence
        setupRun3L1CaloPerfSequence(skipCTPEmulation=True, sequence = l1CaloSimSeq)

        from TrigT1CaloFexPerf.L1PerfControlFlags import L1Phase1PerfFlags as simflags
        # Here we have to add the SuperCell Emulation when running on Run 2 data
        if ConfigFlags.Input.isMC:
          SCellType = "SCell"
        else:
          SCellType = simflags.Calo.SCellType()
        log.info("Using supercell container %s", SCellType )
        l1CaloSimSeq += CfgMgr.LVL1__eFEXDriver('eFEXDriver',
            SCell=SCellType )
        l1CaloSimSeq.eFEXDriver.eSuperCellTowerMapperTool.SCell=SCellType
        l1CaloSimSeq.eFEXDriver.eFEXSysSimTool.SCell=SCellType

    ##################################################
    # Muons
    ##################################################

    from TriggerJobOpts.Lvl1MuonSimulationConfigOldStyle import Lvl1MuonSimulationSequence
    l1MuonSimSeq = Lvl1MuonSimulationSequence(ConfigFlags)
    isMUCTPIOutputProvided = True

    ##################################################
    # L1 Topo 
    ##################################################

    from AthenaCommon.AppMgr import ToolSvc
    l1Phase1TopoSimSeq = None
    l1LegacyTopoSimSeq = None
    from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
    l1Phase1TopoSimSeq = L1TopoSimulation('L1Phase1TopoSimulation')
    l1LegacyTopoSimSeq = L1TopoSimulation('L1LegacyTopoSimulation')
    l1LegacyTopoSimSeq.InputDumpFile = 'inputdump_legacy.txt'
    l1Phase1TopoSimSeq.InputDumpFile = 'inputdump_phase1.txt'
    # Generic flag can be put for enabling l1topo input file
    EnableInputDump = False
    l1LegacyTopoSimSeq.EnableInputDump = EnableInputDump
    l1Phase1TopoSimSeq.EnableInputDump = EnableInputDump
    l1Phase1TopoSimSeq.IsLegacyTopo = False
    l1LegacyTopoSimSeq.IsLegacyTopo = True
    l1Phase1TopoSimSeq.MonHistBaseDir = 'L1/L1Phase1TopoAlgorithms'
    l1LegacyTopoSimSeq.MonHistBaseDir = 'L1/L1LegacyTopoAlgorithms'
    l1Phase1TopoSimSeq.MuonInputProvider.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON
    l1LegacyTopoSimSeq.MuonInputProvider.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON

    # Calo inputs
    if ConfigFlags.Trigger.enableL1CaloPhase1:
        l1Phase1TopoSimSeq.EMTAUInputProvider = 'LVL1::EMTauInputProviderFEX/EMTauInputProviderFEX'
        #TThis can be implemented when appropriate EDM available.
        #l1Phase1TopoSimSeq.JetInputProvider = 'LVL1::JetInputProviderFEX/JetInputProviderFEX'

    # Muon inputs
    from L1TopoSimulation.L1TopoSimulationConfig import MuonInputProviderLegacy
    ToolSvc += MuonInputProviderLegacy('MuonInputProviderLegacy')    
    ToolSvc.MuonInputProviderLegacy.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON

    # TODO: the legacy simulation should not need to deal with muon inputs
    l1Phase1TopoSimSeq.MuonInputProvider.ROIBResultLocation = "" #disable input from RoIBResult
    l1LegacyTopoSimSeq.MuonInputProvider.ROIBResultLocation = "" #disable input from RoIBResult
    ToolSvc.MuonInputProviderLegacy.ROIBResultLocation = "" #disable input from RoIBResult

    if ConfigFlags.Trigger.enableL1MuonPhase1: 
        from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1Tool
        ToolSvc += L1MuctpiPhase1Tool("MUCTPI_AthTool")
        l1Phase1TopoSimSeq.MuonInputProvider.MuctpiSimTool = ToolSvc.MUCTPI_AthTool
        l1LegacyTopoSimSeq.MuonInputProvider.MuctpiSimTool = ToolSvc.MUCTPI_AthTool
        from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
        l1Phase1TopoSimSeq.MuonInputProvider.RecRpcRoiTool = getRun3RPCRecRoiTool(useRun3Config=True)
        l1Phase1TopoSimSeq.MuonInputProvider.RecTgcRoiTool = getRun3TGCRecRoiTool(useRun3Config=True)
        l1LegacyTopoSimSeq.MuonInputProvider.RecRpcRoiTool = getRun3RPCRecRoiTool(useRun3Config=True)
        l1LegacyTopoSimSeq.MuonInputProvider.RecTgcRoiTool = getRun3TGCRecRoiTool(useRun3Config=True)
        l1Phase1TopoSimSeq.MuonInputProvider.MuonROILocation = ""
        l1LegacyTopoSimSeq.MuonInputProvider.MuonROILocation = ""
        l1Phase1TopoSimSeq.MuonInputProvider.MuonEncoding = 1
        l1LegacyTopoSimSeq.MuonInputProvider.MuonEncoding = 1
    else:
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool
        ToolSvc += L1MuctpiTool("L1MuctpiTool")
        ToolSvc.L1MuctpiTool.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc
        ToolSvc.MuonInputProviderLegacy.MuctpiSimTool = ToolSvc.L1MuctpiTool
        ToolSvc.MuonInputProviderLegacy.locationMuCTPItoL1Topo = ""
        # enable the reduced (coarse) granularity topo simulation
        # currently only for MC
        # apparently not needed for phase1
        if not ConfigFlags.Input.isMC:
            ToolSvc.MuonInputProviderLegacy.MuonEncoding = 1
        else:
            ToolSvc.MuonInputProviderLegacy.MuonEncoding = 0

    if not ConfigFlags.Trigger.enableL1MuonPhase1:
        l1Phase1TopoSimSeq.MuonInputProvider = ToolSvc.MuonInputProviderLegacy
        l1LegacyTopoSimSeq.MuonInputProvider = ToolSvc.MuonInputProviderLegacy

    # TODO: at the moment, both simulation are running but they should be configured based on the phase1 flags (ATR-23319)
    isL1TopoLegacyOutputProvided = False
    if ConfigFlags.Trigger.enableL1CaloLegacy:
        isL1TopoLegacyOutputProvided = True
    isL1TopoOutputProvided = True
    if ConfigFlags.Trigger.enableL1MuonPhase1 or ConfigFlags.Trigger.enableL1CaloPhase1:
        isL1TopoOutputProvided = True

    from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInReco
    ctp             = CTPSimulationInReco("CTPSimulation")
    ctp.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON
    ctp.TrigConfigSvc = svcMgr.LVL1ConfigSvc
    ctp.DoL1CaloLegacy = ConfigFlags.Trigger.enableL1CaloLegacy # to en/disable all L1CaloLegacy treatment (Mult and Topo)
    # muon input
    if not isMUCTPIOutputProvided:
        ctp.MuctpiInput = ""
    # topo input
    ctp.LegacyTopoInput = "L1TopoLegacyToCTPLocation"
    ctp.TopoInput = "L1TopoToCTPLocation"
    if not isL1TopoLegacyOutputProvided:
        ctp.LegacyTopoInput = ""
    if not isL1TopoOutputProvided:
        ctp.TopoInput = ""
    if not ConfigFlags.Trigger.enableL1MuonPhase1: # Run 2 simulation of MUCTPI sends a slightly different format to the CTP
        ctp.MuonMultiplicityRun2Format = True
    ctp.jFexJetInput = ""
    ctp.jFexLJetInput = ""
    ctp.gFexJetInput = ""
    ctp.gFexMETPufitInput = ""
    ctp.gFexMETRhoInput = ""
    ctp.gFexMETJwoJInput = ""
    ctp.eFexClusterInput = ""
    ctp.eFexTauInput = ""
    ctpSimSeq = seqAND("CTPSimSeq", [ctp])

    if ConfigFlags.Trigger.enableL1CaloLegacy or not ConfigFlags.Trigger.enableL1MuonPhase1:
        from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilder
        roib = RoIBuilder("RoIBuilder")
        roib.DoCalo = ConfigFlags.Trigger.enableL1CaloLegacy
        roib.DoMuon = not ConfigFlags.Trigger.enableL1MuonPhase1
        ctpSimSeq += [roib]

    ##################################################
    # Combination of all parts
    ##################################################
    if l1Phase1TopoSimSeq and l1LegacyTopoSimSeq:
      l1SimSeq = seqAND("L1SimSeq", [l1CaloSimSeq, l1MuonSimSeq, l1LegacyTopoSimSeq, l1Phase1TopoSimSeq, ctpSimSeq] )
    elif not l1Phase1TopoSimSeq and l1LegacyTopoSimSeq:
      l1SimSeq = seqAND("L1SimSeq", [l1CaloSimSeq, l1MuonSimSeq, l1LegacyTopoSimSeq, ctpSimSeq] )
    elif l1Phase1TopoSimSeq and not l1LegacyTopoSimSeq:
      l1SimSeq = seqAND("L1SimSeq", [l1CaloSimSeq, l1MuonSimSeq, l1Phase1TopoSimSeq, ctpSimSeq] )
    else:
      l1SimSeq = seqAND("L1SimSeq", [l1CaloSimSeq, l1MuonSimSeq, ctpSimSeq] )

    return l1SimSeq


def Lvl1SimulationSequence( ConfigFlags ):
    """ 
    Configure L1 simulation for Athena MT jobs
    """

    log = logging.getLogger('TriggerJobOpts.L1Simulation')
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from TrigConfigSvc.TrigConfigSvcCfg import getL1ConfigSvc

    # this configuration of the LVL1ConfigSvc is only temporary
    TriggerFlags.readLVL1configFromXML = True
    TriggerFlags.outputLVL1configFile = None
    svcMgr += conf2toConfigurable(getL1ConfigSvc(ConfigFlags))

    log.info("UseNewConfig = %s", ConfigFlags.Trigger.readLVL1FromJSON)
    l1SimSeq = Lvl1SimulationSequence_Common( ConfigFlags )

    return l1SimSeq



def Lvl1SimulationMCCfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from AthenaCommon.CFElements import seqAND
    acc.addSequence(seqAND('L1SimSeq'), parentName='AthAlgSeq')
    
    acc.addSequence(seqAND('L1CaloLegacySimSeq'), parentName='L1SimSeq')
    from TrigT1CaloSim.TrigT1CaloSimRun2Config import L1LegacyCaloSimMCCfg
    acc.merge(L1LegacyCaloSimMCCfg(flags), sequenceName='L1CaloLegacySimSeq')

    acc.addSequence(seqAND('L1MuonLegacySimSeq'), parentName='L1SimSeq')
    from TriggerJobOpts.Lvl1MuonSimulationConfig import Lvl1MCMuonSimulationCfg
    acc.merge(Lvl1MCMuonSimulationCfg(flags), sequenceName='L1MuonLegacySimSeq')

    acc.addSequence(seqAND('L1LegacyTopoSimSeq'), parentName='L1SimSeq')
    from L1TopoSimulation.L1TopoSimulationConfig import L1LegacyTopoSimulationMCCfg
    acc.merge(L1LegacyTopoSimulationMCCfg(flags), sequenceName='L1LegacyTopoSimSeq')
    
    acc.addSequence(seqAND('L1TopoSimSeq'), parentName='L1SimSeq')
    from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulationMCCfg
    acc.merge(L1TopoSimulationMCCfg(flags), sequenceName='L1TopoSimSeq')
    
    acc.addSequence(seqAND('L1CTPSimSeq'), parentName='L1SimSeq')
    from TrigT1CTP.CTPSimulationConfig import CTPMCSimulationCfg
    acc.merge(CTPMCSimulationCfg(flags), sequenceName="L1CTPSimSeq")


    return acc

if __name__ == '__main__':    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    flags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000001.pool.root.1']
    flags.Common.isOnline=False
    flags.Exec.MaxEvents=25
    flags.Concurrency.NumThreads = 1
    flags.Concurrency.NumConcurrentEvents=1
    flags.Scheduler.ShowDataDeps=True
    flags.Scheduler.CheckDependencies=True
    flags.Scheduler.ShowDataFlow=True

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))

    acc.merge(Lvl1SimulationMCCfg(flags))
    from AthenaCommon.Constants import DEBUG
    acc.getEventAlgo("CTPSimulation").OutputLevel=DEBUG  # noqa: ATL900

    acc.printConfig(withDetails=True, summariseProps=True, printDefaults=True)
    with open("L1Sim.pkl", "wb") as p:
        acc.store(p)
        p.close()

    status = acc.run()
    
    if status.isFailure():
        import sys
        sys.exit(1)
