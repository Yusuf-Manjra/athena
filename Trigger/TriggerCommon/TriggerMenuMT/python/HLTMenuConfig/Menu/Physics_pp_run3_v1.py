# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# Physics_pp_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
from .ChainDefInMenu import ChainProp
from .SignatureDicts import ChainStore

PhysicsStream='Main'
SingleMuonGroup = ['RATE:SingleMuon', 'BW:Muon']
MultiMuonGroup = ['RATE:MultiMuon', 'BW:Muon']
SingleElectronGroup = ['RATE:SingleElectron', 'BW:Electron']
MultiElectronGroup = ['RATE:MultiElectron', 'BW:Electron']
SinglePhotonGroup = ['RATE:SinglePhoton', 'BW:Photon']
MultiPhotonGroup = ['RATE:MultiPhoton', 'BW:Photon']
SingleMETGroup = ['RATE:MET', 'BW:MET']
MultiMETGroup = ['RATE:MultiMET', 'BW:MultiMET']
SingleJetGroup = ['RATE:SingleJet', 'BW:Jet']
MultiJetGroup = ['RATE:MultiJet', 'BW:Jet']
SingleBjetGroup = ['RATE:SingleBJet', 'BW:BJet']
MultiBjetGroup  = ['RATE:MultiBJet',  'BW:BJet']
EgammaBjetGroup = ['RATE:EgammaBjet', 'BW:BJet']
MuonBjetGroup = ['RATE:MuonBjet', 'BW:BJet']
BjetMETGroup = ['RATE:BjetMET', 'BW:BJet']
SingleTauGroup = ['RATE:SingleTau', 'BW:Tau']
MultiTauGroup = ['RATE:MultiTau', 'BW:Tau']
BphysicsGroup = ['RATE:Bphysics', 'BW:Bphysics']
BphysElectronGroup = ['RATE:BphysicsElectron', 'BW:BphysicsElectron']
EgammaMuonGroup = ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon']
EgammaMETGroup = ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET']
MuonJetGroup =['RATE:MuonJet','BW:Muon', 'BW:Jet']
TauMETGroup =['RATE:TauMET', 'BW:Tau']
TauJetGroup =['RATE:TauJet', 'BW:Tau']
MuonMETGroup =['RATE:MuonMET', 'BW:Muon']
EgammaJetGroup = ['RATE:EgammaJet', 'BW:Egamma']
EgammaTauGroup =['RATE:EgammaTau', 'BW:Egamma', 'BW:Tau']
MuonTauGroup =['RATE:MuonTau', 'BW:Muon', 'BW:Tau']
JetMETGroup = ['RATE:JetMET', 'BW:Jet']
MinBiasGroup = ['RATE:MinBias', 'BW:MinBias']
ZeroBiasGroup = ['RATE:ZeroBias', 'BW:ZeroBias']
EgammaStreamersGroup = ['RATE:SeededStreamers', 'BW:Egamma']
EgammaPhaseIStreamersGroup = ['RATE:PhaseISeededStreamers', 'BW:Egamma']
TauStreamersGroup = ['RATE:SeededStreamers', 'BW:Tau']
TauPhaseIStreamersGroup = ['RATE:PhaseISeededStreamers', 'BW:Tau']
JetStreamersGroup = ['RATE:SeededStreamers', 'BW:Jet']
JetPhaseIStreamersGroup = ['RATE:PhaseISeededStreamers', 'BW:Jet']
METStreamersGroup = ['RATE:SeededStreamers', 'BW:MET']
METPhaseIStreamersGroup = ['RATE:PhaseISeededStreamers', 'BW:MET']
BCIDmonGroup = ['MON:BCID']
# For chains seeded by L1 muon (no calo items)
PrimaryL1MuGroup = ['Primary:L1Muon']
# For chains containing a legacy L1 calo / topo item
PrimaryLegGroup = ['Primary:Legacy']
# For chains containing a phase 1 calo / topo item
PrimaryPhIGroup = ['Primary:PhaseI']
SupportGroup = ['Support']
SupportLegGroup = ['Support:Legacy']
SupportPhIGroup = ['Support:PhaseI']
# For the chains with the TAgAndProbe labels, wewe flag the rate group as being that of the tag leg and NOT the full chain selection
TagAndProbeGroup = ['Support:TagAndProbe']
TagAndProbeLegGroup = ['Support:LegacyTagAndProbe']
TagAndProbePhIGroup = ['Support:PhaseITagAndProbe']
# to identify all chains needed from low mu menu (to be removed once that menu is created and chains moved)
LowMuGroup = ['Primary:LowMu']
EOFBPhysL1MuGroup = ['EOF:BPhysL1Muon']
EOFTLALegGroup = ['EOF:TLALegacy']
# For unconventional tracking chains (ATR-23797)
UnconvTrkGroup = ['RATE:UnconvTrk', 'BW:UnconvTrk'] 

