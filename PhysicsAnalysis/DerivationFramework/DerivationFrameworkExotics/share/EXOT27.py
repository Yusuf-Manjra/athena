################################################################################
# EXOT27 - Official monoHbb+monoScalar+monoVhad derivation
################################################################################

from DerivationFrameworkCore.DerivationFrameworkMaster import (
    derivationFlags, buildFileName, MSMgr, DerivationFrameworkJob,
    DerivationFrameworkIsMonteCarlo)
from DerivationFrameworkCore.DerivationFrameworkCoreConf import (
    DerivationFramework__DerivationKernel)
from DerivationFrameworkTools.DerivationFrameworkToolsConf import (
    DerivationFramework__xAODStringSkimmingTool,
    DerivationFramework__TriggerSkimmingTool,
    DerivationFramework__GenericObjectThinning,
    DerivationFramework__FilterCombinationAND,
    DerivationFramework__FilterCombinationOR)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import (
    DerivationFramework__InDetTrackSelectionToolWrapper,
    DerivationFramework__TrackParticleThinning,
    DerivationFramework__EgammaTrackParticleThinning,
    DerivationFramework__MuonTrackParticleThinning,
    DerivationFramework__TauTrackParticleThinning,
    DerivationFramework__JetTrackParticleThinning)
from ThinningUtils.ThinningUtilsConf import (
    DeltaRThinningTool,
    EleLinkThinningTool,
    ThinAssociatedObjectsTool)
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit


# CP group common variables
import DerivationFrameworkJetEtMiss.JetCommon as JetCommon
import DerivationFrameworkJetEtMiss.ExtendedJetCommon as ExtendedJetCommon
import DerivationFrameworkJetEtMiss.METCommon as METCommon
import DerivationFrameworkEGamma.EGammaCommon as EGammaCommon
import DerivationFrameworkMuons.MuonsCommon as MuonsCommon
import DerivationFrameworkTau.TauCommon as TauCommon
import DerivationFrameworkFlavourTag.HbbCommon as HbbCommon
import DerivationFrameworkInDet.InDetCommon as InDetCommon
from JetRec.JetRecStandardToolManager import jtm
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import (
    DerivationFramework__GenericTruthThinning)
from BTagging.BTaggingFlags import BTaggingFlags
from DerivationFrameworkCore.FullListOfSmartContainers import (
    FullListOfSmartContainers)
# Make sure all the normal truth stuff is there
if DerivationFrameworkIsMonteCarlo:
  import DerivationFrameworkMCTruth.MCTruthCommon as MCTruthCommon
  MCTruthCommon.addStandardTruthContents()

import DerivationFrameworkExotics.EXOT27Utils as EXOT27Utils

# Create a logger for this stream
import AthenaCommon
logger = AthenaCommon.Logging.logging.getLogger("EXOT27")

from collections import defaultdict

################################################################################
# Setup the stream and our private sequence
################################################################################
streamName   = derivationFlags.WriteDAOD_EXOT27Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_EXOT27Stream )
EXOT27Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Create a private sequence to run everything in.
# The advantage of this is that we can run any time-intensive operations (e.g
# jet finding) only if the kernel's selections pass.
EXOT27Seq = CfgMgr.AthSequencer("EXOT27Sequence")
DerivationFrameworkJob += EXOT27Seq
# As we add extra variables add them to this dictionary (key is container name,
# aux data in the value)
EXOT27ExtraVariables = defaultdict(set)
# As we add extra containers add them to these lists (note that the jets have
# their own way of doing this so anything EXOT27Jets is treated differently)
EXOT27SmartContainers = [
  "Electrons", "Photons", "AntiKt4EMTopoJets", "TauJets", "Muons",
    "PrimaryVertices", "AntiKt4EMTopoJets_BTagging201810", "BTagging_AntiKt4EMTopo_201810", "MET_Reference_AntiKt4EMTopo",
    "AntiKt4EMPFlowJets", "AntiKt4EMPFlowJets_BTagging201810","AntiKt4EMPFlowJets_BTagging201903", "MET_Reference_AntiKt4EMPFlow",
  "AntiKt2LCTopoJets","InDetTrackParticles"
  ]
