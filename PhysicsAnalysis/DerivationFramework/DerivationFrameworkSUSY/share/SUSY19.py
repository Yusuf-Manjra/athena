#********************************************************************
# SUSY19.py
# Derivation for pixel dE/dx based search for CHAMPs
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
DerivationFrameworkIsMonteCarlo = True
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

     
### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY19Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY19Stream )
SUSY19Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY19Stream.AcceptAlgs(["SUSY19KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY19ThinningHelper = ThinningHelper( "SUSY19ThinningHelper" )
thinningTools       = []
AugmentationTools   = []
DecorationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY19 = CfgMgr.AthSequencer("SeqSUSY19")
DerivationFrameworkJob += SeqSUSY19



#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY19TriggerList import triggersNavThin
SUSY19ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY19ThinningHelper.AppendToStream( SUSY19Stream )

#====================================================================
# THINNING TOOLS 
#====================================================================

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__InDetTrackSelectionToolWrapper
SUSY19TrackSelection = DerivationFramework__InDetTrackSelectionToolWrapper(name = "SUSY19TrackSelection",
                                                                                 ContainerName = "InDetTrackParticles",
                                                                                 DecorationName = "DFLoose" )
SUSY19TrackSelection.TrackSelectionTool.CutLevel = "Loose"
ToolSvc += SUSY19TrackSelection

AugmentationTools.append(SUSY19TrackSelection)

#thinning_expression = "InDetTrackParticles.DFLoose && (InDetTrackParticles.pt > 0.5*GeV) && (abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta) ) < 3.0)"
#thinning_expression = ""
# JEFF: remove the track thinning (pt > 0.5 GeV shouldn't actually do any thinning)
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) "#&& (abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta) ) < 3.0)
thinning_expression = "(InDetTrackParticles.pt > 0.2*GeV) "#&& (abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta) ) < 3.0)
# Sicong angular thinning requirement
met_var = 'MET_Truth["NonInt"]'
met_var = 'MET_Core_AntiKt4EMPFlow["PVSoftTrkCore"]'
dPhi_expr_list = [
"(abs(%s.phi +(%.0f)*3.1415- InDetTrackParticles.phi) < 3.1415/2.0 )"%(met_var,0)
,"(abs(%s.phi +(%.0f)*3.1415- InDetTrackParticles.phi) < 3.1415/2.0 )"%(met_var,-2)
,"(abs(%s.phi +(%.0f)*3.1415- InDetTrackParticles.phi) < 3.1415/2.0 )"%(met_var,+2)
]

#thinning_expression += "&& ("+" || ".join(dPhi_expr_list)+")"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# TrackParticles directly
SUSY19TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY19TPThinningTool",
                                                                 ThinningService         = SUSY19ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = thinning_expression,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY19TPThinningTool
thinningTools.append(SUSY19TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY19MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY19MuonTPThinningTool",
                                                                         ThinningService         = SUSY19ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY19MuonTPThinningTool
thinningTools.append(SUSY19MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY19ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                     = "SUSY19ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY19ThinningHelper.ThinningSvc(),
                                                                                 SGKey              = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY19ElectronTPThinningTool
thinningTools.append(SUSY19ElectronTPThinningTool)

# Photon thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY19PhotonThinningTool = DerivationFramework__GenericObjectThinning( name             = "SUSY19PhotonThinningTool",
                                                                      ThinningService  = SUSY19ThinningHelper.ThinningSvc(),
                                                                      ContainerName    = "Photons",
                                                                      ApplyAnd         = False,
                                                                      SelectionString  = "Photons.pt > 10*GeV")
ToolSvc += SUSY19PhotonThinningTool
thinningTools.append(SUSY19PhotonThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY19PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name       = "SUSY19PhotonTPThinningTool",
                                                                              ThinningService  = SUSY19ThinningHelper.ThinningSvc(),
                                                                              SGKey      = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY19PhotonTPThinningTool
thinningTools.append(SUSY19PhotonTPThinningTool)



# JEFF: run V0 finder
# JEFF: See also:
#       https://gitlab.cern.ch/atlas/athena/-/tree/21.2/InnerDetector/InDetRecAlgs/InDetV0Finder
#       https://twiki.cern.ch/twiki/bin/view/Main/InclusiveSecondaryVertexFinder
#       https://gitlab.cern.ch/atlas/athena/-/tree/master/InnerDetector/InDetRecAlgs/InDetInclusiveSecVtx
#====================================================================
# Add K_S0->pi+pi- reconstruction (TOPQDERIV-69)
#====================================================================
doSimpleV0Finder = False
if doSimpleV0Finder:
    include("DerivationFrameworkBPhys/configureSimpleV0Finder.py")
else:
    include("DerivationFrameworkBPhys/configureV0Finder.py")

SUSY19_V0FinderTools = BPHYV0FinderTools("SUSY19")
print SUSY19_V0FinderTools

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_V0Finder
SUSY19_Reco_V0Finder   = DerivationFramework__Reco_V0Finder(
    name                   = "SUSY19_Reco_V0Finder",
    V0FinderTool           = SUSY19_V0FinderTools.V0FinderTool,
    OutputLevel            = WARNING,
    V0ContainerName        = "SUSY19RecoV0Candidates",
    KshortContainerName    = "SUSY19RecoKshortCandidates",
    LambdaContainerName    = "SUSY19RecoLambdaCandidates",
    LambdabarContainerName = "SUSY19RecoLambdabarCandidates",
    CheckVertexContainers  = ["PrimaryVertices"]
)

ToolSvc += SUSY19_Reco_V0Finder
DecorationTools.append(SUSY19_Reco_V0Finder)
print SUSY19_Reco_V0Finder

#====================================================================
# TRUTH THINNING
#====================================================================
#if DerivationFrameworkIsMonteCarlo:
if False:

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    SUSY19TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY19TruthThinningTool",
                                                                     ThinningService              = SUSY19ThinningHelper.ThinningSvc(),
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
    
    ToolSvc += SUSY19TruthThinningTool
    thinningTools.append(SUSY19TruthThinningTool)


#==========================================================================================
# ISOLATION DECORATING ( copied from DerivationFrameworkMuons/TrackIsolationDecorator.py )
#==========================================================================================
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
SUSY19TrackIsoTool = xAOD__TrackIsolationTool("SUSY19TrackIsoTool")
SUSY19TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
SUSY19TrackIsoTool.TrackSelectionTool.minPt= 1000.
SUSY19TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += SUSY19TrackIsoTool

from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool
SUSY19IsoCorrectionTool = CP__IsolationCorrectionTool (name = "SUSY19IsoCorrectionTool", IsMC = DerivationFrameworkIsMonteCarlo)
ToolSvc += SUSY19IsoCorrectionTool

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
SUSY19CaloClustersInConeTool = xAOD__CaloClustersInConeTool("SUSY19CaloClustersInConeTool",CaloClusterLocation = "CaloCalTopoClusters")
ToolSvc += SUSY19CaloClustersInConeTool

from CaloIdentifier import SUBCALO

from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
SUSY19CaloIsoTool = xAOD__CaloIsolationTool("SUSY19CaloIsoTool")
SUSY19CaloIsoTool.IsoLeakCorrectionTool = ToolSvc.SUSY19IsoCorrectionTool
SUSY19CaloIsoTool.ClustersInConeTool = ToolSvc.SUSY19CaloClustersInConeTool
SUSY19CaloIsoTool.EMCaloNums  = [SUBCALO.LAREM]
SUSY19CaloIsoTool.HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE]
SUSY19CaloIsoTool.UseEMScale  = True
SUSY19CaloIsoTool.UseCaloExtensionCaching = False
SUSY19CaloIsoTool.saveOnlyRequestedCorrections = True
SUSY19CaloIsoTool.addCaloExtensionDecoration = False
ToolSvc += SUSY19CaloIsoTool

import ROOT, PyCintex
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso

# Calculate ptcone&ptvarcone, etcone&topoetcone
deco_ptcones = [isoPar.ptcone40, isoPar.ptcone30, isoPar.ptcone20]
deco_topoetcones = [isoPar.topoetcone40, isoPar.topoetcone30, isoPar.topoetcone20]
deco_prefix = ''  #'SUSY19_'

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__TrackParametersKVU
SUSY19KVU = DerivationFramework__TrackParametersKVU(name = "SUSY19KVU",
                                                             TrackParticleContainerName = "InDetPixelPrdAssociationTrackParticles",
                                                             VertexContainerName = "PrimaryVertices" )



ToolSvc += SUSY19KVU
DecorationTools.append(SUSY19KVU)

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__CaloIsolationDecorator
SUSY19IDTrackDecorator = DerivationFramework__CaloIsolationDecorator(name = "SUSY19IDTrackDecorator",
                                                                    TrackIsolationTool = SUSY19TrackIsoTool,
                                                                    CaloIsolationTool = SUSY19CaloIsoTool,
                                                                    TargetContainer = "InDetTrackParticles",
                                                                    SelectionString = "InDetTrackParticles.pt>10*GeV",
                                                                    ptcones = deco_ptcones,
                                                                    topoetcones = deco_topoetcones,
                                                                    Prefix = deco_prefix,
                                                                    )
ToolSvc += SUSY19IDTrackDecorator
DecorationTools.append(SUSY19IDTrackDecorator)


SUSY19PixelTrackDecorator = DerivationFramework__CaloIsolationDecorator(name = "SUSY19PixelTrackDecorator",
                                                                       TrackIsolationTool = SUSY19TrackIsoTool,
                                                                       CaloIsolationTool = SUSY19CaloIsoTool,
                                                                       TargetContainer = "InDetPixelPrdAssociationTrackParticles",
                                                                       SelectionString = "InDetPixelPrdAssociationTrackParticles.pt>10*GeV",
                                                                       ptcones = deco_ptcones,
                                                                       topoetcones = deco_topoetcones,
                                                                       Prefix = deco_prefix,
                                                                       )
ToolSvc += SUSY19PixelTrackDecorator
DecorationTools.append(SUSY19PixelTrackDecorator)

        
#====================================================================
# SKIMMING TOOL 
#====================================================================


# ------------------------------------------------------------
# Lepton selection
muonsRequirements = '(Muons.pt > 2.*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 3*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHVeryLoose))'
leptonSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 0)'
stdTrackRequirements = ' ( InDetTrackParticles.pt >= 1*GeV ) && ( ( InDetTrackParticles.ptcone20 / InDetTrackParticles.pt ) < 0.2 )'
pixTrackRequirements = ' ( InDetPixelPrdAssociationTrackParticles.pt >= 1*GeV ) && ( ( InDetPixelPrdAssociationTrackParticles.ptcone20 / InDetPixelPrdAssociationTrackParticles.pt ) < 0.2 ) '
trackExpression='( count('+stdTrackRequirements+') + count('+pixTrackRequirements+')>= 2 )'

# ------------------------------------------------------------
# JEFF: Jet selection for skimming (which jet collection to use?)
applyJetCalibration_xAODColl('AntiKt4EMPFlow',SeqSUSY19) # JEFF: trying this, otherwise breaks with PFlow jets
jetRequirements = 'AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 200*GeV && abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta) < 2.8'
jetSelection = '(count('+jetRequirements+') >= 1)'
jetSelection = '(count('+jetRequirements+') >= 1) && (MET_Truth["NonInt"].met > 50*GeV)' #Sicong: Attempt to use the MET container if possible 
'''
bkg_example_evt = [1522881, 1520251,1522288,1520949,2539848,2540060,2540794,2541214,2540508,2544370,2541868,2544605,2542559,2547392,2546911,2545903]
#[3388408,3387460,3388975,3389723,3390819,3391706,3398433,3397846,3398488,3401562]

run_event_select = '||'.join([ '(EventInfo.eventNumber == %i)'%i for i in bkg_example_evt])
run_event_select = '('+run_event_select+')'
run_event_select = '(EventInfo.eventNumber>1)'
print(("INFO "*10+"\n")*10)
print(run_event_select)
#expression='('+jetSelection+"&&"+run_event_select+')' # Sicong: try specific event for testing...
'''

#expression='('+leptonSelection+' && '+trackExpression+')' # nominal version from Tomasso
#expression='('+leptonSelection+' && '+trackExpression+' && '+jetSelection+')' # JEFF: add jet selection
expression='('+jetSelection+')' # JEFF: add jet selection
#expression='1'


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool

SUSY19LeptonSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY19LeptonSkimmingTool",
                                                                        expression = expression)