def setupMenu():

    from AthenaCommon.Logging import logging
    log = logging.getLogger( __name__ )
    log.info('[setupMenu] going to add the Physics menu chains now')

    chains = ChainStore()
    chains['Muon'] = [
        #ATR-20049
        ChainProp(name='HLT_2mu6_L12MU5VF',     l1SeedThresholds=['MU5VF'],   groups=SupportGroup+MultiMuonGroup),
        #Planned Primaries
        #-- 1 mu iso
        ChainProp(name='HLT_mu24_ivarmedium_L1MU14FCH', groups=SupportGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu26_ivarmedium_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup, monGroups=['muonMon:shifter','idMon:t0']),
        ChainProp(name='HLT_mu28_ivarmedium_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),
        #-- 1 mu
        ChainProp(name='HLT_mu6_idperf_L1MU5VF', groups=SupportGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu26_L1MU14FCH', groups=SupportGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu24_idperf_L1MU14FCH', groups=SupportGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu50_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu60_0eta105_msonly_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu60_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu80_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu80_msonly_3layersEC_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),

        #-- 2 mu
        ChainProp(name='HLT_mu22_mu8noL1_L1MU14FCH',  l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu22_mu10noL1_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu24_mu8noL1_L1MU14FCH',  l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_2mu14_L12MU8F', groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_2mu15_L12MU8F', groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_ivarmedium_mu8noL1_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        #ATR-22107
        ChainProp(name='HLT_mu20_ivarmedium_mu4noL1_10invmAB70_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),

        #-- 2 mu iso invm
        ChainProp(name='HLT_mu10_ivarmedium_mu10_10invmAB70_L12MU8F', groups=PrimaryL1MuGroup+MultiMuonGroup), 
        #-- 3 mu
        ChainProp(name='HLT_mu20_2mu4noL1_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu22_2mu4noL1_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_3mu6_L13MU5VF', l1SeedThresholds=['MU5VF'],   groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_3mu6_msonly_L13MU5VF', l1SeedThresholds=['MU5VF'],   groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_3mu8_msonly_L13MU5VF', groups=PrimaryL1MuGroup+MultiMuonGroup),
        #-- 4 mu
        ChainProp(name='HLT_4mu4_L14MU3V', l1SeedThresholds=['MU3V'],   groups=PrimaryL1MuGroup+MultiMuonGroup),

        # -- LRT mu
        ChainProp(name='HLT_mu24_LRT_d0loose_L1MU14FCH',  groups=PrimaryL1MuGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu24_LRT_d0tight_L1MU14FCH',  groups=PrimaryL1MuGroup+SingleMuonGroup), #back-up
        ChainProp(name='HLT_mu24_LRT_idperf_L1MU14FCH',   groups=SupportGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu20_msonly_L1MU14FCH',       groups=SupportGroup+SingleMuonGroup),
     ]

    chains['Egamma'] = [
        # Electron Chains----------
        #--------- primary 1e
        ChainProp(name='HLT_e26_lhtight_ivarloose_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e60_lhmedium_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup), 
        ChainProp(name='HLT_e140_lhloose_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup), 
        ChainProp(name='HLT_e300_etcut_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup), 

        #---------- primary 2e 
        ChainProp(name='HLT_2e17_lhvloose_L12EM15VHI', groups=PrimaryLegGroup+MultiElectronGroup),
        ChainProp(name='HLT_2e24_lhvloose_L12EM20VH', groups=PrimaryLegGroup+MultiElectronGroup), 

        #---------- primary 3e
        ChainProp(name='HLT_e24_lhvloose_2e12_lhvloose_L1EM20VH_3EM10VH',l1SeedThresholds=['EM20VH','EM10VH'], groups=PrimaryLegGroup+MultiElectronGroup), 

        #--------- primary Zee TnP
        ChainProp(name='HLT_e26_lhtight_e15_etcut_Zee_L1EM22VHI', l1SeedThresholds=['EM22VHI','EM7'], groups=PrimaryLegGroup+MultiElectronGroup), 

        #--------- primary special
        ChainProp(name='HLT_e20_lhtight_ivarloose_L1ZAFB-25DPHI-eEM15M', l1SeedThresholds=['eEM15M'], groups=PrimaryPhIGroup+SingleElectronGroup), 

        #--------- support Zee and J/psi TnP
        ChainProp(name='HLT_e26_lhtight_e15_etcut_idperf_Zee_L1EM22VHI', l1SeedThresholds=['EM22VHI','EM7'], groups=PrimaryLegGroup+MultiElectronGroup),  

        ChainProp(name='HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7', l1SeedThresholds=['EM7','EM3'], groups=SupportLegGroup+MultiElectronGroup), 
        ChainProp(name='HLT_e5_lhtight_e9_etcut_Jpsiee_L1JPSI-1M5-EM7', l1SeedThresholds=['EM3','EM7'], groups=SupportLegGroup+MultiElectronGroup), 
        ChainProp(name='HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12', l1SeedThresholds=['EM12','EM3'], groups=SupportLegGroup+MultiElectronGroup), 
        ChainProp(name='HLT_e5_lhtight_e14_etcut_Jpsiee_L1JPSI-1M5-EM12', l1SeedThresholds=['EM3','EM12'], groups=SupportLegGroup+MultiElectronGroup), 
        ChainProp(name='HLT_e9_lhtight_noringer_e4_etcut_Jpsiee_L1JPSI-1M5-EM7', l1SeedThresholds=['EM7','EM3'], groups=SupportLegGroup+MultiElectronGroup), 
        ChainProp(name='HLT_e5_lhtight_noringer_e9_etcut_Jpsiee_L1JPSI-1M5-EM7', l1SeedThresholds=['EM3','EM7'], groups=SupportLegGroup+MultiElectronGroup), 
        ChainProp(name='HLT_e14_lhtight_noringer_e4_etcut_Jpsiee_L1JPSI-1M5-EM12', l1SeedThresholds=['EM12','EM3'], groups=SupportLegGroup+MultiElectronGroup), 
        ChainProp(name='HLT_e5_lhtight_noringer_e14_etcut_Jpsiee_L1JPSI-1M5-EM12', l1SeedThresholds=['EM3','EM12'], groups=SupportLegGroup+MultiElectronGroup), 

        #------------ support validation of tag-and-probe mass cuts
        ChainProp(name='HLT_e26_lhtight_e15_etcut_L1EM22VHI', l1SeedThresholds=['EM22VHI','EM7'], groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e9_lhtight_e4_etcut_L1JPSI-1M5-EM7', l1SeedThresholds=['EM7','EM3'], groups=SupportLegGroup+SingleElectronGroup),

        #------------ support alternative lowest unprescaled 1e
        ChainProp(name='HLT_e24_lhtight_ivarloose_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),

        #------------ support noringer of primary 1e
        ChainProp(name='HLT_e26_lhtight_ivarloose_noringer_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e60_lhmedium_noringer_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e140_lhloose_noringer_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),

        #------------ support bootstrap
        ChainProp(name='HLT_e50_etcut_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e120_etcut_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e250_etcut_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),

        #------------ support background studies
        ChainProp(name='HLT_e10_lhvloose_L1EM7', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e15_lhvloose_L1EM10VH', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e20_lhvloose_L1EM15VH', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e30_lhvloose_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e40_lhvloose_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e60_lhvloose_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e80_lhvloose_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e100_lhvloose_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e120_lhvloose_L1EM22VHI', groups=SupportLegGroup+SingleElectronGroup),

        # Photon Chains----------
        #----------- primary 1g
        ChainProp(name='HLT_g140_loose_L1EM22VHI', groups=PrimaryLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g300_etcut_L1EM22VHI', groups=PrimaryLegGroup+SinglePhotonGroup), 

        #----------- primary 2g
        ChainProp(name='HLT_2g20_tight_icaloloose_L12EM15VHI', groups=PrimaryLegGroup+MultiPhotonGroup), 
        ChainProp(name='HLT_2g22_tight_L12EM15VHI', groups=PrimaryLegGroup+MultiPhotonGroup), 
        ChainProp(name='HLT_g35_medium_g25_medium_L12EM20VH', l1SeedThresholds=['EM20VH','EM20VH'], groups=PrimaryLegGroup+MultiPhotonGroup), 
        ChainProp(name='HLT_2g50_loose_L12EM20VH', groups=PrimaryLegGroup+MultiPhotonGroup), 

        #------------ primary 3g
        ChainProp(name='HLT_2g25_loose_g15_loose_L12EM20VH',l1SeedThresholds=['EM20VH','EM10VH'], groups=PrimaryLegGroup+MultiPhotonGroup), 

        #------------ primary special
        ChainProp(name='HLT_2g15_tight_dPhi25_L1DPHI-M70-2eEM12M', l1SeedThresholds=['eEM10L'], groups=PrimaryPhIGroup+MultiPhotonGroup), # TODO: mismatch between L1topo threshold and L1 seed to be fixed

        #------------ support legs of multi-photons
        ChainProp(name='HLT_g25_medium_L1EM20VH', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g35_medium_L1EM20VH', groups=SupportLegGroup+SinglePhotonGroup), 

        ChainProp(name='HLT_g20_tight_icaloloose_L1EM15VHI', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g15_tight_L1EM10VH', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g20_tight_L1EM15VHI', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g22_tight_L1EM15VHI', groups=SupportLegGroup+SinglePhotonGroup), 

        ChainProp(name='HLT_2g15_tight_L1DPHI-M70-2eEM12M', l1SeedThresholds=['eEM10L'], groups=SupportPhIGroup+SinglePhotonGroup), # TODO: mismatch between L1topo threshold and L1 seed to be fixed 
        
        #------------ support bootstrap and background studies
        ChainProp(name='HLT_g250_etcut_L1EM22VHI', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g10_loose_L1EM7', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g15_loose_L1EM10VH', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g20_loose_L1EM15VH', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g25_loose_L1EM20VH', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g30_loose_L1EM20VH', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g40_loose_L1EM20VH', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g50_loose_L1EM20VH', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g60_loose_L1EM22VHI', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g80_loose_L1EM22VHI', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g100_loose_L1EM22VHI', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g120_loose_L1EM22VHI', groups=SupportLegGroup+SinglePhotonGroup), 

        #------- Electron+Photon Chains
        # primary e-g chains: electron + photon stay in the same step - these need to be parallel merged!
        ChainProp(name='HLT_e24_lhmedium_g25_medium_02dRAB_L12EM20VH', l1SeedThresholds=['EM20VH','EM20VH'], stream=[PhysicsStream], groups=PrimaryLegGroup+MultiElectronGroup),
        ChainProp(name='HLT_e24_lhmedium_g12_loose_g12_loose_02dRAB_02dRAC_L1EM20VH_3EM10VH', l1SeedThresholds=['EM20VH','EM10VH','EM10VH'], stream=[PhysicsStream], groups=PrimaryLegGroup+MultiElectronGroup), # unsure about l1SeedThresholds

        # Electron LRT chains
        ChainProp(name='HLT_e26_lhloose_nopix_lrttight_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhmedium_nopix_lrttight_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup), #back-up

    ]

    chains['MET'] = [
        ChainProp(name='HLT_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+SingleMETGroup),
        ChainProp(name='HLT_xe75_cell_xe65_tcpufit_xe90_trkmht_L1XE50', l1SeedThresholds=['FSNOSEED']*3, groups=PrimaryLegGroup+SingleMETGroup),
        ChainProp(name='HLT_xe60_cell_xe95_pfsum_cssk_L1XE50', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+SingleMETGroup),
        ChainProp(name='HLT_xe55_cell_xe70_tcpufit_xe90_pfsum_vssk_L1XE50', l1SeedThresholds=['FSNOSEED']*3, groups=PrimaryLegGroup+SingleMETGroup),
        ChainProp(name='HLT_xe65_cell_xe105_mhtpufit_em_subjesgscIS_L1XE50', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+SingleMETGroup),
        ChainProp(name='HLT_xe65_cell_xe100_mhtpufit_pf_subjesgscIS_L1XE50', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+SingleMETGroup),
        ChainProp(name='HLT_xe55_cell_xe70_tcpufit_xe95_pfsum_cssk_L1XE50', l1SeedThresholds=['FSNOSEED']*3, groups=PrimaryLegGroup+SingleMETGroup),
        ChainProp(name='HLT_xe65_cell_xe95_pfsum_vssk_L1XE50', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+SingleMETGroup),

        # MultiMET Chain
        ChainProp(name='HLT_xe65_cell_xe110_tcpufit_L1XE50',l1SeedThresholds=['FSNOSEED']*2, groups=SupportLegGroup+MultiMETGroup), # Intended PS=-1
        ChainProp(name='HLT_xe80_cell_xe115_tcpufit_L1XE50',l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiMETGroup),
    ]

    chains['Jet'] = [
        # Support performance chains (for emulation+calibration studies) ATR-20624
        ChainProp(name='HLT_j0_perf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=['PS:Online']+SingleJetGroup+SupportLegGroup+['RATE:CPS_RD0FILLED']),
        ChainProp(name='HLT_j0_perf_pf_ftf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=['PS:Online']+SingleJetGroup+SupportLegGroup+['RATE:CPS_RD0FILLED']),

        # Central single small-R jet chains
        ChainProp(name='HLT_j15_pf_ftf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_RD0FILLED']),
        ChainProp(name='HLT_j25_pf_ftf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_RD0FILLED']),
        ChainProp(name='HLT_j35_pf_ftf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_RD0FILLED']),
        ChainProp(name='HLT_j45_pf_ftf_preselj20_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_RD0FILLED']),
        ChainProp(name='HLT_j45_pf_ftf_preselj20_L1J15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j60_pf_ftf_preselj20_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j85_pf_ftf_preselj20_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j110_pf_ftf_preselj20_L1J30', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j175_pf_ftf_preselj20_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j260_pf_ftf_preselj20_L1J75', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j360_pf_ftf_preselj20_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j380_pf_ftf_preselj20_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j400_pf_ftf_preselj20_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j420_pf_ftf_preselj135_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j440_pf_ftf_preselj135_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j450_pf_ftf_preselj135_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j460_pf_ftf_preselj135_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j480_pf_ftf_preselj135_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j500_pf_ftf_preselj135_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j520_pf_ftf_preselj135_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        # Central single large-R jet chains
        ChainProp(name='HLT_j110_a10sd_cssk_pf_jes_ftf_L1J30', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j110_a10t_lcw_jes_L1J30', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j175_a10t_lcw_jes_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j260_a10sd_cssk_pf_jes_ftf_L1J75', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j260_a10t_lcw_jes_L1J75', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j360_a10t_lcw_jes_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j360_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j400_a10sd_cssk_pf_jes_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j400_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j400_a10t_lcw_jes_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j400_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j420_a10t_lcw_jes_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j420_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_jes_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j460_a10t_lcw_jes_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j460_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j480_a10sd_cssk_pf_jes_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j480_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j480_a10t_lcw_jes_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j480_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        #Low-threshold large-R chains (for calibration purposes)
        ChainProp(name='HLT_j85_a10sd_cssk_pf_nojcalib_ftf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j85_a10sd_cssk_pf_jes_ftf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j85_a10t_lcw_nojcalib_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j85_a10t_lcw_jes_L1J20',      l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),

        #Forward small-R EMTopo chains 
        ChainProp(name='HLT_j15_320eta490_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_RD0FILLED']),
        ChainProp(name='HLT_j25_320eta490_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_RD0FILLED']),
        ChainProp(name='HLT_j35_320eta490_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_RD0FILLED']),
        ChainProp(name='HLT_j45_320eta490_L1J15p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j60_320eta490_L1J20p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j85_320eta490_L1J20p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j110_320eta490_L1J30p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j175_320eta490_L1J50p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j280_320eta490_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j300_320eta490_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),

        # ATR-20049
        ChainProp(name='HLT_j420_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup + BCIDmonGroup),
        ChainProp(name='HLT_j260_320eta490_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10_lcw_subjes_L1SC111-CJ15',         l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1J100', l1SeedThresholds=['FSNOSEED'],  groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'],  groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10_lcw_subjes_L1J100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j420_a10t_lcw_jes_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_jes_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),

        ChainProp(name='HLT_j420_a10t_lcw_jes_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_jes_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j360_a10t_lcw_jes_60smcINF_j360_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_j370_a10t_lcw_jes_35smcINF_j370_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_60smcINF_j360_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_j370_a10sd_cssk_pf_jes_ftf_35smcINF_j370_a10sd_cssk_pf_jes_ftf_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiJetGroup),
 
        # Small-R multijet chains
        # PFlow primaries
        ChainProp(name='HLT_3j200_pf_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_2j250_ftf_0eta240_j120_ftf_0eta240_L1J100',       l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_2j250_pf_ftf_0eta240_j120_pf_ftf_0eta240_L1J100', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_4j115_pf_ftf_presel4j33_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j70_pf_ftf_0eta240_presel5j24_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j85_pf_ftf_presel5j24_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j55_pf_ftf_0eta240_presel6j36_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j70_pf_ftf_presel6j36_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_7j45_pf_ftf_presel7j21_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_10j40_pf_ftf_presel7j21_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        # EMTopo backups
        ChainProp(name='HLT_3j200_L1J100', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_4j120_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j70_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j85_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j55_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j70_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_7j45_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_10j40_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),

        # FP: workaround tmp for l1SeedThresholds
        ChainProp(name='HLT_j80_0eta240_2j60_320eta490_j0_DJMASS700j80x0eta240_L1J20', l1SeedThresholds=['FSNOSEED']*3, groups=MultiJetGroup),

        #HT chains
        ChainProp(name='HLT_j0_HT1000_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_HT1000_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
    ]

    chains['Bjet'] = [
        # leave the MV2 chain for now since we don't have a 40% DL1r working point
        ChainProp(name="HLT_j225_0eta290_020jvt_pf_ftf_bmv2c1040_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup),

        # these chains are taken from the Run 2 menu for now --- likely to be loosened
        ChainProp(name="HLT_j275_0eta290_020jvt_pf_ftf_bdl1r60_L1J100", l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleBjetGroup),
        ChainProp(name="HLT_j300_0eta290_020jvt_pf_ftf_bdl1r70_L1J100", l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleBjetGroup),
        ChainProp(name="HLT_j360_0eta290_020jvt_pf_ftf_bdl1r77_L1J100", l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleBjetGroup),


        ChainProp(name='HLT_j30_0eta290_020jvt_pf_ftf_boffperf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j45_0eta290_020jvt_pf_ftf_boffperf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j60_0eta290_020jvt_pf_ftf_boffperf_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j80_0eta290_020jvt_pf_ftf_boffperf_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j100_0eta290_020jvt_pf_ftf_boffperf_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j150_0eta290_020jvt_pf_ftf_boffperf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j200_0eta290_020jvt_pf_ftf_boffperf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j300_0eta290_020jvt_pf_ftf_boffperf_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),

        # HH4b primary candidates with 2 sets of potential jet thresholds
        # 3b85 symmetric b-jet pt for Physics_Main
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_3j20_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_3j25_pf_ftf_0eta240_020jvt_bdl1r85_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=SupportLegGroup+MultiBjetGroup),
        # 2b60 asymmetric b-jet pt alternative for Physics_Main
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r60_j20_pf_ftf_0eta240_020jvt_bdl1r60_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=SupportLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j35_pf_ftf_0eta240_020jvt_bdl1r60_j25_pf_ftf_0eta240_020jvt_bdl1r60_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=SupportLegGroup+MultiBjetGroup),
        # 2b77 symmetric b-jet pt for VBFDelayed
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_2j20_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_2j25_pf_ftf_0eta240_020jvt_bdl1r77_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=SupportLegGroup+MultiBjetGroup),

    ]

    chains['Tau'] = [
        #ATR-20049
        ChainProp(name='HLT_tau160_mediumRNN_tracktwoMVA_L1TAU100', groups=SupportLegGroup+SingleTauGroup),
        ChainProp(name='HLT_tau160_mediumRNN_tracktwoMVABDT_L1TAU100', groups=PrimaryLegGroup+SingleTauGroup), 
        ChainProp(name='HLT_tau200_mediumRNN_tracktwoMVA_L1TAU100', groups=SupportLegGroup+SingleTauGroup),
        ChainProp(name='HLT_tau200_mediumRNN_tracktwoMVABDT_L1TAU100', groups=PrimaryLegGroup+SingleTauGroup),

        # displaced tau (ATR-21754)
        ChainProp(name="HLT_tau180_mediumRNN_tracktwoLLP_L1TAU100", groups=PrimaryLegGroup+SingleTauGroup),   

        # ATR-21797
        ChainProp(name='HLT_tau80_medium1_tracktwo_tau60_medium1_tracktwo_03dRAB_L1TAU60_2TAU40',                        l1SeedThresholds=['TAU60','TAU40'],     groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVA_tau60_mediumRNN_tracktwoMVA_03dRAB_L1TAU60_2TAU40',                  l1SeedThresholds=['TAU60','TAU40'],     groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVABDT_tau60_mediumRNN_tracktwoMVABDT_03dRAB_L1TAU60_2TAU40',            l1SeedThresholds=['TAU60','TAU40'],     groups=PrimaryLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau80_medium1_tracktwo_tau35_medium1_tracktwo_03dRAB30_L1TAU60_DR-TAU20ITAU12I',             l1SeedThresholds=['TAU60','TAU12IM'],   groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA_03dRAB30_L1TAU60_DR-TAU20ITAU12I',       l1SeedThresholds=['TAU60','TAU12IM'],   groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVABDT_tau35_mediumRNN_tracktwoMVABDT_03dRAB30_L1TAU60_DR-TAU20ITAU12I', l1SeedThresholds=['TAU60','TAU12IM'],   groups=PrimaryLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dRAB30_L1DR-TAU20ITAU12I-J25',               l1SeedThresholds=['TAU20IM','TAU12IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dRAB30_L1DR-TAU20ITAU12I-J25',         l1SeedThresholds=['TAU20IM','TAU12IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_03dRAB30_L1DR-TAU20ITAU12I-J25',   l1SeedThresholds=['TAU20IM','TAU12IM'], groups=PrimaryLegGroup+MultiTauGroup),
        
        # ATR-20450 
        ChainProp(name='HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dRAB_L1TAU20IM_2TAU12IM_4J12p0ETA25',          l1SeedThresholds=['TAU20IM','TAU12IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dRAB_L1TAU20IM_2TAU12IM_4J12p0ETA25',    l1SeedThresholds=['TAU20IM','TAU12IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1TAU20IM_2TAU12IM_4J12p0ETA25', l1SeedThresholds=['TAU20IM','TAU12IM'], groups=PrimaryLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau40_medium1_tracktwo_tau35_medium1_tracktwo_03dRAB_L1TAU25IM_2TAU20IM_2J25_3J20',            l1SeedThresholds=['TAU25IM','TAU20IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau40_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA_03dRAB_L1TAU25IM_2TAU20IM_2J25_3J20',      l1SeedThresholds=['TAU25IM','TAU20IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau40_mediumRNN_tracktwoMVABDT_tau35_mediumRNN_tracktwoMVABDT_03dRAB_L1TAU25IM_2TAU20IM_2J25_3J20',   l1SeedThresholds=['TAU25IM','TAU20IM'], groups=PrimaryLegGroup+MultiTauGroup),

        # displaced tau+X (ATR-21754)
        ChainProp(name="HLT_tau80_mediumRNN_tracktwoLLP_tau60_mediumRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40",                  l1SeedThresholds=['TAU60','TAU40'],     groups=PrimaryLegGroup+MultiTauGroup), # <-- for physics

    ]

    chains['Bphysics'] = [
        #ATR-20049, dimuon primary triggers
        ChainProp(name='HLT_2mu10_bJpsimumu_L12MU8F', stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup ),
        ChainProp(name='HLT_mu11_mu6_bJpsimumu_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup ),
        ChainProp(name='HLT_2mu10_bUpsimumu_L12MU8F', stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup ),
        ChainProp(name='HLT_mu11_mu6_bUpsimumu_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup ),
        ChainProp(name='HLT_mu11_mu6_bBmumu_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup ),
        ChainProp(name='HLT_mu11_mu6_bDimu_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup ),
        ChainProp(name='HLT_mu11_mu6_bDimu2700_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup ),
        ChainProp(name='HLT_mu11_mu6_bPhi_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup ),
        ChainProp(name='HLT_mu11_mu6_bTau_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup ),
        #ATR-21639, Bmumux primary triggers
        ChainProp(name='HLT_mu11_mu6_bBmumux_BpmumuKp_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumux_BcmumuPi_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumux_BsmumuPhi_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumux_BdmumuKst_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumux_LbPqKm_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        #ATR-22223, multi muon primary triggers
        ChainProp(name='HLT_3mu4_bJpsi_L13MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu4_bUpsi_L13MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu4_bTau_L13MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu4_bPhi_L13MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu6_bDimu_L13MU5VF', stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu6_2mu4_bDimu2700_L1MU5VF_3MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu6_2mu4_bDimu6000_L1MU5VF_3MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_4mu4_bDimu6000_L14MU3V', stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
    ]

    chains['Combined'] = [

        # tau + muon triggers
        ChainProp(name='HLT_mu20_ivarloose_tau20_medium1_tracktwo_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu20_ivarloose_tau20_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu20_ivarloose_tau20_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu14_ivarloose_tau35_medium1_tracktwo_03dRAB_L1MU8F_TAU20IM', l1SeedThresholds=['MU8F','TAU20IM'], stream=[PhysicsStream], groups=SupportLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu14_ivarloose_tau35_mediumRNN_tracktwoMVA_03dRAB_L1MU8F_TAU20IM', l1SeedThresholds=['MU8F','TAU20IM'], stream=[PhysicsStream], groups=SupportLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu14_ivarloose_tau35_mediumRNN_tracktwoMVABDT_03dRAB_L1MU8F_TAU20IM', l1SeedThresholds=['MU8F','TAU20IM'], stream=[PhysicsStream], groups=PrimaryLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu14_ivarloose_tau25_medium1_tracktwo_03dRAB_L1MU8F_TAU12IM_3J12', l1SeedThresholds=['MU8F','TAU12IM'], stream=[PhysicsStream], groups=SupportLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_03dRAB_L1MU8F_TAU12IM_3J12', l1SeedThresholds=['MU8F','TAU12IM'], stream=[PhysicsStream], groups=SupportLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu14_ivarloose_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1MU8F_TAU12IM_3J12', l1SeedThresholds=['MU8F','TAU12IM'], stream=[PhysicsStream], groups=PrimaryLegGroup+MuonTauGroup),

        # tau + electron triggers
        ChainProp(name='HLT_e24_lhmedium_ivarloose_tau20_medium1_tracktwo_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_e24_lhmedium_ivarloose_tau20_mediumRNN_tracktwoMVA_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_e24_lhmedium_ivarloose_tau20_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_e17_lhmedium_ivarloose_tau25_medium1_tracktwo_03dRAB_L1EM15VHI_2TAU12IM_4J12', l1SeedThresholds=['EM15VHI','TAU12IM'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_e17_lhmedium_ivarloose_tau25_mediumRNN_tracktwoMVA_03dRAB_L1EM15VHI_2TAU12IM_4J12', l1SeedThresholds=['EM15VHI','TAU12IM'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_e17_lhmedium_ivarloose_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1EM15VHI_2TAU12IM_4J12', l1SeedThresholds=['EM15VHI','TAU12IM'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),

       # tau + met
        ChainProp(name='HLT_tau60_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_xe50_cell_03dRAB_L1TAU40_2TAU12IM_XE40', l1SeedThresholds=['TAU40','TAU12IM','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+TauMETGroup),  # ATR-22966
        ChainProp(name='HLT_e17_lhmedium_tau25_mediumRNN_tracktwoMVABDT_xe50_cell_03dRAB_L1EM15VHI_2TAU12IM_XE35', l1SeedThresholds=['EM15VHI','TAU12IM','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+TauMETGroup),
        ChainProp(name='HLT_mu14_tau25_mediumRNN_tracktwoMVABDT_xe50_cell_03dRAB_L1MU8F_TAU12IM_XE35', l1SeedThresholds=['MU8F','TAU12IM','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+TauMETGroup),
        #
        ChainProp(name='HLT_tau60_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_xe50_cell_03dRAB_L1TAU40_2TAU12IM_XE40', l1SeedThresholds=['TAU40','TAU12IM','FSNOSEED'], stream=[PhysicsStream], groups=SupportLegGroup+TauMETGroup),
        ChainProp(name='HLT_e17_lhmedium_tau25_mediumRNN_tracktwoMVA_xe50_cell_03dRAB_L1EM15VHI_2TAU12IM_XE35', l1SeedThresholds=['EM15VHI','TAU12IM','FSNOSEED'], stream=[PhysicsStream], groups=SupportLegGroup+TauMETGroup),
        ChainProp(name='HLT_mu14_tau25_mediumRNN_tracktwoMVA_xe50_cell_03dRAB_L1MU8F_TAU12IM_XE35', l1SeedThresholds=['MU8F','TAU12IM','FSNOSEED'], stream=[PhysicsStream], groups=SupportLegGroup+TauMETGroup),

        # T&P alignement-based tau chains
       ChainProp(name='HLT_mu26_ivarmedium_tau20_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU8'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau20_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU8'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_idperf_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_idperf_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_perf_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_perf_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau35_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU20IM'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau35_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU20IM'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau40_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU25IM'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau40_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU25IM'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau60_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU40'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau60_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU40'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau80_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU60'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau80_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU60'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau160_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU100'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau160_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU100'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
        # mu-tag & tau-probe triggers for LLP tau (ATR-23150)
        ChainProp(name='HLT_mu26_ivarmedium_tau60_mediumRNN_tracktwoLLP_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU40'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu26_ivarmedium_tau80_mediumRNN_tracktwoLLP_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU60'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu26_ivarmedium_tau180_mediumRNN_tracktwoLLP_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU100'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),

       ChainProp(name='HLT_e26_lhtight_ivarloose_tau20_mediumRNN_tracktwoMVA_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau20_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau25_mediumRNN_tracktwoMVA_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau35_mediumRNN_tracktwoMVA_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU20IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau35_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU20IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau40_mediumRNN_tracktwoMVA_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU25IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau40_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU25IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau60_mediumRNN_tracktwoMVA_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU40'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau60_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU40'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau80_mediumRNN_tracktwoMVA_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU60'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau80_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU60'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau160_mediumRNN_tracktwoMVA_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU100'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
       ChainProp(name='HLT_e26_lhtight_ivarloose_tau160_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU100'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
        # el-tag & tau-probe triggers for LLP tau (ATR-23150)
        ChainProp(name='HLT_e26_lhtight_ivarloose_tau60_mediumRNN_tracktwoLLP_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU40'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_tau80_mediumRNN_tracktwoLLP_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU60'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e26_lhtight_ivarloose_tau180_mediumRNN_tracktwoLLP_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU100'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleElectronGroup),


        # b-jet trigger calibration chains
        ChainProp(name='HLT_e26_lhtight_ivarloose_2j20_0eta290_020jvt_pf_ftf_boffperf_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=TagAndProbeLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_mu26_ivarmedium_2j20_0eta290_020jvt_pf_ftf_boffperf_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=TagAndProbeGroup+SingleMuonGroup),
        ### Affected by ATR-23722
        #ChainProp(name='HLT_e26_lhtight_ivarloose_mu22noL1_j20_0eta290_020jvt_pf_ftf_boffperf_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'], groups=TagAndProbeLegGroup+EgammaBjetGroup),

        #Isolated High pt Track Trigger
        #Primary
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_medium_iaggrmedium_L1XE50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream], groups=UnconvTrkGroup+PrimaryLegGroup),
        #Backup for Primary Triggers
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk140_isohpttrack_medium_iaggrmedium_L1XE50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream], groups=UnconvTrkGroup+PrimaryLegGroup),
        #Support
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk100_isohpttrack_medium_iaggrmedium_L1XE50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream], groups=UnconvTrkGroup+SupportLegGroup),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_medium_iaggrloose_L1XE50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],  groups=UnconvTrkGroup+SupportLegGroup),
    ]
    chains['MinBias'] = [
        ChainProp(name='HLT_mb_sptrk_L1RD0_FILLED',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=LowMuGroup+MinBiasGroup),
        ChainProp(name='HLT_mb_sp_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=LowMuGroup+MinBiasGroup),
        ChainProp(name='HLT_mb_alfaperf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['DISCARD'],     groups=['PS:Online']+LowMuGroup+MinBiasGroup),
        ChainProp(name='HLT_mb_alfaperf_L1RD0_EMPTY',  l1SeedThresholds=['FSNOSEED'], stream=['DISCARD'],     groups=['PS:Online']+LowMuGroup+MinBiasGroup),
    ]

    chains['Monitor'] = [
        ChainProp(name='HLT_noalg_CostMonDS_L1All',        l1SeedThresholds=['FSNOSEED'], stream=['CostMonitoring'], groups=['Primary:CostAndRate', 'RATE:Monitoring', 'BW:Other']), # HLT_costmonitor
    ]

    return chains
