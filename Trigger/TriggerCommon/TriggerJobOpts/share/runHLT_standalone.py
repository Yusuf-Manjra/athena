
# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
################################################################################
# TriggerJobOpts/runHLT_standalone.py
#
#   Job options to run HLT standalone in AthenaMT
#
#   The file can be used as an include in other JO:
#      include('TriggerJobOpts/runHLT_standalone.py')
#   or directly to run the HLT:
#      athena --threads=1 --filesInput=input.pool.root TriggerJobOpts/runHLT_standalone.py
#
# Several flags are supported on the command line to steer these job options, for example
# by adding -c "setMenu='MC_pp_run3_v1'". See below for a complete list of all flags and their default
# value. If used in athena.py the auto-configuration is used to setup most flags correctly.
#
# Additional "modifiers" can be specified by using
#   -c "myModifier=True/False"
# Existing modifiers can be found in "TriggerJobOpts/python/Modifiers.py"
#
class opt:
    setMenu          = None           # option to overwrite flags.Trigger.triggerMenuSetup
    setDetDescr      = None           # OBSOLETE: force geometry tag, use flags instead
    setGlobalTag     = None           # OBSOLETE: force global conditions tag, use flags instead
    condOverride     = {}             # overwrite conditions folder tags e.g. '{"Folder1":"Tag1", "Folder2":"Tag2"}'
    doHLT            = True           # run HLT?
    doID             = True           # ConfigFlags.Trigger.doID
    doCalo           = True           # ConfigFlags.Trigger.doCalo
    doMuon           = True           # ConfigFlags.Trigger.doMuon
    doWriteRDOTrigger = False         # Write out RDOTrigger?
    doWriteBS        = True           # Write out BS?
    doL1Unpacking    = True           # decode L1 data in input file if True, else setup emulation
    doL1Sim          = False          # (re)run L1 simulation
    doEmptyMenu      = False          # Disable all chains, except those re-enabled by specific slices
    createHLTMenuExternally = False   # Set to True if the menu is build manually outside runHLT_standalone.py
    endJobAfterGenerate = False       # Finish job after menu generation
    forceEnableAllChains = False      # if True, all HLT chains will run even if the L1 item is false
    enableL1MuonPhase1   = None       # option to overwrite flags.Trigger.enableL1MuonPhase1
    enableL1CaloLegacy = True         # Enable Run-2 L1Calo simulation and/or decoding (possible even if enablePhase1 is True)
    enableL1TopoDump = False          # Enable L1Topo simulation to write inputs to txt
    enableL1TopoBWSimulation = False  # Enable bitwise L1Topo simulation
    enableL1NSWEmulation = False      # Enable TGC-NSW coincidence emulator : ConfigFlags.Trigger.L1MuonSim.EmulateNSW
    enableL1NSWVetoMode = True        # Enable TGC-NSW coincidence veto mode: ConfigFlags.Trigger.L1MuonSim.NSWVetoMode
    enableL1NSWMMTrigger = True       # Enable MM trigger for TGC-NSW coincidence : ConfigFlags.Trigger.L1MuonSim.doMMTrigger
    enableL1NSWPadTrigger = True      # Enable sTGC Pad trigger for TGC-NSW coincidence : ConfigFlags.Trigger.L1MuonSim.doPadTrigger
    enableL1NSWStripTrigger = False   # Enable sTGC Strip trigger for TGC-NSW coincidence : ConfigFlags.Trigger.L1MuonSim.doStripTrigger
    enableL1RPCBIS78    = True        # Enable TGC-RPC BIS78 coincidence : ConfigFlags.Trigger.L1MuonSim.doBIS78
#Individual slice flags
    doCalibSlice        = True
    doTestSlice         = True
    doHeavyIonSlice     = True
    doEnhancedBiasSlice = True
    doEgammaSlice     = True
    doMuonSlice       = True
    doMinBiasSlice    = True
    doJetSlice        = True
    doMETSlice        = True
    doBjetSlice       = True
    doTauSlice        = True
    doCombinedSlice   = True
    doBphysicsSlice   = True
    doStreamingSlice  = True
    doMonitorSlice    = True
    doBeamspotSlice   = True
    doCosmicSlice     = True
    doUnconventionalTrackingSlice   = True
    reverseViews      = False
    filterViews       = False
    enabledSignatures = []
    disabledSignatures = []
    selectChains      = []
    disableChains     = []

