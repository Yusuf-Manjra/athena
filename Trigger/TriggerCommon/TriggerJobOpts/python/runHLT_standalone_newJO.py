#!/usr/bin/env python

#
#  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.Logging import logging
log = logging.getLogger('runHLT_standalone_newJO')

from AthenaConfiguration.AllConfigFlags import ConfigFlags, initConfigFlags
from AthenaConfiguration.ComponentAccumulator import CompFactory
from AthenaConfiguration.Enums import Format
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

# Make sure nobody uses deprecated global ConfigFlags
del ConfigFlags

flags = initConfigFlags()

# Output configuration - currently testing offline workflow
flags.Trigger.writeBS = False
flags.Trigger.EDMVersion = 3
flags.Output.doWriteRDO = True
flags.Output.RDOFileName = 'RDO_TRIG.pool.root'

flags.Trigger.doHLT = True
flags.Trigger.CostMonitoring.doCostMonitoring = True
flags.Trigger.doRuntimeNaviVal = True
flags.Scheduler.CheckDependencies = True
flags.Scheduler.ShowDataDeps = True
flags.Scheduler.ShowDataFlow = True
flags.Scheduler.ShowControlFlow = True
flags.Scheduler.EnableVerboseViews = True
from CaloClusterCorrection.constants \
     import CALOCORR_JO, CALOCORR_POOL
flags.Calo.ClusterCorrection.defaultSource = [CALOCORR_POOL, CALOCORR_JO] # temporary, until a complete support for cool is present

flags.Exec.MaxEvents = 50
# TODO this two should be resolved in a smarter way (i.e. required passing the tag from the driver test, however now, parsing of string with - fails)
flags.Common.isOnline = lambda f: not f.Input.isMC
flags.Common.MsgSourceLength=70
flags.Trigger.doLVL1=True # run L1 sim also on data
flags.Trigger.enableL1CaloPhase1=False
flags.Trigger.enableL1CaloLegacy=True
flags.Concurrency.NumThreads = 1

# Disable some forward detetors
flags.Detector.GeometryALFA = False
flags.Detector.GeometryFwdRegion = False
flags.Detector.GeometryLucid = False

# disable Prescales for initial dev phase to allow only few chains in the menu
flags.Trigger.disableCPS=True

# select chains, as in runHLT_standalone
flags.addFlag("Trigger.enabledSignatures",[])  
flags.addFlag("Trigger.disabledSignatures",[]) 
flags.addFlag("Trigger.selectChains",[])       
flags.addFlag("Trigger.disableChains",[]) 


flags.Trigger.enabledSignatures = ['Muon', 'Photon','Electron']
#flags.Trigger.selectChains =  ['HLT_mu4_L1MU3V','HLT_mu8_L1MU5VF','HLT_2mu6_L12MU5VF', 'HLT_mu24_mu6_L1MU14FCH','HLT_mu24_mu6_probe_L1MU14FCH'] #, 'HLT_mu4_mu6_L12MU3V']

#flags.Trigger.disableChains=["HLT_2mu4_l2io_invmDimu_L1BPH-2M9-0DR15-2MU3VF", "HLT_2mu4_l2io_invmDimu_L1BPH-2M9-0DR15-2MU3V", "HLT_2mu6_l2io_invmDimu_L1BPH-2M9-2DR15-2MU5VF"]
# exclude jets for now, since their MenuSeuqnece Structure needs more work to migrate

#--------------#
#Leave commented lines for tests, since this is under development
#from AthenaCommon.Constants import DEBUG
#flags.Exec.OutputLevel=DEBUG
#flags.Trigger.triggerMenuModifier=[ 'emptyMenu','HLT_mu8_L1MU5VF']
#flags.Trigger.selectChains = [ 'HLT_mu8_L1MU5VF']#'HLT_j0_perf_L1RD0_FILLED']#'HLT_mu26_ivarmedium_mu6_l2io_probe_L1MU14FCH'] #'HLT_mu0_muoncalib_L1MU14FCH', 'HLT_mu6_L1MU5VF','HLT_mu6_msonly_L1MU5VF'] #'HLT_mu0_muoncalib_L1MU14FCH',#HLT_mu6_L1MU5VF
#--------------#

