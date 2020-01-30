#********************************************************************
# STDM4.py
# reductionConf flag STDM4 in Reco_tf.py
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.PFlowCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
scheduleTauTruthTools()
from DerivationFrameworkSM import STDMTriggers
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

# Add Truth MetaData
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import *

#===========================================================================================\
# AUGMENTATION  TOOL
#===========================================================================================\
from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__PFlowAugmentationTool
STDM4_PFlowAugmentationTool = DerivationFramework__PFlowAugmentationTool(name = "STDM4_PFlowAugmentationTool")
ToolSvc += STDM4_PFlowAugmentationTool


#====================================================================
# SET UP STREAM
#====================================================================

streamName = derivationFlags.WriteDAOD_STDM4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM4Stream )
STDM4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM4Stream.AcceptAlgs(["STDM4Kernel"])

isMC = globalflags.DataSource()=='geant4'

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools=[]

#=====================
# TRIGGER NAV THINNING
#=====================

# Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
STDM4ThinningHelper = ThinningHelper( "STDM4ThinningHelper" )

#trigger navigation content
#STDM4ThinningHelper.TriggerChains = 'REMOVEALLTHETRIGGERS'
STDM4ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*'
STDM4ThinningHelper.AppendToStream( STDM4Stream )


#=====================
# TRACK  THINNING
#=====================


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
STDM4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "STDM4MuonTPThinningTool",
                                                                         ThinningService         = STDM4ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM4MuonTPThinningTool
thinningTools.append(STDM4MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(      name                    = "STDM4ElectronTPThinningTool",
                                                                                     ThinningService         = STDM4ThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "Electrons",
                                                                                     InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM4ElectronTPThinningTool
thinningTools.append(STDM4ElectronTPThinningTool)


# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
STDM4TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                 = "STDM4TauTPThinningTool",
                                                                        ThinningService         = STDM4ThinningHelper.ThinningSvc(),
                                                                        TauKey                  = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM4TauTPThinningTool
thinningTools.append(STDM4TauTPThinningTool)

STDM4PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "STDM4PhotonTPThinningTool",
                                                                        ThinningService         = STDM4ThinningHelper.ThinningSvc(),
                                                                        SGKey                   = "Photons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM4PhotonTPThinningTool
thinningTools.append(STDM4PhotonTPThinningTool)

# Truth leptons and their ancestors and descendants
truth_cond_boson = "((abs(TruthParticles.pdgId) == 23) || (abs(TruthParticles.pdgId) == 24))"
truth_cond_lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) &&(TruthParticles.pt > 1*GeV) && (TruthParticles.status ==1) && (TruthParticles.barcode<200000))"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning


if globalflags.DataSource()=='geant4':
    from DerivationFrameworkSM.STDMCommonTruthTools import *

    STDM4TruthLepTool = DerivationFramework__GenericTruthThinning(name                         = "STDM4TruthLepTool",
                                                                  ThinningService              = STDM4ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_lepton,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = False,
                                                                  PreserveAncestors            = True)

    STDM4TruthBosTool = DerivationFramework__GenericTruthThinning(name                         = "STDM4TruthBosTool",
                                                                  ThinningService              = STDM4ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_boson,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = True,
                                                                  PreserveAncestors            = False)

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    STDM4TruthThinning = DerivationFramework__MenuTruthThinning(name                       = "STDM4TruthThinning",
                                                                ThinningService            = STDM4ThinningHelper.ThinningSvc(),
                                                                WritePartons               = False,
                                                                WriteHadrons               = False,
                                                                WriteBHadrons              = True,
                                                                WriteCHadrons              = True,
                                                                WritettHFHadrons           = True,
                                                                WriteGeant                 = False,
                                                                GeantPhotonPtThresh        = -1.0,
                                                                WriteTauHad                = True,
                                                                PartonPtThresh             = -1.0,
                                                                WriteBSM                   = True,
                                                                WriteBosons                = True,
                                                                WriteBSMProducts           = True,
                                                                WriteBosonProducts         = True,
                                                                WriteTopAndDecays          = True,
                                                                WriteEverything            = False,
                                                                WriteAllLeptons            = True,
                                                                WriteStatus3               = True,
                                                                PreserveDescendants        = False,
                                                                PreserveGeneratorDescendants = False,
                                                                PreserveAncestors          = True,
                                                                WriteFirstN                = 10)

    STDM4PhotonThinning = DerivationFramework__GenericTruthThinning(name                    = "STDM4PhotonThinning",
                                                                    ThinningService         = STDM4ThinningHelper.ThinningSvc(),
                                                                    ParticlesKey            = "STDMTruthPhotons",
                                                                    ParticleSelectionString = STDMphotonthinningexpr)


    ToolSvc += STDM4TruthLepTool
    ToolSvc += STDM4TruthBosTool
    ToolSvc += STDM4TruthThinning
    ToolSvc += STDM4PhotonThinning
    thinningTools.append(STDM4TruthLepTool)
    thinningTools.append(STDM4TruthBosTool)
    thinningTools.append(STDM4TruthThinning)
    thinningTools.append(STDM4PhotonThinning)


