# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

class TrigMultiTrkComboHypoMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigMultiTrkComboHypoMonitoring, self).__init__(name)
        if 'L2' in name:
            self.Histograms = [
            defineHistogram('nTrk', type='TH1F', path='EXPERT', title="number of tracks in input views", xbins=100, xmin=0, xmax=100),
            defineHistogram('nAcceptedTrk', type='TH1F', path='EXPERT', title="number of selected input tracks", xbins=100, xmin=0, xmax=100),
            defineHistogram('acceptance', type='TH1F',path='EXPERT', title="filter acceptance", xbins=2, xmin=0, xmax=2),
            defineHistogram('TIME_all', type='TH1F', path='EXPERT', title='execution time; [microseconds]', xbins=100, xmin=0, xmax=1000),
            ]
        elif 'EF' in name:
            self.Histograms = [
            defineHistogram('nTrk', type='TH1F', path='EXPERT', title="number of tracks in input views", xbins=100, xmin=0, xmax=100),
            defineHistogram('nAcceptedTrk', type='TH1F', path='EXPERT', title="number of selected input tracks", xbins=100, xmin=0, xmax=100),
            defineHistogram('nCombination', type='TH1F',path='EXPERT', title="number of track combinations before mass preselection", xbins=100, xmin=0, xmax=100),
            defineHistogram('nCombinationBeforeFit', type='TH1F', path='EXPERT', title="number of inputs to the vertex fitter", xbins=100, xmin=0, xmax=100),
            defineHistogram('nBPhysObject', type='TH1F', path='EXPERT', title="number of fitted BPhysObjects", xbins=100, xmin=0, xmax=100),
            defineHistogram('trkMassBeforeFit', type='TH1F', path='EXPERT', title="mass of track combinations BEFORE fit [GeV]", xbins=200, xmin=0, xmax=100),
            defineHistogram('bphysChi2', type='TH1F', path='EXPERT', title="chi2 fit of N tracks; fit chi2 of N selected tracks", xbins=100, xmin=0, xmax=100),
            defineHistogram('bphysFitMass', type='TH1F', path='EXPERT', title="fit mass of N tracks; fit mass of N selected tracks [GeV]", xbins=100, xmin=0, xmax=20),
            defineHistogram('bphysMass', type='TH1F', path='EXPERT', title="mass of N tracks; mass of N selected tracks [GeV]", xbins=100, xmin=0, xmax=20),
            defineHistogram('bphysCharge', type='TH1F', path='EXPERT', title="total charge of N tracks; Total Charge", xbins=20, xmin=-10, xmax=10),
            defineHistogram('TIME_all', type='TH1F', path='EXPERT', title='execution time; [microseconds]', xbins=100, xmin=0, xmax=1000),
            ]

class TrigMultiTrkComboHypoToolMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigMultiTrkComboHypoToolMonitoring, self).__init__(name)
        self.Histograms = [
        defineHistogram('totalCharge', type='TH1F', path='EXPERT', title="Total Charge of N tracks; total charge", xbins=20, xmin=-10, xmax=10),
        defineHistogram('chi2', type='TH1F', path='EXPERT', title="chi2 fit of N tracks; vertex chi2", xbins=100, xmin=0, xmax=100),
        defineHistogram('mass', type='TH1F', path='EXPERT', title="mass of track pairs; m_{#mu#mu} [GeV]", xbins=100, xmin=0, xmax=20),
        defineHistogram('pT_trk1', type='TH1F', path='EXPERT', title="p_{T} of the first track; p_{T}(#mu_{1}) [GeV]", xbins=100, xmin=0, xmax=40),
        defineHistogram('pT_trk2', type='TH1F', path='EXPERT', title="p_{T} of the second track; p_{T}(#mu_{2}) [GeV]", xbins=100, xmin=0, xmax=40),
        ]
