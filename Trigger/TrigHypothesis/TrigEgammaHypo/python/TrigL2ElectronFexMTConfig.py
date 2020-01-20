# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2ElectronFexMT
from AthenaCommon.SystemOfUnits import GeV, mm

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
ParticleCaloExtensionTool= Trk__ParticleCaloExtensionTool(Extrapolator = AtlasExtrapolator())

# ---------------------------------------------------------------
# class for common setups (like monitoring)
class L2ElectronFexBase(TrigL2ElectronFexMT):
    __slots__ = []
    def __init__(self, name):
        super(L2ElectronFexBase,self).__init__(name)

        # Tracking cuts
        self.TrackPt = 1.0 * GeV
        self.TrackPtHighEt = 2.0 * GeV
        self.ClusEt = 20.0 * GeV
        self.CaloTrackdEtaNoExtrap = 0.5
        self.CaloTrackdEtaNoExtrapHighEt = 0.1
        # Calo-Tracking matching cuts
        self.CaloTrackdETA = 0.5
        self.CaloTrackdPHI = 0.5
        self.CaloTrackdEoverPLow  = 0.0
        self.CaloTrackdEoverPHigh = 999.0

        self.ParticleCaloExtensionTool = ParticleCaloExtensionTool

        from TriggerJobOpts.TriggerFlags import TriggerFlags
        if 'Validation' in TriggerFlags.enableMonitoring() or 'Online' in  TriggerFlags.enableMonitoring():
            monTool = GenericMonitoringTool('MonTool')
            monTool.defineHistogram('CaloTrackdEta', path='EXPERT', type='TH1F', title="L2Electron Hypo #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4)
            monTool.defineHistogram('CaloTrackdPhi', path='EXPERT', type='TH1F', title="L2Electron Hypo #Delta #phi between cluster and track;#Delta #phi;Nevents", xbins=80, xmin=-0.4, xmax=0.4)
            monTool.defineHistogram('CaloTrackEoverP', path='EXPERT', type='TH1F', title="L2Electron Hypo E/p;E/p;Nevents", xbins=120, xmin=0, xmax=12)
            monTool.defineHistogram('PtTrack', path='EXPERT', type='TH1F', title="L2Electron Hypo p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000)
            monTool.defineHistogram('PtCalo', path='EXPERT', type='TH1F', title="L2Electron Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000)
            monTool.defineHistogram('CaloEta',path='EXPERT',  type='TH1F', title="L2Electron Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5)
            monTool.defineHistogram('CaloPhi', path='EXPERT', type='TH1F', title="L2Electron Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2)
            monTool.defineHistogram('CaloTrackdEtaNoExtrapMon', path='EXPERT',type='TH1F', title="L2Electron Fex #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4)

            self.MonTool = monTool

 
# ---------------------------------------------------------------
# TrigL2ElectronFex configurations
# ---------------------------------------------------------------

class L2ElectronFex_all(L2ElectronFexBase):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_all"):
        super(L2ElectronFex_all, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # Co-ordinates of calorimeter face for extrapolation 
        self.RCalBarrelFace = 1470.0*mm
        self.ZCalEndcapFace = 3800.0*mm

class L2ElectronFex_1(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_1"):
        super(L2ElectronFex_1, self).__init__(name)
        self.AcceptAll = True
        
class L2ElectronFex_Clean(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_Clean"):
        super(L2ElectronFex_Clean, self).__init__(name)
        self.AcceptAll = False
        self.CaloTrackdETA = 0.2
        self.CaloTrackdPHI = 0.3

class L2ElectronFex_LowPt(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_LowPt"):
        super(L2ElectronFex_1, self).__init__(name)
        self.AcceptAll = False

class L2ElectronFex_HighPt(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_HighPt"):
        super(L2ElectronFex_1, self).__init__(name)
        self.AcceptAll = False
        self.TrackPt = 2.0 * GeV

