# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFBgammaXHypo


# basic cut
class EFBgammaXHypo_1 (TrigEFBgammaXHypo):
    __slots__ = []
    def __init__(self, name = "EFBgammaXHypo_1"):
        super( TrigEFBgammaXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 BgammaX cuts

#        from TrigBphysHypo.TrigEFBgammaXHypoMonitoring import TrigEFBgammaXHypoValidationMonitoring
#        validation = TrigEFBgammaXHypoValidationMonitoring()
        
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")


# basic cut
class EFBgammaXHypo_FS (TrigEFBgammaXHypo):
    __slots__ = []
    def __init__(self, name = "EFBgammaXHypo_FS"):
        super( TrigEFBgammaXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 BgammaX cuts
 
 #       from TrigBphysHypo.TrigEFBgammaXHypoMonitoring import TrigEFBgammaXHypoValidationMonitoring
 #       validation = TrigEFBgammaXHypoValidationMonitoring()
        
 #       from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
 #       time = TrigTimeHistToolConfig("Time")

