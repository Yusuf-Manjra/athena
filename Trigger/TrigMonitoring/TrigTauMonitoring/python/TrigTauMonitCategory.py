# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags

monitoring_tau_FTK = [
        'tau12_idperf_FTK',
        'tau12_perf_FTK',
        'tau12_perf0_FTK',
        'tau12_perf_FTKNoPrec',
        'tau12_perf0_FTKNoPrec',
        'tau12_medium0_FTK',
        'tau12_medium1_FTK',
        'tau12_medium0_FTKNoPrec',
        'tau12_medium1_FTKNoPrec',
        'tau25_idperf_tracktwo_L1TAU12IM_FTK',
        'tau25_perf_tracktwo_L1TAU12IM_FTK',
        'tau25_medium1_tracktwo_L1TAU12IM_FTK'
] 

monitoring_singleTau = [
    'tau25_medium1_tracktwo'

]
monitoring_tau = [
	'tau0_perf_ptonly_L1TAU12',
    'tau0_perf_ptonly_L1TAU60',
    'tau25_idperf_track',
    'tau25_idperf_tracktwo',
    'tau25_perf_tracktwo',
    'tau25_medium1_tracktwo',
    'tau35_perf_tracktwo_tau25_perf_tracktwo',
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo',
	'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',
    'mu14_iloose_tau25_perf_tracktwo',
    'tau160_idperf_track',
    'tau160_idperf_tracktwo',
    'tau160_perf_tracktwo',
	'tau160_medium1_tracktwo',
	'tau1_cosmic_track_L1MU4_EMPTY',
	'tau1_cosmic_ptonly_L1MU4_EMPTY',
	'tau8_cosmic_ptonly',
	'tau8_cosmic_track'
]

monitoring_tau_pp = [
	'tau0_perf_ptonly_L1TAU12',
	'tau0_perf_ptonly_L1TAU60',
	'tau25_idperf_track',
	'tau25_idperf_tracktwo',
	'tau25_perf_tracktwo',
	'tau25_medium1_tracktwo',
	'tau25_medium1_tracktwoEF',
        'tau25_mediumRNN_tracktwoMVA',
        'tau35_mediumRNN_tracktwoMVA_L1TAU12IM',
	'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',  #keep this as reference for the L1Topo plots
	'mu14_iloose_tau25_perf_tracktwo',   # need to move to the ivarloose chain
	'tau160_idperf_track',
	'tau160_idperf_tracktwo',
	'tau160_perf_tracktwo',
	'tau160_medium1_tracktwo',
    'tau25_idperf_track_L1TAU12IM',
    'tau25_idperf_tracktwo_L1TAU12IM',
    'tau25_perf_tracktwo_L1TAU12IM',
    'tau25_medium1_tracktwo_L1TAU12IM',
    'tau25_idperf_tracktwoMVA_L1TAU12IM', #new
    'tau25_perf_tracktwoMVA_L1TAU12IM', #new
    'tau25_medium1_tracktwoMVA_L1TAU12IM', #new
    'tau25_verylooseRNN_tracktwoMVA_L1TAU12IM', #new
    'tau25_looseRNN_tracktwoMVA_L1TAU12IM', #new
    'tau25_mediumRNN_tracktwoMVA_L1TAU12IM',
    'tau25_tightRNN_tracktwoMVA_L1TAU12IM', #new
    'tau25_verylooseRNN_tracktwo_L1TAU12IM', #new
    'tau25_looseRNN_tracktwo_L1TAU12IM', #new
    'tau25_mediumRNN_tracktwo_L1TAU12IM', #new
    'tau25_tightRNN_tracktwo_L1TAU12IM', #new
    'tau25_medium1_tracktwoEF_L1TAU12IM',
    'tau160_idperf_track_L1TAU100',
    'tau0_perf_ptonly_L1TAU100', #new
    'tau160_idperf_tracktwo_L1TAU100',
    'tau160_perf_tracktwo_L1TAU100',
    'tau160_idperf_tracktwoMVA_L1TAU100', #new
    'tau160_perf_tracktwoMVA_L1TAU100' #new

]

monitoring_tau_validation = [
	'tau0_perf_ptonly_L1TAU12',
	'tau0_perf_ptonly_L1TAU60',
	'tau25_idperf_track',
	'tau25_idperf_tracktwo',
	'tau25_perf_ptonly',
	'tau25_perf_tracktwo',
	'tau25_medium1_tracktwo',
	'tau25_loose1_tracktwo',
	'tau25_tight1_tracktwo',
	'tau25_medium1_tracktwo_L1TAU12',
	'tau25_medium1_tracktwo_L1TAU12IL',
	'tau25_medium1_tracktwo_L1TAU12IT',
	'tau25_loose1_ptonly',
	'tau25_medium1_ptonly',
	'tau25_tight1_ptonly',
	'mu14_iloose_tau25_perf_tracktwo',
	'tau35_perf_tracktwo',
	'tau35_perf_ptonly',
	'tau50_medium1_tracktwo_L1TAU12',
	'tau80_medium1_tracktwo',
	'tau80_medium1_tracktwo_L1TAU60',
	'tau125_medium1_tracktwo',
	'tau125_perf_tracktwo',
	'tau125_perf_ptonly',
	'tau160_idperf_track',
	'tau160_idperf_tracktwo',
	'tau160_perf_tracktwo',
	'tau160_medium1_tracktwo',
	'tau160_medium1HighptL_tracktwo',
	'tau160_medium1HighptM_tracktwo',
	'tau160_medium1HighptH_tracktwo',
	'tau12_idperf_FTK',
	'tau12_perf_FTK',
	'tau12_perf0_FTK',
	'tau12_perf_FTKNoPrec',
        'tau12_perf0_FTKNoPrec',
        'tau12_medium0_FTK',
        'tau12_medium1_FTK',
        'tau12_medium0_FTKNoPrec',
	'tau12_medium1_FTKNoPrec',
        'tau25_idperf_tracktwo_L1TAU12IM_FTK',
        'tau25_perf_tracktwo_L1TAU12IM_FTK',
        'tau25_medium1_tracktwo_L1TAU12IM_FTK'
]

monitoring_tau_cosmic = [
    'tau1_cosmic_track_L1MU4_EMPTY',
    'tau1_cosmic_ptonly_L1MU4_EMPTY',
    'tau8_cosmic_ptonly',
    'tau8_cosmic_track'
]


monitoring_singleTau_cosmic = 'tau8_cosmic_track'
