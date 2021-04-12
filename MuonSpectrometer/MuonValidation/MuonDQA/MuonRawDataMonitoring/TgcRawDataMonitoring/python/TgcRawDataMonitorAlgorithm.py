#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@file TgcRawDataMonitorAlgorithm.py
@author M.Aoki
@date 2019-10-03
@brief Python configuration for the Run III AthenaMonitoring package for TGC 
'''

def TgcRawDataMonitoringConfig(inputFlags):
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))
    result.merge(AtlasGeometryCfg(inputFlags))
    result.merge(TrackingGeometrySvcCfg(inputFlags))

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TgcRawDataMonitorCfg')

    tgcRawDataMonAlg = helper.addAlgorithm(CompFactory.TgcRawDataMonitorAlgorithm,'TgcRawDataMonAlg')

    tgcRawDataMonAlg.TagTrigList = 'HLT_mu26_ivarmedium'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu26_ivarmedium'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu26_ivarmedium_L1MU20'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu6'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu6_L1MU6'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu6_idperf'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu14'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu20_mu8noL1;HLT_mu20'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu24_mu8noL1;HLT_mu24'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu50_L1MU20'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu60_0eta105_msonly_L1MU20'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu20_iloose_L1MU15'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu40'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu50'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu24_iloose'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu24_ivarloose'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu24_ivarmedium'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu24_imedium'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu26_imedium'

    tgcRawDataMonAlg.TagAndProbe = True
    tgcRawDataMonAlg.TagAndProbeZmumu = False

    if not inputFlags.DQ.triggerDataAvailable:
        tgcRawDataMonAlg.MuonRoIContainerName = ''

    isBS = (inputFlags.Input.Format == 'BS')
    if isBS or 'TGC_MeasurementsAllBCs' in inputFlags.Input.Collections:
        tgcRawDataMonAlg.AnaTgcPrd=True
    
    mainDir = 'Muon/MuonRawDataMonitoring/TGC/'
    import math

    trigPath = 'Trig/'

    myGroup = helper.addGroup(tgcRawDataMonAlg,'TgcRawDataMonitor',mainDir)

    myGroup.defineHistogram('roi_charge;MuonRoI_Charge',title='MuonRoI Charge;Charge;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=3,xmin=-1.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passCharge,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_Charge',title='MuonRoI_Eff_EtaVsPhi_Charge;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passCharge,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_Charge',title='MuonRoI_Eff_Pt_TGC_Charge;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_Charge_Positive',type='TH2F',
                            title='MuonRoI Eta vs Phi Charge Positive;MuonRoI Eta;MuonRoI Phi',cutmask='roi_posCharge',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_Charge_Negative',type='TH2F',
                            title='MuonRoI Eta vs Phi Charge Negative;MuonRoI Eta;MuonRoI Phi',cutmask='roi_negCharge',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_charge;MuonRoI_ThrVsCharge',type='TH2F',
                            title='MuonRoI Thr vs Charge;MuonRoI Thresholds;MuonRoI Charge',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=3,ymin=-1.5,ymax=1.5)

    myGroup.defineHistogram('roi_bw3coin;MuonRoI_BW3Coin',title='MuonRoI BW3Coin Flag;BW3Coin Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passBW3Coin,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_BW3Coin',title='MuonRoI_Eff_EtaVsPhi_BW3Coin;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passBW3Coin,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_BW3Coin',title='MuonRoI_Eff_Pt_TGC_BW3Coin;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_BW3Coin',type='TH2F',
                            title='MuonRoI Eta vs Phi BW3Coin;MuonRoI Eta;MuonRoI Phi',cutmask='roi_bw3coin',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_bw3coin;MuonRoI_ThrVsBW3Coin',type='TH2F',
                            title='MuonRoI Thr vs BW3Coin;MuonRoI Thresholds;MuonRoI BW3Coin',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_bw3coinveto;MuonRoI_BW3CoinVeto',title='MuonRoI BW3CoinVeto Flag;BW3CoinVeto Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passBW3CoinVeto,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_BW3CoinVeto',title='MuonRoI_Eff_EtaVsPhi_BW3CoinVeto;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passBW3CoinVeto,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_BW3CoinVeto',title='MuonRoI_Eff_Pt_TGC_BW3CoinVeto;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_BW3CoinVeto',type='TH2F',
                            title='MuonRoI Eta vs Phi BW3CoinVeto;MuonRoI Eta;MuonRoI Phi',cutmask='roi_bw3coinveto',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_bw3coinveto;MuonRoI_ThrVsBW3CoinVeto',type='TH2F',
                            title='MuonRoI Thr vs BW3CoinVeto;MuonRoI Thresholds;MuonRoI BW3CoinVeto',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_inncoin;MuonRoI_InnerCoin',title='MuonRoI InnerCoin Flag;InnerCoin Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passInnerCoin,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_InnerCoin',title='MuonRoI_Eff_EtaVsPhi_InnerCoin;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passInnerCoin,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_InnerCoin',title='MuonRoI_Eff_Pt_TGC_InnerCoin;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_InnerCoin',type='TH2F',
                            title='MuonRoI Eta vs Phi InnerCoin;MuonRoI Eta;MuonRoI Phi',cutmask='roi_inncoin',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_inncoin;MuonRoI_ThrVsInnerCoin',type='TH2F',
                            title='MuonRoI Thr vs InnerCoin;MuonRoI Thresholds;MuonRoI InnerCoin',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_innveto;MuonRoI_InnerCoinVeto',title='MuonRoI InnerCoinVeto Flag;InnerCoinVeto Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passInnerCoinVeto,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_InnerCoinVeto',title='MuonRoI_Eff_EtaVsPhi_InnerCoinVeto;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passInnerCoinVeto,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_InnerCoinVeto',title='MuonRoI_Eff_Pt_TGC_InnerCoinVeto;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_InnerCoinVeto',type='TH2F',
                            title='MuonRoI Eta vs Phi InnerCoinVeto;MuonRoI Eta;MuonRoI Phi',cutmask='roi_innveto',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_innveto;MuonRoI_ThrVsInnerCoinVeto',type='TH2F',
                            title='MuonRoI Thr vs InnerCoinVeto;MuonRoI Thresholds;MuonRoI InnerCoinVeto',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_goodmf;MuonRoI_GoodMF',title='MuonRoI GoodMF Flag;GoodMF Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passGoodMF,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_GoodMF',title='MuonRoI_Eff_EtaVsPhi_GoodMF;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passGoodMF,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_GoodMF',title='MuonRoI_Eff_Pt_TGC_GoodMF;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_GoodMF',type='TH2F',
                            title='MuonRoI Eta vs Phi GoodMF;MuonRoI Eta;MuonRoI Phi',cutmask='roi_goodmf',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_goodmf;MuonRoI_ThrVsGoodMF',type='TH2F',
                            title='MuonRoI Thr vs GoodMF;MuonRoI Thresholds;MuonRoI GoodMF',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_badmf;MuonRoI_BadMF',title='MuonRoI BadMF Flag;BadMF Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passBadMF,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_BadMF',title='MuonRoI_Eff_EtaVsPhi_BadMF;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passBadMF,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_BadMF',title='MuonRoI_Eff_Pt_TGC_BadMF;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_BadMF',type='TH2F',
                            title='MuonRoI Eta vs Phi BadMF;MuonRoI Eta;MuonRoI Phi',cutmask='roi_badmf',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_badmf;MuonRoI_ThrVsBadMF',type='TH2F',
                            title='MuonRoI Thr vs BadMF;MuonRoI Thresholds;MuonRoI BadMF',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_ismorecand;MuonRoI_isMoreCandInRoI',title='MuonRoI isMoreCandInRoI Flag;isMoreCandInRoI Flag;Number of events',
                            cutmask='roi_rpc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_IsMoreCandInRoI',type='TH2F',
                            title='MuonRoI Eta vs Phi IsMoreCandInRoI;MuonRoI Eta;MuonRoI Phi',cutmask='roi_ismorecand',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_ismorecand;MuonRoI_ThrVsIsMoreCandInRoI',type='TH2F',
                            title='MuonRoI Thr vs IsMoreCandInRoI;MuonRoI Thresholds;MuonRoI IsMoreCandInRoI',cutmask='roi_rpc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_thr;MuonRoI_Thresholds_RPC',title='MuonRoI Thresholds RPC;MuonRoI Threshold number;Number of events',
                            cutmask='roi_rpc',path=trigPath,xbins=20,xmin=-0.5,xmax=19.5)
    myGroup.defineHistogram('roi_thr;MuonRoI_Thresholds_TGC',title='MuonRoI Thresholds TGC;MuonRoI Threshold number;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=20,xmin=-0.5,xmax=19.5)

    myGroup.defineHistogram('roi_lumiBlock,roi_phi_barrel;MuonRoI_PhiVsLB_Barrel_sideA',title='MuonRoI PhiVsLB Barrel sideA;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideA',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=8,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')
    myGroup.defineHistogram('roi_lumiBlock,roi_phi_barrel;MuonRoI_PhiVsLB_Barrel_sideC',title='MuonRoI PhiVsLB Barrel sideC;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideC',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=8,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')

    myGroup.defineHistogram('roi_lumiBlock,roi_phi_endcap;MuonRoI_PhiVsLB_Endcap_sideA',title='MuonRoI PhiVsLB Endcap sideA;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideA',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=48,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')
    myGroup.defineHistogram('roi_lumiBlock,roi_phi_endcap;MuonRoI_PhiVsLB_Endcap_sideC',title='MuonRoI PhiVsLB Endcap sideC;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideC',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=48,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')

    myGroup.defineHistogram('roi_lumiBlock,roi_phi_forward;MuonRoI_PhiVsLB_Forward_sideA',title='MuonRoI PhiVsLB Forward sideA;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideA',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=24,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')
    myGroup.defineHistogram('roi_lumiBlock,roi_phi_forward;MuonRoI_PhiVsLB_Forward_sideC',title='MuonRoI PhiVsLB Forward sideC;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideC',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=24,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')

    
    for n in range(1,16):

        myGroup.defineHistogram('roi_eta_wInnCoin;MuonRoI_Eta_wInnCoin_Thr%02d' % n,title='MuonRoI Eta wInnCoin Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_eta_wBW3Coin;MuonRoI_Eta_wBW3Coin_Thr%02d' % n,title='MuonRoI Eta wBW3Coin Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)

        myGroup.defineHistogram('roi_eta_wInnCoinVeto;MuonRoI_Eta_wInnCoinVeto_Thr%02d' % n,title='MuonRoI Eta wInnCoinVeto Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_eta_wBW3CoinVeto;MuonRoI_Eta_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Eta wBW3CoinVeto Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)

        myGroup.defineHistogram('roi_phi_wInnCoin;MuonRoI_Phi_wInnCoin_Thr%02d' % n,title='MuonRoI Phi wInnCoin Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_phi_wBW3Coin;MuonRoI_Phi_wBW3Coin_Thr%02d' % n,title='MuonRoI Phi wBW3Coin Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)

        myGroup.defineHistogram('roi_phi_wInnCoinVeto;MuonRoI_Phi_wInnCoinVeto_Thr%02d' % n,title='MuonRoI Phi wInnCoinVeto Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_phi_wBW3CoinVeto;MuonRoI_Phi_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Phi wBW3CoinVeto Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)

        myGroup.defineHistogram('roi_eta_wInnCoin,roi_phi_wInnCoin;MuonRoI_EtaVsPhi_wInnCoin_Thr%02d' % n,title='MuonRoI Eta vs Phi wInnCoin Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi,type='TH2F')
        myGroup.defineHistogram('roi_eta_wBW3Coin,roi_phi_wBW3Coin;MuonRoI_EtaVsPhi_wBW3Coin_Thr%02d' % n,title='MuonRoI Eta vs Phi wBW3Coin Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi,type='TH2F')

        myGroup.defineHistogram('roi_eta_wInnCoinVeto,roi_phi_wInnCoinVeto;MuonRoI_EtaVsPhi_wInnCoinVeto_Thr%02d' % n,title='MuonRoI Eta vs Phi wInnCoinVeto Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi,type='TH2F')
        myGroup.defineHistogram('roi_eta_wBW3CoinVeto,roi_phi_wBW3CoinVeto;MuonRoI_EtaVsPhi_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Eta vs Phi wBW3CoinVeto Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi,type='TH2F')

        myGroup.defineHistogram('roi_eta;MuonRoI_Eta_Thr%02d' % n,title='MuonRoI Eta Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_eta_rpc;MuonRoI_Eta_RPC_Thr%02d' % n,title='MuonRoI Eta RPC Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_eta_tgc;MuonRoI_Eta_TGC_Thr%02d' % n,title='MuonRoI Eta TGC Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_phi_rpc;MuonRoI_Phi_RPC_Thr%02d' % n,title='MuonRoI Phi RPC Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=32,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_phi_tgc;MuonRoI_Phi_TGC_Thr%02d' % n,title='MuonRoI Phi TGC Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_Thr%02d' % n,type='TH2F',title='MuonRoI Eta vs Phi Thr%02d;MuonRoI Eta;MuonRoI Phi' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)

        myGroup.defineHistogram('roi_inncoin,roi_eta_tgc;MuonRoI_Eff_Eta_wInnCoin_Thr%02d' % n,title='MuonRoI Eff Eta wInnCoin Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_inncoin,roi_phi_tgc;MuonRoI_Eff_Phi_wInnCoin_Thr%02d' % n,title='MuonRoI Eff Phi wInnCoin Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_inncoin,roi_eta_tgc,roi_phi_tgc;MuonRoI_Eff_EtaVsPhi_wInnCoin_Thr%02d' % n,title='MuonRoI Eff Eta vs Phi wInnCoin Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)

        myGroup.defineHistogram('roi_innveto,roi_eta_tgc;MuonRoI_Eff_Eta_wInnCoinVeto_Thr%02d' % n,title='MuonRoI Eff Eta wInnCoinVeto Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_innveto,roi_phi_tgc;MuonRoI_Eff_Phi_wInnCoinVeto_Thr%02d' % n,title='MuonRoI Eff Phi wInnCoinVeto Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_innveto,roi_eta_tgc,roi_phi_tgc;MuonRoI_Eff_EtaVsPhi_wInnCoinVeto_Thr%02d' % n,title='MuonRoI Eff Eta vs Phi wInnCoinVeto Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)

        myGroup.defineHistogram('roi_bw3coin,roi_eta_tgc;MuonRoI_Eff_Eta_wBW3Coin_Thr%02d' % n,title='MuonRoI Eff Eta wBW3Coin Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_bw3coin,roi_phi_tgc;MuonRoI_Eff_Phi_wBW3Coin_Thr%02d' % n,title='MuonRoI Eff Phi wBW3Coin Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_bw3coin,roi_eta_tgc,roi_phi_tgc;MuonRoI_Eff_EtaVsPhi_wBW3Coin_Thr%02d' % n,title='MuonRoI Eff Eta vs Phi wBW3Coin Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)

        myGroup.defineHistogram('roi_bw3coinveto,roi_eta_tgc;MuonRoI_Eff_Eta_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Eff Eta wBW3CoinVeto Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_bw3coinveto,roi_phi_tgc;MuonRoI_Eff_Phi_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Eff Phi wBW3CoinVeto Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_bw3coinveto,roi_eta_tgc,roi_phi_tgc;MuonRoI_Eff_EtaVsPhi_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Eff Eta vs Phi wBW3CoinVeto Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)


        myGroup.defineHistogram('muon_l1passThr%d,muon_pt_rpc;MuonRoI_Eff_Pt_RPC_Thr%02d' % (n,n),title='MuonRoI_Eff_Pt_RPC_Thr%02d;Offline muon pT [GeV];Efficiency' % n,
                                type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
        myGroup.defineHistogram('muon_l1passThr%d,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_Thr%02d' % (n,n),title='MuonRoI_Eff_Pt_TGC_Thr%02d;Offline muon pT [GeV];Efficiency' % n,
                                type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
        myGroup.defineHistogram('muon_l1passThr%d,muon_phi_rpc;MuonRoI_Eff_Phi_RPC_Thr%02d' % (n,n),title='MuonRoI_Eff_Phi_RPC_Thr%02d;Offline muon phi [rad.];Efficiency' % n,
                                type='TEfficiency',path=trigPath,xbins=32,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('muon_l1passThr%d,muon_phi_tgc;MuonRoI_Eff_Phi_TGC_Thr%02d' % (n,n),title='MuonRoI_Eff_Phi_TGC_Thr%02d;Offline muon phi [rad.];Efficiency' % n,
                                type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('muon_l1passThr%d,muon_eta;MuonRoI_Eff_Eta_Thr%02d' % (n,n),title='MuonRoI_Eff_Eta_Thr%02d;Offline muon eta;Efficiency' % n,
                                type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('muon_l1passThr%d,muon_eta,muon_phi;MuonRoI_Eff_EtaVsPhi_Thr%02d' % (n,n),title='MuonRoI_Eff_EtaVsPhi_Thr%02d;Offline muon eta; Offline muon phi' % n,
                                type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
        

    hitPath = 'Hit/'
    myGroup.defineHistogram('hit_n;TgcPrd_nHits',title='TgcPrd_nHits;Number of hits;Number of events',
                            path=hitPath,xbins=100,xmin=0,xmax=1000,opt='kAddBinsDynamically')
    myGroup.defineHistogram('hit_bunch;TgcPrd_Timing',title='TgcPrd_Timing;Timing;Number of events',
                            path=hitPath,xbins=4,xmin=-1.5,xmax=1.5,xlabels=['Previous','Current','Next'])

    for side in ['A', 'C']:# side-A or side-C
        for station in range(1,5):# M1,2,3,4
            for s_or_w in ['S','W']:# strip or wire
                name = "%sM%02i%s" % (side,station,s_or_w)
                x_name = "lb_for_%s" % (name)
                y_name = name
                nbins = 10
                if station==1:
                    nbins = 648
                elif station==2 or station==3:
                    nbins = 528
                else: # station==4
                    nbins = 90

                myGroup.defineHistogram(x_name+','+y_name+';'+y_name+'_vs_lb',
                                        title=y_name+'_vs_lb;Luminosity block;Chamber index',type='TH2F',
                                        path=hitPath,xbins=100,xmin=-0.5,xmax=99.5,
                                        ybins=nbins,ymin=0.5,ymax=nbins+0.5,opt='kAddBinsDynamically')
                x_name = name
                y_name = "timing_for_%s" % (name)
                myGroup.defineHistogram(x_name+','+y_name+';'+y_name,
                                        title='Timing_for_'+name+';Chamber index;Timing',type='TH2F',
                                        path=hitPath,xbins=nbins,xmin=0.5,xmax=nbins+0.5,
                                        ybins=3,ymin=-1.5,ymax=1.5,ylabels=['Previous','Current','Next'])

                x_name = "x_%s" % (name)
                y_name = "y_%s" % (name)
                nbinsx = 10
                nbinsy = 10
                if station==1:
                    nbinsx = 15
                    nbinsy = 48
                elif station==2 or station==3:
                    nbinsx = 12
                    nbinsy = 48
                else: # station==4
                    nbinsx = 4
                    nbinsy = 24

                myGroup.defineHistogram(x_name+','+y_name+';'+name+'_vs_iEta',
                                        title=name+'_vs_iEta;iEta;Chamber index',type='TH2F',path=hitPath,
                                        xbins=nbinsx,xmin=0.5,xmax=nbinsx+0.5,
                                        ybins=nbinsy,ymin=0.5,ymax=nbinsy+0.5)


    coinPath = 'Coin/'

    myGroup.defineHistogram('nTgcCoinDetElementIsNull;h_nTgcCoinDetElementIsNull',title='nTgcCoinDetElementIsNull',
                            path=coinPath,xbins=101,xmin=-0.5,xmax=100.5)
    myGroup.defineHistogram('nTgcCoinPostOutPtrIsNull;h_nTgcCoinPostOutPtrIsNull',title='nTgcCoinPostOutPtrIsNull',
                            path=coinPath,xbins=101,xmin=-0.5,xmax=100.5)

    for coinType in ['SL','HPT','LPT']:
        for region in ['','Endcap','Forward']:
            suffix0 = coinType if region == '' else coinType+'_'+region
            nrois = 148 if region == 'Endcap' else 64
            nsectors = 48 if region == 'Endcap' else 24
            for chanType in ['','Wire','Strip']:
                if coinType == 'SL' and chanType != '': continue # no wire or strip for "SL"
                if coinType == 'HPT' and chanType == '': continue # always wire or strip for "HPT"
                if coinType == 'LPT' and chanType == '': continue # always wire or strip for "LPT"
                suffix = suffix0+'_' if chanType == '' else suffix0+'_'+chanType+'_'

                if coinType == 'SL':
                    for thr in range(1,16): #1.2...15
                        cut = suffix+"coin_cutmask_pt"+str(thr)
                        PT = "_Thr%02d" % thr
                        if region == '':
                            myGroup.defineHistogram(suffix+'coin_eta,'+suffix+'coin_phi;'+suffix+'TgcCoin_Eta2Phi'+PT,
                                                    title=suffix+'TgcCoin_Eta2Phi'+PT+';Eta;Phi',
                                                    type='TH2F',path=coinPath,cutmask=cut,
                                                    xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
                        else:
                            myGroup.defineHistogram(suffix+'coin_roi,'+suffix+'coin_sector;'+suffix+'TgcCoin_Roi2Sector'+PT,
                                                    title=suffix+'TgcCoin_Roi2Sector'+PT+';RoI;Trigger Sector',
                                                    type='TH2F',path=coinPath,cutmask=cut,
                                                    xbins=nrois,xmin=-0.5,xmax=nrois-0.5,
                                                    ybins=nsectors*2+1,ymin=-nsectors-0.5,ymax=nsectors+0.5)

                if region == '':
                    myGroup.defineHistogram(suffix+'coin_eta,'+suffix+'coin_phi;'+suffix+'TgcCoin_Eta2Phi',
                                            title=suffix+'TgcCoin_Eta2Phi;Eta;Phi',
                                            type='TH2F',path=coinPath,
                                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
                else:
                    myGroup.defineHistogram(suffix+'coin_sector,'+suffix+'coin_bunch;'+suffix+'TgcCoin_Sector2Timing',
                                            title=suffix+'TgcCoin_Sector2Timing;Trigger Sector;Timing',
                                            type='TH2F',path=coinPath,
                                            xbins=nsectors*2+1,xmin=-nsectors-0.5,xmax=nsectors+0.5,
                                            ybins=3,ymin=-1.5,ymax=1.5,ylabels=['Previous','Current','Next'])
                    myGroup.defineHistogram(suffix+'coin_lb,'+suffix+'coin_sector;'+suffix+'TgcCoin_LumiBlock2Sector',
                                            title=suffix+'TgcCoin_LumiBlock2Sector;LumiBlock;Trigger Sector',
                                            type='TH2F',path=coinPath,
                                            xbins=100,xmin=0.5,xmax=100.5,opt='kAddBinsDynamically',
                                            ybins=nsectors*2+1,ymin=-nsectors-0.5,ymax=nsectors+0.5)
                    if coinType == 'SL':
                        myGroup.defineHistogram(suffix+'coin_roi,'+suffix+'coin_sector;'+suffix+'TgcCoin_Roi2Sector',
                                                title=suffix+'TgcCoin_Roi2Sector;RoI;Trigger Sector',
                                                type='TH2F',path=coinPath,
                                                xbins=nrois,xmin=-0.5,xmax=nrois-0.5,
                                                ybins=nsectors*2+1,ymin=-nsectors-0.5,ymax=nsectors+0.5)
                        myGroup.defineHistogram(suffix+'coin_veto_roi,'+suffix+'coin_veto_sector;'+suffix+'TgcCoin_Roi2Sector_wVeto',
                                                title=suffix+'TgcCoin_Roi2Sector_wVeto;RoI;Trigger Sector',
                                                type='TH2F',path=coinPath,
                                                xbins=nrois,xmin=-0.5,xmax=nrois-0.5,
                                                ybins=nsectors*2+1,ymin=-nsectors-0.5,ymax=nsectors+0.5)
                        myGroup.defineHistogram(suffix+'coin_sector,'+suffix+'coin_pt;'+suffix+'TgcCoin_Sector2Threshold',
                                                title=suffix+'TgcCoin_Sector2Threshold;Trigger Sector;Threshold',
                                                type='TH2F',path=coinPath,
                                                xbins=nsectors*2+1,xmin=-nsectors-0.5,xmax=nsectors+0.5,
                                                ybins=15,ymin=0.5,ymax=15.5)
                        myGroup.defineHistogram(suffix+'coin_sector,'+suffix+'coin_isPositiveDeltaR;'+suffix+'TgcCoin_Sector2isPositiveDeltaR',
                                                title=suffix+'TgcCoin_Sector2isPositiveDeltaR;Trigger Sector;isPositiveDeltaR',
                                                type='TH2F',path=coinPath,
                                                xbins=nsectors*2+1,xmin=-nsectors-0.5,xmax=nsectors+0.5,
                                                ybins=2,ymin=-0.5,ymax=1.5,ylabels=['Negative','Positive'])

    hitDetailsPath = 'HitDetails/'
    for side in ['A', 'C']:# side-A or side-C
        for sector in range(0,16):# Sector 00...15 (00 for FI, 01..15 for EI, 01..12 for BW)
            for station in range(1,5):# M1,2,3,4
                for phi in range(25):# internal phi 0,1,2,3...24 (0..3 for BW and EI, 1..24 for FI)
                    for eta in range(6):# eta index 1,,,5 for Endcap, and 0 for Forward
                        for lay in range(1,4):# sub-layer 1,2,3 (triplet) or 1,2 (doublet)
                            if station<4 and (sector>12 or sector==0):continue # BW only 1..12 sectors
                            if station==4 and eta==0 and sector!=0:continue # FI only sector-0
                            if station==4 and eta==1 and sector%2==0:continue # EI only odd-sectors (1,3,5,7..15)
                            if station==4 and eta==0 and phi==0:continue # FI only 1..24 internal phi
                            if (station<4 or (station==4 and eta==1)) and phi>3: continue # BW and EI only 0..3 internal phi
                            if station==1 and eta==5:continue # BW M1 has only 1..4 eta
                            if station==4 and eta>1:continue # EI/FI eta 0 or 1
                            chamber_name = "%s%02dM%02df%02d%s%02dL%02d" % (side,sector,station,phi,'F' if eta==0 else 'E',eta,lay)
                            for s_or_w in ['S','W']:# strip or wire
                                nbins = 100
                                if s_or_w=="S":
                                    nbins = 32
                                else:
                                    if station==1:
                                        if eta==1:     nbins = 24
                                        elif eta==2:   nbins = 23
                                        elif eta==3:
                                            if lay==1: nbins = 61
                                            else:      nbins = 62
                                        elif eta==4:
                                            if lay==1: nbins = 92
                                            else:      nbins = 91
                                        else: # forward
                                            if lay==2: nbins = 104
                                            else:      nbins = 105
                                    elif station==2:
                                        if eta==1 or eta==2 or eta==3: nbins = 32
                                        elif eta==4:                   nbins = 103
                                        elif eta==5:                   nbins = 110
                                        else:                          nbins = 125 # forward
                                    elif station==3:
                                        if eta==1:   nbins = 31
                                        elif eta==2: nbins = 30
                                        elif eta==3: nbins = 32
                                        elif eta==4: nbins = 106
                                        elif eta==5: nbins = 96
                                        else:        nbins = 122
                                    else: # EI/FI
                                        if eta==1:   nbins = 24 # EI
                                        else:        nbins = 32 # FI

                                myGroup.defineHistogram('hits_on_'+chamber_name+s_or_w,
                                                        title='Hits_on_'+chamber_name+s_or_w+";Channel ID;Number of events",
                                                        path=hitDetailsPath,xbins=nbins,xmin=0.5,xmax=nbins+0.5)


    acc = helper.result()
    result.merge(acc)
    return result
    
if __name__=='__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO,DEBUG
    log.setLevel(INFO)

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    import glob

    inputs = glob.glob('/data01/L1MuonSimulation/aaron/runs3_zmumu/*/AOD.pool.root')

    ConfigFlags.Input.Files = inputs
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'

    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    tgcRawDataMonitorAcc = TgcRawDataMonitoringConfig(ConfigFlags)
    tgcRawDataMonitorAcc.OutputLevel = DEBUG
    cfg.merge(tgcRawDataMonitorAcc)
    cfg.getEventAlgo('TgcRawDataMonAlg').OutputLevel = INFO

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    cfg.merge(MagneticFieldSvcCfg(ConfigFlags))
    cfg.merge(AtlasGeometryCfg(ConfigFlags))
    cfg.merge(TrackingGeometrySvcCfg(ConfigFlags))

    cfg.printConfig(withDetails=True, summariseProps = True)

    cfg.run()
