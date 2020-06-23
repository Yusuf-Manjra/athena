# ******************************************************
# TAUP3.py 
# reductionConf flag TAUP3 in Reco_tf.py   
# ******************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()


# =============================================
# Private sequence here
# =============================================
TAUP3seq = CfgMgr.AthSequencer("TAUP3Sequence")

#==================================================================== 
# JetTagNonPromptLepton decorations 
#==================================================================== 
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
# Build AntiKt4PV0TrackJets and run b-tagging 
JetTagConfig.ConfigureAntiKt4PV0TrackJets(TAUP3seq, 'TAUP3')
# Add BDT decoration algs 
TAUP3seq += JetTagConfig.GetDecoratePromptLeptonAlgs(name="Muons")
TAUP3seq += JetTagConfig.GetDecoratePromptTauAlgs()

# Add private sequence to the DerivationFrameworkJob
DerivationFrameworkJob += TAUP3seq


#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = TAUP3seq)

# =============================================
# Set up stream
# =============================================
streamName      = derivationFlags.WriteDAOD_TAUP3Stream.StreamName
fileName        = buildFileName( derivationFlags.WriteDAOD_TAUP3Stream )
TAUP3Stream     = MSMgr.NewPoolRootStream( streamName, fileName )
TAUP3Stream.AcceptAlgs(["TAUP3Kernel"])


# =============================================
# Thinning tool
# =============================================

# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream   = MSMgr.GetStream( streamName )
evtStream   = augStream.GetEventStream()
svcMgr     += createThinningSvc(
  svcName                   = "TAUP3ThinningSvc",
  outStreams                = [evtStream])

thinningTools = []

# MET/Jet tracks
thinning_expression      = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
TAUP3MetTPThinningTool   = DerivationFramework__TrackParticleThinning(
  name                      = "TAUP3MetTPThinningTool",
  ThinningService           = "TAUP3ThinningSvc",
  SelectionString           = thinning_expression,
  InDetTrackParticlesKey    = "InDetTrackParticles",
  ApplyAnd                  = True)

