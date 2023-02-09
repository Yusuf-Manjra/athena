# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def MainServicesMiniCfg(loopMgr='AthenaEventLoopMgr', masterSequence='AthAlgSeq'):
    #Mininmal basic config, just good enough for HelloWorld and alike
    cfg=ComponentAccumulator(CompFactory.AthSequencer(masterSequence,Sequential=True))
    cfg.setAsTopLevel()
    cfg.setAppProperty('TopAlg',['AthSequencer/'+masterSequence])
    cfg.setAppProperty('MessageSvcType', 'MessageSvc')
    cfg.setAppProperty('EventLoop', loopMgr)
    cfg.setAppProperty('ExtSvcCreates', 'False')
    cfg.setAppProperty('JobOptionsSvcType', 'JobOptionsSvc')
    cfg.setAppProperty('JobOptionsType', 'NONE')
    cfg.setAppProperty('JobOptionsPostAction', '')
    cfg.setAppProperty('JobOptionsPreAction', '')
    cfg.setAppProperty('PrintAlgsSequence', True)
    return cfg


def AvalancheSchedulerSvcCfg(flags, **kwargs):
    kwargs.setdefault("CheckDependencies", flags.Scheduler.CheckDependencies)
    kwargs.setdefault("CheckOutputUsage", flags.Scheduler.CheckOutputUsage)
    kwargs.setdefault("ShowDataDependencies", flags.Scheduler.ShowDataDeps)
    kwargs.setdefault("ShowDataFlow", flags.Scheduler.ShowDataFlow)
    kwargs.setdefault("ShowControlFlow", flags.Scheduler.ShowControlFlow)
    kwargs.setdefault("VerboseSubSlots", flags.Scheduler.EnableVerboseViews)
    kwargs.setdefault("ThreadPoolSize", flags.Concurrency.NumThreads)

    cfg = ComponentAccumulator()
    cfg.addService(CompFactory.AvalancheSchedulerSvc(**kwargs), primary=True)
    return cfg


def OutputUsageIgnoreCfg(flags, algorithm):
    cfg = ComponentAccumulator()
    if flags.Concurrency.NumThreads > 0 and flags.Scheduler.CheckOutputUsage:
       cfg.merge(AvalancheSchedulerSvcCfg(flags, CheckOutputUsageIgnoreList=[algorithm]))
    return cfg


def AthenaEventLoopMgrCfg(flags):

    cfg = ComponentAccumulator()
    elmgr = CompFactory.AthenaEventLoopMgr()
    if flags.Input.OverrideRunNumber:
        from AthenaKernel.EventIdOverrideConfig import EvtIdModifierSvcCfg
        elmgr.EvtIdModifierSvc = cfg.getPrimaryAndMerge( EvtIdModifierSvcCfg(flags) ).name

    if flags.Common.isOverlay:
        if not flags.Overlay.DataOverlay:
            elmgr.RequireInputAttributeList = True
            elmgr.UseSecondaryEventNumber = True

    cfg.addService( elmgr )

    return cfg


def AthenaHiveEventLoopMgrCfg(flags):

    cfg = ComponentAccumulator()
    hivesvc = CompFactory.SG.HiveMgrSvc("EventDataSvc")
    hivesvc.NSlots = flags.Concurrency.NumConcurrentEvents
    cfg.addService( hivesvc )

    from AthenaCommon.Constants import INFO
    arp = CompFactory.AlgResourcePool( OutputLevel = INFO )
    arp.TopAlg = ["AthMasterSeq"] #this should enable control flow
    cfg.addService( arp )

    scheduler = cfg.getPrimaryAndMerge(AvalancheSchedulerSvcCfg(flags))

    from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
    # FailIfNoProxy=False makes it a warning, not an error, if unmet data
    # dependencies are not found in the store.  It should probably be changed
    # to True eventually.
    inputloader_ca = SGInputLoaderCfg(flags, FailIfNoProxy=flags.Input.FailOnUnknownCollections)
    cfg.merge(inputloader_ca, sequenceName="AthAlgSeq")
    # Specifying DataLoaderAlg makes the Scheduler automatically assign
    # all unmet data dependencies to that algorithm
    if flags.Scheduler.AutoLoadUnmetDependencies:
        scheduler.DataLoaderAlg = inputloader_ca.getPrimary().getName()

    elmgr = CompFactory.AthenaHiveEventLoopMgr()
    elmgr.WhiteboardSvc = "EventDataSvc"
    elmgr.SchedulerSvc = scheduler.getName()
    elmgr.OutStreamType = 'AthenaOutputStream'

    if flags.Input.OverrideRunNumber:
        from AthenaKernel.EventIdOverrideConfig import EvtIdModifierSvcCfg
        elmgr.EvtIdModifierSvc =  cfg.getPrimaryAndMerge(EvtIdModifierSvcCfg(flags)).name

    if flags.Common.isOverlay:
        if not flags.Overlay.DataOverlay:
            elmgr.RequireInputAttributeList = True
            elmgr.UseSecondaryEventNumber = True

    cfg.addService( elmgr )

    return cfg


