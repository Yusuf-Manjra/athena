################################################################################
# TrigUpgradeTest/testHLT_MT.py
#
#   Job options for trigger MT tests to be used as first include
#   in signature specific job options. Example:
#
#      include('TrigUpgradeTest/testHLT_MT.py')
#      ... your job options ...
#
# Several flags are supported on the command line to steer these job options.
# See below for a complete list of all flags and their default value. If used
# in athena.py the auto-configuration is used to setup most flags correctly.
#
# Additional "modifiers" can be specified by using
#   -c "myModifier=True/False"
# Existing modifiers can be found in "TriggerRelease/python/Modifiers.py"
#
class opt:
    setupForMC       = None           # force MC setup
    setLVL1XML       = 'TriggerMenuMT/LVL1config_LS2_v1.xml' # 'TriggerMenu/LVL1config_Physics_pp_v7.xml' # default for legacy
    setDetDescr      = None           # force geometry tag
    setGlobalTag     = None           # force global conditions tag
    useCONDBR2       = True           # if False, use run-1 conditions DB
    condOverride     = {}             # overwrite conditions folder tags e.g. '{"Folder1":"Tag1", "Folder2":"Tag2"}'
    doHLT            = True           # run HLT?
    doID             = True           # TriggerFlags.doID
    doCalo           = True           # TriggerFlags.doCalo
    doMuon           = True           # TriggerFlags.doMuon
    doDBConfig       = None           # dump trigger configuration
    trigBase         = None           # file name for trigger config dump
    enableCostD3PD   = False          # enable cost monitoring
    doWriteESD       = True           # Write out an ESD?
    doL1Unpacking    = True           # decode L1 data in input file if True, else setup emulation
    doL1Sim          = False          # (re)run L1 simulation
    isOnline         = False          # isOnline flag (TEMPORARY HACK, should be True by default)
    doEmptyMenu      = False          # Disable all chains, except those re-enabled by specific slices
#Individual slice flags
    doElectronSlice   = True
    doPhotonSlice     = True
    doMuonSlice       = True
    doJetSlice        = True
    doMETSlice        = True
    doBjetSlice       = True
    doTauSlice        = True
    doCombinedSlice   = True
    doBphysicsSlice   = True
    doStreamingSlice  = True
    reverseViews      = False
    enabledSignatures = []
    disabledSignatures = []
#
################################################################################
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr
from AthenaCommon.Logging import logging
log = logging.getLogger('testHLT_MT.py')

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
        setattr(opt, s, True)
    opt.doTauSlice =False #Wait for ATR-17399

# Setting the TriggerFlags.XXXSlice to use in TriggerMenuMT
# This is temporary and will be re-worked for after M3.5
for s in slices:
    signature = s[2:].replace('Slice', '')
    if 'Electron' in s or 'Photon' in s:
        signature = 'Egamma'

    if eval('opt.'+s) is True:
        enabledSig = 'TriggerFlags.'+signature+'Slice.setAll()'
        opt.enabledSignatures.append( enabledSig )
    else:
        disabledSig = 'TriggerFlags.'+signature+'Slice.setAll()'
        opt.disabledSignatures.append( disabledSig )

#-------------------------------------------------------------
# Setting Global Flags
#-------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties
import TriggerRelease.Modifiers

# Auto-configuration for athena
if len(athenaCommonFlags.FilesInput())>0:
    import PyUtils.AthFile as athFile
    af = athFile.fopen(athenaCommonFlags.FilesInput()[0])
    globalflags.InputFormat = 'bytestream' if af.fileinfos['file_type']=='bs' else 'pool'
    globalflags.DataSource = 'data' if af.fileinfos['evt_type'][0]=='IS_DATA' else 'geant4'
    if opt.setDetDescr is None:
        opt.setDetDescr = af.fileinfos.get('geometry',None)
    if opt.setGlobalTag is None:
        opt.setGlobalTag = af.fileinfos.get('conditions_tag',None) or \
            (TriggerFlags.OnlineCondTag() if opt.isOnline else 'CONDBR2-BLKPA-2018-13')
    TriggerRelease.Modifiers._run_number = af.fileinfos['run_number'][0]

