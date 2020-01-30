#********************************************************************
# SUSY16.py
# reductionConf flag SUSY16 in Reco_tf.py
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *


### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY16Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY16Stream )
SUSY16Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY16Stream.AcceptAlgs(["SUSY16KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY16ThinningHelper = ThinningHelper( "SUSY16ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY16 = CfgMgr.AthSequencer("SeqSUSY16")
DerivationFrameworkJob += SeqSUSY16

# used both for thinning and for skimming
muonsRequirements = '(Muons.pt > 2.5*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 4.0*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHVeryLoose))'
photonsRequirements = '(abs(Photons.eta)<2.6) && (Photons.pt > 20*GeV)'
if not DerivationFrameworkIsMonteCarlo:
  muonsRequirements = '(Muons.pt > 3.0*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY16TriggerList import triggersNavThin
SUSY16ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY16ThinningHelper.AppendToStream( SUSY16Stream )

#====================================================================
# THINNING TOOLS
#====================================================================

# -------------------------------------------------------------------
# TrackParticle thinning

# ------------------------------------
# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
#
# this one is for just the muon itself.  need this to be more inclusive to be able to study standalone objects.
SUSY16MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY16MuonTPThinningTool",
                                                                          ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY16MuonTPThinningTool
thinningTools.append(SUSY16MuonTPThinningTool)
#
# this one is for tracks around skim-level muons
SUSY16MuonNearbyTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY16MuonNearbyTPThinningTool",
                                                                                ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
                                                                                MuonKey                 = "Muons",
                                                                                ConeSize                = 0.4,
                                                                                SelectionString         = muonsRequirements,
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY16MuonNearbyTPThinningTool
thinningTools.append(SUSY16MuonNearbyTPThinningTool)
# ------------------------------------

# ------------------------------------
# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
#
# this one is for just the electron itself.  need this to be more inclusive to be able to study standalone objects.
SUSY16ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "SUSY16ElectronTPThinningTool",
                                                                                ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
                                                                                SGKey             	= "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY16ElectronTPThinningTool
thinningTools.append(SUSY16ElectronTPThinningTool)
#
# this one is for tracks around skim-level electrons
SUSY16ElectronNearbyTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "SUSY16ElectronNearbyTPThinningTool",
                                                                                      ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
                                                                                      SGKey                   = "Electrons",
                                                                                      ConeSize                = 0.4,
                                                                                      SelectionString         = electronsRequirements,
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY16ElectronNearbyTPThinningTool
thinningTools.append(SUSY16ElectronNearbyTPThinningTool)
# ------------------------------------

# ------------------------------------
# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
#
# this one is for just the photon itself.  need this to be more inclusive to be able to study standalone objects.
SUSY16PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY16PhotonTPThinningTool",
                                                                              ThinningService	         = SUSY16ThinningHelper.ThinningSvc(),
                                                                              SGKey			 = "Photons",
                                                                              InDetTrackParticlesKey     = "InDetTrackParticles")
ToolSvc += SUSY16PhotonTPThinningTool
thinningTools.append(SUSY16PhotonTPThinningTool)
#
# this one is for tracks around skim-level photons
SUSY16PhotonNearbyTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY16PhotonNearbyTPThinningTool",
                                                                                    ThinningService	         = SUSY16ThinningHelper.ThinningSvc(),
                                                                                    SGKey			 = "Photons",
                                                                                    ConeSize                   = 0.4,
                                                                                    SelectionString            = photonsRequirements,
                                                                                    InDetTrackParticlesKey     = "InDetTrackParticles")
ToolSvc += SUSY16PhotonNearbyTPThinningTool
thinningTools.append(SUSY16PhotonNearbyTPThinningTool)
# ------------------------------------

# ------------------------------------
# TrackParticles associated with taus, not keeping tracks in a full cone around the taus (yet)
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY16TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY16TauTPThinningTool",
                                                                         ThinningService = SUSY16ThinningHelper.ThinningSvc(),
                                                                         TauKey          = "TauJets",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY16TauTPThinningTool
thinningTools.append(SUSY16TauTPThinningTool)
# ------------------------------------
# -------------------------------------------------------------------

# -------------------------------------------------------------------
# Cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning

# Keep clusters associated to electrons.
SUSY16ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(
    name                    = 'SUSY16ElectronCCThinningTool',
    ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
    SGKey                   = 'Electrons',
    CaloClCollectionSGKey   = 'egammaClusters',
    TopoClCollectionSGKey   = 'CaloCalTopoClusters',
    SelectionString         = electronsRequirements,
    ConeSize                = 0.4)
ToolSvc += SUSY16ElectronCCThinningTool
thinningTools.append(SUSY16ElectronCCThinningTool)