ToolSvc  += TAUP3MetTPThinningTool
thinningTools.append(TAUP3MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
TAUP3JetTPThinningTool    = DerivationFramework__JetTrackParticleThinning(
  name                      = "TAUP3JetTPThinningTool",
  ThinningService           = "TAUP3ThinningSvc",
  JetKey                    = "AntiKt4LCTopoJets",
  InDetTrackParticlesKey    = "InDetTrackParticles",
  ApplyAnd                  = True)

ToolSvc  += TAUP3JetTPThinningTool
thinningTools.append(TAUP3JetTPThinningTool)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
TAUP3CaloClusterThinning  = DerivationFramework__CaloClusterThinning(
  name                            = "TAUP3ClusterThinning",
  ThinningService                 = "TAUP3ThinningSvc",
  SGKey                           = "TauJets",
  TopoClCollectionSGKey           = "LCOriginTopoClusters",
  AdditionalTopoClCollectionSGKey = ["CaloCalTopoClusters"],
  ConeSize                        = 0.6)

ToolSvc += TAUP3CaloClusterThinning
thinningTools.append(TAUP3CaloClusterThinning)

# Tracks associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
TAUP3ElectronTPThinningTool= DerivationFramework__EgammaTrackParticleThinning(
  name                      = "TAUP3ElectronTPThinningTool",
  ThinningService           = "TAUP3ThinningSvc",
  SGKey                     = "Electrons",
  InDetTrackParticlesKey    = "InDetTrackParticles")

ToolSvc += TAUP3ElectronTPThinningTool
thinningTools.append(TAUP3ElectronTPThinningTool)

# Tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
TAUP3MuonTPThinningTool    = DerivationFramework__MuonTrackParticleThinning(
  name                      = "TAUP3MuonTPThinningTool",
  ThinningService           = "TAUP3ThinningSvc",
  MuonKey                   = "Muons",
  InDetTrackParticlesKey    = "InDetTrackParticles")

ToolSvc += TAUP3MuonTPThinningTool
thinningTools.append(TAUP3MuonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
TAUP3TauTPThinningTool     = DerivationFramework__TauTrackParticleThinning(
  name                      = "TAUP3TauTPThinningTool",
  ThinningService           = "TAUP3ThinningSvc",
  TauKey                    = "TauJets",
  InDetTrackParticlesKey    = "InDetTrackParticles",
  ConeSize                  = 0.6)

ToolSvc += TAUP3TauTPThinningTool
thinningTools.append(TAUP3TauTPThinningTool)

# truth thinning here:
import DerivationFrameworkTau.TAUPThinningHelper 
TAUP3TruthThinningTools = DerivationFrameworkTau.TAUPThinningHelper.setup("TAUP3",
                                                                     "TAUP3ThinningSvc",
                                                                     ToolSvc)

thinningTools += TAUP3TruthThinningTools


# =============================================
# Skimming tool
# =============================================
muRequirement    = "( count( (Muons.pt > 22.0*GeV) && Muons.DFCommonGoodMuon && (Muons.muonType==0)) >= 1 )"
tauRequirement   = "( count( ((TauJets.pt > 18.0*GeV || TauJets.ptFinalCalib > 18.0*GeV) &&  (abs(TauJets.charge)==1.0 && (TauJets.nTracks == 1 || TauJets.nTracks == 3)) ) || (TauJets.nTracks == 2 && TauJets.BDTJetScoreSigTrans > 0.03 && (TauJets.pt > 27.0*GeV || TauJets.ptFinalCalib > 27.0*GeV) ) ) >= 1 )"

expression       = muRequirement + " && " + tauRequirement

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TAUP3SkimmingTool    = DerivationFramework__xAODStringSkimmingTool(
  name                      = "TAUP3SkimmingTool",
  expression                = expression)

ToolSvc   += TAUP3SkimmingTool

# =============================================
# Create derivation Kernel
# =============================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

TAUP3seq += CfgMgr.DerivationFramework__DerivationKernel(
  "TAUP3Kernel",
  SkimmingTools             = [TAUP3SkimmingTool],
  ThinningTools             = thinningTools
  )


# ======================================================================
# Add the containers to the output stream (slimming done here)
# ======================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTau.TAUPExtraContent import *

TAUP3SlimmingHelper        = SlimmingHelper("TAUP3SlimmingHelper")
TAUP3SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4LCTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4LCTopoJets",
                                        "AntiKt4EMPFlowJets",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"]

if DerivationFrameworkIsMonteCarlo:
  TAUP3SlimmingHelper.StaticContent  = ["xAOD::TruthParticleContainer#TruthElectrons",
                                        "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
                                        "xAOD::TruthParticleContainer#TruthMuons",
                                        "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
                                        "xAOD::TruthParticleContainer#TruthTaus",
                                        "xAOD::TruthParticleAuxContainer#TruthTausAux.",
                                        "xAOD::TruthParticleContainer#TruthPhotons",
                                        "xAOD::TruthParticleAuxContainer#TruthPhotonsAux.",
                                        "xAOD::TruthParticleContainer#TruthNeutrinos",
                                        "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux."]
  TAUP3SlimmingHelper.SmartCollections += ["AntiKt4TruthJets"]

TAUP3SlimmingHelper.IncludeMuonTriggerContent    = True
TAUP3SlimmingHelper.IncludeTauTriggerContent     = True
TAUP3SlimmingHelper.IncludeEGammaTriggerContent  = False
TAUP3SlimmingHelper.IncludeEtMissTriggerContent  = False
TAUP3SlimmingHelper.IncludeJetTriggerContent     = False
TAUP3SlimmingHelper.IncludeBJetTriggerContent    = False
# Fix for tau pi0 container
TAUP3SlimmingHelper.AppendToDictionary = {'finalTauPi0sAux':'xAOD::ParticleAuxContainer'}

TAUP3SlimmingHelper.ExtraVariables               = ExtraContentTAUP3
TAUP3SlimmingHelper.AllVariables                 = ExtraContainersTAUP3

# TODO: do these extra b-jet veto variables in TAUPExtraContent to keep this master file cleaner
TAUP3SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptTauVariablesForDxAOD()
TAUP3SlimmingHelper.ExtraVariables += ["Muons.PromptLeptonIso.PromptLeptonVeto"]
TAUP3SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD(name="Muons")

addOriginCorrectedClusters(TAUP3SlimmingHelper, writeLC=True, writeEM=False) 

if globalflags.DataSource() == "geant4":
  TAUP3SlimmingHelper.ExtraVariables            += ExtraContentTruthTAUP3
  TAUP3SlimmingHelper.AllVariables              += ExtraContainersTruthTAUP3

TAUP3SlimmingHelper.AppendContentToStream(TAUP3Stream)

TAUP3Stream.AddItem("xAOD::EventShape#*")
TAUP3Stream.AddItem("xAOD::EventShapeAuxInfo#*")
