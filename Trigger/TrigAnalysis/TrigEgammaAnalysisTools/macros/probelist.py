# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# This can be imported from TrigEgammaProbelist also
#from TrigEgammaProbelist import *

# default
# probeListLowMidPtSupportingTriggers
# probeListLowMidPtPhysicsTriggers
# probeListHighPtSupportingTriggers
# probeListHighPtPhysicsTriggers
# probeListPhotonTriggers



# this file contains the following probelist of triggers
#
# default
# probeListLowMidPtSupportingTriggers
# probeListLowMidPtPhysicsTriggers
# probeListHighPtSupportingTriggers
# probeListHighPtPhysicsTriggers
# probeListPhotonTriggers
# 
default = [
	'e26_tight_iloose',
    'e24_tight_iloose',
    'e26_lhtight_iloose',
    'e24_tight_iloose_L1EM20VH',
    'e24_medium_iloose_L1EM20VH',
    'e24_lhmedium_iloose_L1EM20VH',
    'e24_tight_iloose_HLTCalo_L1EM20VH',
    'e24_tight_iloose_L2EFCalo_L1EM20VH',
    'e24_lhtight_iloose_L1EM20V',
    'e24_lhtight_iloose_HLTCalo_L1EM20VH',
    'e24_lhtight_iloose_L2EFCalo_L1EM20VH',
	]

# ProbeList 1 - Low/mid pt supporting triggers
# Tools to Run
# EmulationTool,Counts,Efficiency,Perf
# Samples to run - Zee and a background such as dijet or JF17 or any JF sample

probeListLowMidPtSupportingTriggers = [
	"e0_L2Star_perf_L1EM15",
	"e0_L2Star_perf_L1EM3",
	"e0_perf_L1EM15",
	"e0_perf_L1EM3",
	"e10_etcut_L1EM7",
	"e13_etcut_L1EM10_W-MT25",
	"e13_etcut_L1EM10_W-MT30",
	"e13_etcut_L1W-NOMATCH",
	"e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT",
	"e13_etcut_trkcut",
	"e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30",
	"e14_etcut",
	"e17_etcut_L1EM15",
	"e17_etcut_L2StarA_L1EM15",
	"e17_etcut_L2StarB_L1EM15",
	"e17_etcut_L2StarC_L1EM15",
	"e17_etcut_L2Star_idperf_L1EM15",
	"e17_etcut_idperf_L1EM15",
	"e18_etcut_L1EM15_W-MT35",
	"e18_etcut_trkcut",
	"e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE_XS60",
	"e20_etcut_L1EM12",
	"e30_etcut_L1EM15",
	"e3_etcut",
	"e3_etcut_L1EM3_EMPTY",
	"e4_etcut",
	"e5_etcut",
	"e5_etcut_L1W-05DPHI-EMXE-1",
	"e5_etcut_L1W-05DPHI-JXE-0",
	"e5_etcut_L1W-05RO-XEHT-0",
	"e5_etcut_L1W-08RO-XEHT-0",
	"e5_etcut_L1W-10DPHI-EMXE-0",
	"e5_etcut_L1W-10DPHI-JXE-0",
	"e5_etcut_L1W-15DPHI-EMXE-0",
	"e5_etcut_L1W-15DPHI-JXE-0",
	"e5_etcut_L1W-250RO2-XEHT-0",
	"e5_etcut_L1W-90RO2-XEHT-0",
	"e5_etcut_L1W-HT20-JJ15.ETA49",
	"e9_etcut",
]

# ProbeList 2 Low/mid pt physics triggers
# EmulationTool,Eff,Counts,Perf
# Samples to run Zee and any background sample dijet, JFXX

