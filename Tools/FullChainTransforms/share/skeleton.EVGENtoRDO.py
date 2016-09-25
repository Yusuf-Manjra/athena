# skeleton.EVGENtoRDO.py
# skeleton file for running simulation+digi in one job for FastChain
# currently using full simulation and digi, will swap in fast components later
# Richard Hawkings, Dec 2014, based on work by Robert Harrington
# started by merging simulation and digitisaton skeletons, then modifying
# removed cosmics, beam gas/halo and pileup configuration
# trigger will not be run

### Start of Sim

## Include common skeleton
include("SimuJobTransforms/skeleton.EVGENtoHIT.py")

if hasattr(runArgs, 'useISF') and not runArgs.useISF:
    raise RuntimeError("Unsupported configuration! If you want to run with useISF=False, please use AtlasG4_tf.py!")

## Get the logger
from AthenaCommon.Logging import *
fast_chain_log = logging.getLogger('ISF')
fast_chain_log.info('****************** STARTING ISF ******************')

### Force trigger to be off
from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)


## Simulation flags need to be imported first
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.ISFRun=True
from ISF_Config.ISF_jobProperties import ISF_Flags

## Set simulation geometry tag
if hasattr(runArgs, 'geometryVersion'):
    simFlags.SimLayout.set_Value_and_Lock(runArgs.geometryVersion)
    globalflags.DetDescrVersion = simFlags.SimLayout.get_Value()
    fast_chain_log.debug('SimLayout set to %s' % simFlags.SimLayout)
else:
    raise RuntimeError("No geometryVersion provided.")

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# Jobs should stop if an include fails.
if hasattr(runArgs, "IgnoreConfigError"):
    athenaCommonFlags.AllowIgnoreConfigError = runArgs.IgnoreConfigError
else:
    athenaCommonFlags.AllowIgnoreConfigError = False

athenaCommonFlags.DoFullChain=True

from AthenaCommon.BeamFlags import jobproperties

## Input Files
def setInputEvgenFileJobProperties(InputEvgenFile):
    from AthenaCommon.GlobalFlags import globalflags
    globalflags.InputFormat.set_Value_and_Lock('pool')
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.PoolEvgenInput.set_Value_and_Lock( InputEvgenFile )
    athenaCommonFlags.FilesInput.set_Value_and_Lock( InputEvgenFile )

if hasattr(runArgs, "inputFile"):
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
# We don't expect both inputFile and inputEVNT*File to be specified
if hasattr(runArgs, "inputEVNTFile"):
    setInputEvgenFileJobProperties( runArgs.inputEVNTFile )
elif hasattr(runArgs, "inputEVNT_COSMICSFile"):
    raise RuntimeError("Unsupported configuration! Currently cosmics simulation is not supported by FullChain_tf.py.")
elif hasattr(runArgs, "inputEVNT_CAVERNFile"):
    raise RuntimeError("Unsupported configuration! Currently cavern background simulation is not supported by FullChain_tf.py.")
elif hasattr(runArgs, "inputEVNT_STOPPEDFile"):
    raise RuntimeError("Unsupported configuration! Currently stopped gluino simulation is not supported by FullChain_tf.py.")
elif jobproperties.Beam.beamType.get_Value() == 'cosmics':
    fast_chain_log.debug('No inputEVNTFile provided. OK, as performing cosmics simulation.')
    athenaCommonFlags.PoolEvgenInput.set_Off()
else:
    fast_chain_log.info('No inputEVNTFile provided. Assuming that you are running a generator on the fly.')
    athenaCommonFlags.PoolEvgenInput.set_Off()

## Output hits file config
if hasattr(runArgs, "outputHITSFile"):
    athenaCommonFlags.PoolHitsOutput.set_Value_and_Lock( runArgs.outputHITSFile )
else:
    if hasattr(runArgs, "outputEVNT_STOPPEDFile"):
        raise RuntimeError("Unsupported configuration! Currently stopped gluino simulation is not supported by FullChain_tf.py.")
    #raise RuntimeError("No outputHITSFile provided.")
    fast_chain_log.info('No outputHITSFile provided. This simulation job will not write out any HITS file.')
    athenaCommonFlags.PoolHitsOutput = ""
    athenaCommonFlags.PoolHitsOutput.statusOn = False

#==============================================================
# Job Configuration parameters:
#==============================================================
## Pre-exec
if hasattr(runArgs, "preSimExec"):
    fast_chain_log.info("transform pre-sim exec")
    for cmd in runArgs.preSimExec:
        fast_chain_log.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs, "preSimInclude"):
    for fragment in runArgs.preSimInclude:
        include(fragment)


