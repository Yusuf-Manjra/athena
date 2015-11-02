# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Physics_pp_v5'

rules={
    'L1_EM3'                        : { 2 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_TAU8'                       : { 2 : TriggerRule(PS=1, comment='L1', rerun=0) },

    'HLT_noalg_cosmicmuons_L1MU4_EMPTY'        : { 10000 : TriggerRule(PS=1, comment='Cosmic Muon', rerun=0 ,ESValue=50) },
    'HLT_noalg_cosmicmuons_L1MU11_EMPTY'        : { 10000 : TriggerRule(PS=1, comment='Cosmic Muon', rerun=0 ,ESValue=1) },

    'L1_RD0_FILLED'        : { 10000 : TriggerRule(rate=100,   scaling="bunches", comment='L1', rerun=0) },
    'L1_RD0_UNPAIRED_ISO'  : { 10000 : TriggerRule(rate=100,    scaling="bunches", comment='L1', rerun=0) },
    'L1_RD0_ABORTGAPNOTCALIB' : { 10000 : TriggerRule(rate=100,    scaling="bunches", comment='L1', rerun=0) },
    'L1_RD0_EMPTY'         : { 10000 : TriggerRule(rate=100,   scaling="bunches", comment='L1', rerun=0) },
    'L1_RD1_EMPTY'         : { 10000 : TriggerRule(rate=5,    scaling="bunches", comment='L1', rerun=0) },
    'L1_RD2_EMPTY'         : { 10000 : TriggerRule(rate=10, scaling="bunches", comment='L1', rerun=0) },

    }

Cosmic=[
    'HLT_mu4_cosmic_L1MU4_EMPTY',
    'HLT_mu4_cosmic_L1MU11_EMPTY',
    'HLT_mu4_msonly_cosmic_L1MU4_EMPTY',
    'HLT_mu4_msonly_cosmic_L1MU11_EMPTY',
    'HLT_j0_L1J12_EMPTY',
    'HLT_ht0_L1J12_EMPTY',
    'HLT_j0_perf_bperf_L1J12_EMPTY',
    'HLT_tau8_cosmic_track',
    'HLT_tau8_cosmic_ptonly',
    'HLT_tau1_cosmic_track_L1MU4_EMPTY',
    'HLT_tau1_cosmic_ptonly_L1MU4_EMPTY',
    'HLT_g0_perf_L1EM3_EMPTY',
    'HLT_e0_perf_L1EM3_EMPTY',
    'HLT_mb_sptrk_costr',
    'HLT_mb_sptrk_costr_L1RD0_EMPTY',
    #'HLT_tilelarcalib_L1EM3_EMPTY',
    #'HLT_tilelarcalib_L1TAU8_EMPTY',
    #'HLT_tilelarcalib_L1J12_EMPTY',
    #'HLT_tilelarcalib_L1J3031ETA49_EMPTY':,
    ## 'HLT_ibllumi_L1RD0_EMPTY',
    ## 'HLT_pixel_noise',
    ###'HLT_larps_L1TAU8_EMPTY',
   
    #'HLT_id_cosmicid',
    #'HLT_id_cosmicid_trtxk',
    #'HLT_id_cosmicid_trtxk_central',
    ]    

# if you don't import monitoring rules, need to include this block.
inMonitoring=[
    'HLT_id_cosmicid_L1MU11_EMPTY',
    'HLT_tilecalib_laser',
    'HLT_larcalib_L1TAU8_EMPTY',
    'HLT_larcalib_L1J3031ETA49_EMPTY',
    'HLT_larcalib_L1J12_EMPTY',
    'HLT_larps_L1TAU8_EMPTY',
    'HLT_id_cosmicid_L1MU4_EMPTY',
    'HLT_sct_noise',
    'HLT_larcalib_L1EM3_EMPTY',
    'HLT_larps_L1EM7_EMPTY',
    'HLT_larps_L1J30_EMPTY',
    'HLT_larps_L1J12_EMPTY',
    'HLT_larps_L1EM3_EMPTY',
]


Cosmic_Rules=dict(map(None,Cosmic,len(Cosmic)*[{
                1 : TriggerRule(PS=1, comment='Cosmic'),
                }]))

# monitoring
import Monitoring_pp_v5_rules  

RulesList=[ Cosmic_Rules, Monitoring_pp_v5_rules.physics_rules ]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print "FATAL: duplicated rule cannot be added.", newrule            
    rules.update(Rules)
            