def MessageSvcCfg(flags):
    cfg = ComponentAccumulator()
    msgsvc = CompFactory.MessageSvc()
    msgsvc.OutputLevel = flags.Exec.OutputLevel
    msgsvc.Format = "% F%{:d}W%C%7W%R%T %0W%M".format(flags.Common.MsgSourceLength)
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep not in [ProductionStep.Default, ProductionStep.Reconstruction]:
        msgsvc.Format = "% F%18W%S%7W%R%T %0W%M" # Temporary to match legacy configuration for serial simulation/digitization/overlay jobs
    if flags.Concurrency.NumThreads>0:
        # Migrated code from AtlasThreadedJob.py
        msgsvc.defaultLimit = 0
        msgsvc.Format = "% F%{:d}W%C%6W%R%e%s%8W%R%T %0W%M".format(flags.Common.MsgSourceLength)
    if flags.Exec.VerboseMessageComponents:
        msgsvc.verboseLimit=0
    if flags.Exec.DebugMessageComponents:
        msgsvc.debugLimit=0
    if flags.Exec.InfoMessageComponents:
        msgsvc.infoLimit=0
    if flags.Exec.WarningMessageComponents:
        msgsvc.warningLimit=0
    if flags.Exec.ErrorMessageComponents:
        msgsvc.errorLimit=0

    cfg.addService(msgsvc)
    return cfg