#====================================================================
# SKIMMING TOOL
#====================================================================

muonsRequirements = '(Muons.pt >= 15*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 20*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose)'
offlineexpression = '(count('+electronsRequirements+') + count('+muonsRequirements+')) >= 1'

singleElectronTriggerRequirement = STDMTriggers.list_combine_OR(STDMTriggers.single_e_triggers)
singleMuonTriggerRequirement     = STDMTriggers.list_combine_OR(STDMTriggers.single_mu_triggers)
triggerRequirement='('+singleElectronTriggerRequirement+'||'+singleMuonTriggerRequirement+')'
expression = triggerRequirement+' || '+offlineexpression


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM4SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "STDM4SkimmingTool",
                                                               expression = expression)
ToolSvc += STDM4SkimmingTool



#Augmentation

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
# Trigger_match_List=["HLT_e24_medium_iloose_L1EM18VH",
#              "HLT_e24_medium_iloose_L1EM20VH","HLT_e24_tight_iloose_L1EM20VH",
#              "HLT_e24_tight_iloose","HLT_e26_tight_iloose",
#              "HLT_e60_medium","HLT_e120_loose","HLT_e140_loose",
#              "HLT_e24_lhmedium_iloose_L1EM18VH",
#              "HLT_e24_lhmedium_iloose_L1EM20VH",
#              "HLT_e24_lhtight_iloose_L1EM20VH","HLT_e24_lhtight_iloose",
#              "HLT_e26_lhtight_iloose","HLT_e60_lhmedium",
#              "HLT_e120_lhloose","HLT_e140_lhloose","HLT_e24_lhmedium_L1EM20VH",
#              "HLT_e60_lhmedium","HLT_e120_lhloosemc",
#              "HLT_e24_lhmedium_L1EM18VH","HLT_e60_lhmedium","HLT_e120_lhloose",
#              "HLT_mu20_iloose_L1MU15","HLT_mu24_imedium","HLT_mu26_imedium",
#              "HLT_mu50"]

Trigger_di_el_list=["HLT_2e12_loose_L12EM10VH","HLT_2e15_loose_L12EM13VH","HLT_2e17_loose","HLT_2e17_loose_L12EM15","HLT_2e12_lhloose_L12EM10VH","HLT_2e15_lhloose_L12EM13VH","HLT_2e17_lhloose","HLT_2e17_lhloose_L12EM15","HLT_2e17_lhvloose_nod0","HLT_2e12_lhvloose_L12EM10VH"]
Trigger_di_mu_list=["HLT_2mu10","HLT_2mu14","HLT_mu24_mu8noL1","HLT_mu22_mu8noL1","HLT_mu20_mu8noL1"]

# temporarily remove trigger augmentation to work in rel21
#from DerivationFrameworkSM.DerivationFrameworkSMConf import DerivationFramework__TriggerMatchingAugmentation
#STDM4_TriggerMatchingAugmentation=DerivationFramework__TriggerMatchingAugmentation(
"""
                                                             name = "STDM4_TriggerMatchingAugmentation",
                                                             DecorationPrefix = "STDM4_",
                                                             PhotonContainerName = "Photons",
                                                             ElectronContainerName = "Electrons",
                                                             MuonContainerName = "Muons",
                                                             SingleTriggerList = STDMTriggers.single_e_triggers+STDMTriggers.single_mu_triggers,
                                                             DiMuonList = Trigger_di_mu_list,
                                                             DiElectronList = Trigger_di_el_list
                                                             )"""
#ToolSvc += STDM4_TriggerMatchingAugmentation
NewTrigVars=[]
for contain in ["Electrons","Photons","Muons"]:
    new_content=".".join(["STDM4_"+t for t in STDMTriggers.single_e_triggers +
                          STDMTriggers.single_mu_triggers +
                          Trigger_di_el_list +
                          Trigger_di_mu_list])
    NewTrigVars.append(contain+"."+new_content)


#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
STDM4_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name = "STDM4_MaxCellDecoratorTool",
                                                                    SGKey_electrons = "Electrons",
                                                                    SGKey_photons   = "Photons"
                                                                  )
ToolSvc += STDM4_MaxCellDecoratorTool


from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE
STDM4Sequence = CfgMgr.AthSequencer("STDM4Sequence")

# ADD KERNEL
STDM4Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM4Kernel",
                                                              SkimmingTools = [STDM4SkimmingTool],
                                                              ThinningTools = thinningTools,
# removed temporarily to build in rel21
#                                                              AugmentationTools=[STDM4_PFlowAugmentationTool,STDM4_TriggerMatchingAugmentation])
                                                              AugmentationTools=[STDM4_PFlowAugmentationTool,STDM4_MaxCellDecoratorTool])