################################################################################
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
from AthenaConfiguration.AccumulatorCache import AccumulatorDecorator
from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr
from AthenaCommon.Include import include
from AthenaCommon.Logging import logging
from AthenaCommon import Constants
log = logging.getLogger('runHLT_standalone.py')

#-------------------------------------------------------------
# Setup options
#-------------------------------------------------------------
log.info('Setup options:')
defaultOptions = [a for a in dir(opt) if not a.startswith('__')]
for option in defaultOptions:
    if option in globals():
        setattr(opt, option, globals()[option])
        print(' %20s = %s' % (option, getattr(opt, option)))
    else:
        print(' %20s = (Default) %s' % (option, getattr(opt, option)))

if opt.setGlobalTag is not None:
    log.error("setGlobalTag is not supported anymore: set flags.IOVDb.GlobalTag instead")

if opt.setDetDescr is not None:
    log.error("setDetDescr is not supported anymore: set flags.GeoModel.AtlasVersion instead")

import re

sliceRe = re.compile("^do.*Slice")
slices = [a for a in dir(opt) if sliceRe.match(a)]
if opt.doEmptyMenu is True:
    log.info("Disabling all slices")
    for s in slices:
        if s in globals():
            log.info("re-enabling %s ", s)
            setattr(opt, s, globals()[s])
        else:
            setattr(opt, s, False)
else:
    for s in slices:
        if s in globals():
            setattr(opt, s, globals()[s])

# This is temporary and will be re-worked for after M3.5
for s in slices:
    signature = s[2:].replace('Slice', '')

    if eval('opt.'+s) is True:
        opt.enabledSignatures.append( signature )
    else:
        opt.disabledSignatures.append( signature )

#-------------------------------------------------------------
# Setting Global Flags
#-------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties
import TriggerJobOpts.Modifiers

flags.Common.isOnline = True

# Auto-configuration for athena
if len(athenaCommonFlags.FilesInput())>0:
    flags.Input.Files = athenaCommonFlags.FilesInput()
    import PyUtils.AthFile as athFile
    af = athFile.fopen(athenaCommonFlags.FilesInput()[0])
    globalflags.InputFormat = 'bytestream' if af.fileinfos['file_type']=='bs' else 'pool'
    globalflags.DataSource = 'data' if af.fileinfos['evt_type'][0]=='IS_DATA' else 'geant4'
    flags.Input.isMC = False if globalflags.DataSource=='data' else True
    if globalflags.DataSource() != 'data':
        log.info("Setting isOnline = False for MC input")
        flags.Common.isOnline = False
    TriggerJobOpts.Modifiers._run_number = flags.Input.RunNumber[0]
    TriggerJobOpts.Modifiers._lb_number = flags.Input.LumiBlockNumber[0]

else:   # athenaHLT
    globalflags.InputFormat = 'bytestream'
    globalflags.DataSource = 'data'
    flags.Input.isMC = False
    flags.Input.Files = []
    TriggerJobOpts.Modifiers._run_number = globals().get('_run_number')  # set by athenaHLT
    TriggerJobOpts.Modifiers._lb_number = globals().get('_lb_number')  # set by athenaHLT
    if '_run_number' in globals():
        del _run_number  # noqa, set by athenaHLT
    if '_lb_number' in globals():
        del _lb_number  # noqa, set by athenaHLT

from AthenaConfiguration.Enums import BeamType, Format
flags.Input.Format = Format.BS if globalflags.InputFormat == 'bytestream' else Format.POOL

# Load input collection list from POOL metadata
from RecExConfig.ObjKeyStore import objKeyStore
if flags.Input.Format is Format.POOL:
    from PyUtils.MetaReaderPeeker import convert_itemList
    objKeyStore.addManyTypesInputFile(convert_itemList(layout='#join'))

# Run-3 Trigger produces Run-3 EDM
flags.Trigger.EDMVersion = 3

