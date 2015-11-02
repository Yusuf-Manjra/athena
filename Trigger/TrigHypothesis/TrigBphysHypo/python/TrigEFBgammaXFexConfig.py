# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFBgammaXFex


# basic cut
class EFBgammaXFex_1 (TrigEFBgammaXFex):
    __slots__ = []
    def __init__(self, name = "EFBgammaXFex_1"):
        super( TrigEFBgammaXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 BgammaX cuts

#        from TrigBphysHypo.TrigEFBgammaXFexMonitoring import TrigEFBgammaXFexValidationMonitoring_RoI
#        validation = TrigEFBgammaXFexValidationMonitoring_RoI()
        
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")
                
#        self.AthenaMonTools = [ validation, time ]

# full scan
class EFBgammaXFex_FS (TrigEFBgammaXFex):
    __slots__ = []
    def __init__(self, name = "EFBgammaXFex_FS"):
        super( TrigEFBgammaXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 BgammaX cuts

#        from TrigBphysHypo.TrigEFBgammaXFexMonitoring import TrigEFBgammaXFexValidationMonitoring_FS
#        validation = TrigEFBgammaXFexValidationMonitoring_FS()
        
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")
                
#        self.AthenaMonTools = [ validation, time ]
