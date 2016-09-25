#====================================================================
# BPHY5.py
# Bs>J/psiKK 
# It requires the reductionConf flag BPHY5 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True

print isSimulation


#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services
#include( "JpsiUpsilonTools/configureServices.py" )

include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY5_VertexTools = BPHYVertexTools("BPHY5")


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY5_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "BPHY5_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY5_AugOriginalCounts


#--------------------------------------------------------------------
## 2/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY5JpsiFinder = Analysis__JpsiFinder(name                         = "BPHY5JpsiFinder",
                                        OutputLevel                 = INFO,
                                        muAndMu                     = True,
                                        muAndTrack                  = False,
                                        TrackAndTrack               = False,
                                        assumeDiMuons               = True, 
                                        invMassUpper                = 3600.0,
                                        invMassLower                = 2600.0,
                                        Chi2Cut                     = 30.,
                                        oppChargesOnly	            = True,
                                        combOnly		            = True,
                                        atLeastOneComb              = False,
                                        useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
                                        muonCollectionKey           = "Muons",
                                        TrackParticleCollection     = "InDetTrackParticles",
                                        V0VertexFitterTool          = BPHY5_VertexTools.TrkV0Fitter,             # V0 vertex fitter
                                        useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                        TrkVertexFitterTool         = BPHY5_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                        TrackSelectorTool           = BPHY5_VertexTools.InDetTrackSelectorTool,
                                        ConversionFinderHelperTool  = BPHY5_VertexTools.InDetConversionHelper,
                                        VertexPointEstimator        = BPHY5_VertexTools.VtxPointEstimator,
                                        useMCPCuts                  = False)
ToolSvc += BPHY5JpsiFinder
print      BPHY5JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY5JpsiSelectAndWrite = DerivationFramework__Reco_mumu(name                 = "BPHY5JpsiSelectAndWrite",
                                                       JpsiFinder             = BPHY5JpsiFinder,
                                                       OutputVtxContainerName = "BPHY5JpsiCandidates",
                                                       PVContainerName        = "PrimaryVertices",
                                                       RefPVContainerName     = "SHOULDNOTBEUSED",
                                                       DoVertexType           =1)
ToolSvc += BPHY5JpsiSelectAndWrite
print BPHY5JpsiSelectAndWrite


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
BPHY5_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY5_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "BPHY5JpsiCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2000.0,
  MassMax               = 3600.0,
  Chi2Max               = 200,
  DoVertexType =1)

  
ToolSvc += BPHY5_Select_Jpsi2mumu
print      BPHY5_Select_Jpsi2mumu




## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BsKKVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BsKKVertexFit",
                                         Extrapolator        = BPHY5_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = True,
                                         MakeExtendedVertex  = True)
ToolSvc += BsKKVertexFit
print      BsKKVertexFit

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BplKplVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BplKplVertexFit",
                                         Extrapolator        = BPHY5_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = True,
                                         MakeExtendedVertex  = True)
ToolSvc += BplKplVertexFit
print      BplKplVertexFit


## 5/ setup the Jpsi+2 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY5BsJpsiKK = Analysis__JpsiPlus2Tracks(name = "BPHY5BsJpsiKK",
                                        OutputLevel = INFO,
kaonkaonHypothesis			= True,
pionpionHypothesis          = False,
kaonpionHypothesis          = False,
trkThresholdPt			    = 800.0,
trkMaxEta				    = 3.0,
BMassUpper				    = 5800.0,
BMassLower				    = 5000.0,
#DiTrackMassUpper = 1019.445 + 100.,
#DiTrackMassLower = 1019.445 - 100.,
Chi2Cut                     = 20.0,
TrkQuadrupletMassUpper      = 6000.0,
TrkQuadrupletMassLower      = 4800.0,
JpsiContainerKey		    = "BPHY5JpsiCandidates",
TrackParticleCollection     = "InDetTrackParticles",
MuonsUsedInJpsi			    = "Muons",
TrkVertexFitterTool		    = BsKKVertexFit,
TrackSelectorTool		    = BPHY5_VertexTools.InDetTrackSelectorTool,
UseMassConstraint		    = True)
        
ToolSvc += BPHY5BsJpsiKK
print      BPHY5BsJpsiKK    

## 5a/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
BPHY5BplJpsiKpl = Analysis__JpsiPlus1Track(name = "BPHY5BplJpsiKpl",
OutputLevel             = INFO,#DEBUG,
pionHypothesis			= False,
kaonHypothesis			= True,
trkThresholdPt			= 750.0,
trkMaxEta				= 2.5,
BThresholdPt			= 4000.0,
BMassUpper				= 6000.0,
BMassLower				= 4500.0,
Chi2Cut                 = 60.0,
TrkTrippletMassUpper = 6500,
TrkTrippletMassLower = 4000,
JpsiContainerKey		= "BPHY5JpsiCandidates",
TrackParticleCollection = "InDetTrackParticles",
MuonsUsedInJpsi			= "Muons",
TrkVertexFitterTool		= BplKplVertexFit,
TrackSelectorTool		= BPHY5_VertexTools.InDetTrackSelectorTool,
UseMassConstraint		= True)
        