def MainServicesCfg(flags, LoopMgr='AthenaEventLoopMgr'):
    # Run a serial job for threads=0
    if flags.Concurrency.NumThreads>0:
        if flags.Concurrency.NumConcurrentEvents==0:
            # In a threaded job this will mess you up because no events will be processed
            raise Exception("Requested Concurrency.NumThreads>0 and Concurrency.NumConcurrentEvents==0, which will not process events!")
        LoopMgr = "AthenaHiveEventLoopMgr"

    if flags.Concurrency.NumProcs>0:
        LoopMgr = "AthMpEvtLoopMgr"

    ########################################################################
    # Core components needed for serial and threaded jobs
    cfg=MainServicesMiniCfg(loopMgr=LoopMgr, masterSequence='AthMasterSeq')
    cfg.setAppProperty('OutStreamType', 'AthenaOutputStream')

    #Build standard sequences:
    AthSequencer = CompFactory.AthSequencer
    cfg.addSequence(AthSequencer('AthAlgEvtSeq',Sequential=True, StopOverride=True),parentName="AthMasterSeq")
    cfg.addSequence(AthSequencer('AthOutSeq',StopOverride=True),parentName="AthMasterSeq")

    cfg.addSequence(AthSequencer('AthBeginSeq',Sequential=True),parentName='AthAlgEvtSeq')
    cfg.addSequence(AthSequencer('AthAllAlgSeq',StopOverride=True),parentName='AthAlgEvtSeq')

    if flags.Concurrency.NumThreads==0:
        # For serial execution, we need the CondAlgs to execute first.
        cfg.addSequence(AthSequencer('AthCondSeq',StopOverride=True),parentName='AthAllAlgSeq')
        cfg.addSequence(AthSequencer('AthAlgSeq',IgnoreFilterPassed=True,StopOverride=True, ProcessDynamicDataDependencies=True, ExtraDataForDynamicConsumers=[] ),parentName='AthAllAlgSeq')
    else:
        # In MT, the order of execution is irrelevant (determined by data deps).
        # We add the conditions sequence later such that the CondInputLoader gets
        # initialized after all other user Algorithms for MT, so the base classes
        # of data deps can be correctly determined.
        cfg.addSequence(AthSequencer('AthAlgSeq', IgnoreFilterPassed=True, StopOverride=True, ProcessDynamicDataDependencies=True, ExtraDataForDynamicConsumers=[]), parentName='AthAllAlgSeq')
        cfg.addSequence(AthSequencer('AthCondSeq',StopOverride=True),parentName='AthAllAlgSeq')

    cfg.addSequence(AthSequencer('AthEndSeq',Sequential=True),parentName='AthAlgEvtSeq')
    cfg.setAppProperty('PrintAlgsSequence', True)

    #Set up incident firing:
    AthIncFirerAlg=CompFactory.AthIncFirerAlg
    IncidentProcAlg=CompFactory.IncidentProcAlg

    previousPerfmonDomain = cfg.getCurrentPerfmonDomain()
    cfg.flagPerfmonDomain('Incidents')

    cfg.addEventAlgo(AthIncFirerAlg("BeginIncFiringAlg",FireSerial=False,Incidents=['BeginEvent']),sequenceName='AthBeginSeq')
    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg1'),sequenceName='AthBeginSeq')

    cfg.addEventAlgo(AthIncFirerAlg('EndIncFiringAlg',FireSerial=False,Incidents=['EndEvent']), sequenceName="AthEndSeq")
    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg2'),sequenceName="AthEndSeq")

    # Should be after all other algorithms.
    cfg.addEventAlgo(AthIncFirerAlg('EndAlgorithmsFiringAlg',FireSerial=False,Incidents=['EndAlgorithms']), sequenceName="AthMasterSeq")
    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg3'),sequenceName="AthMasterSeq")

    cfg.flagPerfmonDomain(previousPerfmonDomain)

    #Basic services:
    ClassIDSvc=CompFactory.ClassIDSvc
    cfg.addService(ClassIDSvc(CLIDDBFiles= ['clid.db',"Gaudi_clid.db" ]))

    AlgContextSvc=CompFactory.AlgContextSvc
    cfg.addService(AlgContextSvc(BypassIncidents=True))
    cfg.addAuditor(CompFactory.AlgContextAuditor())
    cfg.setAppProperty("AuditAlgorithms", True)

    StoreGateSvc=CompFactory.StoreGateSvc
    cfg.addService(StoreGateSvc())
    cfg.addService(StoreGateSvc("DetectorStore"))
    cfg.addService(StoreGateSvc("HistoryStore"))
    cfg.addService(StoreGateSvc("ConditionStore"))
    from AthenaConfiguration.FPEAndCoreDumpConfig import FPEAndCoreDumpCfg
    cfg.merge(FPEAndCoreDumpCfg(flags))

    cfg.setAppProperty('InitializationLoopCheck',False)

    cfg.setAppProperty('EvtMax', flags.Exec.MaxEvents)

    cfg.merge(MessageSvcCfg(flags))

    if flags.Exec.DebugStage != "":
        cfg.setDebugStage(flags.Exec.DebugStage)

    if flags.Concurrency.NumProcs>0:
        from AthenaMP.AthenaMPConfig import AthenaMPCfg
        mploop = AthenaMPCfg(flags)
        cfg.merge(mploop)

    ########################################################################
    # Additional components needed for threaded jobs only
    if flags.Concurrency.NumThreads>0:
        cfg.merge(AthenaHiveEventLoopMgrCfg(flags))
        # Setup SGCommitAuditor to sweep new DataObjects at end of Alg execute
        cfg.addAuditor( CompFactory.SGCommitAuditor() )
    elif LoopMgr == 'AthenaEventLoopMgr':
        cfg.merge(AthenaEventLoopMgrCfg(flags))

    return cfg


def MainEvgenServicesCfg(flags, LoopMgr='AthenaEventLoopMgr',seqName="AthAlgSeq"):
    """ComponentAccumulator-based equivalent of:
    import AthenaCommon.AtlasUnixGeneratorJob

    NB Must have set ConfigFlags.Input.RunNumber and
    ConfigFlags.Input.TimeStamp before calling to avoid
    attempted auto-configuration from an input file.
    """
    cfg = MainServicesCfg(flags, LoopMgr)
    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge (McEventSelectorCfg (flags))
    # Temporarily inject the xAOD::EventInfo converter here to allow for adiabatic migration of the clients
    cfg.addEventAlgo(CompFactory.xAODMaker.EventInfoCnvAlg(AODKey = 'McEventInfo'),sequenceName=seqName)

    return cfg

if __name__=="__main__":
    from AthenaConfiguration.AllConfigFlags import initConfigFlags
    flags = initConfigFlags()
    try:
        flags.Input.RunNumber = 284500 # Set to either MC DSID or MC Run Number
        flags.Input.TimeStamp = 1 # dummy value
        cfg = MainEvgenServicesCfg(flags)
    except ModuleNotFoundError:
        #  The McEventSelector package required by MainEvgenServicesCfg is not part of the AthAnalysis project
        cfg = MainServicesCfg(flags)
    cfg._wasMerged = True   # to avoid errror that CA was not merged