else:   # athenaHLT
    globalflags.InputFormat = 'bytestream'
    globalflags.DataSource = 'data' if not opt.setupForMC else 'data'
    if '_run_number' not in dir():
        import PyUtils.AthFile as athFile
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        af = athFile.fopen(athenaCommonFlags.BSRDOInput()[0])
        _run_number = af.run_number[0]

    TriggerRelease.Modifiers._run_number = _run_number   # noqa, set by athenaHLT

    from RecExConfig.RecFlags import rec
    rec.RunNumber =_run_number
    del _run_number


# Set final Cond/Geo tag based on input file, command line or default
globalflags.DetDescrVersion = opt.setDetDescr or TriggerFlags.OnlineGeoTag()
globalflags.ConditionsTag = opt.setGlobalTag or TriggerFlags.OnlineCondTag()

# Other defaults
jobproperties.Beam.beamType = 'collisions'
jobproperties.Beam.bunchSpacing = 25
globalflags.DatabaseInstance='CONDBR2' if opt.useCONDBR2 else 'COMP200'
athenaCommonFlags.isOnline.set_Value_and_Lock(opt.isOnline)

log.info('Configured the following global flags:')
globalflags.print_JobProperties()

#-------------------------------------------------------------
# Transfer flags into TriggerFlags
#-------------------------------------------------------------

# To turn off HLT for athena running
TriggerFlags.doHLT = bool(opt.doHLT)
    
# To extract the Trigger configuration
TriggerFlags.Online.doDBConfig = bool(opt.doDBConfig)
if opt.trigBase is not None:
    TriggerFlags.Online.doDBConfigBaseName = opt.trigBase

# Setup list of modifiers
# Common modifiers for MC and data
setModifiers = ['noLArCalibFolders',
                'ForceMuonDataType',
                'useNewRPCCabling',
                'enableCostMonitoring',
                #'enableCoherentPS',
                'useOracle',
                'enableHotIDMasking',
                'openThresholdRPCCabling',
]

if globalflags.DataSource.is_geant4():  # MC modifiers
    setModifiers += ['BFieldFromDCS']
else:           # More data modifiers
    setModifiers += ['allowCOOLUpdates',
                     'BFieldAutoConfig',
                     'useHLTMuonAlign',
                     #Check for beamspot quality flag
                     'UseBeamSpotFlagForBjet',
                     'UseParamFromDataForBjet',
                     #Use online luminosity
                     'useOnlineLumi',
                     #for running with real data
                     'DisableMdtT0Fit',
                     #Setup mufast tuning for data
                     'UseLUTFromDataForMufast',
                     'UseRPCTimeDelayFromDataForMufast',
                     #Set muComb/muIso Backextrapolator tuned for real data
                     'UseBackExtrapolatorDataForMuIso',
                     #Monitoring for L1 muon group
                     #'muCTPicheck',
                     #Monitoring L1Topo at ROB level
                     #'L1TopoCheck',
                     'forceTileRODMap',
    ]

#make some more common trig cost operations easier to setup
if opt.enableCostD3PD:
    enableCostMonitoring = True # This goes without saying!
    enableCostForCAF = True # This sets the cost mon to monitor every event and always export the data.


TriggerFlags.doID = opt.doID
TriggerFlags.doMuon = opt.doMuon
TriggerFlags.doCalo = opt.doCalo

#-------------------------------------------------------------
# Modifiers
#-------------------------------------------------------------
modifierList=[]
from TrigConfigSvc.TrigConfMetaData import TrigConfMetaData
meta = TrigConfMetaData()
    
for mod in dir(TriggerRelease.Modifiers):
    if not hasattr(getattr(TriggerRelease.Modifiers,mod),'preSetup'): continue
    if mod in dir():  #allow turning on and off modifiers by variable of same name
        if globals()[mod]:
            if mod not in setModifiers:
                setModifiers+=[mod]
        else:
            if mod in setModifiers: setModifiers.remove(mod)
    if mod in setModifiers:
        modifierList+=[getattr(TriggerRelease.Modifiers,mod)()]
        meta.Modifiers += [mod]    # store in trig conf meta data
        setModifiers.remove(mod)