# Other defaults
jobproperties.Beam.beamType = 'collisions'
flags.Beam.Type = BeamType(jobproperties.Beam.beamType)
if not flags.Input.isMC:
    globalflags.DatabaseInstance='CONDBR2'
    flags.IOVDb.DatabaseInstance=globalflags.DatabaseInstance()
athenaCommonFlags.isOnline.set_Value_and_Lock(flags.Common.isOnline)

log.info('Configured the following global flags:')
globalflags.print_JobProperties()

# Set default doL1Sim option depending on input type (if not set explicitly)
if 'doL1Sim' not in globals():
    opt.doL1Sim = flags.Input.isMC
    log.info('Setting default doL1Sim=%s because flags.Input.isMC=%s', opt.doL1Sim, flags.Input.isMC)

if flags.Input.Format is Format.BS or opt.doL1Sim:
    flags.Trigger.HLTSeeding.forceEnableAllChains = opt.forceEnableAllChains

# Translate a few other flags
flags.Trigger.doLVL1 = opt.doL1Sim
if opt.enableL1MuonPhase1 is not None:
    flags.Trigger.enableL1MuonPhase1 = opt.enableL1MuonPhase1
flags.Trigger.enableL1CaloLegacy = opt.enableL1CaloLegacy
flags.Trigger.enableL1TopoDump = opt.enableL1TopoDump
flags.Trigger.enableL1TopoBWSimulation = opt.enableL1TopoBWSimulation

flags.Trigger.L1MuonSim.EmulateNSW  = opt.enableL1NSWEmulation
flags.Trigger.L1MuonSim.NSWVetoMode = opt.enableL1NSWVetoMode
flags.Trigger.L1MuonSim.doMMTrigger = opt.enableL1NSWMMTrigger
flags.Trigger.L1MuonSim.doPadTrigger = opt.enableL1NSWPadTrigger
flags.Trigger.L1MuonSim.doStripTrigger = opt.enableL1NSWStripTrigger
flags.Trigger.L1MuonSim.doBIS78 = opt.enableL1RPCBIS78

flags.Trigger.doHLT = bool(opt.doHLT)
flags.Trigger.doID = opt.doID
flags.Trigger.doMuon = opt.doMuon
flags.Trigger.doCalo = opt.doCalo
flags.InDet.useDCS = False   # DCS is in general not available online

# Set legacy Cond/Geo tags:
globalflags.DetDescrVersion = flags.GeoModel.AtlasVersion
globalflags.ConditionsTag = flags.IOVDb.GlobalTag

if opt.setMenu:
    flags.Trigger.triggerMenuSetup = opt.setMenu

# Setup list of modifiers
# Common modifiers for MC and data
setModifiers = ['ForceMuonDataType',
                'useNewRPCCabling',
                'useOracle',
                'BunchSpacing25ns'
]

if flags.Input.isMC:  # MC modifiers
    setModifiers += ['BFieldFromDCS']
else:           # More data modifiers
    setModifiers += ['BFieldAutoConfig',
                     'useDynamicAlignFolders',
                     #Check for beamspot quality flag
                     'useOnlineLumi',
                     #for running with real data
                     'DisableMdtT0Fit',
                     #Set muComb/muIso Backextrapolator tuned for real data
                     #Monitoring for L1 muon group
                     #Monitoring L1Topo at ROB level
                     'enableSchedulerMon',
    ]
    if opt.doL1Sim:
        flags.LAr.LATOME.DTInfoForL1="SC_ET_ID"


#-------------------------------------------------------------
# Modifiers
#-------------------------------------------------------------
modifierList=[]
from TrigConfigSvc.TrigConfMetaData import TrigConfMetaData
meta = TrigConfMetaData()

for mod in dir(TriggerJobOpts.Modifiers):
    if not hasattr(getattr(TriggerJobOpts.Modifiers,mod),'preSetup'):
        continue
    if mod in dir():  #allow turning on and off modifiers by variable of same name
        if globals()[mod]:
            if mod not in setModifiers:
                setModifiers+=[mod]
        elif mod in setModifiers:
                setModifiers.remove(mod)
    if mod in setModifiers:
        modifierList+=[getattr(TriggerJobOpts.Modifiers,mod)()]
        meta.Modifiers += [mod]    # store in trig conf meta data
        setModifiers.remove(mod)

