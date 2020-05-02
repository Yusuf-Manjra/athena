#replace the following with 
#include("TrigValTools/leakCheck.py")	
#
# add for memory tracking
# from RecExConfig.RecFlags import rec
# from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if  ('checkLeak' in dir()):
    printfunc ("setting up Leak-Checking and output File")
    import Hephaestus.MemoryTracker as memtrack
    # Try to write the leaks into separate file 
    try: 
        memtrack.depth( 22 )            # sets the total search depth
        memtrack.MemoryTrace.size( 19 ) # size of traces kept in memory
        memtrack.outstream( open("LeakCheck.txt","w") )
    except: 
        printfunc ("ERROR setting memory depth")


# flags for command-line input
if  ('chainOrderOption' in dir()):
    if  chainOrderOption == 1:
        orderHLTChains=1
    elif chainOrderOption == -1:
        orderHLTChains=-1
    else:
        orderHLTChains=1
#see ATR-11913 or more detailed:https://its.cern.ch/jira/browse/ATR-11913?focusedCommentId=852792&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-852792 
    TriggerFlags.MuonSlice.disableSignatures(['HLT_mu0_muoncalib_ds3'])
    
    # set direct or inverse order of chain execution
    printfunc ("Setting order of trigger chain execution")
    #    from TrigSteering.TrigSteeringConfig import TrigSteer_HLT
    #    TrigSteer_HLT = TrigSteer_HLT("TrigSteer_HLT")
    #    topSequence += TrigSteer_HLT
    topSequence.TrigSteer_HLT.sortChains=orderHLTChains
    printfunc ("topSequence.TrigSteer_HLT.sortChains",)
    printfunc (topSequence.TrigSteer_HLT.sortChains )
    printfunc ("Done setting chain execution order")


# flags for fast calo access 
if ('doFastCaloAccess' in dir()):
    
    printfunc ("ERROR tests of FastCalo Access no longer needed")
    
    ##### MET slice configuration: ######
    EFMissingET_Fex= Configurable.allConfigurables.get("EFMissingET_Fex")
    EFMissingET_Fex_FEB= Configurable.allConfigurables.get("EFMissingET_Fex_FEB")
    EFMissingET_Fex_noiseSupp= Configurable.allConfigurables.get("EFMissingET_Fex_noiseSupp")
    
    if doFastCaloAccess:
        EFMissingET_Fex.Tools["TheCellTool"].useFullCollection = True
        EFMissingET_Fex_FEB.Tools["TheCellTool"].useFullCollection = True
        EFMissingET_Fex_noiseSupp.Tools["TheCellTool"].useFullCollection = True
    else:
        EFMissingET_Fex.Tools["TheCellTool"].useFullCollection = False
        EFMissingET_Fex_FEB.Tools["TheCellTool"].useFullCollection = False
        EFMissingET_Fex_noiseSupp.Tools["TheCellTool"].useFullCollection = False


#-----------------Monitoring -----------------
if not ('checkLeak' in dir()):
    from PerfMonComps import PerfMonFlags
    if not ('doPerfMon' in dir()):
        rec.doPerfMon = True
    if rec.doPerfMon:
        rec.doDetailedPerfMon = False
        rec.doSemiDetailedPerfMon = False
        jobproperties.PerfMonFlags.doMonitoring = True
        jobproperties.PerfMonFlags.doDetailedMonitoring = False
        jobproperties.PerfMonFlags.doSemiDetailedMonitoring = False
        jobproperties.PerfMonFlags.doFastMon = False
        jobproperties.PerfMonFlags.OutputFile = "ntuple.root"


# set this true to measure memory increase on repeated events
if not ('memFreeze' in dir()):
    memFreeze=False

if memFreeze:
    #---turn off extra PefMon stuff to limit memory growth from monitoring
    if rec.doPerfMon:
        jobproperties.PerfMonFlags.doFastMon = True
        jobproperties.PerfMonFlags.doDetailedMonitoring = False
    #---turn off noise in trigger towers -----
    triggerTowerMaker.ADCNoise=0
#-----------------------------------------------------------


# formatting of log output
MessageSvc.debugLimit = 10000
MessageSvc.infoLimit  = 1000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

# disable printing of histograms in finalize
if hasattr(svcMgr,'THistSvc'):
    svcMgr.THistSvc.OutputLevel = WARNING

#from AthenaCommon.AlgSequence import AlgSequence
printfunc (AlgSequence)
printfunc (ServiceMgr)

# Tempory - 24/05/17 # this can be later removed
if not hasattr(svcMgr.ToolSvc,"TileROD_Decoder"):
   from TileByteStream.TileByteStreamConf import TileROD_Decoder
   svcMgr.ToolSvc+=TileROD_Decoder()
svcMgr.ToolSvc.TileROD_Decoder.fullTileMode=0