probeListLowMidPtPhysicsTriggers = [
	"e10_lhvloose_L1EM7",
	"e10_vloose_L1EM7",
	"e12_lhloose",
	"e12_lhloose_L1EM10VH",
	"e12_lhvloose_L1EM10VH",
	"e12_loose",
	"e12_loose_L1EM10VH",
	"e12_vloose_L1EM10VH",
	"e15_lhloose_L1EM13VH",
	"e15_lhvloose_L1EM13VH",
	"e15_lhvloose_L1EM7",
	"e15_loose_L1EM13VH",
	"e15_vloose_L1EM13VH",
	"e15_vloose_L1EM7",
	"e17_lhloose",
	"e17_lhloose_L1EM15",
	"e17_lhloose_L1EM15VHJJ1523ETA49",
	"e17_lhmedium",
	"e17_lhmedium_iloose",
	"e17_lhvloose",
	"e17_loose",
	"e17_loose_L1EM15",
	"e17_loose_L1EM15VHJJ1523ETA49",
	"e17_medium",
	"e17_medium_iloose",
	"e17_vloose",
	"e20_lhmedium",
	"e20_lhvloose",
	"e20_lhvloose_L1EM12",
	"e20_medium",
	"e20_vloose",
	"e20_vloose_L1EM12",
	"e24_lhmedium_L1EM15VH",
	"e24_lhmedium_L1EM18VH",
	"e24_lhmedium_L2Star_idperf_L1EM20VH",
	"e24_lhmedium_cutd0dphideta_L1EM18VH",
	"e24_lhmedium_cutd0dphideta_iloose_L1EM18VH",
	"e24_lhmedium_cutd0dphideta_iloose_L1EM20VH",
	"e24_lhmedium_idperf_L1EM20VH",
	"e24_lhmedium_iloose_HLTCalo_L1EM18VH",
	"e24_lhmedium_iloose_HLTCalo_L1EM20VH",
	"e24_lhmedium_iloose_L1EM18VH",
	"e24_lhmedium_iloose_L1EM20VH",
	"e24_lhmedium_iloose_L2StarA_L1EM20VH",
	"e24_lhmedium_nod0_L1EM18VH",
	"e24_lhmedium_nod0_iloose_L1EM18VH",
	"e24_lhmedium_nodeta_L1EM18VH",
	"e24_lhmedium_nodeta_iloose_L1EM18VH",
	"e24_lhmedium_nodphires_L1EM18VH",
	"e24_lhmedium_nodphires_iloose_L1EM18VH",
	"e24_lhtight_L1EM20VH",
	"e24_lhtight_cutd0dphideta_iloose",
	"e24_lhtight_cutd0dphideta_iloose_L1EM20VH",
	"e24_lhtight_iloose",
	"e24_lhtight_iloose_HLTCalo",
	"e24_lhtight_iloose_HLTCalo_L1EM20VH",
	"e24_lhtight_iloose_L1EM20VH",
	"e24_lhtight_iloose_L2EFCalo_L1EM20VH",
	"e24_lhvloose_L1EM18VH",
	"e24_lhvloose_L1EM20VH",
	"e24_medium1_L1EM18VH",
	"e24_medium1_L2Star_idperf_L1EM18VH",
	"e24_medium1_idperf_L1EM18VH",
	"e24_medium1_iloose_L1EM18VH",
	"e24_medium1_iloose_L2StarA_L1EM18VH",
	"e24_medium_L1EM15VH",
	"e24_medium_L1EM18VH",
	"e24_medium_L2Star_idperf_L1EM20VH",
	"e24_medium_idperf_L1EM20VH",
	"e24_medium_iloose_L1EM18VH",
	"e24_medium_iloose_L1EM20VH",
	"e24_medium_iloose_L2StarA_L1EM20VH",
	"e24_tight1_iloose",
	"e24_tight1_iloose_L1EM20VH",
	"e24_tight_L1EM20VH",
	"e24_tight_iloose",
	"e24_tight_iloose_HLTCalo_L1EM20VH",
	"e24_tight_iloose_L1EM20VH",
	"e24_tight_iloose_L2EFCalo_L1EM20VH",
	"e24_tight_iloose_etisem_L1EM20VH",
	"e24_vloose_L1EM18VH",
	"e24_vloose_L1EM20VH",
	"e25_etcut_L1EM15",
	"e25_lhvloose_L1EM15",
	"e25_vloose_L1EM15",
	"e26_lhtight_cutd0dphideta_iloose",
	"e26_lhtight_iloose",
	"e26_lhtight_iloose_HLTCalo",
	"e26_lhvloose_L1EM20VH",
	"e26_tight1_iloose",
	"e26_tight_iloose",
	"e26_vloose_L1EM20VH",
	"e30_lhvloose_L1EM15",
	"e30_vloose_L1EM15",
]

