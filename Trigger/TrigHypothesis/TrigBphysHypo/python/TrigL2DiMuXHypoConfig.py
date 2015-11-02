# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2DiMuXHypo

class TrigL2DiMuXHypo_1 (TrigL2DiMuXHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuXHypo_1"):
        super( TrigL2DiMuXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.LowerBplusMassCut = 5100.
        self.UpperBplusMassCut = 5500.

        from TrigBphysHypo.TrigL2DiMuXHypoMonitoring import TrigL2DiMuXHypoValidationMonitoring_RoI
        validation = TrigL2DiMuXHypoValidationMonitoring_RoI()
        
        from TrigBphysHypo.TrigL2DiMuXHypoMonitoring import TrigL2DiMuXHypoOnlineMonitoring_RoI
        online = TrigL2DiMuXHypoOnlineMonitoring_RoI()
        
        self.AthenaMonTools = [ validation, online ]