if setModifiers:
    log.error('Unknown modifier(s): %s', setModifiers)

#-------------------------------------------------------------
# Output flags
#-------------------------------------------------------------
from RecExConfig.RecFlags import rec
if opt.doWriteRDOTrigger:
    if flags.Trigger.Online.isPartition:
        log.error('Cannot use doWriteRDOTrigger in athenaHLT or partition')
        theApp.exit(1)
    rec.doWriteRDO = False  # RecExCommon flag
    flags.Output.doWriteRDO = True  # new JO flag
    if not flags.Output.RDOFileName:
        flags.Output.RDOFileName = 'RDO_TRIG.pool.root'  # new JO flag
if opt.doWriteBS:
    rec.doWriteBS = True  # RecExCommon flag
    flags.Output.doWriteBS = True  # new JO flag
    flags.Trigger.writeBS = True  # new JO flag

# never include this
include.block("RecExCond/RecExCommon_flags.py")

# ---------------------------------------------------------------
# Create main sequences
# ---------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.CFElements import seqOR,parOR
hltTop = seqOR("HLTTop")

hltBeginSeq = parOR("HLTBeginSeq")
hltTop += hltBeginSeq
topSequence += hltTop

#-------------------------------------------------------------
# Setting DetFlags
#-------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags
if flags.Input.Format is not Format.BS:
    DetFlags.detdescr.all_setOn()

if flags.Trigger.doID:
    DetFlags.detdescr.ID_setOn()
    DetFlags.makeRIO.ID_setOn()
else:
    DetFlags.ID_setOff()

if flags.Trigger.doMuon:
    DetFlags.detdescr.Muon_setOn()
    DetFlags.makeRIO.all_setOn()
    # Setup muon reconstruction for trigger
    flags.Muon.MuonTrigger=True
else:
    DetFlags.Muon_setOff()

if flags.Trigger.doCalo:
    DetFlags.detdescr.Calo_setOn()
    from LArConditionsCommon.LArCondFlags import larCondFlags
    larCondFlags.LoadElecCalib.set_Value_and_Lock(False)
else:
    DetFlags.Calo_setOff()

DetFlags.Print()

# RecEx flags
from RecExConfig.RecFlags import rec
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
rec.doESD = False
rec.doAOD = False
rec.doTruth = False

#-------------------------------------------------------------
# Apply modifiers
#-------------------------------------------------------------
for mod in modifierList:
    mod.preSetup(flags)

#--------------------------------------------------------------
# Increase scheduler checks and verbosity
#--------------------------------------------------------------
flags.Scheduler.CheckDependencies = True
flags.Scheduler.ShowControlFlow = True
flags.Scheduler.ShowDataDeps = True
flags.Scheduler.EnableVerboseViews = True
flags.Input.FailOnUnknownCollections = True
flags.Scheduler.AutoLoadUnmetDependencies = False

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.CheckDependencies( True )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.EnableVerboseViews( True )

if flags.Input.FailOnUnknownCollections:
    AlgScheduler.setDataLoaderAlg("")
    if not hasattr(topSequence,"SGInputLoader"):
        log.error('Cannot set FailIfNoProxy property because SGInputLoader not found in topSequence')
        theApp.exit(1)
    topSequence.SGInputLoader.FailIfNoProxy = True

# ----------------------------------------------------------------
# Detector geometry
# ----------------------------------------------------------------
# Always enable magnetic field
from AthenaCommon.DetFlags import DetFlags
DetFlags.BField_setOn()
# But don't make the job think it is doing any sim+digi
DetFlags.simulate.all_setOff()
DetFlags.pileup.all_setOff()
DetFlags.overlay.all_setOff()
# Disable some forward detetors
flags.Detector.GeometryALFA = False
flags.Detector.GeometryFwdRegion = False
flags.Detector.GeometryLucid = False

include ("RecExCond/AllDet_detDescr.py")

if flags.Trigger.doID:
    include("InDetTrigRecExample/InDetTrigRec_jobOptions.py")
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    InDetTrigFlags.doPrintConfigurables = log.getEffectiveLevel() <= logging.DEBUG
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doPrintConfigurables = log.getEffectiveLevel() <= logging.DEBUG
    include("InDetRecExample/InDetRecConditionsAccess.py")