EXOT27AllVariables = [
  ]
# Note which small-r jets are used in this list, will be useful later (doing it
# here as it is close to the EXOT27SmartContainers declaration which is what
# ensures that they will be added to the output).
OutputSmallRJets = ["AntiKt4EMTopoJets", "AntiKt4EMPFlowJets","AntiKt4EMPFlowJets_BTagging201903","AntiKt4EMPFlowJets_BTagging201810"]


if DerivationFrameworkIsMonteCarlo:
  EXOT27AllVariables += [
    "TruthParticles",
    "TruthVertices",
    "MET_Truth",
    ]

EXOT27ExtraVariables["TauJets"].update(["truthJetLink", "truthParticleLink",
    "ptDetectorAxis", "etaDetectorAxis", "mDetectorAxis"])
EXOT27ExtraVariables["AntiKt4EMPFlowJets"].update(["NumTrkPt500"])

def outputContainer(container, warnIfNotSmart=True):
  if container in EXOT27SmartContainers + EXOT27AllVariables:
    logger.debug("Container '{0}' already requested for output!")
    return
  if container in FullListOfSmartContainers:
    EXOT27SmartContainers.append(container)
  else:
    if warnIfNotSmart:
      logger.warning(
          "Container '{0}' added but not in smart lists!".format(container) )
      logger.warning("Will be added to AllVariables - could inflate the size!")
    EXOT27AllVariables.append(container)


################################################################################
# Setup preliminary skimming (remove whole events)
################################################################################
# NOTE:
# I've split the skimming into two parts, one which can be run before the more
# CPU intensive operations (e.g. custom jet finding) and one which has to run
# after those
# This section is just for the preliminary skimming
EXOT27PreliminarySkimmingTools = []

# trigger selection
trigger_list = EXOT27Utils.met_triggers + EXOT27Utils.single_electron_triggers +\
        EXOT27Utils.single_muon_triggers + EXOT27Utils.single_photon_triggers
EXOT27TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(
    "EXOT27TriggerSkimmingTool",
    TriggerListOR = trigger_list
    )
EXOT27PreliminarySkimmingTools.append(EXOT27TriggerSkimmingTool)
# Remake the trigger list to use for the matching
triggers_for_matching = EXOT27Utils.single_electron_triggers +\
        EXOT27Utils.single_muon_triggers + EXOT27Utils.single_photon_triggers

# Add the tools to the ToolSvc
for tool in EXOT27PreliminarySkimmingTools:
  ToolSvc += tool

EXOT27Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT27PreliminaryKernel",
    SkimmingTools = EXOT27PreliminarySkimmingTools
    )

################################################################################
# Augmenting (add new objects)
################################################################################
# Create the new jet sequence in JetCommon (if it doesn't already exist)
JetCommon.OutputJets.setdefault("EXOT27Jets", [])
# Create the VR track jets
# Use buildVRJets instead of addVRJets so we can manually control the ghost
# association to the large-R jets
# do_ghost is ghost *tagging* - future improvement, not yet calibrated
vrTrackJets, vrTrackJetGhosts = HbbCommon.buildVRJets(
    sequence = EXOT27Seq, do_ghost = False, logger = logger)
JetCommon.OutputJets["EXOT27Jets"].append(vrTrackJets+"Jets")
vrGhostTagTrackJets, vrGhostTagTrackJetsGhosts = HbbCommon.buildVRJets(
    sequence = EXOT27Seq, do_ghost = True, logger = logger)
JetCommon.OutputJets["EXOT27Jets"].append(vrGhostTagTrackJets+"Jets")


# schedule pflow tagging
FlavorTagInit(JetCollections=['AntiKt4EMPFlowJets'], Sequencer=EXOT27Seq)