# JET REBUILDING
reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt4TruthJets", "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList, STDM4Sequence, "STDM4Jets")

# FAKE LEPTON TAGGER
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
STDM4Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()

# ADD SEQUENCE TO JOB
DerivationFrameworkJob += STDM4Sequence


##############
# PFlow augmentation
applyPFOAugmentation(STDM4Sequence)

#=======================================
# SCHEDULE MET RECONSTRUCTION
#=======================================
# add map with modified association method and make MET:
addHadRecoilMETMap(STDM4Sequence, STDM4Stream, "STDM4")
##############




#====================================================================
# SET UP STREAM
#====================================================================
#streamName = derivationFlags.WriteDAOD_STDM4Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_STDM4Stream )
#STDM4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
#STDM4Stream.AcceptAlgs(["STDM4Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="STDM4ThinningSvc", outStreams=[evtStream] )


# Add fatjets (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets and AntiKt10TruthTrimmedPtFrac5SmallR20Jets)
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(STDM4Sequence, "STDM4")

#====================================================================
# Jet reconstruction/retagging
#====================================================================

#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = STDM4Sequence)

#q/g tagging
truthjetalg='AntiKt4TruthJets'
if not DerivationFrameworkIsMonteCarlo:
    truthjetalg=None
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addQGTaggerTool
addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=STDM4Sequence,algname="QGTaggerToolAlg",truthjetalg=truthjetalg)
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=STDM4Sequence,algname="QGTaggerToolPFAlg",truthjetalg=truthjetalg) 

#improved fJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import applyMVfJvtAugmentation,getPFlowfJVT
# MVfJvt #
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=STDM4Sequence, algname='JetForwardJvtToolBDTAlg')
# PFlow fJvt #
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=STDM4Sequence, algname='JetForwardPFlowJvtToolAlg')

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM4SlimmingHelper = SlimmingHelper("STDM4SlimmingHelper")
STDM4SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMPFlowJets",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "AntiKt4EMPFlowJets_BTagging201810", 
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
                                        ]

STDM4SlimmingHelper.IncludeEGammaTriggerContent = True
STDM4SlimmingHelper.IncludeMuonTriggerContent = True

# removed end of line ot build in rel21
STDM4SlimmingHelper.ExtraVariables = ExtraContentAll + [
  "AntiKt4EMTopoJets.SumPtTrkPt1000.SumPtTrkPt500.NumTrkPt500.NumTrkPt1000",
  "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m",
  "Electrons.UEcorr_Pt", "Muons.UEcorr_Pt",
  "Electrons.Reta.Rphi.Rhad1.Rhad.weta2.Eratio.f3.deltaEta1.deltaPhiRescaled2.wtots1.e277.f1.weta1.fracs1.DeltaE",
  "Photons.Reta.Rphi.Rhad1.Rhad.weta2.Eratio.deltaEta1.deltaPhiRescaled2.wtots1.e277.f1.weta1.fracs1.DeltaE"
]

STDM4SlimmingHelper.ExtraVariables += ["Electrons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z"]+["Photons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z"]

STDM4SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
STDM4SlimmingHelper.AllVariables = ExtraContainersAll

#QGTagger
STDM4SlimmingHelper.ExtraVariables += ["AntiKt4EMTopoJets.NumTrkPt500.PartonTruthLabelID.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_eta"]
STDM4SlimmingHelper.ExtraVariables += ["AntiKt4EMPFlowJets.NumTrkPt500.PartonTruthLabelID.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_eta"]


# # btagging variables
from  DerivationFrameworkFlavourTag.BTaggingContent import *

STDM4SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4EMTopoJets")
STDM4SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt2PV0TrackJets")
STDM4SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4EMPFlowJets")

ExtraDictionary["BTagging_AntiKt4EMTopo"]     = "xAOD::BTaggingContainer"
ExtraDictionary["BTagging_AntiKt4EMTopoAux"]  = "xAOD::BTaggingAuxContainer"
ExtraDictionary["BTagging_AntiKt2Track"]      = "xAOD::BTaggingContainer"
ExtraDictionary["BTagging_AntiKt2TrackAux"]   = "xAOD::BTaggingAuxContainer"
ExtraDictionary["BTagging_AntiKt4EMPFlow"]    = "xAOD::BTaggingContainer"
ExtraDictionary["BTagging_AntiKt4EMPFlowAux"] = "xAOD::BTaggingAuxContainer"

if globalflags.DataSource()=='geant4':

    STDM4SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    STDM4SlimmingHelper.AllVariables += ExtraContainersTruth
    STDM4SlimmingHelper.AppendToDictionary.update(ExtraDictionary)

addJetOutputs(STDM4SlimmingHelper,["STDM4","STDM4Jets"])

addMETOutputs(STDM4SlimmingHelper,["AntiKt4EMPFlow"])

STDM4SlimmingHelper.AppendContentToStream(STDM4Stream)
