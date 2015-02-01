### usually ATN tests runs with following RDO input:
#PoolRDOInput=["/afs/cern.ch/atlas/offline/ReleaseData/v3/testfile/valid1.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s488_d151_tid039414_RDO.039414._00001_extract_10evt.pool.root"]


from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO

###############################

doTrigger=True
TriggerModernConfig=True

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

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

#------------ This is for ATN/RTT tests only ---------
#TriggerFlags.triggerMenuSetup = 'default' 
TriggerFlags.triggerMenuSetup = 'Physics_pp_v5' 

#-------------end of flag for tests-------------------

#------------ run only Muon --------------------------
DetFlags.Calo_setOff()
DetFlags.detdescr.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.detdescr.ID_setOn()

TriggerFlags.doCalo.set_Off()
TriggerFlags.doBcm.set_Off()
TriggerFlags.doLucid.set_Off()

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

print DetFlags
print TriggerFlags

#------------- temporary hack -----------------------------
if hasattr(AlgSequence(), 'StreamBS'):
    print 'Removing LArRawChannels/2721 from StreamBS list'
    streamBS = AlgSequence().StreamBS
    streamBS.ItemList.remove('2721#*')
    print streamBS
