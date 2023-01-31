# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#====================================================================
# IDTR2.py
#====================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

streamName = "StreamDAOD_IDTR2"

def IDTR2Cfg(ConfigFlags):

    acc = ComponentAccumulator()

    # LRT merge
    from DerivationFrameworkInDet.InDetToolsConfig import TrackParticleMergerCfg
    IDTR2TrackParticleMergerTool = acc.getPrimaryAndMerge(TrackParticleMergerCfg(
        ConfigFlags,
        name                        = "IDTR2TrackParticleMergerTool",
        TrackParticleLocation       = ["InDetTrackParticles", "InDetLargeD0TrackParticles"],
        OutputTrackParticleLocation = "InDetWithLRTTrackParticles",
        CreateViewColllection       = True))

    LRTMergeAug = CompFactory.DerivationFramework.CommonAugmentation("InDetLRTMerge", AugmentationTools = [IDTR2TrackParticleMergerTool])
    acc.addEventAlgo(LRTMergeAug)


    # VrtSecInclusive
    from VrtSecInclusive.VrtSecInclusiveConfig import VrtSecInclusiveCfg

    acc.merge(VrtSecInclusiveCfg(ConfigFlags,
                                 name = "VrtSecInclusive",
                                 AugmentingVersionString  = "",
                                 FillIntermediateVertices = False,
                                 TrackLocation            = "InDetWithLRTTrackParticles"))

    # NewVrtSecInclusive
    from NewVrtSecInclusiveTool.NewVrtSecInclusiveConfig import MaterialSVFinderToolCfg, DVFinderToolCfg

    MaterialSVFinderTool = acc.popToolsAndMerge(MaterialSVFinderToolCfg(ConfigFlags))
    acc.addPublicTool(MaterialSVFinderTool)
    acc.addEventAlgo(CompFactory.Rec.NewVrtSecInclusiveAlg(name = "NewVrtSecInclusive_Material", 
                                                       TrackParticleContainer = "InDetWithLRTTrackParticles",
                                                       PrimaryVertexContainer = "PrimaryVertices",
                                                       BVertexContainerName = "NewVrtSecInclusive_SecondaryVertices_Material",  
                                                       BVertexTool = MaterialSVFinderTool
                                                       ))

    DVFinderToolCfg = acc.popToolsAndMerge(DVFinderToolCfg(ConfigFlags))
    acc.addPublicTool(DVFinderToolCfg)
    acc.addEventAlgo(CompFactory.Rec.NewVrtSecInclusiveAlg(name = "NewVrtSecInclusive_DV", 
                                                       TrackParticleContainer = "InDetWithLRTTrackParticles",
                                                       PrimaryVertexContainer = "PrimaryVertices",
                                                       BVertexContainerName = "NewVrtSecInclusive_SecondaryVertices_DV",  
                                                       BVertexTool = DVFinderToolCfg
                                                       ))


    # V0Finder
    from DerivationFrameworkBPhys.commonBPHYMethodsCfg import BPHY_V0ToolCfg
    V0Tools = acc.popToolsAndMerge(BPHY_V0ToolCfg(ConfigFlags, "IDTR2"))
    acc.addPublicTool(V0Tools)

    IDTR2V0ContainerName = "IDTR2RecoV0Candidates"
    IDTR2KshortContainerName = "IDTR2RecoKshortCandidates"
    IDTR2LambdaContainerName = "IDTR2RecoLambdaCandidates"
    IDTR2LambdabarContainerName = "IDTR2RecoLambdabarCandidates"

    V0Decorator = CompFactory.InDet.V0MainDecorator(name = "IDTR2V0Decorator",
                                    V0Tools = V0Tools,
                                    V0ContainerName = IDTR2V0ContainerName,
                                    KshortContainerName = IDTR2KshortContainerName,
                                    LambdaContainerName = IDTR2LambdaContainerName,
                                    LambdabarContainerName = IDTR2LambdabarContainerName)
    acc.addPublicTool(V0Decorator)

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    V0TrackSelectorLoose = CompFactory.InDet.InDetConversionTrackSelectorTool(name = "IDTR2InDetV0VxTrackSelectorLoose",
                                                                              maxSiD0             = 99999.,
                                                                              maxTrtD0            = 99999.,
                                                                              maxSiZ0             = 99999.,
                                                                              maxTrtZ0            = 99999.,
                                                                              minPt               = 500.0,
                                                                              significanceD0_Si   = 0.,
                                                                              significanceD0_Trt  = 0.,
                                                                              significanceZ0_Trt  = 0.,
                                                                              Extrapolator        = acc.popToolsAndMerge(InDetExtrapolatorCfg(ConfigFlags)),
                                                                              IsConversion        = False)

    from DerivationFrameworkBPhys.V0ToolConfig import V0VtxPointEstimatorCfg
    from InDetConfig.InDetV0FinderConfig import InDetV0FinderToolCfg
    from TrackToVertex.TrackToVertexConfig import InDetTrackToVertexCfg
    from TrkConfig.TrkV0FitterConfig import TrkV0VertexFitter_InDetExtrCfg

    args = { "VertexPointEstimator" : acc.popToolsAndMerge(V0VtxPointEstimatorCfg(ConfigFlags, "IDTR2")),
             "TrackToVertexTool"    : acc.popToolsAndMerge(InDetTrackToVertexCfg(ConfigFlags)),
             "VertexFitterTool"     : acc.popToolsAndMerge(TrkV0VertexFitter_InDetExtrCfg(ConfigFlags))
    }

    IDTR2_Reco_V0Finder   = CompFactory.DerivationFramework.Reco_V0Finder(
                                  name                   = "IDTR2_Reco_V0Finder",
                                  V0FinderTool           = acc.popToolsAndMerge(InDetV0FinderToolCfg(ConfigFlags,"IDTR2_V0FinderTool",
                                       TrackParticleCollection = "InDetWithLRTTrackParticles",
                                       TrackSelectorTool = V0TrackSelectorLoose,
                                       V0ContainerName = IDTR2V0ContainerName,
                                       KshortContainerName = IDTR2KshortContainerName,
                                       LambdaContainerName = IDTR2LambdaContainerName,
                                       LambdabarContainerName = IDTR2LambdabarContainerName,
                                       **args)),
                                  Decorator              = V0Decorator,
                                  V0ContainerName        = IDTR2V0ContainerName,
                                  KshortContainerName    = IDTR2KshortContainerName,
                                  LambdaContainerName    = IDTR2LambdaContainerName,
                                  LambdabarContainerName = IDTR2LambdabarContainerName,
                                  CheckVertexContainers  = ['PrimaryVertices'])

    IDTR2TrackFilterTool = CompFactory.InDet.InclusiveTrackFilterTool(name = "IDTR2V0Decorator")
    acc.addPublicTool(IDTR2TrackFilterTool)
    acc.addEventAlgo(CompFactory.InDet.TrackSystematicsAlg(
                        name ="InDetTrackSystematicsAlg",
                        InputTrackContainer  = "InDetWithLRTTrackParticles",
                        OutputTrackContainer = "InDetWithLRTTrackParticles_TRK_EFF_LARGED0_GLOBAL__1down",
                        TrackFilterTool      = IDTR2TrackFilterTool
                      ))

    IDTR2_Reco_V0FinderSyst   = CompFactory.DerivationFramework.Reco_V0Finder(
                                  name                   = "IDTR2_Reco_V0Finder_Syst",
                                  V0FinderTool           = acc.popToolsAndMerge(InDetV0FinderToolCfg(ConfigFlags,"IDTR2_V0FinderTool",
                                       TrackParticleCollection = "InDetWithLRTTrackParticles_TRK_EFF_LARGED0_GLOBAL__1down",
                                       TrackSelectorTool = V0TrackSelectorLoose,
                                       V0ContainerName = IDTR2V0ContainerName + "Syst",
                                       KshortContainerName = IDTR2KshortContainerName + "Syst",
                                       LambdaContainerName = IDTR2LambdaContainerName + "Syst",
                                       LambdabarContainerName = IDTR2LambdabarContainerName + "Syst",
                                       **args)),
                                  Decorator              = V0Decorator,
                                  V0ContainerName        = IDTR2V0ContainerName + "Syst",
                                  KshortContainerName    = IDTR2KshortContainerName + "Syst",
                                  LambdaContainerName    = IDTR2LambdaContainerName + "Syst",
                                  LambdabarContainerName = IDTR2LambdabarContainerName + "Syst",
                                  CheckVertexContainers  = ['PrimaryVertices'])

    skimmingTools     = []
    augmentationTools = [IDTR2_Reco_V0Finder,IDTR2_Reco_V0FinderSyst]
    for t in  augmentationTools : acc.addPublicTool(t)

    # Define the main kernel
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    acc.addEventAlgo(DerivationKernel("IDTR2Kernel",
                                      AugmentationTools = augmentationTools,
                                      SkimmingTools     = skimmingTools))
    

    # ============================
    # Define contents of the format
    # =============================
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

    IDTR2SlimmingHelper = SlimmingHelper("IDTR2SlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections, ConfigFlags = ConfigFlags)

    IDTR2SlimmingHelper.AllVariables = [
                                        "EventInfo",
                                        "PrimaryVertices",
                                        "InDetTrackParticles",
                                        "InDetLargeD0TrackParticles",
                                        "TruthParticles",
                                        "TruthVertices"
                                       ]

    StaticContent = []
    StaticContent += ["xAOD::VertexContainer#VrtSecInclusive_SecondaryVertices"]
    StaticContent += ["xAOD::VertexAuxContainer#VrtSecInclusive_SecondaryVerticesAux."]

    StaticContent += ["xAOD::VertexContainer#NewVrtSecInclusive_SecondaryVertices_DV"]
    StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'NewVrtSecInclusive_SecondaryVertices_DV']

    StaticContent += ["xAOD::VertexContainer#NewVrtSecInclusive_SecondaryVertices_Material"]
    StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'NewVrtSecInclusive_SecondaryVertices_Material']

    StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoV0Candidates']
    StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoV0Candidates']
    StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoKshortCandidates']
    StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoKshortCandidates']
    StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoLambdaCandidates']
    StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoLambdaCandidates']
    StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoLambdabarCandidates']
    StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoLambdabarCandidates']
    StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoV0CandidatesSyst']
    StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoV0CandidatesSyst']
    StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoKshortCandidatesSyst']
    StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoKshortCandidatesSyst']
    StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoLambdaCandidatesSyst']
    StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoLambdaCandidatesSyst']
    StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoLambdabarCandidatesSyst']
    StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoLambdabarCandidatesSyst']

    IDTR2SlimmingHelper.StaticContent = StaticContent

    IDTR2ItemList = IDTR2SlimmingHelper.GetItemList()
    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_IDTR2", ItemList=IDTR2ItemList, AcceptAlgs=["IDTR2Kernel"]))

    return acc

