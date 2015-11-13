# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------phy--------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_pp_v5.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    PhysicsStream="Main"

    #---------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    #---------------------------------------------------------------------

    #---------------------------------------------------------------------
    # if it's needed to temporary remove almost all the chains from the menu
    # be aware that it is necessary to leave at least one chain in the muon slice
    # otherwise athenaHLT will seg-fault 
    #---------------------------------------------------------------------

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []


    TriggerFlags.MuonSlice.signatures = [
        # single muon
        ['mu0_perf',              'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1], 
        ['mu26_imedium',          'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu50',                  'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60_0eta105_msonly',   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu40',                   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_imedium',           'L1_MU20',           [], [PhysicsStream, 'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_iloose_L1MU15',     'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_iloose_L1MU15',     'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu26',                   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_L1MU15',            'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu22',                   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20',                   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_L1MU15',            'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24',                   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_msonly',            'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu18',                   'L1_MU15',           [], [PhysicsStream, 'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu14',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu11',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu14_iloose',            'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_msonly',            'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu8',                    'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1], 
        ['mu6',                    'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6_msonly',             'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4',                    'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4_msonly',             'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_idperf',            'L1_MU20',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu20_idperf',            'L1_MU20',           [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu10_idperf',            'L1_MU10',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu6_idperf',             'L1_MU6',            [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu4_idperf',             'L1_MU4',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu24_L2Star_idperf',     'L1_MU20',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu6_L2Star_idperf',      'L1_MU6',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],



        # multi muons
        ['2mu14',                  'L1_2MU10',          [], [PhysicsStream, 'express'], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6',                   'L1_3MU6',           [], [PhysicsStream, 'express'], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6_msonly',            'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu10',                  'L1_2MU10',          [], [PhysicsStream, 'express'], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu24_mu8noL1',           'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu8noL1']]],
        ['mu24_2mu4noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','2mu4noL1']]],
        ['mu22_mu8noL1',           'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1']]],
        ['mu22_2mu4noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','2mu4noL1']]],
        ['mu20_mu8noL1',           'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','mu8noL1']]],
        ['mu20_2mu4noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],
        ['mu18_mu8noL1',           'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','mu8noL1']]],
        ['mu18_2mu4noL1',          'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','2mu4noL1']]],
        ['mu6_mu4noL1',            'L1_MU6',  ['L1_MU6',''],  [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu6','mu4noL1']]],
        ['mu4_mu4noL1',            'L1_MU4',  ['L1_MU4',''],  [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu4','mu4noL1']]],
	['mu4noL1',                'L1_MU4',  ['L1_MU4',''],  [PhysicsStream], ['RATE:SingleMuon','BW:Muon'], -1, ],
        ['2mu6',                    'L1_2MU6',  [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu4',                    'L1_2MU4',  [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu20_2mu0noL1_JpsimumuFS', 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu0noL1_JpsimumuFS']]],
        ['mu18_2mu0noL1_JpsimumuFS', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','2mu0noL1_JpsimumuFS']]],
        ['mu20_2mu4_JpsimumuL2',     'L1_MU20', ['L1_MU20','L1_2MU4'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['2mu4_JpsimumuL2','mu20']]],
        ['mu18_2mu4_JpsimumuL2',     'L1_MU15', ['L1_MU15','L1_2MU4'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['2mu4_JpsimumuL2','mu18']]],
        # Primary (multi muon chains)
        ['3mu4',                   'L1_3MU4',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu4_l2msonly',          'L1_3MU4',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu6_l2msonly_2mu4_l2msonly_L1MU6_3MU4',        'L1_MU6_3MU4',           ['L1_MU6','L1_3MU4'], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1,['serial',-1,['mu6_l2msonly','2mu4_l2msonly']]],
        ['2mu6_l2msonly_mu4_l2msonly_L12MU6_3MU4',   'L1_2MU6_3MU4',           ['L1_2MU6','L1_3MU4'], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1,['serial',-1,['2mu6_l2msonly','mu4_l2msonly']]],

        
        ## ADD MU*_EMPTY TO L1 MENU
        ['mu4_cosmic_L1MU4_EMPTY',               'L1_MU4_EMPTY', [],   ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_cosmic_L1MU11_EMPTY',              'L1_MU11_EMPTY', [],  ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_msonly_cosmic_L1MU4_EMPTY',        'L1_MU4_EMPTY', [],   ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_msonly_cosmic_L1MU11_EMPTY',       'L1_MU11_EMPTY', [],  ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],                            
                
        ##Streaming name?
        #['mu4_cosmicEF_ds1_L1MU4', 'L1_MU4', [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],
        #['mu4_cosmicEF_ds2_L1MU4', 'L1_MU4', [], ['DataScouting_01_CosmicMuons','DataScouting_02_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],

        # muon calibration
        ['mu0_muoncalib',             'L1_MU20',     [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
        ['mu0_muoncalib_L1MU15',      'L1_MU15',     [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
        ['mu0_muoncalib_L1MU4_EMPTY', 'L1_MU4_EMPTY',[],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],

        # muon calibration, data scouting
        ['mu0_muoncalib_ds3',             'L1_MU20',     [],['DataScouting_03_CosmicMuons'],["RATE:Calibration","BW:Muon"],-1],        
        ['mu0_muoncalib_ds3_L1MU15',      'L1_MU15',     [],['DataScouting_03_CosmicMuons'],["RATE:Calibration","BW:Muon"],-1],
        ['mu0_muoncalib_ds3_L1MU4_EMPTY', 'L1_MU4_EMPTY',[],['DataScouting_03_CosmicMuons'],["RATE:Calibration","BW:Muon"],-1],   
        
        # Toroid-off run (ATR-9923)
        ['mu8_mucombTag_noEF_L1MU40',  'L1_MU20', [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_mucombTag_noEF_L1MU40', 'L1_MU20', [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu15_mucombTag_noEF_L1MU40', 'L1_MU20', [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_mucombTag_noEF_L1MU40', 'L1_MU20', [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu25_mucombTag_noEF_L1MU40', 'L1_MU20', [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # Narrow scan
        # notes: see ATR-11846
        # inputTE for narrow scan sequence (2nd inputTE) is a dummy string for MuonDef.py
        # To indicate narrow scan uses lastTE of previous sequence and also to avoid duplicated sequences with same TE name, write L2_ TE name of the 1st sequence.
        ['mu20_msonly_mu6noL1_msonly_nscan05',       'L1_MU20',      ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],
        ['mu11_L1MU10_2mu4noL1_nscan03_L1MU10_2MU6', 'L1_MU10_2MU6', ['L1_MU10','L2_mu11_L1MU10'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_L1MU10','2mu4noL1_nscan03']]],
        ['mu20_mu6noL1_nscan03',                     'L1_MU20',      ['L1_MU20','L2_mu20'],        [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','mu6noL1_nscan03']]],
	['mu11_2mu4noL1_nscan03_L1MU11_2MU6',        'L1_MU11_2MU6', ['L1_MU11','L2_mu11'],        [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11','2mu4noL1_nscan03']]],
        ['mu11_L1MU10_2mu4noL1_nscan03',             'L1_LFV-MU',    ['L1_MU10','L2_mu11_L1MU10'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_L1MU10','2mu4noL1_nscan03']]],
        ['mu11_2mu4noL1_nscan03_L1MU11_LFV-MU',      'L1_LFV-MU',    ['L1_MU11','L2_mu11'],        [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11','2mu4noL1_nscan03']]],


        ['mu20_llns_mu6noL1_nscan03',                     'L1_MU20',      ['L1_MU20','L2_mu6'],        [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_llns','mu6noL1_nscan03']]],
        ['mu11_llns_2mu4noL1_nscan03_L1MU11_2MU6',        'L1_MU11_2MU6', ['L1_MU11','L2_mu4'],        [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_llns','2mu4noL1_nscan03']]],


        # ----- New Chains L2msonly ---------------------------
        ['mu20_l2msonly_mu6noL1_nscan03_l2msonly',                     'L1_MU20',      ['L1_MU20','L2_mu20_l2msonly'],        [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_l2msonly','mu6noL1_nscan03_l2msonly']]],
        ['mu11_l2msonly_L1MU10_2mu4noL1_nscan03_l2msonly_L1MU10_2MU6', 'L1_MU10_2MU6', ['L1_MU10','L2_mu11_l2msonly_L1MU10'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_l2msonly_L1MU10','2mu4noL1_nscan03_l2msonly']]],
	['mu11_l2msonly_2mu4noL1_nscan03_l2msonly_L1MU11_2MU6',        'L1_MU11_2MU6', ['L1_MU11','L2_mu11_l2monly'],         [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_l2msonly','2mu4noL1_nscan03_l2msonly']]],
        ['2mu10_l2msonly',                  'L1_2MU10',          [], [PhysicsStream, 'express'], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu14_l2msonly',                  'L1_2MU10',          [], [PhysicsStream, 'express'], ['RATE:MultiMuon', 'BW:Muon'], -1],
	# -----------------------------------------------------

        # LLP chains
        # notes: see ATR-11482
        # L1 is seeded from single EMPTY/UNPAIRED_ISO, but, at HLT it does a same sequence as main ones (e.g. MU4_EMPTY single seeded, but 3MU6 is required at HLT)
        # this is done on purpose (ATR-11482)
        ['3mu6_msonly_L1MU4_EMPTY',                              'L1_MU4_EMPTY',        ['L1_3MU6'], ["Late"], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6_msonly_L1MU4_UNPAIRED_ISO',                       'L1_MU4_UNPAIRED_ISO', ['L1_3MU6'], ["Late"], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY',       'L1_MU4_EMPTY',        ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_ISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],

        # exclusive di-lep
        ['2mu6_10invm30_pt2_z10', 'L1_2MU6', [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

	] 
	
    TriggerFlags.JetSlice.signatures = [   
        # data scouting
        ['j0_perf_ds1_L1All',      'L1_All',  [], ['DataScouting_05_Jets'], ['RATE:Cosmic_Jets_DS', 'BW:Jets'], -1],
        ['j0_perf_ds1_L1J75',      'L1_J75',  [], ['DataScouting_05_Jets'], ['RATE:Jets_DS', 'BW:Jets'], -1],
        ['j0_perf_ds1_L1J100',     'L1_J100', [], ['DataScouting_05_Jets'], ['RATE:Jets_DS', 'BW:Jets'], -1],

        # Performance chains
        ['j0_L1J12_EMPTY',         'L1_J12_EMPTY', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['ht0_L1J12_EMPTY',        'L1_J12_EMPTY', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_lcw_jes_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1], 
        ['j0_perf_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j15_320eta490', 'L1_RD0_FILLED',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j25_320eta490', 'L1_RD0_FILLED',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j35_320eta490', 'L1_RD0_FILLED',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j45_320eta490', 'L1_J15.31ETA49', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j55_320eta490', 'L1_J15.31ETA49', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j15_j15_320eta490',              'L1_RD0_FILLED',      [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j25_j25_320eta490',              'L1_RD0_FILLED',      [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j35_j35_320eta490',              'L1_RD0_FILLED',      [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_j45_320eta490',              'L1_RD0_FILLED',      [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j55_j55_320eta490_L1RD0_FILLED', 'L1_RD0_FILLED',      [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j55_j55_320eta490',              'L1_J15_J15.31ETA49', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j60_j60_320eta490',              'L1_J20_J20.31ETA49', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j85_j85_320eta490',              'L1_J20_J20.31ETA49', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        # forward jets
        ['j85_280eta320_nojcalib',         'L1_J20.28ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_lcw_nojcalib',     'L1_J20.28ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        # test chains

        ['j85_test1',                             'L1_J20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_test2',                             'L1_J20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_jes_test1',                         'L1_J20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_jes_test2',                         'L1_J20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['4j45_test1',                            'L1_3J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['4j45_test2',                            'L1_3J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        ['j85_2j45',                              'L1_J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j85_test1_2j45_test1',                  'L1_J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j85_test2_2j45_test2',                  'L1_J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        ['j85_0eta240_test1_j25_240eta490_test1', 'L1_J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j85_0eta240_test2_j25_240eta490_test2', 'L1_J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        ## multijets
        ['3j175',            'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['3j175_jes_PS','L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        ['4j100',                   'L1_3J50', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j25',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j45',                    'L1_3J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85',                    'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_jes',                'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_lcw',                'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_lcw_jes',            'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_lcw_nojcalib',       'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_nojcalib',           'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['5j25',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j45',                    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j55',                    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j60',                    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j70',                    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85',                    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_jes',                'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_lcw',                'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_lcw_jes',            'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_nojcalib',            'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_lcw_nojcalib',        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
 
        ['6j25',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],        
        ['6j45',                          'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j45_0eta240',                  'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j45_0eta240_L14J20',           'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j45_0eta240_L15J150ETA25',     'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j50_0eta240_L14J20',           'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j50_0eta240_L15J150ETA25',     'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j55_0eta240_L14J20',           'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j55_0eta240_L15J150ETA25',     'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],


        ['7j25',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],      
        ['7j45',                          'L1_6J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['7j45_0eta240_L14J20',           'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['7j45_0eta240_L15J150ETA25',     'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['7j45_L14J20',                   'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['ht1000_L1J100',    'L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],



        ## single jet
        ['j100',                   'L1_J25',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j110',                   'L1_J30',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j110_320eta490',          'L1_J30.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j15',            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j150',                   'L1_J40',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j175',                   'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_320eta490',           'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_jes',       'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw',       'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw_jes',    'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw_nojcalib',    'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_nojcalib',        'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_jes',               'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_lcw',               'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_lcw_jes',           'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_lcw_nojcalib',        'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_nojcalib',           'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
 
        ['j200',                   'L1_J50', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j200_jes_PS', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],         

        ['j25',            'L1_RD0_FILLED', [], [PhysicsStream, 'express'], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j260',                   'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j260_320eta490',           'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_jes',       'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw',       'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw_jes',    'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw_nojcalib',    'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_nojcalib',        'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j300',                   'L1_J85', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j300_lcw_nojcalib',       'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j320',                   'L1_J85', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j35',            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_jes',        'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_lcw',        'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_lcw_jes',    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_lcw_nojcalib',     'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_nojcalib',         'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j45',            'L1_J15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j360',                   'L1_J100', [], [PhysicsStream, 'express'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j360_320eta490',           'L1_J100.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_jes',       'L1_J100.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_lcw',       'L1_J100.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_lcw_jes',    'L1_J100.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_lcw_nojcalib',    'L1_J100.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_nojcalib',        'L1_J100.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j380',                   'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_jes',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw_jes',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw_nojcalib',      'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_nojcalib',          'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
 
        ['j400',                   'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_jes',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_lcw',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_lcw_jes',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_nojcalib',          'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_sub',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
 
        ['j420',                   'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_jes',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw_jes',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw_nojcalib',      'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_nojcalib',          'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
 

        ['j440',                   'L1_J120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j45_L1RD0_FILLED',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j460',                   'L1_J120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j460_a10_sub_L1J100', 'L1_J100', [], [PhysicsStream, 'express'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j460_a10r_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j460_a10_nojcalib_L1J100'                   ,  'L1_J100',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_nojcalib_L1J100'               ,  'L1_J100',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_sub_L1J100'                ,  'L1_J100',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_sub_L1J100'               , 'L1_J100',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_sub_L1J100'               , 'L1_J100',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['ht850_L1J75', 'L1_J75', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['ht700_L1J75', 'L1_J75', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 

        ['ht850_L1J100',     'L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['ht700_L1J100',     'L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j360_a10r_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j55',                    'L1_J15',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j55_L1RD0_FILLED',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j60',                    'L1_J20',  [], [PhysicsStream, 'express'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j60_280eta320',          'L1_J20.28ETA31', [], [PhysicsStream, 'express'], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j60_320eta490',          'L1_J20.31ETA49', [], [PhysicsStream, 'express'], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j60_L1RD0_FILLED',      'L1_RD0_FILLED', [], [PhysicsStream, 'express'], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j85',                    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_280eta320',           'L1_J20.28ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_jes',       'L1_J20.28ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_lcw',       'L1_J20.28ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_lcw_jes',    'L1_J20.28ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_320eta490',           'L1_J20.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_L1RD0_FILLED',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_jes',                'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_lcw',                'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_lcw_jes',            'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_lcw_nojcalib',        'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_nojcalib',       'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],


        # VBF triggers
        ['2j40_0eta490_invm250_L1XE55', 'L1_XE55',         [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        # L1Topo HT
        ['j360_a10_nojcalib'                      ,      'L1_HT150-J20s5.ETA31',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_nojcalib_L1HT150-J20.ETA31',      'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_nojcalib'                      ,  'L1_HT150-J20s5.ETA31',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_nojcalib_L1HT150-J20.ETA31',  'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_nojcalib'                          ,  'L1_HT190-J15s5.ETA21',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_nojcalib_L1HT190-J15.ETA21'    ,  'L1_HT190-J15.ETA21',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_nojcalib'                      ,  'L1_HT190-J15s5.ETA21',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_nojcalib_L1HT190-J15.ETA21',  'L1_HT190-J15.ETA21',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_sub'                      ,  'L1_HT150-J20s5.ETA31',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_sub_L1HT150-J20.ETA31',  'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_sub'                      , 'L1_HT150-J20s5.ETA31',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_sub_L1HT150-J20.ETA31', 'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_sub'                          , 'L1_HT190-J15s5.ETA21',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_sub_L1HT190-J15.ETA21'    , 'L1_HT190-J15.ETA21',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_sub'                      , 'L1_HT190-J15s5.ETA21',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_sub_L1HT190-J15.ETA21', 'L1_HT190-J15.ETA21',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['ht400'                       ,    'L1_HT150-J20s5.ETA31',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht400_L1HT150-J20.ETA31' ,    'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht550'                       ,    'L1_HT150-J20s5.ETA31',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht550_L1HT150-J20.ETA31' ,    'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht700'                          , 'L1_HT190-J15s5.ETA21',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht700_L1HT190-J15.ETA21'    , 'L1_HT190-J15.ETA21',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht850'                          , 'L1_HT190-J15s5.ETA21',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht850_L1HT190-J15.ETA21'    , 'L1_HT190-J15.ETA21',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht1000'                          , 'L1_HT190-J15s5.ETA21',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht1000_L1HT190-J15.ETA21'    , 'L1_HT190-J15.ETA21',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        # VBF triggers
        ['2j40_0eta490_invm250',        'L1_XE35_MJJ-200', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        #reclustering chains
        ['j360_a10r', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j460_a10r', 'L1_HT150-J20s5.ETA31', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ]

    TriggerFlags.BjetSlice.signatures = [

        ['j0_perf_bperf_L1RD0_EMPTY',  'L1_RD0_EMPTY',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j0_perf_bperf_L1MU10',       'L1_MU10',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j0_perf_bperf_L1J12_EMPTY',  'L1_J12_EMPTY',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j15_bperf', 'L1_RD0_FILLED', [], [PhysicsStream, 'express'], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j55_bperf', 'L1_J20', [], [PhysicsStream, 'express'], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j110_bperf', 'L1_J30', [], [PhysicsStream, 'express'], ['RATE:SingleBJet', 'BW:Bjet'], -1], 

        #bperf	
        ['j45_bperf_3j45', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_bperf_3j45_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_bperf_3j45_L13J15.0ETA25', 'L1_3J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_bperf_3j45_L13J20', 'L1_3J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j55_bperf_3j55', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j25_bperf', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j35_bperf', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_bperf', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j85_bperf', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j150_bperf', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j260_bperf', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j320_bperf', 'L1_J85', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j400_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],


        ['j15_bperf_split', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j25_bperf_split', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j35_bperf_split', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_bperf_split', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j85_bperf_split', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j110_bperf_split', 'L1_J30', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j150_bperf_split', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j260_bperf_split', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j320_bperf_split', 'L1_J85', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j400_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],

        ## All chains                                                                                                                                                                        
        ['2j75_bmedium_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j70_btight_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j100_2j55_bmedium', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_bmedium_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_btight_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_2j55_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_btight_2j45_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_bmedium_3j75_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_btight_3j70_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j175_bmedium_j60_bmedium', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j300_bloose', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],

        ['2j55_bperf_L14J20.0ETA49', 'L1_4J20.0ETA49', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],

        # low lumi menu                                                                                                                            

        ['2j35_btight_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j35_btight_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_bmedium_2j45_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_bmedium_split_2j45_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_btight_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_btight_split_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_btight_split_2j45_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bloose_L14J20.0ETA49', 'L1_4J20.0ETA49', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_L14J20.0ETA49', 'L1_4J20.0ETA49', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_split_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_split_2j55_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j65_btight_split_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j70_bmedium_split_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j70_btight_split_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j75_bmedium_split_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],

        ['j100_2j55_bmedium_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j150_bmedium_split_j50_bmedium_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j175_bmedium_split_j60_bmedium_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],

        ['j65_btight_split_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j65_btight_split_3j65_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_bmedium_split_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_bmedium_split_3j70_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_btight_split_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_btight_split_3j70_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j75_bmedium_split_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j75_bmedium_split_3j75_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],

        ['j225_bloose_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j300_bloose_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
                      
        ['2j70_bmedium_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j65_btight_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j70_bmedium_3j70_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j65_btight_3j65_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j45_bmedium_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j35_btight_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j70_bmedium_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j65_btight_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j225_bloose', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j150_bmedium_j50_bmedium', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j175_bmedium', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        # split versions of a few chains for validation                                                                                                                                      
        ['2j35_btight_split_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_bmedium_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j175_bmedium_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],

        # Calibration chains (default, offline taggers)
        ['j15_boffperf', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j25_boffperf', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j35_boffperf', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_boffperf', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j55_boffperf', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j85_boffperf', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j110_boffperf', 'L1_J30', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1], 
        ['j150_boffperf', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j260_boffperf', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j320_boffperf', 'L1_J85', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j400_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_boffperf_3j45', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_boffperf_3j45_L13J15.0ETA25', 'L1_3J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_boffperf_3j45_L13J20', 'L1_3J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        
        # Calibration chains (split, offline taggers)
        ['j15_boffperf_split', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j25_boffperf_split', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j35_boffperf_split', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_boffperf_split', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j55_boffperf_split', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j85_boffperf_split', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j110_boffperf_split', 'L1_J30', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1], 
        ['j150_boffperf_split', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j260_boffperf_split', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j320_boffperf_split', 'L1_J85', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j400_boffperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_boffperf_split_3j45', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_boffperf_split_3j45_L13J15.0ETA25', 'L1_3J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_boffperf_split_3j45_L13J20', 'L1_3J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],


        # physics chains for testing offline taggers                                                                                                 
        ['2j35_bmv2c2070_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],                             
        ['2j45_bmv2c2077_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],                             
        ['j175_bmv2c2085', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],                                                      
        ['2j35_bmv2c2070_split_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],                       
        ['2j45_bmv2c2077_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],                       
        ['j175_bmv2c2085_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],                                                
        #set of chains will allow us to make rates estimates for different MV2c20 WPs early on in the 25 ns run.
        ['2j65_boffperf_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_boffperf_2j45_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j35_boffperf_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j70_boffperf_3j70_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j65_boffperf_3j65_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j45_boffperf_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j35_boffperf_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j70_boffperf_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j65_boffperf_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j225_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j150_boffperf_j50_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j175_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        

        #bperf                                                                                                                                       
        ['j55_bperf_split', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_bperf_split_3j45', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_bperf_split_3j45_L13J15.0ETA25', 'L1_3J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j45_bperf_split_3j45_L13J20', 'L1_3J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],


        # L1Topo VBF MJJ bjets
        ['2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-400', 'L1_J30_2J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400', 'L1_J30_2J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-700', 'L1_J30_2J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700', 'L1_J30_2J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-800', 'L1_J30_2J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800', 'L1_J30_2J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-900', 'L1_J30_2J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900', 'L1_J30_2J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bloose_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bloose_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bloose_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bloose_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-400', 'L1_J30_2J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-700', 'L1_J30_2J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-800', 'L1_J30_2J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-900', 'L1_J30_2J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],


        ]
    
    TriggerFlags.METSlice.signatures = [
           # Rerun chains
        ['xe0noL1_l2fsperf',         '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_lcw',  '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_em',   '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_mht',     '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_pueta',   '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_pufit',   '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe35',                                   'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw',                            'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_em',                             'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pueta',                             'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pufit',                             'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_mht',                               'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_L2FS',                              'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_l2fsperf_wEFMuFEB_wEFMu',           'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe35_wEFMu',                    'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw_wEFMu',             'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_em_wEFMu',              'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_mht_wEFMu',                'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pueta_wEFMu',              'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pufit_wEFMu',              'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe35_L1XE35_BGRP7',                                   'L1_XE35_BGRP7',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw_L1XE35_BGRP7',                            'L1_XE35_BGRP7',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_em_L1XE35_BGRP7',                             'L1_XE35_BGRP7',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pueta_L1XE35_BGRP7',                             'L1_XE35_BGRP7',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pufit_L1XE35_BGRP7',                             'L1_XE35_BGRP7',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_mht_L1XE35_BGRP7',                               'L1_XE35_BGRP7',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_L2FS_L1XE35_BGRP7',                              'L1_XE35_BGRP7',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_l2fsperf_wEFMuFEB_wEFMu_L1XE35_BGRP7',           'L1_XE35_BGRP7',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_wEFMu_L1XE35_BGRP7',                    'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw_wEFMu_L1XE35_BGRP7',             'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_em_wEFMu_L1XE35_BGRP7',              'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_mht_wEFMu_L1XE35_BGRP7',                'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pueta_wEFMu_L1XE35_BGRP7',              'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pufit_wEFMu_L1XE35_BGRP7',              'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe50',                                   'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50_tc_lcw',                            'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50_tc_em',                             'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50_mht',                               'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50_pueta',                             'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50_pufit',                             'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe50_L1XE35_BGRP7',                                   'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50_tc_lcw_L1XE35_BGRP7',                            'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50_tc_em_L1XE35_BGRP7',                             'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50_mht_L1XE35_BGRP7',                               'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50_pueta_L1XE35_BGRP7',                             'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50_pufit_L1XE35_BGRP7',                             'L1_XE35_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe60',                     'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_lcw',              'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_em',               'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_mht',                 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pueta',               'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pufit',               'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_wEFMu',               'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_lcw_wEFMu',        'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_em_wEFMu',         'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_mht_wEFMu',           'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pueta_wEFMu',         'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pufit_wEFMu',         'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe60_L1XE40_BGRP7',                     'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_lcw_L1XE40_BGRP7',              'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_em_L1XE40_BGRP7',               'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_mht_L1XE40_BGRP7',                 'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pueta_L1XE40_BGRP7',               'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pufit_L1XE40_BGRP7',               'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_wEFMu_L1XE40_BGRP7',               'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_lcw_wEFMu_L1XE40_BGRP7',        'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_em_wEFMu_L1XE40_BGRP7',         'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_mht_wEFMu_L1XE40_BGRP7',           'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pueta_wEFMu_L1XE40_BGRP7',         'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pufit_wEFMu_L1XE40_BGRP7',         'L1_XE40_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe70_wEFMu',                    'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_lcw_wEFMu',             'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_em_wEFMu',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_mht_wEFMu',                'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pueta_wEFMu',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pufit_wEFMu',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe70_wEFMu_L1XE50_BGRP7',           'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_lcw_wEFMu_L1XE50_BGRP7',    'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_em_wEFMu_L1XE50_BGRP7',     'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_mht_wEFMu_L1XE50_BGRP7',       'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pueta_wEFMu_L1XE50_BGRP7',     'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pufit_wEFMu_L1XE50_BGRP7',     'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_L1XE45',                       'L1_XE45', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ####defaul chains

        ['xe70',                     'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_lcw',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_em',               'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_mht',                 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pueta',               'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pufit',               'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe70_L1XE50_BGRP7',                     'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_lcw_L1XE50_BGRP7',              'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_em_L1XE50_BGRP7',               'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_mht_L1XE50_BGRP7',                 'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pueta_L1XE50_BGRP7',               'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pufit_L1XE50_BGRP7',               'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE50',       'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE50',          'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_L1XE50_BGRP7',              'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE50_BGRP7',       'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE50_BGRP7',        'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE50_BGRP7',          'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE50_BGRP7',        'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE50_BGRP7',        'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe80_wEFMu_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_wEFMu_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_wEFMu_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_wEFMu_L1XE50',    'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_wEFMu_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_wEFMu_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe80_wEFMu_L1XE50_BGRP7',        'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_wEFMu_L1XE50_BGRP7', 'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_wEFMu_L1XE50_BGRP7',  'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_wEFMu_L1XE50_BGRP7',    'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_wEFMu_L1XE50_BGRP7',  'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_wEFMu_L1XE50_BGRP7',  'L1_XE50_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80',                     'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw',              'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em',               'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht',                 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta',               'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit',               'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_L1XE60_BGRP7',                     'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE60_BGRP7',              'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE60_BGRP7',               'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE60_BGRP7',                 'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE60_BGRP7',               'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE60_BGRP7',               'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe80_wEFMu',               'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_wEFMu',        'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_wEFMu',         'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_wEFMu',           'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_wEFMu',         'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_wEFMu',         'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe80_wEFMu_L1XE60_BGRP7',               'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_wEFMu_L1XE60_BGRP7',        'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_wEFMu_L1XE60_BGRP7',         'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_wEFMu_L1XE60_BGRP7',           'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_wEFMu_L1XE60_BGRP7',         'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_wEFMu_L1XE60_BGRP7',         'L1_XE60_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe80_L1XE70',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE70',       'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE70',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE70',          'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE70',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE70',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe80_L1XE70_BGRP7',              'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE70_BGRP7',       'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE70_BGRP7',        'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE70_BGRP7',          'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE70_BGRP7',        'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE70_BGRP7',        'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe80_wEFMu_L1XE70',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_wEFMu_L1XE70', 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_wEFMu_L1XE70',  'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_wEFMu_L1XE70',    'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_wEFMu_L1XE70',  'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_wEFMu_L1XE70',  'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_wEFMu_L1XE70_BGRP7',        'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_wEFMu_L1XE70_BGRP7', 'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_wEFMu_L1XE70_BGRP7',  'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_wEFMu_L1XE70_BGRP7',    'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_wEFMu_L1XE70_BGRP7',  'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_wEFMu_L1XE70_BGRP7',  'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe90',                                   'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_lcw',             'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_em',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_mht',                'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pueta',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pufit',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe90_L1XE70_BGRP7',                    'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_lcw_L1XE70_BGRP7',             'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_em_L1XE70_BGRP7',              'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_mht_L1XE70_BGRP7',                'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pueta_L1XE70_BGRP7',              'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pufit_L1XE70_BGRP7',              'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe90_wEFMu',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_lcw_wEFMu',       'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_em_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_mht_wEFMu',          'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pueta_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pufit_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe90_wEFMu_L1XE70_BGRP7',              'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_lcw_wEFMu_L1XE70_BGRP7',       'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_em_wEFMu_L1XE70_BGRP7',        'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_mht_wEFMu_L1XE70_BGRP7',          'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pueta_wEFMu_L1XE70_BGRP7',        'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pufit_wEFMu_L1XE70_BGRP7',        'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ['xe90_L1XE50',                    'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_lcw_L1XE50',             'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_em_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_mht_L1XE50',                'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pueta_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pufit_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe90_wEFMu_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_lcw_wEFMu_L1XE50',       'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_em_wEFMu_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_mht_wEFMu_L1XE50',          'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pueta_wEFMu_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pufit_wEFMu_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        
        ['xe100_L1XE50',                    'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_L1XE50',             'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_L1XE50',                'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        
        ['xe100_wEFMu_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_wEFMu_L1XE50',       'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_wEFMu_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_wEFMu_L1XE50',          'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_wEFMu_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_wEFMu_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        
        ##add muon corrections
        ['xe100',                    'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw',             'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht',                'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100_L1XE70_BGRP7',                    'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_L1XE70_BGRP7',             'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_L1XE70_BGRP7',              'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_L1XE70_BGRP7',                'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_L1XE70_BGRP7',              'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_L1XE70_BGRP7',              'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100_wEFMu',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_wEFMu',       'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_wEFMu',          'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100_wEFMu_L1XE70_BGRP7',              'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_wEFMu_L1XE70_BGRP7',       'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_wEFMu_L1XE70_BGRP7',        'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_wEFMu_L1XE70_BGRP7',          'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_wEFMu_L1XE70_BGRP7',        'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_wEFMu_L1XE70_BGRP7',        'L1_XE70_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ###xe100 from L1_XE80
        ['xe100_L1XE80',                    'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_L1XE80',             'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_L1XE80',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_L1XE80',                'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_L1XE80',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_L1XE80',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100_L1XE80_BGRP7',                    'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_L1XE80_BGRP7',             'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_L1XE80_BGRP7',              'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_L1XE80_BGRP7',                'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_L1XE80_BGRP7',              'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_L1XE80_BGRP7',              'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100_wEFMu_L1XE80',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_wEFMu_L1XE80',       'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_wEFMu_L1XE80',        'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_wEFMu_L1XE80',          'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_wEFMu_L1XE80',        'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_wEFMu_L1XE80',        'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100_wEFMu_L1XE80_BGRP7',              'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_wEFMu_L1XE80_BGRP7',       'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_wEFMu_L1XE80_BGRP7',        'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_wEFMu_L1XE80_BGRP7',          'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_wEFMu_L1XE80_BGRP7',        'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_wEFMu_L1XE80_BGRP7',        'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ###xe120 from L1_XE80
        ['xe120',                    'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_tc_lcw',             'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_tc_em',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_mht',                'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_pueta',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_pufit',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe120_L1XE80_BGRP7',                    'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_tc_lcw_L1XE80_BGRP7',             'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_tc_em_L1XE80_BGRP7',              'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_mht_L1XE80_BGRP7',                'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_pueta_L1XE80_BGRP7',              'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_pufit_L1XE80_BGRP7',              'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe120_wEFMu',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_tc_lcw_wEFMu',       'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_tc_em_wEFMu',        'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_mht_wEFMu',          'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_pueta_wEFMu',        'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_pufit_wEFMu',        'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe120_wEFMu_L1XE80_BGRP7',              'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_tc_lcw_wEFMu_L1XE80_BGRP7',       'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_tc_em_wEFMu_L1XE80_BGRP7',        'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_mht_wEFMu_L1XE80_BGRP7',          'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_pueta_wEFMu_L1XE80_BGRP7',        'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_pufit_wEFMu_L1XE80_BGRP7',        'L1_XE80_BGRP7', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['te20',                                   'L1_TE40',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['te20_tc_lcw',                            'L1_TE40',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        
        ['xs15_L1XS20',                            'L1_XS20',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xs20_L1XS30',                            'L1_XS30',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xs30',                                   'L1_XS30',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xs30_tc_lcw',                            'L1_XS30',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        # ATR-10848 HMT min bias
        ['te50_L1MBTS_1_1',              'L1_MBTS_1_1',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['te50_L1RD3_FILLED',            'L1_RD3_FILLED',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ]
    
    TriggerFlags.TauSlice.signatures = [
        # Energy calibration chain
        ['tau5_perf_ptonly_L1TAU8',                'L1_TAU8', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Energy calibration chain
        ['tau0_perf_ptonly_L1TAU12',               'L1_TAU12', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau0_perf_ptonly_L1TAU60',               'L1_TAU60', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-I comparison
        ['tau20_r1_idperf',                        'L1_TAU12', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        ['tau25_r1_idperf',                        'L1_TAU12', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        ['tau25_idperf_track',                     'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        ['tau25_idperf_tracktwo',                  'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        # Run-II - No BDT: main track-based items
        ['tau25_perf_track',                       'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_track_L1TAU12',               'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwo',                    'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwo_L1TAU12',            'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - No BDT: variations
        ['tau25_perf_ptonly',                      'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_ptonly_L1TAU12',              'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - With BDT: main track-based items
        ['tau25_medium1_track',                    'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_loose1_tracktwo',                  'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo',                 'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tight1_tracktwo',                  'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo_L1TAU12',         'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo_L1TAU12IL',       'L1_TAU12IL', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo_L1TAU12IT',       'L1_TAU12IT', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - With BDT: main calo-based items
        # Run-II - With BDT: pt only
        ['tau25_loose1_ptonly',                    'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_ptonly',                   'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tight1_ptonly',                    'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - With BDT: no pre-selection
        ['tau25_medium1_mvonly',                   'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - High-pT variations
        ['tau35_loose1_tracktwo',                  'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_loose1_ptonly',                    'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_track',                    'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo',                 'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo_L1TAU20',         'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo_L1TAU20IL',       'L1_TAU20IL', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo_L1TAU20IT',       'L1_TAU20IT', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_ptonly',                   'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_ptonly_L1TAU20',           'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_tight1_tracktwo',                  'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_tight1_ptonly',                    'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_perf_tracktwo',                    'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_perf_ptonly',                      'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau50_medium1_tracktwo_L1TAU12',         'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_track',                    'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwo',                 'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwo_L1TAU60',         'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_track',                   'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_tracktwo',                'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_perf_tracktwo',                   'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_perf_ptonly',                     'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_medium1_tracktwo',                'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_idperf_track',                    'L1_TAU60', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        ['tau160_idperf_tracktwo',                 'L1_TAU60', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        #
        ['tau8_cosmic_track', 'L1_TAU8_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau8_cosmic_ptonly', 'L1_TAU8_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau1_cosmic_track_L1MU4_EMPTY',  'L1_MU4_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau1_cosmic_ptonly_L1MU4_EMPTY', 'L1_MU4_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        
        ['tau35_perf_tracktwo_tau25_perf_tracktwo', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
        ['tau35_perf_tracktwo_L1TAU20_tau25_perf_tracktwo_L1TAU12', 'L1_TAU20_2TAU12' , ['L1_TAU20','L1_TAU12'],[PhysicsStream], ['RATE:MultiTau','BW:Tau'], -1],
        

        ]



    TriggerFlags.EgammaSlice.signatures = [

        ['g0_perf_L1EM3_EMPTY',                  'L1_EM3_EMPTY', [], [PhysicsStream], ['RATE:SinglePhoton',   'BW:Egamma'], -1],        
        ['e0_perf_L1EM3_EMPTY',                  'L1_EM3_EMPTY', [], [PhysicsStream], ['RATE:SinglePhoton',   'BW:Egamma'], -1],        

        ['g10_loose',                            'L1_EM7',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose_L1EM15',                     'L1_EM15', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g10_etcut',                     'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_etcut_L1EM12',              'L1_EM12',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g3_loose_larpeb',                      'L1_EM3',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g12_loose_larpeb',                      'L1_EM10VH',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
	['g20_loose_larpeb_L1EM15',              'L1_EM15', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_loose_larpeb',                     'L1_EM20VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_larpeb',                     'L1_EM20VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_larpeb',                     'L1_EM20VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],        
        ['g200_etcut',                    'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 

        ##########
        # Monopole triggers
        ['g0_hiptrt_L1EM18VH',                    'L1_EM18VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g0_hiptrt_L1EM20VH',                    'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g0_hiptrt_L1EM20VHI',                   'L1_EM20VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g0_hiptrt_L1EM22VHI',                 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],


        # PS-ed trigger to supmbined chains
        ['g15_loose_L1EM7',               'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g20_loose_L1EM12',              'L1_EM12',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], # pass through
        ['g40_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g45_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g50_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g80_loose',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g100_loose',                    'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 

        # Rerun mode and PS
        ['g25_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g30_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g70_loose',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        # Rerun mode
        ['g10_medium',                    'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g15_loose_L1EM3',               'L1_EM3',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g15_loose',                     'L1_EM13VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose',                     'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_tight',                     'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_tight',                     'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g45_tight',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g50_loose',                     'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_L1EM15VH',            'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        # Di-photon triggers
        ['g35_loose_L1EM15_g25_loose_L1EM15',       'L1_2EM15', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g35_medium1_g25_medium1',                 'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_tight',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g50_loose',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g60_loose_L12EM15VH',                    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],  
        ##Adding tight diphoton triggers (ATR-10762)
        ['2g22_tight',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g25_tight',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

        # Tri-photon triggers
        ['3g15_loose',                              'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g20_loose_2g15_loose_L12EM13VH',          'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_loose_g15_loose',                    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['3g20_loose',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 

        #prescaled
        ['2g20_loose_L12EM15',                      'L1_2EM15', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_loose',                   'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 


        ['2g10_loose',                           'L1_2EM7', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

        ['e17_loose_L1EM15',                     'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_L1EM15',                   'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_cutd0dphideta_L1EM15',     'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nod0_L1EM15',              'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nodeta_L1EM15',            'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nodphires_L1EM15',         'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        #electron supporting chains
        ['e24_tight_iloose_L2EFCalo_L1EM20VH',    'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_iloose_L2EFCalo_L1EM20VH',  'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        #nod0 chains:
       	['e24_lhvloose_nod0_L1EM18VH',                'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],  
        ['e24_lhvloose_nod0_L1EM20VH',                'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],  
        ['e26_lhvloose_nod0_L1EM20VH',                'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],  

        ['e24_medium_L1EM20VH',                  'L1_EM20VH',    [], [PhysicsStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_L1EM20VH',                'L1_EM20VH',    [], [PhysicsStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # Single electron/photon chains for Calo sequence optimization (kept as before)
        ['e24_lhtight_iloose_HLTCalo_L1EM20VH',   'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g35_medium_HLTCalo_g25_medium_HLTCalo', 'L1_2EM15VH',  [], [PhysicsStream], ['RATE:MultiPhoton','BW:Egamma'],-1],

        ['2e15_lhloose_cutd0dphideta_L12EM13VH',  'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e17_lhloose_cutd0dphideta',            'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        ['2e12_vloose_L12EM10VH',                'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e12_lhvloose_L12EM10VH',              'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e12_lhvloose_nod0_L12EM10VH',         'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        ['2e12_loose_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e12_lhloose_L12EM10VH',               'L1_2EM10VH', [], [PhysicsStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e12_lhloose_cutd0dphideta_L12EM10VH', 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e12_lhloose_nod0_L12EM10VH', 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e12_lhloose_nodeta_L12EM10VH', 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e12_lhloose_nodphires_L12EM10VH', 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        ['2e17_loose',                      'L1_2EM15VH', [], [PhysicsStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['2e17_lhloose',                    'L1_2EM15VH', [], [PhysicsStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'],-1], 

        #Prescaled performance:
        ['e5_lhtight_nod0_e4_etcut_Jpsiee',  'L1_2EM3',      [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e9_lhtight_nod0_e4_etcut_Jpsiee',  'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e9_etcut_e5_lhtight_nod0_Jpsiee',  'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e14_lhtight_nod0_e4_etcut_Jpsiee', 'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e14_etcut_e5_lhtight_nod0_Jpsiee', 'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        
        # Supporting trigger
        ['e0_perf_L1EM15',              'L1_EM15',[], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['g0_perf_L1EM15',                'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 
 
        ['e5_loose',                            'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e5_lhloose',                          'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_loose_idperf',                     'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e5_lhloose_idperf',                   'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e5_loose_L2Star_idperf',              'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e5_tight_idperf',                     'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],

        # extra id perf chains for TRT HT studies                                                                                               
        ['e10_tight_idperf',                     'L1_EM7',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e5_lhtight_idperf',                     'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e10_lhtight_idperf',                     'L1_EM7',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],

        ['e24_medium_idperf_L1EM20VH',          'L1_EM20VH',    [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e24_lhmedium_idperf_L1EM20VH',        'L1_EM20VH',    [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        # Single electron triggers

        ['e17_medium_L1EM15HI',                           'L1_EM15HI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_medium_iloose_L1EM15HI',                    'L1_EM15HI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_iloose_L1EM15HI',                  'L1_EM15HI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
	['e17_lhmedium_L1EM15HI',                         'L1_EM15HI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0_L1EM15HI',                    'L1_EM15HI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
	
#        ##########        
        # Single electron triggers
        ['e24_medium_L1EM18VH',                  'L1_EM18VH',    [], [PhysicsStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_tight_L1EM20VH',                   'L1_EM20VH',    [], [PhysicsStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_L1EM18VH',                'L1_EM18VH',    [], [PhysicsStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_L1EM20VH',                 'L1_EM20VH',    [], [PhysicsStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        
        ['e26_tight_iloose',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e26_lhtight_iloose',                   'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e26_lhtight_cutd0dphideta_iloose', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0_iloose', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e60_medium',                           'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhmedium',                         'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_lhmedium_cutd0dphideta', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhmedium_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e24_tight_iloose',                     'L1_EM20VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_lhtight_iloose',                   'L1_EM20VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_cutd0dphideta_iloose', 'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_nod0_iloose', 'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e24_medium_iloose_L1EM20VH',           'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_iloose',                  'L1_EM20VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_iloose_L1EM20VH',         'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_lhmedium_nod0_iloose_L1EM20VH', 'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_cutd0dphideta_iloose_L1EM20VH', 'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e24_medium_iloose_L1EM18VH',           'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_iloose_L1EM18VH',         'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_lhmedium_cutd0dphideta_iloose_L1EM18VH',       'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_iloose_L1EM18VH',            'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nodeta_iloose_L1EM18VH',          'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nodphires_iloose_L1EM18VH',       'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e120_loose',                           'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e120_lhloose',                         'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e120_lhloose_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g140_loose',                    'L1_EM22VHI', [], [PhysicsStream, 'express'], ['RATE:SinglePhoton','BW:Egamma'],-1],

        # Run2 cut-based:
        ['e24_tight_iloose_L1EM20VH',            'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_loose',                           'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        # likelihood-based:
        ['e24_lhtight_iloose_L1EM20VH',          'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e140_lhloose',                         'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_lhtight_nod0_iloose_L1EM20VH', 'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_cutd0dphideta_iloose_L1EM20VH', 'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_tight_iloose_HLTCalo_L1EM20VH',     'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_iloose_HLTCalo_L1EM18VH', 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_iloose_HLTCalo_L1EM20VH', 'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_iloose_HLTCalo', 'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_iloose_HLTCalo', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhmedium_HLTCalo', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e120_lhloose_HLTCalo', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_HLTCalo', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        # Single electron trigger for W mass measurement (ATR-11156)
        ['e24_lhtight_smooth_L1EM20VH',                 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_smooth_iloose',                   'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # Prescaled triggers
        # Rate = 1 Hz each
        ['e5_etcut',                             'L1_EM3',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e10_etcut_L1EM7',                      'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_etcut_L1EM7',                      'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_etcut_L1EM12',                     'L1_EM12',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],         
        ['e25_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e30_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e50_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_etcut',                            'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e70_etcut',                            'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_etcut',                            'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e100_etcut',                           'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e120_etcut',                           'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e200_etcut',                           'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        # Rate = 0.02 Hz each
        ['e5_vloose',                           'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e10_vloose_L1EM7',                    'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_vloose_L1EM7',                    'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_vloose_L1EM12',                   'L1_EM12',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e25_vloose_L1EM15',                   'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e30_vloose_L1EM15',                   'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_vloose_L1EM15',                   'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e50_vloose_L1EM15',                   'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_vloose',                          'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e70_vloose',                          'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e80_vloose',                          'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e100_vloose',                         'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e120_vloose',                         'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 

        ['e5_lhvloose',                         'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e10_lhvloose_L1EM7',                  'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_lhvloose_L1EM7',                  'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_lhvloose_L1EM12',                 'L1_EM12',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e25_lhvloose_L1EM15',                 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e30_lhvloose_L1EM15',                 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_lhvloose_L1EM15',                 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e50_lhvloose_L1EM15',                 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e70_lhvloose',                        'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e80_lhvloose',                        'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e100_lhvloose',                       'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e120_lhvloose',                       'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
	['e5_lhvloose_nod0',                         'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        # use coherent PS with lhvloose and lhvloose_nod0 variants below:
        ['e12_vloose_L1EM10VH',                 'L1_EM10VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_vloose_L1EM13VH',                 'L1_EM13VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e17_vloose',                          'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_vloose',                          'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_vloose',                       'L1_EM20VH',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e12_lhvloose_L1EM10VH',               'L1_EM10VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_lhvloose_L1EM13VH',               'L1_EM13VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e17_lhvloose',                        'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_lhvloose',                        'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_lhvloose',                       'L1_EM20VH',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_lhvloose',                       'L1_EM22VHI',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e12_lhvloose_nod0_L1EM10VH',               'L1_EM10VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_lhvloose_nod0_L1EM13VH',               'L1_EM13VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e17_lhvloose_nod0',                        'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
	['e20_lhvloose_nod0',                        'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_lhvloose_nod0',                       'L1_EM20VH',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_lhvloose_nod0',                        'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        #Single electron/chaing for testing mis-ant robust LH ID
        ['e24_lhmedium_nod0_L1EM18VH',            'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nodeta_L1EM18VH',          'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nodphires_L1EM18VH',       'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_cutd0dphideta_L1EM18VH',       'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #New nod0 chains:
        ['e24_lhtight_nod0_L1EM20VH',                 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        # Rerun mode
        ['e4_etcut',                             'L1_EM3',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e9_etcut',                             'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e14_etcut',                            'L1_EM12',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
	['e7_medium',                          'L1_EM3',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],        
	['e9_loose',                          'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_medium',                        'L1_EM8VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_medium',                            'L1_EM7',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_medium',                           'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_medium_iloose',                    'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_medium',                           'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_medium_L1EM15VH',                  'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_medium_L1EM20VHI',              'L1_EM20VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_medium_L1EM22VHI',              'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_loose',                            'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_loose',                            'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_tight',                             'L1_EM3',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_tight',                             'L1_EM7',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e14_tight',                            'L1_EM12',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

	['e7_lhmedium',                          'L1_EM3',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],        
	['e9_lhloose',                          'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhmedium',                        'L1_EM8VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_lhmedium',                          'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium',                         'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_iloose',                  'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_lhmedium',                         'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_L1EM15VH',                'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_L1EM20VHI',              'L1_EM20VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhmedium_L1EM22VHI',              'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e60_lhloose',                          'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhloose',                          'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_lhtight',                           'L1_EM3',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_lhtight',                           'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e14_lhtight',                          'L1_EM12',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

	['e7_lhmedium_nod0',                           'L1_EM3',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],        
	['e9_lhloose_nod0',                           'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
	['e9_lhmedium_nod0',                          'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhmedium_nod0',                        'L1_EM8VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0',                         'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0_iloose',                  'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_lhmedium_nod0',                         'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_L1EM15VH',                'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhloose_nod0',                          'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhloose_nod0',                          'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_nod0',                           'L1_EM3',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_lhtight_nod0',                           'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e14_lhtight_nod0',                          'L1_EM12',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_L1EM20VHI',              'L1_EM20VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhmedium_nod0_L1EM22VHI',              'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e12_lhloose_HLTCalo_L12EM10VH', 'L1_2EM10VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e15_lhloose_HLTCalo_L12EM13VH', 'L1_2EM13VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_HLTCalo', 'L1_2EM15VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

         # Supporting triggers
         # pairs of triggers (cut-based -- likelihood) should be in coherent PS
        ['e24_vloose_L1EM18VH',                  'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_vloose_L1EM20VH',                  'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_vloose_L1EM20VH',                  'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
         
        ['e24_lhvloose_L1EM18VH',                'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhvloose_L1EM20VH',                'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhvloose_L1EM20VH',                'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ##########
        # Single photon triggers        
        ['g120_loose',                    'L1_EM22VHI', [], [PhysicsStream, 'express'], ['RATE:SinglePhoton','BW:Egamma'],-1], 

        # rerun mode
        ['e12_loose',                            'L1_EM8VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_loose_L1EM10VH',                   'L1_EM10VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose',                          'L1_EM8VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_L1EM10VH',                 'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_cutd0dphideta_L1EM10VH',   'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nod0_L1EM10VH',                 'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nodeta_L1EM10VH',          'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nodphires_L1EM10VH',       'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nod0',                          'L1_EM8VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e17_loose',                            'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose',                          'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_cutd0dphideta',            'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nod0',                          'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e15_loose_L1EM13VH',                   'L1_EM13VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e15_lhloose_L1EM13VH',                 'L1_EM13VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e15_lhloose_cutd0dphideta_L1EM13VH',   'L1_EM13VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e15_lhloose_nod0_L1EM13VH',                 'L1_EM13VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e13_etcut_trkcut_L1EM10', 'L1_EM10', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e18_etcut_trkcut_L1EM15', 'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

        ['g25_loose',                     'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium',                    'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g30_loose',                     'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose',                     'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_medium',                    'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium_HLTCalo',             'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_medium_HLTCalo',             'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g140_loose_HLTCalo',                    'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1],


        # Di-electron triggers
        # cut-based
        ['2e15_loose_L12EM13VH',            'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['2e17_loose_L12EM15',              'L1_2EM15',   [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        
        # likelihood
        ['2e15_lhloose_L12EM13VH',          'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['2e17_lhloose_L12EM15',            'L1_2EM15',   [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        #New egamma chains for mc15a (alignment-robust nod0 & TRT commissioning)                                                                                                                    
        ['2e15_lhloose_nod0_L12EM13VH', 'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e17_lhloose_nod0', 'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        ['2e12_lhloose_HLTCalo_L12EM10VH', 'L1_2EM10VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['2e15_lhloose_HLTCalo_L12EM13VH', 'L1_2EM13VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['2e17_lhloose_HLTCalo', 'L1_2EM15VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['2e17_vloose',                          'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e17_lhvloose',                        'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e17_lhvloose_nod0',                   'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        ['2e15_vloose_L12EM13VH',                'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e15_lhvloose_L12EM13VH',              'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e15_lhvloose_nod0_L12EM13VH',         'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        #prescaled
        # cut-based
        ['e9_tight_e4_etcut_Jpsiee',                         'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['e9_etcut_e5_tight_Jpsiee',                         'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],         
        ['e14_tight_e4_etcut_Jpsiee',                        'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e14_etcut_e5_tight_Jpsiee',                        'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        
        # likelihood
        ['e9_lhtight_e4_etcut_Jpsiee',                       'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e9_etcut_e5_lhtight_Jpsiee',                       'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['e14_lhtight_e4_etcut_Jpsiee',                      'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e14_etcut_e5_lhtight_Jpsiee',                      'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],   

        # Trielectron triggers
        ['e17_loose_2e9_loose',             'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_medium_2e9_medium',           'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_lhloose_2e9_lhloose',         'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_medium_iloose_2e9_medium',    'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_lhmedium_2e9_lhmedium',       'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_lhmedium_iloose_2e9_lhmedium','L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
#Rerun:         
#Physics (tri-ele):
        ['e17_lhloose_nod0_2e9_lhloose_nod0',         'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_lhmedium_nod0_2e9_lhmedium_nod0',       'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_lhmedium_nod0_iloose_2e9_lhmedium_nod0','L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        # Di-EM triggers
        # cut-based
        ['e24_medium_L1EM15VH_g25_medium',    'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_medium_g35_loose',              'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        # likelihood
        ['e24_lhmedium_L1EM15VH_g25_medium',  'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_g35_loose',            'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 


#Physics (e+g):
        ['e24_lhmedium_nod0_L1EM15VH_g25_medium',  'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_nod0_g35_loose',            'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_nod0_2g10_loose',           'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_nod0_2g10_medium',          'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 


        # Multielectron chains with m_ee cut for Run2 PID (supporting L1Topo J/psi)                                                                          
        # cut-based                                                                                                                                          
        ['e5_tight_e4_etcut',                                'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        # likelihood                                                                                                                
        ['e5_lhtight_e4_etcut',                              'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_nod0_e4_etcut',                              'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        ['e5_tight_e4_etcut_Jpsiee',                         'L1_2EM3', [], [PhysicsStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['e5_lhtight_e4_etcut_Jpsiee',                       'L1_2EM3', [], [PhysicsStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'],-1], 


        # Z T&P triggers for monitoring
        ['e24_lhtight_L1EM20VH_e15_etcut_Zee',               'L1_EM20VH',  [], [PhysicsStream, 'express'], ['RATE:MultiElectron','BW:Egamma'],-1],
        ['e24_tight_L1EM20VH_e15_etcut_Zee',                 'L1_EM20VH',  [], [PhysicsStream, 'express'], ['RATE:MultiElectron','BW:Egamma'],-1],
        ['e26_lhtight_e15_etcut_Zee',                        'L1_EM22VHI', [], [PhysicsStream, 'express'], ['RATE:MultiElectron','BW:Egamma'],-1], 
        ['e26_tight_e15_etcut_Zee',                          'L1_EM22VHI', [], [PhysicsStream, 'express'], ['RATE:MultiElectron','BW:Egamma'],-1], 
        
        #Diphoton triggers
        ['g35_loose_g25_loose',                      'L1_2EM15VH', [], [PhysicsStream, 'express'], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['g35_medium_g25_medium',                    'L1_2EM15VH', [], [PhysicsStream, 'express'], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 

        # LLP
        ['g35_medium_g25_medium_L1EM7_EMPTY',        'L1_EM7_EMPTY', ['L1_EM7_EMPTY','L1_2EM15VH'], ["Late"], ['RATE:MultiPhoton', 'BW:Egamma'], -1],
        ['g35_medium_g25_medium_L1EM7_UNPAIRED_ISO', 'L1_EM7_UNPAIRED_ISO', ['L1_EM7_UNPAIRED_ISO','L1_2EM15VH'], ["Late"], ['RATE:MultiPhoton', 'BW:Egamma'], -1],

        # L1Topo JPSI
        ['e5_tight1_e4_etcut_L1JPSI-1M5',                   'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_tight1_e4_etcut_Jpsiee_L1JPSI-1M5',            'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_tight_e4_etcut_L1JPSI-1M5',                    'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5',             'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',         'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7',         'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',       'L1_JPSI-1M5-EM12', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12',       'L1_JPSI-1M5-EM12', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_lhtight_e4_etcut_L1JPSI-1M5',                  'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5',           'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',       'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7',       'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',     'L1_JPSI-1M5-EM12', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12',     'L1_JPSI-1M5-EM12', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],

        # L1Topo W T&P 
        ['e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE',  ['L1_EM10'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE', ['L1_EM15'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e5_etcut_L1W-05DPHI-JXE-0',           'L1_W-05DPHI-JXE-0',          ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-10DPHI-JXE-0',           'L1_W-10DPHI-JXE-0',          ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-15DPHI-JXE-0',           'L1_W-15DPHI-JXE-0',          ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-10DPHI-EMXE-0',          'L1_W-10DPHI-EMXE-0',         ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-15DPHI-EMXE-0',          'L1_W-15DPHI-EMXE-0',         ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-05DPHI-EMXE-1',          'L1_W-05DPHI-EMXE-1',         ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-05RO-XEHT-0',            'L1_W-05RO-XEHT-0',           ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-90RO2-XEHT-0',           'L1_W-90RO2-XEHT-0',          ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-250RO2-XEHT-0',          'L1_W-250RO2-XEHT-0',         ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-HT20-JJ15.ETA49',        'L1_W-HT20-JJ15.ETA49',       ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e13_etcut_L1W-NOMATCH',               'L1_W-NOMATCH',               ['L1_EM10'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT', 'L1_W-NOMATCH_W-05RO-XEEMHT', ['L1_EM10'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e13_etcut_L1EM10_W-MT25',             'L1_EM10_W-MT25',             ['L1_EM10'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e13_etcut_L1EM10_W-MT30',             'L1_EM10_W-MT30',             ['L1_EM10'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e18_etcut_L1EM15_W-MT35',             'L1_EM15_W-MT35',             ['L1_EM15'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e13_etcut_trkcut', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0',  ['L1_EM10'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e18_etcut_trkcut', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0', ['L1_EM15'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

        # L1Topo JPSI prescaled performance:
        ['e5_lhtight_nod0_e4_etcut_L1JPSI-1M5',                                'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5',           'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',           'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e9_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM7',       'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True], 
        ['e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',      'L1_JPSI-1M5-EM12', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e14_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM12',     'L1_JPSI-1M5-EM12', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],               
        
        ]

    TriggerFlags.BphysicsSlice.signatures = [

        ['2mu4_bBmumuxv2',                'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumux_BcmumuDsloose',    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumux_BpmumuKp',         'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bDimu',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1], 
        ['2mu4_bDimu_noinvm_novtx_ss',    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bDimu_novtx_noos',         'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],  
        ['2mu4_bJpsimumu',                'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bUpsimumu',                'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumu',                   'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ['mu6_mu4_bBmumuxv2',             'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumux_BcmumuDsloose', 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumux_BpmumuKp',      'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],        
        ['mu6_mu4_bDimu',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu_noinvm_novtx_ss', 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu_novtx_noos',      'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bJpsimumu',             'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bUpsimumu',             'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumu',                'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ['2mu6_bBmumux_BcmumuDsloose',    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bDimu',                    'L1_2MU6', [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1], 
        ['2mu6_bDimu_noinvm_novtx_ss',    'L1_2MU6', [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bDimu_novtx_noos',         'L1_2MU6', [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],  
        ['2mu6_bJpsimumu',                'L1_2MU6', [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bUpsimumu',                'L1_2MU6', [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumu',                   'L1_2MU6', [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],

        ['2mu10_bBmumuxv2',               'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bBmumux_BcmumuDsloose',   'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bBmumux_BpmumuKp',        'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu',                   'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu_noinvm_novtx_ss',   'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu_novtx_noos',        'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bJpsimumu',               'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bUpsimumu',               'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bBmumu',                  'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ['mu4_bJpsi_Trkloose',            'L1_MU4',  [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_bJpsi_Trkloose',            'L1_MU6',  [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu10_bJpsi_Trkloose',           'L1_MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu18_bJpsi_Trkloose',           'L1_MU15', [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],

        ['3mu4_bDimu',                    'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu4_bJpsi',                    'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu4_bTau',                     'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu4_bUpsi',                    'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ['3mu6_bDimu',                   'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_bJpsi',                   'L1_3MU6', [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_bTau',                    'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_bUpsi',                   'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ['mu4_iloose_mu4_11invm60_noos', 'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu4_iloose_mu4_7invm9_noos',   'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_iloose_mu6_11invm24_noos', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_iloose_mu6_24invm60_noos', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ['mu4_iloose_mu4_11invm60_noos_novtx', 'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu4_iloose_mu4_7invm9_noos_novtx',   'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_iloose_mu6_11invm60_noos_novtx', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_iloose_mu6_11invm24_noos_novtx', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_iloose_mu6_24invm60_noos_novtx', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ['mu4_mu4_idperf_bJpsimumu_noid',  'L1_2MU4', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring','BW:Bphys', 'BW:ID'], -1],  
        ['2mu6_bBmumux_BpmumuKp',    'L1_2MU6',     [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumuxv2',              'L1_2MU6', [], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu13_mu13_idperf_Zmumu',     'L1_2MU10', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring','BW:Bphys', 'BW:ID'], -1],

        # Bphysics di-muon triggers w/o MuComb (ATR-11637)
        ['mu10_mu6_bBmumuxv2', 'L1_MU10_2MU6', ['L1_MU10','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['mu10_mu6_bBmumux_BcmumuDsloose', 'L1_MU10_2MU6', ['L1_MU10','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],	
        ['mu10_mu6_bBmumux_BpmumuKp',      'L1_MU10_2MU6', ['L1_MU10','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],        

        # chains with MSOnly muons at L2
        ['2mu4_bDimu_noL2',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bJpsimumu_noL2',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_l2msonly_bDimu_noL2',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_l2msonly_bJpsimumu_noL2',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu4_mu4_l2msonly_bDimu_noL2',                    'L1_2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu4_mu4_l2msonly_bJpsimumu_noL2',                    'L1_2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu_noL2',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bJpsimumu_noL2',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_l2msonly_mu4_l2msonly_bDimu_noL2',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_l2msonly_mu4_l2msonly_bJpsimumu_noL2',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_l2msonly_mu4_bDimu_noL2',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_l2msonly_mu4_bJpsimumu_noL2',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_l2msonly_bDimu_noL2',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_l2msonly_bJpsimumu_noL2',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bDimu_noL2',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bJpsimumu_noL2',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_l2msonly_bDimu_noL2',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_l2msonly_bJpsimumu_noL2',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu6_l2msonly_bDimu_noL2',                    'L1_2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu6_l2msonly_bJpsimumu_noL2',                    'L1_2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu_noL2',                    'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bJpsimumu_noL2',                    'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_l2msonly_bDimu_noL2',                    'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_l2msonly_bJpsimumu_noL2',                    'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu10_mu10_l2msonly_bDimu_noL2',                    'L1_2MU10', ['L1_MU10','L1_MU10'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu10_mu10_l2msonly_bJpsimumu_noL2',                    'L1_2MU10', ['L1_MU10','L1_MU10'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        # L1Topo BPH 2MU4
        ['2mu4_bJpsimumu_L1BPH-DR-2MU4',                            'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumu_L1BPH-DR-2MU4',                                'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4',                             'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4',                 'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-DR-2MU4',                                 'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4',                      'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bJpsimumu_L1BPH-2M-2MU4',                          'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumu_L1BPH-2M-2MU4',                             'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bUpsimumu_L1BPH-2M-2MU4',                          'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-2M-2MU4',                          'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4',              'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-2M-2MU4',                              'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4',                   'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bBmumu_L1BPH-4M8-2MU4',                               'L1_BPH-4M8-2MU4',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-4M8-2MU4',                            'L1_BPH-4M8-2MU4',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4',                'L1_BPH-4M8-2MU4',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-4M8-2MU4',                                'L1_BPH-4M8-2MU4',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4',                     'L1_BPH-4M8-2MU4',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bJpsimumu_L1BPH-DR-2MU4-BPH-2M-2MU4',             'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-BPH-2M-2MU4',                'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-2M-2MU4',             'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-2M-2MU4', 'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-DR-2MU4-BPH-2M-2MU4',                 'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-2M-2MU4',      'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bBmumu_L1BPH-DR-2MU4-BPH-4M8-2MU4',                  'L1_BPH-DR-2MU4_BPH-4M8-2MU4',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-4M8-2MU4',               'L1_BPH-DR-2MU4_BPH-4M8-2MU4',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-4M8-2MU4',   'L1_BPH-DR-2MU4_BPH-4M8-2MU4',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-DR-2MU4-BPH-4M8-2MU4',                   'L1_BPH-DR-2MU4_BPH-4M8-2MU4',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-4M8-2MU4',        'L1_BPH-DR-2MU4_BPH-4M8-2MU4',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        # L1Topo DY L1_2MU4
        ['mu4_iloose_mu4_7invm9_noos_L1DY-BOX-2MU4',     'L1_DY-BOX-2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['mu4_iloose_mu4_11invm60_noos_L1DY-BOX-2MU4',   'L1_DY-BOX-2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['mu4_iloose_mu4_7invm9_noos_novtx_L1DY-BOX-2MU4',     'L1_DY-BOX-2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['mu4_iloose_mu4_11invm60_noos_novtx_L1DY-BOX-2MU4',   'L1_DY-BOX-2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        # L1Topo BPH L1_2MU6
        ['2mu6_bJpsimumu_L1BPH-DR-2MU6',                             'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumu_L1BPH-DR-2MU6',                                'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumuxv2_L1BPH-DR-2MU6',                             'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6',                 'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bDimu_L1BPH-DR-2MU6',                                 'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6',                      'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu6_bJpsimumu_L1BPH-2M-2MU6',                          'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumu_L1BPH-2M-2MU6',                             'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bUpsimumu_L1BPH-2M-2MU6',                          'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumuxv2_L1BPH-2M-2MU6',                          'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumux_BcmumuDsloose_L1BPH-2M-2MU6',              'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bDimu_L1BPH-2M-2MU6',                              'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bDimu_novtx_noos_L1BPH-2M-2MU6',                   'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu6_bBmumu_L1BPH-4M8-2MU6',                               'L1_BPH-4M8-2MU6',                ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumuxv2_L1BPH-4M8-2MU6',                            'L1_BPH-4M8-2MU6',                ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU6',                'L1_BPH-4M8-2MU6',                ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bDimu_L1BPH-4M8-2MU6',                                'L1_BPH-4M8-2MU6',                ['L1_2MU6'], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bDimu_novtx_noos_L1BPH-4M8-2MU6',                     'L1_BPH-4M8-2MU6',                ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu6_bJpsimumu_L1BPH-DR-2MU6-BPH-2M-2MU6',             'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumu_L1BPH-DR-2MU6-BPH-2M-2MU6',                'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-2M-2MU6',             'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-2M-2MU6', 'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bDimu_L1BPH-DR-2MU6-BPH-2M-2MU6',                 'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-2M-2MU6',      'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu6_bBmumu_L1BPH-DR-2MU6-BPH-4M8-2MU6',                  'L1_BPH-DR-2MU6_BPH-4M8-2MU6',   ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-4M8-2MU6',               'L1_BPH-DR-2MU6_BPH-4M8-2MU6',   ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-4M8-2MU6',   'L1_BPH-DR-2MU6_BPH-4M8-2MU6',   ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bDimu_L1BPH-DR-2MU6-BPH-4M8-2MU6',                   'L1_BPH-DR-2MU6_BPH-4M8-2MU6',   ['L1_2MU6'], [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-4M8-2MU6',        'L1_BPH-DR-2MU6_BPH-4M8-2MU6',   ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],       
                
        ['mu6_iloose_mu6_11invm24_noos_L1DY-BOX-2MU6', 'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['mu6_iloose_mu6_24invm60_noos_L1DY-BOX-2MU6', 'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['mu6_iloose_mu6_11invm24_noos_novtx_L1DY-BOX-2MU6', 'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['mu6_iloose_mu6_24invm60_noos_novtx_L1DY-BOX-2MU6', 'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bJpsimumu_L1BPH-DR-2MU4-B',                            'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-B',                                'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-B',                             'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B',                 'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-DR-2MU4-B',                                 'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B',                      'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bJpsimumu_L1BPH-2M-2MU4-B',                          'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumu_L1BPH-2M-2MU4-B',                             'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bUpsimumu_L1BPH-2M-2MU4-B',                          'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-2M-2MU4-B',                          'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-B',              'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-2M-2MU4-B',                              'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-B',                   'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bBmumu_L1BPH-4M8-2MU4-B',                               'L1_BPH-4M8-2MU4-B',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-4M8-2MU4-B',                            'L1_BPH-4M8-2MU4-B',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-B',                'L1_BPH-4M8-2MU4-B',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-4M8-2MU4-B',                                'L1_BPH-4M8-2MU4-B',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-B',                     'L1_BPH-4M8-2MU4-B',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bJpsimumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B',             'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B',                'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B',             'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B',                 'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B',      'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B',                  'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B',               'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B',   'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B',                   'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B',        'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bJpsimumu_L1BPH-DR-2MU4-BO',                            'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-BO',                                'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO',                             'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO',                 'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-DR-2MU4-BO',                                 'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO',                      'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bJpsimumu_L1BPH-2M-2MU4-BO',                          'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumu_L1BPH-2M-2MU4-BO',                             'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bUpsimumu_L1BPH-2M-2MU4-BO',                          'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-2M-2MU4-BO',                          'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-BO',              'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-2M-2MU4-BO',                              'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-BO',                   'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bBmumu_L1BPH-4M8-2MU4-BO',                               'L1_BPH-4M8-2MU4-BO',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-4M8-2MU4-BO',                            'L1_BPH-4M8-2MU4-BO',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-BO',                'L1_BPH-4M8-2MU4-BO',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-4M8-2MU4-BO',                                'L1_BPH-4M8-2MU4-BO',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-BO',                     'L1_BPH-4M8-2MU4-BO',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bJpsimumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO',             'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO',                'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO',             'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO',                 'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO',      'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO',                  'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO',               'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO',   'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO',                   'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO',        'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, True],

        ['mu6_mu4_bDimu_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bDimu_L1BPH-4M8-MU6MU4',  'L1_BPH-4M8-MU6MU4',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4','L1_BPH-DR-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4',   'L1_BPH-DR-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumu_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4',  'L1_BPH-4M8-MU6MU4',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumu_L1BPH-DR-MU6MU4',   'L1_BPH-DR-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4',   'L1_BPH-DR-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4',  'L1_BPH-4M8-MU6MU4',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4',   'L1_BPH-DR-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],

        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-B',   'L1_BPH-2M-MU6MU4-B', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-B',  'L1_BPH-4M8-MU6MU4-B',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-B',   'L1_BPH-DR-MU6MU4-B', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-B',   'L1_BPH-2M-MU6MU4-B', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-B',  'L1_BPH-4M8-MU6MU4-B',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-B',   'L1_BPH-DR-MU6MU4-B', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],

        ['mu6_mu4_bDimu_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bDimu_L1BPH-4M8-MU6MU4-BO',  'L1_BPH-4M8-MU6MU4-BO',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4-BO','L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumu_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4-BO',  'L1_BPH-4M8-MU6MU4-BO',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumu_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4-BO',  'L1_BPH-4M8-MU6MU4-BO',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-BO',  'L1_BPH-4M8-MU6MU4-BO',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-BO',  'L1_BPH-4M8-MU6MU4-BO',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,True],

        
        ]

    TriggerFlags.CombinedSlice.signatures = [
    
        # Non-L1Topo W TP commissioning triggers
        ['e13_etcut_trkcut_xs15', 'L1_EM10_XS20', ['L1_EM10',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs15"]]],
        ['e18_etcut_trkcut_xs20', 'L1_EM15_XS30', ['L1_EM15',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs20"]]],
	
        #W T&P triggers

        ['e13_etcut_trkcut_xs15_mt25', 'L1_EM10_XS20', ['L1_EM10',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs15"]]],
        ['e18_etcut_trkcut_xs20_mt35', 'L1_EM15_XS30', ['L1_EM15',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs20"]]],

        ['e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05',      'L1_EM10_XS20', ['L1_EM10','','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xs15","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05_mt25', 'L1_EM10_XS20', ['L1_EM10','','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xs15","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_2dphi05_mt25',      'L1_EM10_XS20', ['L1_EM10','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_2dphi05',           'L1_EM10_XS20', ['L1_EM10','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],

        ['e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05',      'L1_EM10_XS20', ['L1_EM10','','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xs15","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05_mt25', 'L1_EM10_XS20', ['L1_EM10','','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xs15","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25',      'L1_EM10_XS20', ['L1_EM10','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi05',           'L1_EM10_XS20', ['L1_EM10','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],

        ['e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15',      'L1_EM15_XS30', ['L1_EM15','','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","xs20","j20_perf","xe20"]]],
        ['e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","xs20","j20_perf","xe20"]]],
        ['e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35',      'L1_EM15_XS30', ['L1_EM15','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","j20_perf","xe20"]]],
        ['e18_etcut_trkcut_j20_perf_xe20_6dphi15',           'L1_EM15_XS30', ['L1_EM15','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","j20_perf","xe20"]]],


        ['2e12_loose_mu10',         'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_lhloose_mu10',       'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_lhloose_nod0_mu10',  'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['2e12_medium_mu10',        'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_lhmedium_mu10',      'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_lhmedium_nod0_mu10', 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['e12_loose_2mu10',         'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhloose_2mu10',       'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhloose_nod0_2mu10',  'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['e12_medium_2mu10',        'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhmedium_2mu10',      'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhmedium_nod0_2mu10', 'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['e17_loose_mu14',          'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e17_lhloose_mu14',        'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e17_lhloose_nod0_mu14',   'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['e24_medium_L1EM20VHI_mu8noL1',        'L1_EM20VHI', ['L1_EM20VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e24_medium_L1EM20VHI','mu8noL1'] ]],
        ['e24_lhmedium_L1EM20VHI_mu8noL1',      'L1_EM20VHI', ['L1_EM20VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e24_lhmedium_L1EM20VHI','mu8noL1'] ]],
        ['e24_lhmedium_nod0_L1EM20VHI_mu8noL1', 'L1_EM20VHI', ['L1_EM20VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e24_lhmedium_nod0_L1EM20VHI','mu8noL1'] ]],

        ['e26_medium_L1EM22VHI_mu8noL1',        'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e26_medium_L1EM22VHI','mu8noL1'] ]],
        ['e26_lhmedium_L1EM22VHI_mu8noL1',      'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e26_lhmedium_L1EM22VHI','mu8noL1'] ]],
        ['e26_lhmedium_nod0_L1EM22VHI_mu8noL1', 'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e26_lhmedium_nod0_L1EM22VHI','mu8noL1'] ]],

        ['e60_loose_xe60noL1',           'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e60_loose","xe60noL1"]]],
        ['e60_lhloose_xe60noL1',         'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e60_lhloose","xe60noL1"]]],
        ['e60_lhloose_nod0_xe60noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e60_lhloose_nod0","xe60noL1"]]],

        ['e70_loose_xe70noL1',           'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_loose","xe70noL1"]]],
        ['e70_lhloose_xe70noL1',         'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose","xe70noL1"]]],
        ['e70_lhloose_nod0_xe70noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_nod0","xe70noL1"]]],

        ['e7_medium_mu24',               'L1_MU20', ['L1_EM3', 'L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e7_lhmedium_mu24',             'L1_MU20', ['L1_EM3', 'L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e7_lhmedium_nod0_mu24',        'L1_MU20', ['L1_EM3', 'L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['g40_tight_xe40noL1',           'L1_EM20VH',  ['L1_EM20VH',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g40_tight","xe40noL1"]]],
        ['g45_tight_xe45noL1',           'L1_EM20VH',  ['L1_EM20VH',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight","xe45noL1"]]],
        ['g45_tight_L1EM22VHI_xe45noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_L1EM22VHI","xe45noL1"]]],
        ['g60_loose_xe60noL1',           'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g60_loose","xe60noL1"]]],
        ['g70_loose_xe70noL1',           'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g70_loose","xe70noL1"]]],

        ['g25_medium_mu24',              'L1_MU20', ['L1_EM15VH','L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        ['g15_loose_2mu10_msonly',       'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        ['j100_xe80',                    'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets', 'BW:MET'], -1,['serial',-1,["j100","xe80"]]],


        ['mu4_j15_dr05', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j15'] ]],
        ['mu4_j25_dr05', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j25'] ]],
        ['mu4_j35_dr05', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j35'] ]],
 
        ['mu4_j55_dr05',  'L1_MU4_J12', ['L1_MU4', ''], [PhysicsStream,  'express'], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j55'] ]], 
        ['mu6_j85_dr05',  'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j85'] ]], 
        ['mu6_j110_dr05', 'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j110'] ]],
        ['mu6_j150_dr05', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j150'] ]],
        ['mu6_j175_dr05', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j175']]],
        ['mu6_j260_dr05', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j260'] ]],
        ['mu6_j320_dr05', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j320'] ]],
        ['mu6_j400_dr05', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j400'] ]],

        ['mu4_j15_bperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j15_bperf_split'] ]],
        ['mu4_j25_bperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j25_bperf_split'] ]],
        ['mu4_j35_bperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j35_bperf_split'] ]],

        ['mu4_j55_bperf_split_dr05_dz02',  'L1_MU4_J12', ['L1_MU4', ''], [PhysicsStream, 'express'], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j55_bperf_split'] ]],
        ['mu6_j85_bperf_split_dr05_dz02',  'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j85_bperf_split'] ]],
        ['mu6_j110_bperf_split_dr05_dz02', 'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j110_bperf_split'] ]],
        ['mu6_j150_bperf_split_dr05_dz02', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j150_bperf_split'] ]],
        ['mu6_j175_bperf_split_dr05_dz02', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j175_bperf_split']]],
        ['mu6_j260_bperf_split_dr05_dz02', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j260_bperf_split'] ]],
        ['mu6_j320_bperf_split_dr05_dz02', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j320_bperf_split'] ]],
        ['mu6_j400_bperf_split_dr05_dz02', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j400_bperf_split'] ]],

        # Extra calibration items seeded from L1Topo
        ['mu4_j35_bperf_split_dr05_dz02_L1BTAG-MU4J15', 'L1_BTAG-MU4J15', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j35_bperf_split'] ], True],
        ['mu4_j55_bperf_split_dr05_dz02_L1BTAG-MU4J15', 'L1_BTAG-MU4J15', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j55_bperf_split'] ], True],
        ['mu6_j85_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j85_bperf_split'] ], True],
        ['mu6_j110_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j110_bperf_split'] ], True],
        ['mu6_j150_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j150_bperf_split'] ], True],
        ['mu6_j175_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j175_bperf_split'] ], True],
        ['mu6_j260_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j260_bperf_split'] ], True],
        ['mu6_j320_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j320_bperf_split'] ], True],
        ['mu6_j400_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j400_bperf_split'] ], True],
        
        ['2mu14_2j35_bperf',              'L1_2MU10',   ['L1_2MU10', ''],   [PhysicsStream], ['RATE:MuonBjet',  'BW:Bjet'],-1,['serial',-1,['2mu14', '2j35_bperf']]],
        ['2e17_loose_2j35_bperf',         'L1_2EM15VH', ['L1_2EM15VH', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Bjet'],-1,['serial',-1,['2e17_loose', '2j35_bperf']]],
        ['2e17_lhloose_2j35_bperf',       'L1_2EM15VH', ['L1_2EM15VH', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Bjet'],-1,['serial',-1,['2e17_lhloose', '2j35_bperf']]],
        ['mu26_imedium_2j35_bperf',       'L1_MU20',    ['L1_MU20', ''],    [PhysicsStream], ['RATE:MuonBjet',  'BW:Bjet'],-1,['serial',-1,['mu26_imedium', '2j35_bperf']]],
        ['e26_tight_iloose_2j35_bperf',   'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Bjet'],-1,['serial',-1,['e26_tight_iloose', '2j35_bperf']]],
        ['e26_lhtight_iloose_2j35_bperf', 'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Bjet'],-1,['serial',-1,['e26_lhtight_iloose', '2j35_bperf']]],

        # Wgg triggers
        ['e20_medium_2g10_loose',    'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e20_medium_2g10_medium',   'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e20_lhmedium_2g10_loose',  'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e20_lhmedium_2g10_medium', 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['2g10_loose_mu20',          'L1_MU20',        ['L1_2EM7', 'L1_MU20'],  [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_loose','mu20'] ]],
        ['2g10_medium_mu20',         'L1_MU20',        ['L1_2EM7', 'L1_MU20'],  [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_medium','mu20'] ]],

        # LLP trigger seeded by MUON items
        ['j30_muvtx',                    'L1_2MU10',            [], [PhysicsStream], ['RATE:MuonJet','BW:Muon', 'BW:Jets'], -1],
        ['j30_muvtx_noiso',              'L1_2MU10',            [], [PhysicsStream], ['RATE:MuonJet','BW:Muon', 'BW:Jets'], -1],
        ['j30_muvtx_L1MU4_EMPTY',        'L1_MU4_EMPTY',        [], ["Late"], ['RATE:MuonJet','BW:Muon', 'BW:Jets'], -1],
        ['j30_muvtx_L1MU4_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_ISO', [], ["Late"], ['RATE:MuonJet','BW:Muon', 'BW:Jets'], -1],

        # LLP trigger seeded by TAU items
        ['j30_jes_PS_llp_L1TAU30',             'L1_TAU30',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j30_jes_PS_llp_L1TAU40',             'L1_TAU40',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j30_jes_PS_llp_L1TAU60',             'L1_TAU60',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j30_jes_PS_llp_L1TAU8_EMPTY',        'L1_TAU8_EMPTY',        [], ["Late"], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j30_jes_PS_llp_L1TAU8_UNPAIRED_ISO', 'L1_TAU8_UNPAIRED_ISO', [], ["Late"], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j30_jes_PS_llp_noiso_L1TAU60',       'L1_TAU60',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        # LLP triggers
        ['g15_loose_2mu10_msonly_L1MU4_EMPTY',        'L1_MU4_EMPTY',['L1_EM8VH','L1_2MU10'], ["Late"], ['RATE:EgammaMuon', 'BW:Egamma','BW:Muon'],-1,['parallel',-1,[] ]],
        ['g15_loose_2mu10_msonly_L1MU4_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_ISO',['L1_EM8VH','L1_2MU10'], ["Late"], ['RATE:EgammaMuon', 'BW:Egamma','BW:Muon'],-1,['parallel',-1,[] ]],

        ['j30_xe10_razor170', 'L1_2J15_XE55', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j30','xe10']]],

        #razor triggers
        ['j30_xe10_razor100', 'L1_2J15_XE55', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j30','xe10']]],
        ['j30_xe10_razor185', 'L1_2J15_XE55', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j30','xe10']]],
        ['j30_xe10_razor195', 'L1_2J15_XE55', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j30','xe10']]],

        # tighter razor triggers
        ['j30_xe60_razor100', 'L1_2J15_XE55', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j30','xe60']]],
        ['j30_xe60_razor170', 'L1_2J15_XE55', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j30','xe60']]],
        ['j30_xe60_razor185', 'L1_2J15_XE55', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j30','xe60']]],
        ['j30_xe60_razor195', 'L1_2J15_XE55', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j30','xe60']]],

        ['j55_0eta240_xe50_L1J30_EMPTY', 'L1_J30_EMPTY', ['',''], ['Late'], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j55_0eta240','xe50']]],

        # TAU+X
        # Ditau Items
        ['tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_perf_ptonly","tau25_perf_ptonly"]]],

        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],

        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],

        # Works now
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12',   'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12"]]],

        # High-pT DiTau seeding off of single-tau
        ['tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12', 'L1_TAU60',['L1_TAU60','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau50_medium1_tracktwo_L1TAU12"]]],
        ['tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12', 'L1_TAU60',['L1_TAU60','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau125_medium1_tracktwo","tau50_medium1_tracktwo_L1TAU12"]]],

        # Electron + Muon
        ['g10_etcut_L1EM8I_mu10_taumass',      'L1_EM8I_MU10',  ['L1_EM8I', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut_L1EM8I","mu10"]] ],
        ['g20_etcut_L1EM15I_mu4_taumass',      'L1_EM15I_MU4',  ['L1_EM15I', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut_L1EM15I","mu4"]] ],
        ['g10_loose_L1EM8I_mu10_iloose_taumass',      'L1_EM8I_MU10',  ['L1_EM8I', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_loose_L1EM8I","mu10_iloose"]] ],
        ['g20_loose_L1EM15I_mu4_iloose_taumass',      'L1_EM15I_MU4',  ['L1_EM15I', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_loose_L1EM15I","mu4_iloose"]] ],
        ['g10_etcut_L1EM8I_mu10_iloose_taumass',      'L1_EM8I_MU10',  ['L1_EM8I', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut_L1EM8I","mu10_iloose"]] ],
        ['g20_etcut_L1EM15I_mu4_iloose_taumass',      'L1_EM15I_MU4',  ['L1_EM15I', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut_L1EM15I","mu4_iloose"]] ],

        # HLT photon and di-muon chains
        ['g35_loose_L1EM22VHI_mu18noL1',        'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM22VHI','mu18noL1'] ]],
        ['g35_loose_L1EM22VHI_mu15noL1_mu2noL1', 'L1_EM22VHI', ['L1_EM22VHI', '',''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM22VHI','mu15noL1','mu2noL1']]],

        # Low-threshold with jets
        ['e17_medium_tau25_medium1_tracktwo',   'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo',  'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],

        # Isolated Variant
        ['e17_medium_iloose_tau25_medium1_tracktwo',   'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_medium_iloose","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_iloose_tau25_medium1_tracktwo',  'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo"]]],

        ['e17_lhmedium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25',  'L1_EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo"]]],
        ['e17_medium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25',  'L1_EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium_iloose","tau25_medium1_tracktwo"]]],


        # High-threshold
        ['e17_medium_tau80_medium1_tracktwo',   'L1_EM15HI_TAU40_2TAU15',['L1_EM15HI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_medium","tau80_medium1_tracktwo"]]],
        ['e17_lhmedium_tau80_medium1_tracktwo',   'L1_EM15HI_TAU40_2TAU15',['L1_EM15HI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium","tau80_medium1_tracktwo"]]],
        #new ones                                                                                                                                                                                    
        ['e17_lhmedium_nod0_tau80_medium1_tracktwo',   'L1_EM15HI_TAU40_2TAU15',['L1_EM15HI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau80_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo',  'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_iloose","tau25_medium1_tracktwo"]]],

        ['e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_iloose","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo',  'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo", "xe50"]]],
        #Performance:
        ['e26_lhtight_nod0_iloose_2j35_bperf',   'L1_EM22VHI',      ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Bjet'],-1,['serial',-1,['e26_lhtight_nod0_iloose', '2j35_bperf'] ] ], 
		
        # Tau + Muon
        ['mu14_tau25_medium1_tracktwo',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],     
        ['mu14_tau35_medium1_tracktwo_L1TAU20',  'L1_MU10_TAU20',['L1_MU10','L1_TAU20'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14","tau35_medium1_tracktwo_L1TAU20"]]],
        ['mu14_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14","tau35_medium1_tracktwo"]]],

        ['mu14_iloose_tau25_perf_tracktwo', 'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream, 'express'], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_iloose","tau25_perf_tracktwo"]]],
        ['mu14_iloose_tau25_medium1_tracktwo',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_iloose","tau25_medium1_tracktwo"]]],        
        ['mu14_iloose_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_iloose","tau35_medium1_tracktwo"]]],

        ['mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_iloose","tau25_medium1_tracktwo"]]],        
        ['mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I', 'L1_DR-MU10TAU12I',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_iloose","tau25_medium1_tracktwo"]]],        
        ['mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12I-J25', 'L1_MU10_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_iloose","tau25_medium1_tracktwo"]]],        
        
        
        # Tau + MET
        ['tau35_medium1_tracktwo_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","xe70_L1XE45"]]],  
        ['tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45',  'L1_TAU20_2J20_XE45',['L1_TAU20','L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","xe70_L1XE45"]]],
        # Problematic
        # Cannot handle our naming scheme :D
        #['tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45_TAU20-J20',  'L1_XE45_TAU20-J20',['L1_TAU20', 'L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","xe70_L1XE45"]]],  
        
        # Ditau + MET
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_xe50', 'L1_TAU20_2TAU12_XE35',['L1_TAU20','L1_TAU12','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12","xe50"]]],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50', 'L1_TAU20IM_2TAU12IM_XE35',['L1_TAU20IM','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo","xe50"]]],

        
        # Tau + e + MET

        ['e17_medium_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo", "xe50"]]],

        ['e17_medium_iloose_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_medium_iloose","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_iloose_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo", "xe50"]]],

        # With L1Topo
        

        # Tau + mu + MET

        ['mu14_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_medium1_tracktwo","xe50"]]],
        ['mu14_iloose_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_iloose", "tau25_medium1_tracktwo","xe50"]]],
        
        #['e18_lhloose_tau80_medium1_calo',                 'L1_EM15-TAU40',[], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_lhloose","tau80_medium1_calo"]]],

        ['j80_xe80',               'L1_J40_XE50',['',''], [PhysicsStream, 'express'], ['RATE:JetMET', 'BW:Jets', 'BW:MET'], -1,['serial',-1,["j80","xe80"]]], 

        # lepton + jets
        ['e24_vloose_L1EM18VH_3j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM18VH","3j20noL1"] ]],
        ['e24_vloose_L1EM18VH_4j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM18VH","4j20noL1"] ]],
        ['e24_vloose_L1EM18VH_5j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM18VH","5j15noL1"] ]],
        ['e24_vloose_L1EM18VH_6j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM18VH","6j15noL1"] ]],
         
        ['e24_lhvloose_L1EM18VH_3j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","3j20noL1"] ]],
        ['e24_lhvloose_L1EM18VH_4j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","4j20noL1"] ]],
        ['e24_lhvloose_L1EM18VH_5j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","5j15noL1"] ]],
        ['e24_lhvloose_L1EM18VH_6j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","6j15noL1"] ]],
         
         
        ['e26_vloose_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_vloose_L1EM20VH","3j20noL1"]]],
        ['e26_vloose_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_vloose_L1EM20VH","4j20noL1"] ]],
        ['e26_vloose_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_vloose_L1EM20VH","5j15noL1"] ]],
        ['e26_vloose_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_vloose_L1EM20VH","6j15noL1"] ]],
         
        ['e26_lhvloose_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","3j20noL1"] ]],
        ['e26_lhvloose_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","4j20noL1"] ]],
        ['e26_lhvloose_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","5j15noL1"] ]],
        ['e26_lhvloose_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","6j15noL1"] ]],
         
        ['e24_vloose_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM20VH","3j20noL1"] ]],
        ['e24_vloose_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM20VH","4j20noL1"] ]],
        ['e24_vloose_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM20VH","5j15noL1"] ]],
        ['e24_vloose_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM20VH","6j15noL1"] ]],
         
        ['e24_lhvloose_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","3j20noL1"] ]],
        ['e24_lhvloose_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","4j20noL1"] ]],
        ['e24_lhvloose_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","5j15noL1"] ]],
        ['e24_lhvloose_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","6j15noL1"] ]],
        #nod0 chains:
        ['e26_lhvloose_nod0_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM20VH","3j20noL1"] ]],
        ['e26_lhvloose_nod0_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM20VH","4j20noL1"] ]],
        ['e26_lhvloose_nod0_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM20VH","5j15noL1"] ]],
        ['e26_lhvloose_nod0_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM20VH","6j15noL1"] ]],
        ['e24_lhvloose_nod0_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM20VH","3j20noL1"] ]],
        ['e24_lhvloose_nod0_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM20VH","4j20noL1"] ]],
        ['e24_lhvloose_nod0_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM20VH","5j15noL1"] ]],
        ['e24_lhvloose_nod0_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM20VH","6j15noL1"] ]],
        ['e24_lhvloose_nod0_L1EM18VH_3j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM18VH","3j20noL1"] ]],
        ['e24_lhvloose_nod0_L1EM18VH_4j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM18VH","4j20noL1"] ]],
        ['e24_lhvloose_nod0_L1EM18VH_5j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM18VH","5j15noL1"] ]],
        ['e24_lhvloose_nod0_L1EM18VH_6j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM18VH","6j15noL1"] ]],
	
        ['mu26_2j20noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu26","2j20noL1"] ]],
        ['mu26_3j20noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu26","3j20noL1"] ]],
        ['mu26_4j15noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu26","4j15noL1"] ]],
        ['mu26_5j15noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu26","5j15noL1"] ]],
         
        ['mu24_2j20noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu24","2j20noL1"] ]],
        ['mu24_3j20noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu24","3j20noL1"] ]],
        ['mu24_4j15noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu24","4j15noL1"] ]],
        ['mu24_5j15noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu24","5j15noL1"] ]],
        
        ['mu20_L1MU15_2j20noL1',  'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu20_L1MU15","2j20noL1"] ]],
        ['mu20_L1MU15_3j20noL1',  'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu20_L1MU15","3j20noL1"] ]],
        ['mu20_L1MU15_4j15noL1',  'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu20_L1MU15","4j15noL1"] ]],
        ['mu20_L1MU15_5j15noL1',  'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu20_L1MU15","5j15noL1"] ]],

        
        # VBF triggers
        ['g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM18VH', ['L1_EM18VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1,['serial',-1,["g20_loose_L1EM18VH","2j40_0eta490","3j25_0eta490_invm700"]]],
        ['g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM20VH', ['L1_EM20VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1,['serial',-1,["g25_loose_L1EM20VH","2j40_0eta490","3j25_0eta490_invm700"]]],
        ['g25_loose_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1,['serial',-1,["g25_loose","2j40_0eta490","3j25_0eta490_invm700"]]],
        ['g25_medium_L1EM22VHI_4j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_medium_L1EM22VHI","4j35_0eta490_invm700"]]],
        ['g25_loose_L1EM20VH_4j35_0eta490', 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_loose_L1EM20VH","4j35_0eta490"]]],
        ['g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Bjet','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI', 'j35_0eta490_bmv2c2077','3j35_0eta490_invm700']]],
        ['g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Bjet','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI', '2j35_0eta490_bmv2c2077','2j35_0eta490']]],

        #VBF +photon
        ['g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490', 'L1_EM18VH', ['L1_EM18VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1,['serial',-1,["g20_loose_L1EM18VH","2j40_0eta490","3j25_0eta490"]]],

        # backups for L1Topo VBF items
        ['mu6_2j40_0eta490_invm400_L1MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J30.0ETA49_2J20.0ETA49', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu6","2j40_0eta490_invm400"]]],
        ['mu6_2j40_0eta490_invm600_L1MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J30.0ETA49_2J20.0ETA49', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu6","2j40_0eta490_invm600"]]],
        ['mu6_2j40_0eta490_invm800_L1MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J30.0ETA49_2J20.0ETA49', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu6","2j40_0eta490_invm800"]]],
        ['mu6_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J30.0ETA49_2J20.0ETA49', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu6","2j40_0eta490_invm1000"]]],

        #mujet matching chain 

        # 4-jet items (VBF H->bb, hh->bbbb)
        ['mu4_j40_dr05_3j40_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j40', '3j40'] ]],

        #Backup
        ['mu4_4j40_dr05_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '4j40'] ]],
        ['mu4_4j40_dr05_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '4j40'] ]],

        # 3-jet items (bA->bbb) backup
        ['mu4_3j45_dr05_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j45'] ]],
        ['mu4_3j45_dr05_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j45'] ]],

        # Single mu-jet items backup
        ['mu6_j60_dr05_L1MU6_J20', 'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j60'] ]],
        ['mu4_j70_dr05_L1MU4_J30', 'L1_MU4_J30', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j70'] ]],
        ['mu6_j60_dr05_L1MU6_J40', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j60'] ]],
        ['mu4_j70_dr05_L1MU4_J50', 'L1_MU4_J50', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j70'] ]],

        # Asymmetric items backup
        ['mu6_j50_dr05_2j35_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j50', '2j35'] ]],
        ['mu4_j60_dr05_2j35_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j60', '2j35'] ]],
        ['mu6_j50_dr05_2j35_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j50', '2j35'] ]],
        ['mu4_j60_dr05_2j35_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j60', '2j35'] ]],

        # L1Topo LFV
        ['g10_etcut_mu10_L1LFV-EM8I',         'L1_LFV-EM8I',  ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut","mu10"]] ],
        ['g20_etcut_mu4_L1LFV-EM15I',         'L1_LFV-EM15I', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut","mu4"]] ],
        ['g10_etcut_mu10_taumass',            'L1_LFV-EM8I',  ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut","mu10"]] ],
        ['g20_etcut_mu4_taumass',             'L1_LFV-EM15I', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut","mu4"]] ],
# taumass chains
        ['g10_loose_mu10_iloose_taumass_L1LFV-EM8I',         'L1_LFV-EM8I',  ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_loose","mu10_iloose"]] ],
        ['g20_loose_mu4_iloose_taumass_L1LFV-EM15I',         'L1_LFV-EM15I',  ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_loose","mu4_iloose"]] ],
        ['g10_etcut_mu10_iloose_taumass_L1LFV-EM8I',         'L1_LFV-EM8I',  ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut","mu10_iloose"]] ],
        ['g20_etcut_mu4_iloose_taumass_L1LFV-EM15I',         'L1_LFV-EM15I',  ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut","mu4_iloose"]] ],

        # L1Topo W T&P 
        ['e13_etcut_trkcut_xe20', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM10',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e13_etcut_trkcut_xe20_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM10',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e18_etcut_trkcut_xe35', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0',['L1_EM15',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","xe35"]]],
        ['e18_etcut_trkcut_xe35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30',['L1_EM15',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","xe35"]]],
        ['e13_etcut_trkcut_xs15_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM10',''], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1,['serial',-1,["e13_etcut_trkcut","xs15"]]], # commenting out until megrging is fixed centrally.
        ['e13_etcut_trkcut_xe20_mt25', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM10',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20_xe20_mt25', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM10',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e18_etcut_trkcut_xe35_mt35', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0', ['L1_EM15',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xe35"]]],
        ['e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30_xe35_mt35', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30', ['L1_EM15',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xe35"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM10','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM10','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
        ['e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0', ['L1_EM15','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","j20_perf","xe20"]]],
        ['e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30', ['L1_EM15','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","j20_perf","xe20"]]],

        # L1Topo J+XE+DPHI
        ['j80_xe80_1dphi10_L1J40_DPHI-Js2XE50',   'L1_J40_DPHI-Js2XE50',['',''], [PhysicsStream],    ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        ['j80_xe80_1dphi10_L1J40_DPHI-J20s2XE50', 'L1_J40_DPHI-J20s2XE50',['',''], [PhysicsStream],  ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        ['j80_xe80_1dphi10_L1J40_DPHI-J20XE50', 'L1_J40_DPHI-J20XE50',['',''], [PhysicsStream],  ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        ['j80_xe80_1dphi10_L1J40_DPHI-CJ20XE50','L1_J40_DPHI-CJ20XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        ['j100_xe80_L1J40_DPHI-Js2XE50',    'L1_J40_DPHI-Js2XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80"]]],
        ['j100_xe80_L1J40_DPHI-J20s2XE50',  'L1_J40_DPHI-J20s2XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80"]]],
        ['j100_xe80_L1J40_DPHI-J20XE50',  'L1_J40_DPHI-J20XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80"]]],
        ['j100_xe80_L1J40_DPHI-CJ20XE50', 'L1_J40_DPHI-CJ20XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80"]]],


        ['2j40_0eta490_invm250_xe80_L1XE70',    'L1_XE70', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["2j40_0eta490_invm250","xe80_L1XE70"]]],
        ['j40_0eta490_j30_0eta490_deta25_xe80_L1XE70',    'L1_XE70', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j40_0eta490","j30_0eta490_deta25","xe80_L1XE70"]]],
		
        # L1Topo EM+TAU
        ['e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'L1_XE35_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'L1_XE35_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_medium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'L1_XE40_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'L1_XE40_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo", "xe50"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I',   'L1_DR-TAU20ITAU12I',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20ITAU12I-J25',   'L1_TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1BOX-TAU20ITAU12I',   'L1_BOX-TAU20ITAU12I',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],

        # L1Topo EM+TAU+J
        ['e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],
        # Isolated variant
        ['e17_medium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium_iloose","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo"]]],

        # L1Topo EM+TAU+J backup
        ['e17_medium_tau25_medium1_tracktwo_L1EM15TAU12I-J25',  'L1_EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_L1EM15TAU12I-J25',  'L1_EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],
        ['e17_medium_tau25_medium1_tracktwo_L1EM15-TAU12I',  'L1_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_L1EM15-TAU12I',  'L1_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],

        # High-threshold
        ['e17_medium_tau80_medium1_tracktwo_L1EM15-TAU40',   'L1_EM15-TAU40',['L1_EM15HI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau80_medium1_tracktwo"]]],
        ['e17_lhmedium_tau80_medium1_tracktwo_L1EM15-TAU40',   'L1_EM15-TAU40',['L1_EM15HI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau80_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1 ,["e17_lhmedium_nod0_iloose","tau25_medium1_tracktwo"]]],

        # L1Topo MU+TAU+J
        ['mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25',  'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],
        ['mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I',  'L1_DR-MU10TAU12I',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],
        ['mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25',  'L1_MU10_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],

        # L1Topo LLP with TAU
        ['j30_jes_PS_llp_L1LLP-NOMATCH',       'L1_LLP-NOMATCH',       [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1, False],
        ['j30_jes_PS_llp_noiso_L1LLP-NOMATCH', 'L1_LLP-NOMATCH',       [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1, False],

        # L1Topo VBF 
        ['g15_loose_2j40_0eta490_3j25_0eta490', 'L1_MJJ-400', ['L1_EM13VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g15_loose","2j40_0eta490","3j25_0eta490"]]],
        ['g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700', 'L1_MJJ-700', ['L1_EM15VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g20_loose","2j40_0eta490","3j25_0eta490"]]],
        ['g20_loose_2j40_0eta490_3j25_0eta490', 'L1_MJJ-800', ['L1_EM15VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g20_loose","2j40_0eta490","3j25_0eta490"]]],
        ['g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-900', 'L1_MJJ-900', ['L1_EM15VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g20_loose","2j40_0eta490","3j25_0eta490"]]],
        ['mu6_2j40_0eta490_invm400', 'L1_MU6_MJJ-200', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu6","2j40_0eta490_invm400"]]],
        ['mu6_2j40_0eta490_invm600', 'L1_MU6_MJJ-300', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu6","2j40_0eta490_invm600"]]],
        ['mu6_2j40_0eta490_invm800', 'L1_MU6_MJJ-400', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu6","2j40_0eta490_invm800"]]],
        ['mu6_2j40_0eta490_invm1000', 'L1_MU6_MJJ-500', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu6","2j40_0eta490_invm1000"]]],

        # L1Topo EM+TAU Physics
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15TAU12I-J25',  'L1_EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15-TAU12I',  'L1_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_tau80_medium1_tracktwo_L1EM15-TAU40',   'L1_EM15-TAU40',['L1_EM15HI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau80_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'L1_XE35_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'L1_XE40_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo", "xe50"]]],

        # L1Topo BTAG Single mu-jet items
        ['mu6_j60_dr05_L1BTAG-MU6J25', 'L1_BTAG-MU6J25', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu6', 'j60'] ], True],
        ['mu4_j70_dr05_L1BTAG-MU4J30', 'L1_BTAG-MU4J30', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', 'j70'] ], True],
        
        # Asymmetric items
        ['mu6_j50_dr05_2j35_L13J15_BTAG-MU6J25', 'L1_3J15_BTAG-MU6J25', ['L1_MU6', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu6', 'j50', '2j35'] ], True],
        ['mu4_j60_dr05_2j35_L13J15_BTAG-MU4J30', 'L1_3J15_BTAG-MU4J30', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', 'j60', '2j35'] ], True],
        
   #tau+photon                                                                                                                                                                                                       
        ['g35_medium_tau25_dikaon_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_dikaon_tracktwo_L1TAU12"]]],
        ['g35_medium_tau35_dikaon_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau35_dikaon_tracktwo_L1TAU12"]]],
        ['g35_medium_tau25_dikaontight_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_dikaontight_tracktwo_L1TAU12"]]],
        ['g35_medium_tau35_dikaontight_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["g35_medium","tau35_dikaontight_tracktwo_L1TAU12"]]],

        #photon+tau chain (ATR-11531)                                                                                                                                                                                     
        ['tau25_dikaon_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_dikaon_tracktwo_L1TAU12',            'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_dikaontight_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_dikaontight_tracktwo_L1TAU12',            'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],



        ]


    TriggerFlags.HeavyIonSlice.signatures = [
        ]


 #Beamspot chanis first try ATR-9847                                                                                                               
    TriggerFlags.BeamspotSlice.signatures = [
        #['beamspot_allTE_L2StarB_L1TRT',           'L1_TRT', [], [PhysicsStream], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        #['beamspot_allTE_trkfast_L1TRT',           'L1_TRT', [], [PhysicsStream], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ##trkFS --> no selection, write out PEB events
        ['beamspot_allTE_L2StarB_peb_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_peb_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ['beamspot_allTE_L2StarB_peb_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_peb_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ['beamspot_allTE_L2StarB_peb',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_peb',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ##trkFS --> no selection, add TRT to PEB events        
        ['beamspot_allTE_L2StarB_pebTRT_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ['beamspot_allTE_L2StarB_pebTRT_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

# taking this out all together, not needed and even with PS=-1 makes histograms
#        ['beamspot_allTE_L2StarB_pebTRT',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],


        ##trkFS --> select good vertices, and write out PEB events
        ['beamspot_activeTE_L2StarB_peb_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_activeTE_trkfast_peb_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ['beamspot_activeTE_L2StarB_peb_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_activeTE_trkfast_peb_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],


        ['beamspot_activeTE_L2StarB_peb',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_activeTE_trkfast_peb',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ##trkFS --> select good vertices, add TRT data to PEB events
        ['beamspot_activeTE_L2StarB_pebTRT_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_activeTE_trkfast_pebTRT_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ['beamspot_activeTE_L2StarB_pebTRT_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_activeTE_trkfast_pebTRT_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

# taking this out all together, not needed and even with PS=-1 makes histograms
#        ['beamspot_activeTE_L2StarB_pebTRT',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_activeTE_trkfast_pebTRT',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ##trkFS --> online beam-spot determination
        ['beamspot_trkFS_L2StarB_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ['beamspot_trkFS_L2StarB_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        
# taking this out all together, not needed and even with PS=-1 makes histograms
#        ['beamspot_trkFS_L2StarB',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        
        ]   

    TriggerFlags.MinBiasSlice.signatures = [
        # LS1 chains
        ['mb_sptrk',                     'L1_RD0_FILLED', [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_L1RD0_UNPAIRED_ISO',  'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_L1RD0_EMPTY',         'L1_RD0_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_noisesup',            'L1_RD0_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_noisesup_L1RD0_EMPTY',   'L1_RD0_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ##['mb_sp2000_trk70_hmt',        'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:HMT"], -1], #disabled for M4 see https://savannah.cern.ch/bugs/?104744

	#leading track for low-mu run
	['mb_sptrk_pt4_L1MBTS_1',                     'L1_MBTS_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_pt6_L1MBTS_1',                     'L1_MBTS_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_pt8_L1MBTS_1',                     'L1_MBTS_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
	['mb_sptrk_pt4_L1MBTS_2',                     'L1_MBTS_2', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_pt6_L1MBTS_2',                     'L1_MBTS_2', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
	['mb_sptrk_pt8_L1MBTS_2',                     'L1_MBTS_2', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
	['mb_sptrk_pt4_L1MBTS_1_1',                   'L1_MBTS_1_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_pt6_L1MBTS_1_1',                   'L1_MBTS_1_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_pt8_L1MBTS_1_1',                   'L1_MBTS_1_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],

        # Week 1
        ['mb_sptrk_L1RD3_FILLED',                     'L1_RD3_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_noisesup_L1RD3_FILLED',            'L1_RD3_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
	['mb_sptrk_pt4_L1RD3_FILLED',                 'L1_RD3_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_pt6_L1RD3_FILLED',                 'L1_RD3_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_pt8_L1RD3_FILLED',                 'L1_RD3_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],

        ['mb_perf_L1LUCID_UNPAIRED_ISO', 'L1_LUCID_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_perf_L1LUCID_EMPTY',        'L1_LUCID_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_perf_L1LUCID',              'L1_LUCID',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],

        ['mb_sptrk_costr',               'L1_RD0_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_costr_L1RD0_EMPTY',   'L1_RD0_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],

        ['mb_perf_L1MBTS_2',             'L1_MBTS_2', [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_perf_L1RD1_FILLED',         'L1_RD1_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
 
        ['mb_mbts_L1MBTS_1',             'L1_MBTS_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_2',             'L1_MBTS_2', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1], 
        ['mb_mbts_L1MBTS_1_1',           'L1_MBTS_1_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_1_EMPTY',       'L1_MBTS_1_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_2_EMPTY',       'L1_MBTS_2_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_1_1_EMPTY',     'L1_MBTS_1_1_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_1_UNPAIRED_ISO',             'L1_MBTS_1_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_2_UNPAIRED_ISO',             'L1_MBTS_2_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_1_1_UNPAIRED_ISO',           'L1_MBTS_1_1_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],

        #physics                                                                                                                             
        ['mb_sp400_trk40_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp600_trk45_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp700_trk55_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp900_trk60_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp900_trk65_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1000_trk70_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1200_trk75_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_trk80_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        #['mb_sp1400_trk90_hmt_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        #['mb_sp1400_trk90_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_trk100_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_trk100_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],


        #physics                                                                                                                             
        ['mb_sp700_trk50_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp700_trk50_hmt_L1RD3_FILLED', 'L1_RD3_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ['mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp300_trk10_sumet50_hmt_L1RD3_FILLED', 'L1_RD3_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp600_trk10_sumet50_hmt_L1RD3_FILLED', 'L1_RD3_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ['mb_sp300_trk10_sumet40_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp600_trk10_sumet40_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        #['mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],                     
        #['mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],                      
        ['mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp300_trk10_sumet50_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp600_trk10_sumet50_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp300_trk10_sumet60_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp600_trk10_sumet60_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ['mb_sp2_hmtperf_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2_hmtperf', 'L1_RD3_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp500_hmtperf', 'L1_RD3_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_hmtperf', 'L1_RD3_FILLED', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2500_hmtperf_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	#ATR-9999
	['mb_sp2_hmtperf_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp2_hmtperf_L1TE5.0ETA24', 'L1_TE5.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1300_hmtperf_L1TE5', 'L1_TE5', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1300_hmtperf_L1TE5.0ETA24', 'L1_TE5.0ETA24', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1300_hmtperf_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1300_hmtperf_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1300_hmtperf_L1TE20', 'L1_TE20', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1300_hmtperf_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1300_hmtperf_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1300_hmtperf_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1300_hmtperf_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	['mb_sp1500_hmtperf_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_hmtperf_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_hmtperf_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_hmtperf_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_hmtperf_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_hmtperf_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_hmtperf_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	['mb_sp1700_hmtperf_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1700_hmtperf_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1700_hmtperf_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1700_hmtperf_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1700_hmtperf_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1700_hmtperf_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1700_hmtperf_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	['mb_sp900_trk60_hmt_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp900_trk60_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp900_trk60_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp900_trk60_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp900_pusup350_trk60_hmt_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp900_pusup350_trk60_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp900_pusup350_trk60_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp900_pusup350_trk60_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	['mb_sp1400_trk90_hmt_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1400_trk90_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1400_trk90_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_trk90_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1400_trk90_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1400_trk90_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_trk90_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	['mb_sp1400_pusup550_trk90_hmt_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_pusup550_trk90_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_pusup550_trk90_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_pusup550_trk90_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_pusup550_trk90_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_pusup550_trk90_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1400_pusup550_trk90_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1400_pusup550_trk90_hmt_L1TE50', 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ['mb_sp1500_trk100_hmt_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_trk100_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_trk100_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_trk100_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_trk100_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_trk100_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_trk100_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        
        ['mb_sp1500_pusup700_trk100_hmt_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_pusup700_trk100_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_pusup700_trk100_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_pusup700_trk100_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_pusup700_trk100_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_pusup700_trk100_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1500_pusup700_trk100_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1500_pusup700_trk100_hmt_L1TE50', 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ['mb_sp1800_trk110_hmt_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_trk110_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_trk110_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_trk110_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_trk110_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_trk110_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_trk110_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        
        ['mb_sp1800_pusup800_trk110_hmt_L1TE10', 'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_pusup800_trk110_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_pusup800_trk110_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_pusup800_trk110_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_pusup800_trk110_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_pusup800_trk110_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_pusup800_trk110_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp1800_pusup800_trk110_hmt_L1TE50', 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	['mb_sp2100_pusup1000_trk120_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp2100_pusup1000_trk120_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp2100_pusup1000_trk120_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp2100_pusup1000_trk120_hmt_L1TE50', 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	['mb_sp2400_pusup1100_trk130_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2400_pusup1100_trk130_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2400_pusup1100_trk130_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2400_pusup1100_trk130_hmt_L1TE50', 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	['mb_sp2700_pusup1300_trk140_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2700_pusup1300_trk140_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2700_pusup1300_trk140_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2700_pusup1300_trk140_hmt_L1TE50', 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	['mb_sp3000_pusup1400_trk150_hmt_L1TE20', 'L1_TE20', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp3000_pusup1400_trk150_hmt_L1TE30', 'L1_TE30', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp3000_pusup1400_trk150_hmt_L1TE40', 'L1_TE40', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp3000_pusup1400_trk150_hmt_L1TE50', 'L1_TE50', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp3000_pusup1400_trk150_hmt_L1TE60', 'L1_TE60', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],

	['mb_sp3000_pusup1500_trk160_hmt_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp3000_pusup1500_trk160_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp3000_pusup1500_trk160_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp3000_pusup1500_trk160_hmt_L1TE50', 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp3000_pusup1500_trk160_hmt_L1TE60', 'L1_TE60', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	
        # ALFA
	['mb_sptrk_vetombts2in_L1ALFA_CEP',                  'L1_ALFA_CEP',                  [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sptrk_vetombts2in_L1TRT_ALFA_EINE',             'L1_TRT_ALFA_EINE',             [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

      	['mb_sptrk_vetombts2in_L1ALFA_ANY',                  'L1_ALFA_ANY',                  [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
      	['mb_sptrk_vetombts2in_L1ALFA_ANY_UNPAIRED_ISO',     'L1_ALFA_ANY_UNPAIRED_ISO',     [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sptrk_vetombts2in_L1TRT_ALFA_ANY',              'L1_TRT_ALFA_ANY',              [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sptrk_vetombts2in_L1TRT_ALFA_ANY_UNPAIRED_ISO', 'L1_TRT_ALFA_ANY_UNPAIRED_ISO', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
      	['mb_sptrk_vetombts2in_peb_L1ALFA_ANY',                  'L1_ALFA_ANY',                  [], ['IDFwd'], [ 'RATE:ALFACalibration','BW:Detector'], -1],
      	['mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO',     'L1_ALFA_ANY_UNPAIRED_ISO',     [], ['IDFwd'], [ 'RATE:ALFACalibration','BW:Detector'], -1],
        ['mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY',              'L1_TRT_ALFA_ANY',              [], ['IDFwd'], [ 'RATE:ALFACalibration','BW:Detector'], -1],
        ['mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY_UNPAIRED_ISO', 'L1_TRT_ALFA_ANY_UNPAIRED_ISO', [], ['IDFwd'], [ 'RATE:ALFACalibration','BW:Detector'], -1],

        ]

    TriggerFlags.CalibSlice.signatures   = [
        
        ['lhcfpeb',       'L1_LHCF',       [], ['IDFwd'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lhcfpeb_L1LHCF_UNPAIRED_ISO',       'L1_LHCF_UNPAIRED_ISO',       [], ['IDFwd'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lhcfpeb_L1LHCF_EMPTY',       'L1_LHCF_EMPTY',       [], ['IDFwd'], ["RATE:Calibration", "BW:Detector"], -1],

        ['lumipeb_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], -1],

        ['lumipeb_L1RD0_FILLED',            'L1_RD0_FILLED',            [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_UNPAIRED_ISO',      'L1_RD0_UNPAIRED_ISO',      [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1MBTS_1',                'L1_MBTS_1',                [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1], 
        ['lumipeb_L1MBTS_1_UNPAIRED_ISO',   'L1_MBTS_1_UNPAIRED_ISO',   [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1MBTS_2',                'L1_MBTS_2',                [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1], 
        ['lumipeb_L1MBTS_2_UNPAIRED_ISO',   'L1_MBTS_2_UNPAIRED_ISO',   [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1ALFA_BGT',              'L1_ALFA_BGT',              [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1ALFA_BGT_UNPAIRED_ISO', 'L1_ALFA_BGT_UNPAIRED_ISO', [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1ALFA_BGT_BGRP10',       'L1_ALFA_BGT_BGRP10',       [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],

        # triggers for VdM scan
        ['lumipeb_vdm_L1MBTS_1_BGRP11',       'L1_MBTS_1_BGRP11',       [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_BGRP11',       'L1_MBTS_2_BGRP11',       [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_BGRP11',          'L1_RD0_BGRP11',          [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_BGRP9',        'L1_MBTS_1_BGRP9',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_BGRP9',        'L1_MBTS_2_BGRP9',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_BGRP9',           'L1_RD0_BGRP9',           [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_UNPAIRED_ISO', 'L1_MBTS_1_UNPAIRED_ISO', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO', 'L1_MBTS_2_UNPAIRED_ISO', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO',    [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        
        ['lumipeb_vdm_L1MBTS_1', 'L1_MBTS_1', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2', 'L1_MBTS_2', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_FILLED',         'L1_RD0_FILLED', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        

        ['lumipeb_L1RD0_ABORTGAPNOTCALIB',    'L1_RD0_ABORTGAPNOTCALIB',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],

        ['larnoiseburst_loose_L1All',             'L1_All',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1All',             'L1_All',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE35',            'L1_XE35',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE50',            'L1_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J50',             'L1_J50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J75',             'L1_J75',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J40_XE50',            'L1_J40_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J75_XE40',            'L1_J75_XE40',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],


        ['larnoiseburst_L1XE80', 'L1_XE80', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J100', 'L1_J100', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],

        #Special re-run chains
        ['larnoiseburst_rerun',                  '',             [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_loose_rerun',            '',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        

        ['l1calocalib',             'L1_RD0_FILLED',     [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], -1],
        ['l1calocalib_L1BGRP9',     'L1_BGRP9',          [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], -1],
        
        ['larcalib_L1EM3_EMPTY',    'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1EM7_EMPTY',    'L1_EM7_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1TAU8_EMPTY',   'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J12_EMPTY',    'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],

        #['tilelarcalib_L1EM3_EMPTY',     'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        #['tilelarcalib_L1TAU8_EMPTY',    'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        #['tilelarcalib_L1J12_EMPTY',     'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        #['tilelarcalib_L1J3031ETA49_EMPTY',   'L1_J30.31ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],

        #ALFA Calib
        ['alfacalib',      'L1_ALFA_Calib'    , [],  ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],        
        ['alfacalib_L1ALFA_ANY',   'L1_ALFA_ANY'      , [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],        
        ['alfacalib_L1ALFA_ELAST15', 'L1_ALFA_ELAST15', [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],
        ['alfacalib_L1ALFA_ELAST18', 'L1_ALFA_ELAST18', [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],
        ['alfacalib_L1ALFA_A7L1_OD',                       'L1_ALFA_A7L1_OD',                    [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],
        ['alfacalib_L1ALFA_B7L1_OD',                       'L1_ALFA_B7L1_OD',                    [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],
        ['alfacalib_L1ALFA_A7R1_OD',                       'L1_ALFA_A7R1_OD',                    [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],
        ['alfacalib_L1ALFA_B7R1_OD',                       'L1_ALFA_B7R1_OD',                    [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],
        ['alfacalib_L1ALFA_SYS',   'L1_ALFA_SYS'      , [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],        
        ['alfacalib_L1ALFA_ELAS',   'L1_ALFA_ELAS'      , [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],        

        # ALFA+LHCf+L1 triggers bits only readout
        ['alfacalib_L1LHCF_ALFA_ANY_A',                    'L1_LHCF_ALFA_ANY_A',                 [], ['ALFACalib'], [ 'RATE:ALFA','BW:Detector'], -1],
        ['alfacalib_L1LHCF_ALFA_ANY_C',                    'L1_LHCF_ALFA_ANY_C',                 [], ['ALFACalib'], [ 'RATE:ALFA','BW:Detector'], -1],
        ['alfacalib_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO',       'L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO',    [], ['ALFACalib'], [ 'RATE:ALFA','BW:Detector'], -1],
        ['alfacalib_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO',       'L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO',    [], ['ALFACalib'], [ 'RATE:ALFA','BW:Detector'], -1],

        # ALFA+ID+LHCf+L1 triggers bits only readout
        ['lhcfpeb_L1LHCF_ALFA_ANY_A',                    'L1_LHCF_ALFA_ANY_A',                 [], ['IDFwd'], [ 'RATE:ALFA','BW:Detector'], -1],
        ['lhcfpeb_L1LHCF_ALFA_ANY_C',                    'L1_LHCF_ALFA_ANY_C',                 [], ['IDFwd'], [ 'RATE:ALFA','BW:Detector'], -1],
        ['lhcfpeb_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO',       'L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO',    [], ['IDFwd'], [ 'RATE:ALFA','BW:Detector'], -1],
        ['lhcfpeb_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO',       'L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO',    [], ['IDFwd'], [ 'RATE:ALFA','BW:Detector'], -1],

        ['ibllumi_L1RD0_ABORTGAPNOTCALIB',    'L1_RD0_ABORTGAPNOTCALIB', [], ['IBLLumi'], [ 'RATE:IBLLumi','BW:Detector'], -1],
        ['ibllumi_L1RD0_FILLED',    'L1_RD0_FILLED', [], ['IBLLumi'], [ 'RATE:IBLLumi','BW:Detector'], -1],
        ['ibllumi_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO', [], ['IBLLumi'], [ 'RATE:IBLLumi','BW:Detector'], -1],

        #
        ['idcalib_trk9_central',    'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk9_fwd',        'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk16_central',   'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk16_fwd',       'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk29_central',   'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk29_fwd',       'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],

        ['conej40_larpebj',  'L1_J12',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej165_larpebj', 'L1_J75',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1], 
        ['conej75_320eta490_larpebj',  'L1_J30.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej140_320eta490_larpebj', 'L1_J75.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],

        # Monitoritems for saturated towers
        ['satu20em_l1satmon_L1J100_FIRSTEMPTY',  'L1_J100_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY',  'L1_J100.31ETA49_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1EM20VH_FIRSTEMPTY',  'L1_EM20VH_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        #['satu20em_l1satmon_L1EM20VHI_FIRSTEMPTY',  'L1_EM20VHI_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        # the one above was replaced with a em22vhi_firstempty seeded one:
        ['satu20em_l1satmon_L1EM22VHI_FIRSTEMPTY',  'L1_EM22VHI_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1J100',  'L1_J100',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1J100.31ETA49',  'L1_J100.31ETA49',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],

        ]

    TriggerFlags.CosmicSlice.signatures  = [ 
        ['tilecalib_laser',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1],
        #['pixel_noise',        'L1_RD0_EMPTY', [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], -1],
        #['pixel_beam',         'L1_RD0_FILLED', [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['sct_noise',          'L1_RD0_EMPTY', [], ['SCTNoise'], ["RATE:Calibration", "RATE:SCTCalibration", "BW:Detector"], -1],
        
        # CosmicCalo
        ['larps_L1EM3_EMPTY',  'L1_EM3_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1EM7_EMPTY',  'L1_EM7_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1EM7_FIRSTEMPTY',  'L1_EM7_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J12_EMPTY',  'L1_J12_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_EMPTY',  'L1_J30_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1TAU8_EMPTY', 'L1_TAU8_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
	
        ['larps_L1J3031ETA49_EMPTY', 'L1_J30.31ETA49_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        #JetTauEtmiss 
        ['larps_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],

        ['larhec_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1EM7_FIRSTEMPTY', 'L1_EM7_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1TAU8_FIRSTEMPTY', 'L1_TAU8_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
#        ['larhec_L1RD0_FIRSTEMPTY', 'L1_RD0_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        #CosmicID
        ['id_cosmicid_L1MU4_EMPTY',         'L1_MU4_EMPTY', [],  ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        ['id_cosmicid_L1MU11_EMPTY',        'L1_MU11_EMPTY', [], ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],

        #['id_cosmicid',               '', [], ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        #['id_cosmicid_trtxk',         '', [], ['HLT_IDCosmic'],            ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        #['id_cosmicid_trtxk_central', '', [], ['HLT_IDCosmic'],            ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],

        # ['id_cosmicid_ds',            '', [], ['DataScouting_04_IDCosmic'], ['RATE:CosmicSlice', 'BW:Detector'], -1],        

        ]
    TriggerFlags.StreamingSlice.signatures = [

        ['noalg_L1RD0_EMPTY', 'L1_RD0_EMPTY', [],   [PhysicsStream, 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1RD0_FILLED', 'L1_RD0_FILLED', [], [PhysicsStream, 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1RD1_FILLED', 'L1_RD1_FILLED', [], [PhysicsStream], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1RD2_EMPTY', 'L1_RD2_EMPTY', [],   [PhysicsStream], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1RD2_FILLED', 'L1_RD2_FILLED', [], [PhysicsStream], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1RD3_EMPTY', 'L1_RD3_EMPTY', [],   [PhysicsStream], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1RD3_FILLED', 'L1_RD3_FILLED', [], [PhysicsStream], ["RATE:MinBias", "BW:MinBias"], -1],

        ['noalg_standby_L1RD0_FILLED', 'L1_RD0_FILLED', [], ['Standby'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_standby_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], ['Standby'], ["RATE:CoMinBias", "BW:MinBias"], -1], 

        # Egamma streamers
        ['noalg_L1EM18VH',         'L1_EM18VH',         [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],
        ['noalg_L1EM15',           'L1_EM15',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],
        ['noalg_L1EM12',           'L1_EM12',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],
        ['noalg_L12EM7',           'L1_2EM7',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],
        ['noalg_L12EM15',          'L1_2EM15',          [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],
        ['noalg_L1EM15VH_3EM7',    'L1_EM15VH_3EM7',    [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],
        ['noalg_L1EM7',            'L1_EM7',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],
        ['noalg_L1EM3',            'L1_EM3',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],
        ['noalg_L12EM3',           'L1_2EM3',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],

        # Muon streamers
        ['noalg_L1MU20',           'L1_MU20',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU15',           'L1_MU15',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU10',           'L1_MU10',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L12MU4',           'L1_2MU4',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],

        ['noalg_L1MU4_EMPTY',      'L1_MU4_EMPTY',      [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU4_FIRSTEMPTY', 'L1_MU4_FIRSTEMPTY', [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU11_EMPTY',      'L1_MU11_EMPTY',      [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU4_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_ISO', [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],

        ['noalg_L1MU20_FIRSTEMPTY', 'L1_MU20_FIRSTEMPTY', [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        
        ['noalg_mb_L1TE5',  'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['noalg_mb_L1TE10',  'L1_TE10', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE20',  'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['noalg_mb_L1TE30',  'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE40',  'L1_TE40', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['noalg_mb_L1TE50',  'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['noalg_mb_L1TE60',  'L1_TE60', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['noalg_mb_L1TE70',  'L1_TE70', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ['noalg_mb_L1TE5.0ETA24',  'L1_TE5.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['noalg_mb_L1TE10.0ETA24',  'L1_TE10.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE20.0ETA24',  'L1_TE20.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE30.0ETA24',  'L1_TE30.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE40.0ETA24',  'L1_TE40.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE50.0ETA24',  'L1_TE50.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE60.0ETA24',  'L1_TE60.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['noalg_mb_L1TE70.0ETA24',  'L1_TE70.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        # Tau streamers
        ['noalg_L1TAU30',            'L1_TAU30',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU20',            'L1_TAU20',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU20_2TAU12',     'L1_TAU20_2TAU12',     [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU20IM_2TAU12IM', 'L1_TAU20IM_2TAU12IM', [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU12',            'L1_TAU12',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU12IM',            'L1_TAU12IM',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU12IL',            'L1_TAU12IL',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU12IT',            'L1_TAU12IT',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU20IM',            'L1_TAU20IM',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU20IL',            'L1_TAU20IL',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU20IT',            'L1_TAU20IT',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],

        # MET streamers
        ['noalg_L1XE35',             'L1_XE35',             [], [PhysicsStream, 'express'], ['RATE:SeededStreamers', 'BW:MET'], -1],

        ['noalg_L1J15_J15.31ETA49',                'L1_J15_J15.31ETA49',                [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Jets'], -1],
        ['noalg_L1J20_J20.31ETA49',                'L1_J20_J20.31ETA49',                [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Jets'], -1],

        # VBF triggers
        ['noalg_L1J30.0ETA49_2J20.0ETA49',    'L1_J30.0ETA49_2J20.0ETA49',    [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
        ['noalg_L1MJJ-100', 'L1_MJJ-100', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],  

        # Chains for Week1 menu (ATR-11119)
        ['noalg_L1MU4',   'L1_MU4',  [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Muon"], -1 ],
        ['noalg_L1MU6',   'L1_MU6',  [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Muon"], -1 ],
        ['noalg_L1J50',   'L1_J50',  [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],

        #ATR-10976
        ['noalg_L1MU4_3J15',   'L1_MU4_3J15',  [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Bjet"], -1 ],
        ['noalg_L1MU4_J30',    'L1_MU4_J30',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Bjet"], -1 ],
        ['noalg_L1MU6_J20',    'L1_MU6_J20',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Bjet"], -1 ],
        ['noalg_L1MU4_J12',    'L1_MU4_J12',   [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Bjet'], -1],

      
        ['noalg_L1CALREQ2',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1],

        ['noalg_to_L12MU20_OVERLAY',   'L1_2MU20_OVERLAY', [], ['TauOverlay'], ["RATE:TauOverlay",  "BW:TauOverlay"], -1],

        # beam splash streamer
#        ['noalg_L1EM20A',   'L1_EM20A',        [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
#        ['noalg_L1EM20C',   'L1_EM20C',        [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
#        
#        ['noalg_L1J75A',   'L1_J75A',          [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
#        ['noalg_L1J75C',   'L1_J75C',          [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
#        
#        ['noalg_L1TAU20A',   'L1_TAU20A',      [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1], 
#        ['noalg_L1TAU20C',   'L1_TAU20C',      [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1], 
#
        # ALFA_ANY streamer
        ['noalg_L1ALFA_ANY',     'L1_ALFA_ANY',     [], ['MinBias'], ["RATE:MinBias", "RATE:ALFA", "BW:Other"], -1], 
        ['noalg_L1ALFA_Phys',    'L1_ALFA_Phys',    [], ['MinBias'], ["RATE:MinBias", "RATE:ALFA", "BW:Other"], -1], 
        ['noalg_L1ALFA_PhysAny', 'L1_ALFA_PhysAny', [], ['MinBias'], ["RATE:MinBias", "RATE:ALFA", "BW:Other"], -1], 
        ['noalg_L1ALFA_SYS',     'L1_ALFA_SYS',     [], ['MinBias'], ["RATE:MinBias", "RATE:ALFA", "BW:Other"], -1], 

        # ALFA Physics Streamer
        ['noalg_L1ALFA_Diff_Phys',  'L1_ALFA_Diff_Phys',  [], ['MinBias'], ["RATE:ALFA","RATE:MinBias", "BW:Other"], -1], 
        ['noalg_L1ALFA_CDiff_Phys', 'L1_ALFA_CDiff_Phys',  [], ['MinBias'], ["RATE:ALFA","RATE:MinBias", "BW:Other"], -1], 
        ['noalg_L1ALFA_Jet_Phys', 'L1_ALFA_Jet_Phys',  [], ['MinBias'], ["RATE:ALFA","RATE:MinBias", "BW:Other"], -1], 

        # 
        ['noalg_L1TGC_BURST',   'L1_TGC_BURST', [], ['TgcNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        
        #DCM level monitoring test chain
        ['noalg_dcmmon_L1RD0_EMPTY',   'L1_RD0_EMPTY',        [], ['monitoring_random'], ["RATE:DISCARD","BW:DISCARD"], -1], 

        ['noalg_dcmmon_L1RD2_EMPTY',   'L1_RD2_EMPTY',        [], ['monitoring_random'], ["RATE:DISCARD","BW:DISCARD"], -1], 
        ['noalg_dcmmon_L1RD2_BGRP12',  'L1_RD2_BGRP12',       [], ['monitoring_random'], ["RATE:DISCARD","BW:DISCARD"], -1], 

        #background streamers
        ['noalg_bkg_L1Bkg',               'L1_Bkg',               [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1J12',               'L1_J12',               [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1J30.31ETA49',       'L1_J30.31ETA49',       [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1J12_BGRP12',        'L1_J12_BGRP12',         [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1J30.31ETA49_BGRP12', 'L1_J30.31ETA49_BGRP12', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        #['noalg_bkg_L1RD0_UNPAIRED_ISO', 'L1_RD0_UNPAIRED_ISO', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_bkg_L1RD0_EMPTY',        'L1_RD0_EMPTY',        [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        #['noalg_bkg_L1RD1_FILLED',       'L1_RD1_FILLED',       [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 

        ['noalg_L1Standby',              'L1_Standby',          [], ['Standby'],    ["RATE:SeededStreamers", "BW:Other"], -1],         
        #['noalg_idcosmic_L1TRT',         'L1_TRT',              [], ['IDCosmic'],   ["RATE:SeededStreamers", "BW:Other"], -1],        
        ['noalg_idcosmic_L1TRT_EMPTY',          'L1_TRT_EMPTY',              [], ['IDCosmic', 'express'],   ["RATE:SeededStreamers", "BW:Other"], -1],        
        ['noalg_idcosmic_L1TRT_FILLED',         'L1_TRT_FILLED',              [], ['IDCosmic'],   ["RATE:SeededStreamers", "BW:Other"], -1],        

        # standby streamer
        # disabled (ATR-9101) ['noalg_L1Standby',  'L1_Standby',          [], ['Standby'],    ["RATE:SeededStreamers", "BW:Other"], -1], 
        #
        # L1 streamers
        #disabled see #104204   ['noalg_L1Muon',                'L1_Muon',             [], ['L1Muon'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        #disabled see #104204   ['noalg_L1Muon_EMPTY',          'L1_Muon_EMPTY',       [], ['L1Muon'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        
        ['noalg_L1Calo',                  'L1_Calo',             [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_L1Topo',                  'L1_Topo',             [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_larcells_L1LAR-EM',                'L1_LAR-EM',           [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_larcells_L1LAR-J',                 'L1_LAR-J',            [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        #['noalg_L1MinBias',               'L1_MinBias',          [], ['L1MinBias'],  ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_L1Calo_EMPTY',            'L1_Calo_EMPTY',       [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        # #['noalg_L1MinBias_EMPTY',      'L1_MinBias_EMPTY', [], ['L1MinBias'], [], -1], 

        ['noalg_l1calo_L12EM3',   'L1_2EM3',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L12MU4',   'L1_2MU4',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L12MU6',   'L1_2MU6',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L14J15',   'L1_4J15',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L14J20.0ETA49',   'L1_4J20.0ETA49',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1EM12_2EM3',   'L1_EM12_2EM3',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1EM15HI',   'L1_EM15HI',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1EM15I_MU4',   'L1_EM15I_MU4',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1EM3',   'L1_EM3',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1EM15',   'L1_EM15',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1EM7_2EM3',   'L1_EM7_2EM3',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1EM8I_MU10',   'L1_EM8I_MU10',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1EM7',   'L1_EM7',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1J100',   'L1_J100',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1J15',   'L1_J15',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1J20',   'L1_J20',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1J25',   'L1_J25',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1J30.0ETA49_2J20.0ETA49',   'L1_J30.0ETA49_2J20.0ETA49',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1J40_XE50',   'L1_J40_XE50',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1MU10_2MU6',   'L1_MU10_2MU6',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1MU10_TAU12IM',   'L1_MU10_TAU12IM',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1MU4',   'L1_MU4',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1MU4_J30',   'L1_MU4_J30',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1MU6_2MU4',   'L1_MU6_2MU4',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1MU6_J20',   'L1_MU6_J20',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1TAU12IM',   'L1_TAU12IM',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1TAU30',   'L1_TAU30',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1TAU40',   'L1_TAU40',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1XE35',   'L1_XE35',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1XE45',   'L1_XE45',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1XE55',   'L1_XE55',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1XE60',   'L1_XE60',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1calo_L1XE65',   'L1_XE65',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1calo_L1XE70',   'L1_XE70',    [], ['L1Calo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
                                                                                                                                                    
        
        # #minbias streamer
        ['noalg_mb_L1MBTS_1',             'L1_MBTS_1',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_EMPTY',       'L1_MBTS_1_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_UNPAIRED_ISO','L1_MBTS_1_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_2',             'L1_MBTS_2',        [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_2_EMPTY',       'L1_MBTS_2_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_2_UNPAIRED_ISO',  'L1_MBTS_2_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_1',             'L1_MBTS_1_1',        [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_1_EMPTY',       'L1_MBTS_1_1_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_1_UNPAIRED_ISO','L1_MBTS_1_1_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],

        # triggers for VdM scan
        ['noalg_L1MBTS_1_BGRP9',      'L1_MBTS_1_BGRP9',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['noalg_L1MBTS_2_BGRP9',      'L1_MBTS_2_BGRP9',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['noalg_L1RD0_BGRP9',         'L1_RD0_BGRP9',           [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['noalg_L1MBTS_1_BGRP11',     'L1_MBTS_1_BGRP11',       [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['noalg_L1MBTS_2_BGRP11',     'L1_MBTS_2_BGRP11',       [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['noalg_L1RD0_BGRP11',        'L1_RD0_BGRP11',          [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],

	#others
        ['noalg_mb_L1LUCID',             'L1_LUCID',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1LUCID_EMPTY',       'L1_LUCID_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1LUCID_UNPAIRED_ISO','L1_LUCID_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['MinBias', 'express'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD1_FILLED',         'L1_RD1_FILLED',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD2_EMPTY',          'L1_RD2_EMPTY',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD2_FILLED',         'L1_RD2_FILLED',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD3_EMPTY',          'L1_RD3_EMPTY',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD3_FILLED',         'L1_RD3_FILLED',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        # L1 seed doesn't exits ['noalg_mb_L1RD1_UNPAIRED_ISO',  'L1_RD1_UNPAIRED_ISO', [], ['MinBias'], ["BW:Unpaired_MinBias", "RATE:Cosmic_MinBias"], -1],

        # LHCF
        ['noalg_L1LHCF',             'L1_LHCF',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1LHCF_UNPAIRED_ISO','L1_LHCF_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1LHCF_EMPTY',       'L1_LHCF_EMPTY',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
                
        # id cosmic streamer
        #['noalg_cosmicid_L1TRT',    'L1_TRT',              [], ['IDCosmic'], ["BW:MinBias", "RATE:MinBias"], -1],

        #Zero bias streamer
        ['noalg_zb_L1ZB','L1_ZB', [], ['ZeroBias'], ["BW:ZeroBias", "RATE:ZeroBias"], -1],
        #Zero bias plus HLT jet
        ['j40_L1ZB','L1_ZB', [], ['ZeroBias'], ["BW:ZeroBias", "RATE:ZeroBias"], -1],
        

        # Cosmic calo streamer
        ['noalg_cosmiccalo_L1EM3_EMPTY',         'L1_EM3_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],        
        ['noalg_cosmiccalo_L1RD1_EMPTY',         'L1_RD1_EMPTY',        [], ['CosmicCalo', 'express'], ["RATE:Calibration", "BW:Detector"], -1],        
        ['noalg_cosmiccalo_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',[], ['CosmicCalo'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J12_EMPTY',         'L1_J12_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J30_EMPTY',         'L1_J30_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J12_FIRSTEMPTY',    'L1_J12_FIRSTEMPTY',   [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J30_FIRSTEMPTY',    'L1_J30_FIRSTEMPTY',   [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        
        #HLT pass through
        ['noalg_L1All',                  'L1_All',              [], ['HLTPassthrough'], ["RATE:SeededStreamers", "BW:Detector"], -1],
                
        # Enhanced bias Streamer items
        # noalg_eb_ defined in MC menu 
        
        #idmon streamer
        ['noalg_idmon_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['IDMonitoring','express'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],
        
        #cosmicmuon streamer
        ['noalg_cosmicmuons_L1MU4_EMPTY',   'L1_MU4_EMPTY',        [], ['CosmicMuons','express'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],        
        ['noalg_cosmicmuons_L1MU11_EMPTY',  'L1_MU11_EMPTY',        [], ['CosmicMuons','express'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],        

        ['noalg_L1LowLumi', 'L1_LowLumi', [], [PhysicsStream, 'express'], ["RATE:SeededStreamers", "BW:Other"], -1],

        # L1Topo streamers for M8
        #['noalg_L1MJJ-4',   'L1_MJJ-4',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #['noalg_L1MJJ-3',   'L1_MJJ-3',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #['noalg_L1MJJ-2',   'L1_MJJ-2',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #['noalg_L1MJJ-1',   'L1_MJJ-1',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        
        #['noalg_L1MJJ-350-0',   'L1_MJJ-350-0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #['noalg_L1MJJ-300-0',   'L1_MJJ-300-0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #['noalg_L1MJJ-250-0',   'L1_MJJ-250-0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #['noalg_L1MJJ-200-0',   'L1_MJJ-200-0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        
        #['noalg_L1HT200-J20s5.ETA49', 'L1_HT200-J20s5.ETA49', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #['noalg_L1HT190-J20s5.ETA49', 'L1_HT190-J20s5.ETA49', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #['noalg_L1HT1-J0.ETA49',   'L1_HT1-J0.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        
        #['noalg_L1JPSI-1M5-EMs',  'L1_JPSI-1M5-EMs',  [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #['noalg_L1JPSI-1M5-EM6s', 'L1_JPSI-1M5-EM6s', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #['noalg_L1JPSI-1M5-EM12s','L1_JPSI-1M5-EM12s',[], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],

        #['noalg_L1DETA-JJ',         'L1_DETA-JJ',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        
        # M9 L1Topo streamers
        ['noalg_L1LFV-MU',          'L1_LFV-MU',          [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DY-DR-2MU4',      'L1_DY-DR-2MU4',      [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-DR-2MU4',     'L1_BPH-DR-2MU4',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],   
        ['noalg_L1BPH-DR-2MU6',     'L1_BPH-DR-2MU6',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-DR-MU6MU4',   'L1_BPH-DR-MU6MU4',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-2M-2MU4',     'L1_BPH-2M-2MU4',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-2M-2MU6',     'L1_BPH-2M-2MU6',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-4M8-2MU4',    'L1_BPH-4M8-2MU4',    [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-4M8-2MU6',    'L1_BPH-4M8-2MU6',    [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-4M8-MU6MU4',  'L1_BPH-4M8-MU6MU4',  [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DY-BOX-2MU4',     'L1_DY-BOX-2MU4',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DY-BOX-MU6MU4',   'L1_DY-BOX-MU6MU4',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DY-BOX-2MU6',     'L1_DY-BOX-2MU6',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DR-TAU20ITAU12I', 'L1_DR-TAU20ITAU12I', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DR-MU10TAU12I',   'L1_DR-MU10TAU12I',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],

        # Note: These EB noalg chains (bar RD3) are multi-seeded. All L1 seeds must be PS=1 during EB campaign for unweighting to work
        ## Enhanced Bias Physics ##
        ['noalg_eb_L1RD3_FILLED',             'L1_RD3_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1PhysicsLow_noPS',        'L1_PhysicsLow_noPS',        [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1PhysicsHigh_noPS',       'L1_PhysicsHigh_noPS',       [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ## Enhanced Bias Background ##
        ['noalg_eb_L1RD3_EMPTY',              'L1_RD3_EMPTY',              [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EMPTY_noPS',             'L1_EMPTY_noPS',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1FIRSTEMPTY_noPS',        'L1_FIRSTEMPTY_noPS',        [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1UNPAIRED_ISO_noPS',      'L1_UNPAIRED_ISO_noPS',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1UNPAIRED_NONISO_noPS',   'L1_UNPAIRED_NONISO_noPS',   [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1ABORTGAPNOTCALIB_noPS',  'L1_ABORTGAPNOTCALIB_noPS',  [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
	
        #L1Calo requested streamers
	['noalg_L1J100',  'L1_J100',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1],
	['noalg_L1J100.31ETA49',  'L1_J100.31ETA49',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ], 
	['noalg_L1J12',  'L1_J12',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ], 
	['noalg_L1J120',  'L1_J120',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1],
	['noalg_L1J15',  'L1_J15',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
	['noalg_L1J15.28ETA31',  'L1_J15.28ETA31',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
	['noalg_L1J15.31ETA49',  'L1_J15.31ETA49',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
	['noalg_L1J20',  'L1_J20',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
        ['noalg_L1J20.28ETA31',  'L1_J20.28ETA31',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
        ['noalg_L1J20.31ETA49',  'L1_J20.31ETA49',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
        ['noalg_L1J25',  'L1_J25',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
        ['noalg_L1J30',  'L1_J30',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
        ['noalg_L1J30.31ETA49',  'L1_J30.31ETA49',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
        ['noalg_L1J40',  'L1_J40',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
        ['noalg_L1J400',  'L1_J400',   [], [PhysicsStream, 'express'], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
        ['noalg_L1J50.31ETA49',  'L1_J50.31ETA49',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
        ['noalg_L1J75',  'L1_J75',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],
        ['noalg_L1J75.31ETA49',  'L1_J75.31ETA49',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ], 
        ['noalg_L1J85',  'L1_J85',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Jets"], -1 ],

        ['noalg_L1EM8VH',  'L1_EM8VH',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Egamma"], -1 ],
        ['noalg_L1EM22VHI',  'L1_EM22VHI',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Egamma"], -1 ],
        ['noalg_L1EM20VHI',  'L1_EM20VHI',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Egamma"], -1 ],
        ['noalg_L1EM20VH',  'L1_EM20VH',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Egamma"], -1 ],
        ['noalg_L1EM15VH',  'L1_EM15VH',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Egamma"], -1 ],
        ['noalg_L1EM13VH',  'L1_EM13VH',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Egamma"], -1 ],
        ['noalg_L1EM10VH',  'L1_EM10VH',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Egamma"], -1 ],
        ['noalg_L1EM10',  'L1_EM10',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Egamma"],-1],

        ['noalg_L1TAU8',  'L1_TAU8',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Tau"], -1 ],
        ['noalg_L1TAU60',  'L1_TAU60',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Tau"], -1],
        ['noalg_L1TAU40',  'L1_TAU40',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Tau"], -1],

        ['noalg_L1XE80',  'L1_XE80',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XE70',  'L1_XE70',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XE60',  'L1_XE60',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XE55',  'L1_XE55',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XE50',  'L1_XE50',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XE45',  'L1_XE45',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XE40',  'L1_XE40',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
	
        ['noalg_L1TE70',  'L1_TE70',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1TE60',  'L1_TE60',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1TE50',  'L1_TE50',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ], 
        ['noalg_L1TE40',  'L1_TE40',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ], 
        ['noalg_L1TE30',  'L1_TE30',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ], 
        ['noalg_L1TE20',  'L1_TE20',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ], 
        ['noalg_L1TE10',   'L1_TE10', [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1TE5',   'L1_TE5', [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1TE5.0ETA24',   'L1_TE5.0ETA24', [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1TE10.0ETA24',   'L1_TE10.0ETA24', [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1TE20.0ETA24',  'L1_TE20.0ETA24',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ], 
        ['noalg_L1TE30.0ETA24',  'L1_TE30.0ETA24',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ], 
        ['noalg_L1TE40.0ETA24',  'L1_TE40.0ETA24',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ], 
        ['noalg_L1TE50.0ETA24',  'L1_TE50.0ETA24',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ], 
	['noalg_L1TE60.0ETA24',  'L1_TE60.0ETA24',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ], 
        
        ['noalg_L1XS60',  'L1_XS60',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XS50',  'L1_XS50',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XS40',  'L1_XS40',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XS30',  'L1_XS30',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XS20',  'L1_XS20',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
	
        ]

    TriggerFlags.MonitorSlice.signatures = [
        ## # enhancedbias
        ['timeburner', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],   
        
        ['costmonitor', '', [], ['CostMonitoring'], ['RATE:Monitoring','BW:Other'],1],

        #['cscmon_L1MU10',   'L1_MU10',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        #['cscmon_L1EM3',    'L1_EM3',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        #['cscmon_L1J12',    'L1_J12',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['cscmon_L1All', 'L1_All',         [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1]
        
        ## check L1 ['idmon_trkFS_L14J15', 'L1_4J15', [], [PhysicsStream], ['RATE:InDetTracking', 'BW:Detector'], -1],

        ]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = [
        # Enhanced bias HLT items
        ['eb_low_L1RD2_FILLED',               'L1_RD2_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['eb_high_L1RD2_FILLED',              'L1_RD2_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ]

    #TriggerFlags.GenericSlice.signatures = []
    


    ###############################################################
    #################################################################
    signatureList=[]
    for prop in dir(TriggerFlags):
        if prop[-5:]=='Slice':
            sliceName=prop
            slice=getattr(TriggerFlags,sliceName)
            if slice.signatures():
                signatureList.extend(slice.signatures())
            else:
                log.debug('SKIPPING '+str(sliceName))
    mySigList=[]
    for allInfo in signatureList:
        mySigList.append(allInfo[0])
    mydict={}
    for chain in mySigList:
        mydict[chain]=[-1,0,0]
    mydict.update(Prescales.HLTPrescales_cosmics)
    from copy import deepcopy
    Prescales.HLTPrescales_cosmics = deepcopy(mydict)
    

class Prescales:
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {}

    #   Signature name   | [ HLTprescale, HLTpass-through, rerun]
    #   - Prescale values should be a positive integer (default=1)
    #   - If the current pass_through value is non-zero,
    #     the value given here will be used as pass_through rate
    #     Assuming that pass through chains are configured so
    #     in the slice files and won't change. Also prescale
    #     and pass_through will not be used together.
    #   - If only the first value is specified,
    #     the default value of pass-through (=0) will be used
    #----------------------------------------------------------
    HLTPrescales = {
        'larnoiseburst_rerun'       : [ 0, 0 , 1, "LArNoiseBurst"],
        'larnoiseburst_loose_rerun' : [ 0, 0 , 1, "LArNoiseBurst"],
        }

    L1Prescales_cosmics  = {}
    HLTPrescales_cosmics = {}
    chain_list=[
        'e0_perf_L1EM3_EMPTY',
        'g0_perf_L1EM3_EMPTY',
        'ht0_L1J12_EMPTY',
        'id_cosmicid_L1MU11_EMPTY',
        'id_cosmicid_L1MU4_EMPTY',
        'j0_L1J12_EMPTY',
        'j0_perf_bperf_L1J12_EMPTY',
        'larcalib_L1EM3_EMPTY',
        'larcalib_L1J12_EMPTY',
        'larcalib_L1J3031ETA49_EMPTY',
        'larcalib_L1TAU8_EMPTY',
        'larps_L1EM3_EMPTY',
        'larps_L1EM7_EMPTY',
        'larps_L1J12_EMPTY',
        'larps_L1J3031ETA49_EMPTY',
        'larps_L1J30_EMPTY',
        'larps_L1TAU8_EMPTY',
        'mb_sptrk_costr',
        'mb_sptrk_costr_L1RD0_EMPTY',
        'mu4_cosmic_L1MU11_EMPTY',
        'mu4_cosmic_L1MU4_EMPTY',
        'mu4_msonly_cosmic_L1MU11_EMPTY',
        'mu4_msonly_cosmic_L1MU4_EMPTY',
        'sct_noise',
        'tau1_cosmic_ptonly_L1MU4_EMPTY',
        'tau1_cosmic_track_L1MU4_EMPTY',
        'tau8_cosmic_ptonly',
        'tau8_cosmic_track',
        'tilecalib_laser',
        ]
    HLTPrescales_cosmics.update(dict(map(None,chain_list,len(chain_list)*[ [1, 0, 0] ])))


######################################################
# TIGHT physics prescales
######################################################
from copy import deepcopy
# enable if desired: # setting all L1 prescales to 1
#Prescales.L1Prescales = dict([(ctpid,1) for ctpid in Prescales.L1Prescales]) 
 
Prescales.L1Prescales_tight_physics_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tight_physics_prescale = deepcopy(Prescales.HLTPrescales)

chain_list=[
    #
    # ID cosmic
    'id_cosmicid_L1MU11_EMPTY',
    'id_cosmicid_L1MU4_EMPTY',
    #
    # minBias chains
    'mb_mbts_L1MBTS_1',
    'mb_mbts_L1MBTS_1_1',
    'mb_mbts_L1MBTS_1_1_EMPTY',
    'mb_mbts_L1MBTS_1_1_UNPAIRED_ISO',
    'mb_mbts_L1MBTS_1_EMPTY',
    'mb_mbts_L1MBTS_1_UNPAIRED_ISO',
    'mb_mbts_L1MBTS_2',
    'mb_mbts_L1MBTS_2_EMPTY',
    'mb_mbts_L1MBTS_2_UNPAIRED_ISO',
    'mb_perf_L1LUCID',
    'mb_perf_L1LUCID_EMPTY',
    'mb_perf_L1LUCID_UNPAIRED_ISO',
    'mb_perf_L1MBTS_2',
    'mb_perf_L1RD1_FILLED',
    'mb_sp1000_trk70_hmt_L1MBTS_1_1',
    'mb_sp1200_trk75_hmt_L1MBTS_1_1',
    'mb_sp1300_hmtperf_L1TE20',
    'mb_sp1300_hmtperf_L1TE20.0ETA24',
    'mb_sp1300_hmtperf_L1TE30',
    'mb_sp1300_hmtperf_L1TE40',
    'mb_sp1300_hmtperf_L1TE40.0ETA24',
    'mb_sp1300_hmtperf_L1TE10',
    'mb_sp1300_hmtperf_L1TE10.0ETA24',
    'mb_sp1300_hmtperf_L1TE20',
    'mb_sp1300_hmtperf_L1TE20.0ETA24',
    'mb_sp1300_hmtperf_L1TE40',
    'mb_sp1300_hmtperf_L1TE40.0ETA24',
    'mb_sp1300_hmtperf_L1TE5',
    'mb_sp1300_hmtperf_L1TE5.0ETA24',
    'mb_sp1400_pusup550_trk90_hmt_L1TE20',
    'mb_sp1400_pusup550_trk90_hmt_L1TE20.0ETA24',
    'mb_sp1400_pusup550_trk90_hmt_L1TE30',
    'mb_sp1400_pusup550_trk90_hmt_L1TE40',
    'mb_sp1400_pusup550_trk90_hmt_L1TE40.0ETA24',
    'mb_sp1400_pusup550_trk90_hmt_L1TE10',
    'mb_sp1400_pusup550_trk90_hmt_L1TE10.0ETA24',
    'mb_sp1500_trk100_hmt_L1TE40',
    'mb_sp1500_trk100_hmt_L1TE40.0ETA24',
    'mb_sp1400_trk80_hmt_L1MBTS_1_1',
    'mb_sp1400_trk90_hmt_L1TE20',
    'mb_sp1400_trk90_hmt_L1TE20.0ETA24',
    'mb_sp1400_trk90_hmt_L1TE30',
    'mb_sp1400_trk90_hmt_L1TE40',
    'mb_sp1400_trk90_hmt_L1TE40.0ETA24',
    'mb_sp1400_trk90_hmt_L1TE10',
    'mb_sp1400_trk90_hmt_L1TE10.0ETA24',
    'mb_sp1500_hmtperf_L1TE20',
    'mb_sp1500_hmtperf_L1TE20.0ETA24',
    'mb_sp1500_hmtperf_L1TE30',
    'mb_sp1500_hmtperf_L1TE40',
    'mb_sp1500_hmtperf_L1TE40.0ETA24',
    'mb_sp1500_hmtperf_L1TE10',
    'mb_sp1500_hmtperf_L1TE10.0ETA24',
    'mb_sp1500_pusup700_trk100_hmt_L1TE20',
    'mb_sp1500_pusup700_trk100_hmt_L1TE20.0ETA24',
    'mb_sp1500_pusup700_trk100_hmt_L1TE30',
    'mb_sp1500_pusup700_trk100_hmt_L1TE40',
    'mb_sp1500_pusup700_trk100_hmt_L1TE50',
    'mb_sp1500_pusup700_trk100_hmt_L1TE40.0ETA24',
    'mb_sp1500_pusup700_trk100_hmt_L1TE10',
    'mb_sp1500_pusup700_trk100_hmt_L1TE10.0ETA24',
    'mb_sp1500_trk100_hmt_L1TE20',
    'mb_sp1500_trk100_hmt_L1TE20.0ETA24',
    'mb_sp1500_trk100_hmt_L1TE30',
    'mb_sp1500_trk100_hmt_L1TE40',
    'mb_sp1500_trk100_hmt_L1TE40.0ETA24',
    'mb_sp1500_trk100_hmt_L1TE10',
    'mb_sp1500_trk100_hmt_L1TE10.0ETA24',
    'mb_sp1700_hmtperf_L1TE20',
    'mb_sp1700_hmtperf_L1TE20.0ETA24',
    'mb_sp1700_hmtperf_L1TE30',
    'mb_sp1700_hmtperf_L1TE40',
    'mb_sp1700_hmtperf_L1TE40.0ETA24',
    'mb_sp1700_hmtperf_L1TE10',
    'mb_sp1700_hmtperf_L1TE10.0ETA24',
    'mb_sp1800_hmtperf',
    'mb_sp1800_pusup800_trk110_hmt_L1TE20',
    'mb_sp1800_pusup800_trk110_hmt_L1TE20.0ETA24',
    'mb_sp1800_pusup800_trk110_hmt_L1TE30',
    'mb_sp1800_pusup800_trk110_hmt_L1TE40',
    'mb_sp1800_pusup800_trk110_hmt_L1TE40.0ETA24',
    'mb_sp1800_pusup800_trk110_hmt_L1TE10',
    'mb_sp1800_pusup800_trk110_hmt_L1TE10.0ETA24',
    'mb_sp1800_pusup800_trk110_hmt_L1TE50',
    'mb_sp1800_trk110_hmt_L1TE20',
    'mb_sp1800_trk110_hmt_L1TE20.0ETA24',
    'mb_sp1800_trk110_hmt_L1TE30',
    'mb_sp1800_trk110_hmt_L1TE40',
    'mb_sp1800_trk110_hmt_L1TE40.0ETA24',
    'mb_sp1800_trk110_hmt_L1TE10',
    'mb_sp1800_trk110_hmt_L1TE10.0ETA24',
    'mb_sp2100_pusup1000_trk120_hmt_L1TE20',
    'mb_sp2100_pusup1000_trk120_hmt_L1TE30',
    'mb_sp2100_pusup1000_trk120_hmt_L1TE40',
    'mb_sp2100_pusup1000_trk120_hmt_L1TE50',
    'mb_sp2400_pusup1100_trk130_hmt_L1TE20',
    'mb_sp2400_pusup1100_trk130_hmt_L1TE30',
    'mb_sp2400_pusup1100_trk130_hmt_L1TE40',
    'mb_sp2400_pusup1100_trk130_hmt_L1TE50',
    'mb_sp2700_pusup1300_trk140_hmt_L1TE20',
    'mb_sp2700_pusup1300_trk140_hmt_L1TE30',
    'mb_sp2700_pusup1300_trk140_hmt_L1TE40',
    'mb_sp2700_pusup1300_trk140_hmt_L1TE50',
    'mb_sp3000_pusup1400_trk150_hmt_L1TE20',
    'mb_sp3000_pusup1400_trk150_hmt_L1TE30',
    'mb_sp3000_pusup1400_trk150_hmt_L1TE40',
    'mb_sp3000_pusup1400_trk150_hmt_L1TE50',
    'mb_sp3000_pusup1400_trk150_hmt_L1TE60',
    'mb_sp3000_pusup1500_trk160_hmt_L1TE20',
    'mb_sp3000_pusup1500_trk160_hmt_L1TE30',
    'mb_sp3000_pusup1500_trk160_hmt_L1TE40',
    'mb_sp3000_pusup1500_trk160_hmt_L1TE50',
    'mb_sp3000_pusup1500_trk160_hmt_L1TE60',
    'mb_sp2500_hmtperf_L1TE40',
    'mb_sp2_hmtperf',
    'mb_sp2_hmtperf_L1MBTS_1_1',
    'mb_sp2_hmtperf_L1TE5',
    'mb_sp2_hmtperf_L1TE5.0ETA24',
    'mb_sp300_trk10_sumet40_hmt_L1MBTS_1_1',
    'mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1',
    'mb_sp300_trk10_sumet50_hmt_L1RD3_FILLED',
    'mb_sp300_trk10_sumet50_hmt_L1TE20',
    'mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1',
    'mb_sp300_trk10_sumet60_hmt_L1TE40',
    'mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1',
    'mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1',
    'mb_sp400_trk40_hmt_L1MBTS_1_1',
    'mb_sp500_hmtperf',
    'mb_sp600_trk10_sumet40_hmt_L1MBTS_1_1',
    'mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1',
    'mb_sp600_trk10_sumet50_hmt_L1RD3_FILLED',
    'mb_sp600_trk10_sumet50_hmt_L1TE20.0ETA24',
    'mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1',
    'mb_sp600_trk10_sumet60_hmt_L1TE40.0ETA24',
    'mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1',
    'mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1',
    'mb_sp600_trk45_hmt_L1MBTS_1_1',
    'mb_sp700_trk50_hmt_L1MBTS_1_1',
    'mb_sp700_trk50_hmt_L1RD3_FILLED',
    'mb_sp700_trk55_hmt_L1MBTS_1_1',
    'mb_sp900_pusup350_trk60_hmt_L1TE20',
    'mb_sp900_pusup350_trk60_hmt_L1TE20.0ETA24',
    'mb_sp900_pusup350_trk60_hmt_L1TE10',
    'mb_sp900_pusup350_trk60_hmt_L1TE10.0ETA24',
    'mb_sp900_trk60_hmt_L1MBTS_1_1',
    'mb_sp900_trk60_hmt_L1TE20',
    'mb_sp900_trk60_hmt_L1TE20.0ETA24',
    'mb_sp900_trk60_hmt_L1TE10',
    'mb_sp900_trk60_hmt_L1TE10.0ETA24',
    'mb_sp900_trk65_hmt_L1MBTS_1_1',
    'mb_sptrk',
    'mb_sptrk_L1RD0_EMPTY',
    'mb_sptrk_L1RD0_UNPAIRED_ISO',
    'mb_sptrk_L1RD3_FILLED',
    'mb_sptrk_costr',
    'mb_sptrk_costr_L1RD0_EMPTY',
    'mb_sptrk_noisesup',
    'mb_sptrk_noisesup_L1RD0_EMPTY',
    'mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO',
    'mb_sptrk_noisesup_L1RD3_FILLED',
    'mb_sptrk_pt4_L1MBTS_1',
    'mb_sptrk_pt4_L1MBTS_1_1',
    'mb_sptrk_pt4_L1MBTS_2',
    'mb_sptrk_pt4_L1RD3_FILLED',
    'mb_sptrk_pt6_L1MBTS_1',
    'mb_sptrk_pt6_L1MBTS_1_1',
    'mb_sptrk_pt6_L1MBTS_2',
    'mb_sptrk_pt6_L1RD3_FILLED',
    'mb_sptrk_pt8_L1MBTS_1',
    'mb_sptrk_pt8_L1MBTS_1_1',
    'mb_sptrk_pt8_L1MBTS_2',
    'mb_sptrk_pt8_L1RD3_FILLED',
    'mb_sptrk_vetombts2in_L1ALFA_CEP',
    'mb_sptrk_vetombts2in_L1TRT_ALFA_EINE',
    'mb_sptrk_vetombts2in_L1ALFA_ANY',
    'mb_sptrk_vetombts2in_L1ALFA_ANY_UNPAIRED_ISO',
    'mb_sptrk_vetombts2in_L1TRT_ALFA_ANY',
    'mb_sptrk_vetombts2in_L1TRT_ALFA_ANY_UNPAIRED_ISO',
    'mb_sptrk_vetombts2in_peb_L1ALFA_ANY',
    'mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO',
    'mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY',
    'mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY_UNPAIRED_ISO',
    #
    # minBias streamer
    'noalg_mb_L1LUCID',
    'noalg_mb_L1LUCID_EMPTY',
    'noalg_mb_L1LUCID_UNPAIRED_ISO',
    'noalg_mb_L1MBTS_1',
    'noalg_mb_L1MBTS_1_1',
    'noalg_mb_L1MBTS_1_1_EMPTY',
    'noalg_mb_L1MBTS_1_1_UNPAIRED_ISO',
    'noalg_mb_L1MBTS_1_EMPTY',
    'noalg_mb_L1MBTS_1_UNPAIRED_ISO',
    'noalg_mb_L1MBTS_2',
    'noalg_mb_L1MBTS_2_EMPTY',
    'noalg_mb_L1MBTS_2_UNPAIRED_ISO',
    'noalg_mb_L1RD0_EMPTY',
    'noalg_mb_L1RD0_FILLED',
    'noalg_mb_L1RD0_UNPAIRED_ISO',
    'noalg_mb_L1RD1_FILLED',
    'noalg_mb_L1RD2_EMPTY',
    'noalg_mb_L1RD2_FILLED',
    'noalg_mb_L1RD3_EMPTY',
    'noalg_mb_L1RD3_FILLED',
    'noalg_mb_L1TE5',
    'noalg_mb_L1TE5.0ETA24',
    'noalg_mb_L1TE10',
    'noalg_mb_L1TE10.0ETA24',
    'noalg_mb_L1TE20',
    'noalg_mb_L1TE20.0ETA24',
    'noalg_mb_L1TE30',
    'noalg_mb_L1TE30.0ETA24',
    'noalg_mb_L1TE40',
    'noalg_mb_L1TE40.0ETA24',
    'noalg_mb_L1TE50',
    'noalg_mb_L1TE50.0ETA24',
    'noalg_mb_L1TE60',
    'noalg_mb_L1TE60.0ETA24',
    'noalg_mb_L1TE70',
    'noalg_mb_L1TE70.0ETA24',
]
Prescales.HLTPrescales_tight_physics_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
######################################################

