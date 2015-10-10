# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import MuonCluster
from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import TrigMuonJetFex
from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import TrigJetSplitter

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr


def getJetSplitterInstance( ):
    return JetSplitter( name="JetSplitter" )


class JetSplitter (TrigJetSplitter):
    __slots__ = []
    
    def __init__(self, name):
        super( JetSplitter, self ).__init__( name )
        
        self.JetInputKey  = "TrigJetRec"
        self.JetOutputKey = "SplitJet"
        self.EtaHalfWidth = 0.4
        self.PhiHalfWidth = 0.4
        self.JetLogRatio     = 1.2


class MuonClusterConfig(MuonCluster): 
   __slots__ = []
   def __init__ (self, name="MuonClusterConfig"): 
        super(MuonClusterConfig, self).__init__(name)

        from TrigL2LongLivedParticles.TrigL2LongLivedParticlesMonitoring import TrigMuonClusterValidationMonitoring, TrigMuonClusterOnlineMonitoring, TrigMuonClusterCosmicMonitoring
        validation = TrigMuonClusterValidationMonitoring()
        online     = TrigMuonClusterOnlineMonitoring()
        cosmic     = TrigMuonClusterCosmicMonitoring()

 
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("muClu_Time")
 
        self.AthenaMonTools = [ validation, online, time, cosmic]
 
        # muClu Parameters
        self.DeltaR      = 0.4
        self.DeltaRJet   = 0.7
        self.DeltaRTrk   = 0.4
       

class TrigL2MuonJetFexGlobal (TrigMuonJetFex):
    __slots__ = []
    def __init__(self, name):
        super( TrigL2MuonJetFexGlobal, self ).__init__( name )
        
        self.Instance                    = "L2"


class TrigEFMuonJetFexGlobal (TrigMuonJetFex):
    __slots__ = []
    def __init__(self, name):
        super( TrigEFMuonJetFexGlobal, self ).__init__( name )

        self.Instance       = "EF"


class L2MuonJetFex (TrigL2MuonJetFexGlobal):
    __slots__ = []
    def __init__(self, name = "L2MuonJetFex"):
        super( L2MuonJetFex, self ).__init__( name )


class EFMuonJetFex (TrigEFMuonJetFexGlobal):
    __slots__ = []
    def __init__(self, name = "EFMuonJetFex"):
        super( EFMuonJetFex, self ).__init__( name )

                                                               
