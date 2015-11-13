# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFBMuMuHypo

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigEFBMuMuHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFBMuMuHypoValidationMonitoring"):
        super(TrigEFBMuMuHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
#        self.defineTarget("Never")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title= "EF cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.0) ]

class TrigEFBMuMuHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFBMuMuHypoOnlineMonitoring"):
        super(TrigEFBMuMuHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title= "EFBMuMu cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="EFBMuMu Inv.mass(mumu) [GeV]",
                                           xbins=130, xmin=0., xmax=13.0) ]

class EFBMuMuHypo_1 (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_1"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 500.
        self.UpperMassCut      = 13000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 1500.
        self.UpperMassCut      = 14000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 10000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY25 (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY25"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 25000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY20 (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY20"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 20000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY14 (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY14"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 14000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_Jpsi (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_Jpsi"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 2500.
        self.UpperMassCut      = 4300.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_Upsi (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_Upsi"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 8000.
        self.UpperMassCut      = 12000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_B (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_B"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 4000.
        self.UpperMassCut      = 8500.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_B_Lxy (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_B_Lxy"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 4000.
        self.UpperMassCut      = 8500.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.
        self.ApplyLxyCut = True
        self.LxyCut        = 0.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_Z (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_Z"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 60000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_noVtx (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_noVtx"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 1500.
        self.UpperMassCut      = 14000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY_noVtx (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY_noVtx"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 10000.
        self.UpperMassCut      = 140000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY14_noVtx (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY14_noVtx"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 14000.
        self.UpperMassCut      = 140000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_7invm9_noVtx (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_7invm9_noVtx"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 7000.
        self.UpperMassCut      = 9000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_7invm9 (EFBMuMuHypo_DiMu_7invm9_noVtx):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_7invm9"):
        super( EFBMuMuHypo_DiMu_7invm9, self ).__init__( name )
        
        # EF Bmumu cuts
        self.ApplyChi2Cut      = True
        
        
class EFBMuMuHypo_DiMu_11invm60_noVtx (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_11invm60_noVtx"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 11000.
        self.UpperMassCut      = 60000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]
        
class EFBMuMuHypo_DiMu_11invm60 (EFBMuMuHypo_DiMu_11invm60_noVtx):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_11invm60"):
        super( EFBMuMuHypo_DiMu_11invm60, self ).__init__( name )
        
        # EF Bmumu cuts
        self.ApplyChi2Cut      = True
        
        
class EFBMuMuHypo_DiMu_11invm24_noVtx (EFBMuMuHypo_DiMu_11invm60_noVtx):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_11invm24_noVtx"):
        super( EFBMuMuHypo_DiMu_11invm24_noVtx, self ).__init__( name )
        
        # EF Bmumu cuts
        self.LowerMassCut      = 11000.
        self.UpperMassCut      = 24000.
        
class EFBMuMuHypo_DiMu_11invm24 (EFBMuMuHypo_DiMu_11invm24_noVtx):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_11invm24"):
        super( EFBMuMuHypo_DiMu_11invm24, self ).__init__( name )
        
        # EF Bmumu cuts
        self.ApplyChi2Cut      = True
        
        
class EFBMuMuHypo_DiMu_24invm60_noVtx (EFBMuMuHypo_DiMu_11invm60_noVtx):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_24invm60_noVtx"):
        super( EFBMuMuHypo_DiMu_24invm60_noVtx, self ).__init__( name )
        
        # EF Bmumu cuts
        self.LowerMassCut      = 24000.
        self.UpperMassCut      = 60000.
        
class EFBMuMuHypo_DiMu_24invm60 (EFBMuMuHypo_DiMu_24invm60_noVtx):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_24invm60"):
        super( EFBMuMuHypo_DiMu_24invm60, self ).__init__( name )
        
        # EF Bmumu cuts
        self.ApplyChi2Cut      = True
        
        
class EFBMuMuHypo_DiMu_18invm60_noVtx (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_18invm60_noVtx"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 18000.
        self.UpperMassCut      = 60000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_noinvm_noVtx (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_noinvm_noVtx"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 0.
        self.UpperMassCut      = 14000.
        self.ApplyUpperMassCut = False
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_passEF (TrigEFBMuMuHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_passEF"):
        super( TrigEFBMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # EF Bmumu cuts
        self.LowerMassCut      = 500.
        self.UpperMassCut      = 13000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]