#ToolSvc += SUSY19LeptonSkimmingTool
ToolSvc += SUSY19LeptonSkimmingTool # JEFF: turn this back on to add jet skimming


# ------------------------------------------------------------
# JetMET trigger name contained ' - ' cause crash when using xAODStringSkimmingTool
from DerivationFrameworkSUSY.SUSY19TriggerList import triggersMET,triggersSoftMuon,triggersJetPlusMet,triggersSoftMuonEmulation
#trigReq=triggersMET+triggersSoftMuon+triggersJetPlusMet
trigReq=triggersMET+triggersSoftMuon
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool,DerivationFramework__FilterCombinationOR,DerivationFramework__FilterCombinationAND
SUSY19InclusiveTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY19InclusiveTriggerSkimmingTool",
                                                                               TriggerListOR = trigReq)
ToolSvc += SUSY19InclusiveTriggerSkimmingTool

SUSY19TriggerSkimmingTool=None
if DerivationFrameworkIsMonteCarlo:
    # one muon + jet + met trigger
    #SUSY19SoftOneMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY19OneMuonTriggerSkimmingTool",
    #                                                                                TriggerListAND = ['HLT_mu4','HLT_xe50_mht','HLT_j110'])
    #ToolSvc += SUSY19SoftOneMuonTriggerSkimmingTool
    
    # dimuon + jet + met trigger
    #SUSY19SoftTwoMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY19TwoMuonTriggerSkimmingTool",
    #                                                                                TriggerListAND = ['HLT_2mu4','HLT_j85'])
    #ToolSvc += SUSY19SoftTwoMuonTriggerSkimmingTool

    # OR of soft muon stuff or inclusive MET triggers
    SUSY19TriggerSkimmingTool = DerivationFramework__FilterCombinationOR(name = "SUSY19TriggerSkimmingTool", 
                                                                         FilterList = [SUSY19InclusiveTriggerSkimmingTool])
    ToolSvc += SUSY19TriggerSkimmingTool