#=======================================
# JETS
#=======================================
# Create TCC objects
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
# Set up geometry and BField
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(EXOT27Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")


# *Something* is asking for the pseudo jet getters for the FR track jets so I'm
# still producing them, just not outputting them.
replace_jet_list = [
  "AntiKt2PV0TrackJets",
  "AntiKt4PV0TrackJets",
  "AntiKt10TrackCaloClusterJets"]
if JetCommon.jetFlags.useTruth:
  replace_jet_list += ["AntiKt4TruthJets"]
ExtendedJetCommon.replaceAODReducedJets(
    jetlist=replace_jet_list, sequence=EXOT27Seq, outputlist="EXOT27Jets")

# Includes the 5% pT trimmed R=1.0 jets
ExtendedJetCommon.addDefaultTrimmedJets(EXOT27Seq, "EXOT27Jets")
ExtendedJetCommon.addTCCTrimmedJets(EXOT27Seq,"EXOT27Jets")

# add in the retrained vr jets
HbbCommon.addVRJets(
    sequence=EXOT27Seq, do_ghost=False, logger=logger, training='201903')
JetCommon.OutputJets["EXOT27Jets"].append(
    "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903")

# add akt2
JetCommon.addStandardJets("AntiKt",0.2,"LCTopo", mods="lctopo_ungroomed", calibOpt="none", ghostArea=0.01, ptmin=2000, ptminFilter=7000, algseq=EXOT27Seq, outputGroup="EXOT27Jets")

EXOT27ExtraVariables["AntiKt2LCTopoJets"].update([
    "Width",
    "GhostBQuarksFinal",
    "GhostTrack",
    "GhostTrackCount",
    ])

OutputLargeR = [
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
  "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
  ]
# XAMPP seems to use the 'Width' variable from these?
for lrj in OutputLargeR:
  EXOT27ExtraVariables[lrj].update([
      "Width",
      "GhostBQuarksFinal",
      "Tau4_wta",
      ])
OutputLargeRParent = [
    "AntiKt10LCTopoJets",
    "AntiKt10TrackCaloClusterJets",
  ]
for lrj in OutputLargeRParent:
  EXOT27ExtraVariables[lrj].update(["GhostBQuarksFinal"])

# Ghost-associated the track jets to these large-R jets
toBeAssociatedTo = [
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
  "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
  ]
toAssociate = {
  vrTrackJetGhosts : vrTrackJetGhosts.lower(),
  vrGhostTagTrackJetsGhosts : vrGhostTagTrackJetsGhosts.lower()
}
for collection in toBeAssociatedTo:
  ungroomed, labels = EXOT27Utils.linkPseudoJetGettersToExistingJetCollection(
      EXOT27Seq, collection, toAssociate)
  EXOT27ExtraVariables[ungroomed].update(labels)

# Alias b-tagging container for VR track jets
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]
# Schedule for output
outputContainer("BTagging_AntiKtVR30Rmax4Rmin02Track_201810")
outputContainer("BTagging_AntiKtVR30Rmax4Rmin02Track_201903")
outputContainer("BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag")
outputContainer("BTagging_AntiKt4EMPFlow_201810")
outputContainer("BTagging_AntiKt4EMPFlow_201903")

# Add in Xbb tagging variables
HbbCommon.addRecommendedXbbTaggers(EXOT27Seq, ToolSvc, logger=logger)
# Add the new variables too
for extra in HbbCommon.xbbTaggerExtraVariables:
  partition = extra.partition('.')
  EXOT27ExtraVariables[partition[0]].update(partition[2].split('.') )

# Augment AntiKt4 jets with QG tagging variables
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addQGTaggerTool
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=EXOT27Seq,algname="QGTaggerToolPFAlg")

################################################################################
# Setup augmentation (add new decorations to objects)
################################################################################
EXOT27AugmentationTools = []

EXOT27TrackSelection = DerivationFramework__InDetTrackSelectionToolWrapper(name = "EXOT27TrackSelection",
                                                                           ContainerName = "InDetTrackParticles",
                                                                           DecorationName = "EXOT27DFLoose" )

EXOT27TrackSelection.TrackSelectionTool.CutLevel = "Loose"
ToolSvc += EXOT27TrackSelection

EXOT27AugmentationTools.append(EXOT27TrackSelection)