## Select detectors

from ISF_Config.ISF_jobProperties import ISF_Flags
if hasattr(runArgs, 'simulator'):
    ISF_Flags.Simulator = runArgs.simulator
else:
    ISF_Flags.Simulator = 'MC12G4'

# temporary fix to ensure TRT will record hits if using FATRAS
# this should eventually be removed when it is configured properly in ISF
if 'ATLFASTIIF' in ISF_Flags.Simulator():
    from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
    TrkDetFlags.TRT_BuildStrawLayers=True
    fast_chain_log.info('Enabled TRT_BuildStrawLayers to get hits in ATLFASTIIF')


try:
    from ISF_Config import FlagSetters
    FlagSetters.configureFlagsBase()
    ## Check for any simulator-specific configuration
    configureFlags = getattr(FlagSetters, ISF_Flags.Simulator.configFlagsMethodName(), None)
    if configureFlags is not None:
        configureFlags()
except:
    if 'DetFlags' not in dir():
       ## If you configure one det flag, you're responsible for configuring them all!
        from AthenaCommon.DetFlags import DetFlags
        DetFlags.all_setOn()
    DetFlags.LVL1_setOff() # LVL1 is not part of G4 sim
    DetFlags.Truth_setOn()
    DetFlags.Forward_setOff() # Forward dets are off by default
    checkHGTDOff = getattr(DetFlags, 'HGTD_setOff', None)
    if checkHGTDOff is not None:
        checkHGTDOff() #Default for now

from AthenaCommon.DetFlags import DetFlags
DetFlags.FTK_setOff() #FTK is not part of G4 sim
DetFlags.DBM_setOff() #Separate treatment of DBM is on hold
# note this makeRIO enables forward detectors, so have to set them off after
DetFlags.makeRIO.all_setOn()
## Configure Forward Detector DetFlags based on command-line options
if hasattr(runArgs, "AFPOn"):
    if runArgs.AFPOn:
        DetFlags.AFP_setOn()
if hasattr(runArgs, "ALFAOn"):
    if runArgs.ALFAOn:
        DetFlags.ALFA_setOn()
if hasattr(runArgs, "FwdRegionOn"):
    if runArgs.FwdRegionOn:
        DetFlags.FwdRegion_setOn()
if hasattr(runArgs, "LucidOn"):
    if runArgs.LucidOn:
        DetFlags.Lucid_setOn()
if hasattr(runArgs, "ZDCOn"):
    if runArgs.ZDCOn:
        DetFlags.ZDC_setOn()
if hasattr(runArgs, "HGTDOn"):
    if runArgs.HGTDOn:
        checkHGTDOn = getattr(DetFlags, 'HGTD_setOn', None)
        if checkHGTDOn is not None:
            checkHGTDOn()
        else:
            fast_chain_log.warning('The HGTD DetFlag is not supported in this release')
if not simFlags.SimulateNewSmallWheel():
    DetFlags.Micromegas_setOff()
    DetFlags.sTGC_setOff()
DetFlags.Print()
# removed configuration of forward detectors from standard simulation config
# corresponding code block removed

## Set the PhysicsList
if hasattr(runArgs, 'physicsList'):
    simFlags.PhysicsList = runArgs.physicsList


## Random seed
if hasattr(runArgs, "randomSeed"):
    simFlags.RandomSeedOffset = int(runArgs.randomSeed)
else:
    fast_chain_log.warning('randomSeed not set')
## Don't use the SeedsG4 override
simFlags.SeedsG4.set_Off()


## Set the Run Number (if required)
if hasattr(runArgs,"DataRunNumber"):
    if runArgs.DataRunNumber>0:
        fast_chain_log.info( 'Overriding run number to be: %s ', runArgs.DataRunNumber )
        simFlags.RunNumber=runArgs.DataRunNumber
elif hasattr(runArgs,'jobNumber'):
    if runArgs.jobNumber>=0:
        fast_chain_log.info( 'Using job number '+str(runArgs.jobNumber)+' to derive run number.' )
        simFlags.RunNumber = simFlags.RunDict.GetRunNumber( runArgs.jobNumber )
        fast_chain_log.info( 'Set run number based on dictionary to '+str(simFlags.RunNumber) )

## removed code block for handling cosmics track record

# get top sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Set Overall per-Algorithm time-limit on the AlgSequence
topSeq.TimeOut = 43200 * Units.s