#ProbeList 3 High pt triggers
#Should run with Zprime and / or WPrime samples
#For Wprime - Perf
#For Zprime - Perf,Counts,Eff,EmulationTool
#If we have Zprime, check the Z'->ee mass and change the mass window to select events in the Z' mass window.
#Separate into supporting and primary triggers

probeListHighPtSupportingTriggers = [
# Etcut Trigger list
	"e100_etcut",
	"e120_etcut",
	"e40_etcut_L1EM15",
	"e60_etcut",
	"e80_etcut",
	"e50_etcut_L1EM15",
]

probeListHighPtPhysicsTriggers = [
# The Primary Trigger List
	"e120_lhloose",
	"e120_lhloose_HLTCalo",
	"e120_lhvloose",
	"e120_loose",
	"e120_loose1",
	"e120_vloose",
	"e140_lhloose",
	"e140_lhloose_HLTCalo",
	"e140_loose",
	"e40_lhvloose_L1EM15",
	"e40_vloose_L1EM15",
	"e50_lhvloose_L1EM15",
	"e50_vloose_L1EM15",
	"e60_lhloose",
	"e60_lhmedium",
	"e60_lhmedium_HLTCalo",
	"e60_lhmedium_cutd0dphideta",
	"e60_lhvloose",
	"e60_loose",
	"e60_medium",
	"e60_medium1",
	"e60_vloose",
	"e70_lhloose",
	"e70_lhvloose",
	"e70_loose",
	"e70_vloose",
	"e80_lhvloose",
	"e80_vloose",
]

# ProbeList 4 Photon triggers
# EmulationTool,Perf
# Sample ggH125 and background dijet / JFXX

probeListPhotonTriggers = [
	"g0_hiptrt_L1EM18VH",
	"g0_hiptrt_L1EM20VH",
	"g0_hiptrt_L1EM20VHI",
	"g0_hiptrt_L1EM22VHI",
	"g0_perf_L1EM15",
	"g0_perf_L1EM3",
	"g100_loose",
	"g10_etcut",
	"g10_loose",
	"g10_medium",
	"g120_loose",
	"g120_loose1",
	"g120_loose_HLTCalo",
	"g140_loose",
	"g15_loose",
	"g15_loose_L1EM3",
	"g15_loose_L1EM7",
	"g200_etcut",
	"g20_etcut_L1EM12",
	"g20_loose",
	"g20_loose_L1EM12",
	"g20_loose_L1EM15",
	"g20_tight",
	"g25_loose",
	"g25_loose_L1EM15",
	"g25_medium",
	"g35_loose",
	"g35_loose_L1EM15",
	"g35_loose_L1EM15_g25_loose_L1EM15",
	"g35_loose_g25_loose",
	"g35_medium",
	"g3_etcut",
	"g3_etcut_L1EM3_EMPTY",
	"g40_loose_L1EM15",
	"g40_tight",
	"g45_loose_L1EM15",
	"g45_tight",
	"g50_loose",
	"g50_loose_L1EM15",
	"g60_loose",
	"g60_loose_L1EM15VH",
	"g70_loose",
	"g80_loose",
]

probelist = default