# Keep clusters associated to photons, but only in a very small cone so we get the photon cluster itself.
# Don't currently keep clusters assocuated with photons far away from the photons, e.g. to recalc isolation.
SUSY16PhotonCCThinningTool = DerivationFramework__CaloClusterThinning(
    name                    = 'SUSY16PhotonCCThinningTool',
    ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
    SGKey                   = 'Photons',
    CaloClCollectionSGKey   = 'egammaClusters',
    TopoClCollectionSGKey   = 'CaloCalTopoClusters',
    ConeSize                = 0.1)
ToolSvc += SUSY16PhotonCCThinningTool
thinningTools.append(SUSY16PhotonCCThinningTool)


# Calo Clusters associated with Muons
SUSY16MuonCCThinningTool = DerivationFramework__CaloClusterThinning(
    name = "SUSYxMuonCCThinningTool",
    ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
    SGKey                   = "Muons",
    TopoClCollectionSGKey   = "CaloCalTopoClusters",
    SelectionString         = muonsRequirements,
    ConeSize                = 0.4)
ToolSvc += SUSY16MuonCCThinningTool
thinningTools.append(SUSY16MuonCCThinningTool)
# -------------------------------------------------------------------


#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    SUSY16TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY16TruthThinningTool",
                                                                     ThinningService              = SUSY16ThinningHelper.ThinningSvc(),
                                                                     WritePartons                 = False,
                                                                     WriteHadrons                 = False,
                                                                     WriteBHadrons                = True,
                                                                     WriteGeant                   = False,
                                                                     GeantPhotonPtThresh          = 20000,
                                                                     WriteTauHad                  = True,
                                                                     PartonPtThresh               = -1.0,
                                                                     WriteBSM                     = True,
                                                                     WriteBosons                  = True,
                                                                     WriteBosonProducts           = True,
                                                                     WriteBSMProducts             = True,
                                                                     WriteTopAndDecays            = True,
                                                                     WriteEverything              = False,
                                                                     WriteAllLeptons              = True,
                                                                     WriteLeptonsNotFromHadrons   = False,
                                                                     WriteStatus3                 = False,
                                                                     WriteFirstN                  = 10,
                                                                     PreserveAncestors            = True,
                                                                     PreserveGeneratorDescendants = False,
                                                                     SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)


    # Decorate Electron with bkg electron type/origin
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
    BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",MCTruthClassifierTool = BkgElectronMCTruthClassifier)
    ToolSvc += BkgElectronClassificationTool
    AugmentationTools.append(BkgElectronClassificationTool)

    ToolSvc += SUSY16TruthThinningTool
    thinningTools.append(SUSY16TruthThinningTool)


#====================================================================
# SKIMMING TOOL
#====================================================================

# ------------------------------------------------------------
# Lepton selection
leptonSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 2)'
expression='('+leptonSelection+')'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY16LeptonSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY16LeptonSkimmingTool",
                                                                        expression = expression)
ToolSvc += SUSY16LeptonSkimmingTool
# ------------------------------------------------------------

# ------------------------------------------------------------
# Trigger selection
from DerivationFrameworkSUSY.SUSY16TriggerList import triggersMET,triggersSoftMuon,triggersJetPlusMet,triggersSoftMuonEmulation,triggersTriLep,triggersSingleLep,triggersDiLep
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool,DerivationFramework__FilterCombinationOR,DerivationFramework__FilterCombinationAND


# JetMET trigger name contained ' - ' cause crash when using xAODStringSkimmingTool
trigReq=triggersMET+triggersSoftMuon+triggersJetPlusMet
SUSY16InclusiveTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY16InclusiveTriggerSkimmingTool",
                                                                               TriggerListOR = trigReq)
ToolSvc += SUSY16InclusiveTriggerSkimmingTool

# full trigger selection, do it slightly differently for data and MC
SUSY16TriggerSkimmingTool=None
if DerivationFrameworkIsMonteCarlo:
    # one muon + jet + met trigger
    SUSY16SoftOneMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY16OneMuonTriggerSkimmingTool",
                                                                                     TriggerListAND = ['HLT_mu4','HLT_xe50_mht','HLT_j110'])
    ToolSvc += SUSY16SoftOneMuonTriggerSkimmingTool

    # dimuon + jet + met trigger
    SUSY16SoftTwoMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY16TwoMuonTriggerSkimmingTool",
                                                                                     TriggerListAND = ['HLT_2mu4','HLT_j85'])
    ToolSvc += SUSY16SoftTwoMuonTriggerSkimmingTool

    # OR of soft muon stuff or inclusive MET triggers
    SUSY16TriggerSkimmingTool = DerivationFramework__FilterCombinationOR(name = "SUSY16TriggerSkimmingTool",
                                                                         FilterList = [SUSY16InclusiveTriggerSkimmingTool,
                                                                                       SUSY16SoftOneMuonTriggerSkimmingTool,
                                                                                       SUSY16SoftTwoMuonTriggerSkimmingTool])
    ToolSvc += SUSY16TriggerSkimmingTool