else:
    # OR of soft muon stuff or inclusive MET triggers
    SUSY19TriggerSkimmingTool = DerivationFramework__FilterCombinationOR(name = "SUSY19TriggerSkimmingTool", 
                                                                         FilterList = [SUSY19InclusiveTriggerSkimmingTool])
    ToolSvc += SUSY19TriggerSkimmingTool
    
# ------------------------------------------------------------

# ------------------------------------------------------------
# Final MET-based skim selection, with trigger selection and lepton selection
#SUSY19SkimmingTool = DerivationFramework__FilterCombinationAND(name = "SUSY19SkimmingTool", 
#                                                               FilterList = [SUSY19TriggerSkimmingTool])

# JEFF: add the jet cut
SUSY19SkimmingTool = DerivationFramework__FilterCombinationAND(name = "SUSY19SkimmingTool",
                                                               FilterList = [SUSY19LeptonSkimmingTool, SUSY19TriggerSkimmingTool])

ToolSvc += SUSY19SkimmingTool

#====================================================================
# Max Cell sum decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
SUSY19_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "SUSY19_MaxCellDecoratorTool",
                                                                     SGKey_electrons         = "Electrons",
                                                                     SGKey_photons           = "Photons",
                                                                     )
ToolSvc += SUSY19_MaxCellDecoratorTool

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
   SeqSUSY19 += CfgMgr.DerivationFramework__DerivationKernel("SUSY19KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY19")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY19 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY19KernelSkim",
  AugmentationTools = DecorationTools,
  SkimmingTools = [SUSY19SkimmingTool]
)