try:
    from RecAlgs.RecAlgsConf import TimingAlg
    topSeq+=TimingAlg("SimTimerBegin", TimingObjOutputName = "EVNTtoHITS_timings")
except:
    fast_chain_log.warning('Could not add TimingAlg, no timing info will be written out.')

if hasattr(runArgs, 'truthStrategy'):
    ISF_Flags.BarcodeService   = 'Barcode_' + runArgs.truthStrategy + 'BarcodeSvc'
    ISF_Flags.TruthService     = 'ISF_'     + runArgs.truthStrategy + 'TruthService'
    ISF_Flags.EntryLayerFilter = 'ISF_'     + runArgs.truthStrategy + 'EntryLayerFilter'
    ISF_Flags.TruthStrategy    = runArgs.truthStrategy
    try:
        from BarcodeServices.BarcodeServicesConfig import barcodeOffsetForTruthStrategy
        simFlags.SimBarcodeOffset  = barcodeOffsetForTruthStrategy(runArgs.truthStrategy)
    except RuntimeError:
        if 'MC12' in runArgs.truthStrategy or 'MC15a' in runArgs.truthStrategy:
            simFlags.SimBarcodeOffset  = 200000 #MC12 setting
        else:
            simFlags.SimBarcodeOffset  = 1000000 #MC15 setting
        fast_chain_log.warning('Using unknown truth strategy '+str(runArgs.truthStrategy)+' guessing that barcode offset is '+str(simFlags.SimBarcodeOffset))
    except ImportError:
        # Temporary back-compatibility
        if 'MC12' in runArgs.truthStrategy or 'MC15a' in runArgs.truthStrategy:
            simFlags.SimBarcodeOffset  = 200000 #MC12 setting
        else:
            simFlags.SimBarcodeOffset  = 1000000 #MC15 setting
else:
    ISF_Flags.BarcodeService   = 'Barcode_MC12BarcodeSvc'
    ISF_Flags.TruthService     = 'ISF_TruthService'
    ISF_Flags.EntryLayerFilter = 'ISF_MC12EntryLayerFilter'
    ISF_Flags.TruthStrategy    = 'MC12'
    simFlags.SimBarcodeOffset  = 200000 #MC12 setting
#### *********** import ISF_Example code here **************** ####

include("ISF_Config/ISF_ConfigJobInclude.py")

## check to see if  pileup emulation is being used, if so do post-ISF-config
## actions to enable simulation of pileup collection
if 'AthSequencer/EvgenGenSeq' in topSeq.getSequence():
    fast_chain_log.info("Pileup emulation enabled - setup GenEventStackFiller")
    include("FastChainPileup/FastPileupSimConfig.py")

## Add AMITag MetaData to TagInfoMgr
if hasattr(runArgs, 'AMITag'):
    if runArgs.AMITag != "NONE":
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", runArgs.AMITag]

## Increase max RDO output file size to 10 GB
## NB. We use 10GB since Athena complains that 15GB files are not supported
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.AthenaPoolCnvSvc.MaxFileSizes = [ "10000000000" ]

### Changing to post-sim include/exec
## Post-include
if hasattr(runArgs, "postSimInclude"):
    for fragment in runArgs.postSimInclude:
        include(fragment)

# Avoid command line postInclude for stopped particles
if hasattr(runArgs, "outputEVNT_STOPPEDFile"):
    include('SimulationJobOptions/postInclude.StoppedParticleWrite.py')

## Post-exec
if hasattr(runArgs, "postSimExec"):
    fast_chain_log.info("transform post-sim exec")
    for cmd in runArgs.postSimExec:
        fast_chain_log.info(cmd)
        exec(cmd)

## Always enable the looper killer, unless it's been disabled
if ISF_Flags.UsingGeant4():
    if not hasattr(runArgs, "enableLooperKiller") or runArgs.enableLooperKiller:
        print "GOT HERE 1"
        try:
            if (hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions()):
                print "GOT HERE 2"
                # this configures the MT LooperKiller
                from G4UserActions import G4UserActionsConfig
                try:
                    G4UserActionsConfig.addLooperKillerTool()
                except AttributeError:
                    fast_chain_log.warning("Could not add the MT-version of the LooperKiller")
            else:
                print "GOT HERE 3"
                # this configures the non-MT looperKiller
                try:
                    from G4AtlasServices.G4AtlasUserActionConfig import UAStore
                except ImportError:
                    print "GOT HERE 4"
                    from G4AtlasServices.UserActionStore import UAStore
                # add default configurable
                UAStore.addAction('LooperKiller',['Step'])
        except:
            print "GOT HERE 3"
            # Pre UserAction Migration
            def use_looperkiller():
                from G4AtlasApps import PyG4Atlas, AtlasG4Eng
                lkAction = PyG4Atlas.UserAction('G4UserActions', 'LooperKiller', ['BeginOfRun', 'EndOfRun', 'BeginOfEvent', 'EndOfEvent', 'Step'])
                AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(lkAction)
            simFlags.InitFunctions.add_function("postInit", use_looperkiller)
    else:
        fast_chain_log.warning("The looper killer will NOT be run in this job.")