else:
    # for data we can keep all of the prescaled triggers, may be nice to have them.
    SUSY16SoftMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY16SoftMuonTriggerSkimmingTool",
                                                                                     TriggerListOR = triggersSoftMuonEmulation)
    ToolSvc += SUSY16SoftMuonTriggerSkimmingTool

    # OR of soft muon stuff or inclusive MET triggers
    SUSY16TriggerSkimmingTool = DerivationFramework__FilterCombinationOR(name = "SUSY16TriggerSkimmingTool",
                                                                         FilterList = [SUSY16InclusiveTriggerSkimmingTool,
                                                                                       SUSY16SoftMuonTriggerSkimmingTool])
    ToolSvc += SUSY16TriggerSkimmingTool

# ------------------------------------------------------------

# ------------------------------------------------------------
# Final MET-based skim selection, with trigger selection and lepton selection
SUSY16SkimmingTool_MET = DerivationFramework__FilterCombinationAND(name = "SUSY16SkimmingTool_MET",
                                                                   FilterList = [SUSY16LeptonSkimmingTool,
                                                                                 SUSY16TriggerSkimmingTool])
ToolSvc += SUSY16SkimmingTool_MET

# Trilepton final skim
includeTrileptonEvents=False
if includeTrileptonEvents:
    # trilepton selection
    trileptonSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 3)'
    SUSY16TriLeptonSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY16LeptonSkimmingTool",
                                                                               expression = '('+trileptonSelection+')')
    ToolSvc += SUSY16TriLeptonSkimmingTool

    # lepton-based triggers done separately, will use separate lepton multiplicity requirements for those
    trileptrigReq=triggersSingleLep+triggersDiLep+triggersTriLep
    SUSY16TriLepTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY16TriLepTriggerSkimmingTool",
                                                                                TriggerListOR = trileptrigReq)
    ToolSvc += SUSY16TriLepTriggerSkimmingTool

    # Trilepton trigger+lepton selection
    SUSY16SkimmingTool_TriLep = DerivationFramework__FilterCombinationAND(name = "SUSY16SkimmingTool_TriLep",
                                                                          FilterList = [SUSY16TriLeptonSkimmingTool,
                                                                                        SUSY16TriLepTriggerSkimmingTool])
    ToolSvc += SUSY16SkimmingTool_TriLep

    # and the final-final selection is an OR of the trilepton selection and the inclusive-MET selection:
    SUSY16SkimmingTool = DerivationFramework__FilterCombinationOR(name = "SUSY16SkimmingTool",
                                                                  FilterList = [SUSY16SkimmingTool_MET,
                                                                                SUSY16SkimmingTool_TriLep])
    ToolSvc += SUSY16SkimmingTool
else:
    # just using inclusive MET
    SUSY16SkimmingTool = SUSY16SkimmingTool_MET

# ------------------------------------------------------------

#====================================================================
# Max Cell sum decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
SUSY16_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "SUSY16_MaxCellDecoratorTool",
                                                                     SGKey_electrons         = "Electrons",
                                                                     SGKey_photons           = "Photons",
                                                                     )
ToolSvc += SUSY16_MaxCellDecoratorTool

# May want to add this in at some point for isolated-track selection studies?
if False:
    #====================================================================
    # ISOLATION TOOL
    #====================================================================
    #Track selection
    from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
    TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
    TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
    TrackIsoTool.TrackSelectionTool.minPt= 1000.
    TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
    ToolSvc += TrackIsoTool

    TrackIsoTool500 = xAOD__TrackIsolationTool("TrackIsoTool500")
    TrackIsoTool500.TrackSelectionTool.maxZ0SinTheta= 3.
    TrackIsoTool500.TrackSelectionTool.minPt= 500.
    TrackIsoTool500.TrackSelectionTool.CutLevel= "Loose"
    ToolSvc += TrackIsoTool500

    from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
    import ROOT, PyCintex
    PyCintex.loadDictionary('xAODCoreRflxDict')
    PyCintex.loadDictionary('xAODPrimitivesDict')
    isoPar = ROOT.xAOD.Iso
    Pt1000IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt1000IsoTrackDecorator",
                                                                    TrackIsolationTool = TrackIsoTool,
                                                                    TargetContainer = "InDetTrackParticles",
                                                                    ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                    Prefix = 'TrkIsoPt1000_'
                                                                   )
    Pt500IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt500IsoTrackDecorator",
                                                                    TrackIsolationTool = TrackIsoTool500,
                                                                    TargetContainer = "InDetTrackParticles",
                                                                    ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                    Prefix = 'TrkIsoPt500_'
                                                                   )
    ToolSvc += Pt1000IsoTrackDecorator
    ToolSvc += Pt500IsoTrackDecorator

    AugmentationTools.append(Pt1000IsoTrackDecorator)
    AugmentationTools.append(Pt500IsoTrackDecorator)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# SUSY signal augmentation (before skimming!)
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():

    from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
    SeqSUSY16 += CfgMgr.DerivationFramework__DerivationKernel("SUSY16KernelSigAug",
                                                              AugmentationTools = DecorateSUSYProcess("SUSY16")
                                                              )

    from DerivationFrameworkSUSY.SUSYWeightMetadata import *

