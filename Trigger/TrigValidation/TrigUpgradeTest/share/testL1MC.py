#Adapted from L1 test in TriggerTest

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO
#scan for RTT files (only if dsName and fileRange set)
include("TriggerTest/TrigScanFiles.py")
###############################

doTrigger=True
TriggerModernConfig=True

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.OutputLevel( INFO )
AlgScheduler.CheckDependencies( True )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )

rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False 
rec.doESD.set_Value_and_Lock(False) 
doTAG=False
rec.doCBNT=False

#rec.doTruth=False
rec.doTruth.set_Value_and_Lock(False)

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

#------------ This is for ATN/RTT tests only ---------
#TriggerFlags.triggerMenuSetup = 'default' 
TriggerFlags.triggerMenuSetup = 'Physics_pp_v7' 

#------------ run only LVL1 -------------------------- 
TriggerFlags.doHLT=False
#TriggerFlags.doL1Topo=True 


def L1Only():
    TriggerFlags.Slices_all_setOff()

try:
    from TriggerMenu import useNewTriggerMenu
    useNewTM = useNewTriggerMenu()
    log.info("Using new TriggerMenu: %r" % useNewTM)
except:
    useNewTM = False
    log.info("Using old TriggerMenuPython since TriggerMenu.useNewTriggerMenu can't be imported")

if useNewTM:
    from TriggerMenu.menu.GenerateMenu import GenerateMenu
else:
    from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(L1Only)

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

#------------------------------------------------------------
include("TriggerTest/TriggerTestCommon.py")
#------------------------------------------------------------

DetFlags.makeRIO.Calo_setOff()

