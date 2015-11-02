# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigEFDsPhiPiFexValidationMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFFexValidation_RoI"):
        super(TrigEFDsPhiPiFexValidationMonitoring_RoI, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('Ntrack', type='TH1F', title="EFDsPhipiFex Ntrack",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('trackPt', type='TH1F', title="EFDsPhipiFex trackPt; P_{t} [GeV]",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="EFDsPhipiFex track deta ",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="EFDsPhipiFex track dphi",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dz0', type='TH1F', title="EFDsPhipiFex track dz0",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('Ptsum', type='TH1F', title="EFDsPhipiFex track pTsum",
                                           xbins=50, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('PhiMass', type='TH1F', title="EFDsPhipiFex PhiMass; PhiMass[MeV]",
                                           xbins=100, xmin=970., xmax=1070.) ]
#        self.Histograms += [ defineHistogram('PhiFitMass', type='TH1F', title="EFDsPhipiFex FitPhiMass; FitPhiMass [MeV]",
#                                           xbins=100, xmin=970., xmax=1070.) ]
        self.Histograms += [ defineHistogram('DsMass', type='TH1F', title="EFDsPhipiFex DsMass; DsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
#        self.Histograms += [ defineHistogram('DsFitMass', type='TH1F', title="EFDsPhipiFex FitDsMass; FitDsMass [MeV]",
#                                           xbins=100, xmin=1750., xmax=2200.) ]

class TrigEFDsPhiPiFexOnlineMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFFexOnline_RoI"):
        super(TrigEFDsPhiPiFexOnlineMonitoring_RoI, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('Ntrack', type='TH1F', title="EFDsPhipiFex Ntrack",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('trackPt', type='TH1F', title="EFDsPhipiFex trackPt; P_{t} [GeV]",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="EFDsPhipiFex track deta ",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="EFDsPhipiFex track dphi",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dz0', type='TH1F', title="EFDsPhipiFex track dz0",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('Ptsum', type='TH1F', title="EFDsPhipiFex track pTsum",
                                           xbins=50, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('PhiMass', type='TH1F', title="EFDsPhipiFex PhiMass; PhiMass[MeV]",
                                           xbins=100, xmin=970., xmax=1070.) ]
#        self.Histograms += [ defineHistogram('PhiFitMass', type='TH1F', title="EFDsPhipiFex FitPhiMass; FitPhiMass [MeV]",
#                                           xbins=100, xmin=970., xmax=1070.) ]
        self.Histograms += [ defineHistogram('DsMass', type='TH1F', title="EFDsPhipiFex DsMass; DsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
#        self.Histograms += [ defineHistogram('DsFitMass', type='TH1F', title="EFDsPhipiFex FitDsMass; FitDsMass [MeV]",
#                                           xbins=100, xmin=1750., xmax=2200.) ]


class TrigEFDsPhiPiFexValidationMonitoring_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFFexValidation_FS"):
        super(TrigEFDsPhiPiFexValidationMonitoring_FS, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('Ntrack', type='TH1F', title="EFDsPhipiFex Ntrack",
                                           xbins=100, xmin=0.0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('trackPt', type='TH1F', title="EFDsPhipiFex trackPt; P_{t} [GeV]",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="EFDsPhipiFex track deta ",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="EFDsPhipiFex track dphi",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dz0', type='TH1F', title="EFDsPhipiFex track dz0",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('Ptsum', type='TH1F', title="EFDsPhipiFex track pTsum",
                                           xbins=50, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('PhiMass', type='TH1F', title="EFDsPhipiFex PhiMass; PhiMass[MeV]",
                                           xbins=100, xmin=970., xmax=1070.) ]
#        self.Histograms += [ defineHistogram('PhiFitMass', type='TH1F', title="EFDsPhipiFex FitPhiMass; FitPhiMass [MeV]",
#                                           xbins=100, xmin=970., xmax=1070.) ]
        self.Histograms += [ defineHistogram('DsMass', type='TH1F', title="EFDsPhipiFex DsMass; DsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
#        self.Histograms += [ defineHistogram('DsFitMass', type='TH1F', title="EFDsPhipiFex FitDsMass; FitDsMass [MeV]",
#                                           xbins=100, xmin=1750., xmax=2200.) ]

class TrigEFDsPhiPiFexOnlineMonitoring_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFFexOnline_FS"):
        super(TrigEFDsPhiPiFexOnlineMonitoring_FS, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('Ntrack', type='TH1F', title="EFDsPhipiFex Ntrack",
                                           xbins=100, xmin=0.0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('trackPt', type='TH1F', title="EFDsPhipiFex trackPt; P_{t} [GeV]",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="EFDsPhipiFex track deta ",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="EFDsPhipiFex track dphi",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dz0', type='TH1F', title="EFDsPhipiFex track dz0",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('Ptsum', type='TH1F', title="EFDsPhipiFex track pTsum",
                                           xbins=50, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('PhiMass', type='TH1F', title="EFDsPhipiFex PhiMass; PhiMass[MeV]",
                                           xbins=100, xmin=970., xmax=1070.) ]
#        self.Histograms += [ defineHistogram('PhiFitMass', type='TH1F', title="EFDsPhipiFex FitPhiMass; FitPhiMass [MeV]",
#                                           xbins=100, xmin=970., xmax=1070.) ]
        self.Histograms += [ defineHistogram('DsMass', type='TH1F', title="EFDsPhipiFex DsMass; DsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
#        self.Histograms += [ defineHistogram('DsFitMass', type='TH1F', title="EFDsPhipiFex FitDsMass; FitDsMass [MeV]",
#                                           xbins=100, xmin=1750., xmax=2200.) ]