ToolSvc += BPHY5BplJpsiKpl
print      BPHY5BplJpsiKpl    


## 6/ setup the combined augmentation/skimming tool for the Bpm
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk	
BPHY5BsKKSelectAndWrite = DerivationFramework__Reco_dimuTrkTrk(name                 = "BPHY5BsKKSelectAndWrite",
                                                           Jpsi2PlusTrackName       = BPHY5BsJpsiKK,
                                                           OutputVtxContainerName   = "BsJpsiKKCandidates",
                                                           PVContainerName          = "PrimaryVertices",
                                                           RefPVContainerName       = "BPHY5RefittedPrimaryVertices",
                                                           RefitPV                  = True,
                                                           MaxPVrefit               = 10000, DoVertexType = 7)
ToolSvc += BPHY5BsKKSelectAndWrite 
print      BPHY5BsKKSelectAndWrite

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
BPHY5BplKplSelectAndWrite = DerivationFramework__Reco_dimuTrk(name				     	= "BPHY5BplKplSelectAndWrite",
															  Jpsi1PlusTrackName	    =  BPHY5BplJpsiKpl,
															  OutputVtxContainerName 	= "BpmJpsiKpmCandidates",
                                                              PVContainerName           = "PrimaryVertices",
                                                              RefPVContainerName        = "BPHY5RefBplJpsiKplPrimaryVertices",                                                              
                                                              RefitPV                   = True,
                                                              MaxPVrefit                = 10000 )
ToolSvc += BPHY5BplKplSelectAndWrite
print      BPHY5BplKplSelectAndWrite


## b/ augment and select Bs->JpsiKK candidates
BPHY5_Select_Bs2JpsiKK = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY5_Select_Bs2JpsiKK",
  HypothesisName             = "Bs",
  InputVtxContainerName      = "BsJpsiKKCandidates",
  TrkMasses                  = [105.658, 105.658, 493.677, 493.677],
  VtxMassHypo                = 5366.3,
  MassMin                    = 5000.0,
  MassMax                    = 5800.0,
  Chi2Max                    = 200)

ToolSvc += BPHY5_Select_Bs2JpsiKK
print      BPHY5_Select_Bs2JpsiKK

BPHY5_Select_Bpl2JpsiKpl     = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY5_Select_Bpl2JpsiKpl",
  HypothesisName             = "Bplus",
  InputVtxContainerName      = "BpmJpsiKpmCandidates",
  TrkMasses                  = [105.658, 105.658, 493.677],
  VtxMassHypo                = 5279.26,
  MassMin                    = BPHY5BplJpsiKpl.BMassLower,
  MassMax                    = BPHY5BplJpsiKpl.BMassUpper,
  Chi2Max                    = BPHY5BplJpsiKpl.Chi2Cut)

ToolSvc += BPHY5_Select_Bpl2JpsiKpl
print      BPHY5_Select_Bpl2JpsiKpl


#expression = "count(BpmJpsiKpmCandidates.passed_Bplus) > 0"
#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
#BPHY5_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY5_SelectEvent",
#                                                                expression = expression)
#ToolSvc += BPHY5_SelectEvent
#print BPHY5_SelectEvent


#from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__SelectEvent

if not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
   BPHY5_SelectBsJpsiKKEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "BPHY5_SelectBsJpsiKKEvent",
     expression = "count(BsJpsiKKCandidates.passed_Bs > 0) > 0")
                                                    
   ToolSvc += BPHY5_SelectBsJpsiKKEvent
   print BPHY5_SelectBsJpsiKKEvent

   BPHY5_SelectBplJpsiKplEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY5_SelectBplJpsiKplEvent",
                                                                    expression = "count(BpmJpsiKpmCandidates.passed_Bplus>0) > 0")
   ToolSvc += BPHY5_SelectBplJpsiKplEvent
   print      BPHY5_SelectBplJpsiKplEvent

   #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   BPHY5SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR("BPHY5SkimmingOR",
                                                                     FilterList = [BPHY5_SelectBsJpsiKKEvent, BPHY5_SelectBplJpsiKplEvent],)
   ToolSvc += BPHY5SkimmingOR
   print      BPHY5SkimmingOR

if isSimulation:
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
   BPHY5TruthThinning = DerivationFramework__MenuTruthThinning(name                      = "BPHY5TruthThinning",
                                                            ThinningService            = "BPHY5ThinningSvc",
                                                            WritePartons               = False,
                                                            WriteHadrons               = True,
                                                            WriteBHadrons              = True,
                                                            WritettHFHadrons           = True,
                                                            WriteGeant                 = False,
                                                            GeantPhotonPtThresh        = -1.0,
                                                            WriteAllStable             = False,
                                                            PartonPtThresh             = -1.0,
                                                            WriteBSM                   = False,
                                                            WriteBosons                = False,
                                                            WriteBSMProducts           = False,
                                                            WriteBosonProducts         = False,
                                                            WriteTopAndDecays          = False,
                                                            WriteEverything            = False,
                                                            WriteAllLeptons            = True,
                                                            WriteStatus3               = False,
                                                            PreserveDescendants        = False,
                                                            PreserveGeneratorDescendants = False,
                                                            PreserveAncestors          = True,
                                                            WriteFirstN                = -1)
   ToolSvc += BPHY5TruthThinning


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY5_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY5_thinningTool_Tracks",
  ThinningService            = "BPHY5ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BsJpsiKKCandidates", "BpmJpsiKpmCandidates"],
  PassFlags                  = ["passed_Bs", "passed_Bplus"] )