flags.InDet.useSctDCS = False
flags.InDet.usePixelDCS = False

# command line handling
# options that are defined in: AthConfigFlags are handled here
# they override values from above
parser = flags.getArgumentParser()
args = flags.fillFromArgs(parser=parser)
log.info("Command line arguments:")
import sys
log.info(" ".join(sys.argv))

#Since isOnline is set by determining if input file is MC
#can only set these once we've read in the input file (usually from command line)
if flags.Common.isOnline:
  flags.GeoModel.AtlasVersion = flags.Trigger.OnlineGeoTag
  flags.IOVDb.GlobalTag = flags.Trigger.OnlineCondTag
#otherwise, read these from file metadata (e.g. for MC)

flags.lock()
flags.dump()
# Enable when debugging deduplication issues
# ComponentAccumulator.debugMode = "trackCA trackEventAlog ... and so on"
log.setLevel(logging.DEBUG)

acc = MainServicesCfg(flags)
acc.getService('AvalancheSchedulerSvc').VerboseSubSlots = True

# this delcares to the scheduler that EventInfo object comes from the input
loadFromSG = [('xAOD::EventInfo', 'StoreGateSvc+EventInfo')]

if flags.Input.Format is Format.BS:
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge(ByteStreamReadCfg(flags))
else:
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))


from TriggerJobOpts.TriggerHistSvcConfig import TriggerHistSvcConfig
acc.merge(TriggerHistSvcConfig(flags))



from TriggerMenuMT.HLT.Config.GenerateMenuMT_newJO import generateMenuMT
from TriggerJobOpts.TriggerConfig import triggerRunCfg
menu = triggerRunCfg(flags, menu=generateMenuMT)
# uncomment to obtain printout of menu (and associated components)
# menu.printConfig(withDetails=True, summariseProps=True)
acc.merge(menu)

from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
acc.merge(LumiBlockMuWriterCfg(flags), sequenceName="HLTBeginSeq")

if flags.Trigger.doTransientByteStream and flags.Trigger.doCalo:
    from TriggerJobOpts.TriggerTransBSConfig import triggerTransBSCfg_Calo
    acc.merge(triggerTransBSCfg_Calo(flags), sequenceName="HLTBeginSeq")

if flags.Input.isMC and flags.Trigger.doMuon:
    loadFromSG += [( 'RpcPadContainer' , 'StoreGateSvc+RPCPAD' ), ( 'TgcRdoContainer' , 'StoreGateSvc+TGCRDO' )]

if flags.Trigger.doLVL1:
    from TriggerJobOpts.Lvl1SimulationConfig import Lvl1SimulationCfg
    acc.merge(Lvl1SimulationCfg(flags), sequenceName="HLTBeginSeq")

acc.addEventAlgo(CompFactory.SGInputLoader(Load=loadFromSG), sequenceName="AthAlgSeq")

#track overlay needs this to ensure that the collections are copied correctly (due to the hardcoding of the name in the converters)
if flags.Overlay.doTrackOverlay:
    from TrkEventCnvTools.TrkEventCnvToolsConfigCA import TrkEventCnvSuperToolCfg
    acc.merge(TrkEventCnvSuperToolCfg(flags))

if log.getEffectiveLevel() <= logging.DEBUG:
    acc.printConfig(withDetails=False, summariseProps=True, printDefaults=True)

if flags.Common.isOnline:
  from TrigOnlineMonitor.TrigOnlineMonitorConfig import trigOpMonitorCfg
  acc.merge( trigOpMonitorCfg(flags) )

log.info("Running ...")
status = acc.run()
if status.isFailure():
  import sys
  sys.exit(1)
