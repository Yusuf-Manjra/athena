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


def Lvl1SimulationSequence_Data( ConfigFlags ):
    """ 
    Configure L1 simulation for Athena MT data jobs with rerunning L1

    At the moment this sequence sets upt the simulation of all the L1 components
    Later we can think about partial simulation where we only simulation
    """

    log = logging.getLogger('TriggerJobOpts.L1Simulation.Data')

    from AthenaCommon.DetFlags import DetFlags
    DetFlags.detdescr.ALFA_setOff()

    ##################################################
    # Calo rerun on data
    ##################################################
    l1CaloSimDataSeq = seqAND('L1CaloSimDataSeq',[])
    if ConfigFlags.Trigger.enableL1CaloLegacy:
        log.info("Configuring legacy L1Calo simulation on data")
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
        from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
        CAtoGlobalWrapper(LVL1CaloRun2ReadBSCfg, ConfigFlags)

        l1CaloSimDataSeq += seqAND('L1CaloLegacySimSeq',[
            CfgMgr.LVL1__CPMSim( 'CPMSim' ),
            CfgMgr.LVL1__JEMJetSim( 'JEMJetSim' ),
            CfgMgr.LVL1__JEMEnergySim( 'JEMEnergySim' ),
            CfgMgr.LVL1__CPCMX( 'CPCMX' ),
            CfgMgr.LVL1__JetCMX( 'JetCMX' ),
            CfgMgr.LVL1__EnergyCMX( 'EnergyCMX' ),
            CfgMgr.LVL1__RoIROD( 'RoIROD' ),
        ])

    if ConfigFlags.Trigger.enableL1CaloPhase1:
        # Placeholder for phase-I L1Calo simulation
        log.info("Configuring Phase-I L1Calo simulation on data")

        #Adding the floating point simulation for now. 
        from TrigT1CaloFexPerf.Rel22L1PerfSequence import setupRun3L1CaloPerfSequence
        setupRun3L1CaloPerfSequence(skipCTPEmulation=True, sequence = l1CaloSimDataSeq)

        from TrigT1CaloFexPerf.L1PerfControlFlags import L1Phase1PerfFlags as simflags
        # Here we have to add the SuperCell Emulation when running on Run 2 data
        # add bitwise correct eFEX simulation (enable once the SC emulation is available)
        log.info("Using supercell container %s", simflags.Calo.SCellType() )
        l1CaloSimDataSeq += CfgMgr.LVL1__eFEXDriver('eFEXDriver',
                                                    SCell=simflags.Calo.SCellType() )
        l1CaloSimDataSeq.eFEXDriver.eSuperCellTowerMapperTool.SCell=simflags.Calo.SCellType()
        l1CaloSimDataSeq.eFEXDriver.eFEXSysSimTool.SCell=simflags.Calo.SCellType()
    ##################################################
    # Muons rerun on data
    ##################################################

    log.info("Configuring L1Muon simulation on data")
    from TriggerJobOpts.Lvl1MuonSimulationConfigOldStyle import Lvl1MuonSimulationSequence
    l1MuonSimDataSeq = Lvl1MuonSimulationSequence(ConfigFlags)
    isMUCTPIOutputProvided = True

    ##################################################
    # L1 Topo rerun on data
    ##################################################

    from AthenaCommon.AppMgr import ToolSvc
    l1TopoSimDataSeq = None
    from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
    l1TopoSimDataSeq = L1TopoSimulation()
    l1TopoSimDataSeq.MuonInputProvider.ROIBResultLocation = "" #disable input from RoIBResult
    
    if ConfigFlags.Trigger.enableL1MuonPhase1:
        from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1Tool
        ToolSvc += L1MuctpiPhase1Tool("MUCTPI_AthTool")
        l1TopoSimDataSeq.MuonInputProvider.MuctpiSimTool = ToolSvc.MUCTPI_AthTool
        from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
        l1TopoSimDataSeq.MuonInputProvider.RecRpcRoiTool = getRun3RPCRecRoiTool(useRun3Config=True)
        l1TopoSimDataSeq.MuonInputProvider.RecTgcRoiTool = getRun3TGCRecRoiTool(useRun3Config=True)
    else:
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool
        ToolSvc += L1MuctpiTool("L1MuctpiTool")
        ToolSvc.L1MuctpiTool.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc
        l1TopoSimDataSeq.MuonInputProvider.MuctpiSimTool = ToolSvc.L1MuctpiTool

    #TODO: is this meant to be final?
    from AthenaCommon.GlobalFlags  import globalflags
    if globalflags.DataSource()!='data':
        l1TopoSimDataSeq.MuonInputProvider.MuonEncoding = 1
    else:
        l1TopoSimDataSeq.MuonInputProvider.MuonEncoding = 0

    l1TopoSimDataSeq.MuonInputProvider.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON

    isL1TopoLegacyOutputProvided = False
    if ConfigFlags.Trigger.enableL1CaloLegacy:
        isL1TopoLegacyOutputProvided = False
        # TODO disable L1Topo sim not ready yet with json file
        l1TopoSimDataSeq = None
    isL1TopoOutputProvided = False
    if ConfigFlags.Trigger.enableL1MuonPhase1 or ConfigFlags.Trigger.enableL1CaloPhase1:
        isL1TopoOutputProvided = False
        # TODO disable L1Topo sim with Phase1, not ready yet
        l1TopoSimDataSeq = None

    ##################################################
    # CTP rerun on data
    ##################################################
    # Placeholder for phase-I L1Calo simulation
    log.info("Configuring CTP simulation on data")

    from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInReco
    ctp             = CTPSimulationInReco("CTPSimulation")
    ctp.DoLUCID     = False
    ctp.DoBCM       = False
    #ctp.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON  #TODO
    ctp.TrigConfigSvc = svcMgr.LVL1ConfigSvc
    ctp.DoL1CaloLegacy = ConfigFlags.Trigger.enableL1CaloLegacy # to en/disable all L1CaloLegacy treatment (Mult and Topo)
    # muon input
    if not isMUCTPIOutputProvided:
        ctp.MuctpiInput = ""
    # topo input
    if not isL1TopoLegacyOutputProvided:
        ctp.LegacyTopoInput = ""
    if not isL1TopoOutputProvided:
        ctp.TopoInput = ""
    ctp.jFexJetInput = ""
    ctp.jFexLJetInput = ""
    ctp.gFexJetInput = ""
    ctp.gFexMETPufitInput = ""
    ctp.gFexMETRhoInput = ""
    ctp.gFexMETJwoJInput = ""
    ctp.eFexClusterInput = ""
    ctp.eFexTauInput = ""
    ctpSimDataSeq = seqAND("CTPSimDataSeq", [ctp])

    if ConfigFlags.Trigger.enableL1CaloLegacy or not ConfigFlags.Trigger.enableL1MuonPhase1:
        from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilder
        roib = RoIBuilder("RoIBuilder")
        roib.DoCalo = ConfigFlags.Trigger.enableL1CaloLegacy
        roib.DoMuon = not ConfigFlags.Trigger.enableL1MuonPhase1
        ctpSimDataSeq += [roib]

    if l1TopoSimDataSeq:
      l1SimDataSeq = seqAND("L1SimDataSeq", [l1CaloSimDataSeq, l1MuonSimDataSeq, l1TopoSimDataSeq, ctpSimDataSeq] )
    else:
      l1SimDataSeq = seqAND("L1SimDataSeq", [l1CaloSimDataSeq, l1MuonSimDataSeq, ctpSimDataSeq] )
    return l1SimDataSeq