if DerivationFrameworkIsMonteCarlo:
    #add STXS inputs
    from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__TruthCategoriesDecorator
    DFHTXSdecorator = DerivationFramework__TruthCategoriesDecorator(name = "DFHTXSdecorator")

    ToolSvc += DFHTXSdecorator
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
    DerivationFrameworkJob += DerivationFramework__CommonAugmentation("TruthCategoriesCommonKernel",
                                                                             AugmentationTools = [DFHTXSdecorator]
                                                                             )
# Trigger matching augmentation
matching_helper = TriggerMatchingHelper(trigger_list=triggers_for_matching)
EXOT27Seq += matching_helper.alg

################################################################################
# Setup thinning (remove objects from collections)
################################################################################
EXOT27ThinningHelper = ThinningHelper("EXOT27ThinningHelper")
# Thin the navigation with the chains we're interested in
EXOT27ThinningHelper.TriggerChains = "|".join(trigger_list)
EXOT27ThinningHelper.AppendToStream(EXOT27Stream)
EXOT27ThinningTools = []

# Apply a pt cut on output large r jet collections
# TODO - revisit if this is harmful/necessary
for large_r in OutputLargeR:
  EXOT27ThinningTools.append(DerivationFramework__GenericObjectThinning(
        "EXOT27{0}ThinningTool".format(large_r),
        ThinningService = EXOT27ThinningHelper.ThinningSvc(),
        SelectionString = "{0}.pt > 100*GeV".format(large_r),
        ContainerName = large_r) )

EXOT27Akt2Jet = "(AntiKt2LCTopoJets.pt > 10*GeV) && (abs(AntiKt2LCTopoJets.eta) < 3.1)"
EXOT27Akt4Jet = "(AntiKt4EMTopoJets.pt > 10*GeV) && (abs(AntiKt4EMTopoJets.eta) < 3.1)"

EXOT27ThinningTools.append(DerivationFramework__GenericObjectThinning(
      "EXOT27AntiKt2LCTopoJetsThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SelectionString = EXOT27Akt2Jet,
      ContainerName = "AntiKt2LCTopoJets") )

EXOT27BaselineElectron = "Electrons.DFCommonElectronsLHLooseBL"
EXOT27BaselineMuon     = "Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection"
EXOT27BaselinePhoton   = "Photons.pt > 10.*GeV && Photons.DFCommonPhotonsIsEMTight"
EXOT27BaselineTauJet   = "TauJets.pt > 10.*GeV && TauJets.DFCommonTausLoose"
EXOT27SignalElectron   = (EXOT27BaselineElectron + " && Electrons.DFCommonElectronsLHTight "
    + "&& Electrons.pt > 20.*GeV")
EXOT27SignalMuon       = EXOT27BaselineMuon + " && Muons.pt > 20.*GeV"
EXOT27SignalPhoton     = EXOT27BaselinePhoton + " && Photons.pt > 100.*GeV"


EXOT27BaselineTrack = "(InDetTrackParticles.EXOT27DFLoose) && (InDetTrackParticles.pt > 0.5*GeV) && (abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm) && (InDetTrackParticles.d0 < 2.0*mm)"

# Set up the track thinning tools
EXOT27ThinningTools += [
  DerivationFramework__JetTrackParticleThinning(
      "EXOT27AKt2JetTPThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      JetKey = "AntiKt2LCTopoJets",
      SelectionString = EXOT27Akt2Jet,
      InDetTrackParticlesKey  = "InDetTrackParticles",
      DeltaRMatch     = 0.33,
      ApplyAnd        = False,
      ),
  DerivationFramework__JetTrackParticleThinning(
      "EXOT27AKt4JetTPThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      JetKey = "AntiKt4EMTopoJets",
      SelectionString = EXOT27Akt4Jet,
      InDetTrackParticlesKey  = "InDetTrackParticles",
      DeltaRMatch     = 0.53,
      ApplyAnd        = False,
      ),
  DerivationFramework__TrackParticleThinning(
      "EXOT27TrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SelectionString = EXOT27BaselineTrack,
      InDetTrackParticlesKey = "InDetTrackParticles",
      ApplyAnd        = True,
      ),
  DerivationFramework__EgammaTrackParticleThinning(
      "EXOT27ElectronTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey           = "Electrons",
      SelectionString = EXOT27BaselineElectron,
      ),
  DerivationFramework__EgammaTrackParticleThinning(
      "EXOT27PhotonTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey           = "Photons",
      SelectionString = EXOT27BaselinePhoton,
      ),
  DerivationFramework__MuonTrackParticleThinning(
      "EXOT27MuonTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      MuonKey         = "Muons",
      ),
  DerivationFramework__TauTrackParticleThinning(
      "EXOT27TauTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      TauKey          = "TauJets",
      SelectionString = EXOT27BaselineTauJet,
      ),
]

