# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2LooseMuonHypo

# Jpsi
class TrigL2LooseMuonHypo_1 (TrigL2LooseMuonHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2LooseMuonHypo_1"):
        super( TrigL2LooseMuonHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time ]