ToolSvc += BPHY5_thinningTool_Tracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY5_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
  name                       = "BPHY5_thinningTool_PV",
  ThinningService            = "BPHY5ThinningSvc",
  CandidateCollections       = ["BsJpsiKKCandidates", "BpmJpsiKpmCandidates"],
  KeepPVTracks  =True
 )

ToolSvc += BPHY5_thinningTool_PV


## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY5MuonTPThinningTool",
                                                                         ThinningService         = "BPHY5ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY5MuonTPThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(  
    name                    = "BPHY5ElectronTPThinningTool",
    ThinningService         = "BPHY5ThinningSvc",
    SGKey                   = "Electrons",
    GSFTrackParticlesKey = "GSFTrackParticles",        
    InDetTrackParticlesKey  = "InDetTrackParticles",
    SelectionString = "",
    BestMatchOnly = True,
    ConeSize = 0.3,
    ApplyAnd = False)

ToolSvc+=BPHY5ElectronTPThinningTool
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

thiningCollection = [BPHY5_thinningTool_Tracks, BPHY5_thinningTool_PV, BPHY5MuonTPThinningTool, BPHY5ElectronTPThinningTool] 
if isSimulation :
   thiningCollection+= [ ToolSvc.BPHY5TruthThinning ]

 

print thiningCollection
# The name of the kernel (BPHY5Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("BPHY5Kernel",
                                                                       AugmentationTools = [BPHY5JpsiSelectAndWrite,  BPHY5_Select_Jpsi2mumu,
                                                                                            BPHY5BsKKSelectAndWrite,  BPHY5_Select_Bs2JpsiKK,
                                                                                            BPHY5BplKplSelectAndWrite,BPHY5_Select_Bpl2JpsiKpl,
                                                                                            BPHY5_AugOriginalCounts],
                                                                       #Only skim if not MC
                                                                       SkimmingTools     = [BPHY5SkimmingOR] if not isSimulation else [],
                                                                       ThinningTools     = thiningCollection
                                                                       
                                                                       )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_BPHY5Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_BPHY5Stream )
BPHY5Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY5Stream.AcceptAlgs(["BPHY5Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

BPHY5ThinningSvc = createThinningSvc( svcName="BPHY5ThinningSvc", outStreams=[evtStream] )
svcMgr += BPHY5ThinningSvc

#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY5SlimmingHelper = SlimmingHelper("BPHY5SlimmingHelper")
AllVariables  = []
StaticContent = []

# Needed for trigger objects
BPHY5SlimmingHelper.IncludeMuonTriggerContent  = TRUE
BPHY5SlimmingHelper.IncludeBPhysTriggerContent = TRUE

## primary vertices
AllVariables  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY5RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY5RefittedPrimaryVerticesAux."]
StaticContent += ["xAOD::VertexContainer#BPHY5RefBplJpsiKplPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY5RefBplJpsiKplPrimaryVerticesAux."]



## ID track particles
AllVariables += ["InDetTrackParticles"]

## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
AllVariables += ["CombinedMuonTrackParticles"]
AllVariables += ["ExtrapolatedMuonTrackParticles"]

## muon container
AllVariables += ["Muons"] 

## Jpsi candidates 
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY5JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY5JpsiSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY5BsKKSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY5BsKKSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY5BplKplSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY5BplKplSelectAndWrite.OutputVtxContainerName]


# Tagging information (in addition to that already requested by usual algorithms)
#AllVariables += ["Electrons"] 
AllVariables += ["GSFTrackParticles"] 
tagJetCollections = ['AntiKt4LCTopoJets']

for jet_collection in tagJetCollections:
    AllVariables += [jet_collection]
    AllVariables += ["BTagging_%s"       % (jet_collection[:-4]) ]
    AllVariables += ["BTagging_%sJFVtx"  % (jet_collection[:-4]) ]
    AllVariables += ["BTagging_%sSecVtx" % (jet_collection[:-4]) ]




# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]
    AllVariables += ["AntiKt4TruthJets","egammaTruthParticles"]

BPHY5SlimmingHelper.AllVariables = AllVariables
BPHY5SlimmingHelper.StaticContent = StaticContent
BPHY5SlimmingHelper.SmartCollections = ["Electrons"]

BPHY5SlimmingHelper.AppendContentToStream(BPHY5Stream)