if setModifiers:
    log.error('Unknown modifier(s): '+str(setModifiers))


# never include this
include.block("RecExCond/RecExCommon_flags.py")

#-------------------------------------------------------------
# Setting DetFlags
#-------------------------------------------------------------
if globalflags.InputFormat.is_bytestream():
    TriggerFlags.doLVL1 = False

from AthenaCommon.DetFlags import DetFlags
if TriggerFlags.doLVL1():
    DetFlags.detdescr.all_setOn()
if TriggerFlags.doID():
    DetFlags.detdescr.ID_setOn()
    DetFlags.makeRIO.ID_setOn()
else:
    DetFlags.ID_setOff()
if TriggerFlags.doMuon():
    DetFlags.detdescr.Muon_setOn()
    DetFlags.makeRIO.all_setOn()
else:
    DetFlags.Muon_setOff()
if TriggerFlags.doCalo():
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

TriggerFlags.writeBS = True

#-------------------------------------------------------------
# Apply modifiers
#-------------------------------------------------------------
for mod in modifierList:
    mod.preSetup()

#--------------------------------------------------------------
# Conditions setup.
#--------------------------------------------------------------
from IOVDbSvc.CondDB import conddb #This import will also set up CondInputLoader
conddb.setGlobalTag(globalflags.ConditionsTag())

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
if jobproperties.ConcurrencyFlags.NumThreads() > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )
    AlgScheduler.EnableVerboseViews( True )

#--------------------------------------------------------------
# Event Info setup
#--------------------------------------------------------------
# If no xAOD::EventInfo is found in a POOL file, schedule conversion from old EventInfo
if globalflags.InputFormat.is_pool():
    from RecExConfig.ObjKeyStore import objKeyStore
    from PyUtils.MetaReaderPeeker import convert_itemList
    objKeyStore.addManyTypesInputFile(convert_itemList(layout='#join'))
    if ( not objKeyStore.isInInput("xAOD::EventInfo") ) and ( not hasattr(topSequence, "xAODMaker::EventInfoCnvAlg") ):
        from xAODEventInfoCnv.xAODEventInfoCnvAlgDefault import xAODEventInfoCnvAlgDefault
        xAODEventInfoCnvAlgDefault(sequence=topSequence)

# ----------------------------------------------------------------
# Detector geometry 
# ----------------------------------------------------------------
# Always enable AtlasFieldSvc
from AthenaCommon.DetFlags import DetFlags
DetFlags.BField_setOn()
include ("RecExCond/AllDet_detDescr.py")

from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
svcMgr += RegSelSvcDefault()

if TriggerFlags.doID():
    include( "InDetRecExample/InDetRecCabling.py" )

if TriggerFlags.doCalo():
    from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigDataAccess
    svcMgr.ToolSvc += TrigDataAccess()

if TriggerFlags.doMuon():
    import MuonCnvExample.MuonCablingConfig  # noqa: F401
    import MuonRecExample.MuonReadCalib      # noqa: F401
    if globalflags.InputFormat.is_pool():
        include( "MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py" )

    include ("MuonRecExample/MuonRecLoadTools.py")

# ----------------------------------------------------------------
# Pool input
# ----------------------------------------------------------------
if globalflags.InputFormat.is_pool():
    import AthenaPoolCnvSvc.ReadAthenaPool   # noqa
    svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]
    svcMgr.PoolSvc.AttemptCatalogPatch=True
    # enable transient BS 
    if TriggerFlags.writeBS():
        log.info("setting up transient BS")
        include( "TriggerRelease/jobOfragment_TransBS_standalone.py" )
     
# ----------------------------------------------------------------
# ByteStream input
# ----------------------------------------------------------------
elif globalflags.InputFormat.is_bytestream():

    # This is only needed running athena (as opposed to athenaHLT)
    if not hasattr(svcMgr,"ByteStreamCnvSvc"):
        from ByteStreamCnvSvc import ReadByteStream   # noqa
        # Define the input
        svcMgr.ByteStreamInputSvc.FullFileName = athenaCommonFlags.FilesInput()
        theApp.ExtSvc += [ "ByteStreamCnvSvc"]

    # Online specific setup of BS converters
    include( "TriggerRelease/jobOfragment_ReadBS_standalone.py" )    