#-------------------------------------------------------------
# Switch off CPS mechanism if we only run selected
# signatures or chains, to avoid single-chain sets
#-------------------------------------------------------------
if len(opt.enabledSignatures)==1 or opt.selectChains:
    flags.Trigger.disableCPS=True

#-------------------------------------------------------------
# Lock flags !
#
# This is the earliest we can lock since InDetJobProperties.py
# above still modifies the flags.

from TriggerJobOpts import runHLT
runHLT.lock_and_restrict(flags)

# Only import this here to avoid we accidentally use CAs before locking
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
#-------------------------------------------------------------

from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg
CAtoGlobalWrapper(IOVDbSvcCfg, flags)


#-------------------------------------------------------------
# Cost Monitoring
#-------------------------------------------------------------
# HLTPreSeq only used for CostMon so far, configure it only here
if flags.Trigger.CostMonitoring.doCostMonitoring:
    hltPreSeq = parOR("HLTPreSeq")
    hltTop.insert(0, hltPreSeq)
# The CA below handles doCostMonitoring=False nicely, so we call it unconditionally
from TrigCostMonitor.TrigCostMonitorConfig import TrigCostMonitorCfg
CAtoGlobalWrapper(TrigCostMonitorCfg, flags, seqName="HLTPreSeq")


if flags.Trigger.doCalo:
    from TrigT2CaloCommon.TrigCaloDataAccessConfig import trigCaloDataAccessSvcCfg
    CAtoGlobalWrapper(trigCaloDataAccessSvcCfg, flags)
    if flags.Trigger.doTransientByteStream:
        from TriggerJobOpts.TriggerTransBSConfig import triggerTransBSCfg_Calo
        CAtoGlobalWrapper(triggerTransBSCfg_Calo, flags, seqName="HLTBeginSeq")


if flags.Trigger.doMuon:
    from MuonConfig.MuonCablingConfig import MuonCablingConfigCfg
    CAtoGlobalWrapper(MuonCablingConfigCfg, flags)
    import MuonRecExample.MuonReadCalib      # noqa: F401

    include ("MuonRecExample/MuonRecLoadTools.py")

# restore logger after above includes
log = logging.getLogger('runHLT_standalone.py')

# ---------------------------------------------------------------
# Track Overlay
# ---------------------------------------------------------------
from OverlayCommonAlgs.OverlayFlags import overlayFlags
if overlayFlags.doTrackOverlay():
    from TrkEventCnvTools.TrkEventCnvToolsConfigCA import TrkEventCnvSuperToolCfg
    CAtoGlobalWrapper(TrkEventCnvSuperToolCfg, flags)

# ----------------------------------------------------------------
# Pool input
# ----------------------------------------------------------------
print("flags.Input.Format", flags.Input.Format)
print("flags.Trigger.Online.isPartition", flags.Trigger.Online.isPartition)
if flags.Input.Format is Format.POOL:
    import AthenaPoolCnvSvc.ReadAthenaPool   # noqa
    svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]
    svcMgr.PoolSvc.AttemptCatalogPatch=True

# ----------------------------------------------------------------
# ByteStream input
# ----------------------------------------------------------------
elif flags.Input.Format is Format.BS and not flags.Trigger.Online.isPartition:
    # Set up ByteStream reading services
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    CAtoGlobalWrapper(ByteStreamReadCfg, flags)

# ---------------------------------------------------------------
# Trigger config
# ---------------------------------------------------------------
from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu, createL1PrescalesFileFromMenu
generateL1Menu(flags)
createL1PrescalesFileFromMenu(flags)

from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
CAtoGlobalWrapper(L1ConfigSvcCfg,flags)