# Also thin the output objects by the same rules
EXOT27ThinningTools += [
  DerivationFramework__GenericObjectThinning(
      "EXOT27ElectronsThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SelectionString = EXOT27BaselineElectron,
      ContainerName   = "Electrons"),
  DerivationFramework__GenericObjectThinning(
      "EXOT27PhotonsThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SelectionString = EXOT27BaselinePhoton,
      ContainerName   = "Photons"),
  DerivationFramework__GenericObjectThinning(
      "EXOT27TauJetsThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SelectionString = EXOT27BaselineTauJet,
      ContainerName   = "TauJets"),
  ]

# Create tools that perform 'smart' thinning, thin containers that are used by
# the CP collections.
# Electron 'smart' thinning - TODO, the ElectronTrackParticle thinning can also
# be done like this
ToolSvc += EleLinkThinningTool(
    "EXOT27CaloClusterLinksThinningTool",
    LinkName = "caloClusterLinks(egammaClusters)",
    ThinningService = EXOT27ThinningHelper.ThinningSvc() )
EXOT27ThinningTools.append(
    ThinAssociatedObjectsTool(
      "EXOT27ElectronAssocThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey = "Electrons",
      ChildThinningTools = [ToolSvc.EXOT27CaloClusterLinksThinningTool] ) )
# Photon 'smart' thinning
EXOT27ThinningTools.append(
    ThinAssociatedObjectsTool(
      "EXOT27PhotonAssocThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey = "Photons",
      ChildThinningTools = [ToolSvc.EXOT27CaloClusterLinksThinningTool] ) )

# Tau 'smart' thinning
ToolSvc += EleLinkThinningTool(
    "EXOT27TauTrackLinksThinningTool",
    LinkName = "tauTrackLinks(TauTracks)",
    ThinningService = EXOT27ThinningHelper.ThinningSvc() )
EXOT27ThinningTools.append(
    ThinAssociatedObjectsTool(
      "EXOT27TauAssocThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey = "TauJets",
      ChildThinningTools = [ToolSvc.EXOT27TauTrackLinksThinningTool]) )

# subjet thinning, only applied to GhostTag selection for now
ToolSvc += EleLinkThinningTool(
    "EXOT27SubjetThinningTool",
    LinkName = "Parent.{glink}({jets}).btaggingLink({btag})".format(
        glink="GhostVR30Rmax4Rmin02TrackJetGhostTag",
        jets="AntiKtVR30Rmax4Rmin02TrackGhostTagJets",
        btag="BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag"),
    ThinningService = EXOT27ThinningHelper.ThinningSvc() )
large_r = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
# for now we only apply this above 100 GeV (same as the large-R jet
# thinning above) but we could set ths separately if needed
EXOT27ThinningTools.append(
    ThinAssociatedObjectsTool(
        "EXOT27SubjetAssocThinningTool",
        ThinningService = EXOT27ThinningHelper.ThinningSvc(),
        SGKey = large_r,
        ChildThinningTools = [ToolSvc.EXOT27SubjetThinningTool] ) )

