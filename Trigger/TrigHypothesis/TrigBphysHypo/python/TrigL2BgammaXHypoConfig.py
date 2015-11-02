# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2BgammaXHypo


# basic cut
class L2BgammaXHypo_1 (TrigL2BgammaXHypo):
    __slots__ = []
    def __init__(self, name = "L2BgammaXHypo_1"):
        super( TrigL2BgammaXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 BgammaX cuts

#        from TrigBphysHypo.TrigL2BgammaXHypoMonitoring import TrigL2BgammaXHypoValidationMonitoring
#        validation = TrigL2BgammaXHypoValidationMonitoring()
        
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")
                
#        self.AthenaMonTools = [ validation, time ]


# full scan
class L2BgammaXHypo_FS (TrigL2BgammaXHypo):
    __slots__ = []
    def __init__(self, name = "L2BgammaXHypo_FS"):
        super( TrigL2BgammaXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 BgammaX cuts
 
 #       from TrigBphysHypo.TrigL2BgammaXHypoMonitoring import TrigL2BgammaXHypoValidationMonitoring
 #       validation = TrigL2BgammaXHypoValidationMonitoring()
        
 #       from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
 #       time = TrigTimeHistToolConfig("Time")
                
 #       self.AthenaMonTools = [ validation, time ]