def Lvl1SimulationSequence_MC( ConfigFlags ):
    """ 
    Configure L1 simulation for Athena MT simulation jobs
    """
    log = logging.getLogger('TriggerJobOpts.L1Simulation')

    from AthenaCommon.AppMgr import ToolSvc

    ##################################################
    # Calo MC
    ##################################################

    if ConfigFlags.Trigger.enableL1CaloLegacy:
        from TrigT1CaloSim.TrigT1CaloSimRun2Config import Run2TriggerTowerMaker
        caloTowerMaker              = Run2TriggerTowerMaker("Run2TriggerTowerMaker25ns")
        caloTowerMaker.ExtraInputs   = ["LArTTL1Container#LArTTL1EM", "LArTTL1Container#LArTTL1HAD", "TileTTL1Container#TileTTL1Cnt" ]
        caloTowerMaker.ZeroSuppress = True
        caloTowerMaker.CellType     = 3

        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2CPMTowerMaker
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2JetElementMaker
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMSim
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMJetSim
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMEnergySim
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMX
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetCMX
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyCMX
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__RoIROD
        from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
        from TrigT1ZDC.TrigT1ZDCConf import LVL1__TrigT1ZDC

        l1CaloSim = seqAND('L1CaloLegacySimSeq',[
            caloTowerMaker,
            #LVL1__Run2CPMTowerMaker( 'CPMTowerMaker', ExtraInputs=["XYZ#1"], ExtraOutputs=["XYZ#2"]) ,
            LVL1__Run2CPMTowerMaker( 'CPMTowerMaker') ,
            LVL1__Run2JetElementMaker( 'JetElementMaker'),
            LVL1__CPMSim( 'CPMSim' ) ,
            LVL1__JEMJetSim( 'JEMJetSim' ) ,
            LVL1__JEMEnergySim( 'JEMEnergySim' ) ,
            LVL1__CPCMX( 'CPCMX' ) ,
            LVL1__JetCMX( 'JetCMX' ) ,
            LVL1__EnergyCMX( 'EnergyCMX' ) ,
            LVL1__RoIROD( 'RoIROD' ),
            LVL1__TrigT1MBTS(),
            LVL1__TrigT1ZDC()
        ])

        from IOVDbSvc.CondDB import conddb
        L1CaloFolderList = []
        L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib"]
        L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Conditions/DisabledTowers"]
        L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels"]
        L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"]

        for l1calofolder in L1CaloFolderList:
            conddb.addFolder( "TRIGGER_OFL", l1calofolder )

    if ConfigFlags.Trigger.enableL1CaloPhase1:
        #from AthenaCommon import CfgMgr
        
        #Adding the floating point simulation for now. 
        from TrigT1CaloFexPerf.Rel22L1PerfSequence import setupRun3L1CaloPerfSequence
        setupRun3L1CaloPerfSequence(skipCTPEmulation=True, sequence = l1CaloSim)

        l1CaloSim += CfgMgr.LVL1__eFEXDriver('MyeFEXDriver')

    ##################################################
    # Muons MC
    ##################################################

    log.info("Configuring L1Muon simulation on MC")
    from TriggerJobOpts.Lvl1MuonSimulationConfigOldStyle import Lvl1MuonSimulationSequence
    l1MuonSim = Lvl1MuonSimulationSequence(ConfigFlags)

    ##################################################
    # Topo MC
    ##################################################

    l1TopoSim = None
    from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
    l1TopoSim = L1TopoSimulation()
    l1TopoSim.MuonInputProvider.ROIBResultLocation = "" #disable input from RoIBResult
    if ConfigFlags.Trigger.enableL1CaloPhase1:
        l1TopoSim.EMTAUInputProvider = 'LVL1::EMTauInputProviderFEX/EMTauInputProviderFEX'
    if ConfigFlags.Trigger.enableL1MuonPhase1:
        from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1Tool
        ToolSvc += L1MuctpiPhase1Tool("MUCTPI_AthTool")
        l1TopoSim.MuonInputProvider.MuctpiSimTool = ToolSvc.MUCTPI_AthTool
        from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
        l1TopoSim.MuonInputProvider.RecRpcRoiTool = getRun3RPCRecRoiTool(useRun3Config=True)
        l1TopoSim.MuonInputProvider.RecTgcRoiTool = getRun3TGCRecRoiTool(useRun3Config=True)
    else:
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool
        ToolSvc += L1MuctpiTool("L1MuctpiTool")
        ToolSvc.L1MuctpiTool.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc
        l1TopoSim.MuonInputProvider.MuctpiSimTool = ToolSvc.L1MuctpiTool
    # enable the reduced (coarse) granularity topo simulation
    # currently only for MC
    from AthenaCommon.GlobalFlags  import globalflags
    if globalflags.DataSource()!='data':
        l1TopoSim.MuonInputProvider.MuonEncoding = 1
    else:
        l1TopoSim.MuonInputProvider.MuonEncoding = 0

    l1TopoSim.MuonInputProvider.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON

    ##################################################
    # CTP MC
    ##################################################
    from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInReco
    ctp             = CTPSimulationInReco("CTPSimulation")
    ctp.DoLUCID     = False
    ctp.DoBCM       = False
    ctp.DoL1CaloLegacy = ConfigFlags.Trigger.enableL1CaloLegacy # to en/disable all L1CaloLegacy treatment (Mult and Topo)
    ctp.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON
    ctp.TrigConfigSvc = svcMgr.LVL1ConfigSvc
    if not ConfigFlags.Trigger.enableL1MuonPhase1: # Run 2 simulation of MUCTPI sends a slightly different format to the CTP
        ctp.MuonMultiplicityRun2Format = True
    ctpSim      = seqAND("CTPSimSeq", [ctp])

    if ConfigFlags.Trigger.enableL1CaloLegacy or not ConfigFlags.Trigger.enableL1MuonPhase1:
        from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilder
        roib = RoIBuilder("RoIBuilder")
        roib.DoCalo = ConfigFlags.Trigger.enableL1CaloLegacy
        roib.DoMuon = not ConfigFlags.Trigger.enableL1MuonPhase1
        ctpSim += [roib]


    ##################################################
    # Combination of all parts
    ##################################################
    #l1Sim = seqAND("l1Sim", [caloTowerMaker] )
    if l1TopoSim:
      l1Sim = seqAND("L1SimSeq", [l1CaloSim, l1MuonSim, l1TopoSim, ctpSim] )
    else:
      l1Sim = seqAND("L1SimSeq", [l1CaloSim, l1MuonSim, ctpSim] )

    return l1Sim



