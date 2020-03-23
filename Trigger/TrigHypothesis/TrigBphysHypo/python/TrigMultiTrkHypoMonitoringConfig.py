# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

class TrigMultiTrkHypoMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigMultiTrkHypoMonitoring, self).__init__(name)
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
        defineHistogram('bphysCharge', type='TH1F', path='EXPERT', title="total charge of N tracks", xbins=20, xmin=-10, xmax=10),
        defineHistogram('TIME_all', type='TH1F', path='EXPERT', title='execution time; [microseconds]', xbins=100, xmin=0, xmax=100),
        ]

class TrigMultiTrkHypoToolMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigMultiTrkHypoToolMonitoring, self).__init__(name)
        self.Histograms = [
        defineHistogram('totCharge', type='TH1F', path='EXPERT', title="Total Charge of tracks", xbins=21, xmin=-10, xmax=10),
        defineHistogram('CutCounter', type='TH1F', path='EXPERT', title="mass of track pairs; m_{#mu#mu} [GeV]", xbins=5, xmin=-0.5, xmax=4.5),
        defineHistogram('FitChi2', type='TH1F', path='EXPERT', title="chi2 fit of N tracks; fit chi2 of N selected tracks", xbins=100, xmin=0, xmax=100),
        defineHistogram('VertexMass', type='TH1F', path='EXPERT', title="Number of tracks selected; N selected tracks", xbins=100, xmin=0, xmax=20),
        defineHistogram('trackPts', type='TH1F', path='EXPERT', title="pair mass of N tracks; pair mass of N selected tracks [GeV]", xbins=100, xmin=0, xmax=20)
        ]