#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY19)


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY19 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY19KernelAug",
  AugmentationTools = AugmentationTools,
  ThinningTools = thinningTools,
)

# Sicong Try VrtSecInclusive
#
#
#
#

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
InDetSCT_ConditionsSummarySvc = SCT_ConditionsSummarySvc(name = "InDetSCT_ConditionsSummarySvc")
ServiceMgr += InDetSCT_ConditionsSummarySvc

from RecExConfig.RecFlags import rec

# get inputFileSummary - will use it to extract info for MC/DATA
from RecExConfig.InputFilePeeker import inputFileSummary

# import the data types 
import EventKernel.ParticleDataType

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

include ("RecExCond/RecExCommon_flags.py")

include( "RecExCond/AllDet_detDescr.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#----------------------------------------------------------
#  VrtSecInclusive creates also TrackSummary tool.
#   TrackSummary tool creates InDetExtrapolator and AtlasMagneticFieldTool
#
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
SUSY19_VSI = VrtSecInclusive()


##SeqSUSY19.VrtSecInclusive.OutputLevel = DEBUG
#SUSY19_VSI.OutputLevel = DEBUG
SUSY19_VSI.OutputLevel = INFO
#SUSY19_VSI.CutBLayHits = 1
#SUSY19_VSI.CutPixelHits = 1
SUSY19_VSI.CutSctHits = 1
SUSY19_VSI.TrkA0ErrCut = 200000
SUSY19_VSI.TrkZErrCut = 200000
SUSY19_VSI.a0TrkPVDstMinCut = 0.0
SUSY19_VSI.TrkPtCut = 300
SUSY19_VSI.SelVrtChi2Cut=4.5
SUSY19_VSI.CutSharedHits=5
SUSY19_VSI.TrkChi2Cut=5.0
SUSY19_VSI.TruthTrkLen=1
SUSY19_VSI.DoSAloneTRT=False
SUSY19_VSI.DoTruth = False
# following is when there is no GEN_AOD in input file,
# e.g., when I run on output of InDetRecExample or on a ESD file
# when running on AOD output of InDetRecEx, explicitly uncomment the next line and comment out rec.readESD
#     SUSY19_VSI.MCEventContainer = "TruthEvent"

if rec.readESD():
    SUSY19_VSI.MCEventContainer = "TruthEvent"

if 'IS_SIMULATION' in inputFileSummary['evt_type']:
    SUSY19_VSI.DoTruth=True


from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
	                                      Extrapolator        = ToolSvc.AtlasExtrapolator,
	                                      IterationNumber     = 30
					     )
