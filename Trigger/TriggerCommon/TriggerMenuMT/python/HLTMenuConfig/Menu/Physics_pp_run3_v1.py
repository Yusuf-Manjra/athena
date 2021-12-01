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
TauPhotonGroup =['RATE:TauPhoton', 'BW:Tau']
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
# For chains seeded by L1 muon (no calo items)
PrimaryL1MuGroup = ['Primary:L1Muon']
# For chains containing a legacy L1 calo / topo item
PrimaryLegGroup = ['Primary:Legacy']
# For chains containing a phase 1 calo / topo item
PrimaryPhIGroup = ['Primary:PhaseI']
SupportGroup = ['Support']
SupportLegGroup = ['Support:Legacy']
SupportPhIGroup = ['Support:PhaseI']
# For the chains with the TAgAndProbe labels, we flag the rate group as being that of the tag leg and NOT the full chain selection
TagAndProbeGroup = ['Support:TagAndProbe']
TagAndProbeLegGroup = ['Support:LegacyTagAndProbe']
TagAndProbePhIGroup = ['Support:PhaseITagAndProbe']
# to identify all chains needed from low mu menu (to be removed once that menu is created and chains moved)
LowMuGroup = ['Primary:LowMu']
EOFL1MuGroup = ['EOF:L1Muon']
EOFBPhysL1MuGroup = ['EOF:BPhysL1Muon']
EOFBeeLegGroup = ['EOF:BeeLegacy']
EOFTLALegGroup = ['EOF:TLALegacy']
EOFTLAPhIGroup = ['EOF:TLAPhaseI']
# For unconventional tracking chains (ATR-23797)
UnconvTrkGroup = ['RATE:UnconvTrk', 'BW:UnconvTrk'] 
# For development chains, not for physics
DevGroup = ['Development']

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
        ChainProp(name='HLT_mu26_ivarmedium_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup, monGroups=['muonMon:shifter','idMon:t0','muonMon:online']),
        ChainProp(name='HLT_mu28_ivarmedium_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),
        #-- 1 mu
        ChainProp(name='HLT_mu6_idperf_L1MU5VF', stream=[PhysicsStream,'express'], groups=SupportGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu26_L1MU14FCH', groups=SupportGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu24_idperf_L1MU14FCH', stream=[PhysicsStream,'express'], groups=SupportGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu50_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup, monGroups=['muonMon:online','muonMon:shifter']),
        ChainProp(name='HLT_mu60_0eta105_msonly_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu60_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu80_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu80_msonly_3layersEC_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup),

        #-- 2 mu
        ChainProp(name='HLT_mu22_mu8noL1_L1MU14FCH',  l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup, monGroups=['muonMon:online']),
        ChainProp(name='HLT_mu22_mu10noL1_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu24_mu8noL1_L1MU14FCH',  l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_2mu14_L12MU8F', groups=PrimaryL1MuGroup+MultiMuonGroup, monGroups=['muonMon:online']),
        ChainProp(name='HLT_2mu15_L12MU8F', groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_ivarmedium_mu8noL1_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        #ATR-22107
        ChainProp(name='HLT_mu20_ivarmedium_mu4noL1_10invmAB70_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),

        #-- 2 mu iso invm
        ChainProp(name='HLT_mu10_ivarmedium_mu10_10invmAB70_L12MU8F', groups=PrimaryL1MuGroup+MultiMuonGroup), 
        #-- 3 mu
        ChainProp(name='HLT_mu20_2mu4noL1_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu22_2mu4noL1_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_3mu6_L13MU5VF', l1SeedThresholds=['MU5VF'],   groups=PrimaryL1MuGroup+MultiMuonGroup, monGroups=['muonMon:online']),
        ChainProp(name='HLT_3mu6_msonly_L13MU5VF', l1SeedThresholds=['MU5VF'],   groups=PrimaryL1MuGroup+MultiMuonGroup, monGroups=['muonMon:online']),
        ChainProp(name='HLT_3mu8_msonly_L13MU5VF', groups=PrimaryL1MuGroup+MultiMuonGroup),
        #-- 4 mu
        ChainProp(name='HLT_4mu4_L14MU3V', l1SeedThresholds=['MU3V'],   groups=PrimaryL1MuGroup+MultiMuonGroup, monGroups=['muonMon:online']),

        # -- LRT mu
        ChainProp(name='HLT_mu24_LRT_d0loose_L1MU14FCH',  groups=PrimaryL1MuGroup+SingleMuonGroup, monGroups=['muonMon:online']),
        ChainProp(name='HLT_mu24_LRT_d0tight_L1MU14FCH',  groups=PrimaryL1MuGroup+SingleMuonGroup), #back-up
        ChainProp(name='HLT_mu24_LRT_idperf_L1MU14FCH',   groups=SupportGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu20_msonly_L1MU14FCH',       groups=SupportGroup+SingleMuonGroup),

        # ATR-20505
        ChainProp(name='HLT_2mu50_msonly_L1MU14FCH', groups=PrimaryL1MuGroup+SingleMuonGroup+PrimaryLegGroup),

        # LATE
        ChainProp(name='HLT_mu10_lateMu_L1LATE-MU8F_J50', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup+PrimaryL1MuGroup+PrimaryLegGroup),
        ChainProp(name='HLT_mu10_lateMu_L1LATE-MU8F_XE30', l1SeedThresholds=['FSNOSEED'], groups=SingleMuonGroup+PrimaryLegGroup),

        #-- nscan ATR-19376
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_J40', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryLegGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH_XE30', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=PrimaryLegGroup+MultiMuonGroup),
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L110DR-MU14FCH-MU5VF', l1SeedThresholds=['MU14FCH','FSNOSEED'],   groups=PrimaryL1MuGroup+MultiMuonGroup),
        ## ATR-24198        
        ChainProp(name='HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=SupportGroup+MultiMuonGroup),

        # Late stream for LLP
        ChainProp(name='HLT_3mu6_msonly_L1MU5VF_EMPTY', l1SeedThresholds=['MU5VF'], stream=['Late'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_3mu6_msonly_L1MU3V_UNPAIRED_ISO', l1SeedThresholds=['MU3V'], stream=['Late'], groups=PrimaryL1MuGroup+MultiMuonGroup),

     ]

    chains['Egamma'] = [
        # Electron Chains----------
        #--------- primary 1e
        ChainProp(name='HLT_e26_lhtight_ivarloose_L1EM22VHI', stream=[PhysicsStream,'express'], groups=PrimaryLegGroup+SingleElectronGroup, monGroups=['egammaMon:online','egammaMon:shifter']),
        ChainProp(name='HLT_e60_lhmedium_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup), 
        ChainProp(name='HLT_e140_lhloose_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup), 
        ChainProp(name='HLT_e300_etcut_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup), 

        # dnn chains
        ChainProp(name='HLT_e26_dnntight_gsf_ivarloose_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e60_dnnmedium_gsf_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e140_dnnloose_gsf_L1EM22VHI', groups=PrimaryLegGroup+SingleElectronGroup),

        #---------- primary 2e 
        ChainProp(name='HLT_2e17_lhvloose_L12EM15VHI', groups=PrimaryLegGroup+MultiElectronGroup),
        ChainProp(name='HLT_2e24_lhvloose_L12EM20VH', stream=[PhysicsStream,'express'], groups=PrimaryLegGroup+MultiElectronGroup), 

        #---------- primary 3e
        ChainProp(name='HLT_e24_lhvloose_2e12_lhvloose_L1EM20VH_3EM10VH',l1SeedThresholds=['EM20VH','EM10VH'], groups=PrimaryLegGroup+MultiElectronGroup), 

        #--------- primary special
        ChainProp(name='HLT_e20_lhtight_ivarloose_L1ZAFB-25DPHI-eEM15M', l1SeedThresholds=['eEM15M'], groups=PrimaryPhIGroup+SingleElectronGroup), 

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
        ChainProp(name='HLT_g140_loose_L1EM22VHI', stream=[PhysicsStream,'express'], groups=PrimaryLegGroup+SinglePhotonGroup, monGroups=['egammaMon:t0']), 
        ChainProp(name='HLT_g300_etcut_L1EM22VHI', groups=PrimaryLegGroup+SinglePhotonGroup), 

        #----------- primary 2g
        ChainProp(name='HLT_2g20_tight_icaloloose_L12EM15VHI', groups=PrimaryLegGroup+MultiPhotonGroup), 
        ChainProp(name='HLT_2g22_tight_L12EM15VHI', groups=PrimaryLegGroup+MultiPhotonGroup), 
        ChainProp(name='HLT_g35_medium_g25_medium_L12EM20VH', stream=[PhysicsStream,'express'], l1SeedThresholds=['EM20VH','EM20VH'], groups=PrimaryLegGroup+MultiPhotonGroup), 
        ChainProp(name='HLT_2g50_loose_L12EM20VH', groups=PrimaryLegGroup+MultiPhotonGroup), 
        ChainProp(name='HLT_2g20_tight_icaloloose_L12eEM15M', stream=[PhysicsStream,'express'], groups=PrimaryPhIGroup+MultiPhotonGroup),
        ChainProp(name='HLT_g35_medium_g25_medium_L12eEM20L', l1SeedThresholds=['eEM20L','eEM20L'], stream=[PhysicsStream,'express'], groups=PrimaryPhIGroup+MultiPhotonGroup),

        #------------ primary 3g
        ChainProp(name='HLT_2g25_loose_g15_loose_L12EM20VH',l1SeedThresholds=['EM20VH','EM10VH'], groups=PrimaryLegGroup+MultiPhotonGroup), 

        #------------ support legs of multi-photons
        ChainProp(name='HLT_g25_medium_L1EM20VH', groups=SupportLegGroup+SinglePhotonGroup), 
        ChainProp(name='HLT_g35_medium_L1EM20VH', groups=SupportLegGroup+SinglePhotonGroup), 

        ChainProp(name='HLT_g20_tight_icaloloose_L1EM15VHI', stream=[PhysicsStream,'express'], groups=SupportLegGroup+SinglePhotonGroup), 
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
        
        #------------ ATR-23609
        ChainProp(name='HLT_e25_mergedtight_g35_medium_Heg_L12EM20VH',l1SeedThresholds=['EM20VH','EM20VH'], groups=PrimaryLegGroup+MultiElectronGroup), # the comboHypo 02dRAB was removed in ATR-23609 becasue we verified that it wasn't run
        ChainProp(name='HLT_e25_mergedtight_g35_medium_90invmAB_02dRAB_L12EM20VH',l1SeedThresholds=['EM20VH','EM20VH'], groups=PrimaryLegGroup+MultiElectronGroup), 

        #----------- idperf triggers
        ChainProp(name='HLT_e26_idperf_tight_L1EM22VHI', stream=[PhysicsStream,'express'], groups=SingleElectronGroup+SupportLegGroup),

        #------------ support validation of tag-and-probe mass cuts
        # Zee
        ChainProp(name='HLT_e26_lhtight_e15_etcut_probe_50invmAB130_L1EM22VHI', l1SeedThresholds=['EM22VHI','EM7'], stream=[PhysicsStream,'express'], groups=PrimaryLegGroup+MultiElectronGroup), 
        ChainProp(name='HLT_e26_lhtight_e15_etcut_50invmAB130_L1EM22VHI', l1SeedThresholds=['EM22VHI','EM7'], groups=PrimaryLegGroup+MultiElectronGroup), 
        ChainProp(name='HLT_e26_lhtight_e15_etcut_L1EM22VHI', l1SeedThresholds=['EM22VHI','EM7'], stream=[PhysicsStream,'express'], groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e9_lhtight_e4_etcut_L1JPSI-1M5-EM7', stream=[PhysicsStream,'express'], l1SeedThresholds=['EM7','EM3'], groups=SupportLegGroup+SingleElectronGroup),
        ChainProp(name='HLT_e14_lhtight_e4_etcut_1invmAB5_L1JPSI-1M5-EM12', stream=[PhysicsStream,'express'], l1SeedThresholds=['EM12','EM3'], groups=SupportLegGroup+MultiElectronGroup),

        # ATR-24268
        # B->K*ee chains
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1BKeePrimary', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=PrimaryLegGroup+BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L1BKeePrimary', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=PrimaryLegGroup+BphysElectronGroup),
        ChainProp(name='HLT_e5_lhvloose_bBeeM6000_L1BKeePrescaled', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=EOFBeeLegGroup+BphysElectronGroup),
        ChainProp(name='HLT_2e5_lhvloose_bBeeM6000_L1BKeePrescaled', l1SeedThresholds=['EM3'], stream=['BphysDelayed'], groups=EOFBeeLegGroup+BphysElectronGroup),

        # Late stream for LLP
        ChainProp(name='HLT_g35_medium_g25_medium_L1EM7_EMPTY', l1SeedThresholds=['EM7']*2, stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_g35_medium_g25_medium_L1EM7_UNPAIRED_ISO', l1SeedThresholds=['EM7']*2, stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g22_tight_L1EM7_EMPTY', l1SeedThresholds=['EM7'], stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g22_tight_L1EM7_UNPAIRED_ISO', l1SeedThresholds=['EM7'], stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g50_tight_L1EM7_EMPTY', l1SeedThresholds=['EM7'], stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),
        ChainProp(name='HLT_2g50_tight_L1EM7_UNPAIRED_ISO', l1SeedThresholds=['EM7'], stream=['Late'], groups=PrimaryLegGroup+MultiPhotonGroup),

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
        ChainProp(name='HLT_j60_pf_ftf_preselj50_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j85_pf_ftf_preselj50_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j110_pf_ftf_preselj80_L1J30', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j175_pf_ftf_preselj140_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j260_pf_ftf_preselj200_L1J75', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j360_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j380_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j400_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j420_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j440_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j450_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j460_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j480_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j500_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j520_pf_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        # Central single large-R jet chains
        ChainProp(name='HLT_j110_a10sd_cssk_pf_jes_ftf_preselj80_L1J30', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j110_a10t_lcw_jes_L1J30', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_preselj140_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j175_a10t_lcw_jes_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j260_a10sd_cssk_pf_jes_ftf_preselj200_L1J75', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j260_a10t_lcw_jes_L1J75', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_preselj225_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j360_a10t_lcw_jes_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j360_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j400_a10sd_cssk_pf_jes_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j400_a10sd_cssk_pf_jes_ftf_preselj225_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j400_a10t_lcw_jes_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j400_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_preselj225_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j420_a10t_lcw_jes_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup+['RATE:CPS_J100']),
        ChainProp(name='HLT_j420_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_jes_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_jes_ftf_preselj225_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j460_a10t_lcw_jes_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j460_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j480_a10sd_cssk_pf_jes_ftf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j480_a10sd_cssk_pf_jes_ftf_preselj225_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j480_a10t_lcw_jes_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j480_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        #Low-threshold large-R chains (for calibration purposes)
        ChainProp(name='HLT_j85_a10sd_cssk_pf_nojcalib_ftf_preselj50_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j85_a10sd_cssk_pf_jes_ftf_preselj50_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportLegGroup),
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
        ChainProp(name='HLT_j420_L1J100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j260_320eta490_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10_lcw_subjes_L1SC111-CJ15',         l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1J100', l1SeedThresholds=['FSNOSEED'],  groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'],  groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10_lcw_subjes_L1J100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j420_a10t_lcw_jes_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_jes_35smcINF_L1J100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_35smcINF_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_presel2j225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),

        ChainProp(name='HLT_j420_a10t_lcw_jes_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_jes_35smcINF_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_35smcINF_preselj225_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_presel2j225_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j360_a10t_lcw_jes_60smcINF_j360_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_j370_a10t_lcw_jes_35smcINF_j370_a10t_lcw_jes_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_60smcINF_j360_a10sd_cssk_pf_jes_ftf_presel2j225_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_j370_a10sd_cssk_pf_jes_ftf_35smcINF_j370_a10sd_cssk_pf_jes_ftf_presel2j225_L1SC111-CJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryLegGroup+MultiJetGroup),

        # Small-R multijet chains
        # PFlow primaries
        ChainProp(name='HLT_2j250_pf_ftf_0eta240_j120_pf_ftf_0eta240_presel2j180XXj80_L1J100', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_3j200_pf_ftf_presel3j150_L1J100', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_4j115_pf_ftf_presel4j85_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j70_pf_ftf_0eta240_presel5j50_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j85_pf_ftf_presel5j50_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j55_pf_ftf_0eta240_presel6j40_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j70_pf_ftf_presel6j40_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_7j45_pf_ftf_presel7j30_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_10j40_pf_ftf_presel7j30_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        # EMTopo backups
        ChainProp(name='HLT_3j200_L1J100', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_4j120_L13J50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j70_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_5j85_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j55_0eta240_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_6j70_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_7j45_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_10j40_L14J15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryLegGroup),

        #HT chains
        ChainProp(name='HLT_j0_HT1000_L1J100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_HT1000_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_preselj180_L1J100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_preselj180_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleJetGroup),

        # Multijet delayed stream
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_020jvt_presel6c25_L14J15', l1SeedThresholds=['FSNOSEED'], stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiJetGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_020jvt_presel6c25_L14J15', l1SeedThresholds=['FSNOSEED'], stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiJetGroup),

        # TLA chains
        ChainProp(name='HLT_j20_PhysicsTLA_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j20_PhysicsTLA_L1J50_DETA20-J50J', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=EOFTLALegGroup+SingleJetGroup),
        ChainProp(name='HLT_j20_PhysicsTLA_L1HT190-J15s5pETA21', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j70_0eta320_j50_0eta490_j0_DJMASS1000j50dphi200x400deta_L1MJJ-500-NFF', l1SeedThresholds=['FSNOSEED']*3,stream=['VBFDelayed'],groups=PrimaryLegGroup+MultiJetGroup), # previously HLT_j70_j50_0eta490_invm1000j70_dphi20_deta40_L1MJJ-500-NFF

        #ATR-24411 Phase I inputs
        # Single jet support 
        ChainProp(name='HLT_j45_pf_ftf_preselj20_L1jJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j60_pf_ftf_preselj50_L1jJ20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j85_pf_ftf_preselj50_L1jJ20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j110_pf_ftf_preselj80_L1jJ30', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j175_pf_ftf_preselj140_L1jJ50', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j260_pf_ftf_preselj200_L1jJ75', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j360_pf_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jJ100']),
        ChainProp(name='HLT_j380_pf_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jJ100']),
        ChainProp(name='HLT_j400_pf_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jJ100']),
        ChainProp(name='HLT_j420_pf_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j440_pf_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j450_pf_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j460_pf_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j480_pf_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j500_pf_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j520_pf_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        
        # Central single large-R jets
        ChainProp(name='HLT_j110_a10sd_cssk_pf_jes_ftf_preselj80_L1jJ30', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j110_a10t_lcw_jes_L1jJ30', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j175_a10sd_cssk_pf_jes_ftf_preselj140_L1jJ50', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j175_a10t_lcw_jes_L1jJ50', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j260_a10sd_cssk_pf_jes_ftf_preselj200_L1jJ75', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j260_a10t_lcw_jes_L1jJ75', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jJ100']),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_preselj225_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j360_a10t_lcw_jes_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jJ100']),
        ChainProp(name='HLT_j360_a10t_lcw_jes_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j400_a10sd_cssk_pf_jes_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jJ100']),
        ChainProp(name='HLT_j400_a10sd_cssk_pf_jes_ftf_preselj225_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j400_a10t_lcw_jes_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jJ100']),
        ChainProp(name='HLT_j400_a10t_lcw_jes_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jJ100']),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_preselj225_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j420_a10t_lcw_jes_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup+['RATE:CPS_jJ100']),
        ChainProp(name='HLT_j420_a10t_lcw_jes_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_jes_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j460_a10sd_cssk_pf_jes_ftf_preselj225_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j460_a10t_lcw_jes_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j460_a10t_lcw_jes_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j480_a10sd_cssk_pf_jes_ftf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j480_a10sd_cssk_pf_jes_ftf_preselj225_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j480_a10t_lcw_jes_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j480_a10t_lcw_jes_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        # Low threshold large-R chains (for calibration purposes)
        ChainProp(name='HLT_j85_a10sd_cssk_pf_nojcalib_ftf_preselj50_L1jJ20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j85_a10sd_cssk_pf_jes_ftf_preselj50_L1jJ20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j85_a10t_lcw_nojcalib_L1jJ20', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j85_a10t_lcw_jes_L1jJ20',      l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
    
        #Forward small-R EMTopo chains
        ChainProp(name='HLT_j45_320eta490_L1jJ15p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j60_320eta490_L1jJ20p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j85_320eta490_L1jJ20p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j110_320eta490_L1jJ30p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j175_320eta490_L1jJ50p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+SupportPhIGroup),
        ChainProp(name='HLT_j280_320eta490_L1jJ75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j300_320eta490_L1jJ75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),

        # ATR-20049
        ChainProp(name='HLT_j420_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j260_320eta490_L1jJ75p31ETA49', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10_lcw_subjes_L1SC111-CjJ15',         l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1jJ100', l1SeedThresholds=['FSNOSEED'],  groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'],  groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j460_a10_lcw_subjes_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j420_a10t_lcw_jes_35smcINF_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_jes_35smcINF_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_35smcINF_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_presel2j225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),

        ChainProp(name='HLT_j420_a10t_lcw_jes_35smcINF_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_jes_35smcINF_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j420_a10sd_cssk_pf_jes_ftf_35smcINF_preselj225_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_2j330_a10sd_cssk_pf_jes_ftf_35smcINF_presel2j225_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED'], groups=SingleJetGroup+PrimaryPhIGroup),
        ChainProp(name='HLT_j360_a10t_lcw_jes_60smcINF_j360_a10t_lcw_jes_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryPhIGroup+MultiJetGroup),
        ChainProp(name='HLT_j370_a10t_lcw_jes_35smcINF_j370_a10t_lcw_jes_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryPhIGroup+MultiJetGroup),
        ChainProp(name='HLT_j360_a10sd_cssk_pf_jes_ftf_60smcINF_j360_a10sd_cssk_pf_jes_ftf_presel2j225_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryPhIGroup+MultiJetGroup),
        ChainProp(name='HLT_j370_a10sd_cssk_pf_jes_ftf_35smcINF_j370_a10sd_cssk_pf_jes_ftf_presel2j225_L1SC111-CjJ15', l1SeedThresholds=['FSNOSEED']*2, groups=PrimaryPhIGroup+MultiJetGroup),

        # Small-R multijet chains
        # PFlow primaries
        ChainProp(name='HLT_2j250_pf_ftf_0eta240_j120_pf_ftf_0eta240_presel2j180XXj80_L1jJ100', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_3j200_pf_ftf_presel3j150_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_4j115_pf_ftf_presel4j85_L13jJ50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_5j70_pf_ftf_0eta240_presel5j50_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_5j85_pf_ftf_presel5j50_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_6j55_pf_ftf_0eta240_presel6j40_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_6j70_pf_ftf_presel6j40_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_7j45_pf_ftf_presel7j30_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_10j40_pf_ftf_presel7j30_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        # EMTopo backups
        ChainProp(name='HLT_3j200_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_4j120_L13jJ50', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_5j70_0eta240_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_5j85_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_6j55_0eta240_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_6j70_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_7j45_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_10j40_L14jJ15', l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup + PrimaryPhIGroup),

        #HT chains
        ChainProp(name='HLT_j0_HT1000_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_HT1000_L1HT190-jJ15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=SupportPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_preselj180_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_HT1000_pf_ftf_preselj180_L1HT190-jJ15s5pETA21', l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleJetGroup),

        # Multijet delayed stream
        ChainProp(name='HLT_6j35_pf_ftf_0eta240_020jvt_presel6c25_L14jJ15', l1SeedThresholds=['FSNOSEED'], stream=['VBFDelayed'], groups=PrimaryPhIGroup+MultiJetGroup),
        ChainProp(name='HLT_6j45_pf_ftf_0eta240_020jvt_presel6c25_L14jJ15', l1SeedThresholds=['FSNOSEED'], stream=['VBFDelayed'], groups=PrimaryPhIGroup+MultiJetGroup),

        # TLA chains
        ChainProp(name='HLT_j20_PhysicsTLA_L1jJ100', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j20_PhysicsTLA_L1jJ50_DETA20-jJ50J', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=EOFTLAPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j20_PhysicsTLA_L1HT190-jJ15s5pETA21', l1SeedThresholds=['FSNOSEED'], stream=['TLA'], groups=PrimaryPhIGroup+SingleJetGroup),
        ChainProp(name='HLT_j70_0eta320_j50_0eta490_j0_DJMASS1000j50dphi200x400deta_L1jMJJ-500-NFF', l1SeedThresholds=['FSNOSEED']*3,stream=['VBFDelayed'],groups=PrimaryPhIGroup+MultiJetGroup), 
        
    ]

    chains['Bjet'] = [
        # these chains are taken from the Run 2 menu for now --- likely to be loosened
        ChainProp(name="HLT_j275_0eta290_020jvt_pf_ftf_bdl1r60_preselj225_L1J100", l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleBjetGroup),
        ChainProp(name="HLT_j300_0eta290_020jvt_pf_ftf_bdl1r70_preselj225_L1J100", l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleBjetGroup),
        ChainProp(name="HLT_j360_0eta290_020jvt_pf_ftf_bdl1r77_preselj225_L1J100", l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleBjetGroup),
        
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r70_preselj180_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryLegGroup, monGroups=['bJetMon:online']),
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r77_preselj180_L1J100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1r85_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryLegGroup),
        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1r85_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryLegGroup),

        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r77_presel3j45_L13J35p0ETA23", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup + PrimaryLegGroup),
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r77_presel4j25_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup + PrimaryLegGroup),
        ChainProp(name="HLT_j150_0eta320_pf_ftf_2j55_0eta290_020jvt_pf_ftf_bdl1r70_preselj80XX2j45_L1J85_3J30", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_3j35_0eta290_020jvt_pf_ftf_bdl1r70_j35_pf_ftf_0eta320_presel4j25_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j175_0eta290_020jvt_pf_ftf_bdl1r60_j60_0eta290_020jvt_pf_ftf_bdl1r60_preselj140XXj45_L1J100", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta290_020jvt_pf_ftf_bdl1r70_2j35_0eta290_020jvt_pf_ftf_bdl1r85_presel4j25_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j55_0eta290_020jvt_pf_ftf_bdl1r60_2j55_pf_ftf_0eta320_presel4j25_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta290_020jvt_pf_ftf_bdl1r60_3j35_pf_ftf_0eta320_presel5j25_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r60_3j45_pf_ftf_0eta320_presel5j25_L15J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j75_0eta290_020jvt_pf_ftf_bdl1r60_3j75_pf_ftf_presel4j50_L14J20", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),                                        
        ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r60_2j45_pf_ftf_presel4j25_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),
        # Run 2 HH4b low-threshold chain                                               
        ChainProp(name="HLT_2j35_0eta240_020jvt_pf_ftf_bdl1r60_2j35_0eta240_020jvt_pf_ftf_presel4j25_L14J15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryLegGroup+MultiBjetGroup),

        # HT-seeded
        ChainProp(name='HLT_2j45_pf_ftf_bdl1r70_j0_pf_ftf_HT300_j0_pf_ftf_DJMASS700j35_L1HT150-J20s5pETA31_MJJ-400-CF', l1SeedThresholds=['FSNOSEED']*3, groups=PrimaryLegGroup+MultiBjetGroup),

        # VBF chains                
       ChainProp(name='HLT_j80_pf_ftf_0eta240_j60_pf_ftf_0eta320_j45_pf_ftf_320eta490_SHARED_2j45_pf_ftf_0eta290_bdl1r60_preselc60XXj45XXf40_L1J40p0ETA25_2J25_J20p31ETA49', l1SeedThresholds=['FSNOSEED']*4, groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j80_pf_ftf_0eta320_bdl1r70_j60_pf_ftf_0eta320_bdl1r85_j45_pf_ftf_320eta490_preselj60XXj45XXf40_L1J40p0ETA25_2J25_J20p31ETA49", l1SeedThresholds=['FSNOSEED']*3,stream=[PhysicsStream], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name="HLT_j55_pf_ftf_0eta320_bdl1r70_2j45_pf_ftf_320eta490_preselj45XX2f40_L1J25p0ETA23_2J15p31ETA49",l1SeedThresholds=['FSNOSEED']*2,  stream=[PhysicsStream], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j70_pf_ftf_0eta490_j50_pf_ftf_0eta490_2j35_pf_ftf_0eta490_SHARED_2j35_pf_ftf_0eta290_bdl1r70_j0_pf_ftf_presela60XXa40XX2a25_DJMASS1000j50_L1MJJ-500-NFF', l1SeedThresholds=['FSNOSEED']*5,stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),

        ChainProp(name='HLT_j30_0eta290_020jvt_pf_ftf_boffperf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j45_0eta290_020jvt_pf_ftf_boffperf_L1J20', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j60_0eta290_020jvt_pf_ftf_boffperf_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j80_0eta290_020jvt_pf_ftf_boffperf_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j100_0eta290_020jvt_pf_ftf_boffperf_preselj80_L1J50', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j150_0eta290_020jvt_pf_ftf_boffperf_preselj120_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j200_0eta290_020jvt_pf_ftf_boffperf_preselj140_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j300_0eta290_020jvt_pf_ftf_boffperf_preselj225_L1J100', l1SeedThresholds=['FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),

        # HH4b primary candidates with 2 sets of potential jet thresholds
        # 3b85 symmetric b-jet pt for Physics_Main
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_3j20_pf_ftf_0eta240_020jvt_bdl1r85_preselc60XXc45XXc25XXc20_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_3j25_pf_ftf_0eta240_020jvt_bdl1r85_preselc60XXc45XXc25XXc20_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=SupportLegGroup+MultiBjetGroup),
        # 2b60 asymmetric b-jet pt alternative for Physics_Main
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r60_j20_pf_ftf_0eta240_020jvt_bdl1r60_preselc60XXc45XXc25XXc20_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=SupportLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j35_pf_ftf_0eta240_020jvt_bdl1r60_j25_pf_ftf_0eta240_020jvt_bdl1r60_preselc60XXc45XXc25XXc20_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=SupportLegGroup+MultiBjetGroup),
        # 2b77 symmetric b-jet pt for VBFDelayed
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_2j20_pf_ftf_0eta240_020jvt_bdl1r77_preselc60XXc45XXc25XXc20_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_2j25_pf_ftf_0eta240_020jvt_bdl1r77_preselc60XXc45XXc25XXc20_L1J45p0ETA21_3J15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=SupportLegGroup+MultiBjetGroup),

        # Candidates for allhad ttbar delayed stream
        ChainProp(name='HLT_5j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_presel6c25_L14J15', l1SeedThresholds=['FSNOSEED']*2, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),
        ChainProp(name='HLT_5j45_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_presel6c25_L14J15', l1SeedThresholds=['FSNOSEED']*2, stream=['VBFDelayed'], groups=PrimaryLegGroup+MultiBjetGroup),

        # ATR-24411 Phase I inputs
        # Primary single-jet
        ChainProp(name="HLT_j275_0eta290_020jvt_pf_ftf_bdl1r60_preselj225_L1jJ100", l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleBjetGroup),
        ChainProp(name="HLT_j300_0eta290_020jvt_pf_ftf_bdl1r70_preselj225_L1jJ100", l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleBjetGroup),
        ChainProp(name="HLT_j360_0eta290_020jvt_pf_ftf_bdl1r77_preselj225_L1jJ100", l1SeedThresholds=['FSNOSEED'], groups=PrimaryPhIGroup+SingleBjetGroup),

        # More primary
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r70_preselj180_L1jJ100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryPhIGroup),
        ChainProp(name="HLT_j225_0eta290_pf_ftf_bdl1r77_preselj180_L1jJ100", l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_j275_0eta290_pf_ftf_bdl1r85_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryPhIGroup),
        ChainProp(name='HLT_j300_0eta290_pf_ftf_bdl1r85_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SingleBjetGroup + PrimaryPhIGroup),

        # Multi-b
        ChainProp(name="HLT_3j65_0eta290_020jvt_pf_ftf_bdl1r77_presel3j45_L13jJ35p0ETA23", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup + PrimaryPhIGroup), 
        ChainProp(name="HLT_4j35_0eta290_020jvt_pf_ftf_bdl1r77_presel4j25_L14jJ15p0ETA25", l1SeedThresholds=['FSNOSEED'], groups=MultiBjetGroup + PrimaryPhIGroup),
        ChainProp(name="HLT_j150_0eta320_pf_ftf_2j55_0eta290_020jvt_pf_ftf_bdl1r70_preselj80XX2j45_L1jJ85_3jJ30", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_3j35_0eta290_020jvt_pf_ftf_bdl1r70_j35_pf_ftf_0eta320_presel4j25_L14jJ15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_j175_0eta290_020jvt_pf_ftf_bdl1r60_j60_0eta290_020jvt_pf_ftf_bdl1r60_preselj140XXj45_L1jJ100", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta290_020jvt_pf_ftf_bdl1r70_2j35_0eta290_020jvt_pf_ftf_bdl1r85_presel4j25_L14jJ15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j55_0eta290_020jvt_pf_ftf_bdl1r60_2j55_pf_ftf_0eta320_presel4j25_L14jJ15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta290_020jvt_pf_ftf_bdl1r60_3j35_pf_ftf_0eta320_presel5j25_L15jJ15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r60_3j45_pf_ftf_0eta320_presel5j25_L15jJ15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_j75_0eta290_020jvt_pf_ftf_bdl1r60_3j75_pf_ftf_presel4j50_L14jJ20", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j45_0eta290_020jvt_pf_ftf_bdl1r60_2j45_pf_ftf_presel4j25_L14jJ15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_2j35_0eta240_020jvt_pf_ftf_bdl1r60_2j35_0eta240_020jvt_pf_ftf_presel4j25_L14jJ15p0ETA25", l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=PrimaryPhIGroup+MultiBjetGroup),


        # HT-seeded 
        ChainProp(name='HLT_2j45_pf_ftf_bdl1r70_j0_pf_ftf_HT300_j0_pf_ftf_DJMASS700j35_L1jHT150-jJ20s5pETA31_jMJJ-400-CF', l1SeedThresholds=['FSNOSEED']*3, groups=PrimaryPhIGroup+MultiBjetGroup),

        # VBF chains
        ChainProp(name='HLT_j80_pf_ftf_0eta240_j60_pf_ftf_0eta320_j45_pf_ftf_320eta490_SHARED_2j45_pf_ftf_0eta290_bdl1r60_preselc60XXj45XXf40_L1jJ40p0ETA25_2jJ25_jJ20p31ETA49', l1SeedThresholds=['FSNOSEED']*4, groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_j80_pf_ftf_0eta320_bdl1r70_j60_pf_ftf_0eta320_bdl1r85_j45_pf_ftf_320eta490_preselj60XXj45XXf40_L1jJ40p0ETA25_2jJ25_jJ20p31ETA49", l1SeedThresholds=['FSNOSEED']*3,stream=[PhysicsStream], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name="HLT_j55_pf_ftf_0eta320_bdl1r70_2j45_pf_ftf_320eta490_preselj45XX2f40_L1jJ25p0ETA23_2jJ15p31ETA49",l1SeedThresholds=['FSNOSEED']*2,  stream=[PhysicsStream], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name='HLT_j70_pf_ftf_0eta490_j50_pf_ftf_0eta490_2j35_pf_ftf_0eta490_SHARED_2j35_pf_ftf_0eta290_bdl1r70_j0_pf_ftf_presela60XXa40XX2a25_DJMASS1000j50_L1jMJJ-500-NFF', l1SeedThresholds=['FSNOSEED']*5,stream=['VBFDelayed'], groups=PrimaryPhIGroup+MultiBjetGroup),

        ChainProp(name='HLT_j30_0eta290_020jvt_pf_ftf_boffperf_L1jJ20', l1SeedThresholds=['FSNOSEED'], groups=SupportPhIGroup+SingleBjetGroup),
        ChainProp(name='HLT_j45_0eta290_020jvt_pf_ftf_boffperf_L1jJ20', l1SeedThresholds=['FSNOSEED'], groups=SupportPhIGroup+SingleBjetGroup),
        ChainProp(name='HLT_j60_0eta290_020jvt_pf_ftf_boffperf_L1jJ50', l1SeedThresholds=['FSNOSEED'], groups=SupportPhIGroup+SingleBjetGroup),
        ChainProp(name='HLT_j80_0eta290_020jvt_pf_ftf_boffperf_L1jJ50', l1SeedThresholds=['FSNOSEED'], groups=SupportPhIGroup+SingleBjetGroup),
        ChainProp(name='HLT_j100_0eta290_020jvt_pf_ftf_boffperf_preselj80_L1jJ50', l1SeedThresholds=['FSNOSEED'], groups=SupportPhIGroup+SingleBjetGroup),
        ChainProp(name='HLT_j150_0eta290_020jvt_pf_ftf_boffperf_preselj120_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SupportPhIGroup+SingleBjetGroup),
        ChainProp(name='HLT_j200_0eta290_020jvt_pf_ftf_boffperf_preselj140_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SupportPhIGroup+SingleBjetGroup),
        ChainProp(name='HLT_j300_0eta290_020jvt_pf_ftf_boffperf_preselj225_L1jJ100', l1SeedThresholds=['FSNOSEED'], groups=SupportPhIGroup+SingleBjetGroup),

        # HH4b primary candidates with 2 sets of potential jet thresholds
        # 3b85 symmetric b-jet pt for Physics_Main
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_3j20_pf_ftf_0eta240_020jvt_bdl1r85_preselc60XXc45XXc25XXc20_L1jJ45p0ETA21_3jJ15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_3j25_pf_ftf_0eta240_020jvt_bdl1r85_preselc60XXc45XXc25XXc20_L1jJ45p0ETA21_3jJ15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=[PhysicsStream], groups=SupportPhIGroup+MultiBjetGroup),
        # 2b60 asymmetric b-jet pt alternative for Physics_Main
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_j28_pf_ftf_0eta240_020jvt_bdl1r60_j20_pf_ftf_0eta240_020jvt_bdl1r60_preselc60XXc45XXc25XXc20_L1jJ45p0ETA21_3jJ15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=SupportPhIGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_j35_pf_ftf_0eta240_020jvt_bdl1r60_j25_pf_ftf_0eta240_020jvt_bdl1r60_preselc60XXc45XXc25XXc20_L1jJ45p0ETA21_3jJ15p0ETA25', l1SeedThresholds=['FSNOSEED']*6, stream=['VBFDelayed'], groups=SupportPhIGroup+MultiBjetGroup),
        # 2b77 symmetric b-jet pt for VBFDelayed
        ChainProp(name='HLT_j80_pf_ftf_0eta240_020jvt_j55_pf_ftf_0eta240_020jvt_j28_pf_ftf_0eta240_020jvt_j20_pf_ftf_0eta240_020jvt_SHARED_2j20_pf_ftf_0eta240_020jvt_bdl1r77_preselc60XXc45XXc25XXc20_L1jJ45p0ETA21_3jJ15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name='HLT_j75_pf_ftf_0eta240_020jvt_j50_pf_ftf_0eta240_020jvt_j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_SHARED_2j25_pf_ftf_0eta240_020jvt_bdl1r77_preselc60XXc45XXc25XXc20_L1jJ45p0ETA21_3jJ15p0ETA25', l1SeedThresholds=['FSNOSEED']*5, stream=['VBFDelayed'], groups=SupportPhIGroup+MultiBjetGroup),

        # Candidates for allhad ttbar delayed stream
        ChainProp(name='HLT_5j35_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_presel6c25_L14jJ15', l1SeedThresholds=['FSNOSEED']*2, stream=['VBFDelayed'], groups=PrimaryPhIGroup+MultiBjetGroup),
        ChainProp(name='HLT_5j45_pf_ftf_0eta240_020jvt_j25_pf_ftf_0eta240_020jvt_bdl1r60_presel6c25_L14jJ15', l1SeedThresholds=['FSNOSEED']*2, stream=['VBFDelayed'], groups=PrimaryPhIGroup+MultiBjetGroup),


    ]

    chains['Tau'] = [
        #ATR-20049
        ChainProp(name='HLT_tau160_mediumRNN_tracktwoMVA_L1TAU100', groups=SupportLegGroup+SingleTauGroup, monGroups=['tauMon:online']),
        ChainProp(name='HLT_tau160_mediumRNN_tracktwoMVABDT_L1TAU100', stream=[PhysicsStream,'express'], groups=PrimaryLegGroup+SingleTauGroup, monGroups=['tauMon:t0']), 
        ChainProp(name='HLT_tau200_mediumRNN_tracktwoMVA_L1TAU100', groups=SupportLegGroup+SingleTauGroup),
        ChainProp(name='HLT_tau200_mediumRNN_tracktwoMVABDT_L1TAU100', groups=PrimaryLegGroup+SingleTauGroup),

        # displaced tau (ATR-21754)
        ChainProp(name="HLT_tau180_mediumRNN_tracktwoLLP_L1TAU100", stream=[PhysicsStream,'express'], groups=PrimaryLegGroup+SingleTauGroup, monGroups=['tauMon:t0']),   

        # ATR-21797
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVA_tau60_mediumRNN_tracktwoMVA_03dRAB_L1TAU60_2TAU40',                  l1SeedThresholds=['TAU60','TAU40'],     groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVABDT_tau60_mediumRNN_tracktwoMVABDT_03dRAB_L1TAU60_2TAU40',            l1SeedThresholds=['TAU60','TAU40'],     groups=PrimaryLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA_03dRAB30_L1TAU60_DR-TAU20ITAU12I',       l1SeedThresholds=['TAU60','TAU12IM'],   groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVABDT_tau35_mediumRNN_tracktwoMVABDT_03dRAB30_L1TAU60_DR-TAU20ITAU12I', stream=[PhysicsStream,'express'], l1SeedThresholds=['TAU60','TAU12IM'],   groups=PrimaryLegGroup+MultiTauGroup, monGroups=['tauMon:online']),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dRAB30_L1DR-TAU20ITAU12I-J25',         l1SeedThresholds=['TAU20IM','TAU12IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_03dRAB30_L1DR-TAU20ITAU12I-J25',   l1SeedThresholds=['TAU20IM','TAU12IM'], stream=[PhysicsStream,'express'], groups=PrimaryLegGroup+MultiTauGroup),
        
        # ATR-20450 
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dRAB_L1TAU20IM_2TAU12IM_4J12p0ETA25',    l1SeedThresholds=['TAU20IM','TAU12IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1TAU20IM_2TAU12IM_4J12p0ETA25', l1SeedThresholds=['TAU20IM','TAU12IM'], groups=PrimaryLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau40_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA_03dRAB_L1TAU25IM_2TAU20IM_2J25_3J20',      l1SeedThresholds=['TAU25IM','TAU20IM'], groups=SupportLegGroup+MultiTauGroup),
        ChainProp(name='HLT_tau40_mediumRNN_tracktwoMVABDT_tau35_mediumRNN_tracktwoMVABDT_03dRAB_L1TAU25IM_2TAU20IM_2J25_3J20',   l1SeedThresholds=['TAU25IM','TAU20IM'], groups=PrimaryLegGroup+MultiTauGroup),

        # displaced tau+X (ATR-21754)
        ChainProp(name="HLT_tau80_mediumRNN_tracktwoLLP_tau60_mediumRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40",                  l1SeedThresholds=['TAU60','TAU40'],     groups=PrimaryLegGroup+MultiTauGroup), # <-- for physics

        #------- 
        ChainProp(name="HLT_tau0_ptonly_L1TAU8", stream=[PhysicsStream,'express'], groups=SingleTauGroup+SupportLegGroup, monGroups=['tauMon:t0']),
        ChainProp(name="HLT_tau0_ptonly_L1TAU60", stream=[PhysicsStream,'express'], groups=SingleTauGroup+SupportLegGroup, monGroups=['tauMon:online']),
        ChainProp(name="HLT_tau25_idperf_tracktwoMVABDT_L1TAU12IM", stream=[PhysicsStream,'express'], groups=SingleTauGroup+SupportLegGroup, monGroups=['tauMon:t0']),
        ChainProp(name="HLT_tau25_perf_tracktwoMVABDT_L1TAU12IM", stream=[PhysicsStream,'express'], groups=SingleTauGroup+SupportLegGroup, monGroups=['tauMon:t0']),
        ChainProp(name="HLT_tau160_idperf_tracktwoMVABDT_L1TAU100", stream=[PhysicsStream,'express'], groups=SingleTauGroup+SupportLegGroup, monGroups=['tauMon:online','tauMon:t0']),
        ChainProp(name="HLT_tau160_perf_tracktwoMVABDT_L1TAU100", stream=[PhysicsStream,'express'], groups=SingleTauGroup+SupportLegGroup, monGroups=['tauMon:online','tauMon:t0']),
        ChainProp(name="HLT_tau25_mediumRNN_tracktwoMVABDT_L1TAU12IM", groups=SingleTauGroup+SupportLegGroup, monGroups=['tauMon:online']),

        #------ Phase-I
        ChainProp(name="HLT_tau160_mediumRNN_tracktwoMVABDT_L1eTAU100", stream=[PhysicsStream], groups=PrimaryPhIGroup+SingleTauGroup),
        ChainProp(name='HLT_tau180_mediumRNN_tracktwoLLP_L1eTAU100', stream=[PhysicsStream,'express'], groups=PrimaryPhIGroup+SingleTauGroup),

    ]

    chains['Bphysics'] = [
        #-- dimuon primary triggers
        ChainProp(name='HLT_2mu10_bJpsimumu_L12MU8F', stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_2mu10_bUpsimumu_L12MU8F', stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        #-- RCP multiple candidate
        ChainProp(name='HLT_mu10_l2mt_mu4_l2mt_bJpsimumu_L1MU10BOM', l1SeedThresholds=['MU10BOM']*2, stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu10_l2mt_mu4_l2mt_bJpsimumu_L1MU12BOM', l1SeedThresholds=['MU10BOM']*2, stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        #-- mu11_mu6 chains
        ChainProp(name='HLT_mu11_mu6_bJpsimumu_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bJpsimumu_Lxy0_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bUpsimumu_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumu_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bDimu_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bDimu_Lxy0_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bDimu2700_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bDimu2700_Lxy0_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bPhi_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bTau_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        #-- mu11_mu6 chains with L1Topo
        ChainProp(name='HLT_mu11_mu6_bJpsimumu_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bUpsimumu_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumu_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bDimu_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bDimu2700_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bPhi_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bTau_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        #-- 2mu6 chains with L1Topo
        ChainProp(name='HLT_2mu6_bJpsimumu_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_2mu6_bBmumu_Lxy0_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_2mu6_bUpsimumu_L1BPH-8M15-0DR22-2MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        #-- dimuon EOF triggers
        #-- 2mu6 chains
        ChainProp(name='HLT_2mu6_bDimu_L12MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        #-- 2mu6 chains with L1Topo
        ChainProp(name='HLT_2mu6_bBmumu_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bDimu_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bDimu_L1LFV-MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bPhi_L1LFV-MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        #-- mu6_mu4 chains
        ChainProp(name='HLT_mu6_mu4_bDimu_L1MU5VF_2MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        #-- mu6_mu4 chains with L1Topo
        ChainProp(name='HLT_mu6_mu4_bJpsimumu_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumu_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumu_Lxy0_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bDimu_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bUpsimumu_L1BPH-8M15-0DR22-MU5VFMU3V-BO', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        #-- mu6_mu4 chains with L1 charge cut (ATR-19639)
        ChainProp(name='HLT_mu6_mu4_bJpsimumu_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumu_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumu_Lxy0_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bDimu_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        #-- 2mu4 chains with L1Topo
        ChainProp(name='HLT_2mu4_bJpsimumu_Lxy0_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumu_Lxy0_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumu_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),

        #-- multi muon primary triggers (only two muons are fitted to the common vertex except the case of bTau topology)
        #-- 3mu
        ChainProp(name='HLT_3mu6_bJpsi_L13MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu6_bUpsi_L13MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu6_bDimu_L13MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu6_bTau_L13MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_2mu6_mu4_bTau_L12MU5VF_3MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_2mu6_mu4_bUpsi_L12MU5VF_3MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu6_2mu4_bJpsi_L1MU5VF_3MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu6_2mu4_bUpsi_L1MU5VF_3MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu6_2mu4_bTau_L1MU5VF_3MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu6_2mu4_bDimu2700_L1MU5VF_3MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu6_2mu4_bDimu6000_L1MU5VF_3MU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu4_bJpsi_L13MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu4_bUpsi_L13MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu4_bTau_L13MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu4_bPhi_L13MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        #-- 4mu
        ChainProp(name='HLT_4mu4_bDimu6000_L14MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        #-- EOF triggers
        ChainProp(name='HLT_3mu4_bDimu2700_L13MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),

        #-- Bmumux primary triggers
        #-- mu11_mu6 chains
        ChainProp(name='HLT_mu11_mu6_bBmumux_BpmumuKp_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumux_BcmumuPi_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumux_BsmumuPhi_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumux_BdmumuKst_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu11_mu6_bBmumux_LbPqKm_L1MU8VF_2MU5VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        #-- mu11_mu6 chains with L1Topo
        ChainProp(name='HLT_mu11_mu6_bBmumux_BpmumuKp_L1LFV-MU8VF', l1SeedThresholds=['MU8VF','MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        #-- 2mu6 chains with L1Topo
        ChainProp(name='HLT_2mu6_bBmumux_BpmumuKp_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_2mu6_bBmumux_BsmumuPhi_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_2mu6_bBmumux_LbPqKm_L1BPH-2M9-2DR15-2MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup), # MR!46945
        #-- Bmumux EOF triggers
        #-- 2mu6 chains with L1Topo
        ChainProp(name='HLT_2mu6_bBmumux_BpmumuKp_L1LFV-MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bBmumux_BsmumuPhi_L1LFV-MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu6_bBmumux_LbPqKm_L1LFV-MU5VF', l1SeedThresholds=['MU5VF'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup), # MR!46945
        #-- mu6_mu4 chains with L1Topo
        ChainProp(name='HLT_mu6_mu4_bBmumux_BpmumuKp_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumux_BsmumuPhi_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumux_LbPqKm_L1BPH-2M9-0DR15-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup), # MR!46945
        #-- mu6_mu4 chains with L1 charge cut (ATR-19639)
        ChainProp(name='HLT_mu6_mu4_bBmumux_BpmumuKp_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumux_BsmumuPhi_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_bBmumux_LbPqKm_L1BPH-2M9-0DR15-C-MU5VFMU3V', l1SeedThresholds=['MU5VF','MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup), # MR!46945
        #-- 2mu4 chains with L1Topo
        ChainProp(name='HLT_2mu4_bBmumux_BpmumuKp_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumux_BsmumuPhi_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_bBmumux_LbPqKm_L1BPH-2M9-0DR15-2MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+EOFBPhysL1MuGroup), # MR!46945
        #-- supplementary PEB triggers
        ChainProp(name='HLT_mu4_bJpsimutrk_MuonTrkPEB_L1MU3V', l1SeedThresholds=['MU3V'], stream=['BphysPEB'], groups=SupportGroup+BphysicsGroup+['PS:Online']),
        ChainProp(name='HLT_mu6_bJpsimutrk_MuonTrkPEB_L1MU5VF', l1SeedThresholds=['MU5VF'], stream=['BphysPEB'], groups=SupportGroup+BphysicsGroup+['PS:Online']),
        ChainProp(name='HLT_mu10_bJpsimutrk_MuonTrkPEB_L1MU8F', l1SeedThresholds=['MU8F'], stream=['BphysPEB'], groups=SupportGroup+BphysicsGroup+['PS:Online']),
        ChainProp(name='HLT_mu20_bJpsimutrk_MuonTrkPEB_L1MU14FCH', l1SeedThresholds=['MU14FCH'], stream=['BphysPEB'], groups=SupportGroup+BphysicsGroup+['PS:Online']),

        # 3mu inv mass (ATR-19355, ATR-19638)
        ChainProp(name='HLT_3mu4_b3mu_noos_L1BPH-0M10-3MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_3mu4_b3mu_L1BPH-0M10C-3MU3V', l1SeedThresholds=['MU3V'], stream=["BphysDelayed"], groups=BphysicsGroup+PrimaryL1MuGroup),

    ]

    chains['Combined'] = [

        # Primary e-mu chains
        ChainProp(name='HLT_e17_lhloose_mu14_L1EM15VH_MU8F', l1SeedThresholds=['EM15VH','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e7_lhmedium_mu24_L1MU14FCH',l1SeedThresholds=['EM3','MU14FCH'],  stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e12_lhloose_2mu10_L12MU8F', l1SeedThresholds=['EM8VH','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_2e12_lhloose_mu10_L12EM8VH_MU8F', l1SeedThresholds=['EM8VH','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        # Primary g-mu chains
        ChainProp(name='HLT_g25_medium_mu24_L1MU14FCH',l1SeedThresholds=['EM15VH','MU14FCH'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup), #ATR-22594
        ChainProp(name='HLT_g35_loose_mu18_L1EM24VHI', l1SeedThresholds=['EM24VHI','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_2g10_loose_mu20_L1MU14FCH', l1SeedThresholds=['EM7','MU14FCH'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup), # unsure what EM seed should be
        #LLP
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L12MU8F', l1SeedThresholds=['EM8VH','MU8F'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        #ATR-22107
        ChainProp(name='HLT_e26_lhmedium_mu8noL1_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e28_lhmedium_mu8noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_e9_lhvloose_mu20_mu8noL1_L1MU14FCH', l1SeedThresholds=['EM3','MU14FCH','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_loose_mu15_mu2noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','MU5VF','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_tight_icalotight_mu18noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g35_tight_icalotight_mu15noL1_mu2noL1_L1EM24VHI', l1SeedThresholds=['EM24VHI','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        # Late stream for LLP
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L1MU3V_EMPTY', l1SeedThresholds=['EM8VH','MU3V'], stream=['Late'], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L1MU5VF_EMPTY', l1SeedThresholds=['EM8VH','MU5VF'], stream=['Late'], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L1MU3V_UNPAIRED_ISO', l1SeedThresholds=['EM8VH','MU3V'], stream=['Late'], groups=PrimaryLegGroup+EgammaMuonGroup),

        # tau + muon triggers
        ChainProp(name='HLT_mu20_ivarloose_tau20_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu20_ivarloose_tau20_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu14_ivarloose_tau35_mediumRNN_tracktwoMVA_03dRAB_L1MU8F_TAU20IM', l1SeedThresholds=['MU8F','TAU20IM'], stream=[PhysicsStream], groups=SupportLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu14_ivarloose_tau35_mediumRNN_tracktwoMVABDT_03dRAB_L1MU8F_TAU20IM', l1SeedThresholds=['MU8F','TAU20IM'], stream=[PhysicsStream], groups=PrimaryLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_03dRAB_L1MU8F_TAU12IM_3J12', l1SeedThresholds=['MU8F','TAU12IM'], stream=[PhysicsStream], groups=SupportLegGroup+MuonTauGroup),
        ChainProp(name='HLT_mu14_ivarloose_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1MU8F_TAU12IM_3J12', l1SeedThresholds=['MU8F','TAU12IM'], stream=[PhysicsStream], groups=PrimaryLegGroup+MuonTauGroup),

        # tau + electron triggers
        ChainProp(name='HLT_e24_lhmedium_ivarloose_tau20_mediumRNN_tracktwoMVA_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=SupportLegGroup+EgammaTauGroup),
        ChainProp(name='HLT_e24_lhmedium_ivarloose_tau20_mediumRNN_tracktwoMVABDT_03dRAB_L1EM22VHI', l1SeedThresholds=['EM22VHI','TAU8'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaTauGroup),
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
       ChainProp(name='HLT_mu26_ivarmedium_tau20_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU8'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau20_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU8'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_idperf_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_idperf_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_perf_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_perf_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU12IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau35_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU20IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau35_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU20IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau40_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU25IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau40_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU25IM'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau60_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU40'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau60_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU40'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau80_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU60'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau80_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU60'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau160_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU100'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
       ChainProp(name='HLT_mu26_ivarmedium_tau160_mediumRNN_tracktwoMVABDT_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU100'], stream=[PhysicsStream], groups=TagAndProbeGroup+SingleMuonGroup),
        # mu-tag & tau-probe triggers for LLP tau (ATR-23150)
        ChainProp(name='HLT_mu26_ivarmedium_tau60_mediumRNN_tracktwoLLP_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU40'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu26_ivarmedium_tau80_mediumRNN_tracktwoLLP_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU60'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu26_ivarmedium_tau180_mediumRNN_tracktwoLLP_03dRAB_L1MU14FCH', l1SeedThresholds=['MU14FCH','TAU100'], stream=[PhysicsStream], groups=TagAndProbeLegGroup+SingleMuonGroup),

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

        # MET + tau tag and probe chains (ATR-23507)
        ChainProp(name='HLT_tau20_mediumRNN_tracktwoMVA_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU8','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau20_mediumRNN_tracktwoMVABDT_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU8','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVA_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU12IM','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau25_mediumRNN_tracktwoMVABDT_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU12IM','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVA_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU20IM','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau35_mediumRNN_tracktwoMVABDT_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU20IM','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau40_mediumRNN_tracktwoMVA_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU25IM','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau40_mediumRNN_tracktwoMVABDT_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU25IM','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau60_mediumRNN_tracktwoMVA_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU40','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau60_mediumRNN_tracktwoMVABDT_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU40','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVA_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU60','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoMVABDT_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU60','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau160_mediumRNN_tracktwoMVA_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU100','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau160_mediumRNN_tracktwoMVABDT_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU100','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau60_mediumRNN_tracktwoLLP_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU40','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau80_mediumRNN_tracktwoLLP_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU60','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau180_mediumRNN_tracktwoLLP_probe_xe65_cell_xe90_pfopufit_L1XE50', l1SeedThresholds=['PROBETAU100','FSNOSEED','FSNOSEED'],  groups=TagAndProbeLegGroup+TauMETGroup),

        # b-jet trigger calibration chains
        ChainProp(name='HLT_e26_lhtight_ivarloose_2j20_0eta290_020jvt_pf_ftf_boffperf_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED'], groups=TagAndProbeLegGroup+SingleElectronGroup, monGroups=['bJetMon:online']),
        ChainProp(name='HLT_mu26_ivarmedium_2j20_0eta290_020jvt_pf_ftf_boffperf_L1MU14FCH', l1SeedThresholds=['MU14FCH','FSNOSEED'], groups=TagAndProbeGroup+SingleMuonGroup, monGroups=['bJetMon:online']),
        ChainProp(name='HLT_e26_lhtight_ivarloose_mu22noL1_j20_0eta290_020jvt_pf_ftf_boffperf_L1EM22VHI', l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'], groups=TagAndProbeLegGroup+EgammaBjetGroup),

        #Isolated High pt Track Trigger
        #Primary
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_medium_iaggrmedium_L1XE50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream], groups=UnconvTrkGroup+PrimaryLegGroup),
        #Backup for Primary Triggers
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk140_isohpttrack_medium_iaggrmedium_L1XE50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream], groups=UnconvTrkGroup+PrimaryLegGroup),
        #Support
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk100_isohpttrack_medium_iaggrmedium_L1XE50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream], groups=UnconvTrkGroup+SupportLegGroup),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk120_isohpttrack_medium_iaggrloose_L1XE50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],  groups=UnconvTrkGroup+SupportLegGroup),


        # electron + MET (ATR-22594)
        ChainProp(name='HLT_e70_lhloose_xe70_cell_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMETGroup),

        #tau + met ATR-23600
        ChainProp(name='HLT_tau50_mediumRNN_tracktwoMVA_xe80_tcpufit_xe50_cell_L1XE50', l1SeedThresholds=['TAU25IM','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=SupportLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau50_mediumRNN_tracktwoMVA_xe80_pfopufit_xe50_cell_L1XE50', l1SeedThresholds=['TAU25IM','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=SupportLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau50_mediumRNN_tracktwoMVABDT_xe80_tcpufit_xe50_cell_L1XE50', l1SeedThresholds=['TAU25IM','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+TauMETGroup),
        ChainProp(name='HLT_tau50_mediumRNN_tracktwoMVABDT_xe80_pfopufit_xe50_cell_L1XE50', l1SeedThresholds=['TAU25IM','FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+TauMETGroup),

        ChainProp(name='HLT_j80_pf_ftf_bdl1r60_xe60_cell_L12J50_XE40', l1SeedThresholds=['FSNOSEED','FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+BjetMETGroup),
        ChainProp(name='HLT_g25_medium_mu24_ivarmedium_L1MU14FCH', l1SeedThresholds=['EM22VHI','MU14FCH'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        # SUSY
        ChainProp(name='HLT_g45_loose_6j45_L14J15p0ETA25',l1SeedThresholds=['EM15','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaJetGroup),

        # VBF triggers (ATR-22594)                                       
        ChainProp(name='HLT_2mu6_2j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['MU5VF','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+MuonJetGroup), # Formerly HLT_2mu6_2j50_0eta490_invm900j50                       
        ChainProp(name='HLT_e5_lhvloose_j70_0eta320_j50_0eta490_j0_DJMASS1000j50_xe50_tcpufit_L1MJJ-500-NFF',l1SeedThresholds=['EM3','FSNOSEED','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_2e5_lhmedium_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['EM3','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_g25_medium_4j35_0eta490_j0_DJMASS1000j35_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_e10_lhmedium_ivarloose_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['EM8VH','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+EgammaJetGroup),
        ChainProp(name='HLT_mu4_j70_0eta320_j50_0eta490_j0_DJMASS1000j50_xe50_tcpufit_L1MJJ-500-NFF',l1SeedThresholds=['MU3V','FSNOSEED','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+MuonJetGroup),
        ChainProp(name='HLT_mu10_ivarmedium_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1MJJ-500-NFF',l1SeedThresholds=['MU8F','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryLegGroup+MuonJetGroup),
        ChainProp(name='HLT_j70_0eta320_j50_0eta490_j0_DJMASS1000j50dphi240_xe90_tcpufit_xe50_cell_L1MJJ-500-NFF',l1SeedThresholds=['FSNOSEED']*5,stream=['VBFDelayed'], groups=PrimaryLegGroup+JetMETGroup),

        # Photon+VBF
        ChainProp(name='HLT_g20_tight_icaloloose_j35_pf_ftf_bdl1r77_3j35_pf_ftf_0eta490_j0_pf_ftf_DJMASS500j35_L1EM18VHI_MJJ-300',l1SeedThresholds=['EM18VHI','FSNOSEED','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaBjetGroup),
        
        # LLP late stream
        ChainProp(name='HLT_j55_0eta240_xe50_cell_L1J30_EMPTY', l1SeedThresholds=['FSNOSEED']*2, stream=['Late'], groups=PrimaryLegGroup+JetMETGroup),
        ChainProp(name='HLT_j55_0eta240_xe50_cell_L1J30_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED']*2, stream=['Late'], groups=PrimaryLegGroup+JetMETGroup),

        # AFP + dijet
        ChainProp(name='HLT_2j120_mb_afprec_afpdijet_L1AFP_A_AND_C_TOF_J50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportLegGroup),
        ChainProp(name='HLT_2j175_mb_afprec_afpdijet_L1AFP_A_AND_C_TOF_J75', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportLegGroup),

        # Muon-in-jet
        ChainProp(name='HLT_mu4_j20_0eta290_pf_ftf_boffperf_dRAB03_L1MU3V_J15', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SupportLegGroup+SingleBjetGroup),

        # Phase I inputs ATR-24411
        # SUSY
        ChainProp(name='HLT_g45_loose_6j45_L14jJ15p0ETA25',l1SeedThresholds=['eEM15','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryPhIGroup+EgammaJetGroup),
        
        # VBF triggers (ATR-22594)                                       
        ChainProp(name='HLT_2mu6_2j50_0eta490_j0_DJMASS900j50_L1jMJJ-500-NFF',l1SeedThresholds=['MU5VF','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryPhIGroup+MuonJetGroup), # Formerly HLT_2mu6_2j50_0eta490_invm900j50                       
        ChainProp(name='HLT_e5_lhvloose_j70_0eta320_j50_0eta490_j0_DJMASS1000j50_xe50_tcpufit_L1jMJJ-500-NFF',l1SeedThresholds=['eEM3','FSNOSEED','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryPhIGroup+EgammaJetGroup),
        ChainProp(name='HLT_2e5_lhmedium_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1jMJJ-500-NFF',l1SeedThresholds=['eEM3','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryPhIGroup+EgammaJetGroup),
        ChainProp(name='HLT_g25_medium_4j35_0eta490_j0_DJMASS1000j35_L1eEM22M',l1SeedThresholds=['eEM22M','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryPhIGroup+EgammaJetGroup),
        ChainProp(name='HLT_e10_lhmedium_ivarloose_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1jMJJ-500-NFF',l1SeedThresholds=['eEM8L','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryPhIGroup+EgammaJetGroup),
        ChainProp(name='HLT_mu4_j70_0eta320_j50_0eta490_j0_DJMASS1000j50_xe50_tcpufit_L1jMJJ-500-NFF',l1SeedThresholds=['MU3V','FSNOSEED','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryPhIGroup+MuonJetGroup),
        ChainProp(name='HLT_mu10_ivarmedium_j70_0eta320_j50_0eta490_j0_DJMASS900j50_L1jMJJ-500-NFF',l1SeedThresholds=['MU8F','FSNOSEED','FSNOSEED','FSNOSEED'],stream=['VBFDelayed'], groups=PrimaryPhIGroup+MuonJetGroup),
        ChainProp(name='HLT_j70_0eta320_j50_0eta490_j0_DJMASS1000j50dphi240_xe90_tcpufit_xe50_cell_L1jMJJ-500-NFF',l1SeedThresholds=['FSNOSEED']*5,stream=['VBFDelayed'], groups=PrimaryPhIGroup+JetMETGroup),

        # Photon+VBF
        ChainProp(name='HLT_g20_tight_icaloloose_j35_pf_ftf_bdl1r77_3j35_pf_ftf_0eta490_j0_pf_ftf_DJMASS500j35_L1eEM18M_jMJJ-300-NFF',l1SeedThresholds=['eEM18M','FSNOSEED','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryPhIGroup+EgammaBjetGroup),

        # photon + VBF Hbb (ATR-23293) 
        ChainProp(name='HLT_g25_tight_icaloloose_2j35_pf_ftf_0eta490_bdl1r77_2j35_pf_ftf_0eta490_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaBjetGroup),
        ChainProp(name='HLT_g25_tight_icaloloose_j35_pf_ftf_0eta490_bdl1r77_3j35_pf_ftf_0eta490_j0_pf_ftf_DJMASS700j35_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaBjetGroup),
        ChainProp(name='HLT_g25_medium_2j35_pf_ftf_0eta490_bdl1r77_2j35_pf_ftf_0eta490_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=SupportLegGroup+EgammaBjetGroup),
        ChainProp(name='HLT_g25_medium_j35_pf_ftf_0eta490_bdl1r77_3j35_pf_ftf_0eta490_j0_pf_ftf_DJMASS700j35_L1EM22VHI',l1SeedThresholds=['EM22VHI','FSNOSEED','FSNOSEED','FSNOSEED'],stream=[PhysicsStream], groups=SupportLegGroup+EgammaBjetGroup),

        # LLP late stream
        ChainProp(name='HLT_j55_0eta240_xe50_cell_L1jJ30_EMPTY', l1SeedThresholds=['FSNOSEED']*2, stream=['Late'], groups=PrimaryPhIGroup+JetMETGroup),
        ChainProp(name='HLT_j55_0eta240_xe50_cell_L1jJ30_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED']*2, stream=['Late'], groups=PrimaryPhIGroup+JetMETGroup),

        # AFP + dijet
        ChainProp(name='HLT_2j120_mb_afprec_afpdijet_L1AFP_A_AND_C_TOF_jJ50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportPhIGroup),
        ChainProp(name='HLT_2j175_mb_afprec_afpdijet_L1AFP_A_AND_C_TOF_jJ75', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportPhIGroup),

        # Muon-in-jet
        ChainProp(name='HLT_mu4_j20_0eta290_pf_ftf_boffperf_dRAB03_L1MU3V_jJ15', l1SeedThresholds=['MU3V','FSNOSEED'], groups=SingleBjetGroup+SupportPhIGroup),

        # ATR-20505
        ChainProp(name='HLT_g40_loose_mu40_msonly_L1MU14FCH', l1SeedThresholds=['EM20VH','MU14FCH'], stream=[PhysicsStream], groups=PrimaryLegGroup+EgammaMuonGroup),

        ChainProp(name='HLT_2j135_mb_afprec_afpdijet_L1CEP-CjJ60', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportLegGroup),
        ChainProp(name='HLT_2j120_mb_afprec_afpdijet_L1CEP-CjJ50', l1SeedThresholds=['FSNOSEED']*2, stream=[PhysicsStream],groups=MinBiasGroup+LowMuGroup+SupportLegGroup),

        ## Unconventional tracking ATR-23797
        # hit-based DV
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk200_hitdv_tight_L1XE50', groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk200_hitdv_medium_L1XE50', groups=SupportLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),
        # disappearing track trigger
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk20_distrk_tight_L1XE50',  groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk20_distrk_medium_L1XE50', groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),
        # dEdx triggers
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk25_dedx_medium_L1XE50', groups=SupportLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),
        ChainProp(name='HLT_xe80_tcpufit_unconvtrk50_dedx_medium_L1XE50', groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']*2),


    ]
    chains['MinBias'] = [
        ChainProp(name='HLT_mb_sptrk_L1RD0_FILLED',    l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=LowMuGroup+MinBiasGroup),
        ChainProp(name='HLT_mb_sptrk_L1MBTS_1', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sp_L1RD0_FILLED',       l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=LowMuGroup+MinBiasGroup),
        ChainProp(name='HLT_mb_sp15_pusup0_trk5_hmt_L1RD0_FILLED',          l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt2_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt2_L1MBTS_2', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt4_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt4_L1MBTS_2', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt6_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt6_L1MBTS_2', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt8_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt8_L1MBTS_2', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt2_L1AFP_A_OR_C', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt4_L1AFP_A_OR_C', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt6_L1AFP_A_OR_C', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt8_L1AFP_A_OR_C', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt2_L1AFP_A_AND_C', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt4_L1AFP_A_AND_C', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt6_L1AFP_A_AND_C', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_sptrk_pt8_L1AFP_A_AND_C', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_alfaperf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['DISCARD'],     groups=['PS:Online']+LowMuGroup+MinBiasGroup),
        ChainProp(name='HLT_mb_alfaperf_L1RD0_EMPTY',  l1SeedThresholds=['FSNOSEED'], stream=['DISCARD'],     groups=['PS:Online']+LowMuGroup+MinBiasGroup),
        ChainProp(name="HLT_mb_mbts_L1MBTS_1_EMPTY",               l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+LowMuGroup),
        ChainProp(name="HLT_mb_mbts_L1MBTS_1",                     l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+LowMuGroup),
        ChainProp(name="HLT_mb_mbts_L1MBTS_1_1",                   l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+LowMuGroup),
        ChainProp(name="HLT_mb_mbts_L1MBTS_2",                     l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+LowMuGroup),
        ChainProp(name="HLT_mb_mbts_L1RD0_FILLED",                 l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+LowMuGroup),
        ChainProp(name="HLT_mb_mbts_L1RD0_EMPTY",                  l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+LowMuGroup),
    
        # AFP
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_J20', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_J20', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_J30', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_J30', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_J50', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_J50', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_J75', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_J75', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        
        # Phase I jet inputs ATR-24411
        # AFP
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_jJ20', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_jJ20', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_jJ30', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_jJ30', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_jJ50', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_jJ50', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_jJ75', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),
        ChainProp(name='HLT_mb_afprec_L1AFP_A_AND_C_TOF_T0T1_jJ75', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online']+MinBiasGroup+LowMuGroup),

    ]

    chains['Monitor'] = [
        ChainProp(name='HLT_noalg_CostMonDS_L1All',        l1SeedThresholds=['FSNOSEED'], stream=['CostMonitoring'], groups=['Primary:CostAndRate', 'RATE:Monitoring', 'BW:Other']), # HLT_costmonitor
    ]

    chains['Calib'] += [
        
        ChainProp(name='HLT_larpsall_L1J15', l1SeedThresholds=['J15'], stream=['CosmicCalo'],groups=['Support:Legacy','RATE:Calibration','BW:Detector']),

        # Phase I jet inputs ATR-24411, seed needs to be checked
        #ChainProp(name='HLT_larpsall_L1jJ15', l1SeedThresholds=['jJ15'], stream=['CosmicCalo'],groups=['Support:PhaseI','RATE:Calibration','BW:Detector']),

    ]

    chains['UnconventionalTracking'] += [

        # hit-based DV                                 
        ChainProp(name='HLT_unconvtrk260_hitdv_tight_L1J100', groups=PrimaryLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk260_hitdv_medium_L1J100', groups=SupportLegGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']),

        # Phase I jet inputs ATR-24411
        # hit-based DV                                 
        ChainProp(name='HLT_unconvtrk260_hitdv_tight_L1jJ100', groups=PrimaryPhIGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']),
        ChainProp(name='HLT_unconvtrk260_hitdv_medium_L1jJ100', groups=SupportPhIGroup+UnconvTrkGroup, l1SeedThresholds=['FSNOSEED']),
        
    ]

    chains['Streaming'] += [
        ChainProp(name='HLT_noalg_L1J400',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+JetStreamersGroup+['BW:Other']), # catch all high-Et
        ChainProp(name='HLT_noalg_L1XE300', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=PrimaryLegGroup+METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE30',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=SupportLegGroup+METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE35',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=SupportLegGroup+METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE40',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=SupportLegGroup+METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE45',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=SupportLegGroup+METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE50',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=SupportLegGroup+METStreamersGroup),
        
        # Phase I jet inputs ATR-24411
        ChainProp(name='HLT_noalg_L1jJ400', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=PrimaryPhIGroup+JetStreamersGroup+['BW:Other']), # catch all high-Et

    ]

    # check all chains are classified as either primary, support or T&P chains
    for sig in chains:
        for chain in chains[sig]:
            groupFound = False
            for group in chain.groups:
                if 'Primary' in group or 'Support' in group or 'EOF' in group:
                   groupFound = True
            if not groupFound:
                log.error("chain %s in Physics menu needs to be classified as either primary or support chain", chain.name)
                raise RuntimeError("Add either the primary or support group to the chain %s",chain.name)

    return chains
