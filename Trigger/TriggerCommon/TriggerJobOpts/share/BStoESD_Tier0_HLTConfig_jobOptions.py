# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
##############################################################
# BStoESD_Tier0_HLTConfig_jobOptions.py
# For DATA reconstruction
##############################################################

# Configuration depends on TriggerFlags.configForStartup():
#   HLToffline       : HLT is ran offline, configuration is read from JSON files
#   HLTonline        : Normal running, everything is taken from COOL

from RecExConfig.RecFlags import rec
from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.Include import include

from AthenaCommon.Logging import logging
_log = logging.getLogger("BStoESD_Tier0_HLTConfig_jobOptions.py")
_log.info("TriggerFlag.configForStartup = %s", tf.configForStartup())

assertMsg = 'This file is meant for Trigger configuration in RAWtoESD/RAWtoALL data reconstruction.'
assert rec.doTrigger(), assertMsg + ' Since rec.doTrigger is disabled, this file should not be included.'

# First check if HLT psk is ok, if not, turn trigger off.
if tf.configForStartup() != 'HLToffline':
    include( "TriggerJobOpts/TriggerConfigCheckHLTpsk.py" )

if rec.doTrigger():
    
    # configure the HLT config
    if not hasattr( ServiceMgr, "ByteStreamAddressProviderSvc" ):
        from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
        ServiceMgr += ByteStreamAddressProviderSvc()

    ConfigFlags.Trigger.readBS = True
    tf.doLVL1= False # needed to not rerun the trigger
    tf.doHLT= False # needed to not rerun the trigger
    if ConfigFlags.Trigger.EDMVersion >= 3:
        # for Run 3 we eventually want to disable TrigConfigSvc
        tf.configurationSourceList = []
    elif ConfigFlags.Trigger.EDMVersion == 1 or ConfigFlags.Trigger.EDMVersion == 2:
        # for reconstructing Run 1/2 data we need to run the trigger configuration
        # from the Run 1/2 TriggerDB, which is done by the TrigConfigSvc(DSConfigSvc)
        tf.configurationSourceList = ['ds']
        # this configurations are in the old format
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        _log.info("Setting ConfigFlags.Trigger.readLVL1FromJSON to False as we are reconstructing Run %s data",
                  ConfigFlags.Trigger.EDMVersion)
        ConfigFlags.Trigger.readLVL1FromJSON = False
    else:
        raise RuntimeError("Invalid EDMVersion=%s " % ConfigFlags.Trigger.EDMVersion)


    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter()

    from TriggerJobOpts.T0TriggerGetter import T0TriggerGetter
    triggerGetter = T0TriggerGetter()

    if rec.doWriteBS():
        include( "ByteStreamCnvSvc/RDP_ByteStream_jobOptions.py" )

del _log
## end of configure the HLT config