ToolSvc +=  InclusiveVxFitterTool;
InclusiveVxFitterTool.OutputLevel = INFO
SUSY19_VSI.VertexFitterTool=InclusiveVxFitterTool
SUSY19_VSI.Extrapolator = ToolSvc.AtlasExtrapolator

print VrtSecInclusive
SeqSUSY19 += SUSY19_VSI
# The input file
#ServiceMgr.EventSelector.InputCollections = jp.AthenaCommonFlags.FilesInput()

#==============================================================================
# VrtSecInclusive IP Augmentor
#==============================================================================

from VrtSecInclusive.IPAugmentor import IPAugmentor

IPAugmentor = IPAugmentor("VsiIPAugmentor")
IPAugmentor.doAugmentDVimpactParametersToMuons     = True
IPAugmentor.doAugmentDVimpactParametersToElectrons = True
IPAugmentor.VertexFitterTool=InclusiveVxFitterTool

SeqSUSY19 += IPAugmentor

MSMgr.GetStream("StreamDAOD_SUSY19").AddItem( [ 'xAOD::TrackParticleContainer#InDetTrackParticles*',
                                                  'xAOD::TrackParticleAuxContainer#InDetTrackParticles*',
                                                  'xAOD::VertexContainer#VrtSecInclusive*',
                                                  'xAOD::VertexAuxContainer#VrtSecInclusive*'] )


# Sicong End Try VrtSecInclusive

#====================================================================
# Prompt Lepton Tagger
#====================================================================


import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# simple call to replaceAODReducedJets(["AntiKt4PV0TrackJets"], SeqSUSY19, "SUSY19")
JetTagConfig.ConfigureAntiKt4PV0TrackJets(SeqSUSY19, "SUSY19")

# Electron and Muon algorithms: PromptLeptonIso and PromptLeptonVeto
SeqSUSY19 += JetTagConfig.GetDecoratePromptLeptonAlgs()

# Tau algorithm: PromptTauVeto
#SeqSUSY19 += JetTagConfig.GetDecoratePromptTauAlgs()
#Sicong: Temporarily remove for Pile-Up



