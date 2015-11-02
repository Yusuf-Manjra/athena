# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigEFDiMuHypoValidationMonitoring_Jpsi(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDiMuHypoValidationMonitoring_Jpsi"):
        super(TrigEFDiMuHypoValidationMonitoring_Jpsi, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="Jpsi mass [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="Jpsi fitted mass [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
        self.Histograms += [ defineHistogram('MassCut', type='TH1F', title="Jpsi mass with cut [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
#        self.Histograms += [ defineHistogram('E', type='TH1F', title="Jpsi E", xbins=100, xmin=2000, xmax=150000) ]
#        self.Histograms += [ defineHistogram('ECut', type='TH1F', title="Jpsi E", xbins=100, xmin=2000, xmax=150000) ]


class TrigEFDiMuHypoOnlineMonitoring_Jpsi(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDiMuHypoOnlineMonitoring_Jpsi"):
        super(TrigEFDiMuHypoOnlineMonitoring_Jpsi, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="Jpsi mass [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="Jpsi fitted mass [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
        self.Histograms += [ defineHistogram('MassCut', type='TH1F', title="Jpsi mass with cut [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
#        self.Histograms += [ defineHistogram('E', type='TH1F', title="Jpsi E", xbins=100, xmin=2000, xmax=150000) ]
#        self.Histograms += [ defineHistogram('ECut', type='TH1F', title="Jpsi E", xbins=100, xmin=2000, xmax=150000) ]


class TrigEFDiMuHypoValidationMonitoring_Upsi(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDiMuHypoValidationMonitoring_Upsi"):
        super(TrigEFDiMuHypoValidationMonitoring_Upsi, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="Upsi mass [GeV]", xbins=30, xmin=8.5, xmax=10.0) ]
        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="Upsi fitted mass [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
        self.Histograms += [ defineHistogram('MassCut', type='TH1F', title="Upsi mass with cut [GeV]", xbins=30, xmin=8.5, xmax=10.0) ]


class TrigEFDiMuHypoOnlineMonitoring_Upsi(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDiMuHypoOnlineMonitoring_Upsi"):
        super(TrigEFDiMuHypoOnlineMonitoring_Upsi, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="Upsi mass [GeV]", xbins=30, xmin=8.5, xmax=10.0) ]
        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="Upsi fitted mass [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
        self.Histograms += [ defineHistogram('MassCut', type='TH1F', title="Upsi mass with cut [GeV]", xbins=30, xmin=8.5, xmax=10.0) ]


class TrigEFDiMuHypoValidationMonitoring_B(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDiMuHypoValidationMonitoring_B"):
        super(TrigEFDiMuHypoValidationMonitoring_B, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="B mass [GeV]", xbins=60, xmin=4.0, xmax=7.0) ]
        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="B fitted mass [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
        self.Histograms += [ defineHistogram('MassCut', type='TH1F', title="B mass with cut [GeV]", xbins=60, xmin=4.0, xmax=7.0) ]


class TrigEFDiMuHypoOnlineMonitoring_B(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDiMuHypoOnlineMonitoring_B"):
        super(TrigEFDiMuHypoOnlineMonitoring_B, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="B mass [GeV]", xbins=60, xmin=4.0, xmax=7.0) ]
        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="B fitted mass [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
        self.Histograms += [ defineHistogram('MassCut', type='TH1F', title="B mass with cut [GeV]", xbins=60, xmin=4.0, xmax=7.0) ]


class TrigEFDiMuHypoValidationMonitoring_DiMu(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDiMuHypoValidationMonitoring_DiMu"):
        super(TrigEFDiMuHypoValidationMonitoring_DiMu, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="DiMu mass [GeV]", xbins=130, xmin=0, xmax=13.0) ]
        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="DiMu fitted mass [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
        self.Histograms += [ defineHistogram('MassCut', type='TH1F', title="DiMu mass with cut [GeV]", xbins=130, xmin=0, xmax=13.0) ]


class TrigEFDiMuHypoOnlineMonitoring_DiMu(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDiMuHypoOnlineMonitoring_DiMu"):
        super(TrigEFDiMuHypoOnlineMonitoring_DiMu, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="DiMu mass [GeV]", xbins=130, xmin=0, xmax=13.0) ]
        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="DiMu fitted mass [GeV]", xbins=26, xmin=2.7, xmax=4.0) ]
        self.Histograms += [ defineHistogram('MassCut', type='TH1F', title="DiMu mass with cut [GeV]", xbins=130, xmin=0, xmax=13.0) ]