# TODO (perhaps): truth thinning
# What I have here is extremely simplistic - designed to at least have what I
# need for my immediate studies and (by inspection) what is used by XAMPP truth
# code
if DerivationFrameworkIsMonteCarlo:
  truth_with_descendants = [6, 22, 23, 24, 25, 54]  # pdg id 54: scalar particle in mono-scalar signal model
  truth_sel_with_descendants = "||".join(map("(abs(TruthParticles.pdgId) == {0})".format, truth_with_descendants) )
  EXOT27ThinningTools += [
    DerivationFramework__GenericTruthThinning(
        "EXOT27TruthDescendantsThinningTool",
        ThinningService = EXOT27ThinningHelper.ThinningSvc(),
        ParticleSelectionString = truth_sel_with_descendants,
        PreserveDescendants     = True),
    ]
  truth_no_descendants = [5, 11, 12, 13, 14, 15, 16, 17, 18, 55]  # pdg id 55: Z' boson in mono-scalar signal model
  truth_sel_no_descendants = "||".join(map("(abs(TruthParticles.pdgId) == {0})".format, truth_no_descendants) )
  EXOT27ThinningTools += [
    DerivationFramework__GenericTruthThinning(
        "EXOT27TruthNoDescendantsThinningTool",
        ThinningService = EXOT27ThinningHelper.ThinningSvc(),
        ParticleSelectionString = truth_sel_no_descendants,
        PreserveDescendants     = False),
    ]

for tool in EXOT27ThinningTools:
  ToolSvc += tool


################################################################################
# Setup secondary skimming (remove whole events)
################################################################################
EXOT27SkimmingTools = []
# string selection
sel_list = []
# Common SR selection
# Resolved requirement - analysis level selection is 1 central jet with pT > 45
# GeV. Use 30 GeV and |eta| < 2.8 to allow for future differences in calibration
sel_list += [("count(({0}.DFCommonJets_Calib_pt > 30.*GeV) && " +
    "(abs({0}.DFCommonJets_Calib_eta) < 2.8)) >= 1").format(jets)
    for jets in ["AntiKt4EMTopoJets"] ]

# NB - this selection is only applied to the TopoJets as the PFlow jets don't
# have the DFCommonJets_Calib_* decorations. Is this going to be an issue? Do we
# need to remove this whole part of the preselection?
# NB - this selection is almost comically loose - is there really nothing
# tighter we can apply?


# Merged requirement - analysis level selection is 1 central large-R jet with pT
# > 200 GeV. Use 100 GeV and |eta| < 2.4 to allow for future differences in
# calibration. Do this for all of the large-R jet collections that are output
sel_list += ["count(({0}.pt > 100.*GeV) && (abs({0}.eta) < 2.4)) >= 1".format(
    lrj) for lrj in OutputLargeR]

# This incantation gives us an OR'd string, encasing each expression in brackets
# to ensure that everything works as expected
sel_string = " || ".join(map("({0})".format, sel_list) )
if sel_string:
  # Empty strings are falsey so this will only be executed if sel_list
  # contained something
  EXOT27StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
      "EXOT27JetSkimmingTool",
      expression = sel_string
      )
  EXOT27SkimmingTools.append(EXOT27StringSkimmingTool)


# Add additional skimming for events passing lepton and photon triggers
# Ideally we would add similar requirements for events passing the MET triggers
# but I don't know how far I trust the resolution on the reference MET
# collection. If we find that the size is still too great we can look at it.
EXOT27SkimmingORTools = []
# The lepton triggers are only used for the 2l control regions (1l is MET
# trigger)
# These regions require >=1 signal lepton && >=2 baseline leptons
# Electrons:
ToolSvc += DerivationFramework__TriggerSkimmingTool(
    "EXOT27EleTriggerSkimmingTool",
    TriggerListOR = EXOT27Utils.single_electron_triggers)
ToolSvc += DerivationFramework__xAODStringSkimmingTool(
    "EXOT27EleOfflineSkimmingTool",
    expression = " && ".join(map("({0})".format, [
      "count("+EXOT27BaselineElectron+") >= 2",
      "count("+EXOT27SignalElectron+") >= 1"]) ) )
EXOT27SkimmingORTools.append(DerivationFramework__FilterCombinationAND(
      "EXOT27EleChannelSkim",
      FilterList=[
        ToolSvc.EXOT27EleTriggerSkimmingTool,
        ToolSvc.EXOT27EleOfflineSkimmingTool]) )

# Muons:
ToolSvc += DerivationFramework__TriggerSkimmingTool(
    "EXOT27MuonTriggerSkimmingTool",
    TriggerListOR = EXOT27Utils.single_muon_triggers)