#====================================================================
# CONTENT LIST  
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSUSY.SUSY19ContentList import *
SUSY19SlimmingHelper = SlimmingHelper("SUSY19SlimmingHelper")
SUSY19SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons",
                                         #"AntiKt4EMTopoJets", # JEFF
                                         "AntiKt4EMPFlowJets",
                                         #"MET_Reference_AntiKt4EMTopo", # JEFF
                                         "MET_Reference_AntiKt4EMPFlow",
                                         "PrimaryVertices",
                                         #"BTagging_AntiKt4EMTopo",
                                         #"BTagging_AntiKt4EMPFlow",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                         "AntiKt4EMPFlowJets_BTagging201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        #"AntiKt4EMTopoJets_BTagging201810", # JEFF
                                        #"BTagging_AntiKt4EMTopo_201810", # JEFF
                                         #"MET_Reference_AntiKt4EMTopo", # JEFF
                                         "MET_Reference_AntiKt4EMPFlow",
                                         "InDetTrackParticles"
                                         ]

SUSY19SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth",
                                     #"MET_Core_AntiKt4EMTopo", # JEFF: why no equivalent for PFlow?
                                     #"METAssoc_AntiKt4EMTopo", # JEFF: why no equivalent for PFlow?
                                     "InDetPixelPrdAssociationTrackParticles"]
SUSY19SlimmingHelper.ExtraVariables = SUSY19ExtraVariables
SUSY19SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
#SUSY19SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptTauVariablesForDxAOD()
#Sicong: Temporarily remove for Pile-Up
SUSY19SlimmingHelper.IncludeMuonTriggerContent = True
SUSY19SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY19SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
SUSY19SlimmingHelper.IncludeJetTriggerContent = True
#SUSY19SlimmingHelper.IncludeTauTriggerContent = False
#Sicong: Temporarily remove for Pile-Up
SUSY19SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY19SlimmingHelper.IncludeBJetTriggerContent = False




# JEFF
StaticContent = []
'''
StaticContent += [
            'xAOD::VertexContainer#SUSY19RecoV0Candidates',
            'xAOD::VertexAuxContainer#SUSY19RecoV0CandidatesAux'
            + '.-vxTrackAtVertex'
            + '.-vertexType'
            + '.-neutralParticleLinks'
            + '.-neutralWeights'
            + '.-KshortLink'
            + '.-LambdaLink'
            + '.-LambdabarLink'
            + '.-gamma_fit'
            + '.-gamma_mass'
            + '.-gamma_massError'
            + '.-gamma_probability',
 ]
'''
for containerName in ["SUSY19RecoV0Candidates","SUSY19RecoKshortCandidates","SUSY19RecoLambdaCandidates","SUSY19RecoLambdabarCandidates"]:
  StaticContent += [
            'xAOD::VertexContainer#%s'%containerName,
            'xAOD::VertexAuxContainer#%sAux'%containerName
            + '.-vxTrackAtVertex'  
            + '.-vertexType'
            + '.-neutralParticleLinks'
            + '.-neutralWeights'
            + '.-KshortLink'
            + '.-LambdaLink'
            + '.-LambdabarLink'
            + '.-gamma_fit'
            + '.-gamma_mass'
            + '.-gamma_massError'
            + '.-gamma_probability',
  ]
SUSY19SlimmingHelper.StaticContent = StaticContent

# Sicong: Try VSI
SUSY19SlimmingHelper.AllVariables += [
                                        "VrtSecInclusive_SecondaryVertices"
                                       ]

# Include dvtrack variables from re-running of VSI 
original_dvtrack_vars = "is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV".split(".")
SUSY19SlimmingHelper.ExtraVariables += [ "InDetTrackParticles." + ".".join(original_dvtrack_vars) ]

vtx_vars = ["trackParticleLinks","neutralParticleLinks"]
SUSY19SlimmingHelper.ExtraVariables += [ "VrtSecInclusive_SecondaryVertices." + ".".join(vtx_vars) ]
StaticContent = []
StaticContent += [
            'xAOD::VertexContainer#VrtSecInclusive_SecondaryVertices',
            'xAOD::VertexAuxContainer#VrtSecInclusive_SecondaryVerticesAux'
            + '.-vxTrackAtVertex'
            + '.-vertexType'
            + '.-neutralParticleLinks'
            + '.-trackParticleLinks'
            + '.-neutralWeights'
 ]

# Sicong: End VSI



# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY19SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY19SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   

SUSY19SlimmingHelper.AppendContentToStream(SUSY19Stream)
