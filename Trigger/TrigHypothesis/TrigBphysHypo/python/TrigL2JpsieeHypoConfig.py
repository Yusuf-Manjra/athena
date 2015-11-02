# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2JpsieeHypo

####
#### !!!!!!!!!!!!!!!!!!!!!!!!!!
# we have to do something with this, where was this defined before?
#from AthenaCommon.SystemOfUnits import GeV

# basic cut
class L2JpsieeHypo_1 (TrigL2JpsieeHypo):
    __slots__ = []
    def __init__(self, name = "L2JpsieeHypo_1"):
        super( TrigL2JpsieeHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Jpsiee cuts
        self.JpsiMasslow_cut      = 1500.
        self.JpsiMasshigh_cut     = 3800.


        from TrigBphysHypo.TrigL2JpsieeHypoMonitoring import TrigL2JpsieeHypoValidationMonitoring
        validation = TrigL2JpsieeHypoValidationMonitoring()
        
        from TrigBphysHypo.TrigL2JpsieeHypoMonitoring import TrigL2JpsieeHypoOnlineMonitoring
        online = TrigL2JpsieeHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


# full scan
class L2JpsieeHypo_FS (TrigL2JpsieeHypo):
    __slots__ = []
    def __init__(self, name = "L2JpsieeHypo_FS"):
        super( TrigL2JpsieeHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Jpsiee cuts
        self.JpsiMasslow_cut      = 1500.
        self.JpsiMasshigh_cut     = 3800.


        from TrigBphysHypo.TrigL2JpsieeHypoMonitoring import TrigL2JpsieeHypoValidationMonitoring
        validation = TrigL2JpsieeHypoValidationMonitoring()
        
        from TrigBphysHypo.TrigL2JpsieeHypoMonitoring import TrigL2JpsieeHypoOnlineMonitoring
        online = TrigL2JpsieeHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]
