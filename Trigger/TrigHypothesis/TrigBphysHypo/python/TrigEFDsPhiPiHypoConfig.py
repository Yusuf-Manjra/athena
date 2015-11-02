# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFDsPhiPiHypo

####
#### !!!!!!!!!!!!!!!!!!!!!!!!!!
# we have to do something with this, where was this defined before?
#from AthenaCommon.SystemOfUnits import GeV

# basic cut
class EFDsPhiPiHypo_1 (TrigEFDsPhiPiHypo):
    __slots__ = []
    def __init__(self, name = "EFDsPhiPiHypo_1"):
        super( TrigEFDsPhiPiHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 DsPhiPi cuts
        self.EtaBins             = [0.,1.0,1.5]
        self.PhiMasslow_cut      = [1005.,1003.,1002.]
        self.PhiMasshigh_cut     = [1035.,1037.,1037.]
        self.DsMasslow_cut       = [1887.,1870.,1863.]
        self.DsMasshigh_cut      = [2047.,2062.,2071.]
        self.useVertexFit        = False

        from TrigBphysHypo.TrigEFDsPhiPiHypoMonitoring import TrigEFDsPhiPiHypoValidationMonitoring
        validation = TrigEFDsPhiPiHypoValidationMonitoring()
        
        from TrigBphysHypo.TrigEFDsPhiPiHypoMonitoring import TrigEFDsPhiPiHypoOnlineMonitoring
        online = TrigEFDsPhiPiHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")


# basic cut
class EFDsPhiPiHypo_FS (TrigEFDsPhiPiHypo):
    __slots__ = []
    def __init__(self, name = "EFDsPhiPiHypo_FS"):
        super( TrigEFDsPhiPiHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 DsPhiPi cuts
        self.EtaBins             = [0.,1.0,1.5]
        self.PhiMasslow_cut      = [1005.,1003.,1002.]
        self.PhiMasshigh_cut     = [1035.,1037.,1037.]
        self.DsMasslow_cut       = [1887.,1870.,1863.]
        self.DsMasshigh_cut      = [2047.,2062.,2071.]
        self.useVertexFit        = False

        from TrigBphysHypo.TrigEFDsPhiPiHypoMonitoring import TrigEFDsPhiPiHypoValidationMonitoring
        validation = TrigEFDsPhiPiHypoValidationMonitoring()
        
        from TrigBphysHypo.TrigEFDsPhiPiHypoMonitoring import TrigEFDsPhiPiHypoOnlineMonitoring
        online = TrigEFDsPhiPiHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