#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY16 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY16KernelSkim",
    SkimmingTools = [SUSY16SkimmingTool]
    )


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY16)

#==============================================================================
OutputJets["SUSY16"] = []
reducedJetList = [ "AntiKt2PV0TrackJets" ]
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]

# AntiKt2PV0TrackJets is flavour-tagged automatically
replaceAODReducedJets(reducedJetList, SeqSUSY16, "SUSY16")


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY16 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY16KernelAug",
    AugmentationTools = AugmentationTools,
    ThinningTools = thinningTools,
    )

#====================================================================
# Prompt Lepton Tagger
#====================================================================

import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# simple call to replaceAODReducedJets(["AntiKt4PV0TrackJets"], SeqSUSY16, "SUSY16")
JetTagConfig.ConfigureAntiKt4PV0TrackJets(SeqSUSY16, "SUSY16")

# add decoration
SeqSUSY16 += JetTagConfig.GetDecoratePromptLeptonAlgs(addSpectators=True)
SeqSUSY16 += JetTagConfig.GetDecoratePromptTauAlgs()


#====================================================================
# CONTENT LIST
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY16SlimmingHelper = SlimmingHelper("SUSY16SlimmingHelper")
SUSY16SlimmingHelper.SmartCollections = ["Electrons",
                                         "Photons",
                                         "Muons",
                                         "TauJets",
                                         "AntiKt4EMTopoJets",
                                         "AntiKt4EMPFlowJets",
                                         "MET_Reference_AntiKt4EMTopo",
                                         "MET_Reference_AntiKt4EMPFlow",
                                         #"BTagging_AntiKt4EMTopo",
                                         #"BTagging_AntiKt4EMPFlow",
                                         "AntiKt4EMPFlowJets_BTagging201810",
                                         "AntiKt4EMPFlowJets_BTagging201903",
                                         "BTagging_AntiKt4EMPFlow_201810",
                                         "BTagging_AntiKt4EMPFlow_201903",
                                         "AntiKt4EMTopoJets_BTagging201810",
                                         "BTagging_AntiKt4EMTopo_201810",
                                         "InDetTrackParticles",
                                         "PrimaryVertices"]
SUSY16SlimmingHelper.AllVariables = ["TruthParticles",
                                     "TruthEvents",
                                     "TruthVertices",
                                     "MET_Track",
                                     "MET_LocHadTopo",
                                     "MuonSegments",
                                     "MET_Truth"]
SUSY16SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo_201810.MV1_discriminant.MV1c_discriminant",
                                      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag.eta_sampl.phi_sampl.etcone20.ptconecoreTrackPtrCorrection",
                                      "MuonClusterCollection.eta_sampl.phi_sampl",
                                      "Photons.author.Loose.Tight",
                                      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi.DFCommonJets_jetClean_VeryLooseBadLLP",
                                      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix","CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink.PanTau_isPanTauCandidate.ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased",
                                      "TauNeutralParticleFlowObjects.pt.eta.phi.m",
                                      "TauChargedParticleFlowObjects.pt.eta.phi.m",
                                      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                      "Electrons.bkgTruthType.bkgTruthOrigin.firstEgMotherPdgId.truthPdgId.bkgTruthParticleLink.bkgMotherPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherTruthParticleLink.etcone20.ptconecoreTrackPtrCorrection",
                                      "AntiKt2PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
#                                       "BTagging_AntiKt2Track.MV2c10_discriminant",
                                      "CaloCalTopoClusters.rawE.rawEta.rawPhi.rawM.calE.calEta.calPhi.calM.e_sampl"
]

# Saves BDT and input variables for light lepton algorithms.
# Can specify just electrons or just muons by adding 'name="Electrons"' or 'name="Muons"' as the argument.
SUSY16SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD(addSpectators=True)
# Saves BDT and input variables tau algorithm
SUSY16SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptTauVariablesForDxAOD()


SUSY16SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY16SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY16SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY16SlimmingHelper.IncludeJetTriggerContent    = True
SUSY16SlimmingHelper.IncludeTauTriggerContent    = True
SUSY16SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY16SlimmingHelper.IncludeBJetTriggerContent   = False

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY16SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}

  SUSY16SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]


SUSY16SlimmingHelper.AppendContentToStream(SUSY16Stream)