# ---------------------------------------------------------------
# Trigger config
# ---------------------------------------------------------------
TriggerFlags.inputLVL1configFile = opt.setLVL1XML
TriggerFlags.readLVL1configFromXML = True
TriggerFlags.outputLVL1configFile = None
from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc, findFileInXMLPATH
svcMgr += LVL1ConfigSvc()
svcMgr.LVL1ConfigSvc.XMLMenuFile = findFileInXMLPATH(TriggerFlags.inputLVL1configFile())

if opt.doL1Sim:
    from TriggerJobOpts.Lvl1SimulationConfig import Lvl1SimulationSequence
    topSequence += Lvl1SimulationSequence()

if opt.doL1Unpacking:
    if globalflags.InputFormat.is_bytestream():
        from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import RoIBResultByteStreamDecoderAlg
        from L1Decoder.L1DecoderConfig import L1Decoder
        topSequence += RoIBResultByteStreamDecoderAlg() # creates RoIBResult (input for L1Decoder) from ByteStream
        topSequence += L1Decoder("L1Decoder")
        #topSequence.L1Decoder.ChainToCTPMapping = MenuTest.CTPToChainMapping
    elif opt.doL1Sim:
        from L1Decoder.L1DecoderConfig import L1Decoder
        topSequence += L1Decoder("L1Decoder")
    else:
        from TrigUpgradeTest.TestUtils import L1EmulationTest
        topSequence += L1EmulationTest()


if TriggerFlags.doID:
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    InDetTrigFlags.doPixelClusterSplitting = False
  
    # PixelLorentzAngleSvc and SCTLorentzAngleSvc
    from AthenaCommon.Include import include
    include("InDetRecExample/InDetRecConditionsAccess.py")


# ---------------------------------------------------------------
# Monitoring
# ---------------------------------------------------------------
if not hasattr(svcMgr, 'THistSvc'):        
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc()
if hasattr(svcMgr.THistSvc, "Output"):
    from TriggerJobOpts.HLTTriggerGetter import setTHistSvcOutput
    setTHistSvcOutput(svcMgr.THistSvc.Output)

#-------------------------------------------------------------
# Apply modifiers
#-------------------------------------------------------------
for mod in modifierList:
    mod.postSetup()

#-------------------------------------------------------------
# Conditions overrides
#-------------------------------------------------------------    
if len(opt.condOverride)>0:
    for folder,tag in opt.condOverride.iteritems():
        log.warning('Overriding folder %s with tag %s', folder, tag)
        conddb.addOverride(folder,tag)

if svcMgr.MessageSvc.OutputLevel<INFO:
    from AthenaCommon.JobProperties import jobproperties
    jobproperties.print_JobProperties('tree&value')
    print(svcMgr)


from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=True
from TriggerJobOpts.TriggerConfig import triggerIDCCacheCreatorsCfg
from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.lock()
triggerIDCCacheCreatorsCfg(ConfigFlags).appendToGlobals()
Configurable.configurableRun3Behavior=False

#-------------------------------------------------------------
# Non-ComponentAccumulator Cost Monitoring
#-------------------------------------------------------------

from AthenaCommon.AppMgr import ServiceMgr
from TrigCostMonitorMT.TrigCostMonitorMTConf import TrigCostMTAuditor, TrigCostMTSvc

# This should be temporary, it is doing the same job as TrigCostMonitorMTConfig but without using a ComponentAccumulator
if ConfigFlags.Trigger.CostMonitoring.doCostMonitoring:
    trigCostService = TrigCostMTSvc()
    trigCostService.MonitorAllEvents = ConfigFlags.Trigger.CostMonitoring.monitorAllEvents
    trigCostService.SaveHashes = True # This option will go away once the TrigConfigSvc is fully up & running
    ServiceMgr += trigCostService
    #
    ServiceMgr.AuditorSvc += TrigCostMTAuditor()
    theApp.AuditAlgorithms=True
