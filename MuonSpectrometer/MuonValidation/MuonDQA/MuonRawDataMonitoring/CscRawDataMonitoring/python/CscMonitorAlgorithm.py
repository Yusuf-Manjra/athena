#
#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from MuonConfig.MuonSegmentFindingConfig import CalibCscStripFitterCfg

from .CscMonUtils import getCSCLabelx

def CscMonitoringConfigOld(inputFlags):
    from AthenaMonitoring import AthMonitorCfgHelperOld
    helper = AthMonitorCfgHelperOld(inputFlags,'CscAthMonitorCfg')
    return helper.result()

def CscMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # If you need to set up special tools, etc., you will need your own ComponentAccumulator;
    # uncomment the following 2 lines and use the last three lines of this function instead of the ones
    # just before
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))


       # Temporary, until we move to services/private tools-- from MuonSpectrometer/MuonConfig
   # result.addPublicTool( CompFactory.Muon__MuonIdHelperTool() )
    strp = CalibCscStripFitterCfg(inputFlags)
    strp.popPrivateTools()
    result.merge(strp)

   # CalibCscStripFitter = CompFactory.CalibCscStripFitter
  #  result.addPublicTool(CompFactory.Calib__CalibCscStripFitterTool())
   # res = CalibCscStripFitter()
   # result.addPublicTool(CompFactory.CalibCscStripFitter())
    


    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'CscAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    # This uses the new Configurables object system.
    cscClusMonAlg = helper.addAlgorithm(CompFactory.CscClusterValMonAlg,'CscClusMonAlg')
    cscPrdMonAlg = helper.addAlgorithm(CompFactory.CscPrdValMonAlg,'CscPrdMonAlg')
   # cscSegmMonAlg = helper.addAlgorithm(CompFactory.CscSegmValMonAlg,'CscSegmValMonAlg')


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # exampleMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    #exampleMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # exampleMonAlg.MyDomainTool = MyDomainTool()
    
 
    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    cscClusGroup = helper.addGroup(cscClusMonAlg,'CscClusMonitor','Muon/MuonRawDataMonitoring/CSC/')
    cscPrdGroup = helper.addGroup(cscPrdMonAlg,'CscPrdMonitor','Muon/MuonRawDataMonitoring/CSC/')
  #  cscSegmGroup = helper.addGroup(cscSegmMonAlg,'CscSegmMonitor','Muon/MuonRawDataMonitoring/CSC/')



    ### STEP 5 ###
    # Configure histograms
    #Cluster
    cscClusGroup.defineHistogram('z,r;h2csc_clus_r_vs_z_hitmap',type='TH2F',title='R vs. Z Cluster hitmap;z(mm);R(mm)',
                                 path='Clusters/Shift',xbins=200,xmin=-10000.,xmax=10000., ybins=40, ymin=0., ymax=4000.)

    cscClusGroup.defineHistogram('y,x;h2csc_clus_y_vs_x_hitmap',type='TH2F',title='X vs. Y Cluster hitmap;y(mm);x(mm)',
                                 path='Clusters/Shift',xbins=100,xmin=-5000.,xmax=5000.,ybins=100,ymin=-5000,ymax=5000)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_phicluswidth',type='TH2F',cutmask='clus_phi',title='Phi-Cluster width;# strips;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=48,xmin=0,xmax=48,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_etacluswidth',type='TH2F',cutmask='clus_eta',title='Eta-Cluster width;# strips;[sector] + [0.2 #times layer]',
                                  path='Clusters/Expert',xbins=192, xmin=0, xmax=192, ybins=175, ymin=-17, ymax=18)

    cscClusGroup.defineHistogram('stripid,secLayer;h2csc_clus_hitmap',type='TH2F',title='Cluster occupancy;channel;[sector]+[0.2 #times layer]',
                                  path='Clusters/Expert',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('fStripIDs_col,secLayer;h2csc_clus_hitmap_signal',cutmask='signal_mon',type='TH2F',title='Cluster occupancy, Qmax > 100 counts;channel;[sector] + [0.2 #times layer]',
                                  path='Clusters/Shift',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_phicluswidth_signal',type='TH2F',cutmask='clus_phiSig',title='#phi-cluster width, Qmax > 100 counts;# strips;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=48,xmin=0,xmax=48,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_etacluswidth_signal',type='TH2F',cutmask='clus_etaSig',title='#eta-cluster width, Qmax > 100 counts;# strips;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=192,xmin=0,xmax=192,ybins=175,ymin=-17,ymax=18)

    thisLabelx=getCSCLabelx("labels_clus_occupancy_signal_EA")
    cscClusGroup.defineHistogram('secLayer;h1csc_clus_occupancy_signal_EA',type='TH1F',cutmask='sideA',title='EndCap A: Layer occupancy, Qmax > 100 counts;;entries/layer',
                                 path='Overview/CSCEA/Cluster',xbins=90,xmin=0,xmax=18, xlabels=thisLabelx)

    thisLabelx=getCSCLabelx("labels_clus_occupancy_signal_EC")
    cscClusGroup.defineHistogram('secLayer;h1csc_clus_occupancy_signal_EC',type='TH1F',cutmask='sideC',title='EndCap C: Layer occupancy, Qmax > 100 counts;;entries/layer',
                                 path='Overview/CSCEC/Cluster',xbins=85,xmin=-17.,xmax=0., xlabels=thisLabelx) 

    cscClusGroup.defineHistogram('fStripIDs_col,secLayer;h2csc_clus_hitmap_noise',cutmask='noise_mon',type='TH2F',title='Cluster occupancy, Qmax #leq 100 counts;channel;[sector] + [0.2 #times layer]',
                                  path='Clusters/Expert',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_phicluswidth_noise',type='TH2F',cutmask='clus_phiNoise',title='#phi-cluster width, Qmax #leq 100 counts;# strips;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=48,xmin=0,xmax=48,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('noStrips,secLayer;h2csc_clus_etacluswidth_noise',type='TH2F',cutmask='clus_etaNoise',title='#eta-cluster width, Qmax #leq 100 counts;# strips;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=192,xmin=0,xmax=192,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC,secLayer;h2csc_clus_qmax',type='TH2F',title='Cluster peak-strip charge, Qmax;counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC,secLayer;h2csc_clus_qmax_signal',cutmask='signal_mon',type='TH2F',title='Cluster peak-strip charge, Qmax > 100 counts;counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Shift',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC,secLayer;h2csc_clus_qmax_noise',cutmask='noise_mon',type='TH2F',title='Cluster peak-strip charge, Qmax #leq 100 counts;counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC,secLayer;h2csc_clus_qmax_signal_EA',cutmask='sideA',type='TH2F',title='EndCap A: Cluster peak-strip charge, Qmax > 100 counts;counts;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEA/Cluster',xbins=400,xmin=0,xmax=8000,ybins=90,ymin=0,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC;h1csc_clus_qmax_signal_EA_count',cutmask='sideA',type='TH1F',title='EndCap A: Cluster peak-strip charge, Qmax > 100 counts;counts;entries/20 counts;',
                                 path='Overview/CSCEA/Cluster',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('QmaxADC,secLayer;h2csc_clus_qmax_signal_EC',cutmask='sideC',type='TH2F',title='EndCap C: Cluster peak-strip charge, Qmax > 100 counts;counts;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEC/Cluster',xbins=400,xmin=0,xmax=8000,ybins=90,ymin=0,ymax=18)

    cscClusGroup.defineHistogram('QmaxADC;h1csc_clus_qmax_signal_EC_count',cutmask='sideC',type='TH1F',title='EndCap C: Cluster peak-strip charge, Qmax > 100 counts;counts;entries/20 counts;',
                                 path='Overview/CSCEC/Cluster',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('QsumADC,secLayer;h2csc_clus_qsum',type='TH2F',title='Cluster charge (Qsum);counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QsumADC,secLayer;h2csc_clus_qsum_signal',cutmask='signal_mon',type='TH2F',title='Cluster charge(Qsum), Qmax > 100 counts;counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Shift',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('QsumADC,secLayer;h2csc_clus_qsum_signal_EA',cutmask='sideA',type='TH2F',title='EndCap A: Cluster charge(Qsum), Qmax > 100 counts;counts;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEA/Cluster',xbins=400,xmin=0,xmax=8000,ybins=90,ymin=0,ymax=18)

    cscClusGroup.defineHistogram('QsumADC;h1csc_clus_qsum_signal_EA_count',cutmask='sideA',type='TH1F',title='EndCap A: Cluster charge(Qsum), Qmax > 100 counts;counts;entries/20 counts;',
                                 path='Overview/CSCEA/Cluster',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('QsumADC;h2csc_clus_qsum_signal_EC',cutmask='sideC',type='TH1F',title='EndCap C: Cluster charge(Qsum), Qmax > 100 counts;counts;[sector] + [0.2 #times layer]',
                                 path='Overview/CSCEC/Cluster',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('QsumADC;h1csc_clus_qsum_signal_EC_count',cutmask='sideC',type='TH1F',title='EndCap C: Cluster charge(Qsum), Qmax > 100 counts;counts;entries/20 counts;',
                                 path='Overview/CSCEC/Cluster',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('QsumADC,secLayer;h2csc_clus_qsum_noise',cutmask='noise_mon',type='TH2F',title='Cluster charge(Qsum), Qmax #leq 100 counts;counts;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000,ybins=175,ymin=-17,ymax=18)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_transverse_time',cutmask='clus_phi',type='TH1F',title='#phi-cluster sampling time;ns;entries/ns',
                                 path='Clusters/Expert',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_transverse_charge',cutmask='clus_phi',type='TH1F',title='#phi-cluster charge;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_transverse_time_signal',cutmask='clus_phiSig',type='TH1F',title='#phi-cluster sampling time, Qmax > 100 counts;ns;entries/ns',
                                 path='Clusters/Expert',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_transverse_charge_signal',cutmask='clus_phiSig',type='TH1F',title='#phi-cluster charge, Qmax > 100 counts;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_transverse_time_noise',cutmask='clus_phiNoise',type='TH1F',title='#phi-cluster sampling time, Qmax #leq 100 counts;ns;entries/ns',
                                 path='Clusters/Expert',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_transverse_charge_noise',cutmask='clus_phiNoise',type='TH1F',title='#phi-cluster charge, Qmax #leq 100 counts;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_precision_time',cutmask='clus_eta',type='TH1F',title='#eta-cluster sampling time;ns;entries/ns',
                                 path='Clusters/Expert',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_precision_charge',cutmask='clus_eta',type='TH1F',title='eta-cluster charge;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_precision_time_signal',cutmask='clus_etaSig',type='TH1F',title='#eta-cluster sampling time, Qmax > 100 counts;ns;entries/ns',
                                 path='Clusters/Shift',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_precision_charge_signal',cutmask='clus_etaSig',type='TH1F',title='#eta-cluster charge, Qmax > 100 counts;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_precision_time_signal_EA',cutmask='sideA',type='TH1F',title='EndCap A: #eta-cluster sampling time, Qmax > 100 counts;ns;entries/ns',
                                 path='Overview/CSCEA/Cluster',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_precision_time_signal_EC',cutmask='sideC',type='TH1F',title='EndCap C: #eta-cluster sampling time, Qmax > 100 counts;ns;entries/ns',
                                 path='Overview/CSCEC/Cluster',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_time;h1csc_clus_precision_time_noise',cutmask='clus_etaNoise',type='TH1F',title='#eta-cluster sampling time, Qmax #leq 100 counts;ns;entries/ns',
                                 path='Clusters/Expert',xbins=260,xmin=-60,xmax=200)

    cscClusGroup.defineHistogram('clu_charge_kiloele;h1csc_clus_precision_charge_noise',cutmask='clus_etaNoise',type='TH1F',title='#eta-cluster charge, Qmax #leq 100 counts;counts;entries/count',
                                 path='Clusters/Expert',xbins=400,xmin=0,xmax=8000)

    cscClusGroup.defineHistogram('stripsSum_EA_mon;h1csc_clus_totalWidth_EA',type='TH1F',title='EndCap A: Cluster hits in all EA eta(#eta) & phi(#phi) strips;strips;cluster hits',
                                 path='Overview/CSCEA/Cluster',xbins=15360,xmin=1.,xmax=15361.)

    cscClusGroup.defineHistogram('stripsSum_EC_mon;h1csc_clus_totalWidth_EC',type='TH1F',title='EndCap C: Cluster hits in all EC eta(#eta) & phi(#phi) strips;strips;cluster hits',
                                 path='Overview/CSCEC/Cluster',xbins=15360,xmin=1.,xmax=15361.)

    cscClusGroup.defineHistogram('nPhiClusWidthCnt_mon,nEtaClusWidthCnt_mon;h2csc_clus_eta_vs_phi_cluswidth',type='TH2F',title='Eta vs. Phi Cluster width correlation;#varphi-cluster width;#eta-cluster width',
                                 path='Clusters/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0,ymax=100)

    cscClusGroup.defineHistogram('count_mon,secLayer;h2csc_clus_phicluscount',cutmask='mphi_true',type='TH2F',title='#phi-cluster count;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('scount_mon,secLayer;h2csc_clus_phicluscount_signal',cutmask='scount_phi_true',type='TH2F',title='#phi-cluster count;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('count_diff,secLayer;h2csc_clus_phicluscount_noise',cutmask='scount_phi_false',type='TH2F',title='#phi-cluster count, Qmax #leq 100 counts;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('count_mon,secLayer;h2csc_clus_etacluscount',cutmask='mphi_false',type='TH2F',title='#eta-cluster count;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('scount_mon,secLayer;h2csc_clus_etacluscount_signal',cutmask='scount_eta_true',type='TH2F',title='#eta-cluster count;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('count_diff,secLayer;h2csc_clus_etacluscount_noise',cutmask='scount_eta_false',type='TH2F',title='#eta-cluster count, Qmax #leq 100 counts;# clusters;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=20,xmin=0,xmax=20,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('segNum_mon,sec_mon;h2csc_clus_segmap_signal',type='TH2F',title='Segment occupancy, Qmax > 100 counts;segment;[sector] + [0.2 #times layer]',
                                 path='Clusters/Expert',xbins=16,xmin=-0.5,xmax=15.5,ybins=175,ymin=-17.,ymax=18.)

    cscClusGroup.defineHistogram('numphi_numeta_mon;h1csc_clus_count',type='TH1F',title='Clusters per event;no.of clusters;entries',
                                 path='Clusters/Expert',xbins=26,xmin=-1,xmax=25)

    cscClusGroup.defineHistogram('numphi_numeta_sig_mon;h1csc_clus_count_signal',type='TH1F',title='Clusters per event, Qmax > 100 counts;no.of clusters;entries',
                                 path='Clusters/Expert',xbins=26,xmin=-1,xmax=25)

    cscClusGroup.defineHistogram('num_num_noise_mon;h1csc_clus_count_noise',type='TH1F',title='Clusters per event, Qmax #leq 100 counts;no.of clusters;entries',
                                 path='Clusters/Expert',xbins=26,xmin=-1,xmax=25)

    cscClusGroup.defineHistogram('numphi_mon,numeta_mon;h2csc_clus_eta_vs_phi_cluscount',type='TH2F',title='Eta vs. Phi Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                 path='Clusters/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0,ymax=100)

    cscClusGroup.defineHistogram('numphi_sig_mon,numeta_sig_mon;h2csc_clus_eta_vs_phi_cluscount_signal',type='TH2F',title='Eta vs. Phi Signal-Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                 path='Clusters/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0,ymax=100)

    cscClusGroup.defineHistogram('numphi_diff_mon,numeta_diff_mon;h2csc_clus_eta_vs_phi_cluscount_noise',type='TH2F',title='Eta vs. Phi Noise-Cluster count correlation;#varphi-cluster count;#eta-cluster count',
                                 path='Clusters/Expert',xbins=100,xmin=0,xmax=100,ybins=100,ymin=0,ymax=100)

    #PRD
    cscPrdGroup.defineHistogram('spid, secLayer;h2csc_prd_hitmap', type='TH2F', title='Hit Occupancy; channel; [sector] + [0.2 #times layer]',
                                 path='PRD/Expert',xbins=242,xmin=-49.,xmax=193.,ybins=175,ymin=-17.,ymax=18.)

    cscPrdGroup.defineHistogram('noStrips,secLayer;h2csc_prd_phicluswidth',type='TH2F',cutmask='measphi',title='PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                 path='PRD/Expert',xbins=48,xmin=0,xmax=48, ybins=175,ymin=-17.,ymax=18.)

    cscPrdGroup.defineHistogram('noStrips,secLayer;h2csc_prd_etacluswidth',type='TH2F',cutmask='measeta',title='PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]',
                                 path='PRD/Expert',xbins=192,xmin=0,xmax=192, ybins=175,ymin=-17.,ymax=18.)
    #maybe no needed
    cscPrdGroup.defineHistogram('z,r;h2csc_prd_r_vs_z_hitmap',type='TH2F',title='R vs. Z Cluster hitmap;z(mm);R(mm)',
                                path='PRD/Shift',xbins=200,xmin=-10000.,xmax=10000., ybins=40, ymin=0., ymax=4000.) 

    cscPrdGroup.defineHistogram('y,x;h2csc_prd_y_vs_x_hitmap',type='TH2F',title='Y vs. X Cluster hitmap;x(mm);y(mm)',
                                path='PRD/Shift',xbins=100,xmin=-5000.,xmax=5000., ybins=100, ymin=-5000., ymax=5000.) 

    #myGroup.defineHistogram('lb', title='Luminosity Block;lb;Events',
    #                       path='ToFindThem',xbins=1000,xmin=-0.5,xmax=999.5,weight='testweight')
    #myGroup.defineHistogram('random', title='LB;x;Events',
    #                        path='ToBringThemAll',xbins=30,xmin=0,xmax=1,opt='kLBNHistoryDepth=10')
    #myGroup.defineHistogram('random', title='title;x;y',path='ToBringThemAll',
    #                        xbins=[0,.1,.2,.4,.8,1.6])
    ##myGroup.defineHistogram('random,pT', type='TH2F', title='title;x;y',path='ToBringThemAll',
     #                       xbins=[0,.1,.2,.4,.8,1.6],ybins=[0,10,30,40,60,70,90])
    # TEfficiencies
    ##myGroup.defineHistogram('pT_passed,pT', type='TEfficiency', title='Test TEfficiency;x;Eff',
    #                        path='AndInTheDarkness', xbins=100, xmin=0.0, xmax=50.0)
    #myGroup.defineHistogram('pT_passed,pT,random', type='TEfficiency', title='Test TEfficiency 2D;x;y;Eff',
    #                        path='AndInTheDarkness', xbins=100, xmin=0.0, xmax=50.0,
    #                        ybins=10, ymin=0.0, ymax=2.0)
    # # use a cutmask to only fill certain events
    #myGroup.defineHistogram('pT;pT_with_cut', title='p_{T};p_{T};Events', path='AndInTheDarkness',
    #                         xbins=50, xmin=0, xmax=50, cutmask='pT_passed')
    # make a TTree
    #myGroup.defineTree('pT,lb,pT_vec,strvec,str;testtree', path='BindThem',
    #                    treedef='pT/F:lb/i:pT_vec/vector<float>:strvec/vector<string>:str/string')

    #anotherGroup.defineHistogram('lbWithFilter',title='Lumi;lb;Events',
    #                             path='top',xbins=1000,xmin=-0.5,xmax=999.5)
    #anotherGroup.defineHistogram('run',title='Run Number;run;Events',
    #                             path='top',xbins=1000000,xmin=-0.5,xmax=999999.5)

    # Example defining an array of histograms. This is useful if one seeks to create a
    # number of histograms in an organized manner. (For instance, one plot for each ASIC
    # in the subdetector, and these components are mapped in eta, phi, and layer.) Thus,
    # one might have an array of TH1's such as quantity[etaIndex][phiIndex][layerIndex].
   # for alg in [exampleMonAlg,anotherExampleMonAlg]:
        # Using an array of groups
    #    topPath = 'OneRing' if alg == exampleMonAlg else ''
    #    array = helper.addArray([2],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('a,b',title='AB',type='TH2F',path='Eta',
    #                          xbins=10,xmin=0.0,xmax=10.0,
    #                          ybins=10,ymin=0.0,ymax=10.0)
    #    array.defineHistogram('c',title='C',path='Eta',
     #                         xbins=10,xmin=0.0,xmax=10.0)
    #    array = helper.addArray([4,2],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('a',title='A',path='EtaPhi',
     #                         xbins=10,xmin=0.0,xmax=10.0)
        # Using a map of groups
    #    layerList = ['layer1','layer2']
    #    clusterList = ['clusterX','clusterB']
    #    array = helper.addArray([layerList],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('c',title='C',path='Layer',
     #                         xbins=10,xmin=0,xmax=10.0)
    #    array = helper.addArray([layerList,clusterList],alg,'ExampleMonitor', topPath=topPath)
    #    array.defineHistogram('c',title='C',path='LayerCluster',
     #                         xbins=10,xmin=0,xmax=10.0)

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    #return helper.result()
    
    # # Otherwise, merge with result object and return
    acc = helper.result()
    result.merge(acc)
    return result

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD


    # ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'CscMonitorOutput.root'
    #ConfigFlags.fillFromArgs(sys.argv[1:])
    # ConfigFlags.GeoModel.AtlasVersion="ATLAS-R2-2016-00-01-00"
    ConfigFlags.Muon.doCSCs = True
    ConfigFlags.Muon.doRPCs = False
    ConfigFlags.Muon.doTGCs = False
    ConfigFlags.Detector.GeometryMuon=False
    ConfigFlags.Detector.GeometryCSC=True
    ConfigFlags.Detector.GeometryRPC=False
    ConfigFlags.Detector.GeometryTGC=False
    ConfigFlags.Detector.GeometryMM=False
    ConfigFlags.Detector.GeometryMDT=False
    ConfigFlags.Muon.doMicromegas = False
    #ConfigFlags.Muon.useAlignmentCorrections=False
    ConfigFlags.Muon.Align.UseILines = False
    #ConfigFlags.Muon.Align.UseAsBuilt = True
    ConfigFlags.Muon.Align.UseALines = False
    ConfigFlags.Muon.Align.UseBLines = False


    ConfigFlags.lock()
    ConfigFlags.dump()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    cscMonitorAcc = CscMonitoringConfig(ConfigFlags)
    cfg.merge(cscMonitorAcc)

    # If you want to turn on more detailed messages ...
    # exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run(20) #use cfg.run(20) to only run on first 20 events
