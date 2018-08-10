# Import the configurable algorithms for TrigT1Calo
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger('TrigT1CaloSimJO_reprocessTT')
log.debug("Starting reprocessing of LVL1 trigger towers")

globalflags.DatabaseInstance = 'CONDBR2'

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc,findFileInXMLPATH
LVL1ConfigSvc = LVL1ConfigSvc('LVL1ConfigSvc')
LVL1ConfigSvc.XMLFile = findFileInXMLPATH(TriggerFlags.inputLVL1configFile())
svcMgr +=  LVL1ConfigSvc

# Do the reprocessing of TriggerTowers      
#include('TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py')

from IOVDbSvc.CondDB import conddb

conddb.blockFolder("/TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels")
conddb.blockFolder("/TRIGGER/L1Calo/V2/Conditions/DisabledTowers")
conddb.blockFolder("/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults")
conddb.blockFolder("/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib")

conddb.addFolder("", "<dbConnection>frontier://ATLF/();schema=ATLAS_COOLOFL_TRIGGER;dbname=OFLP200</dbConnection>/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib<tag>V2-PHYSICS-CHANCALIB-00-00</tag>", force=True)
conddb.addFolder("", "<dbConnection>frontier://ATLF/();schema=ATLAS_COOLOFL_TRIGGER;dbname=OFLP200</dbConnection>/TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels<tag>V2-DEADCHANNELS-00-00</tag>", force=True)
conddb.addFolder("", "<dbConnection>frontier://ATLF/();schema=ATLAS_COOLOFL_TRIGGER;dbname=OFLP200</dbConnection>/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults<tag>V2-CHANDEFAULTS-00-00</tag>", force=True)
conddb.addFolder("", "<dbConnection>frontier://ATLF/();schema=ATLAS_COOLOFL_TRIGGER;dbname=OFLP200</dbConnection>/TRIGGER/L1Calo/V2/Conditions/DisabledTowers<tag>V2-DISABLEDTOWERS-00-00</tag>", force=True)

#reprocess
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2TriggerTowerMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2CPMTowerMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2JetElementMaker
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMSim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMJetSim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMEnergySim
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyCMX
from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__RoIROD

# Get the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Get RunNumber from input file
_my_run_number = None
try:
    # first attempt RecExConfig ...
    from RecExConfig.AutoConfiguration import GetRunNumber
    _my_run_number = GetRunNumber()
    # sometime GetRunNumber doesn't raise an exception if something goes wrong ...
    if _my_run_number < 100000: raise ValueError('non-sensible RunNumber')
except BaseException, E:
    log.info('Exception retrieving RunNumber from RecExConfig.AutoConfiguration: %s' % E)
    #  let's see if we are lucky with AthFile directly ...
    try:
        import PyUtils.AthFile as athFile
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        af = athFile.fopen(athenaCommonFlags.BSRDOInput()[0])
        _my_run_number = af.run_number[0] # don't modify global state
    except BaseException, EE:
        log.info('Exception retrieving RunNumber from AthFile: %s' % EE)
        # one last shot ...
        try:
            global _run_number
            _my_run_number = _run_number
        except:
            # now give up
            _my_run_number = None
            log.warning("Could not retrieve RunNumber from inputs. Assuming Run-2 data.")
            
# on Run-1 data, schedule the {CPMTower,JetElement}CnvAlg if necessary
if _my_run_number:
    log.info('Using RunNumber %d' % _my_run_number)
if _my_run_number and _my_run_number < 222222:
    job += CfgMgr.xAODMaker__CPMTowerCnvAlg()
    job += CfgMgr.xAODMaker__JetElementCnvAlg()
    # add to bytestream decoder if not already there
    if globalflags.InputFormat() == 'bytestream':
        if not CfgMgr.ByteStreamAddressProviderSvc() in svcMgr:
            # might not have been added to the ServiceMgr yet ...
            svcMgr += CfgMgr.ByteStreamAddressProviderSvc()
        if not "DataVector<LVL1::CPMTower>/CPMTowers" in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
            svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMTower>/CPMTowers" ]
        if not "DataVector<LVL1::JetElement>/JetElements" in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
            svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JetElement>/JetElements" ]

        try:
            # unpacking Run-1 data directly to xAOD::CPMTowers is not supported
            svcMgr.ByteStreamAddressProviderSvc.TypeNames.remove("xAOD::CPMTowerContainer/CPMTowers")
            svcMgr.ByteStreamAddressProviderSvc.TypeNames.remove("xAOD::CPMTowerAuxContainer/CPMTowersAux.")
            svcMgr.ByteStreamAddressProviderSvc.TypeNames.remove("xAOD::JetElementContainer/JetElements")
            svcMgr.ByteStreamAddressProviderSvc.TypeNames.remove("xAOD::JetElementAuxContainer/JetElementsAux.")
        except ValueError:
            pass
# clean up
del _my_run_number

#add algorithms to sequence
job += LVL1__Run2TriggerTowerMaker( 'Run2TriggerTowerMaker',
                                            CellType=2,)
job.Run2TriggerTowerMaker.ZeroSuppress = False # store all trigger towers 
job.Run2TriggerTowerMaker.IsReprocessing = True # tell run2triggertowermaker that this is a reprocessing job 
job += LVL1__Run2CPMTowerMaker( 'CPMTowerMaker',)
job += LVL1__Run2JetElementMaker( 'JetElementMaker',)
job += LVL1__CPMSim( 'CPMSim',)
job += LVL1__JEMJetSim( 'JEMJetSim',)
job += LVL1__JEMEnergySim( 'JEMEnergySim',)
job += LVL1__CPCMX( 'CPCMX',)
job += LVL1__JetCMX( 'JetCMX',)
job += LVL1__EnergyCMX( 'EnergyCMX',)
job += LVL1__RoIROD( 'RoIROD',)

