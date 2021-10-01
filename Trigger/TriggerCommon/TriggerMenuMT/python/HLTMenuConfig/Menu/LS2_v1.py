# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# LS2_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],

import TriggerMenuMT.HLTMenuConfig.Menu.MC_pp_run3_v1 as mc_menu
import TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_pp_run3_v1 as p1_menu
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp
# this is not the best option, due to flake violation, this list has to be changed when some groups are removed
from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import (PhysicsStream,
                                                                 SingleMuonGroup,
                                                                 MultiMuonGroup,
                                                                 SingleElectronGroup,
                                                                 MultiElectronGroup,
                                                                 SinglePhotonGroup,
                                                                 MultiPhotonGroup,
                                                                 SingleMETGroup,
                                                                 MultiMETGroup,
                                                                 SingleJetGroup,
                                                                 MultiJetGroup,
                                                                 SingleBjetGroup,
                                                                 MultiBjetGroup,
                                                                 BjetMETGroup,
                                                                 SingleTauGroup,
                                                                 MultiTauGroup,
                                                                 BphysicsGroup,
                                                                 BphysElectronGroup,
                                                                 EgammaMuonGroup,
                                                                 EgammaMETGroup,
                                                                 EgammaBjetGroup,
                                                                 EgammaTauGroup,
                                                                 MuonJetGroup,
                                                                 TauMETGroup,
                                                                 TauJetGroup,
                                                                 MuonMETGroup,
                                                                 EgammaJetGroup,
                                                                 JetMETGroup,
                                                                 MinBiasGroup,
                                                                 PrimaryL1MuGroup,
                                                                 PrimaryLegGroup,
                                                                 PrimaryPhIGroup,
                                                                 SupportLegGroup,
                                                                 SupportPhIGroup,
                                                                 TagAndProbeGroup,
                                                                 TagAndProbeLegGroup,
                                                                 LowMuGroup,
                                                                 EOFBPhysL1MuGroup,
                                                                 EOFTLALegGroup,
                                                                 UnconvTrkGroup,
                                                                 TauPhaseIStreamersGroup,
                                                                 EgammaPhaseIStreamersGroup,  
                                                                 JetPhaseIStreamersGroup,
                                                                 METPhaseIStreamersGroup,
                                                                 )