# ---------------------------------------------------------------
# Event Info setup
# ---------------------------------------------------------------
# If no xAOD::EventInfo is found in a POOL file, schedule conversion from old EventInfo
if flags.Input.Format is Format.POOL:
    if objKeyStore.isInInput("xAOD::EventInfo"):
        topSequence.SGInputLoader.Load += [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
    else:
        if not hasattr(hltBeginSeq, "xAODMaker::EventInfoCnvAlg"):
            from xAODEventInfoCnv.xAODEventInfoCnvAlgDefault import xAODEventInfoCnvAlgDefault
            xAODEventInfoCnvAlgDefault(sequence=hltBeginSeq)
else:
    topSequence.SGInputLoader.Load += [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]

# ---------------------------------------------------------------
# Add LumiBlockMuWriter creating xAOD::EventInfo decorations for pileup values
# ---------------------------------------------------------------
from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
CAtoGlobalWrapper(LumiBlockMuWriterCfg, flags, seqName="HLTBeginSeq")


# ---------------------------------------------------------------
# Level 1 simulation
# ---------------------------------------------------------------
if opt.doL1Sim:
    from TriggerJobOpts.Lvl1SimulationConfig import Lvl1SimulationCfg
    CAtoGlobalWrapper(Lvl1SimulationCfg, flags, seqName="HLTBeginSeq")


# ---------------------------------------------------------------
# Add HLTSeeding providing inputs to HLT
# ---------------------------------------------------------------
if opt.doL1Unpacking:
    if flags.Input.Format is Format.BS or opt.doL1Sim:
        from HLTSeeding.HLTSeedingConfig import HLTSeedingCfg
        CAtoGlobalWrapper(HLTSeedingCfg, flags, seqName="HLTBeginSeq")
    else:
        from DecisionHandling.TestUtils import L1EmulationTest
        hltBeginSeq += L1EmulationTest()

# ---------------------------------------------------------------
# HLT generation
# ---------------------------------------------------------------
if not opt.createHLTMenuExternally:

    from TriggerMenuMT.HLT.Config.GenerateMenuMT import GenerateMenuMT
    menu = GenerateMenuMT()

    # Define as functor, to retain knowledge of the select/disableChains lists
    class ChainsToGenerate(object):
        def __init__(self,opt):
            self.enabledSignatures  = opt.enabledSignatures
            self.disabledSignatures = opt.disabledSignatures
            self.selectChains       = opt.selectChains
            self.disableChains      = opt.disableChains
        def __call__(self, signame, chain):
            return ((signame in self.enabledSignatures and signame not in self.disabledSignatures) and
                (not self.selectChains or chain in self.selectChains) and chain not in self.disableChains)

    chainsToGenerate = ChainsToGenerate(opt)
    menu.setChainFilter(chainsToGenerate)

    # generating the HLT structure requires
    # the HLTSeeding to be defined in the topSequence
    menu.generateMT(flags)
    # Note this will also create the requested HLTPrescale JSON
    # - the default file (with all prescales set to 1) is not really needed.
    # - If no file is provided all chains are either enabled or disabled,
    #   depending on the property HLTSeeding.PrescalingTool.KeepUnknownChains being True or False


    if opt.endJobAfterGenerate:
        from AthenaCommon.AlgSequence import dumpSequence
        dumpSequence( topSequence )
        theApp.exit()



from TrigConfigSvc.TrigConfigSvcCfg import HLTConfigSvcCfg
CAtoGlobalWrapper(HLTConfigSvcCfg,flags)

# ---------------------------------------------------------------
# Tell the SGInputLoader about L1 and HLT menu in the DetectorStore
# ---------------------------------------------------------------
if hasattr(topSequence,"SGInputLoader"):
    topSequence.SGInputLoader.Load += [
        ('TrigConf::L1Menu','DetectorStore+L1TriggerMenu'),
        ('TrigConf::HLTMenu','DetectorStore+HLTTriggerMenu')]

# ---------------------------------------------------------------
# Monitoring
# ---------------------------------------------------------------
from TriggerJobOpts.TriggerHistSvcConfig import TriggerHistSvcConfig
CAtoGlobalWrapper(TriggerHistSvcConfig, flags)

if flags.Common.isOnline:
    from TrigOnlineMonitor.TrigOnlineMonitorConfig import trigOpMonitorCfg
    CAtoGlobalWrapper(trigOpMonitorCfg, flags)

#-------------------------------------------------------------
# Conditions overrides
#-------------------------------------------------------------
if len(opt.condOverride)>0:
    for folder,tag in iter(opt.condOverride.items()):
        log.warning('Overriding folder %s with tag %s', folder, tag)
        from IOVDbSvc.IOVDbSvcConfig import addOverride
        CAtoGlobalWrapper(addOverride, flags,folder=folder,tag=tag)

if svcMgr.MessageSvc.OutputLevel < Constants.INFO:
    from AthenaCommon.JobProperties import jobproperties
    jobproperties.print_JobProperties('tree&value')
    print(svcMgr)

#-------------------------------------------------------------
# ID Cache Creators
#-------------------------------------------------------------
from TriggerJobOpts.TriggerConfig import triggerIDCCacheCreatorsCfg
CAtoGlobalWrapper(triggerIDCCacheCreatorsCfg, flags, seqName="HLTBeginSeq")

#-------------------------------------------------------------
# Output configuration
#-------------------------------------------------------------
if opt.doWriteBS or opt.doWriteRDOTrigger:
    from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, collectHypoDecisionObjects, triggerOutputCfg
    from AthenaCommon.CFElements import findAlgorithm,findSubSequence
    hypos = collectHypos(findSubSequence(topSequence, "HLTAllSteps"))
    filters = collectFilters(findSubSequence(topSequence, "HLTAllSteps"))

    nfilters = sum(len(v) for v in filters.values())
    nhypos = sum(len(v) for v in hypos.values())
    log.info( "Algorithms counting: Number of Filter algorithms: %d  -  Number of Hypo algoirthms: %d", nfilters , nhypos)

    summaryMakerAlg = findAlgorithm(topSequence, "DecisionSummaryMakerAlg")
    hltSeeding = findAlgorithm(topSequence, "HLTSeeding")

    if hltSeeding and summaryMakerAlg:
        decObj = collectDecisionObjects( hypos, filters, hltSeeding, summaryMakerAlg )
        decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)
        log.debug("Decision Objects to write to output [hack method - should be replaced with triggerRunCfg()]")
        log.debug(decObj)
    else:
        log.error("Failed to find HLTSeeding or DecisionSummaryMakerAlg, cannot determine Decision names for output configuration")
        decObj = []
        decObjHypoOut = []

    # Add HLT Navigation to EDM list
    from TrigEDMConfig import TriggerEDMRun3
    TriggerEDMRun3.addHLTNavigationToEDMList(flags, TriggerEDMRun3.TriggerHLTListRun3, decObj, decObjHypoOut)

    # Configure output writing
    CAtoGlobalWrapper(triggerOutputCfg, flags, hypos=hypos)

