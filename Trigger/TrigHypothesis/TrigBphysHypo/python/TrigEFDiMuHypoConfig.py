# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFDiMuHypo

# Jpsi
class TrigEFDiMuHypo_Jpsi (TrigEFDiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigEFDiMuHypo_Jpsi"):
        super( TrigEFDiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.MuMuMassMin = 2500.
        self.MuMuMassMax = 4300.
        self.ApplyMuMuMassMax = True

        ## added 2008-01-27
        from TrigBphysHypo.TrigEFDiMuHypoMonitoring import TrigEFDiMuHypoValidationMonitoring_Jpsi
        validation = TrigEFDiMuHypoValidationMonitoring_Jpsi()
        from TrigBphysHypo.TrigEFDiMuHypoMonitoring import TrigEFDiMuHypoOnlineMonitoring_Jpsi
        online = TrigEFDiMuHypoOnlineMonitoring_Jpsi()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]
        ## end of adding
# Upsilon
class TrigEFDiMuHypo_Upsi (TrigEFDiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigEFDiMuHypo_Upsi"):
        super( TrigEFDiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.MuMuMassMin = 8000.
        self.MuMuMassMax = 12000.
        self.ApplyMuMuMassMax = True

       ## added 2008-04-07
        from TrigBphysHypo.TrigEFDiMuHypoMonitoring import TrigEFDiMuHypoValidationMonitoring_Upsi
        validation = TrigEFDiMuHypoValidationMonitoring_Upsi()
        from TrigBphysHypo.TrigEFDiMuHypoMonitoring import TrigEFDiMuHypoOnlineMonitoring_Upsi
        online = TrigEFDiMuHypoOnlineMonitoring_Upsi()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]
        ## end of adding

# Bmumu
class TrigEFDiMuHypo_B (TrigEFDiMuHypo):   
    __slots__ = []   
    def __init__(self, name = "TrigEFDiMuHypo_B"):   
        super( TrigEFDiMuHypo, self ).__init__( name )   
     
        # AcceptAll flag: if true take events regardless of cuts   
        self.AcceptAll = False   
        # mass cuts   
        self.MuMuMassMin = 4000.   
        self.MuMuMassMax = 8500. 
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigEFDiMuHypoMonitoring import TrigEFDiMuHypoValidationMonitoring_B
        validation = TrigEFDiMuHypoValidationMonitoring_B()
        from TrigBphysHypo.TrigEFDiMuHypoMonitoring import TrigEFDiMuHypoOnlineMonitoring_B
        online = TrigEFDiMuHypoOnlineMonitoring_B()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]
        ## end of adding

# generic di-muon
class TrigEFDiMuHypo_DiMu (TrigEFDiMuHypo):   
    __slots__ = []   
    def __init__(self, name = "TrigEFDiMuHypo_DiMu"):   
        super( TrigEFDiMuHypo, self ).__init__( name )   
     
        # AcceptAll flag: if true take events regardless of cuts   
        self.AcceptAll = False   
        # mass cuts   
        self.MuMuMassMin = 1500.   
        self.MuMuMassMax = 14000. 
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigEFDiMuHypoMonitoring import TrigEFDiMuHypoValidationMonitoring_DiMu
        validation = TrigEFDiMuHypoValidationMonitoring_DiMu()
        from TrigBphysHypo.TrigEFDiMuHypoMonitoring import TrigEFDiMuHypoOnlineMonitoring_DiMu
        online = TrigEFDiMuHypoOnlineMonitoring_DiMu()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]
        ## end of adding


# generic di-muon no Opp charge
class TrigEFDiMuHypo_DiMu_noOS (TrigEFDiMuHypo):   
    __slots__ = []   
    def __init__(self, name = "TrigEFDiMuHypo_DiMu_noOS"):   
        super( TrigEFDiMuHypo, self ).__init__( name )   
     
        # AcceptAll flag: if true take events regardless of cuts   
        self.AcceptAll = False   
        # mass cuts   
        self.MuMuMassMin = 1500.   
        self.MuMuMassMax = 14000. 
        self.ApplyOppositeCharge = False
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigEFDiMuHypoMonitoring import TrigEFDiMuHypoValidationMonitoring_DiMu
        validation = TrigEFDiMuHypoValidationMonitoring_DiMu()
        from TrigBphysHypo.TrigEFDiMuHypoMonitoring import TrigEFDiMuHypoOnlineMonitoring_DiMu
        online = TrigEFDiMuHypoOnlineMonitoring_DiMu()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]
        ## end of adding