def setupMenu():

    mc_menu.setupMenu()
    p1_menu.addP1Signatures()

    from AthenaCommon.Logging import logging
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    log = logging.getLogger( __name__ )
    log.info('[setupMenu] going to add the Dev menu chains now')

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + []


    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        #ATR-19985
        ChainProp(name='HLT_mu6_mu6noL1_L1MU5VF', l1SeedThresholds=['MU5VF','FSNOSEED'], groups=MultiMuonGroup),

        #test chains
        ChainProp(name='HLT_mu6_L1MU5VF',     groups=SingleMuonGroup),
        ChainProp(name="HLT_noalg_L1MU5VF",   l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup+LowMuGroup),

        ChainProp(name='HLT_mu6_LRT_idperf_L1MU5VF',           groups=SingleMuonGroup),
        ChainProp(name='HLT_mu24_LRT_d0medium_L1MU14FCH',     groups=SingleMuonGroup),
        
        ChainProp(name='HLT_mu6_ivarmedium_L1MU5VF', groups=SingleMuonGroup),

        # commented because it is conflict with dimuon noL1 serial chain
        # ChainProp(name='HLT_mu6noL1_L1MU5VF', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup),

        ChainProp(name='HLT_mu6_msonly_L1MU5VF',     groups=SingleMuonGroup, monGroups=['muonMon:shifter','muonMon:val','idMon:t0']),

        ChainProp(name='HLT_2mu6_10invmAA70_L1MU5VF', groups=SingleMuonGroup),

        # LATE
        ChainProp(name='HLT_mu10_lateMu_L1LATE-MU8F_J50', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu10_lateMu_L1LATE-MU8F_XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup),

        # ATR-20049
        ChainProp(name='HLT_mu6_mu4_L12MU3V',  l1SeedThresholds=['MU3V']*2, groups=MultiMuonGroup),

        # Additional intermediate thresholds for validation comparisons to match with Physics_pp_v7_primaries menu
        ChainProp(name='HLT_mu24_mu10noL1_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=MultiMuonGroup),
        ChainProp(name="HLT_mu10_L1MU8F", groups=SingleMuonGroup),
        ChainProp(name='HLT_2mu4_L12MU3V',  groups=MultiMuonGroup),

        # ATR-19452
        ChainProp(name='HLT_2mu4_muonqual_L12MU3V',  groups=MultiMuonGroup),
        ChainProp(name='HLT_2mu6_muonqual_L12MU5VF',  groups=MultiMuonGroup),

        # ATR-20650
        ChainProp(name='HLT_mu0_muoncalib_L1MU3V_EMPTY', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu0_muoncalib_L1MU14FCH',      groups=SingleMuonGroup),

        # Unisolated single muons for monitoring isolated muons
        ChainProp(name="HLT_mu8_L1MU5VF", groups=SingleMuonGroup),
        ChainProp(name="HLT_mu14_L1MU8F", groups=SingleMuonGroup),
        ChainProp(name="HLT_mu24_L1MU14FCH", groups=SingleMuonGroup),

        #ATR-21003
        ChainProp(name='HLT_mu4_l2io_L1MU3V', groups=SingleMuonGroup),
        ChainProp(name='HLT_2mu14_l2io_L12MU8F', groups=MultiMuonGroup),
        ChainProp(name='HLT_2mu6_l2io_L12MU5VF',     l1SeedThresholds=['MU5VF'],   groups=MultiMuonGroup),

        #ATR-22537
        ChainProp(name='HLT_2mu10_l2mt_L1MU8F', groups=MultiMuonGroup),
        ChainProp(name='HLT_2mu10_l2mt_L1MU10BOM', groups=MultiMuonGroup),

        #performance chain for isolation (ATR-21905)
        ChainProp(name='HLT_mu26_ivarperf_L1MU14FCH', groups=SingleMuonGroup),

        #-- nscan ATR-19376
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_J40', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryLegGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_XE30', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryLegGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L110DR-MU14FCH-MU5VF', l1SeedThresholds=['MU14FCH','FSNOSEED'],   groups=PrimaryL1MuGroup+MultiMuonGroup),

        #ATR-20505
        ChainProp(name='HLT_2mu50_msonly_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),   

        #ATR-23614 
        ChainProp(name='HLT_mu20_mu2noL1_invmJPsi_os_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=MultiMuonGroup),

        #ART-23577
        ChainProp(name='HLT_mu20_L1MU5VF_AFP_A_OR_C',     l1SeedThresholds=['MU5VF'],   groups=SingleMuonGroup+LowMuGroup),
        ChainProp(name='HLT_mu20_L1MU5VF_AFP_A_AND_C',     l1SeedThresholds=['MU5VF'],   groups=SingleMuonGroup+LowMuGroup),
        ChainProp(name='HLT_mu20_L1MU5VF',     l1SeedThresholds=['MU5VF'],   groups=SingleMuonGroup+LowMuGroup),

        # Late stream for LLP
        ChainProp(name='HLT_3mu6_msonly_L1MU5VF_EMPTY', l1SeedThresholds=['MU5VF'], stream=['Late'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_3mu6_msonly_L1MU3V_UNPAIRED_ISO', l1SeedThresholds=['MU3V'], stream=['Late'], groups=PrimaryL1MuGroup+MultiMuonGroup),

        # Test T&P dimuon
        ChainProp(name='HLT_mu24_mu6_L1MU14FCH', l1SeedThresholds=['MU14FCH','MU3V'], groups=MultiMuonGroup),
        ChainProp(name='HLT_mu24_mu6_probe_L1MU14FCH', l1SeedThresholds=['MU14FCH','PROBEMU3V'], groups=MultiMuonGroup),
        ChainProp(name='HLT_mu24_ivarmedium_mu6_L1MU14FCH', l1SeedThresholds=['MU14FCH','MU3V'], groups=MultiMuonGroup),
        ChainProp(name='HLT_mu24_ivarmedium_mu6_ivarmedium_L1MU14FCH', l1SeedThresholds=['MU14FCH','MU3V'], groups=MultiMuonGroup),
        ChainProp(name='HLT_mu24_ivarmedium_mu6_ivarperf_L1MU14FCH', l1SeedThresholds=['MU14FCH','MU3V'], groups=MultiMuonGroup),
        ChainProp(name='HLT_mu24_ivarmedium_mu6_probe_L1MU14FCH', l1SeedThresholds=['MU14FCH','PROBEMU3V'], groups=MultiMuonGroup),
        ChainProp(name='HLT_mu24_ivarmedium_mu6_ivarmedium_probe_L1MU14FCH', l1SeedThresholds=['MU14FCH','PROBEMU3V'], groups=MultiMuonGroup),
        ChainProp(name='HLT_mu24_ivarmedium_mu6_ivarperf_probe_L1MU14FCH', l1SeedThresholds=['MU14FCH','PROBEMU3V'], groups=MultiMuonGroup),

        # 3mu inv mass (ATR-19355, ATR-19638), TODO: HLT invm to be added and moved to BLS signature
        ChainProp(name='HLT_3mu4_L1BPH-0M10-3MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_3mu4_L1BPH-0M10C-3MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),

        # ATR-22782
        ChainProp(name='HLT_2mu4_L1BPH-0M16-20DR99-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_L1BPH-0M16-15DR99-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_L1BPH-8M15-20DR99-C-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_L1BPH-8M15-20DR99-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_L1BPH-8M15-15DR99-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),

   ]

    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
        
        # ElectronChains----------
        # Phase1 eEM chains
        ChainProp(name='HLT_e5_etcut_L1eEM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_L1eEM22M', groups=PrimaryPhIGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_L1eEM22T', groups=PrimaryPhIGroup+SingleElectronGroup),
        ChainProp(name='HLT_e60_lhmedium_L1eEM22M', groups=PrimaryPhIGroup+SingleElectronGroup),
        ChainProp(name='HLT_e140_lhloose_L1eEM22M', groups=PrimaryPhIGroup+SingleElectronGroup),
        ChainProp(name='HLT_e300_etcut_L1eEM22M', groups=PrimaryPhIGroup+SingleElectronGroup),
        # More PhaseI eEM chains (ATR-24182)
        ChainProp(name='HLT_2e17_lhvloose_L12eEM15M', groups=PrimaryPhIGroup+MultiElectronGroup),
        ChainProp(name='HLT_2e24_lhvloose_L12eEM20L', groups=PrimaryPhIGroup+MultiElectronGroup),
        ChainProp(name='HLT_e24_lhvloose_2e12_lhvloose_L1eEM20L_3eEM10L',l1SeedThresholds=['eEM20L','eEM10L'], groups=PrimaryPhIGroup+MultiElectronGroup),
        ChainProp(name='HLT_e26_lhtight_e15_etcut_Zee_L1eEM22M', l1SeedThresholds=['eEM22M','eEM8'], groups=PrimaryPhIGroup+MultiElectronGroup),
        ChainProp(name='HLT_e26_lhtight_e15_etcut_idperf_Zee_L1eEM22M', l1SeedThresholds=['eEM22M','eEM8'], groups=PrimaryPhIGroup+MultiElectronGroup),
        ChainProp(name='HLT_g140_loose_L1eEM22M', groups=PrimaryPhIGroup+SinglePhotonGroup),
        ChainProp(name='HLT_g300_etcut_L1eEM22M', groups=PrimaryPhIGroup+SinglePhotonGroup),
        ChainProp(name='HLT_2g20_tight_icaloloose_L12eEM15M', groups=PrimaryPhIGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g22_tight_L12eEM15M', groups=PrimaryPhIGroup+MultiPhotonGroup),
        ChainProp(name='HLT_g35_medium_g25_medium_L12eEM20L', l1SeedThresholds=['eEM20L','eEM20L'], groups=PrimaryPhIGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g50_loose_L12eEM20L', groups=PrimaryPhIGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g25_loose_g15_loose_L12eEM20L', l1SeedThresholds=['eEM20L','eEM10L'], groups=PrimaryPhIGroup+MultiPhotonGroup),
        ChainProp(name='HLT_e24_lhmedium_g25_medium_02dRAB_L12eEM20L', l1SeedThresholds=['eEM20L','eEM20L'], groups=PrimaryPhIGroup+MultiElectronGroup),
        ChainProp(name='HLT_e24_lhmedium_g12_loose_g12_loose_02dRAB_02dRAC_L1eEM20L_3eEM10L', l1SeedThresholds=['eEM20L','eEM10L','eEM10L'], groups=PrimaryPhIGroup+MultiElectronGroup),
        ChainProp(name='HLT_e26_lhloose_nopix_lrttight_L1eEM22M', groups=PrimaryPhIGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhmedium_nopix_lrttight_L1eEM22M', groups=PrimaryPhIGroup+SingleElectronGroup),
        ChainProp(name='HLT_g35_loose_PhotonDS_L1eEM22M', stream=['PhotonDS'], groups=PrimaryPhIGroup+SinglePhotonGroup),
        ChainProp(name='HLT_e25_mergedtight_g35_medium_Heg_02dRAB_L12eEM20L', l1SeedThresholds=['eEM20L','eEM20L'], groups=PrimaryPhIGroup+MultiElectronGroup),
        
        
        # lrt chains
        ChainProp(name='HLT_e5_idperf_loose_lrtloose_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_idperf_loose_lrtloose_L1EM22VHI', groups=SingleElectronGroup),  

        # electron forward triggers (keep this only for dev now)
        #ChainProp(name='HLT_e30_etcut_fwd_L1EM22VHI', groups=SingleElectronGroup),

        #ATR-22749
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L12EM3', l1SeedThresholds=['EM3'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e9_lhvloose_e5_lhvloose_bBeeM6000_L1BPH-0M9-EM7-EM5', l1SeedThresholds=['EM7','EM3'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1BPH-0DR3-EM7J15', l1SeedThresholds=['EM7'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e9_lhvloose_bBeeM6000_L1BPH-0DR3-EM7J15', l1SeedThresholds=['EM7'], groups=BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L1EM22VHI', l1SeedThresholds=['EM3'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1EM22VHI', l1SeedThresholds=['EM3'], groups=BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L1BPH-0M9-EM7-EM5_MU5VF', l1SeedThresholds=['EM3'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1BPH-0DR3-EM7J15_MU5VF', l1SeedThresholds=['EM3'], groups=BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L14J15', l1SeedThresholds=['EM3'], groups=BphysElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L14J15', l1SeedThresholds=['EM3'], groups=BphysElectronGroup),

        #ART-23577
        ChainProp(name='HLT_e20_lhloose_L1EM7_AFP_A_OR_C', l1SeedThresholds=['EM7'], groups=SingleElectronGroup+LowMuGroup),
        ChainProp(name='HLT_e20_lhloose_L1EM7_AFP_A_AND_C', l1SeedThresholds=['EM7'], groups=SingleElectronGroup+LowMuGroup),
        ChainProp(name='HLT_e20_lhloose_L1EM7', l1SeedThresholds=['EM7'], groups=LowMuGroup),

        # Low eT photon chains for TLA
        ChainProp(name='HLT_g35_loose_L1EM22VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g35_medium_L1EM22VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g35_tight_L1EM22VHI', groups=SinglePhotonGroup),
        ChainProp(name='HLT_g35_tight_icaloloose_L1EM22VHI', groups=SinglePhotonGroup),

        # PhotonDS chains for TLA
        ChainProp(name='HLT_g35_loose_PhotonDS_L1EM22VHI',stream=['PhotonDS'], groups=PrimaryLegGroup+SinglePhotonGroup),
     
        #ATR-21882
        ChainProp(name='HLT_2g15_tight_dPhi25_m80_L1DPHI-M70-2eEM12M', l1SeedThresholds=['eEM10L'], groups=PrimaryPhIGroup+MultiPhotonGroup), # TODO: mismatch between L1topo threshold and L1 seed to be fixed

        #Support photon chains ATR-23425
        ChainProp(name='HLT_2g15_loose_dPhi25_m80_L1DPHI-M70-2eEM12M', l1SeedThresholds=['eEM10L'], groups=SupportPhIGroup+MultiPhotonGroup), # TODO: mismatch between L1topo threshold and L1 seed to be fixed
        ChainProp(name='HLT_2g20_loose_L12EM15VH', groups=SupportLegGroup+MultiPhotonGroup),
        # Copy with generic TrigComboHypoTool
        ChainProp(name='HLT_2g15_tight_25dphiAA_invmAA80_L1DPHI-M70-2eEM12M', l1SeedThresholds=['eEM10L'], groups=PrimaryPhIGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g15_loose_25dphiAA_invmAA80_L1DPHI-M70-2eEM12M', l1SeedThresholds=['eEM10L'], groups=SupportPhIGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g15_tight_25dphiAA_L1DPHI-M70-2eEM12M', l1SeedThresholds=['eEM10L'], groups=PrimaryPhIGroup+MultiPhotonGroup),
        
        #------------ 1e_1g HEG ATR-23158
        ChainProp(name='HLT_e25_mergedtight_g35_medium_Heg_02dRAB_L12EM20VH',l1SeedThresholds=['EM20VH','EM20VH'], groups=PrimaryLegGroup+MultiElectronGroup), 

        #------------ hipTRT trigger, ATR-22603
        ChainProp(name='HLT_g0_hiptrt_L1EM22VHI', groups=SinglePhotonGroup), 

        #------------ GSF triggers
        ChainProp(name='HLT_e26_gsf_lhtight_ivarloose_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e60_gsf_lhmedium_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_2e17_gsf_lhvloose_L12EM15VHI', groups=MultiElectronGroup),

        #------------ idperf triggers
        ChainProp(name='HLT_e26_idperf_tight_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e60_idperf_medium_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_2e17_idperf_loose_L12EM15VHI', groups=MultiElectronGroup),

        ChainProp(name='HLT_e5_idperf_gsf_tight_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_idperf_gsf_tight_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e60_idperf_gsf_medium_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_2e17_idperf_gsf_loose_L12EM15VHI', groups=MultiElectronGroup),

        # idperf for id validation, can some be removed?
        ChainProp(name='HLT_e5_idperf_loose_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_idperf_medium_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_idperf_tight_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_idperf_loose_L1EM24VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e28_idperf_loose_L1EM24VHI', groups=SingleElectronGroup),

        #------------ dnn chains
        ChainProp(name='HLT_e5_dnnloose_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_dnnmedium_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_dnntight_L1EM3', groups=SingleElectronGroup),

        ChainProp(name='HLT_e26_dnnloose_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_dnnmedium_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_dnntight_L1EM22VHI', groups=SingleElectronGroup),

        ChainProp(name='HLT_e60_dnnmedium_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e140_dnnloose_L1EM22VHI', groups=SingleElectronGroup),

        ChainProp(name='HLT_e26_gsf_dnntight_ivarloose_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e60_gsf_dnnmedium_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e140_gsf_dnnloose_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),

        #------------ support chains
        ChainProp(name='HLT_e26_lhtight_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarmedium_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivartight_L1EM22VHI', groups=SingleElectronGroup),

        

        # Generic test T&P chain
        ChainProp(name='HLT_e26_lhtight_ivarloose_e5_lhvloose_idperf_probe_L1EM22VHI',l1SeedThresholds=['EM22VHI','PROBEEM3'],groups=SingleElectronGroup),
        # T&P chains for displaced electrons
        ChainProp(name='HLT_e26_lhtight_ivarloose_e5_lhvloose_nopix_lrtloose_idperf_probe_L1EM22VHI',l1SeedThresholds=['EM22VHI','PROBEEM3'],groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e26_lhloose_nopix_lrttight_probe_L1EM22VHI',l1SeedThresholds=['EM22VHI','PROBEEM22VHI'],groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_nopix_lrtloose_idperf_probe_g25_medium_L1EM20VH',l1SeedThresholds=['PROBEEM3','EM20VH'],groups=TagAndProbeLegGroup+SinglePhotonGroup),
        ChainProp(name='HLT_e26_lhloose_nopix_lrttight_probe_g25_medium_L1EM20VH',l1SeedThresholds=['PROBEEM22VHI','EM20VH'],groups=TagAndProbeLegGroup+SinglePhotonGroup),
        
        #----------- egamma Tag&Probe
        ChainProp(name='HLT_e26_lhtight_ivarloose_e12_lhvloose_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM10VH'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e17_lhvloose_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM15VHI'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e24_lhvloose_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM20VHI'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e26_lhtight_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM22VHI'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e20_lhtight_ivarloose_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM15VHI'], groups=TagAndProbeLegGroup+SingleElectronGroup),

        ChainProp(name='HLT_e26_lhtight_ivarloose_e15_etcut_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM7'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e15_etcut_idperf_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM7'], groups=TagAndProbeLegGroup+SingleElectronGroup),

        ChainProp(name='HLT_e26_lhtight_ivarloose_e14_lhtight_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM7'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e14_lhtight_noringer_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM7'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e14_etcut_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM7'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e9_lhtight_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM3'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e9_lhtight_noringer_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM3'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e9_etcut_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM3'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e5_lhtight_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM3'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e5_lhtight_noringer_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM3'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_e4_etcut_probe_L1EM22VHI', l1SeedThresholds=['EM22VHI','PROBEEM3'], groups=TagAndProbeLegGroup+SingleElectronGroup),

        # Late stream for LLP
        ChainProp(name='HLT_g35_medium_g25_medium_L1EM7_EMPTY', l1SeedThresholds=['EM7']*2, stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_g35_medium_g25_medium_L1EM7_UNPAIRED_ISO', l1SeedThresholds=['EM7']*2, stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g22_tight_L1EM7_EMPTY', l1SeedThresholds=['EM7'], stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g22_tight_L1EM7_UNPAIRED_ISO', l1SeedThresholds=['EM7'], stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g50_tight_L1EM7_EMPTY', l1SeedThresholds=['EM7'], stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g50_tight_L1EM7_UNPAIRED_ISO', l1SeedThresholds=['EM7'], stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),

        # Alternative formulation of T&P chains with generic mass cut combohypotool
        # With & without 'probe' expression to check count consistency
        # Zee
        ChainProp(name='HLT_e26_lhtight_e15_etcut_probe_50invmAB130_L1EM22VHI', l1SeedThresholds=['EM22VHI','EM7'], groups=TagAndProbeLegGroup+SingleElectronGroup), 
        ChainProp(name='HLT_e26_lhtight_e15_etcut_50invmAB130_L1EM22VHI', l1SeedThresholds=['EM22VHI','EM7'], groups=MultiElectronGroup), 
        # Jpsiee
        ChainProp(name='HLT_e9_lhtight_e4_etcut_probe_1invmAB5_L1JPSI-1M5-EM7', l1SeedThresholds=['EM7','EM3'], groups=SupportLegGroup+MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_e9_etcut_probe_1invmAB5_L1JPSI-1M5-EM7', l1SeedThresholds=['EM3','EM7'], groups=SupportLegGroup+MultiElectronGroup),
        ChainProp(name='HLT_e14_lhtight_e4_etcut_probe_1invmAB5_L1JPSI-1M5-EM12', l1SeedThresholds=['EM12','EM3'], groups=SupportLegGroup+MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_e14_etcut_probe_1invmAB5_L1JPSI-1M5-EM12', l1SeedThresholds=['EM3','EM12'], groups=SupportLegGroup+MultiElectronGroup),
        ChainProp(name='HLT_e9_lhtight_noringer_e4_etcut_probe_1invmAB5_L1JPSI-1M5-EM7', l1SeedThresholds=['EM7','EM3'], groups=SupportLegGroup+MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_e9_etcut_probe_1invmAB5_L1JPSI-1M5-EM7', l1SeedThresholds=['EM3','EM7'], groups=SupportLegGroup+MultiElectronGroup),
        ChainProp(name='HLT_e14_lhtight_noringer_e4_etcut_probe_1invmAB5_L1JPSI-1M5-EM12', l1SeedThresholds=['EM12','EM3'], groups=SupportLegGroup+MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_e14_etcut_probe_1invmAB5_L1JPSI-1M5-EM12', l1SeedThresholds=['EM3','EM12'], groups=SupportLegGroup+MultiElectronGroup),
        #
        ChainProp(name='HLT_e9_lhtight_e4_etcut_1invmAB5_L1JPSI-1M5-EM7', l1SeedThresholds=['EM7','EM3'], groups=MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_e9_etcut_1invmAB5_L1JPSI-1M5-EM7', l1SeedThresholds=['EM3','EM7'], groups=MultiElectronGroup),
        ChainProp(name='HLT_e14_lhtight_e4_etcut_1invmAB5_L1JPSI-1M5-EM12', l1SeedThresholds=['EM12','EM3'], groups=MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_e14_etcut_1invmAB5_L1JPSI-1M5-EM12', l1SeedThresholds=['EM3','EM12'], groups=MultiElectronGroup),
        ChainProp(name='HLT_e9_lhtight_noringer_e4_etcut_1invmAB5_L1JPSI-1M5-EM7', l1SeedThresholds=['EM7','EM3'], groups=MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_e9_etcut_1invmAB5_L1JPSI-1M5-EM7', l1SeedThresholds=['EM3','EM7'], groups=MultiElectronGroup),
        ChainProp(name='HLT_e14_lhtight_noringer_e4_etcut_1invmAB5_L1JPSI-1M5-EM12', l1SeedThresholds=['EM12','EM3'], groups=MultiElectronGroup),
        ChainProp(name='HLT_e5_lhtight_noringer_e14_etcut_1invmAB5_L1JPSI-1M5-EM12', l1SeedThresholds=['EM3','EM12'], groups=MultiElectronGroup),
    ]

    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
      
        ChainProp(name='HLT_xe30_cell_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mht_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_tcpufit_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_trkmht_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_cssk_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfsum_vssk_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_pfopufit_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_cvfpufit_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mhtpufit_em_subjesgscIS_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mhtpufit_pf_subjesgscIS_L1XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),

        ChainProp(name='HLT_xe110_tc_em_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_mht_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_tcpufit_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_pfsum_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_pfsum_cssk_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        # MultiMET Chain
        ChainProp(name='HLT_xe30_cell_xe30_tcpufit_L1XE30', l1SeedThresholds=['FSNOSEED']*2, groups=MultiMETGroup, monGroups=['metMon:shifter']), #must be FS seeded

        # Test chains to determine rate after calo-only preselection for tracking
        ChainProp(name='HLT_xe60_cell_L1XE50', l1SeedThresholds=['FSNOSEED'], groups=SingleMETGroup),
        ChainProp(name='HLT_xe55_cell_xe70_tcpufit_L1XE50', l1SeedThresholds=['FSNOSEED']*2, groups=MultiMETGroup),
    ]


    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [

        ChainProp(name='HLT_j85_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j80_j60_SHARED_j40__L1J15', l1SeedThresholds=['FSNOSEED']*3, groups=MultiJetGroup),
        ChainProp(name='HLT_j85_CLEANlb_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j85_CLEANllp_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_L1J15', l1SeedThresholds=['FSNOSEED'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgsc_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subresjesgsc_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_011jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_015jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_059jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j85_ftf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        # Test chains with moment cuts
        ChainProp(name='HLT_j85_050momemfrac100_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j85_momhecfrac010_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j85_050momemfrac100XXmomhecfrac010_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        ChainProp(name='HLT_j45_pf_subresjesgsc_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_010jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_020jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_ftf_050jvt_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_ftf_preselj20_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_subjesIS_ftf_preselj20_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_subjesgsc_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_subjesgscIS_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j85_pf_ftf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        ChainProp(name='HLT_j45_nojcalib_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_sk_nojcalib_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_cssk_nojcalib_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_pf_nojcalib_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j45_cssk_pf_nojcalib_ftf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        ChainProp(name='HLT_j260_320eta490_L1J20', l1SeedThresholds=['FSNOSEED'], groups=['PS:Online']+SingleJetGroup),

        ChainProp(name='HLT_j460_a10_lcw_subjes_L1J20', l1SeedThresholds=['FSNOSEED'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1J20', l1SeedThresholds=['FSNOSEED'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j460_a10t_lcw_nojcalib_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10t_lcw_nojcalib_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_nojcalib_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_lcw_nojcalib_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_pf_nojcalib_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        ChainProp(name='HLT_j460_a10sd_cssk_pf_nojcalib_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_nojcalib_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_nojcalib_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        ChainProp(name='HLT_j0_FBDJNOSHARED10etXX20etXX34massXX50fbet_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_FBDJSHARED_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j60_j0_FBDJSHARED_L1J20', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

        # Central Exclusive Production for SM group
        ChainProp(name='HLT_2j100_L1CEP-CjJ50', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+MultiJetGroup),
        ChainProp(name='HLT_2j100_L1CEP-CjJ60', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+MultiJetGroup),

        # HT chains using pt conditions
        ChainProp(name='HLT_j0_HT1000_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_HT500_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50XX010jvt_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        # HT chains using et conditions
        ChainProp(name='HLT_j0_HT1000XX30et_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_HT500XX30et_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50XX30et_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_pf_ftf_HT50XX30etXX010jvt_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        # multiscenario with HT chains using pt conditions
        ChainProp(name='HLT_j0_HT1000_j0_DIJET80j12ptXX0j12eta240XX700djmass_L1J20', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

        # dijet chains using pt
        ChainProp(name='HLT_j0_DIJET80j12ptXX0j12eta240XX700djmass_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET80j12ptXX700djmassXXdjdphi260_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET70j12ptXX1000djmassXXdjdphi200XX400djdeta_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET20j12ptXX110djmass_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET20j12ptXX110djmass_PTRANGE2r3_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        # dijet chains using et
        ChainProp(name='HLT_j0_DIJET80j12etXX0j12eta240XX700djmass_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET80j12etXX700djmassXXdjdphi260_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET70j12etXX1000djmassXXdjdphi200XX400djdeta_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET20j12etXX110djmass_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),
        ChainProp(name='HLT_j0_DIJET20j12etXX110djmass_PTRANGE2r3_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup),

        ChainProp(name='HLT_j85_ftf_MASK300ceta210XX300nphi10_L1J20', l1SeedThresholds=['FSNOSEED'],
                  groups=SingleJetGroup),

        ChainProp(name='HLT_j40_j0_HT50XX10etXX0eta320_L1J20',
                  l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),
        ChainProp(name='HLT_j40_j0_HT50XX10ptXX0eta320_L1J20',
                  l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup), # same as above but using pt conditions

        # ATR-20624
        ChainProp(name='HLT_j0_perf_L1J12_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=['PS:Online']+SingleJetGroup),

        # TLA test chains, ATR-20395
        ChainProp(name='HLT_j20_JetDS_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['JetDS'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j20_JetDS_L1J50_DETA20-J50J', l1SeedThresholds=['FSNOSEED'], stream=['JetDS'], groups=EOFTLALegGroup+SingleJetGroup),
        # TLA test HT chains, ATR-21594
        ChainProp(name='HLT_j20_JetDS_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], stream=['JetDS'], groups=PrimaryLegGroup+SingleJetGroup),
        # Piggybacking on HH4b, exact thresholds for preselections to be determined later - this is for rate and cost
        ChainProp(name='HLT_j60_j45_2j20_JetDS_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*3, stream=['JetDS'], groups=PrimaryLegGroup+MultiJetGroup),

        # Emerging Jets test chains ATR-21593

        # dijet emtopo RC jets
        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_ExoticPTF0p0dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_ExoticPTF0p1dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_ExoticPTF0p2dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_ExoticPTF0p0dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_ExoticPTF0p1dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_ExoticPTF0p2dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_j110_a10r_subjesIS_ftf_0eta200_ExoticPTF0p0dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j110_a10r_subjesIS_ftf_0eta200_ExoticPTF0p1dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j110_a10r_subjesIS_ftf_0eta200_ExoticPTF0p2dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_ExoticPTF0p0dR1p2_L1SC111-CJ15', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_ExoticPTF0p1dR1p2_L1SC111-CJ15', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_ExoticPTF0p2dR1p2_L1SC111-CJ15', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_ExoticPTF0p0dR1p2_L1SC111-CJ15', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_ExoticPTF0p1dR1p2_L1SC111-CJ15', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_ExoticPTF0p2dR1p2_L1SC111-CJ15', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_j110_a10r_subjesIS_ftf_0eta200_ExoticPTF0p0dR1p2_L1SC111-CJ15', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j110_a10r_subjesIS_ftf_0eta200_ExoticPTF0p1dR1p2_L1SC111-CJ15', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j110_a10r_subjesIS_ftf_0eta200_ExoticPTF0p2dR1p2_L1SC111-CJ15', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        # dijet pflow jet chains
        ChainProp(name='HLT_j175_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p0dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p1dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p2dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_j110_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p0dR1p2_L1J50', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j110_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p1dR1p2_L1J50', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j110_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p2dR1p2_L1J50', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_2j175_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p0dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_2j175_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p1dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_2j175_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p2dR1p2_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_2j110_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p0dR1p2_L1J50', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_2j110_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p1dR1p2_L1J50', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_2j110_a10sd_pf_nojcalib_ftf_0eta200_ExoticPTF0p2dR1p2_L1J50', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        # dijet Trackless jets chains
        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_TracklessdR1p2_L1J100',    groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_TracklessdR1p2_L1J100',    groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j110_a10r_subjesIS_ftf_0eta200_TracklessdR1p2_L1J100',    groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_j260_a10r_subjesIS_ftf_0eta200_TracklessdR1p2_L1SC111-CJ15',    groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j175_a10r_subjesIS_ftf_0eta200_TracklessdR1p2_L1SC111-CJ15',    groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j110_a10r_subjesIS_ftf_0eta200_TracklessdR1p2_L1SC111-CJ15',    groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        ChainProp(name='HLT_j175_a10sd_pf_nojcalib_ftf_0eta200_TracklessdR1p2_L1J100',    groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_j110_a10sd_pf_nojcalib_ftf_0eta200_TracklessdR1p2_L1J50',    groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_2j175_a10sd_pf_nojcalib_ftf_0eta200_TracklessdR1p2_L1J100',    groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_2j110_a10sd_pf_nojcalib_ftf_0eta200_TracklessdR1p2_L1J50',    groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),

        # end of emerging jets chains



       
        # ATR-22096
        ChainProp(name='HLT_j420_ftf_L1J100', l1SeedThresholds=['FSNOSEED'],             groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j420_pf_ftf_L1J100', l1SeedThresholds=['FSNOSEED'],          groups=SingleJetGroup+SupportLegGroup),

        #Adding testing chains for Tight,Medium,Loose preselections (ATR-23547) and also corresponding emtopo chain
        ChainProp(name='HLT_j420_pf_ftf_preselj180_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j420_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j135_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j180_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),

        ChainProp(name='HLT_3j200_ftf_L1J100', l1SeedThresholds=['FSNOSEED'],            groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_4j115_ftf_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),


        ChainProp(name='HLT_5j70_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],     groups=MultiJetGroup+SupportLegGroup),

        ChainProp(name='HLT_2j250_pf_ftf_0eta240_j120_pf_ftf_0eta240_presel2j135XXj60_L1J100', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_2j250_pf_ftf_0eta240_j120_pf_ftf_0eta240_presel2j180XXj80_L1J100', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_2j250_pf_ftf_0eta240_j120_pf_ftf_0eta240_presel2j225XXj100_L1J100', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup + PrimaryLegGroup),

        #Adding testing chains for Tight,Medium,Loose preselections (ATR-23547) and also corresponding emtopo chain
        ChainProp(name='HLT_4j115_pf_ftf_presel4j55_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_4j115_pf_ftf_presel4j85_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_4j33_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_4j55_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_4j85_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),

 
        #Adding testing chains for Tight,Medium,Loose preselections (ATR-23547) and also corresponding emtopo chain
        ChainProp(name='HLT_5j70_pf_ftf_0eta240_presel5j35_L14J15', l1SeedThresholds=['FSNOSEED'],  groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_5j70_pf_ftf_0eta240_presel5j50_L14J15', l1SeedThresholds=['FSNOSEED'],  groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_5j24_L14J15', l1SeedThresholds=['FSNOSEED'],  groups=MultiJetGroup+SupportLegGroup ),
        ChainProp(name='HLT_5j35_L14J15', l1SeedThresholds=['FSNOSEED'],  groups=MultiJetGroup+SupportLegGroup ),
        ChainProp(name='HLT_5j50_L14J15', l1SeedThresholds=['FSNOSEED'],  groups=MultiJetGroup+SupportLegGroup ),

        ChainProp(name='HLT_5j85_ftf_L14J15', l1SeedThresholds=['FSNOSEED'],             groups=MultiJetGroup+SupportLegGroup),

        #Adding testing chains for Tight,Medium,Loose preselections (ATR-23547) and also corresponding emtopo chain
        ChainProp(name='HLT_5j85_pf_ftf_presel5j35_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_5j85_pf_ftf_presel5j50_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),

        ChainProp(name='HLT_6j55_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],     groups=MultiJetGroup+SupportLegGroup),

        #Adding testing chains for Tight,Medium,Loose preselections (ATR-23547) and also corresponding emtopo chain
        ChainProp(name='HLT_6j55_pf_ftf_0eta240_presel6j40_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_6j55_pf_ftf_0eta240_presel6j45_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_6j36_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_6j40_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_6j45_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),

        ChainProp(name='HLT_6j70_ftf_L14J15', l1SeedThresholds=['FSNOSEED'],             groups=MultiJetGroup+SupportLegGroup),

        #Adding testing chains for Tight,Medium,Loose preselections (ATR-23547) and also corresponding emtopo chain
        ChainProp(name='HLT_6j70_pf_ftf_presel6j40_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_6j70_pf_ftf_presel6j45_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),

        ChainProp(name='HLT_7j45_ftf_L14J15', l1SeedThresholds=['FSNOSEED'],            groups=MultiJetGroup+SupportLegGroup),

        #Adding testing chains for Tight,Medium,Loose preselections (ATR-23547) and also corresponding emtopo chain
        ChainProp(name='HLT_7j45_pf_ftf_presel7j28_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_7j45_pf_ftf_presel7j35_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_7j21_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup ),
        ChainProp(name='HLT_7j28_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup ),
        ChainProp(name='HLT_7j35_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup ),

        ChainProp(name='HLT_10j40_ftf_L14J15', l1SeedThresholds=['FSNOSEED'],            groups=MultiJetGroup+SupportLegGroup),
 
        #Adding testing chains for Tight,Medium,Loose preselections (ATR-23547) and also corresponding emtopo chain
        ChainProp(name='HLT_10j40_pf_ftf_presel7j28_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),
        ChainProp(name='HLT_10j40_pf_ftf_presel7j35_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup+SupportLegGroup),

        #  Chains to test JVT and PFlow effects in low-threshold 6j
        ChainProp(name='HLT_6j25_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],                           groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),

        ChainProp(name='HLT_6j35_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],                           groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),

        ChainProp(name='HLT_6j45_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],                           groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),

        ChainProp(name='HLT_6j25_pf_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_6j25_pf_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),

        ChainProp(name='HLT_6j35_pf_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),

        ChainProp(name='HLT_6j45_pf_ftf_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'],        groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_010jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_020jvt_L14J15', l1SeedThresholds=['FSNOSEED'], stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_050jvt_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),

        # HH4b
        # PFlow variants
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j55_pf_ftf_0eta240_j28_pf_ftf_0eta240_j20_pf_ftf_0eta240_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        # Adjusted thresholds for BSM (k_l=10)
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        # Mu-seeded
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_L1MU14FCH', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j55_pf_ftf_0eta240_j28_pf_ftf_0eta240_j20_pf_ftf_0eta240_L1MU14FCH', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j55_pf_ftf_0eta240_j28_pf_ftf_0eta240_j20_pf_ftf_0eta240_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, groups=MultiJetGroup),

        # ATR-22594
        ChainProp(name='HLT_j150_ftf_2j55_ftf_L1J85_3J30',                    l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),
        # SC L1 seed
        # Note: l1SeedThresholds needs to be given explicitly, as it cannot
        # be auto-filled by DictFromChainName for L1Topo items
        # Jet-only VBF chain
        ChainProp(name="HLT_j110_subjesgscIS_ftf_j45_subjesgscIS_ftf_L1J50", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup),
        ChainProp(name='HLT_j70_0eta320_j50_0eta490_j0_DJMASS1000j50dphi200x400deta_L1MJJ-500-NFF', l1SeedThresholds=['FSNOSEED']*3,stream=['VBFDelayed'],groups=PrimaryLegGroup+MultiJetGroup), # previously HLT_j70_j50_0eta490_invm1000j70_dphi20_deta40_L1MJJ-500-NFF

        # Test chains to check tracking/b-tagging rates for Run 2 HH4b chain
        ChainProp(name="HLT_2j35_0eta240_020jvt_pf_ftf_2j35_0eta240_020jvt_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta240_2j35_0eta240_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

    ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        # TO BE REMOVED
        # leave one split chain for one validation round
        ChainProp(name='HLT_j45_0eta290_020jvt_pf_ftf_boffperf_split_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),


        # single bjet pflow options, # changes according to ATR-23883
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r60_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r70_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryLegGroup),
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r77_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryLegGroup),
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r85_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),

        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1r70_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1r77_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1r85_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryLegGroup),

        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1r60_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1r77_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1r85_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryLegGroup),

        ChainProp(name='HLT_j360_0eta290_pf_ftf_bdl1r60_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j360_0eta290_pf_ftf_bdl1r70_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j360_0eta290_pf_ftf_bdl1r85_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),


        # ATR-22937
        # multi-b chains for assessing mistag rates and flavor fractions
        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r60_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r70_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r77_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r85_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),

        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r60_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r70_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r77_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r85_L1J45p0ETA21_3J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),

        # ATR-23883
        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r77_L13J35p0ETA23", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r77_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),

        # for monitoring
        ChainProp(name='HLT_j45_0eta290_020jvt_020jvt_pf_ftf_bdl1r70_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),

        # very loose chain for tagger training
        ChainProp(name='HLT_j20_0eta290_020jvt_pf_ftf_boffperf_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),


        # Test chains for ttbar and HH->4b chains
        # 3j20_b[70,77,85]
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_3j20_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_3j20_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=MultiBjetGroup),
        # 2j20_b[60,70,77,85]
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_2j20_pf_ftf_0eta240_020jvt_bdl1r60_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_2j20_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_2j20_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup),
        # j[55,28,20]_b[70,77,85]
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j55_pf_ftf_0eta240_020jvt_bdl1r70_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j55_pf_ftf_0eta240_020jvt_bdl1r77_j28_pf_ftf_0eta240_020jvt_bdl1r77_j20_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j55_pf_ftf_0eta240_020jvt_bdl1r85_j28_pf_ftf_0eta240_020jvt_bdl1r85_j20_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup),
        # j[28,20]_b[60,70,77,85]
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r77_j20_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup),
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r85_j20_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup),
        # Adjusted thresholds for BSM (k_l=10)
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j50_pf_ftf_0eta240_020jvt_bdl1r70_j35_pf_ftf_0eta240_020jvt_bdl1r70_j25_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j50_pf_ftf_0eta240_020jvt_bdl1r77_j35_pf_ftf_0eta240_020jvt_bdl1r77_j25_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j50_pf_ftf_0eta240_020jvt_bdl1r85_j35_pf_ftf_0eta240_020jvt_bdl1r85_j25_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*7, stream=[PhysicsStream], groups=MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j35_pf_ftf_0eta240_020jvt_bdl1r70_j25_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j35_pf_ftf_0eta240_020jvt_bdl1r77_j25_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j35_pf_ftf_0eta240_020jvt_bdl1r85_j25_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_3j25_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_3j25_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_2j25_pf_ftf_0eta240_020jvt_bdl1r60_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_2j25_pf_ftf_0eta240_020jvt_bdl1r70_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_2j25_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=MultiBjetGroup),
        # Copies with L1 muon+jet seed
        # j[28,20]_b60
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r70_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),
        # j20_b60
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_bdl1r70_L1MU8F_2J15_J20', l1SeedThresholds=['FSNOSEED']*4, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),

        # Boffperf to facilitate emulation studies
        ChainProp(name='HLT_j80_pf_ftf_j55_pf_ftf_j28_pf_ftf_j20_0eta290_pf_ftf_boffperf_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*4, groups=MultiBjetGroup),

        # Tests of potential TLA chains for cost/rate
        # ATR-23002 - b-jets
        ChainProp(name='HLT_j20_0eta290_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j100_pf_ftf_0eta320_j20_0eta290_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED']*2, groups=SingleBjetGroup),
        ChainProp(name='HLT_4j20_0eta290_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name='HLT_4j20_pf_ftf_020jvt_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),
        ChainProp(name='HLT_3j20_pf_ftf_020jvt_j20_0eta290_pf_ftf_boffperf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED']*2, groups=MultiBjetGroup),
        ChainProp(name='HLT_4j20_pf_ftf_020jvt_boffperf_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup),


        # Candidates for allhad ttbar delayed stream
        ChainProp(name='HLT_5j25_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_L14J15', l1SeedThresholds=['FSNOSEED']*2, groups=MultiBjetGroup),
        ChainProp(name='HLT_5j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_L14J15', l1SeedThresholds=['FSNOSEED']*2, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_5j45_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_L14J15', l1SeedThresholds=['FSNOSEED']*2, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),

        # Boffperf copy to facilitate emulation studies
        ChainProp(name='HLT_5j25_pf_ftf_0eta240_j25_pf_ftf_0eta240_boffperf_L14J15', l1SeedThresholds=['FSNOSEED']*2, groups=MultiBjetGroup),

        # These are VBF chains
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j60_pf_ftf_0eta320_j45_pf_ftf_320eta490_SHARED_2j45_pf_ftf_0eta290_bdl1r60_L1J40p0ETA25_2J25_J20p31ETA49', l1SeedThresholds=['FSNOSEED']*4, groups=PrimaryLegGroup+MultiBjetGroup),
        # This chain should reduce to the following one, kept to test
        ChainProp(name='HLT_2j40_pf_ftf_0eta490_SHARED_2j45_pf_ftf_0eta290_bdl1r70_j0_pf_ftf_HT300_j0_pf_ftf_DJMASS700j35_L1HT150-J20s5pETA31_MJJ-400-CF', l1SeedThresholds=['FSNOSEED']*4, groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_2j45_pf_ftf_bdl1r70_j0_pf_ftf_HT300_j0_pf_ftf_DJMASS700j35_L1HT150-J20s5pETA31_MJJ-400-CF', l1SeedThresholds=['FSNOSEED']*3, groups=PrimaryLegGroup+MultiBjetGroup),
        # While it may appear that the 2j35 selections are redundant, in fact the SHARED expression
        # allows the b-tags to be on any of the four jets preceding "SHARED"
        ChainProp(name='HLT_j70_pf_ftf_0eta490_j50_pf_ftf_0eta490_2j35_pf_ftf_0eta490_SHARED_2j35_pf_ftf_0eta290_bdl1r70_j0_pf_ftf_DJMASS1000j50_L1MJJ-500-NFF', l1SeedThresholds=['FSNOSEED']*5,stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),

        # TODO
        ### Are these VBF chains?
        ChainProp(name="HLT_j55_pf_ftf_0eta320_bdl1r70_2j45_pf_ftf_320eta490_L1J25p0ETA23_2J15p31ETA49",l1SeedThresholds=['FSNOSEED']*2,  stream=[PhysicsStream], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j80_pf_ftf_0eta320_bdl1r70_j60_pf_ftf_0eta320_bdl1r85_j45_pf_ftf_320eta490_L1J40p0ETA25_2J25_J20p31ETA49", l1SeedThresholds=['FSNOSEED']*3,stream=[PhysicsStream], groups=PrimaryLegGroup+MultiBjetGroup),


        # Chris doesn't know which of these are for what signature
        ChainProp(name="HLT_j150_0eta320_pf_ftf_2j55_0eta290_020jvt_pf_ftf_bdl1r70_L1J85_3J30", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_3j35_0eta290_020jvt_pf_ftf_bdl1r70_j35_pf_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j175_0eta290_020jvt_pf_ftf_bdl1r60_j60_0eta290_020jvt_pf_ftf_bdl1r60_L1J100", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta290_020jvt_pf_ftf_bdl1r70_2j35_0eta290_020jvt_pf_ftf_bdl1r85_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j55_0eta290_020jvt_pf_ftf_bdl1r60_2j55_pf_ftf_0eta320_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta290_020jvt_pf_ftf_bdl1r60_3j35_pf_ftf_0eta320_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r60_3j45_pf_ftf_0eta320_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j75_0eta290_020jvt_pf_ftf_bdl1r60_3j75_pf_ftf_L14J20", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        #ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r50_2j45_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r60_2j45_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        # Run 2 HH4b low-threshold chain
        ChainProp(name="HLT_2j35_0eta240_020jvt_pf_ftf_bdl1r60_2j35_0eta240_020jvt_pf_ftf_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),

        # EMTopo Chains (likely not used)
        # ATR-22165
        ChainProp(name='HLT_j275_subjesgscIS_ftf_bdl1r60_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j300_subjesgscIS_ftf_bdl1r70_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j360_subjesgscIS_ftf_bdl1r77_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_j45_subjesgscIS_ftf_bdl1r70_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),

        ChainProp(name="HLT_j110_subjesgscIS_ftf_bdl1r60_j45_subjesgscIS_ftf_bdl1r70_L1J50", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiBjetGroup),

    ]

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        #ATR-20049
        ChainProp(name="HLT_tau0_ptonly_L1TAU8", groups=SingleTauGroup),
        ChainProp(name="HLT_tau0_ptonly_L1TAU60", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_ptonly_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_idperf_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_perf_tracktwo_L1TAU12IM", groups=SingleTauGroup),
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
        ChainProp(name="HLT_tau25_medium1_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_ptonly_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_idperf_tracktwo_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_idperf_tracktwoMVA_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_idperf_tracktwoMVABDT_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_perf_tracktwo_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_perf_tracktwoMVA_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_perf_tracktwoMVABDT_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_looseRNN_tracktwoMVA_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_looseRNN_tracktwoMVABDT_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVA_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVABDT_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_tightRNN_tracktwoMVA_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_tightRNN_tracktwoMVABDT_L1TAU20IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_ptonly_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_tracktwo_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_tracktwoMVA_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_idperf_tracktwoMVABDT_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_tracktwo_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_tracktwoMVA_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_tracktwoMVABDT_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_medium1_tracktwo_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_perf_tracktwoMVATest_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_mediumRNN_tracktwoMVATest_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau180_tightRNN_tracktwoLLP_L1TAU100", groups=SupportLegGroup+SingleTauGroup),    # 
        ChainProp(name="HLT_tau200_ptonly_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau200_medium1_tracktwo_L1TAU100", groups=SingleTauGroup),
        ChainProp(name="HLT_tau200_mediumRNN_tracktwoLLP_L1TAU100", groups=SupportLegGroup+SingleTauGroup),   # 
        ChainProp(name="HLT_tau200_tightRNN_tracktwoLLP_L1TAU100", groups=SupportLegGroup+SingleTauGroup),

        # displaced tau+X (ATR-21754)
        ChainProp(name="HLT_tau80_mediumRNN_tracktwoLLP_tau60_tightRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40", l1SeedThresholds=['TAU60','TAU40'], groups=SupportLegGroup+TauJetGroup),
        ChainProp(name="HLT_tau80_tightRNN_tracktwoLLP_tau60_tightRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40", l1SeedThresholds=['TAU60','TAU40'], groups=SupportLegGroup+TauJetGroup),
        ChainProp(name="HLT_tau100_mediumRNN_tracktwoLLP_tau80_mediumRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40", l1SeedThresholds=['TAU60','TAU40'], groups=SupportLegGroup+TauJetGroup),

        # Phase-I 
        ChainProp(name="HLT_tau25_mediumRNN_tracktwoMVABDT_L1eTAU12",   groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_mediumRNN_tracktwoMVABDT_L1eTAU12M",  groups=SingleTauGroup),
        #ChainProp(name="HLT_tau25_mediumRNN_tracktwoMVABDT_L1cTAU12",   groups=SingleTauGroup), #TODO: cTAU seeding missing
        ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVABDT_L1eTAU20",   groups=SingleTauGroup),
        #ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVABDT_L1cTAU20",   groups=SingleTauGroup),
        ChainProp(name="HLT_tau160_mediumRNN_tracktwoMVABDT_L1eTAU100", groups=SingleTauGroup),
        # More Phase-I tau chains (ATR-24182)
        ChainProp(name='HLT_tau200_mediumRNN_tracktwoMVABDT_L1eTAU100', groups=PrimaryPhIGroup+SingleTauGroup),
        ChainProp(name='HLT_tau180_mediumRNN_tracktwoLLP_L1eTAU100', groups=PrimaryPhIGroup+SingleTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVABDT_tau60_mediumRNN_tracktwoMVABDT_03dRAB_L1eTAU60_2eTAU40', l1SeedThresholds=['eTAU60','eTAU40'], groups=PrimaryPhIGroup+MultiTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoLLP_tau60_mediumRNN_tracktwoLLP_03dRAB_L1eTAU60_2eTAU40', l1SeedThresholds=['eTAU60','eTAU40'], groups=PrimaryPhIGroup+MultiTauGroup),

    ]

    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
        #ATR-21003; default dimuon and Bmumux chains from Run2; l2io validation; should not be moved to Physics
        ChainProp(name='HLT_2mu4_noL2Comb_bJpsimumu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_noL2Comb_mu4_noL2Comb_bJpsimumu_L1MU5VF_2MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_noL2Comb_bJpsimumu_L12MU5VF', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_BpmumuKp_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_BcmumuPi_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_BsmumuPhi_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_BdmumuKst_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_noL2Comb_bBmumux_LbPqKm_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),

        #ATR-20603; B-trigger monitoring, ART tests; should not be moved to Physics
        ChainProp(name='HLT_2mu4_bJpsimumu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bUpsimumu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        #ATR-20839; validation and ART; should not be moved to Physics
        ChainProp(name='HLT_2mu4_bDimu_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),

        #ATR-21639; validation and monitoring of Bmumux chains; should not be moved to Physics
        ChainProp(name='HLT_2mu4_bBmumux_BpmumuKp_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BcmumuPi_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BsmumuPhi_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BdmumuKst_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumux_LbPqKm_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BcmumuDsloose_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BcmumuDploose_L12MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),

        #ATR-23576; dimuon + L1Topo; primary triggers; should be moved to Physics after validation
        ChainProp(name='HLT_2mu4_bBmumu_Lxy0_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bJpsimumu_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumu_Lxy0_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bUpsimumu_L1BPH-8M15-0DR22-MU5VFMU3V-BO', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bJpsimumu_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_2mu6_bBmumu_Lxy0_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_2mu6_bUpsimumu_L1BPH-8M15-0DR22-2MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_2mu6_bPhi_L1LFV-MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumu_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_mu11_mu6_bDimu2700_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_mu11_mu6_bDimu_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_mu11_mu6_bJpsimumu_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_mu11_mu6_bUpsimumu_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_mu11_mu6_bPhi_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_mu11_mu6_bTau_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),

        #ATR-23576, yellow chains, should be moved to Physics
        ChainProp(name='HLT_3mu6_bJpsi_L13MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu6_bUpsi_L13MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu6_bTau_L13MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_2mu6_mu4_bTau_L12MU5VF_3MU3V', l1SeedThresholds=['MU5VF', 'MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_2mu6_mu4_bUpsi_L12MU5VF_3MU3V', l1SeedThresholds=['MU5VF', 'MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu6_2mu4_bTau_L1MU5VF_3MU3V', l1SeedThresholds=['MU5VF', 'MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu6_2mu4_bJpsi_L1MU5VF_3MU3V', l1SeedThresholds=['MU5VF', 'MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu6_2mu4_bUpsi_L1MU5VF_3MU3V', l1SeedThresholds=['MU5VF', 'MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu4_bDimu2700_L13MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        #
        ChainProp(name='HLT_mu11_mu6_bDimu_Lxy0_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bDimu2700_Lxy0_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bJpsimumu_Lxy0_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumux_BpmumuKp_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        #
        ChainProp(name='HLT_2mu6_bDimu_L12MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_2mu6_bBmumu_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bDimu_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bBmumux_BpmumuKp_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_2mu6_bBmumux_BsmumuPhi_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=PrimaryL1MuGroup+BphysicsGroup),
        ChainProp(name='HLT_2mu6_bDimu_L1LFV-MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bBmumux_BpmumuKp_L1LFV-MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bBmumux_BsmumuPhi_L1LFV-MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        #
        ChainProp(name='HLT_mu6_mu4_bDimu_L1MU5VF_2MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumux_BsmumuPhi_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumu_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumux_BpmumuKp_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bDimu_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        #
        ChainProp(name='HLT_2mu4_bBmumu_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bJpsimumu_Lxy0_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BpmumuKp_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BsmumuPhi_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup),

        # supplementary PEB triggers
        ChainProp(name='HLT_mu4_bJpsimutrk_MuonTrkPEB_L1MU3V', stream=['BphysPEB'], groups=BphysicsGroup+['PS:Online']),
        ChainProp(name='HLT_mu6_bJpsimutrk_MuonTrkPEB_L1MU5VF', stream=['BphysPEB'], groups=BphysicsGroup+['PS:Online']),
        ChainProp(name='HLT_mu10_bJpsimutrk_MuonTrkPEB_L1MU8F', stream=['BphysPEB'], groups=BphysicsGroup+['PS:Online']),
        ChainProp(name='HLT_mu20_bJpsimutrk_MuonTrkPEB_L1MU14FCH', stream=['BphysPEB'], groups=BphysicsGroup+['PS:Online']),

        # RCP multiple candidate
        ChainProp(name='HLT_mu10_l2mt_mu4_l2mt_bJpsimumu_L1MU10BOM', l1SeedThresholds=['MU10BOM']*2, stream=["BphysDelayed"], groups=BphysicsGroup),

        # mu6+mu4 chains with L1 charge cut (ATR-19639)
        ChainProp(name='HLT_mu6_mu4_bBmumux_BsmumuPhi_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumu_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumux_BpmumuKp_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_mu6_mu4_bDimu_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_mu6_mu4_bJpsimumu_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumu_Lxy0_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup),
        #ATR-21566, di-muon TLA
        ChainProp(name='HLT_2mu4_b0dRAB12vtx20_L1BPH-0DR12C-2MU3V', l1SeedThresholds=['MU3V'],stream=["BphysDelayed"], groups=BphysicsGroup),
    ]

    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [


        # Primary e-mu chains
        ChainProp(name='HLT_e17_lhloose_mu14_L1EM15VH_MU8F', l1SeedThresholds=['EM15VH','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e7_lhmedium_mu24_L1MU14FCH',l1SeedThresholds=['EM3','MU14FCH'],  stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e12_lhloose_2mu10_L12MU8F', l1SeedThresholds=['EM8VH','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_2e12_lhloose_mu10_L12EM8VH_MU8F', l1SeedThresholds=['EM8VH','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        # Primary g-mu chains
        ChainProp(name='HLT_g25_medium_mu24_L1MU14FCH',l1SeedThresholds=['EM15VH','MU14FCH'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup), #ATR-22594
        ChainProp(name='HLT_g35_loose_mu18_L1EM22VHI', l1SeedThresholds=['EM22VHI','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_loose_mu18_L1EM24VHI', l1SeedThresholds=['EM24VHI','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_2g10_loose_mu20_L1MU14FCH', l1SeedThresholds=['EM7','MU14FCH'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup), # unsure what EM seed should be
        #LLP
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L12MU8F', l1SeedThresholds=['EM8VH','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g40_loose_mu40_msonly_L1MU14FCH', l1SeedThresholds=['EM20VH','MU14FCH'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup), #ATR-20505
        
        # Test chains for muon + jet/MET merging/aligning
        ChainProp(name='HLT_mu6_xe30_mht_L1XE30', l1SeedThresholds=['MU5VF','FSNOSEED'], stream=[PhysicsStream], groups=MuonMETGroup),
        ChainProp(name='HLT_mu6_j45_nojcalib_L1J20', l1SeedThresholds=['MU5VF','FSNOSEED'], stream=[PhysicsStream], groups=MuonJetGroup),

        #ATR-22107
        ChainProp(name='HLT_e24_lhmedium_mu8noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], stream=[PhysicsStream], groups=EgammaMuonGroup),
        ChainProp(name='HLT_e26_lhmedium_mu8noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e28_lhmedium_mu8noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e9_lhvloose_mu20_mu8noL1_L1MU14FCH', l1SeedThresholds=['EM3','MU14FCH','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_loose_mu15_mu2noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','MU5VF','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_loose_mu15_mu2noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','MU5VF','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_tight_icalotight_mu18noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_tight_icalotight_mu18noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_tight_icalotight_mu15noL1_mu2noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_tight_icalotight_mu15noL1_mu2noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        # tau+X chains (ATR-21609) 
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVA_tau20_mediumRNN_tracktwoMVA_03dRAB_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['TAU8','TAU8','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=SupportLegGroup+TauJetGroup),
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVABDT_tau20_mediumRNN_tracktwoMVABDT_03dRAB_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['TAU8','TAU8','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+TauJetGroup),
        # mu-tag & tau-probe triggers for LLP (ATR-23150)
        ChainProp(name='HLT_mu26_ivarmedium_tau100_mediumRNN_tracktwoLLP_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU60'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_tau100_mediumRNN_tracktwoLLP_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU60'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),

        # MET + tau tag and probe chains (ATR-23507)
        ChainProp(name='HLT_tau25_medium1_tracktwo_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU12IM','FSNOSEED'], groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau20_mediumRNN_tracktwoMVA_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU8','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau20_mediumRNN_tracktwoMVABDT_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU8','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVA_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU12IM','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVABDT_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU12IM','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVA_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU20IM','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVABDT_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU20IM','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau40_mediumRNN_tracktwoMVA_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU25IM','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau40_mediumRNN_tracktwoMVABDT_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU25IM','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau60_mediumRNN_tracktwoMVA_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU40','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau60_mediumRNN_tracktwoMVABDT_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU40','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVA_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU60','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVABDT_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU60','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau160_mediumRNN_tracktwoMVA_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU100','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau160_mediumRNN_tracktwoMVABDT_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU100','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau60_mediumRNN_tracktwoLLP_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU40','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoLLP_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU60','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau180_mediumRNN_tracktwoLLP_probe_xe110_pfsum_vssk_L1XE50', l1SeedThresholds=['PROBETAU100','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
 
        
        # photon + multijets (ATR-22594)
        ChainProp(name='HLT_g85_tight_3j50_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_g45_loose_6j45_L14J15p0ETA25',l1SeedThresholds=['EM15','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaJetGroup),

        # photon + MET (ATR-22594, ATR-21565)
        ChainProp(name='HLT_g90_loose_xe90_cell_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMETGroup),
        ChainProp(name='HLT_g25_tight_icalotight_xe40_cell_xe50_tcpufit_18dphiAB_18dphiAC_80mTAC_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMETGroup),
        ChainProp(name='HLT_g25_tight_icalotight_xe40_cell_xe40_tcpufit_xe40_pfopufit_18dphiAB_18dphiAC_80mTAC_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMETGroup),


        # electron + MET (ATR-22594)
        ChainProp(name='HLT_e70_lhloose_xe70_cell_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMETGroup),

        # photon + VBF Hbb (ATR-23293) 
        ChainProp(name='HLT_g25_medium_2j35_pf_ftf_0eta490_bdl1r77_2j35_pf_ftf_0eta490_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaBjetGroup), 
        ChainProp(name='HLT_g25_medium_j35_pf_ftf_0eta490_bdl1r77_3j35_pf_ftf_0eta490_j0_pf_ftf_DJMASS700j35_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaBjetGroup),
        ChainProp(name='HLT_g20_tight_icaloloose_j35_pf_ftf_bdl1r77_3j35_pf_ftf_0eta490_j0_pf_ftf_DJMASS500j35_L1EM18VHI_MJJ-300',l1SeedThresholds=['EM18VHI','FSNOSEED','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaBjetGroup),        
        
        
        # VBF triggers (ATR-22594)
        ChainProp(name='HLT_2mu6_2j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['MU5VF','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+MuonJetGroup), # Formerly HLT_2mu6_2j50_0eta490_invm900j50
        ChainProp(name='HLT_e5_lhvloose_j70_0eta320_j50_0eta490_j0_DJMASS1000j50_xe50_tcpufit_L1MJJ-500-NFF',l1SeedThresholds=['EM3','FSNOSEED','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_2e5_lhmedium_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['EM3','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_g25_medium_4j35_0eta490_j0_DJMASS1000j35_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_e10_lhmedium_ivarloose_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['EM8VH','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_mu4_j70_0eta320_j50_0eta490_j0_DJMASS1000j50_xe50_tcpufit_L1MJJ-500-NFF',l1SeedThresholds=['MU3V','FSNOSEED','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+MuonJetGroup),
        ChainProp(name='HLT_mu10_ivarmedium_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['MU8F','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+MuonJetGroup),
        ChainProp(name='HLT_j70_0eta320_j50_0eta490_j0_DJMASS1000j50dphi240_xe90_tcpufit_xe50_cell_L1MJJ-500-NFF',l1SeedThresholds=['FSNOSEED']*5,stream=['VBFDelayed'], groups=PrimaryLegGroup+JetMETGroup),

        # meson + photon (ATR-22644, ATR-23239)
        ChainProp(name='HLT_g25_medium_tau25_dikaonmass_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_kaonpi1_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_kaonpi2_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_singlepion_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_dipion1_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_dipion2_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g35_medium_tau25_dipion3_tracktwoMVA_60invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_dipion4_tracktwoMVA_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),

        ChainProp(name='HLT_g25_medium_tau25_dikaonmass_tracktwoMVABDT_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_kaonpi1_tracktwoMVABDT_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_kaonpi2_tracktwoMVABDT_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_singlepion_tracktwoMVABDT_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_dipion1_tracktwoMVABDT_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_dipion2_tracktwoMVABDT_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g35_medium_tau25_dipion3_tracktwoMVABDT_60invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_g25_medium_tau25_dipion4_tracktwoMVABDT_50invmAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),


        #Combined BPhys
        #ATR-22749; chain configuration is broken, temporarily comment them out, see ATR-23839 and ATR-23965
        #ChainProp(name='HLT_e9_lhvloose_e5_lhvloose_bBeeM6000_mu6_l2io_L1BPH-0M9-EM7-EM5_MU5VF', l1SeedThresholds=['EM7','EM3','MU5VF'], groups=BphysElectronGroup),
        #ChainProp(name='HLT_e9_lhvloose_e5_lhvloose_bBeeM6000_2mu4_l2io_L1BPH-0M9-EM7-EM5_2MU3V', l1SeedThresholds=['EM7','EM3','MU3V'], groups=BphysElectronGroup),
        #ChainProp(name='HLT_e5_lhvloose_bBeeM6000_mu6_l2io_L1BPH-0DR3-EM7J15_MU5VF', l1SeedThresholds=['EM7','MU5VF'], groups=BphysElectronGroup),
        #ChainProp(name='HLT_e5_lhvloose_bBeeM6000_2mu4_l2io_L1BPH-0DR3-EM7J15_2MU3V', l1SeedThresholds=['EM7','MU3V'], groups=BphysElectronGroup),

        # Tests of potential TLA chains for cost/rate
        # ATR-19317 - dijet+ISR 
        ChainProp(name='HLT_g35_loose_3j25_pf_ftf_L1EM22VHI',          l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),
        ChainProp(name='HLT_g35_medium_3j25_pf_ftf_L1EM22VHI',         l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),
        ChainProp(name='HLT_g35_tight_3j25_pf_ftf_L1EM22VHI',          l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),
        ChainProp(name='HLT_g35_tight_3j25_0eta290_pf_ftf_boffperf_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=EgammaJetGroup),


        #ATR-23600
        ChainProp(name='HLT_tau50_mediumRNN_tracktwoMVA_xe80_tcpufit_xe50_cell_L1XE50', l1SeedThresholds=['TAU25IM','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau50_mediumRNN_tracktwoMVA_xe80_pfopufit_xe50_cell_L1XE50', l1SeedThresholds=['TAU25IM','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau50_mediumRNN_tracktwoMVABDT_xe80_tcpufit_xe50_cell_L1XE50', l1SeedThresholds=['TAU25IM','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau50_mediumRNN_tracktwoMVABDT_xe80_pfopufit_xe50_cell_L1XE50', l1SeedThresholds=['TAU25IM','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+TauMETGroup),
        ChainProp(name='HLT_j80_pf_ftf_bdl1r60_xe60_cell_L12J50_XE40', l1SeedThresholds=['FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+BjetMETGroup),
        ChainProp(name='HLT_g25_medium_mu24_ivarmedium_L1MU14FCH', l1SeedThresholds=['EM22VHI','MU14FCH'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        # Late stream for LLP
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L1MU3V_EMPTY', l1SeedThresholds=['EM8VH','MU3V'], stream=['Late'], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L1MU5VF_EMPTY', l1SeedThresholds=['EM8VH','MU5VF'], stream=['Late'], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L1MU3V_UNPAIRED_ISO', l1SeedThresholds=['EM8VH','MU3V'], stream=['Late'], groups=PrimaryLegGroup+EgammaMuonGroup),
        #
        ChainProp(name='HLT_j55_0eta240_xe50_cell_L1J30_EMPTY', l1SeedThresholds=['FSNOSEED']*2, stream=['Late'], groups=PrimaryLegGroup+JetMETGroup),
        ChainProp(name='HLT_j55_0eta240_xe50_cell_L1J30_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED']*2, stream=['Late'], groups=PrimaryLegGroup+JetMETGroup),

        # high-mu AFP
        ChainProp(name='HLT_2j20_mb_afprec_afpdijet_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportLegGroup),
        ChainProp(name='HLT_2j135_mb_afprec_afpdijet_L1CEP-CjJ60', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportLegGroup),
        ChainProp(name='HLT_2j120_mb_afprec_afpdijet_L1CEP-CjJ50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportLegGroup),
        ChainProp(name='HLT_j120_mb_afprec_afpdijet_L1AFP_A_AND_C_TOF_J50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportLegGroup),
        ChainProp(name='HLT_j175_mb_afprec_afpdijet_L1AFP_A_AND_C_TOF_J75', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportLegGroup),

        # bjet+met+met
        ChainProp(name="HLT_j100_pf_ftf_bdl1r60_xe50_cell_xe85_tcpufit_L1XE55", l1SeedThresholds=['FSNOSEED','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+BjetMETGroup),
        ChainProp(name="HLT_2j45_pf_ftf_bdl1r60_xe50_cell_xe85_tcpufit_L12J15_XE55", l1SeedThresholds=['FSNOSEED','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+BjetMETGroup),
        ChainProp(name="HLT_3j35_pf_ftf_bdl1r60_xe50_cell_xe70_tcpufit_L13J15p0ETA25_XE40", l1SeedThresholds=['FSNOSEED','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+BjetMETGroup),
        ChainProp(name="HLT_j100_pf_ftf_bdl1r60_xe50_cell_xe85_pfopufit_L1XE55", l1SeedThresholds=['FSNOSEED','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+BjetMETGroup),
        ChainProp(name="HLT_2j45_pf_ftf_bdl1r60_xe50_cell_xe85_pfopufit_L12J15_XE55", l1SeedThresholds=['FSNOSEED','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+BjetMETGroup),
        ChainProp(name="HLT_3j35_pf_ftf_bdl1r60_xe50_cell_xe70_pfopufit_L13J15p0ETA25_XE40", l1SeedThresholds=['FSNOSEED','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+BjetMETGroup),
    

        ## Unconventional tracking ATR-23797
        # hit-based DV
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk200_hitdv_tight_L1XE50', groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk200_hitdv_medium_L1XE50', groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),
        # disappearing track trigger
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk20_distrk_tight_L1XE50',  groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk20_distrk_medium_L1XE50', groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),
        
        # dEdx triggers
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk25_dedx_medium_L1XE50', groups=SupportLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk50_dedx_medium_L1XE50', groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),

        #ATR-23156
        ChainProp(name='HLT_mu4_j20_0eta290_pf_ftf_boffperf_dRAB03_L1MU3V_J15', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
        ChainProp(name='HLT_mu4_j20_0eta290_pf_ftf_boffperf_dRAB03_L1MU3V', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup),
    
        #ATR-23394
        ChainProp(name='HLT_e14_lhtight_mu6_dRAB15_invmAB10_L1LFV-eEM12L-MU5VF', l1SeedThresholds=['eEM10L','MU5VF'], stream=[PhysicsStream], groups=BphysicsGroup), #TODO: eEM12L->eEM10L in HLT seeding to fix
        ChainProp(name='HLT_e12_lhtight_mu11_dRAB15_invmAB10_L1LFV-eEM8L-MU8VF', l1SeedThresholds=['eEM8L','MU8VF'], stream=[PhysicsStream], groups=BphysicsGroup),    


        # Emerging Jets test chains ATR-21593
        # dijet + photon pflow emerging jets chains
        ChainProp(name='HLT_g85_tight_icaloloose_2j85_pf_ftf_0eta200_ExoticPTF0p0dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),
        ChainProp(name='HLT_g85_tight_icaloloose_2j85_pf_ftf_0eta200_ExoticPTF0p1dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),
        ChainProp(name='HLT_g85_tight_icaloloose_2j85_pf_ftf_0eta200_ExoticPTF0p2dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),

        ChainProp(name='HLT_g60_tight_icaloloose_2j85_pf_ftf_0eta200_ExoticPTF0p0dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),
        ChainProp(name='HLT_g60_tight_icaloloose_2j85_pf_ftf_0eta200_ExoticPTF0p1dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),
        ChainProp(name='HLT_g60_tight_icaloloose_2j85_pf_ftf_0eta200_ExoticPTF0p2dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),

        ChainProp(name='HLT_g45_tight_icaloloose_2j85_pf_ftf_0eta200_ExoticPTF0p0dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),
        ChainProp(name='HLT_g45_tight_icaloloose_2j85_pf_ftf_0eta200_ExoticPTF0p1dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),
        ChainProp(name='HLT_g45_tight_icaloloose_2j85_pf_ftf_0eta200_ExoticPTF0p2dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),


        ChainProp(name='HLT_g45_tight_icaloloose_2j55_pf_ftf_0eta200_ExoticPTF0p0dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),
        ChainProp(name='HLT_g45_tight_icaloloose_2j55_pf_ftf_0eta200_ExoticPTF0p1dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),
        ChainProp(name='HLT_g45_tight_icaloloose_2j55_pf_ftf_0eta200_ExoticPTF0p2dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),

        ChainProp(name='HLT_g60_tight_icaloloose_2j55_pf_ftf_0eta200_ExoticPTF0p0dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),
        ChainProp(name='HLT_g60_tight_icaloloose_2j55_pf_ftf_0eta200_ExoticPTF0p1dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),
        ChainProp(name='HLT_g60_tight_icaloloose_2j55_pf_ftf_0eta200_ExoticPTF0p2dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED']),

        ChainProp(name='HLT_g45_tight_icaloloose_3j35_pf_ftf_0eta240_2j55_pf_ftf_0eta200_ExoticPTF0p0dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup+MultiJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_g45_tight_icaloloose_3j35_pf_ftf_0eta240_2j55_pf_ftf_0eta200_ExoticPTF0p1dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup+MultiJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED']),
        ChainProp(name='HLT_g45_tight_icaloloose_3j35_pf_ftf_0eta240_2j55_pf_ftf_0eta200_ExoticPTF0p2dR1p2_L1EM22VHI', groups=PrimaryLegGroup+EgammaJetGroup+MultiJetGroup, l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED']),


        ]

    TriggerFlags.HeavyIonSlice.signatures  = TriggerFlags.HeavyIonSlice.signatures() + []
    TriggerFlags.BeamspotSlice.signatures  = TriggerFlags.BeamspotSlice.signatures() + [
        ChainProp(name='HLT_beamspot_allTE_trkfast_BeamSpotPEB_L1J15',  l1SeedThresholds=['FSNOSEED'], stream=['BeamSpot'], groups=['PS:Online', 'RATE:BeamSpot',  'BW:BeamSpot']),
    ]
    TriggerFlags.MinBiasSlice.signatures   = TriggerFlags.MinBiasSlice.signatures() + [

        ChainProp(name="HLT_mb_mbts_L1MBTS_1_EMPTY",               l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name="HLT_mb_mbts_L1MBTS_1",                     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name="HLT_mb_mbts_L1MBTS_1_1",                   l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name="HLT_mb_mbts_L1MBTS_2",                     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name="HLT_mb_mbts_L1RD0_FILLED",                 l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name="HLT_mb_mbts_L1RD0_EMPTY",                  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp400_trk40_hmt_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp600_trk45_hmt_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp700_trk55_hmt_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp900_trk60_hmt_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp900_trk65_hmt_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1000_trk70_hmt_L1RD0_FILLED',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1200_trk75_hmt_L1RD0_FILLED',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1400_trk80_hmt_L1RD0_FILLED',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1400_trk90_hmt_L1RD0_FILLED',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1600_trk100_hmt_L1RD0_FILLED',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1800_trk110_hmt_L1RD0_FILLED',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp600_pusup300_trk40_hmt_L1RD0_FILLED',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp900_pusup350_trk50_hmt_L1RD0_FILLED',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp900_pusup400_trk60_hmt_L1RD0_FILLED',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1000_pusup450_trk70_hmt_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1100_pusup450_trk70_hmt_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1200_pusup500_trk80_hmt_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1400_pusup550_trk90_hmt_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1600_pusup600_trk100_hmt_L1RD0_FILLED',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp1800_pusup600_trk110_hmt_L1RD0_FILLED',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp2100_pusup700_trk120_hmt_L1RD0_FILLED',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp2300_pusup1000_trk130_hmt_L1RD0_FILLED',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sp15_pusup0_trk5_hmt_L1RD0_FILLED',          l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup+['PS:Online']+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_L1MBTS_1', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online',MinBiasGroup]+LowMuGroup),

        ChainProp(name='HLT_mb_sptrk_pt2_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt2_L1MBTS_2', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt4_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt4_L1MBTS_2', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt6_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt6_L1MBTS_2', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt8_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt8_L1MBTS_2', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt2_L1AFP_A_OR_C', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt4_L1AFP_A_OR_C', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt6_L1AFP_A_OR_C', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt8_L1AFP_A_OR_C', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt2_L1AFP_A_AND_C', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt4_L1AFP_A_AND_C', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt6_L1AFP_A_AND_C', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt8_L1AFP_A_AND_C', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),

        #noalg chains
        #ChainProp(name='HLT_noalg_L1AFP_A_AND_C_TOF_J50', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        # ChainProp(name='HLT_mb_noalg_L1AFP_A_AND_C_TOF_TOT1_J50', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        # ChainProp(name='HLT_mb_noalg_L1AFP_A_AND_C_TOF_J75', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        # ChainProp(name='HLT_mb_noalg_L1AFP_A_AND_C_TOF_TOT1_J75', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),

        # afprec chains
        ChainProp(name='HLT_mb_afprec_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1CEP-CjJ60', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1CEP-CjJ50', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_J20', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_J20', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_J30', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_J30', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_J50', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_J50', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_J75', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_J75', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_vetombts2in_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
    ]

    TriggerFlags.CalibSlice.signatures     =    TriggerFlags.CalibSlice.signatures() + [
        ChainProp(name='HLT_noalg_AlfaPEB_L1ALFA_ANY', l1SeedThresholds=['FSNOSEED'], stream=['ALFACalib'], groups=['RATE:ALFACalibration','BW:Detector']+LowMuGroup),
        # Calib Chains
        ChainProp(name='HLT_larpsallem_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_larpsall_L1J15', l1SeedThresholds=['J15'], stream=['CosmicCalo'],groups=['RATE:Calibration','BW:Detector']),
    ]
    TriggerFlags.CosmicSlice.signatures    = TriggerFlags.CosmicSlice.signatures() + [
    ]

    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
        #ChainProp(name='HLT_noalg_1RD2_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup),
        #ChainProp(name='HLT_noalg_L1ZB', l1SeedThresholds=['FSNOSEED'], stream=['ZeroBias'], groups=ZeroBiasGroup),
        ChainProp(name='HLT_noalg_L1All', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['Primary:CostAndRate', 'RATE:SeededStreamers', 'BW:Other']), # ATR-22072, for rates in MC. To move to MC menu once good nightly in LS2_v1.

        #Phase-I
        ChainProp(name='HLT_noalg_L1eTAU8',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eTAU12',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jTAU12',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jTAU12M',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1cTAU12M',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eTAU12L',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eTAU12M',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eTAU20',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1cTAU20M',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eTAU25',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1cTAU25M',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eTAU30H',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eTAU40',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eTAU60',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eTAU100',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauPhaseIStreamersGroup),

        ChainProp(name='HLT_noalg_L1eEM3',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM5',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM8',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM8L',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM8M',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM10L',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM15',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM15L',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM15M',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM20',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM20L',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM20M',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM22',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM22L',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM22M',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1eEM22T',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),

        ChainProp(name='HLT_noalg_L1jEM15',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jEM15M',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jEM18M',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaPhaseIStreamersGroup),

        ChainProp(name='HLT_noalg_L1jJ12',          l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ12p0ETA25',   l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ15',          l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ15p0ETA25',   l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ15p31ETA49',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ20',          l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ20p31ETA49',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ25',          l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ25p0ETA23',   l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ30',          l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ30p31ETA49',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ35p0ETA23',   l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ40',          l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ40p0ETA25',   l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ45p0ETA20',   l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ50',          l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ50p31ETA49',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ75',          l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ75p31ETA49',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ85',          l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ100',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ120',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jJ400',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup+PrimaryPhIGroup),

        ChainProp(name='HLT_noalg_L1jLJ80',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jLJ100',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jLJ140',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jLJ160',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetPhaseIStreamersGroup),

        ChainProp(name='HLT_noalg_L1jXE30',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jXE35',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jXE40',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jXE50',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jXE55',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jXE300',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1gXE30',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1gXE40',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1gXE50',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1gXERHO30',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1gXERHO50',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1gXEPUFIT30',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1gXEPUFIT50',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),

        ChainProp(name='HLT_noalg_L1jXEC50',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1gTE50',         l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jTE100',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jTEC100',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jTEFWD100',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jTEFWDA100',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),
        ChainProp(name='HLT_noalg_L1jTEFWDC100',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),

        # ATR-24037
        ChainProp(name='HLT_noalg_L1jXEPerf50',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METPhaseIStreamersGroup),

    ]

    TriggerFlags.MonitorSlice.signatures   = TriggerFlags.MonitorSlice.signatures() + [
       ChainProp(name='HLT_noalg_CSCPEB_L1All', l1SeedThresholds=['FSNOSEED'], stream=['CSC'], groups=['RATE:Monitoring','BW:Other']),
       ChainProp(name='HLT_l1topodebug_legacy_L1All', l1SeedThresholds=['FSNOSEED'], stream=['L1TopoMismatches'], groups=['PS:Online', 'RATE:Monitoring', 'BW:Other']),
    ]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = TriggerFlags.EnhancedBiasSlice.signatures() + [
        ChainProp(name='HLT_eb_low_L1RD2_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_eb_medium_L1RD2_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),

        ChainProp(name='HLT_noalg_L1PhysicsHigh_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_L1PhysicsVeryHigh_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),

        ChainProp(name='HLT_noalg_L1RD3_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_L1RD3_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),

        ChainProp(name='HLT_noalg_L1EMPTY_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_L1FIRSTEMPTY_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_L1UNPAIRED_ISO_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_L1UNPAIRED_NONISO_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] ),
        ChainProp(name='HLT_noalg_L1ABORTGAPNOTCALIB_noPS', l1SeedThresholds=['FSNOSEED'], stream=['EnhancedBias'], groups= ["RATE:EnhancedBias", "BW:Detector"] )
    ]


    TriggerFlags.UnconventionalTrackingSlice.signatures = TriggerFlags.UnconventionalTrackingSlice.signatures() + [
        #Isolated High Pt Trigger Test chain for optimisation studies
        ChainProp(name='HLT_unconvtrk50_isohpttrack_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED']),


        ChainProp(name='HLT_unconvtrk0_fslrt_L1J100', groups=SingleJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk0_fslrt_L14J15', groups=MultiJetGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk0_fslrt_L1XE50', groups=SingleMETGroup, l1SeedThresholds=['FSNOSEED']),

        # hit-based DV
        ChainProp(name='HLT_unconvtrk260_hitdv_tight_L1J100',   groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk260_hitdv_medium_L1J100',  groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']),

        # disappearing track trigger
        ChainProp(name='HLT_unconvtrk20_distrk_tight_L1XE50',               groups=SupportLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk20_distrk_medium_L1XE50',              groups=SupportLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']),

    ]

