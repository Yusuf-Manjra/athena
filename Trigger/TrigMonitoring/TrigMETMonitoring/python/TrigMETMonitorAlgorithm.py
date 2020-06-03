#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigMETMonitoringAlgorithm.py
@author K. Hamano
@author G. Gallardo
@date 2019-05-13
@brief MET Trigger python configuration for Run 3 AthenaMonitoring package

'''


def TrigMETMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigMETAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    #The added algorithm must exist as a .h file 

    from AthenaConfiguration.ComponentFactory import CompFactory
    TrigMETMonAlg = helper.addAlgorithm(CompFactory.TrigMETMonitorAlgorithm,'TrigMETMonAlg')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    TrigMETMonChain1Alg = helper.addAlgorithm(CompFactory.TrigMETMonitorAlgorithm,'TrigMETMonChain1Alg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # expertTrigMETMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    # TrigMETMonAlg.TriggerChain = 'HLT_xe30_cell_L1XE10'
    # without filters, all events are processed.
    TrigMETMonChain1Alg.TriggerChain = 'HLT_xe65_cell_L1XE50'
    

    ### use the follwoing if you run on Run2 AOD
    #TrigMETMonAlg.hlt_cell_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET'
    #TrigMETMonAlg.hlt_mt_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht'
    #TrigMETMonAlg.hlt_tc_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
    #TrigMETMonAlg.hlt_tcpufit_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC'

    ### use the follwoing if you run on older Run3 AOD
    #TrigMETMonAlg.hlt_tcpufit_key = 'HLT_MET_tcPufit'
    #TrigMETMonAlg.hlt_trkmht_key = 'HLT_MET_mht'
       
    ### set chain names
    ### These are the active chains as of 01 April 2020 for testing 
    #TrigMETMonAlg.L1Chain1 = 'L1_XE10'
    #TrigMETMonAlg.HLTChain1 = 'HLT_xe65_cell_L1XE50'
    #TrigMETMonAlg.HLTChain2 = 'HLT_xe100_tcpufit_L1XE50'
    TrigMETMonAlg.HLTChain2 = 'HLT_xe100_trkmht_L1XE50'
    #TrigMETMonAlg.HLTChain2 = 'HLT_xe100_pfsum_L1XE50'


    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # expertTrigMETMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    metGroup = helper.addGroup(TrigMETMonAlg,'TrigMETMonitor','HLT/METMon/')

    # Add a GMT for the other example monitor algorithm
    metChain1Group = helper.addGroup(TrigMETMonChain1Alg,'TrigMETMonitor','HLT/METMon/HLT_xe65_cell_L1XE50/')

    ### STEP 5 ###
    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly
    #
    # Binning
    et_bins=205 #Et
    et_min=-13.5
    et_max=401.5
    ec_bins=199 #Ex, Ey, Ez
    ec_min=-298.5
    ec_max=298.5
    sumet_bins=305 #sumEt
    sumet_min=-27.0
    sumet_max=4203.0
    sume_bins=153 #sumE
    sume_min=-27.0
    sume_max=24003.0
    phi_bins=100 # phi
    phi_bins_2d=24 # phi
    phi_min=-3.1416
    phi_max=3.1416
    eta_bins=100 # eta
    eta_bins_2d=24# eta
    eta_min=-4.8
    eta_max=4.8
    eff_bins=42 # efficiency
    eff_min=-13.5
    eff_max=401.5
    # Histograms
    metGroup.defineHistogram('offline_Ex',title='Offline Missing E_{x};E_{x} [GeV];Events',
                             path='Expert/Offline',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('offline_Ey',title='Offline Missing E_{y};E_{y} [GeV];Events',
                             path='Expert/Offline',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('offline_Et',title='Offline Missing E_{T};E_{T} [GeV];Events',                            
                             path='Expert/Offline',xbins=et_bins,xmin=et_min,xmax=et_max)
    metGroup.defineHistogram('offline_sumEt',title='Offline sumE_{T};sumE_{T} [GeV];Events',
                             path='Expert/Offline',xbins=sumet_bins,xmin=sumet_min,xmax=sumet_max)
    metGroup.defineHistogram('L1_Ex',title='L1 Missing E_{x};E_{x} [GeV];Events',
                             path='Shifter/L1',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('L1_Ey',title='L1 Missing E_{y};E_{y} [GeV];Events',
                             path='Shifter/L1',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('L1_Et',title='L1 Missing E_{T};E_{T} [GeV];Events',
                             path='Shifter/L1',xbins=et_bins,xmin=et_min,xmax=et_max)
    metGroup.defineHistogram('cell_Ex',title='cell Missing E_{x};E_{x} [GeV];Events',
                             path='Shifter/cell',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('cell_Ey',title='cell Missing E_{y};E_{y} [GeV];Events',
                             path='Shifter/cell',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('cell_Et',title='cell Missing E_{T};E_{T} [GeV];Events',
                             path='Shifter/cell',xbins=et_bins,xmin=et_min,xmax=et_max)
    metGroup.defineHistogram('trkmht_Ex',title='trkmht Missing E_{x};E_{x} [GeV];Events',
                             path='Shifter/trkmht',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('trkmht_Ey',title='trkmht Missing E_{y};E_{y} [GeV];Events',
                             path='Shifter/trkmht',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('trkmht_Et',title='trkmht Missing E_{T};E_{T} [GeV];Events',
                             path='Shifter/trkmht',xbins=et_bins,xmin=et_min,xmax=et_max)
    metGroup.defineHistogram('tcpufit_Ex',title='tcpufit Missing E_{x};E_{x} [GeV];Events',
                             path='Shifter/tcpufit',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('tcpufit_Ey',title='tcpufit Missing E_{y};E_{y} [GeV];Events',
                             path='Shifter/tcpufit',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('tcpufit_Ez',title='tcpufit Missing E_{z};E_{z} [GeV];Events',
                             path='Shifter/tcpufit',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('tcpufit_Et',title='tcpufit Missing E_{T};E_{T} [GeV];Events',
                             path='Shifter/tcpufit',xbins=et_bins,xmin=et_min,xmax=et_max)
    metGroup.defineHistogram('tcpufit_sumEt',title='tcpufit sumEt;sumEt [GeV];Events',
                             path='Shifter/tcpufit',xbins=sumet_bins,xmin=sumet_min,xmax=sumet_max)
    metGroup.defineHistogram('tcpufit_sumE',title='tcpufit sumE;sumE [GeV];Events',
                             path='Shifter/tcpufit',xbins=sume_bins,xmin=sume_min,xmax=sume_max)
    metGroup.defineHistogram('tcpufit_eta',title='tcpufit #eta;#eta;Events',
                             path='Shifter/tcpufit',xbins=eta_bins,xmin=eta_min,xmax=eta_max)
    metGroup.defineHistogram('tcpufit_phi',title='tcpufit #phi;#phi;Events',
                             path='Shifter/tcpufit',xbins=phi_bins,xmin=phi_min,xmax=phi_max)
    metGroup.defineHistogram('tcpufit_phi;tcpufit_phi_etweight', title='tcpufit #phi (etweighted);#phi;E_{T} weighted events', 
                             weight='tcpufit_Et',
                             path='Shifter/tcpufit',xbins=phi_bins,xmin=phi_min,xmax=phi_max)
    metGroup.defineHistogram('tcpufit_eta,tcpufit_phi;tcpufit_eta_phi', type='TH2F', title='tcpufit #eta - #phi;#eta;#phi',
                             path='Shifter/tcpufit',
                             xbins=eta_bins_2d,xmin=eta_min,xmax=eta_max,ybins=phi_bins_2d,ymin=phi_min,ymax=phi_max)
    metGroup.defineHistogram('tcpufit_eta,tcpufit_phi;tcpufit_eta_phi_etweight', type='TH2F', title='tcpufit #eta - #phi (etweighted);#eta;#phi',
                             weight='tcpufit_Et',
                             path='Shifter/tcpufit',
                             xbins=eta_bins_2d,xmin=eta_min,xmax=eta_max,ybins=phi_bins_2d,ymin=phi_min,ymax=phi_max)
    metGroup.defineHistogram('offline_Et,pass_L11;L11_eff', type='TProfile',title='L1 efficiency;offline E_{T} [GeV];Efficiency',
                             path='Shifter/eff',xbins=eff_bins,xmin=eff_min,xmax=eff_max)
    metGroup.defineHistogram('offline_Et,pass_HLT1;HLT1_eff', type='TProfile',title='HLT1 efficiency;offline E_{T} [GeV];Efficiency',
                             path='Shifter/eff',xbins=eff_bins,xmin=eff_min,xmax=eff_max)
    metGroup.defineHistogram('offline_Et,pass_HLT2;HLT2_eff', type='TProfile',title='HLT2 efficiency;offline E_{T} [GeV];Efficiency',
                             path='Shifter/eff',xbins=eff_bins,xmin=eff_min,xmax=eff_max)
    metGroup.defineHistogram('mht_Ex',title='mht Missing E_{x};E_{x} [GeV];Events',
                             path='Shifter/mht',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('mht_Ey',title='mht Missing E_{y};E_{y} [GeV];Events',
                             path='Shifter/mht',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('mht_Et', title='mht E_{T};E_{T} [GeV];Events',
                             path='Shifter/mht',xbins=et_bins,xmin=et_min,xmax=et_max)
    metGroup.defineHistogram('pfsum_Ex',title='pfsum Missing E_{x};E_{x} [GeV];Events',
                             path='Shifter/pfsum',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('pfsum_Ey',title='pfsum Missing E_{y};E_{y} [GeV];Events',
                             path='Shifter/pfsum',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('pfsum_Et', title='pfsum E_{T};E_{T} [GeV];Events',
                             path='Shifter/pfsum',xbins=et_bins,xmin=et_min,xmax=et_max)
    metGroup.defineHistogram('tc_Ex',title='tc Missing E_{x};E_{x} [GeV];Events',
                             path='Expert/tc',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('tc_Ey',title='tc Missing E_{y};E_{y} [GeV];Events',
                             path='Expert/tc',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metGroup.defineHistogram('tc_Et', title='tc E_{T};E_{T} [GeV];Events',
                             path='Expert/tc',xbins=et_bins,xmin=et_min,xmax=et_max)
    metChain1Group.defineHistogram('cell_Ex',title='cell Missing E_{x};E_{x} [GeV];Events',
                                  path='cell',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metChain1Group.defineHistogram('cell_Ey',title='cell Missing E_{y};E_{y} [GeV];Events',
                                  path='cell',xbins=ec_bins,xmin=ec_min,xmax=ec_max)
    metChain1Group.defineHistogram('cell_Et',title='cell Missing E_{T};E_{T} [GeV];Events',
                                  path='cell',xbins=et_bins,xmin=et_min,xmax=et_max)
    
    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    return helper.result()
    
if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    #ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.Files = ['/hep300/data/khamano/data18_athenaMT/fromElin/AOD_old.pool.root']
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'TrigMETMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigMETMonitorAcc = TrigMETMonConfig(ConfigFlags)
    cfg.merge(trigMETMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigMETMonitorAcc.getEventAlgo('TrigMETMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events