def Lvl1SimulationSequence( ConfigFlags ):
    """ 
    Configure L1 simulation for Athena MT jobs
    """

    log = logging.getLogger('TriggerJobOpts.L1Simulation')
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from TrigConfigSvc.TrigConfigSvcCfg import getL1ConfigSvc, getL1TopoConfigSvc

    # this configuration of the LVL1ConfigSvc and L1TopoConfigSvc is only temporary
    TriggerFlags.readLVL1configFromXML = True
    TriggerFlags.outputLVL1configFile = None
    svcMgr += conf2toConfigurable(getL1ConfigSvc(ConfigFlags))
    svcMgr += conf2toConfigurable(getL1TopoConfigSvc(ConfigFlags))

    log.info("UseNewConfig = %s", ConfigFlags.Trigger.readLVL1FromJSON)
    if ConfigFlags.Input.isMC and not ConfigFlags.Common.isOnline:
        log.info("Setting up L1 simulation for MC")
        l1SimSeq = Lvl1SimulationSequence_MC( ConfigFlags )
    else:
        log.info("Setting up L1 simulation for rerunning L1 on data")
        l1SimSeq = Lvl1SimulationSequence_Data( ConfigFlags )

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

    from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulationMCCfg
    acc.merge(L1TopoSimulationMCCfg(flags), sequenceName='L1SimSeq')

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