### End of Sim


### Start of Digi

include("SimuJobTransforms/CommonSkeletonJobOptions.py")

if hasattr(runArgs, "jobNumber"):
    if runArgs.jobNumber < 1:
        raise ValueError('jobNumber must be a postive integer. %s lies outside this range', str(runArgs.jobNumber))

from AthenaCommon.GlobalFlags import globalflags
if hasattr(runArgs,"geometryVersion"):
    # strip _VALIDATION
    print "stripping _VALIDATION"
    if runArgs.geometryVersion.endswith("_VALIDATION"):
        pos=runArgs.geometryVersion.find("_VALIDATION")
        globalflags.DetDescrVersion.set_Value_and_Lock( runArgs.geometryVersion[:pos] )
    else:
        globalflags.DetDescrVersion.set_Value_and_Lock( runArgs.geometryVersion )

### Do not invoke another logger
# get the logger
#from AthenaCommon.Logging import logging
digilog = logging.getLogger('Digi_trf')
fast_chain_log.info( '****************** STARTING DIGITIZATION *****************' )


fast_chain_log.info( '**** Transformation run arguments' )
fast_chain_log.info( str(runArgs) )

#==============================================================
# Job Configuration parameters:
#==============================================================
### Changing pre-exec to pre-digi exec
## Pre-exec
if hasattr(runArgs,"preDigiExec"):
    fast_chain_log.info("transform pre-digi exec")
    for cmd in runArgs.preDigiExec:
        fast_chain_log.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preDigiInclude"):
    for fragment in runArgs.preDigiInclude:
        include(fragment)

#--------------------------------------------------------------
# Override pile-up configuration on the command-line
#--------------------------------------------------------------

from Digitization.DigitizationFlags import digitizationFlags

#--------------------------------------------------------------
# Get the flags
#--------------------------------------------------------------
if hasattr(runArgs,"digiSeedOffset1"):
    digitizationFlags.rndmSeedOffset1=int(runArgs.digiSeedOffset1)
else:
    fast_chain_log.warning( 'digiSeedOffset1 not set' )
    digitizationFlags.rndmSeedOffset1=1

if hasattr(runArgs,"digiSeedOffset2"):
    digitizationFlags.rndmSeedOffset2=int(runArgs.digiSeedOffset2)
else:
    fast_chain_log.warning( 'digiSeedOffset2 not set' )
    digitizationFlags.rndmSeedOffset2=2

if hasattr(runArgs,"samplingFractionDbTag"): #FIXME change this to PhysicsList?
    digitizationFlags.physicsList=runArgs.samplingFractionDbTag

if hasattr(runArgs,"digiRndmSvc"):
    digitizationFlags.rndmSvc=runArgs.digiRndmSvc

if hasattr(runArgs,"conditionsTag"):
    if(runArgs.conditionsTag!='NONE'):
        digitizationFlags.IOVDbGlobalTag = runArgs.conditionsTag

### Avoid meta data reading
digitizationFlags.overrideMetadata=['ALL']

if hasattr(runArgs,"digiSteeringConf"):
    if not (digitizationFlags.digiSteeringConf.get_Value()==runArgs.digiSteeringConf+"PileUpToolsAlg"):
        digilog.info( "Changing digitizationFlags.digiSteeringConf from %s to %s", digitizationFlags.digiSteeringConf.get_Value(),runArgs.digiSteeringConf)
        digitizationFlags.digiSteeringConf=runArgs.digiSteeringConf+"PileUpToolsAlg"
        PileUpConfigOverride=True

#--------------------------------------------------------------
# Pileup configuration - removed as pileup will be handled on-the-fly
#--------------------------------------------------------------

#--------------------------------------------------------------
# Other configuration: LVL1, turn off sub detectors, calo noise
#--------------------------------------------------------------

