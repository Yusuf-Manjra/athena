# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################# Validation, DQ checks
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

errorcalolabels = 'NoROIDescr:NoCellCont:EmptyCellCont:NoClustCont:NoClustKey:EmptyClustCont:NoJetAttach:NoHLTtauAttach:NoHLTtauDetAttach:NoHLTtauXdetAttach'

errortracklabels = 'NoTrkCont:NoVtxCont'

class TrigTauRecOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigTauRecOnlineMonitoring"):
        super(TrigTauRecOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        
        self.Histograms += [ defineHistogram('nCand', type='TH1F', title=" Number of tau candidates;Number of tau candidates; nevents", xbins=10, xmin=-1.0, xmax=9.) ]
        self.Histograms += [ defineHistogram('nRoI_EFTauCells', type='TH1F', title="EF N RoI cells; N Cells; nRoIs", xbins=100, xmin=0., xmax=6000.) ]
        self.Histograms += [ defineHistogram('nRoI_EFTauTracks', type='TH1F', title="EF N RoI Tracks; N Tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.) ]

        self.Histograms += [ defineHistogram('Et', type='TH1F', title=" EF Et had+em (at EM scale); Et had+em (at EM scale) [GeV]; nRoIs", xbins=100, xmin=0., xmax=500.) ]
        self.Histograms += [ defineHistogram('EtFinal', type='TH1F', title=" EF Calibrated Et (tauJet->et()); EF Calibrated Et [GeV]; nRoIs", xbins=100, xmin=0., xmax=500.) ]

        self.Histograms += [ defineHistogram('EMRadius', type='TH1F', title="EF EMRadius; EM radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.) ]
        self.Histograms += [ defineHistogram('HadRadius', type='TH1F', title="EF HadRadius; Had radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.) ]

        self.Histograms += [ defineHistogram('PhiL1', type='TH1F', title="L1 RoI Phi; L1 RoI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.) ]
        self.Histograms += [ defineHistogram('EtaL1', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55) ]       
        self.Histograms += [ defineHistogram('EtaL1, PhiL1', type='TH2F', title="L1 ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi ",
                                             xbins=51, xmin=-2.55, xmax=2.55,
                                             ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)]
        self.Histograms += [ defineHistogram('EtaEF, PhiEF', type='TH2F', title="EF ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi ",
                                             xbins=51, xmin=-2.55, xmax=2.55,
                                             ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)]
        self.Histograms += [ defineHistogram('EtHad, EtEm', type='TH2F', title="EF Et had vs Em in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                             xbins=30, xmin=0., xmax=150.,
                                             ybins=30, ymin=0., ymax=150.)]
        
        self.Histograms += [ defineHistogram('dPhiEFTau_RoI', type='TH1F', title="diff EF vs RoI phi ; Dphi; nRoIs", xbins=100, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('dEtaEFTau_RoI', type='TH1F', title="diff EF vs RoI eta ; Deta; nRoIs", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('dEtaEFTau_RoI, dPhiEFTau_RoI', type='TH2F', title="dEta vs dPhi in TrigTauRec FEX; Delta-eta; Delta-phi",
                                             xbins=40 , xmin=-0.2, xmax=0.2,
                                             ybins=40 , ymin=-0.2, ymax=0.2) ]

        self.Histograms += [ defineHistogram('NTrk', type='TH1F', title=" Number of tracks;Number of tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.) ]
        self.Histograms += [ defineHistogram('nWideTrk',        type='TH1F', title=" EF N Wide Tracks;N Wide Tracks; nRoIs"              , xbins=17, xmin=-2.0, xmax=15.) ]

        self.Histograms += [ defineHistogram('EMFrac', type='TH1F', title=" EM Fraction;EM Fraction;nRoIs", xbins=70, xmin=-0.1, xmax=1.3) ]
        self.Histograms += [ defineHistogram('IsoFrac', type='TH1F', title=" Isolation Fraction;Isolation Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2) ]
        self.Histograms += [ defineHistogram('centFrac',        type='TH1F', title=" EF central Fraction;central Fraction; nRoIs"        , xbins=80, xmin=-0.4, xmax=1.2) ]

        self.Histograms += [ defineHistogram('TrkAvgDist', type='TH1F', title=" Track Average Distance; TrkAvgDist; nRoIs", xbins=41, xmin=-0.01, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EtovPtLead', type='TH1F', title=" Et over lead track Pt; EtovPtLead; nRoIs", xbins=41, xmin=-0.5, xmax=20.0) ]
        self.Histograms += [ defineHistogram('ipSigLeadTrk',    type='TH1F', title=" EF IPsig Leading Track;iIPsig Leading Track; nRoIs" , xbins=100, xmin=-50., xmax=50) ]
        self.Histograms += [ defineHistogram('trFlightPathSig', type='TH1F', title=" EF Flightpath sig Track;Flightpath sig Track; nRoIs", xbins=100, xmin=-20., xmax=40) ]
        self.Histograms += [ defineHistogram('massTrkSys',      type='TH1F', title=" EF Mass Trk Sys;Mass Trk Sys [GeV]; nRoIs"                , xbins=100, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('dRmax',           type='TH1F', title=" EF dR max;dR max; nRoIs"                            , xbins=50, xmin=-0., xmax=0.25) ]
        self.Histograms += [ defineHistogram('PSSFraction',        type='TH1F', title=" EF Presampler strip energy fraction;PSS energy fraction; nRoIs", xbins=50, xmin=-0.5, xmax=1.) ]
        self.Histograms += [ defineHistogram('EMPOverTrkSysP',     type='TH1F', title=" EF EMP over TrkSysP;EMP over TrkSysP; nRoIs", xbins=41, xmin=-0.5, xmax=20.0) ]
        self.Histograms += [ defineHistogram('ChPiEMEOverCaloEME', type='TH1F', title=" EF EM energy of charged pions over calorimetric EM energy;ChPiEME over CaloEME; nRoIs", xbins=40, xmin=-20., xmax=20.) ]
        self.Histograms += [ defineHistogram('innerTrkAvgDist', type='TH1F', title=" EF inner track average distance; innerTrkAvgDist; nRoIs", xbins=40, xmin=-0.05, xmax=0.5) ]
        self.Histograms += [ defineHistogram('SumPtTrkFrac', type='TH1F', title=" EF Sum Pt Track Fraction; SumPtTrkFrac; nRoIs", xbins=40, xmin=-0.5, xmax=1.1) ]
 
        self.Histograms += [ defineHistogram('ActualInteractions', type='TH1F', title=" Number of actual interaction per bunch crossing;ActualInteractions;nevents", xbins=80, xmin=0.0, xmax=80.) ]
        self.Histograms += [ defineHistogram('AvgInteractions', type='TH1F', title=" Number of average interaction per bunch crossing;AvgInteractions;nevents", xbins=80, xmin=0.0, xmax=80.) ]

        self.Histograms += [ defineHistogram('beamspot_x', type='TH1F', title=" Beamspot position;Beamspot x;nevents", xbins=50, xmin=-10.0, xmax=10.) ]
        self.Histograms += [ defineHistogram('beamspot_y', type='TH1F', title=" Beamspot position;Beamspot y;nevents", xbins=50, xmin=-10.0, xmax=10.) ]
        self.Histograms += [ defineHistogram('beamspot_z', type='TH1F', title=" Beamspot position;Beamspot z;nevents", xbins=100, xmin=-500.0, xmax=500.) ]


        self.Histograms += [ defineHistogram('calo_errors', type='TH1F', title=" EF Calo Errors ;  ; nRoIs", xbins=10, xmin=-0.5, xmax=9.5 , labels=errorcalolabels) ]
        self.Histograms += [ defineHistogram('track_errors', type='TH1F', title=" EF Track Errors ;  ; nRoIs", xbins=2, xmin=-0.5, xmax=2.5 , labels=errortracklabels) ]

        # RNN ID
        # scalar inputs
        self.Histograms += [ defineHistogram('RNN_scalar_ptRatioEflowApprox', type='TH1F', title=" EF RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.)", xbins=50, xmin=0., xmax=4.) ]
        self.Histograms += [ defineHistogram('RNN_scalar_mEflowApprox', type='TH1F', title=" EF RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.)", xbins=50, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram('RNN_scalar_pt_jetseed_log', type='TH1F', title=" EF RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed)", xbins=50, xmin=3.5, xmax=7.) ]
        # cluster inputs
        self.Histograms += [ defineHistogram('RNN_Nclusters', type='TH1F', title=" EF RNN Nclusters; Number of clusters", xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('RNN_cluster_et_log', type='TH1F', title=" EF RNN cluster et_log; Log10(cluster E_{T})", xbins=50, xmin=1., xmax=7.) ]
        self.Histograms += [ defineHistogram('RNN_cluster_dEta', type='TH1F', title=" EF RNN cluster dEta; #Delta#eta(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5) ]
        self.Histograms += [ defineHistogram('RNN_cluster_dPhi', type='TH1F', title=" EF RNN cluster dPhi; #Delta#phi(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5) ]
        # track inputs
        self.Histograms += [ defineHistogram('RNN_Ntracks', type='TH1F', title=" EF RNN Ntracks; Number of tracks", xbins=12, xmin=0., xmax=12.) ]
        self.Histograms += [ defineHistogram('RNN_track_pt_log', type='TH1F', title=" EF RNN track pt_log; Log10(track p_{T})", xbins=50, xmin=2.7, xmax=7.) ]
        self.Histograms += [ defineHistogram('RNN_track_dEta', type='TH1F', title=" EF RNN track dEta; #Delta#eta(track tau)", xbins=50, xmin=-0.5, xmax=0.5) ]
        self.Histograms += [ defineHistogram('RNN_track_dPhi', type='TH1F', title=" EF RNN track dPhi; #Delta#phi(track xtau)", xbins=50, xmin=-0.5, xmax=0.5) ]
        self.Histograms += [ defineHistogram('RNN_track_d0_abs_log', type='TH1F', title=" EF RNN track d0_abs_log; Log10(Abs(track.d0()) + 1e-6)", xbins=50, xmin=-6.1, xmax=2.) ]
        self.Histograms += [ defineHistogram('RNN_track_z0sinThetaTJVA_abs_log', type='TH1F', title=" EF RNN track z0sinThetaTJVA_abs_log; Log10(Abs(track.z0sinThetaTJVA(tau)) + 1e-6)", xbins=50, xmin=-6.1, xmax=4.) ]
        self.Histograms += [ defineHistogram('RNN_track_nInnermostPixelHits', type='TH1F', title=" EF RNN track nInnermostPixelHits; nInnermostPixelHits", xbins=4, xmin=0., xmax=4.) ]
        self.Histograms += [ defineHistogram('RNN_track_nPixelHits', type='TH1F', title=" EF RNN track nPixelHits; nPixelHits", xbins=11, xmin=0., xmax=11.) ]
        self.Histograms += [ defineHistogram('RNN_track_nSCTHits', type='TH1F', title=" EF RNN track nSCTHits; nSCTHits", xbins=20, xmin=0., xmax=20.) ]
        # output
        self.Histograms += [ defineHistogram('RNNJetScore', type='TH1F', title=" EF RNNJetScore; RNNJetScore", xbins=50, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('RNNJetScoreSigTrans', type='TH1F', title=" EF RNNJetScoreSigTrans; RNNJetScoreSigTrans", xbins=50, xmin=0., xmax=1.) ]



########## add validation specific histograms 
class TrigTauRecValidationMonitoring(TrigTauRecOnlineMonitoring):
    def __init__ (self, name="TrigTauRecValidationMonitoring"):
        super(TrigTauRecValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def tauMonitoringCaloOnly():
   monTool = GenericMonitoringTool('MonTool')
   monTool.defineHistogram('mEflowApprox',path="EXPERT", type='TH1F', title=" EF RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.)", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('ptRatioEflowApprox', path="EXPERT",type='TH1F', title=" EF RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.)", xbins=50, xmin=0., xmax=4.) 
   monTool.defineHistogram('ptDetectorAxis', path='EXPERT',type='TH1F', title=" EF RNN ptDetectorAxis log; ptDetectorAxis_log ", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('pt_jetseed_log', path='EXPERT',type='TH1F', title=" EF RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed)", xbins=50, xmin=3.5, xmax=7.)
   monTool.defineHistogram('RNN_clusternumber',path='EXPERT', type='TH1F', title=" EF RNN cluster number; N RNNClusters", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('cluster_et_log',path="EXPERT", type='TH1F', title=" EF RNN cluster et_log; Log10(cluster E_{T})", xbins=50, xmin=1., xmax=7.)
   monTool.defineHistogram('cluster_dEta',path="EXPERT", type='TH1F', title=" EF RNN cluster dEta; #Delta#eta(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_dPhi',path="EXPERT", type='TH1F', title=" EF RNN cluster dPhi; #Delta#phi(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_log_SECOND_R',path="EXPERT",type='TH1F',title='cluster_SECOND_R_log10; cluster_SECOND_R_log10;Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_SECOND_LAMBDA',path="EXPERT",type='TH1F',title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10;Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_CENTER_LAMBDA',path="EXPERT",type='TH1F',title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10;Events',xbins=50,xmin=-2,xmax=5)
   monTool.defineHistogram('RNN_tracknumber',path='EXPERT', type='TH1F', title=" EF RNN track number; N RNNTracks", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('track_pt_log',path='EXPERT', type='TH1F', title=" EF RNN track pt_log; Log10(track p_{T})", xbins=50, xmin=2.7, xmax=7.)
   monTool.defineHistogram('track_dEta',path='EXPERT', type='TH1F', title=" EF RNN track dEta; #Delta#eta(track tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_dPhi',path='EXPERT', type='TH1F', title=" EF RNN track dPhi; #Delta#phi(track xtau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_d0_abs_log',path='EXPERT',type='TH1F', title=" EF RNN track d0_abs_log; Log10(Abs(track.d0()) + 1e-6)", xbins=50, xmin=-6.1, xmax=2.)
   monTool.defineHistogram('track_z0sinThetaTJVA_abs_log',path='EXPERT', type='TH1F', title=" EF RNN track z0sinThetaTJVA_abs_log; Log10(Abs(track.z0sinThetaTJVA(tau)) + 1e-6)", xbins=50, xmin=-6.1, xmax=4.)
   monTool.defineHistogram('track_nPixelHitsPlusDeadSensors',path='EXPERT', type='TH1F', title=" EF RNN track nPixelHitsPlusDeadSensors; nPixelHits", xbins=11, xmin=0., xmax=11.)
   monTool.defineHistogram('track_nSCTHitsPlusDeadSensors',path='EXPERT', type='TH1F', title=" EF RNN track nSCTHitsPlusDeadSensors; nSCTHits", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('nRoI_EFTauCells', path='EXPERT', type='TH1F', title="EF N RoI cells; N Cells; nRoIs", xbins=100, xmin=0., xmax=6000.)
   monTool.defineHistogram('dPhiEFTau_RoI', path='EXPERT', type='TH1F', title="diff EF vs RoI phi ; Dphi; nRoIs", xbins=100, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI', path='EXPERT', type='TH1F', title="diff EF vs RoI eta ; Deta; nRoIs", xbins=80, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI, dPhiEFTau_RoI', path='EXPERT', type='TH2F', title="dEta vs dPhi in TrigTauRec FEX; Delta-eta; Delta-phi",
                                                           xbins=40 , xmin=-0.2, xmax=0.2,
                                                           ybins=40 , ymin=-0.2, ymax=0.2)
   monTool.defineHistogram('EMRadius', path='EXPERT', type='TH1F', title="EF EMRadius; EM radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('HadRadius', path='EXPERT', type='TH1F', title="EF HadRadius; Had radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('EtHad, EtEm', path='EXPERT', type='TH2F', title="EF Et had vs Em in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                                xbins=30, xmin=0., xmax=150.,
                                                ybins=30, ymin=0., ymax=150.)
   monTool.defineHistogram('EMFrac', path='EXPERT', type='TH1F', title="EM Fraction;EM Fraction;nRoIs", xbins=70, xmin=-0.1, xmax=1.3)
   monTool.defineHistogram('IsoFrac', path='EXPERT', type='TH1F', title="Isolation Fraction;Isolation Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('centFrac', path='EXPERT', type='TH1F', title="EF central Fraction;central Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('nCand', path='EXPERT', type='TH1F', title="Number of tau candidates;Number of tau candidates; nevents", xbins=10, xmin=-1.0, xmax=9.)
   monTool.defineHistogram('PhiL1', path='EXPERT', type='TH1F', title="L1 RoI Phi; L1 RoI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('EtaL1, PhiL1', path='EXPERT', type='TH2F', title="L1 ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=51, xmin=-2.55, xmax=2.55,
                                           ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaEF, PhiEF', path='EXPERT', type='TH2F', title="EF ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=51, xmin=-2.55, xmax=2.55,
                                           ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)

   monTool.defineHistogram('EtaEF', path='EXPERT', type='TH1F', title="EF ROI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('PhiEF', path='EXPERT', type='TH1F', title="EF ROI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)
       

   return monTool


def tauMonitoringCaloOnlyMVA():
   monTool = GenericMonitoringTool('MonTool')
   
   monTool.defineHistogram('mEflowApprox',path="EXPERT", type='TH1F', title=" EF RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.)", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('ptRatioEflowApprox', path="EXPERT",type='TH1F', title=" EF RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.)", xbins=50, xmin=0., xmax=4.)
   monTool.defineHistogram('ptDetectorAxis', path='EXPERT',type='TH1F', title=" EF RNN ptDetectorAxis log; ptDetectorAxis_log ", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('pt_jetseed_log', path='EXPERT',type='TH1F', title=" EF RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed)", xbins=50, xmin=3.5, xmax=7.)
   monTool.defineHistogram('RNN_clusternumber',path='EXPERT', type='TH1F', title=" EF RNN cluster number; N RNNClusters", xbins=15, xmin=0., xmax=15.)
   monTool.defineHistogram('cluster_et_log',path="EXPERT", type='TH1F', title=" EF RNN cluster et_log; Log10(cluster E_{T})", xbins=50, xmin=1., xmax=7.) 
   monTool.defineHistogram('cluster_dEta',path="EXPERT", type='TH1F', title=" EF RNN cluster dEta; #Delta#eta(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_dPhi',path="EXPERT", type='TH1F', title=" EF RNN cluster dPhi; #Delta#phi(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_log_SECOND_R',path="EXPERT",type='TH1F',title='cluster_SECOND_R_log10; cluster_SECOND_R_log10;Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_SECOND_LAMBDA',path="EXPERT",type='TH1F',title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10;Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_CENTER_LAMBDA',path="EXPERT",type='TH1F',title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10;Events',xbins=50,xmin=-2,xmax=5)
   monTool.defineHistogram('RNN_tracknumber',path='EXPERT', type='TH1F', title=" EF RNN track number; N RNNTracks", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('track_pt_log',path='EXPERT', type='TH1F', title=" EF RNN track pt_log; Log10(track p_{T})", xbins=50, xmin=2.7, xmax=7.)
   monTool.defineHistogram('track_dEta',path='EXPERT', type='TH1F', title=" EF RNN track dEta; #Delta#eta(track tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_dPhi',path='EXPERT', type='TH1F', title=" EF RNN track dPhi; #Delta#phi(track xtau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_d0_abs_log',path='EXPERT',type='TH1F', title=" EF RNN track d0_abs_log; Log10(Abs(track.d0()) + 1e-6)", xbins=50, xmin=-6.1, xmax=2.)
   monTool.defineHistogram('track_z0sinThetaTJVA_abs_log',path='EXPERT', type='TH1F', title=" EF RNN track z0sinThetaTJVA_abs_log; Log10(Abs(track.z0sinThetaTJVA(tau)) + 1e-6)", xbins=50, xmin=-6.1, xmax=4.)
   monTool.defineHistogram('track_nPixelHitsPlusDeadSensors',path='EXPERT', type='TH1F', title=" EF RNN track nPixelHitsPlusDeadSensors; nPixelHits", xbins=11, xmin=0., xmax=11.)
   monTool.defineHistogram('track_nSCTHitsPlusDeadSensors',path='EXPERT', type='TH1F', title=" EF RNN track nSCTHitsPlusDeadSensors; nSCTHits", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('nRoI_EFTauCells', path='EXPERT', type='TH1F', title="EF N RoI cells; N Cells; nRoIs", xbins=100, xmin=0., xmax=6000.)
   monTool.defineHistogram('EMRadius', path='EXPERT', type='TH1F', title="EF EMRadius; EM radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('HadRadius', path='EXPERT', type='TH1F', title="EF HadRadius; Had radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('EtHad, EtEm', path='EXPERT', type='TH2F', title="EF Et had vs Em in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                                xbins=30, xmin=0., xmax=150.,
                                                ybins=30, ymin=0., ymax=150.)
   monTool.defineHistogram('EMFrac', path='EXPERT', type='TH1F', title="EM Fraction;EM Fraction;nRoIs", xbins=70, xmin=-0.1, xmax=1.3)
   monTool.defineHistogram('IsoFrac', path='EXPERT', type='TH1F', title="Isolation Fraction;Isolation Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('centFrac', path='EXPERT', type='TH1F', title="EF central Fraction;central Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('nCand', path='EXPERT', type='TH1F', title="Number of tau candidates;Number of tau candidates; nevents", xbins=10, xmin=-1.0, xmax=9.)
   monTool.defineHistogram('PhiL1', path='EXPERT', type='TH1F', title="L1 RoI Phi; L1 RoI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('EtaL1, PhiL1', path='EXPERT', type='TH2F', title="L1 ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=51, xmin=-2.55, xmax=2.55,
                                           ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaEF, PhiEF', path='EXPERT', type='TH2F', title="EF ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=51, xmin=-2.55, xmax=2.55,
                                           ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaEF', path='EXPERT', type='TH1F', title="EF ROI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('PhiEF', path='EXPERT', type='TH1F', title="EF ROI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)

   return monTool


def tauMonitoringPreselection():

   monTool = GenericMonitoringTool('MonTool')
   
   monTool.defineHistogram('mEflowApprox',path="EXPERT", type='TH1F', title=" EF RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.)", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('ptRatioEflowApprox', path="EXPERT",type='TH1F', title=" EF RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.)", xbins=50, xmin=0., xmax=4.)
   monTool.defineHistogram('ptDetectorAxis', path='EXPERT',type='TH1F', title=" EF RNN ptDetectorAxis log; ptDetectorAxis_log ", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('pt_jetseed_log', path='EXPERT',type='TH1F', title=" EF RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed)", xbins=50, xmin=3.5, xmax=7.)
   monTool.defineHistogram('RNN_clusternumber',path='EXPERT', type='TH1F', title=" EF RNN cluster number; N RNNClusters", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('cluster_et_log',path="EXPERT", type='TH1F', title=" EF RNN cluster et_log; Log10(cluster E_{T})", xbins=50, xmin=1., xmax=7.)
   monTool.defineHistogram('cluster_dEta',path="EXPERT", type='TH1F', title=" EF RNN cluster dEta; #Delta#eta(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_dPhi',path="EXPERT", type='TH1F', title=" EF RNN cluster dPhi; #Delta#phi(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_log_SECOND_R',path="EXPERT",type='TH1F',title='cluster_SECOND_R_log10; cluster_SECOND_R_log10;Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_SECOND_LAMBDA',path="EXPERT",type='TH1F',title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10;Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_CENTER_LAMBDA',path="EXPERT",type='TH1F',title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10;Events',xbins=50,xmin=-2,xmax=5)
   monTool.defineHistogram('RNN_tracknumber',path='EXPERT', type='TH1F', title=" EF RNN track number; N RNNTracks", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('track_pt_log',path='EXPERT', type='TH1F', title=" EF RNN track pt_log; Log10(track p_{T})", xbins=50, xmin=2.7, xmax=7.)
   monTool.defineHistogram('track_dEta',path='EXPERT', type='TH1F', title=" EF RNN track dEta; #Delta#eta(track tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_dPhi',path='EXPERT', type='TH1F', title=" EF RNN track dPhi; #Delta#phi(track xtau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_d0_abs_log',path='EXPERT',type='TH1F', title=" EF RNN track d0_abs_log; Log10(Abs(track.d0()) + 1e-6)", xbins=50, xmin=-6.1, xmax=2.)
   monTool.defineHistogram('track_z0sinThetaTJVA_abs_log',path='EXPERT', type='TH1F', title=" EF RNN track z0sinThetaTJVA_abs_log; Log10(Abs(track.z0sinThetaTJVA(tau)) + 1e-6)", xbins=50, xmin=-6.1, xmax=4.)
   monTool.defineHistogram('track_nPixelHitsPlusDeadSensors',path='EXPERT', type='TH1F', title=" EF RNN track nPixelHitsPlusDeadSensors; nPixelHits", xbins=11, xmin=0., xmax=11.)
   monTool.defineHistogram('track_nSCTHitsPlusDeadSensors',path='EXPERT', type='TH1F', title=" EF RNN track nSCTHitsPlusDeadSensors; nSCTHits", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('nRoI_EFTauCells', path='EXPERT', type='TH1F', title="EF N RoI cells; N Cells; nRoIs", xbins=100, xmin=0., xmax=6000.)
   monTool.defineHistogram('nRoI_EFTauTracks', path='EXPERT', type='TH1F', title="EF N RoI Tracks; N Tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('dPhiEFTau_RoI', path='EXPERT', type='TH1F', title="diff EF vs RoI phi ; Dphi; nRoIs", xbins=100, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI', path='EXPERT', type='TH1F', title="diff EF vs RoI eta ; Deta; nRoIs", xbins=80, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI, dPhiEFTau_RoI', path='EXPERT', type='TH2F', title="dEta vs dPhi in TrigTauRec FEX; Delta-eta; Delta-phi",
                                                           xbins=40 , xmin=-0.2, xmax=0.2,
                                                           ybins=40 , ymin=-0.2, ymax=0.2)
   monTool.defineHistogram('EMRadius', path='EXPERT', type='TH1F', title="EF EMRadius; EM radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('HadRadius', path='EXPERT', type='TH1F', title="EF HadRadius; Had radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('EtHad, EtEm', path='EXPERT', type='TH2F', title="EF Et had vs Em in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                                xbins=30, xmin=0., xmax=150.,
                                                ybins=30, ymin=0., ymax=150.)
   monTool.defineHistogram('EMFrac', path='EXPERT', type='TH1F', title="EM Fraction;EM Fraction;nRoIs", xbins=70, xmin=-0.1, xmax=1.3)
   monTool.defineHistogram('IsoFrac', path='EXPERT', type='TH1F', title="Isolation Fraction;Isolation Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('centFrac', path='EXPERT', type='TH1F', title="EF central Fraction;central Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('nWideTrk', path='EXPERT', type='TH1F', title="EF N Wide Tracks;N Wide Tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('ipSigLeadTrk', path='EXPERT', type='TH1F', title="EF IPsig Leading Track;iIPsig Leading Track; nRoIs", xbins=100, xmin=-50., xmax=50)
   monTool.defineHistogram('trFlightPathSig', path='EXPERT', type='TH1F', title="EF Flightpath sig Track;Flightpath sig Track; nRoIs", xbins=100, xmin=-20., xmax=40)
   monTool.defineHistogram('massTrkSys', path='EXPERT', type='TH1F', title="EF Mass Trk Sys;Mass Trk Sys [GeV]; nRoIs", xbins=100, xmin=0., xmax=50.)
   monTool.defineHistogram('dRmax', path='EXPERT', type='TH1F', title="EF dR max;dR max; nRoIs", xbins=50, xmin=-0., xmax=0.25)
   monTool.defineHistogram('NTrk', path='EXPERT', type='TH1F', title="Number of tracks;Number of tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('TrkAvgDist', path='EXPERT', type='TH1F', title="Track Average Distance; TrkAvgDist; nRoIs", xbins=41, xmin=-0.01, xmax=0.4)
   monTool.defineHistogram('EtovPtLead', path='EXPERT', type='TH1F', title="Et over lead track Pt; EtovPtLead; nRoIs", xbins=41, xmin=-0.5, xmax=20.0)
   monTool.defineHistogram('PSSFraction', path='EXPERT', type='TH1F', title="EF Presampler strip energy fraction;PSS energy fraction; nRoIs", xbins=50, xmin=-0.5, xmax=1.)
   monTool.defineHistogram('EMPOverTrkSysP', path='EXPERT', type='TH1F', title="EF EMP over TrkSysP;EMP over TrkSysP; nRoIs", xbins=41, xmin=-0.5, xmax=20.0)
   monTool.defineHistogram('ChPiEMEOverCaloEME', path='EXPERT', type='TH1F', title="EF EM energy of charged pions over calorimetric EM energy;ChPiEME over CaloEME; nRoIs", xbins=40, xmin=-20., xmax=20.)
   monTool.defineHistogram('innerTrkAvgDist', path='EXPERT', type='TH1F', title="EF inner track average distance; innerTrkAvgDist; nRoIs", xbins=40, xmin=-0.05, xmax=0.5)
   monTool.defineHistogram('nCand', path='EXPERT', type='TH1F', title="Number of tau candidates;Number of tau candidates; nevents", xbins=10, xmin=-1.0, xmax=9.)
   monTool.defineHistogram('PhiL1', path='EXPERT', type='TH1F', title="L1 RoI Phi; L1 RoI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('EtaL1, PhiL1', path='EXPERT', type='TH2F', title="L1 ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=51, xmin=-2.55, xmax=2.55,
                                           ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaEF, PhiEF', path='EXPERT', type='TH2F', title="EF ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=51, xmin=-2.55, xmax=2.55,
                                           ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)

   monTool.defineHistogram('EtaEF', path='EXPERT', type='TH1F', title="EF ROI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('PhiEF', path='EXPERT', type='TH1F', title="EF ROI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)

   return monTool


def tauMonitoringPrecision():

   monTool = GenericMonitoringTool('MonTool')
 
   monTool.defineHistogram('mEflowApprox',path="EXPERT", type='TH1F', title=" EF RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.)", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('ptRatioEflowApprox', path="EXPERT",type='TH1F', title=" EF RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.)", xbins=50, xmin=0., xmax=4.)
   monTool.defineHistogram('ptDetectorAxis', path='EXPERT',type='TH1F', title=" EF RNN ptDetectorAxis log; ptDetectorAxis_log ", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('pt_jetseed_log', path='EXPERT',type='TH1F', title=" EF RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed)", xbins=50, xmin=3.5, xmax=7.)
   monTool.defineHistogram('RNN_clusternumber',path='EXPERT', type='TH1F', title=" EF RNN cluster number; N RNNClusters", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('cluster_et_log',path="EXPERT", type='TH1F', title=" EF RNN cluster et_log; Log10(cluster E_{T})", xbins=50, xmin=1., xmax=7.)
   monTool.defineHistogram('cluster_dEta',path="EXPERT", type='TH1F', title=" EF RNN cluster dEta; #Delta#eta(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_dPhi',path="EXPERT", type='TH1F', title=" EF RNN cluster dPhi; #Delta#phi(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_log_SECOND_R',path="EXPERT",type='TH1F',title='cluster_SECOND_R_log10; cluster_SECOND_R_log10;Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_SECOND_LAMBDA',path="EXPERT",type='TH1F',title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10;Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_CENTER_LAMBDA',path="EXPERT",type='TH1F',title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10;Events',xbins=50,xmin=-2,xmax=5)
   monTool.defineHistogram('RNN_tracknumber',path='EXPERT', type='TH1F', title=" EF RNN track number; N RNNTracks", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('track_pt_log',path='EXPERT', type='TH1F', title=" EF RNN track pt_log; Log10(track p_{T})", xbins=50, xmin=2.7, xmax=7.) 
   monTool.defineHistogram('track_dEta',path='EXPERT', type='TH1F', title=" EF RNN track dEta; #Delta#eta(track tau)", xbins=50, xmin=-0.5, xmax=0.5) 
   monTool.defineHistogram('track_dPhi',path='EXPERT', type='TH1F', title=" EF RNN track dPhi; #Delta#phi(track xtau)", xbins=50, xmin=-0.5, xmax=0.5) 
   monTool.defineHistogram('track_d0_abs_log',path='EXPERT',type='TH1F', title=" EF RNN track d0_abs_log; Log10(Abs(track.d0()) + 1e-6)", xbins=50, xmin=-6.1, xmax=2.) 
   monTool.defineHistogram('track_z0sinThetaTJVA_abs_log',path='EXPERT', type='TH1F', title=" EF RNN track z0sinThetaTJVA_abs_log; Log10(Abs(track.z0sinThetaTJVA(tau)) + 1e-6)", xbins=50, xmin=-6.1, xmax=4.)
   monTool.defineHistogram('track_nPixelHitsPlusDeadSensors',path='EXPERT', type='TH1F', title=" EF RNN track nPixelHitsPlusDeadSensors; nPixelHits", xbins=11, xmin=0., xmax=11.)
   monTool.defineHistogram('track_nSCTHitsPlusDeadSensors',path='EXPERT', type='TH1F', title=" EF RNN track nSCTHitsPlusDeadSensors; nSCTHits", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('nRoI_EFTauCells', path='EXPERT', type='TH1F', title="EF N RoI cells; N Cells; nRoIs", xbins=100, xmin=0., xmax=6000.)
   monTool.defineHistogram('nRoI_EFTauTracks', path='EXPERT', type='TH1F', title="EF N RoI Tracks; N Tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('dPhiEFTau_RoI', path='EXPERT', type='TH1F', title="diff EF vs RoI phi ; Dphi; nRoIs", xbins=100, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI', path='EXPERT', type='TH1F', title="diff EF vs RoI eta ; Deta; nRoIs", xbins=80, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI, dPhiEFTau_RoI', path='EXPERT', type='TH2F', title="dEta vs dPhi in TrigTauRec FEX; Delta-eta; Delta-phi",
                                                           xbins=40 , xmin=-0.2, xmax=0.2,
                                                           ybins=40 , ymin=-0.2, ymax=0.2)
   monTool.defineHistogram('EMRadius', path='EXPERT', type='TH1F', title="EF EMRadius; EM radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('HadRadius', path='EXPERT', type='TH1F', title="EF HadRadius; Had radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('EtHad, EtEm', path='EXPERT', type='TH2F', title="EF Et had vs Em in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                                xbins=30, xmin=0., xmax=150.,
                                                ybins=30, ymin=0., ymax=150.)
   monTool.defineHistogram('EMFrac', path='EXPERT', type='TH1F', title="EM Fraction;EM Fraction;nRoIs", xbins=70, xmin=-0.1, xmax=1.3)
   monTool.defineHistogram('IsoFrac', path='EXPERT', type='TH1F', title="Isolation Fraction;Isolation Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('centFrac', path='EXPERT', type='TH1F', title="EF central Fraction;central Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('nWideTrk', path='EXPERT', type='TH1F', title="EF N Wide Tracks;N Wide Tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('ipSigLeadTrk', path='EXPERT', type='TH1F', title="EF IPsig Leading Track;iIPsig Leading Track; nRoIs", xbins=100, xmin=-50., xmax=50)
   monTool.defineHistogram('trFlightPathSig', path='EXPERT', type='TH1F', title="EF Flightpath sig Track;Flightpath sig Track; nRoIs", xbins=100, xmin=-20., xmax=40)
   monTool.defineHistogram('massTrkSys', path='EXPERT', type='TH1F', title="EF Mass Trk Sys;Mass Trk Sys [GeV]; nRoIs", xbins=100, xmin=0., xmax=50.)
   monTool.defineHistogram('dRmax', path='EXPERT', type='TH1F', title="EF dR max;dR max; nRoIs", xbins=50, xmin=-0., xmax=0.25)
   monTool.defineHistogram('NTrk', path='EXPERT', type='TH1F', title="Number of tracks;Number of tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('TrkAvgDist', path='EXPERT', type='TH1F', title="Track Average Distance; TrkAvgDist; nRoIs", xbins=41, xmin=-0.01, xmax=0.4)
   monTool.defineHistogram('EtovPtLead', path='EXPERT', type='TH1F', title="Et over lead track Pt; EtovPtLead; nRoIs", xbins=41, xmin=-0.5, xmax=20.0)
   monTool.defineHistogram('PSSFraction', path='EXPERT', type='TH1F', title="EF Presampler strip energy fraction;PSS energy fraction; nRoIs", xbins=50, xmin=-0.5, xmax=1.)
   monTool.defineHistogram('EMPOverTrkSysP', path='EXPERT', type='TH1F', title="EF EMP over TrkSysP;EMP over TrkSysP; nRoIs", xbins=41, xmin=-0.5, xmax=20.0)
   monTool.defineHistogram('ChPiEMEOverCaloEME', path='EXPERT', type='TH1F', title="EF EM energy of charged pions over calorimetric EM energy;ChPiEME over CaloEME; nRoIs", xbins=40, xmin=-20., xmax=20.)
   monTool.defineHistogram('innerTrkAvgDist', path='EXPERT', type='TH1F', title="EF inner track average distance; innerTrkAvgDist; nRoIs", xbins=40, xmin=-0.05, xmax=0.5)
   monTool.defineHistogram('nCand', path='EXPERT', type='TH1F', title="Number of tau candidates;Number of tau candidates; nevents", xbins=10, xmin=-1.0, xmax=9.)
   monTool.defineHistogram('PhiL1', path='EXPERT', type='TH1F', title="L1 RoI Phi; L1 RoI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('EtaL1, PhiL1', path='EXPERT', type='TH2F', title="L1 ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=51, xmin=-2.55, xmax=2.55,
                                           ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaEF, PhiEF', path='EXPERT', type='TH2F', title="EF ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=51, xmin=-2.55, xmax=2.55,
                                           ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)
 
   monTool.defineHistogram('EtaEF', path='EXPERT', type='TH1F', title="EF ROI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('PhiEF', path='EXPERT', type='TH1F', title="EF ROI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)

   return monTool


def tauMonitoringPrecisionMVA():
   monTool = GenericMonitoringTool('MonTool')

   monTool.defineHistogram('mEflowApprox',path="EXPERT", type='TH1F', title=" EF RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.)", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('ptRatioEflowApprox', path="EXPERT",type='TH1F', title=" EF RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.)", xbins=50, xmin=0., xmax=4.)
   monTool.defineHistogram('pt_jetseed_log', path='EXPERT',type='TH1F', title=" EF RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed)", xbins=50, xmin=3.5, xmax=7.)
   monTool.defineHistogram('ptDetectorAxis', path='EXPERT',type='TH1F', title=" EF RNN ptDetectorAxis log; ptDetectorAxis_log ", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('RNN_clusternumber',path='EXPERT', type='TH1F', title=" EF RNN cluster number; N RNNClusters", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('cluster_et_log',path="EXPERT", type='TH1F', title=" EF RNN cluster et_log; Log10(cluster E_{T})", xbins=50, xmin=1., xmax=7.)
   monTool.defineHistogram('cluster_dEta',path="EXPERT", type='TH1F', title=" EF RNN cluster dEta; #Delta#eta(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_dPhi',path="EXPERT", type='TH1F', title=" EF RNN cluster dPhi; #Delta#phi(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_log_SECOND_R',path="EXPERT",type='TH1F',title='cluster_SECOND_R_log10; cluster_SECOND_R_log10;Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_SECOND_LAMBDA',path="EXPERT",type='TH1F',title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10;Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_CENTER_LAMBDA',path="EXPERT",type='TH1F',title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10;Events',xbins=50,xmin=-2,xmax=5)
   monTool.defineHistogram('RNN_tracknumber',path='EXPERT', type='TH1F', title=" EF RNN track number; N RNNTracks", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('track_pt_log',path='EXPERT', type='TH1F', title=" EF RNN track pt_log; Log10(track p_{T})", xbins=50, xmin=2.7, xmax=7.)
   monTool.defineHistogram('track_dEta',path='EXPERT', type='TH1F', title=" EF RNN track dEta; #Delta#eta(track tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_dPhi',path='EXPERT', type='TH1F', title=" EF RNN track dPhi; #Delta#phi(track xtau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_d0_abs_log',path='EXPERT',type='TH1F', title=" EF RNN track d0_abs_log; Log10(Abs(track.d0()) + 1e-6)", xbins=50, xmin=-6.1, xmax=2.)
   monTool.defineHistogram('track_z0sinThetaTJVA_abs_log',path='EXPERT', type='TH1F', title=" EF RNN track z0sinThetaTJVA_abs_log; Log10(Abs(track.z0sinThetaTJVA(tau)) + 1e-6)", xbins=50, xmin=-6.1, xmax=4.)
   monTool.defineHistogram('track_nPixelHitsPlusDeadSensors',path='EXPERT', type='TH1F', title=" EF RNN track nPixelHitsPlusDeadSensors; nPixelHits", xbins=11, xmin=0., xmax=11.)
   monTool.defineHistogram('track_nSCTHitsPlusDeadSensors',path='EXPERT', type='TH1F', title=" EF RNN track nSCTHitsPlusDeadSensors; nSCTHits", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('nRoI_EFTauCells', path='EXPERT', type='TH1F', title="EF N RoI cells; N Cells; nRoIs", xbins=100, xmin=0., xmax=6000.)
   monTool.defineHistogram('nRoI_EFTauTracks', path='EXPERT', type='TH1F', title="EF N RoI Tracks; N Tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('dPhiEFTau_RoI', path='EXPERT', type='TH1F', title="diff EF vs RoI phi ; Dphi; nRoIs", xbins=100, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI', path='EXPERT', type='TH1F', title="diff EF vs RoI eta ; Deta; nRoIs", xbins=80, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI, dPhiEFTau_RoI', path='EXPERT', type='TH2F', title="dEta vs dPhi in TrigTauRec FEX; Delta-eta; Delta-phi",
                                                           xbins=40 , xmin=-0.2, xmax=0.2,
                                                           ybins=40 , ymin=-0.2, ymax=0.2)
   monTool.defineHistogram('EMRadius', path='EXPERT', type='TH1F', title="EF EMRadius; EM radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('HadRadius', path='EXPERT', type='TH1F', title="EF HadRadius; Had radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('EtHad, EtEm', path='EXPERT', type='TH2F', title="EF Et had vs Em in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                                xbins=30, xmin=0., xmax=150.,
                                                ybins=30, ymin=0., ymax=150.)
   monTool.defineHistogram('EMFrac', path='EXPERT', type='TH1F', title="EM Fraction;EM Fraction;nRoIs", xbins=70, xmin=-0.1, xmax=1.3)
   monTool.defineHistogram('IsoFrac', path='EXPERT', type='TH1F', title="Isolation Fraction;Isolation Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('centFrac', path='EXPERT', type='TH1F', title="EF central Fraction;central Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('nWideTrk', path='EXPERT', type='TH1F', title="EF N Wide Tracks;N Wide Tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('ipSigLeadTrk', path='EXPERT', type='TH1F', title="EF IPsig Leading Track;iIPsig Leading Track; nRoIs", xbins=100, xmin=-50., xmax=50)
   monTool.defineHistogram('trFlightPathSig', path='EXPERT', type='TH1F', title="EF Flightpath sig Track;Flightpath sig Track; nRoIs", xbins=100, xmin=-20., xmax=40)
   monTool.defineHistogram('massTrkSys', path='EXPERT', type='TH1F', title="EF Mass Trk Sys;Mass Trk Sys [GeV]; nRoIs", xbins=100, xmin=0., xmax=50.)
   monTool.defineHistogram('dRmax', path='EXPERT', type='TH1F', title="EF dR max;dR max; nRoIs", xbins=50, xmin=-0., xmax=0.25)
   monTool.defineHistogram('NTrk', path='EXPERT', type='TH1F', title="Number of tracks;Number of tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('TrkAvgDist', path='EXPERT', type='TH1F', title="Track Average Distance; TrkAvgDist; nRoIs", xbins=41, xmin=-0.01, xmax=0.4)
   monTool.defineHistogram('EtovPtLead', path='EXPERT', type='TH1F', title="Et over lead track Pt; EtovPtLead; nRoIs", xbins=41, xmin=-0.5, xmax=20.0)
   monTool.defineHistogram('PSSFraction', path='EXPERT', type='TH1F', title="EF Presampler strip energy fraction;PSS energy fraction; nRoIs", xbins=50, xmin=-0.5, xmax=1.)
   monTool.defineHistogram('EMPOverTrkSysP', path='EXPERT', type='TH1F', title="EF EMP over TrkSysP;EMP over TrkSysP; nRoIs", xbins=41, xmin=-0.5, xmax=20.0)
   monTool.defineHistogram('ChPiEMEOverCaloEME', path='EXPERT', type='TH1F', title="EF EM energy of charged pions over calorimetric EM energy;ChPiEME over CaloEME; nRoIs", xbins=40, xmin=-20., xmax=20.)
   monTool.defineHistogram('innerTrkAvgDist', path='EXPERT', type='TH1F', title="EF inner track average distance; innerTrkAvgDist; nRoIs", xbins=40, xmin=-0.05, xmax=0.5)
   monTool.defineHistogram('nCand', path='EXPERT', type='TH1F', title="Number of tau candidates;Number of tau candidates; nevents", xbins=10, xmin=-1.0, xmax=9.)
   monTool.defineHistogram('PhiL1', path='EXPERT', type='TH1F', title="L1 RoI Phi; L1 RoI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('EtaL1, PhiL1', path='EXPERT', type='TH2F', title="L1 ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi",
                                            xbins=51, xmin=-2.55, xmax=2.55,
                                           ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)
   monTool.defineHistogram('EtaEF, PhiEF', path='EXPERT', type='TH2F', title="EF ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=51, xmin=-2.55, xmax=2.55,
                                           ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)

   monTool.defineHistogram('EtaEF', path='EXPERT', type='TH1F', title="EF ROI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55)
   monTool.defineHistogram('PhiEF', path='EXPERT', type='TH1F', title="EF ROI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.)

   return monTool