ToolSvc += DerivationFramework__xAODStringSkimmingTool(
    "EXOT27MuonOfflineSkimmingTool",
    expression = " && ".join(map("({0})".format, [
      "count("+EXOT27BaselineMuon+") >= 1",
      "count("+EXOT27SignalMuon+") >= 1"]) ) )
EXOT27SkimmingORTools.append(DerivationFramework__FilterCombinationAND(
      "EXOT27MuonChannelSkim",
      FilterList=[
        ToolSvc.EXOT27MuonTriggerSkimmingTool,
        ToolSvc.EXOT27MuonOfflineSkimmingTool]) )

# Photons:
ToolSvc += DerivationFramework__TriggerSkimmingTool(
    "EXOT27PhotonTriggerSkimmingTool",
    TriggerListOR = EXOT27Utils.single_photon_triggers)
ToolSvc += DerivationFramework__xAODStringSkimmingTool(
    "EXOT27PhotonOfflineSkimmingTool",
    expression = "count("+EXOT27SignalPhoton+") >= 1")
EXOT27SkimmingORTools.append(DerivationFramework__FilterCombinationAND(
      "EXOT27PhotonChannelSkim",
      FilterList=[
        ToolSvc.EXOT27PhotonTriggerSkimmingTool,
        ToolSvc.EXOT27PhotonOfflineSkimmingTool]) )

# Apply no extra selection to events passing the MET trigger
EXOT27SkimmingORTools.append(DerivationFramework__TriggerSkimmingTool(
    "EXOT27METTriggerSkimmingTool",
    TriggerListOR = EXOT27Utils.met_triggers) )

for tool in EXOT27SkimmingORTools:
  ToolSvc += tool

EXOT27SkimmingTools.append(DerivationFramework__FilterCombinationOR(
      "EXOTTriggerChannelORTool",
      FilterList = EXOT27SkimmingORTools) )

for tool in EXOT27SkimmingTools:
  ToolSvc += tool

EXOT27Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT27SecondaryKernel",
    SkimmingTools = EXOT27SkimmingTools,
    AugmentationTools = EXOT27AugmentationTools,
    ThinningTools = EXOT27ThinningTools
    )




################################################################################
# Setup slimming (remove variables/collections)
################################################################################
EXOT27SlimmingHelper = SlimmingHelper("EXOT27SlimmingHelper")
EXOT27SlimmingHelper.SmartCollections += EXOT27SmartContainers
EXOT27SlimmingHelper.AllVariables += EXOT27AllVariables
logger.info("EXOT27Jets: {0}".format(JetCommon.OutputJets["EXOT27Jets"]) )
for alg in EXOT27Seq:
  logger.info("Alg: {0}".format(alg) )
JetCommon.addJetOutputs(
    slimhelper = EXOT27SlimmingHelper,
    contentlist=["EXOT27Jets"],
    smartlist = [
      "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
      "AntiKt2LCTopoJets",
      "AntiKtVR30Rmax4Rmin02TrackGhostTagJets",
      "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
    ],
    vetolist = [
    "AntiKt2PV0TrackJets",
    "AntiKt4PV0TrackJets",
    "AntiKt10LCTopoJets",
    "AntiKt10TruthJets"]
    )

EXOT27SlimmingHelper.ExtraVariables += [
  "{0}.{1}".format(k, '.'.join(v) ) for k, v in EXOT27ExtraVariables.iteritems()
]

EXOT27SlimmingHelper.ExtraVariables  += ["AntiKt4EMPFlowJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_eta"]


matching_helper.add_to_slimming(EXOT27SlimmingHelper)

EXOT27SlimmingHelper.IncludeMuonTriggerContent = True
EXOT27SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT27SlimmingHelper.IncludeEtMissTriggerContent = True
EXOT27SlimmingHelper.AppendContentToStream(EXOT27Stream)

################################################################################
# Finalise
################################################################################
# Any remaining tasks, e.g. adding the kernels to the stream
EXOT27Stream.AcceptAlgs(["EXOT27SecondaryKernel"])