#-------------------------------------------------------------
# Debugging for view cross-dependencies
#-------------------------------------------------------------
if opt.reverseViews or opt.filterViews:
    from TriggerJobOpts.TriggerConfig import collectViewMakers
    viewMakers = collectViewMakers( topSequence )
    theFilter = []
    if opt.filterViews:
        # no idea why the FS vertex would be needed here, but I'll add the FSJet vertex also for good measure
        theFilter = [ "Cache", "EventInfo", "HLT_IDVertex_FS", "HLT_IDVertex_FSJet" ]
    for alg in viewMakers:
        alg.ReverseViewsDebug = opt.reverseViews
        alg.FallThroughFilter = theFilter

#-------------------------------------------------------------
# Disable overly verbose and problematic ChronoStatSvc print-out
#-------------------------------------------------------------
include("TriggerTest/disableChronoStatSvcPrintout.py")

#-------------------------------------------------------------
# MessageSvc
#-------------------------------------------------------------
svcMgr.MessageSvc.Format = "% F%40W%C%4W%R%e%s%8W%R%T %0W%M"
svcMgr.MessageSvc.enableSuppression = False

if flags.Input.isMC:
    # Disable spurious warnings from HepMcParticleLink, ATR-21838
    svcMgr.MessageSvc.setError += ['HepMcParticleLink']

#-------------------------------------------------------------
# Apply modifiers
#-------------------------------------------------------------
for mod in modifierList:
    mod.postSetup(flags)

#-------------------------------------------------------------
# Print top sequence
#-------------------------------------------------------------
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)

#-------------------------------------------------------------
# Print caching statistics
#-------------------------------------------------------------
AccumulatorDecorator.printStats()