if hasattr(runArgs,"doAllNoise"):
    if runArgs.doAllNoise!="NONE":
        fast_chain_log.info('doAllNoise = %s: Overriding doInDetNoise, doCaloNoise and doMuonNoise', runArgs.doAllNoise)
        if runArgs.doAllNoise=="True":
            digitizationFlags.doInDetNoise=True
            digitizationFlags.doCaloNoise=True
            digitizationFlags.doMuonNoise=True
        else:
            digitizationFlags.doInDetNoise=False
            digitizationFlags.doCaloNoise=False
            digitizationFlags.doMuonNoise=False



### No RDO output
## Output RDO File
if hasattr(runArgs,"outputRDOFile") or hasattr(runArgs,"tmpRDO"):
    if hasattr(runArgs,"outputRDOFile"):
        if hasattr(runArgs,"tmpRDO"):
            fast_chain_log.fatal("Both outputRDOFile and tmpRDO specified - this configuration should not be used!")
            raise SystemError
        athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.outputRDOFile )
    if hasattr(runArgs,"tmpRDO"):
        athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.tmpRDO )
    if hasattr(runArgs, "AddCaloDigi"):
        AddCaloDigi = runArgs.AddCaloDigi
        if AddCaloDigi:
            fast_chain_log.info("Will write out all LArDigitContainers and TileDigitsContainers to RDO file.")
            digitizationFlags.experimentalDigi+=["AddCaloDigi"]
else:
    fast_chain_log.info("no output file (outputRDOFile or tmpRDO) specified - switching off output StreamRDO")

# Adjusting DetFlags for Digitization
fast_chain_log.info('Adjusting DetFlags for Digitization configuration')
def TidyDetFlagsForDigi():
    subdetectors=[]
    subdetectors+=["bpipe","pixel","SCT","TRT","BCM","DBM"]#ID
    subdetectors+=["Lucid", "ZDC", "ALFA", "AFP", "FwdRegion"]#Forward
    subdetectors+=["em","HEC","FCal","Tile"]#Calo
    subdetectors+=["MDT","CSC","TGC","RPC","sTGC","Micromegas"]#Muon
    subdetectors+=["Truth"]#Truth
    subdetectors+=["LVL1", "BField","FTK"]#misc

    for detector in subdetectors:
        checkSimulate = getattr(DetFlags.simulate, '%s_on'%detector, None)
        checkGeometry = getattr(DetFlags.simulate, '%s_on'%detector, None)
        if checkSimulate is not None:
            if checkSimulate() and checkGeometry():
                detString = '%s_setOn'%detector
            else:
                detString = '%s_setOff'%detector
            set_digitize = getattr(DetFlags.digitize, detString, None)
            set_writeRDOPool = getattr(DetFlags.writeRDOPool, detString, None)
            set_makeRIO = getattr(DetFlags.makeRIO, detString, None)
            set_digitize()
            set_writeRDOPool()
            set_makeRIO() #FIXME this is overridden by Digitization/Digitization.py
TidyDetFlagsForDigi()
DetFlags.Print()

#--------------------------------------------------------------
# Go for it
#--------------------------------------------------------------
if hasattr(runArgs,"DataRunNumber"):
    if runArgs.DataRunNumber>0:
        fast_chain_log.info( 'Overriding run number to be: %s ', runArgs.DataRunNumber )
        digitizationFlags.dataRunNumber=runArgs.DataRunNumber

print "lvl1: -14... " + str(DetFlags.digitize.LVL1_on())

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Set Overall per-Algorithm time-limit on the AlgSequence
topSeq.TimeOut = 43200 * Units.s

try:
    from RecAlgs.RecAlgsConf import TimingAlg
    topSeq+=TimingAlg("DigiTimerBegin", TimingObjOutputName = "HITStoRDO_timings")
except:
    fast_chain_log.warning('Could not add TimingAlg, no timing info will be written out.')

include ("Digitization/Digitization.py")

if hasattr(runArgs,"AMITag"):
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", runArgs.AMITag ]

### No RDO output to increase file size of
# Increase max RDO output file size to 10 GB

#from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#svcMgr.AthenaPoolCnvSvc.MaxFileSizes = [ "10000000000" ] #[ "15000000000" ] #Athena complains that 15GB files are not supported

## Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    fast_chain_log.info("transform post-exec")
    for cmd in runArgs.postExec:
        fast_chain_log.info(cmd)
        exec(cmd)

### End of Digi
 
#from AthenaCommon.ConfigurationShelve import saveToAscii
#saveToAscii('config.txt')
