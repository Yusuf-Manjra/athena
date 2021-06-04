# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# LS2_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

import TriggerMenuMT.HLTMenuConfig.Menu.MC_pp_run3_v1 as mc_menu
import TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_pp_run3_v1 as p1_menu

# this is not the best option, due to flake violation, this list has to be changed when some groups are removed
from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import (PhysicsStream, SingleMuonGroup, MultiMuonGroup, SingleElectronGroup,
                                                                MultiElectronGroup, SinglePhotonGroup, MultiPhotonGroup, SingleMETGroup, 
                                                                MultiMETGroup, SingleJetGroup, MultiJetGroup, SingleBjetGroup, MultiBjetGroup,
                                                                EgammaBjetGroup, MuonBjetGroup,
                                                                SingleTauGroup, BphysicsGroup, EgammaMuonGroup, EgammaMETGroup,
                                                                MuonJetGroup, MuonMETGroup, EgammaJetGroup,
                                                                JetMETGroup, TauJetGroup, MinBiasGroup,PrimaryLegGroup, PrimaryL1MuGroup,
                                                                 BphysElectronGroup, SupportLegGroup)

def setupMenu():

    mc_menu.setupMenu()
    p1_menu.addP1Signatures()

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( __name__ )
    log.info('[setupMenu] going to add the Dev menu chains now')

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + []


    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        #ATR-19985
        ChainProp(name='HLT_mu6_mu6noL1_L1MU6', l1SeedThresholds=['MU6','FSNOSEED'], groups=MultiMuonGroup),

        #test chains
        ChainProp(name='HLT_mu6_L1MU6',     groups=SingleMuonGroup),

        ChainProp(name='HLT_mu6_LRT_idperf_l2lrt_L1MU6',     groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6_LRT_idperf_L1MU6',           groups=SingleMuonGroup),
        ChainProp(name='HLT_mu24_LRT_l2lrt_d0loose_L1MU20',     groups=SingleMuonGroup),
        ChainProp(name='HLT_mu24_LRT_l2lrt_d0medium_L1MU20',     groups=SingleMuonGroup),
        ChainProp(name='HLT_mu24_LRT_l2lrt_d0tight_L1MU20',     groups=SingleMuonGroup),
        ChainProp(name='HLT_mu24_LRT_d0loose_L1MU20',     groups=SingleMuonGroup),
        ChainProp(name='HLT_mu24_LRT_d0medium_L1MU20',     groups=SingleMuonGroup),
        ChainProp(name='HLT_mu24_LRT_d0tight_L1MU20',     groups=SingleMuonGroup),

        ChainProp(name='HLT_mu6_ivarmedium_L1MU6', groups=SingleMuonGroup),

        # commented because it is conflict with dimuon noL1 serial chain
        # ChainProp(name='HLT_mu6noL1_L1MU6', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup),

        ChainProp(name='HLT_mu6_msonly_L1MU6',     groups=SingleMuonGroup, monGroups=['muonMon:shifter','muonMon:val','idMon:t0']),

        ChainProp(name='HLT_2mu6_10invm70_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu10_lateMu_L1LATE-MU10_XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup),

        # ATR-20049
        ChainProp(name='HLT_mu6_mu4_L12MU4',  l1SeedThresholds=['MU4']*2, groups=MultiMuonGroup),

        # Additional intermediate thresholds for validation comparisons to match with Physics_pp_v7_primaries menu
        ChainProp(name='HLT_mu24_mu10noL1_L1MU20', l1SeedThresholds=['MU20','FSNOSEED'], groups=MultiMuonGroup),
        ChainProp(name="HLT_mu10_L1MU10", groups=SingleMuonGroup),
        ChainProp(name='HLT_2mu4_L12MU4',  groups=MultiMuonGroup),

        # ATR-19452
        ChainProp(name='HLT_2mu4_muonqual_L12MU4',  groups=MultiMuonGroup),
        ChainProp(name='HLT_2mu6_muonqual_L12MU6',  groups=MultiMuonGroup),

        # ATR-20650
        ChainProp(name='HLT_mu0_muoncalib_L1MU4_EMPTY', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu0_muoncalib_L1MU20',      groups=SingleMuonGroup),

        # Unisolated single muons for monitoring isolated muons
        ChainProp(name="HLT_mu8_L1MU6", groups=SingleMuonGroup),
        ChainProp(name="HLT_mu14_L1MU10", groups=SingleMuonGroup),
        ChainProp(name="HLT_mu24_L1MU20", groups=SingleMuonGroup),

        #ATR-21003
        ChainProp(name='HLT_mu4_l2io_L1MU4', groups=SingleMuonGroup),
        ChainProp(name='HLT_2mu14_l2io_L12MU10', groups=MultiMuonGroup),
        ChainProp(name='HLT_2mu6_l2io_L12MU6',     l1SeedThresholds=['MU6'],   groups=MultiMuonGroup),

        #ATR-22537
        ChainProp(name='HLT_2mu10_l2mt_L1MU10', groups=MultiMuonGroup),

        #performance chain for isolation (ATR-21905)
        ChainProp(name='HLT_mu26_ivarperf_L1MU20', groups=SingleMuonGroup),

        #-- nscan ATR-19376
        ChainProp(name='HLT_mu20_mu6noL1_nscan_L1MU20', l1SeedThresholds=['MU20','FSNOSEED'],   groups=MultiMuonGroup),
        

    ]

    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
        # ElectronChains----------
        ChainProp(name='HLT_e3_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e7_etcut_L1EM3', stream=[PhysicsStream, 'express'], groups=SingleElectronGroup, monGroups=['electronMon:shifter']),

        ChainProp(name='HLT_2e3_etcut_L12EM3', stream=[PhysicsStream], groups=MultiElectronGroup),

        # low et threshold for debugging
        ChainProp(name='HLT_e5_lhloose_noringer_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhmedium_noringer_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_nod0_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_idperf_loose_L1EM3', groups=SingleElectronGroup),        #idperf chains
        ChainProp(name='HLT_e5_idperf_medium_L1EM3', groups=SingleElectronGroup),        #idperf chains
        ChainProp(name='HLT_e5_idperf_tight_L1EM3', groups=SingleElectronGroup),        #idperf chains

        # low et threshold chains using Ringer
        ChainProp(name='HLT_e5_lhloose_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhmedium_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_gsf_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_nod0_L1EM3', groups=SingleElectronGroup),

        # dnn chain
        ChainProp(name='HLT_e5_dnnloose_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_dnnmedium_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_dnntight_L1EM3', groups=SingleElectronGroup),

        # lrt chains
        # Commented out: ATR-23564
        # ChainProp(name='HLT_e5_lhloose_lrtloose_L1EM3', groups=SingleElectronGroup),
        # ChainProp(name='HLT_e5_lhmedium_lrtmedium_L1EM3', groups=SingleElectronGroup),
        # ChainProp(name='HLT_e5_lhtight_lrttight_L1EM3', groups=SingleElectronGroup),

        # ChainProp(name='HLT_e26_lhloose_lrtloose_L1EM22VHI', groups=SingleElectronGroup),
        # ChainProp(name='HLT_e26_lhmedium_lrtloose_L1EM22VHI', groups=SingleElectronGroup),
        # ChainProp(name='HLT_e26_lhtight_lrtloose_L1EM22VHI', groups=SingleElectronGroup),

        ChainProp(name='HLT_e5_idperf_loose_lrtloose_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_idperf_loose_lrtloose_L1EM22VHI', groups=SingleElectronGroup),  

        # Primary
        ChainProp(name='HLT_e17_lhvloose_nod0_L1EM15VH',  groups=SingleElectronGroup),
        ChainProp(name='HLT_e17_lhvloose_nod0_L1EM15VHI',  groups=SingleElectronGroup),
        ChainProp(name='HLT_e17_lhvloose_L1EM15VH',  groups=SingleElectronGroup),
        ChainProp(name='HLT_e17_lhvloose_L1EM15VHI',  groups=SingleElectronGroup),
        ChainProp(name='HLT_e17_lhvloose_gsf_L1EM15VHI',  groups=SingleElectronGroup),
        ChainProp(name='HLT_e24_lhvloose_gsf_L1EM20VH', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhloose_L1EM15VH', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhmedium_L1EM15VH', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_L1EM15VH', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhloose_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhmedium_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_gsf_L1EM22VHI', groups=SingleElectronGroup), #Placeholder for GSF chain
        ChainProp(name='HLT_e26_lhtight_gsf_ivarloose_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarmedium_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivartight_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_nod0_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_nod0_L1EM24VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_idperf_loose_L1EM24VHI', groups=SingleElectronGroup),      #idperf chains
        ChainProp(name='HLT_e28_idperf_loose_L1EM24VHI', groups=SingleElectronGroup),      #idperf chains
        ChainProp(name='HLT_e60_lhmedium_nod0_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e140_lhloose_nod0_L1EM22VHI', groups=SingleElectronGroup),

        ChainProp(name='HLT_2e17_lhvloose_L12EM15VH', stream=[PhysicsStream], groups=MultiElectronGroup),

        ## ATR-22109
        # Note: l1SeedThresholds needs to be given explicitly, as it cannot
        # be auto-filled by DictFromChainName for L1Topo items
        ChainProp(name='HLT_e20_lhtight_ivarloose_L1ZAFB-25DPHI-eEM18I', l1SeedThresholds=['EM12'], stream=[PhysicsStream], groups=PrimaryLegGroup+MultiElectronGroup),

        ## Zee triggers
        ChainProp(name='HLT_e20_lhmedium_e15_idperf_medium_Zee_L1EM15VH', l1SeedThresholds=['EM15VH','EM7'], groups=MultiElectronGroup), #idperf leg

        ## Jpsiee triggers
        ChainProp(name='HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',l1SeedThresholds=['EM7','EM3'], groups=MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_e9_etcut_Jpsiee_L1JPSI-1M5-EM7',l1SeedThresholds=['EM3','EM7'],groups=MultiElectronGroup),
        ChainProp(name='HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',l1SeedThresholds=['EM12','EM3'],groups=MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_e14_etcut_Jpsiee_L1JPSI-1M5-EM12',l1SeedThresholds=['EM3','EM12'],groups=MultiElectronGroup),
        ChainProp(name='HLT_e9_lhtight_noringer_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',l1SeedThresholds=['EM7','EM3'],groups=MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_e9_etcut_Jpsiee_L1JPSI-1M5-EM7',l1SeedThresholds=['EM3','EM7'],groups=MultiElectronGroup),
        ChainProp(name='HLT_e14_lhtight_noringer_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',l1SeedThresholds=['EM12','EM3'],groups=MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_e14_etcut_Jpsiee_L1JPSI-1M5-EM12',l1SeedThresholds=['EM3','EM12'],groups=MultiElectronGroup),

        # electron forward triggers (keep this only for dev now)
        #ChainProp(name='HLT_e30_etcut_fwd_L1EM22VHI', groups=SingleElectronGroup),
        # for moving to PhysicsP1, ATR-21242
        # ChainProp(name='HLT_2e17_etcut_L12EM15VHI', stream=[PhysicsStream], groups=MultiElectronGroup),

        #ATR-22749
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L12EM3', l1SeedThresholds=['EM3'], groups=BphysElectronGroup),


        # PhotonChains------------
        # these are to debug photon working points should be removed in production
        ChainProp(name='HLT_g5_etcut_L1EM3', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g5_loose_L1EM3', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g5_medium_L1EM3', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g5_tight_L1EM3', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g25_etcut_L1EM20VH', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g25_loose_L1EM20VH', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_loose_L1EM15VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_medium_L1EM15VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_tight_L1EM15VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_tight_icalomedium_L1EM15VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_tight_icalotight_L1EM15VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_loose_L1EM15VH', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_medium_L1EM15VH', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_tight_L1EM15VH', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_tight_icalomedium_L1EM15VH', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g20_tight_icalotight_L1EM15VH', groups=SinglePhotonGroup),
        # Low eT photon chains for TLA
        ChainProp(name='HLT_g35_loose_L1EM22VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g35_medium_L1EM22VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g35_tight_L1EM22VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g35_tight_icaloloose_L1EM22VHI', groups=SinglePhotonGroup),

        
     
        # PhotonDS chains for TLA
        ChainProp(name='HLT_g35_loose_PhotonDS_L1EM22VHI',stream=['PhotonDS'], groups=SinglePhotonGroup),
        ChainProp(name='HLT_2g5_tight_PhotonDS_L12EM15VHI', stream=['PhotonDS'], groups=MultiPhotonGroup),
     
        # Primary photon chains
        ChainProp(name='HLT_g120_loose_L1EM22VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_2g35_medium_L12EM20VH', groups=MultiPhotonGroup),
        ChainProp(name='HLT_2g20_tight_L12EM15VHI', groups=MultiPhotonGroup),


        # ATR-21355 - cannot be moved to the calibSlice because they need to configure the photon/ sequence
        ChainProp(name='HLT_g3_loose_LArPEBHLT_L1EM3',	   stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),
        ChainProp(name='HLT_g12_loose_LArPEBHLT_L1EM10VH', stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),
        ChainProp(name='HLT_g20_loose_LArPEBHLT_L1EM15',   stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),
        ChainProp(name='HLT_g40_loose_LArPEBHLT_L1EM20VHI',stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),
        ChainProp(name='HLT_g60_loose_LArPEBHLT_L1EM20VHI',stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),
        ChainProp(name='HLT_g80_loose_LArPEBHLT_L1EM20VHI',stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),

        # for moving to PhysicsP1, ATR-21242
        ChainProp(name='HLT_g140_etcut_L1EM22VHI', groups=SinglePhotonGroup),

        #ATR-21882
        ChainProp(name='HLT_2g15_tight_dPhi15_L1DPHI-M70-2eEM12I', l1SeedThresholds=['EM12'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g15_tight_dPhi15_m80_L1DPHI-M70-2eEM12I', l1SeedThresholds=['EM12'], groups=PrimaryLegGroup+MultiPhotonGroup),

        #Support photon chains ATR-23425
        ChainProp(name='HLT_2g15_loose_dPhi15_L1DPHI-M70-2eEM12I', l1SeedThresholds=['EM12'], groups=SupportLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g20_loose_L12EM15VH', groups=SupportLegGroup+MultiPhotonGroup),
       
    ]

    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
        ChainProp(name='HLT_xe30_cell_L1XE30', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mht_L1XE30', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_tcpufit_L1XE30', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_trkmht_L1XE30', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_L1XE30', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_cssk_L1XE30', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_vssk_L1XE30', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfopufit_L1XE30', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_cvfpufit_L1XE30', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mhtpufit_em_subjesgscIS_L1XE30', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mhtpufit_pf_subjesgscIS_L1XE30', groups=SingleMETGroup),

        ChainProp(name='HLT_xe110_tc_em_L1XE50', groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_mht_L1XE50', groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_tcpufit_L1XE50', groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_pfsum_L1XE50', groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_pfsum_cssk_L1XE50', groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_pfsum_vssk_L1XE50', groups=SingleMETGroup),
        # MultiMET Chain
        ChainProp(name='HLT_xe30_cell_xe30_tcpufit_L1XE30', l1SeedThresholds=['FSNOSEED']*2, groups=MultiMETGroup), #must be FS seeded
        # ATR-21934
        ChainProp(name='HLT_xe100_trkmht_xe85_tcpufit_xe65_cell_L1XE50',l1SeedThresholds=['FSNOSEED']*3,  groups=PrimaryLegGroup+MultiMETGroup),
        ChainProp(name='HLT_xe95_trkmht_xe90_tcpufit_xe75_cell_L1XE50', l1SeedThresholds=['FSNOSEED']*3,  groups=PrimaryLegGroup+MultiMETGroup),
    ]


    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        ChainProp(name='HLT_j85_L1J20', groups=[SingleJetGroup]),
        ChainProp(name='HLT_j85_CLEANlb_L1J20', groups=[SingleJetGroup]),
        ChainProp(name='HLT_j45_L1J15', groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgsc_ftf_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subresjesgsc_ftf_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_011jvt_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_015jvt_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_059jvt_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j85_ftf_L1J20', groups=SingleJetGroup),

        # Test chains with moment cuts
        ChainProp(name='HLT_j85_050momemfrac100_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j85_momhecfrac010_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j85_050momemfrac100XXmomhecfrac010_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_j45_pf_subresjesgsc_ftf_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_010jvt_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_020jvt_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_050jvt_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_preselj20_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesIS_ftf_preselj20_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_subjesgsc_ftf_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_subjesgscIS_ftf_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j85_pf_ftf_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_j45_nojcalib_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_sk_nojcalib_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_cssk_nojcalib_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_nojcalib_ftf_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_cssk_pf_nojcalib_ftf_L1J15', groups=SingleJetGroup),

        ChainProp(name='HLT_j260_320eta490_L1J20', groups=['PS:Online']+SingleJetGroup),

        ChainProp(name='HLT_j460_a10_lcw_subjes_L1J20', groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1J20', groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j460_a10t_lcw_nojcalib_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10t_lcw_nojcalib_35smcINF_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_nojcalib_35smcINF_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_lcw_nojcalib_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_pf_nojcalib_ftf_L1J100', groups=SingleJetGroup),

        ChainProp(name='HLT_j460_a10sd_cssk_pf_nojcalib_ftf_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_nojcalib_ftf_35smcINF_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_nojcalib_ftf_35smcINF_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_jes_ftf_35smcINF_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_L1J100', groups=SingleJetGroup),

        ChainProp(name='HLT_j0_FBDJNOSHARED10etXX20etXX34massXX50fbet_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_FBDJSHARED_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j60_j0_FBDJSHARED_L1J20', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

        # HT chains using pt conditions
        ChainProp(name='HLT_j0_HT1000_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_HT500_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50XX010jvt_L1J20', groups=SingleJetGroup),

        # HT chains using et conditions
        ChainProp(name='HLT_j0_HT1000XX30et_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_HT500XX30et_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50XX30et_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50XX30etXX010jvt_L1J20', groups=SingleJetGroup),

        # dijet chains using pt
        ChainProp(name='HLT_j0_DIJET80j12ptXX0j12eta240XX700djmass_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET80j12ptXX700djmassXXdjdphi260_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET70j12ptXX1000djmassXXdjdphi200XX400djdeta_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET20j12ptXX110djmass_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET20j12ptXX110djmass_PTRANGE2r3_L1J20', groups=SingleJetGroup),

        # dijet chains using et
        ChainProp(name='HLT_j0_DIJET80j12etXX0j12eta240XX700djmass_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET80j12etXX700djmassXXdjdphi260_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET70j12etXX1000djmassXXdjdphi200XX400djdeta_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET20j12etXX110djmass_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET20j12etXX110djmass_PTRANGE2r3_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_j85_ftf_MASK300ceta210XX300nphi10_L1J20',
                  groups=SingleJetGroup),

        ChainProp(name='HLT_j40_j0_HT50XX10etXX0eta320_L1J20',
                  l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),
        ChainProp(name='HLT_j40_j0_HT50XX10ptXX0eta320_L1J20',
                  l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup), # same as above but using pt conditions

        # ATR-20624
        ChainProp(name='HLT_j0_perf_L1J12_EMPTY', stream=['Main'], groups=['PS:Online']+SingleJetGroup),

        # ATR-21368 - cannot be moved to the calibSlice because they need to configure the jet sequence
        ChainProp(name='HLT_j40_LArPEBHLT_L1J20',			stream=['LArCells'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j75_320eta490_LArPEBHLT_L1J30_31ETA49',	stream=['LArCells'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j140_320eta490_LArPEBHLT_L1J75_31ETA49',	stream=['LArCells'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j165_LArPEBHLT_L1J100',			stream=['LArCells'], groups=['PS:Online']+SingleJetGroup),

        # TLA test chains, ATR-20395
        ChainProp(name='HLT_j20_JetDS_L1J100', stream=['JetDS'], groups=SingleJetGroup),
        ChainProp(name='HLT_j20_JetDS_L1J50_DETA20-J50J', l1SeedThresholds=['FSNOSEED'], stream=['JetDS'], groups=SingleJetGroup),
        # TLA test HT chains, ATR-21594
        ChainProp(name='HLT_j20_JetDS_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], stream=['JetDS'], groups=SingleJetGroup),
        # Piggybacking on HH4b, exact thresholds for preselections to be determined later - this is for rate and cost
        ChainProp(name='HLT_j60_j45_2j20_JetDS_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*3, stream=['JetDS'], groups=MultiJetGroup),

       
        # ATR-22096
        ChainProp(name='HLT_j420_ftf_L1J100',             groups=SingleJetGroup+ PrimaryLegGroup),
        ChainProp(name='HLT_j420_pf_ftf_L1J100',          groups=SingleJetGroup+ PrimaryLegGroup),

        ChainProp(name='HLT_3j200_ftf_L1J100',            groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_3j200_pf_ftf_L1J100',         groups=MultiJetGroup + PrimaryLegGroup),

        ChainProp(name='HLT_4j120_subjesIS_L13J50',       groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_4j115_ftf_L13J50',            groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_4j115_pf_ftf_L13J50',         groups=MultiJetGroup + PrimaryLegGroup),

        ChainProp(name='HLT_5j70_ftf_0eta240_L14J15',     groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j70_pf_ftf_0eta240_L14J15',  groups=MultiJetGroup + PrimaryLegGroup),

        ChainProp(name='HLT_5j85_subjesIS_L14J15',        groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j85_ftf_L14J15',             groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j85_pf_ftf_L14J15',          groups=MultiJetGroup + PrimaryLegGroup),

        ChainProp(name='HLT_6j55_subjesIS_0eta240_L14J15',groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j55_ftf_0eta240_L14J15',     groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j55_pf_ftf_0eta240_L14J15',  groups=MultiJetGroup + PrimaryLegGroup),

        ChainProp(name='HLT_6j70_subjesIS_L14J15',        groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j70_ftf_L14J15',             groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j70_pf_ftf_L14J15',          groups=MultiJetGroup + PrimaryLegGroup),

        ChainProp(name='HLT_7j45_subjesIS_L14J15',        groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_7j45_ftf_L14J15',             groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_7j45_pf_ftf_L14J15',          groups=MultiJetGroup + PrimaryLegGroup),

        ChainProp(name='HLT_10j40_subjesIS_L14J15',       groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_10j40_ftf_L14J15',            groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_10j40_pf_ftf_L14J15',         groups=MultiJetGroup + PrimaryLegGroup),


        #  Chains to test JVT and PFlow effects in low-threshold 6j
        ChainProp(name='HLT_6j25_0eta240_L14J15',                           groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_L14J15',        groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_010jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_020jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_050jvt_L14J15', groups=MultiJetGroup),

        ChainProp(name='HLT_6j35_0eta240_L14J15',                           groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_L14J15',        groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_010jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_020jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_050jvt_L14J15', groups=MultiJetGroup),

        ChainProp(name='HLT_6j45_0eta240_L14J15',                           groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_L14J15',        groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_010jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_020jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_050jvt_L14J15', groups=MultiJetGroup),

        ChainProp(name='HLT_6j25_pf_ftf_0eta240_L14J15',        groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_010jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_020jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_050jvt_L14J15', groups=MultiJetGroup),

        ChainProp(name='HLT_6j35_pf_ftf_0eta240_L14J15',        groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_010jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_020jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_050jvt_L14J15', groups=MultiJetGroup),

        ChainProp(name='HLT_6j45_pf_ftf_0eta240_L14J15',        groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_010jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_020jvt_L14J15', groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_050jvt_L14J15', groups=MultiJetGroup),

        # HH4b
        # 4J15 seed temporarily for rate estimates
        ChainProp(name='HLT_j80_ftf_0eta240_020jvt_j55_ftf_0eta240_020jvt_j28_ftf_0eta240_020jvt_j20_ftf_0eta240_020jvt_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        ChainProp(name='HLT_j80_ftf_0eta240_j55_ftf_0eta240_j28_ftf_0eta240_j20_ftf_0eta240_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        # PFlow variants
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j55_pf_ftf_0eta240_j28_pf_ftf_0eta240_j20_pf_ftf_0eta240_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        # Mu-seeded
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_L1MU20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j55_pf_ftf_0eta240_j28_pf_ftf_0eta240_j20_pf_ftf_0eta240_L1MU20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_L1MU10_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j55_pf_ftf_0eta240_j28_pf_ftf_0eta240_j20_pf_ftf_0eta240_L1MU10_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),

        # ATR-22594
        ChainProp(name='HLT_2j250_ftf_0eta240_j120_ftf_0eta240_L1J100',       l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_2j250_pf_ftf_0eta240_j120_pf_ftf_0eta240_L1J100', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_j150_ftf_2j55_ftf_L1J85_3J30',                    l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),
        # SC L1 seed
        # Note: l1SeedThresholds needs to be given explicitly, as it cannot
        # be auto-filled by DictFromChainName for L1Topo items
        ChainProp(name='HLT_j460_a10r_L1SC111-CJ15',                   l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10_lcw_subjes_L1SC111-CJ15',         l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10t_lcw_jes_L1SC111-CJ15',           l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j420_a10t_lcw_jes_35smcINF_L1SC111-CJ15',  l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_jes_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j360_a10t_lcw_jes_60smcINF_j360_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_j370_a10t_lcw_jes_35smcINF_j370_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiJetGroup),
        # Jet-only VBF chain
        ChainProp(name="HLT_j110_subjesgscIS_ftf_j45_subjesgscIS_ftf_L1J50", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_j70_0eta320_j50_0eta490_j0_DJMASS1000j50dphi200x400deta_L1MJJ-500-NFF', l1SeedThresholds=['FSNOSEED']*3,stream=['VBFDelayed'],groups=PrimaryLegGroup+MultiJetGroup) # previously HLT_j70_j50_0eta490_invm1000j70_dphi20_deta40_L1MJJ-500-NFF

    ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        # pflow options
        ChainProp(name='HLT_j275_pf_ftf_bdl1r60_split_L1J100',  groups=PrimaryLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j300_pf_ftf_bdl1r70_split_L1J100',  groups=PrimaryLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j360_pf_ftf_bdl1r77_split_L1J100',  groups=PrimaryLegGroup+SingleBjetGroup),

        ChainProp(name='HLT_j45_pf_ftf_bdl1r70_split_L1J20',    groups=SingleBjetGroup),


        # ATR-22937

        # unclear which of these can go in the final menu.
        # for now keep all to assess mistag rate.
        ChainProp(name="HLT_3j65_pf_ftf_bdl1r60_split_L1J20",               groups=MultiBjetGroup),
        ChainProp(name="HLT_3j65_pf_ftf_bdl1r70_split_L1J20",               groups=MultiBjetGroup),
        ChainProp(name="HLT_3j65_pf_ftf_bdl1r77_split_L1J20",               groups=MultiBjetGroup),
        ChainProp(name="HLT_3j65_pf_ftf_bdl1r85_split_L1J20",               groups=MultiBjetGroup),

        ChainProp(name="HLT_4j35_pf_ftf_bdl1r60_split_L1J20",               groups=MultiBjetGroup),
        ChainProp(name="HLT_4j35_pf_ftf_bdl1r70_split_L1J20",               groups=MultiBjetGroup),
        ChainProp(name="HLT_4j35_pf_ftf_bdl1r77_split_L1J20",               groups=MultiBjetGroup),
        ChainProp(name="HLT_4j35_pf_ftf_bdl1r85_split_L1J20",               groups=MultiBjetGroup),


        ChainProp(name="HLT_j150_pf_ftf_0eta320_2j55_pf_ftf_bdl1r70_split_L1J85_3J30", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

        ChainProp(name="HLT_3j35_pf_ftf_bdl1r70_split_j35_pf_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

        ChainProp(name="HLT_j175_pf_ftf_bdl1r60_split_j60_pf_ftf_bdl1r60_L1J100", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_2j35_pf_ftf_bdl1r70_split_2j35_pf_ftf_bdl1r85_split_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

        ChainProp(name="HLT_2j55_pf_ftf_bdl1r60_split_2j55_pf_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_2j35_pf_ftf_bdl1r60_split_3j35_pf_ftf_0eta320_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_2j45_pf_ftf_bdl1r60_split_3j45_pf_ftf_0eta320_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),


        # dl1r WP does not exist -> use tightest available threshold (60% WP)
        # the commented chains are left to document the source of the
        # new chains
        #ChainProp(name="HLT_j75_pf_ftf_bdl1r40_split_3j75_pf_ftf_L14J20", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        #ChainProp(name="HLT_j85_pf_ftf_bdl1r50_split_3j85_pf_ftf_L14J20", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

        ChainProp(name="HLT_j75_pf_ftf_bdl1r60_split_3j75_pf_ftf_L14J20", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

        #ChainProp(name="HLT_2j45_pf_ftf_bdl1r50_split_2j45_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

        ChainProp(name="HLT_2j45_pf_ftf_bdl1r60_split_2j45_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),


        # Test chains for ttbar and HH->4b chains
        # Adding non-final b-tag selection
        # 4J15 for comparison
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_bdl1r60_j20_pf_ftf_0eta240_020jvt_bdl1r60_L14J15', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r70_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L14J15', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        # Intended L1 seeds -- 3b70 for Physics_Main, 2b60 for Delayed
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r70_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_bdl1r60_j20_pf_ftf_0eta240_020jvt_bdl1r60_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r70_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1MU20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_bdl1r60_j20_pf_ftf_0eta240_020jvt_bdl1r60_L1MU20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r70_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1MU10_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_bdl1r60_j20_pf_ftf_0eta240_020jvt_bdl1r60_L1MU10_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        # Swap legs for b-tags pending b counting ComboHypo
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_bdl1r70_j55_pf_ftf_0eta240_020jvt_bdl1r70_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r60_j28_pf_ftf_0eta240_020jvt_bdl1r60_j20_pf_ftf_0eta240_020jvt_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_bdl1r60_j55_pf_ftf_0eta240_020jvt_bdl1r60_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        # Looser b-tag working points -- 2b70, 3b77, 3b85
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r77_j28_pf_ftf_0eta240_020jvt_bdl1r77_j20_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r85_j28_pf_ftf_0eta240_020jvt_bdl1r85_j20_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1MU20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r77_j28_pf_ftf_0eta240_020jvt_bdl1r77_j20_pf_ftf_0eta240_020jvt_bdl1r77_L1MU20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r85_j28_pf_ftf_0eta240_020jvt_bdl1r85_j20_pf_ftf_0eta240_020jvt_bdl1r85_L1MU20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1MU10_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r77_j28_pf_ftf_0eta240_020jvt_bdl1r77_j20_pf_ftf_0eta240_020jvt_bdl1r77_L1MU10_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_bdl1r85_j28_pf_ftf_0eta240_020jvt_bdl1r85_j20_pf_ftf_0eta240_020jvt_bdl1r85_L1MU10_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),

        # Boffperf to facilitate emulation studies
        ChainProp(name='HLT_j80_pf_ftf_j55_pf_ftf_j28_pf_ftf_j20_pf_ftf_boffperf_split_L14J15', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),

        # Tests of potential TLA chains for cost/rate
        # ATR-23002 - b-jets
        ChainProp(name='HLT_j20_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j100_pf_ftf_0eta320_j20_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_4j20_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name='HLT_4j20_pf_ftf_010jvt_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name='HLT_3j20_pf_ftf_010jvt_j20_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED']*2, groups=MultiBjetGroup),
        ChainProp(name='HLT_4j20_pf_ftf_010jvt_boffperf_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),

        # Chains for b-jet trigger training
        ChainProp(name='HLT_j20_pf_ftf_boffperf_L1J15', groups=SingleBjetGroup),

        # EMTopo Chains (likely not used)
        # ATR-22165
        ChainProp(name='HLT_j275_subjesgscIS_ftf_bdl1r60_split_L1J100',     groups=SingleBjetGroup),
        ChainProp(name='HLT_j300_subjesgscIS_ftf_bdl1r70_split_L1J100',     groups=SingleBjetGroup),
        ChainProp(name='HLT_j360_subjesgscIS_ftf_bdl1r77_split_L1J100',     groups=SingleBjetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_bdl1r70_split_L1J20',       groups=SingleBjetGroup),

        ChainProp(name="HLT_j110_subjesgscIS_ftf_bdl1r60_split_j45_subjesgscIS_ftf_bdl1r70_split_L1J50", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),


        # MV2c10 chains (not to be used)
        ChainProp(name="HLT_j45_subjesgscIS_ftf_bmv2c1070_split_L1J20",     groups=SingleBjetGroup),
        ChainProp(name="HLT_3j65_ftf_bmv2c1077_split_L1J20",                groups=MultiBjetGroup),
        ChainProp(name="HLT_4j35_ftf_bmv2c1077_split_L1J20",                groups=MultiBjetGroup),

        ChainProp(name="HLT_j150_ftf_0eta320_2j55_ftf_bmv2c1070_split_L1J85_3J30", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

        ChainProp(name="HLT_j75_ftf_bmv2c1040_split_3j75_ftf_0eta320_L14J20", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_3j35_ftf_bmv2c1070_split_j35_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

        ChainProp(name="HLT_j175_ftf_bmv2c1060_split_j60_ftf_bmv2c1060_L1J100", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_2j35_ftf_bmv2c1070_split_2j35_ftf_bmv2c1085_split_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

        ChainProp(name="HLT_j85_ftf_bmv2c1050_split_3j85_ftf_0eta320_L14J20", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_2j45_ftf_bmv2c1050_split_2j45_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_2j55_ftf_bmv2c1060_split_2j55_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_2j35_ftf_bmv2c1060_split_3j35_ftf_0eta320_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_2j45_ftf_bmv2c1060_split_3j45_ftf_0eta320_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),


        # Candidates for allhad ttbar delayed stream
        ChainProp(name='HLT_5j25_pf_ftf_0eta240_010jvt_j25_pf_ftf_0eta240_010jvt_bdl1r60_split_L14J15', groups=MultiBjetGroup),
        ChainProp(name='HLT_5j35_pf_ftf_0eta240_010jvt_j25_pf_ftf_0eta240_010jvt_bdl1r60_split_L14J15', groups=MultiBjetGroup),
        ChainProp(name='HLT_5j45_pf_ftf_0eta240_010jvt_j25_pf_ftf_0eta240_010jvt_bdl1r60_split_L14J15', groups=MultiBjetGroup),
        # Boffperf copy to facilitate emulation studies
        ChainProp(name='HLT_5j25_pf_ftf_0eta240_j25_pf_ftf_0eta240_boffperf_split_L14J15', groups=MultiBjetGroup),
        
        ChainProp(name="HLT_j55_ftf_0eta320_bmv2c1070_2j45_ftf_320eta490_L1J40p0ETA25_2J25_J20p31ETA49",l1SeedThresholds=['FSNOSEED','FSNOSEED'],  stream=[PhysicsStream], groups=MultiBjetGroup),
        ChainProp(name="HLT_j80_ftf_0eta320_bmv2c1070_j60_ftf_0eta320_bmv2c1085_j45_ftf_320eta490_320eta490_L1J40p0ETA25_2J25_J20p31ETA49", l1SeedThresholds=['FSNOSEED']*3,stream=[PhysicsStream], groups=MultiBjetGroup),
        ChainProp(name="HLT_j55_pf_ftf_0eta320_bdl1r70_2j45_pf_ftf_320eta490_L1J40p0ETA25_2J25_J20p31ETA49",l1SeedThresholds=['FSNOSEED','FSNOSEED'],  stream=[PhysicsStream], groups=MultiBjetGroup),
        ChainProp(name="HLT_j80_pf_ftf_0eta320_bdl1r70_j60_pf_ftf_0eta320_bdl1r85_j45_pf_ftf_320eta490_L1J40p0ETA25_2J25_J20p31ETA49", l1SeedThresholds=['FSNOSEED']*3,stream=[PhysicsStream], groups=MultiBjetGroup),

    ]

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        #ATR-20049
        ChainProp(name="HLT_tau0_ptonly_L1TAU8", groups=SingleTauGroup),
        ChainProp(name="HLT_tau0_ptonly_L1TAU60", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_ptonly_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_track_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_tracktwoEF_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_perf_track_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_perf_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_perf_tracktwoEF_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_perf_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_perf_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_looseRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_looseRNN_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_looseRNN_tracktwoLLP_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_mediumRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_mediumRNN_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_mediumRNN_tracktwoLLP_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_tightRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_tightRNN_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_tightRNN_tracktwoLLP_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_medium1_track_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_medium1_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_medium1_tracktwoEF_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_ptonly_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_idperf_track_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_idperf_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_idperf_tracktwoEF_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_idperf_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_idperf_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_perf_track_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_perf_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_perf_tracktwoEF_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_perf_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_perf_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_looseRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_looseRNN_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_tightRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_tightRNN_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_ptonly_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_track_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_tracktwo_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_tracktwoEF_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_tracktwoMVA_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_tracktwoMVABDT_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_track_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_tracktwo_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_tracktwoEF_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_tracktwoMVA_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_tracktwoMVABDT_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_medium1_track_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_medium1_tracktwo_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_medium1_tracktwoEF_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_looseRNN_tracktwoLLP_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_mediumRNN_tracktwoLLP_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_tightRNN_tracktwoLLP_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau200_ptonly_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau200_medium1_track_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau200_medium1_tracktwo_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau200_medium1_tracktwoEF_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau200_looseRNN_tracktwoLLP_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau200_mediumRNN_tracktwoLLP_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau200_tightRNN_tracktwoLLP_L1TAU100", groups=SingleTauGroup),

    ]

    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
        #ATR-20603
        ChainProp(name='HLT_2mu4_bJpsimumu_L12MU4', groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_bUpsimumu_L12MU4', groups=BphysicsGroup),
        #ATR-20839
        ChainProp(name='HLT_2mu4_bDimu_L12MU4', groups=BphysicsGroup),
        #ATR-21639
        ChainProp(name='HLT_2mu4_bBmumux_BpmumuKp_L12MU4', groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BcmumuPi_L12MU4', groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BsmumuPhi_L12MU4', groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BdmumuKst_L12MU4', groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_bBmumux_LbPqKm_L12MU4', groups=BphysicsGroup),
        #ATR-22223
        ChainProp(name='HLT_3mu4_bJpsi_L13MU4', groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_3mu4_bUpsi_L13MU4', groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_3mu4_bTau_L13MU4', groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_3mu4_bPhi_L13MU4', groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_3mu6_bDimu_L13MU6', groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_mu6_2mu4_bDimu2700_L1MU6_3MU4', groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_mu6_2mu4_bDimu6000_L1MU6_3MU4', groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_4mu4_bDimu6000_L14MU4', groups=PrimaryL1MuGroup+BphysicsGroup),
        #ATR-21003
        ChainProp(name='HLT_2mu6_bJpsimumul2io_L12MU6', groups=BphysicsGroup),
        ChainProp(name='HLT_2mu6_bJpsimumu_L12MU6',     groups=BphysicsGroup),
    ]

    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        # groups need to be properly assigned here later
        # Primary e-mu chains
        ChainProp(name='HLT_e17_lhloose_mu14_L1EM15VH_MU10', l1SeedThresholds=['EM15VH','MU10'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e7_lhmedium_mu24_L1MU20',l1SeedThresholds=['EM3','MU20'],  stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e12_lhloose_2mu10_L12MU10', l1SeedThresholds=['EM8VH','MU10'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_2e12_lhloose_mu10_L12EM8VH_MU10', l1SeedThresholds=['EM8VH','MU10'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        # Primary g-mu chains
        ChainProp(name='HLT_g25_medium_mu24_L1MU20',l1SeedThresholds=['EM15VH','MU20'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup), #ATR-22594
        ChainProp(name='HLT_g35_loose_mu18_L1EM22VHI', l1SeedThresholds=['EM22VHI','MU10'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_loose_mu18_L1EM24VHI', l1SeedThresholds=['EM24VHI','MU10'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_2g10_loose_mu20_L1MU20', l1SeedThresholds=['EM7','MU20'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup), # unsure what EM seed should be
        #LLP
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L12MU10', l1SeedThresholds=['EM8VH','MU10'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        # Test chains for muon + jet/MET merging/aligning
        ChainProp(name='HLT_mu6_xe30_mht_L1XE30', l1SeedThresholds=['MU6','FSNOSEED'], stream=[PhysicsStream], groups=MuonMETGroup),
        ChainProp(name='HLT_mu6_j45_nojcalib_L1J20', l1SeedThresholds=['MU6','FSNOSEED'], stream=[PhysicsStream], groups=MuonJetGroup),

        #ATR-22107
        ChainProp(name='HLT_e24_lhmedium_mu8noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], stream=[PhysicsStream], groups=EgammaMuonGroup),
        ChainProp(name='HLT_e26_lhmedium_mu8noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e28_lhmedium_mu8noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e9_lhvloose_mu20_mu8noL1_L1MU20', l1SeedThresholds=['EM3','MU20','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_loose_mu15_mu2noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','MU6','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_loose_mu15_mu2noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','MU6','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_tight_icalotight_mu18noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_tight_icalotight_mu18noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_tight_icalotight_mu15noL1_mu2noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_tight_icalotight_mu15noL1_mu2noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        # tau+X chains (ATR-21609) 
        # TODO: need to add dRtt to chain below. How do we avoid overlap between taus and jets?
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVA_tau20_mediumRNN_tracktwoMVA_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['TAU8','TAU8','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+TauJetGroup),
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVABDT_tau20_mediumRNN_tracktwoMVABDT_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['TAU8','TAU8','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+TauJetGroup),
        # displaced tau+X (ATR-21754)
        ChainProp(name="HLT_tau80_mediumRNN_tracktwoLLP_tau60_mediumRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40",                  l1SeedThresholds=['TAU60','TAU40'],     groups=TauJetGroup),
        
        # photon + multijets (ATR-22594)
        ChainProp(name='HLT_g85_tight_3j50_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_g45_loose_6j45_L14J15p0ETA25',l1SeedThresholds=['EM15','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaJetGroup),

        # photon + MET (ATR-22594)
        ChainProp(name='HLT_g90_loose_xe90_cell_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED'],stream=[PhysicsStream], groups=EgammaMETGroup),

        # electron + MET (ATR-22594)
        ChainProp(name='HLT_e70_lhloose_xe70_cell_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED'],stream=[PhysicsStream], groups=EgammaMETGroup),

        # VBF triggers (ATR-22594)
        ChainProp(name='HLT_2mu6_2j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['MU6','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+MuonJetGroup), # Formerly HLT_2mu6_2j50_0eta490_invm900j50
        ChainProp(name='HLT_e5_lhvloose_j70_0eta320_j50_0eta490_j0_DJMASS1000j50_xe50_tcpufit_L1MJJ-500-NFF',l1SeedThresholds=['EM3','FSNOSEED','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_2e5_lhmedium_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['EM3','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_g25_medium_4j35_0eta490_j0_DJMASS1000j35_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_e10_lhmedium_ivarloose_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['EM8VH','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_mu4_j70_0eta320_j50_0eta490_j0_DJMASS1000j50_L1MJJ-500-NFF',l1SeedThresholds=['MU4','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+MuonJetGroup),
        ChainProp(name='HLT_mu10_ivarmedium_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['MU10','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+MuonJetGroup),
        ChainProp(name='HLT_j70_0eta320_j50_0eta490_j0_DJMASS1000j50dphi240_xe90_tcpufit_xe50_cell_L1MJJ-500-NFF',l1SeedThresholds=['FSNOSEED']*5,stream=['VBFDelayed'], groups=PrimaryLegGroup+JetMETGroup),

        # meson + photon (ATR-22644) - note: each of these chains requires a photon+tau cut that will be included in a later update
        ChainProp(name='HLT_g25_medium_tau25_dikaonmass_tracktwoMVA_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+['RATE:TauGamma', 'BW:Tau', 'BW:Egamma']),
        ChainProp(name='HLT_g25_medium_tau25_kaonpi1_tracktwoMVA_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+['RATE:TauGamma', 'BW:Tau', 'BW:Egamma']),
        ChainProp(name='HLT_g25_medium_tau25_kaonpi2_tracktwoMVA_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+['RATE:TauGamma', 'BW:Tau', 'BW:Egamma']),
        ChainProp(name='HLT_g25_medium_tau25_singlepion_tracktwoMVA_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+['RATE:TauGamma', 'BW:Tau', 'BW:Egamma']),
        ChainProp(name='HLT_g35_medium_tau25_dipion3_tracktwoMVA_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+['RATE:TauGamma', 'BW:Tau', 'BW:Egamma']),

        #Combined BPhys
        #ATR-22749 Added on May 25
        ChainProp(name='HLT_e9_lhvloose_e5_lhvloose_bBeeM6000_mu6_noL2Comb_L1BPH-0M9-EM7-EM5_MU6', l1SeedThresholds=['EM7','EM5','MU6'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e9_lhvloose_e5_lhvloose_bBeeM6000_2mu4_noL2Comb_L1BPH-0M9-EM7-EM5_2MU4', l1SeedThresholds=['EM7','EM5','MU4'], groups=BphysElectronGroup),

        # Tests of potential TLA chains for cost/rate
        # ATR-19317 - dijet+ISR 
        ChainProp(name='HLT_g35_loose_3j25_pf_ftf_L1EM22VHI',          l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),
        ChainProp(name='HLT_g35_medium_3j25_pf_ftf_L1EM22VHI',         l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),
        ChainProp(name='HLT_g35_tight_3j25_pf_ftf_L1EM22VHI',          l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),
        ChainProp(name='HLT_g35_tight_3j25_pf_ftf_boffperf_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),

        # Tests for b-jet trigger calibration chains
        ChainProp(name='HLT_e26_lhtight_ivarloose_2j20_pf_ftf_boffperf_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaBjetGroup),
        ChainProp(name='HLT_mu26_ivarmedium_2j20_pf_ftf_boffperf_L1MU20', l1SeedThresholds=['MU20','FSNOSEED'], groups=MuonBjetGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_mu22noL1_2j20_pf_ftf_boffperf_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'], groups=EgammaBjetGroup),

    ]
    TriggerFlags.HeavyIonSlice.signatures  = TriggerFlags.HeavyIonSlice.signatures() + []
    TriggerFlags.BeamspotSlice.signatures  = TriggerFlags.BeamspotSlice.signatures() + [
        ChainProp(name='HLT_beamspot_allTE_trkfast_BeamSpotPEB_L1J15',  l1SeedThresholds=['FSNOSEED'], stream=['BeamSpot'], groups=['PS:Online', 'RATE:BeamSpot',  'BW:BeamSpot']),
    ]
    TriggerFlags.MinBiasSlice.signatures   = TriggerFlags.MinBiasSlice.signatures() + [
        ChainProp(name="HLT_mb_mbts_L1MBTS_1_EMPTY",               l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name="HLT_mb_mbts_L1MBTS_1",                     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name="HLT_mb_mbts_L1MBTS_1_1",                   l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name="HLT_mb_mbts_L1MBTS_2",                     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name="HLT_mb_mbts_L1RD0_FILLED",                 l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name="HLT_mb_mbts_L1RD0_EMPTY",                  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_sp400_trk40_hmt_L1RD0_FILLED',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_sp600_trk45_hmt_L1RD0_FILLED',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_sp1200_trk60_hmt_L1RD0_FILLED',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_sp1400_trk80_hmt_L1RD0_FILLED',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_sp2000_trk100_hmt_L1RD0_FILLED',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_sp3000_trk100_hmt_L1RD0_FILLED',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_sp5000_trk80_hmt_L1RD0_FILLED',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_sp5000_trk100_hmt_L1RD0_FILLED',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_sp5000_trk200_hmt_L1RD0_FILLED',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_afprec_2j20_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED','FSNOSEED'], stream=[PhysicsStream],groups=MinBiasGroup),
        ChainProp(name='HLT_mb_sp400_pusup300_trk40_hmt_L1RD0_FILLED',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']),
    ]
    TriggerFlags.CalibSlice.signatures     =    TriggerFlags.CalibSlice.signatures() + [
        ChainProp(name='HLT_noalg_AlfaPEB_L1ALFA_ANY', l1SeedThresholds=['FSNOSEED'], stream=['ALFACalib'], groups=['RATE:ALFACalibration','BW:Detector']),
    ]
    TriggerFlags.CosmicSlice.signatures    = TriggerFlags.CosmicSlice.signatures() + [
    ]

    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
        #ChainProp(name='HLT_noalg_mb_L1RD2_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup),
        #ChainProp(name='HLT_noalg_zb_L1ZB', l1SeedThresholds=['FSNOSEED'], stream=['ZeroBias'], groups=ZeroBiasGroup),
        ChainProp(name='HLT_noalg_L1All', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['RATE:SeededStreamers', 'BW:Other']), # ATR-22072, for rates in MC. To move to MC menu once good nightly in LS2_v1.
    ]

    TriggerFlags.MonitorSlice.signatures   = TriggerFlags.MonitorSlice.signatures() + [
       ChainProp(name='HLT_noalg_CSCPEB_L1All', l1SeedThresholds=['FSNOSEED'], stream=['CSC'], groups=['RATE:Monitoring','BW:Other']),
    ]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = TriggerFlags.EnhancedBiasSlice.signatures() + [
        ChainProp(name='HLT_eb_low_L1RD2_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_eb_medium_L1RD2_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),

        ChainProp(name='HLT_noalg_eb_L1PhysicsHigh_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_eb_L1PhysicsVeryHigh_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),

        ChainProp(name='HLT_noalg_eb_L1RD3_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_eb_L1RD3_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),

        ChainProp(name='HLT_noalg_eb_L1EMPTY_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_eb_L1FIRSTEMPTY_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_eb_L1UNPAIRED_ISO_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_eb_L1UNPAIRED_NONISO_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_eb_L1ABORTGAPNOTCALIB_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] )
    ]


    TriggerFlags.UnconventionalTrackingSlice.signatures = TriggerFlags.UnconventionalTrackingSlice.signatures() + [

        #These chains are temproray to get final rates, they will be reduced to a few chains in the future
        ChainProp(name='HLT_unconvtrk50_isohpttrack_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_tight_imedium_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_medium_imedium_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_loose_imedium_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_tight_iaggrmedium_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_medium_iaggrmedium_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_loose_iaggrmedium_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_tight_iloose_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_medium_iloose_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_loose_iloose_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_tight_iaggrloose_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_medium_iaggrloose_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_loose_iaggrloose_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),


        ChainProp(name='HLT_unconvtrk0_fslrt_L1All', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk0_fslrt_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED']),

        # dEdx triggers
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk25_dedx_medium_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk50_dedx_medium_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED','FSNOSEED']),
    ]
