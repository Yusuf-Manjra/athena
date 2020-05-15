# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
from AthenaCommon.CFElements import parOR
from AthenaCommon.Logging import logging
from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool, L1Decoder
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import EmptyMenuSequence
log = logging.getLogger('EmuStepProcessingConfig')


def thresholdToChains( chains ):
    """
    Produces list "threshod : chain" for all chains passed. Uses the L1Thresholds/vseeds Chain property
    """
    ret = []
    for c in chains:
        for t in c.vseeds:
            ret.append(t+ " : " + c.name)
    return ret
            

def generateL1DecoderAndChains():
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataFlow( True )

    # 4 events
    data = {
        'noreco': [';', ';', ';',';'],
        'emclusters': [';', ';', ';',';'],
        'msmu': [';', ';', ';',';'],
        'ctp': [';', ';', ';',';'],
        'l1emroi': [';', ';', ';',';'],
        'l1muroi': [';', ';', ';',';']
        }  # in the lists there are the events


    
    # event 0: empty
    data['ctp'] [0]      =  'HLT_e20_L1EM10 HLT_e5_e8_L1EM3_EM5 HLT_e5_L1EM3 HLT_e8_L1EM5 HLT_g5_EM7'
    data['l1emroi'][0]   = ';'
    data['emclusters'][0]= ';'
    data['l1muroi'][0]   = ';'
    data['msmu'][0]      = ';'


    #event 1: 3e (1 not passing at L1, 1 not passing at step1) + 2mu (2 not passing) - HLT_e5_e8_L1EM3_EM5 HLT_2mu6_L12MU6
    data['ctp'] [1]      =  'HLT_e20_L1EM10 HLT_e5_L1EM3 HLT_e8_L1EM5 HLT_g5_L1EM7 HLT_e5_v3_L1EM7 HLT_e5_e8_L1EM3_EM5 HLT_mu8_e8_L1MU6_EM7 HLT_2mu6_L12MU6 HLT_mu6_mu10_L12MU6 HLT_2mu4_bDimu_L12MU6'
    data['l1emroi'][1]   =  '1,1,0,EM3,EM5,EM7,EM20,EM50,EM100; 2.,-1.2,0,EM3,EM5,EM7; 3.,0.2,0,EM3;'
    data['emclusters'][1]=  'eta:1,phi:1,et:180000; eta:1,phi:-1.2,et:6000; eta:0.5,phi:-1.2,et:3000;'
    data['l1muroi'][1]   =  '2,0.5,0,MU6; 3,0.5,0,MU6;'
    #data['l1muroi'][1]   =  '0,0,0,MU0;'
    data['msmu'][1]      = 'eta:-1.2,phi:0.7,pt:1500,pt2:1500; eta:-1.1,phi:0.6,pt:1500,pt2:1500;'

    # event 2: 2e+ 3mu : HLT_e5_e8_2mu6_L1EM3_EM5_L12MU6, HLT_mu6_e10_L1MU6_EM5
    data['ctp'] [2]      =  'HLT_mu6_L1MU6 HLT_mu8_L1MU10 HLT_mu10_L1MU10 HLT_mu8_1step_L1MU6 HLT_e20_L1EM10 HLT_e5_L1EM3 HLT_e8_L1EM5 HLT_mu6_e10_L1MU6_EM5 HLT_mu6Comb_e8_L1MU6_EM5 HLT_e3_e5_L1EM3_EM5 HLT_2mu6_L12MU6 HLT_2mu6Comb_L12MU6 HLT_mu6_mu10_L12MU6 HLT_2mu4_bDimu_L12MU6 HLT_e5_e8_L1EM3_EM5 HLT_e5_e8_2mu6_L1EM3_EM5_L12MU6 HLT_mu6_mu6noL1_L1MU6'
    data['l1emroi'][2]   =  '2,0.2,0,EM3,EM5,EM7,EM15,EM20,EM50,EM100; 1,-1.1,0,EM3,EM5,EM7,EM15,EM20,EM50;'
    data['emclusters'][2]=  'eta:0.5,phi:0,et:120000; eta:1,phi:-1.2,et:65000;'
    data['l1muroi'][2]   =  '2,0.5,0,MU6,MU8; 3,0.5,0,MU6,MU8,MU10;2.2,0.6,0,MU6;'
    data['msmu'][2]      =  'eta:-1.2,phi:0.7,pt:6500,pt2:8500; eta:-1.1,phi:0.6,pt:8500,pt2:8500;eta:-1.1,phi:0.6,pt:8500,pt2:8500;'

    #event 3: 1e + 1mu; HLT_mu6_e10_L1MU6_EM5 does not pass because of e10
    data['ctp'] [3]      =  'HLT_mu20_L1MU10 HLT_mu10_L1MU10 HLT_mu8_L1MU10 HLT_mu8_1step_L1MU6 HLT_2mu8 HLT_e8_L1EM5 HLT_mu6_e10_L1MU6_EM5 HLT_mu6Comb_e8_L1MU6_EM5'

    data['l1emroi'][3]   =  '1,1.5,0,EM3,EM5,EM7;'
    data['emclusters'][3]=  'eta:-0.6,phi:1.7,et:9000;'
    data['l1muroi'][3]   =  '2,-0.1,0,MU6,MU8,MU10;'
    data['msmu'][3]      =  'eta:-1.7,phi:-0.2,pt:29500,pt2:8500;'

    # otehr vectors

    data['tracks'] = ['eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                      'eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                      'eta:0.5,phi:0,pt:130000; eta:1,phi:-1.2,pt:60000;eta:-1.2,phi:0.7,pt:6700; eta:-1.1,phi:0.6,pt:8600;',
                      'eta:-0.6,phi:1.7,et:9000;'] # no MU track for MS candidate 'eta:-1.7,phi:-0.2,pt:9500;'

    data['mucomb'] = [';',
                      ';',
                      'eta:-1.2,phi:0.7,pt:6600; eta:-1.1,phi:0.6,pt:8600;',
                      ';']

    data['electrons'] = [';',
                         'eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                         ';',
                         ';']
    data['photons'] = [';',
                       'eta:1,phi:1,pt:130000;',
                       ';',
                       ';']



    from TrigUpgradeTest.TestUtils import writeEmulationFiles, makeChain
    writeEmulationFiles(data)

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep


    doMuon     = True
    doElectron = True
    doCombo    = True

    HLTChains = []

    # muon chains
    if doMuon:
        from TrigUpgradeTest.HLTSignatureConfig import  muMenuSequence
        #step1
        mu11 = muMenuSequence(step="1",reconame="v1", hyponame="v1")
        mu12 = muMenuSequence(step="1",reconame="v2", hyponame="v2")
                    
        #step2
        mu21 = muMenuSequence(step="2",reconame="v1", hyponame="v1")
        mu22 = muMenuSequence(step="2",reconame="v2", hyponame="v2")
        #step3
        mu31 = muMenuSequence(step="3",reconame="v1", hyponame="v1")
        mu32 = muMenuSequence(step="3",reconame="v2", hyponame="v2")
        #step4
        mu41 = muMenuSequence(step="4",reconame="v1", hyponame="v1")

        step_mu11  = ChainStep("Step1_mu11", [mu11])
        step_mu21  = ChainStep("Step2_mu21", [mu21] )
        step_mu22  = ChainStep("Step2_mu22", [mu22] )
        step_mu31  = ChainStep("Step3_mu31", [mu31] )
        step_mu32  = ChainStep("Step3_mu32", [mu32] )
        step_mu41  = ChainStep("Step4_mu41", [mu41] )
        
        step_empy= ChainStep("Step2_empty")


        MuChains  = [
            makeChain(name='HLT_mu8_1step_L1MU6',  L1Thresholds=["MU6"],  ChainSteps=[step_mu11]),
            makeChain(name='HLT_mu20_L1MU10',   L1Thresholds=["MU10"],   ChainSteps=[step_mu11 , step_mu21 , step_mu31] ),
            makeChain(name='HLT_mu10_L1MU10',   L1Thresholds=["MU10"],   ChainSteps=[step_mu11 , step_mu22 , step_mu31] ),
            makeChain(name='HLT_mu8_L1MU10',    L1Thresholds=["MU10"],   ChainSteps=[step_mu11 , step_mu21 , step_mu32, step_mu41] ),
            makeChain(name='HLT_mu6_L1MU6',     L1Thresholds=["MU6"],    ChainSteps=[step_mu11 , step_empy , step_mu32, step_mu41] ),
 #           Chain(name='HLT_mu6_1step_L1MU6',  L1Item="L1_MU6",  ChainSteps=[step_mu11 , step_empy , step_mu31]),
#            Chain(name='HLT_2mu6_L12MU6',  L1Item="L1_2MU6",      ChainSteps=[ChainStep("Step_2muSA", [muSA,muSA])  ] )
            ]
            

        HLTChains += MuChains


    ## #electron chains
    if doElectron:
        from TrigUpgradeTest.HLTSignatureConfig import  elMenuSequence, gamMenuSequence
        el11 = elMenuSequence(step="1",reconame="v1", hyponame="v1")
        el21 = elMenuSequence(step="2",reconame="v1", hyponame="v1")
        el22 = elMenuSequence(step="2",reconame="v2", hyponame="v2")
        el23 = elMenuSequence(step="2",reconame="v2", hyponame="v3")
        el31 = elMenuSequence(step="3",reconame="v1", hyponame="v1")

        # gamma
        gamm11 = gamMenuSequence("1", reconame="v1", hyponame="v1")

    
        ElChains  = [
            makeChain(name='HLT_e5_L1EM3'   , L1Thresholds=["EM3"], ChainSteps=[ ChainStep("Step1_em11", [el11]), ChainStep("Step2_em21",  [el21]) ] ),
            makeChain(name='HLT_e5_v2_L1EM7', L1Thresholds=["EM7"], ChainSteps=[ ChainStep("Step1_em11", [el11]), ChainStep("Step2_em22",  [el22]) ] ),
            makeChain(name='HLT_e5_v3_L1EM7', L1Thresholds=["EM7"], ChainSteps=[ ChainStep("Step1_em11", [el11]), ChainStep("Step2_em23",  [el23]) ] ),
            makeChain(name='HLT_e8_L1EM5'   , L1Thresholds=["EM5"], ChainSteps=[ ChainStep("Step1_em11", [el11]), ChainStep("Step2_em21",  [el21]), ChainStep("Step3_em31",  [el31]) ] ),
            makeChain(name='HLT_g5_L1EM7'   , L1Thresholds=["EM7"], ChainSteps=[ ChainStep("Step1_gam11", [gamm11]) ] )
        ]

        HLTChains += ElChains

        

    # combined chain
    if doCombo:
        emptySeq1 = EmptyMenuSequence("step1EmptySeqence")
        emptySeq2 = EmptyMenuSequence("step2EmptySeqence")
        if not doElectron:
            from TrigUpgradeTest.HLTSignatureConfig import elMenuSequence        
            el11 = elMenuSequence(step="1",reconame="v1", hyponame="v1")    
            el21 = elMenuSequence(step="2",reconame="v1", hyponame="v1")
            
        if not doMuon:
            from TrigUpgradeTest.HLTSignatureConfig import muMenuSequence
            #step1
            mu11 = muMenuSequence(step="1",reconame="v1", hyponame="v1")
            mu12 = muMenuSequence(step="1",reconame="v2", hyponame="v2")
            #step2
            mu21 = muMenuSequence(step="2",reconame="v1", hyponame="v1")
            mu22 = muMenuSequence(step="2",reconame="v2", hyponame="v2")
            #step3
            mu31 = muMenuSequence(step="3",reconame="v1", hyponame="v1")
            mu32 = muMenuSequence(step="3",reconame="v2", hyponame="v2")
            #step4
            mu41 = muMenuSequence(step="4",reconame="v1", hyponame="v1")
           
           
        from TrigUpgradeTest.HLTSignatureHypoTools import dimuDrComboHypoTool

            
           
        # multiplicity here indicates the number of objects to be combined:
        # for the chain dictionary, get the sum of the multiplicity in the multiplicy array
        # in symmetric chains, multiplicity=2 but only one sequence is used


     
        CombChains =[
            # This is an example of a chain running in "serial"
            makeChain(name='HLT_mu6_e10_L1MU6_EM5',  L1Thresholds=["MU6","EM5"], ChainSteps=[
                ChainStep("Step1_mu_em_serial", [mu11, emptySeq1], multiplicity=[1,1]),
                ChainStep("Step2_mu_em_serial", [emptySeq2, el21], multiplicity=[1,1])] ),

            makeChain(name='HLT_mu6Comb_e8_L1MU6_EM5', L1Thresholds=["MU6","EM5"], ChainSteps=[
                ChainStep("Step1_mu2_em", [mu12, el11], multiplicity=[1,1]),
                ChainStep("Step2_mu_em", [mu21, el21], multiplicity=[1,1])] ),

            makeChain(name='HLT_e5_e8_L12EM3',   L1Thresholds=["EM3","EM3"], ChainSteps=[
                ChainStep("Step1_2emAs",   [el11, el11], multiplicity=[1,1]),
                ChainStep("Step2_2emAs",   [el21, el21], multiplicity=[1,1]) ]),
                
            makeChain(name='HLT_e5_e8_2mu6_L1EM3_EM5_L12MU6',   L1Thresholds=["EM3","EM5","MU6"], ChainSteps=[
                ChainStep("Step1_2em_2mu",   [el11,el11,mu11], multiplicity=[1,1,2]),
                ChainStep("Step2_2em_2mu",   [el21,el21,mu21], multiplicity=[1,1,2]) ]),

            makeChain(name='HLT_2mu6_L12MU6',       L1Thresholds=["MU6"], ChainSteps=[
                ChainStep("Step1_2mu",   [mu11], multiplicity=[2]),
                ChainStep("Step2_2mu",   [mu21], multiplicity=[2]) ]),

            makeChain(name='HLT_mu6_mu10_L12MU6',       L1Thresholds=["MU6", "MU6"], ChainSteps=[
                ChainStep("Step1_2muAs",   [mu11,mu11], multiplicity=[1,1]),
                ChainStep("Step2_2muAs",   [mu21,mu21], multiplicity=[1,1]) ]),
                
            makeChain(name='HLT_2mu6Comb_L12MU6',   L1Thresholds=["MU6"], ChainSteps=[
                ChainStep("Step1_2mu_empty",  multiplicity=[2]),
                ChainStep("Step2_2mu", [mu21], multiplicity=[2]) ]),

            makeChain(name='HLT_mu6_e5_L1MU6_EM5',  L1Thresholds=["MU6","EM5"], ChainSteps=[
                ChainStep("Step1_mu_em", [mu11, el11], multiplicity=[1,1], comboToolConfs=[dimuDrComboHypoTool]),
                ChainStep("Step2_mu_em", [mu21, el21], multiplicity=[1,1], comboToolConfs=[dimuDrComboHypoTool])] ),
                                                                                       
            makeChain(name='HLT_2mu4_bDimu_L12MU6', L1Thresholds=["MU6"], ChainSteps=[
                ChainStep("Step1_2mu",    [mu11], multiplicity=[2], comboToolConfs=[dimuDrComboHypoTool]),
                ChainStep("Step2_2mu22",  [mu22], multiplicity=[2]),
                ChainStep("Step3_2mu",    [mu31], multiplicity=[2]) ] ),
                                                                                       
            makeChain(name='HLT_mu6_mu6noL1_L1MU6', L1Thresholds=["MU6", "FSNOSEED"], ChainSteps=[
                ChainStep("Step1_2muAs",   [mu11, mu11], multiplicity=[1,1]),
                ChainStep("Step2_2muAs",   [mu21, mu21], multiplicity=[1,1]) ])
                                                                              
            ]


        HLTChains += CombChains




    ########################## L1 #################################################

    L1UnpackingSeq = parOR("L1UnpackingSeq")

    l1Decoder = L1Decoder( RoIBResult="", L1TriggerResult="" )
    l1Decoder.L1DecoderSummaryKey = "L1DecoderSummary"
    ctpUnpacker = CTPUnpackingEmulationTool( ForceEnableAllChains=False , InputFilename="ctp.dat" )
    l1Decoder.ctpUnpacker = ctpUnpacker

    ## hack to solve the PS crash:
    from L1Decoder.L1DecoderConf import PrescalingEmulationTool
    psEmulation = PrescalingEmulationTool()
    l1Decoder.prescaler = psEmulation

    from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection, mapThresholdToL1DecisionCollection

    emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool", InputFilename="l1emroi.dat", OutputTrigRoIs=mapThresholdToL1RoICollection("EM"), Decisions=mapThresholdToL1DecisionCollection("EM"), ThresholdPrefix="EM" )

    muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool", InputFilename="l1muroi.dat",  OutputTrigRoIs=mapThresholdToL1RoICollection("MU"), Decisions=mapThresholdToL1DecisionCollection("MU"), ThresholdPrefix="MU" )

    l1Decoder.roiUnpackers = [emUnpacker, muUnpacker]

    #print l1Decoder
    L1UnpackingSeq += l1Decoder
    log.debug(L1UnpackingSeq)

    ########################## L1 #################################################

    return l1Decoder, HLTChains
