# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

def _setupAtlasThreadedEsJob():
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon import Constants

    from AthenaCommon.ConcurrencyFlags import jobproperties as jps

    if (jps.ConcurrencyFlags.NumProcs() == 0) :
        theApp.MessageSvcType = "InertMessageSvc"
    else:
        # InertMessageSvc doesn't play nice with MP
        theApp.MessageSvcType = "MessageSvc"

    svcMgr.MessageSvc.defaultLimit = 0
    msgFmt = "% F%40W%S%4W%e%s%7W%R%T %0W%M"
    svcMgr.MessageSvc.Format = msgFmt

    theApp.StatusCodeCheck = False

    svcMgr.StatusCodeSvc.AbortOnError = False

    numStores = jps.ConcurrencyFlags.NumConcurrentEvents()

    from StoreGate.StoreGateConf import SG__HiveMgrSvc
    svcMgr += SG__HiveMgrSvc("EventDataSvc")
    svcMgr.EventDataSvc.NSlots = numStores


    from GaudiHive.GaudiHiveConf import AlgResourcePool
    arp=AlgResourcePool( OutputLevel = Constants.INFO )
    arp.TopAlg=["AthMasterSeq"] #this should enable control flow
    svcMgr += arp

    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.ShowDataDependencies(False)
    AlgScheduler.ShowControlFlow(False)

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    from SGComps.SGCompsConf import SGInputLoader
    # FailIfNoProxy=False makes it a warning, not an error, if unmet data
    # dependencies are not found in the store.  It should probably be changed
    # to True eventually.
    topSequence += SGInputLoader (FailIfNoProxy = False)
    AlgScheduler.setDataLoaderAlg ('SGInputLoader' )

    from AthenaServices.AthenaServicesConf import AthenaMtesEventLoopMgr

    svcMgr += AthenaMtesEventLoopMgr()
    svcMgr.AthenaMtesEventLoopMgr.WhiteboardSvc = "EventDataSvc"
    svcMgr.AthenaMtesEventLoopMgr.SchedulerSvc = AlgScheduler.getScheduler().getName()

    theApp.EventLoop = "AthenaMtesEventLoopMgr"

    # enable timeline recording
    from GaudiHive.GaudiHiveConf import TimelineSvc
    svcMgr += TimelineSvc( RecordTimeline = True, Partial = False )
    
    #
    ## Setup SGCommitAuditor to sweep new DataObjects at end of Alg execute
    #
    
    theAuditorSvc = svcMgr.AuditorSvc
    theApp.AuditAlgorithms=True 
    from SGComps.SGCompsConf import SGCommitAuditor
    theAuditorSvc += SGCommitAuditor()
    
    
## load basic services configuration at module import
_setupAtlasThreadedEsJob()

## clean-up: avoid running multiple times this method
del _setupAtlasThreadedEsJob
