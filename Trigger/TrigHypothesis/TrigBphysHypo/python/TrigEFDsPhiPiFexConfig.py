# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFDsPhiPiFex

####
#### !!!!!!!!!!!!!!!!!!!!!!!!!!
# we have to do something with this, where was this defined before?
#from AthenaCommon.SystemOfUnits import GeV

# basic cut
class EFDsPhiPiFex_1 (TrigEFDsPhiPiFex):
    __slots__ = []
    def __init__(self, name = "EFDsPhiPiFex_1"):
        super( TrigEFDsPhiPiFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 DsPhiPi cuts
        self.TrackPTthr          = 1400
        self.dEtaTrackPair       = 0.2
        self.dPhiTrackPair       = 0.2
        self.dz0TrackPair        = 3.
        self.pTsumTrackPair      = 0.
        self.EtaBins             = [0.,1.0,1.5]
        self.PhiMasslow_cut      = [990.,990.,990.]
        self.PhiMasshigh_cut     = [1050.,1050.,1050.]
        self.DsMasslow_cut       = [1863.,1863.,1863.]
        self.DsMasshigh_cut      = [2071.,2071.,2071.]

        from TrigBphysHypo.TrigEFDsPhiPiFexMonitoring import TrigEFDsPhiPiFexValidationMonitoring_RoI
        validation = TrigEFDsPhiPiFexValidationMonitoring_RoI()
        
        from TrigBphysHypo.TrigEFDsPhiPiFexMonitoring import TrigEFDsPhiPiFexOnlineMonitoring_RoI
        online = TrigEFDsPhiPiFexOnlineMonitoring_RoI()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]

# full scan
class EFDsPhiPiFex_FS (TrigEFDsPhiPiFex):
    __slots__ = []
    def __init__(self, name = "EFDsPhiPiFex_FS"):
        super( TrigEFDsPhiPiFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 DsPhiPi cuts
        self.TrackPTthr          = 1400
        self.dEtaTrackPair       = 0.2
        self.dPhiTrackPair       = 0.2
        self.dz0TrackPair        = 3.
        self.pTsumTrackPair      = 0.
        self.EtaBins             = [0.,1.0,1.5]
        self.PhiMasslow_cut      = [990.,990.,990.]
        self.PhiMasshigh_cut     = [1050.,1050.,1050.]
        self.DsMasslow_cut       = [1863.,1863.,1863.]
        self.DsMasshigh_cut      = [2071.,2071.,2071.]

        from TrigBphysHypo.TrigEFDsPhiPiFexMonitoring import TrigEFDsPhiPiFexValidationMonitoring_FS
        validation = TrigEFDsPhiPiFexValidationMonitoring_FS()
        
        from TrigBphysHypo.TrigEFDsPhiPiFexMonitoring import TrigEFDsPhiPiFexOnlineMonitoring_FS
        online = TrigEFDsPhiPiFexOnlineMonitoring_FS()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]
