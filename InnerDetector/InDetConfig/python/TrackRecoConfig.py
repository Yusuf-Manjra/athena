# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

##------------------------------------------------------------------------------
def BCM_ZeroSuppressionCfg(flags, name="InDetBCM_ZeroSuppression", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BcmContainerName", "BCM_RDOs")
    algo = CompFactory.BCM_ZeroSuppression(name=name, **kwargs)
    acc.addEventAlgo(algo, primary = True)
    return acc

##------------------------------------------------------------------------------
def PixelClusterizationCfg(flags, name = "InDetPixelClusterization", **kwargs) :
    acc = ComponentAccumulator()
    merged_pixels_tool = acc.popToolsAndMerge(MergedPixelsToolCfg(flags, **kwargs))
    ambi_finder = acc.popToolsAndMerge(PixelGangedAmbiguitiesFinderCfg(flags))

    # Region selector tools for Pixel
    from RegionSelector.RegSelToolConfig import regSelTool_Pixel_Cfg
    RegSelTool_Pixel = acc.popToolsAndMerge(regSelTool_Pixel_Cfg(flags))

    kwargs.setdefault("clusteringTool", merged_pixels_tool)
    kwargs.setdefault("gangedAmbiguitiesFinder", ambi_finder)
    kwargs.setdefault("DataObjectName", "PixelRDOs")
    kwargs.setdefault("ClustersName", "PixelClusters")
    kwargs.setdefault("RegSelTool", RegSelTool_Pixel)

    acc.addEventAlgo(CompFactory.InDet.PixelClusterization(name=name, **kwargs))
    return acc
##------------------------------------------------------------------------------
def PixelClusterizationPUCfg(flags, name="InDetPixelClusterizationPU", **kwargs) :
    kwargs.setdefault("DataObjectName", "Pixel_PU_RDOs")
    kwargs.setdefault("ClustersName", "PixelPUClusters")
    kwargs.setdefault("AmbiguitiesMap", "PixelClusterAmbiguitiesMapPU")
    return PixelClusterizationCfg(flags, name=name, **kwargs)

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def SCTClusterizationCfg(flags, name="InDetSCT_Clusterization", **kwargs) :
    acc = ComponentAccumulator()

    # Need to get SCT_ConditionsSummaryTool for e.g. SCT_ClusteringTool
    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(SCT_ConditionsSummaryToolCfg(flags, withFlaggedCondTool=False))

    #### Clustering tool ######
    InDetClusterMakerTool = acc.popToolsAndMerge(ClusterMakerToolCfg(flags))
    InDetSCT_ClusteringTool = CompFactory.InDet.SCT_ClusteringTool( name           = "InDetSCT_ClusteringTool",
                                                                    globalPosAlg   = InDetClusterMakerTool,
                                                                    conditionsTool = InDetSCT_ConditionsSummaryToolWithoutFlagged)
    if flags.InDet.selectSCTIntimeHits :
       if flags.InDet.InDet25nsec :
          InDetSCT_ClusteringTool.timeBins = "01X"
       else:
          InDetSCT_ClusteringTool.timeBins = "X1X"

    kwargs.setdefault("clusteringTool", InDetSCT_ClusteringTool)
    kwargs.setdefault("DataObjectName", 'SCT_RDOs') ##InDetKeys.SCT_RDOs()
    kwargs.setdefault("ClustersName", 'SCT_Clusters') ##InDetKeys.SCT_Clusters()
    kwargs.setdefault("conditionsTool", InDetSCT_ConditionsSummaryToolWithoutFlagged)

    acc.addEventAlgo( CompFactory.InDet.SCT_Clusterization(name=name, **kwargs))

    return acc

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def SCTClusterizationPUCfg(flags, name="InDetSCT_ClusterizationPU", **kwargs) :
    kwargs.setdefault("DataObjectName", "SCT_PU_RDOs" )   #flags.InDetKeys.SCT_PU_RDOs
    kwargs.setdefault("ClustersName", "SCT_PU_Clusters")  #flags.InDetKeys.SCT_PU_Clusters
    return SCTClusterizationCfg(flags, name=name, **kwargs)

##------------------------------------------------------------------------------
def PixelGangedAmbiguitiesFinderCfg(flags) :
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    acc = PixelReadoutGeometryCfg(flags)

    InDetPixelGangedAmbiguitiesFinder = CompFactory.InDet.PixelGangedAmbiguitiesFinder( name = "InDetPixelGangedAmbiguitiesFinder")
    acc.setPrivateTools( InDetPixelGangedAmbiguitiesFinder )
    return acc

##------------------------------------------------------------------------------
def MergedPixelsToolCfg(flags, **kwargs) :
      acc = ComponentAccumulator()
      # --- now load the framework for the clustering
      kwargs.setdefault("globalPosAlg", acc.popToolsAndMerge(ClusterMakerToolCfg(flags)) )

      # PixelClusteringToolBase uses PixelConditionsSummaryTool
      from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
      kwargs.setdefault("PixelConditionsSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)) )

      # Enable duplcated RDO check for data15 because duplication mechanism was used.
      if len(flags.Input.ProjectName)>=6 and flags.Input.ProjectName[:6]=="data15":
          kwargs.setdefault("CheckDuplicatedRDO", True )

      InDetMergedPixelsTool = CompFactory.InDet.MergedPixelsTool(  name = "InDetMergedPixelsTool", **kwargs)

      acc.setPrivateTools(InDetMergedPixelsTool)
      return acc

##------------------------------------------------------------------------------
def ClusterMakerToolCfg(flags, name="InDetClusterMakerTool", **kwargs) :
    acc = ComponentAccumulator()

    # This directly needs the following Conditions data:
    # PixelChargeCalibCondData & PixelOfflineCalibData
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelChargeCalibCondAlgCfg, PixelOfflineCalibCondAlgCfg
    acc.merge(PixelChargeCalibCondAlgCfg(flags))
    acc.merge(PixelOfflineCalibCondAlgCfg(flags))

    from PixelReadoutGeometry.PixelReadoutGeometryConfig import PixelReadoutManagerCfg
    acc.merge(PixelReadoutManagerCfg(flags))

    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg
    PixelLorentzAngleTool = acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    SCTLorentzAngleTool = acc.popToolsAndMerge( SCT_LorentzAngleCfg(flags) )

    kwargs.setdefault("PixelLorentzAngleTool", PixelLorentzAngleTool)
    kwargs.setdefault("SCTLorentzAngleTool", SCTLorentzAngleTool)

    InDetClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name = name, **kwargs)
    acc.setPrivateTools(InDetClusterMakerTool)
    return acc


def TrackToVertexCfg(flags, name="AtlasTrackToVertexTool", **kwargs):
    result = ComponentAccumulator()
    if "Extrapolator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs["Extrapolator"] = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    result.merge(BeamSpotCondAlgCfg(flags))
    result.setPrivateTools(CompFactory.Reco.TrackToVertex(name, **kwargs))
    return result

def TrackParticleCreatorToolCfg(flags, name="TrackParticleCreatorTool", **kwargs):
    result = ComponentAccumulator()
    if "TrackToVertex" not in kwargs:
        kwargs["TrackToVertex"] = result.popToolsAndMerge(TrackToVertexCfg(flags))
    if "TrackSummaryTool" not in kwargs:
        from InDetConfig.TrackingCommonConfig import InDetTrackSummaryToolSharedHitsCfg
        TrackSummaryTool = result.getPrimaryAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags))
        kwargs["TrackSummaryTool"] = TrackSummaryTool
    p_expr = flags.InDet.perigeeExpression
    kwargs.setdefault("BadClusterID", flags.InDet.pixelClusterBadClusterID)
    kwargs.setdefault("KeepParameters", True)
    kwargs.setdefault("KeepFirstParameters", flags.InDet.KeepFirstParameters)
    kwargs.setdefault(
        "PerigeeExpression",
        p_expr if p_expr != "Vertex" else "BeamLine")
    result.addPublicTool(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs), primary = True)
    return result

def TrackCollectionCnvToolCfg(flags, name="TrackCollectionCnvTool", TrackParticleCreator = None):
    result = ComponentAccumulator()
    if TrackParticleCreator is None:
        TrackParticleCreator = result.getPrimaryAndMerge(TrackParticleCreatorToolCfg(flags))
    result.setPrivateTools(CompFactory.xAODMaker.TrackCollectionCnvTool(
        name,
        TrackParticleCreator=TrackParticleCreator,
    ))
    return result

def TrackCollectionMergerAlgCfg(flags, name="InDetTrackCollectionMerger", InputCombinedTracks=None, **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("TracksLocation", InputCombinedTracks)
    kwargs.setdefault("OutputTracksLocation", "CombinedInDetTracks")
    from InDetConfig.TrackingCommonConfig import InDetPRDtoTrackMapToolGangedPixelsCfg
    InDetPRDtoTrackMapToolGangedPixels = result.popToolsAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags))
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", "PRDtoTrackMapCombinedInDetTracks")
    kwargs.setdefault("UpdateSharedHits", True)
    kwargs.setdefault("UpdateAdditionalInfo", True)
    from InDetConfig.TrackingCommonConfig import InDetTrackSummaryToolSharedHitsCfg
    TrackSummaryTool = result.getPrimaryAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags))
    kwargs.setdefault("SummaryTool", TrackSummaryTool)

    result.addEventAlgo(CompFactory.Trk.TrackCollectionMerger(name, **kwargs))
    return result

def TrackParticleCnvAlgCfg(flags, name="TrackParticleCnvAlg", TrackContainerName="CombinedInDetTracks", OutputTrackParticleContainer="InDetTrackParticles", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ConvertTracks", True)
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("TrackContainerName", TrackContainerName)
    kwargs.setdefault("xAODContainerName", OutputTrackParticleContainer)
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", OutputTrackParticleContainer)
    if "TrackParticleCreator" not in kwargs:
        kwargs["TrackParticleCreator"] = result.getPrimaryAndMerge(TrackParticleCreatorToolCfg(flags))
    if "TrackCollectionCnvTool" not in kwargs:
        kwargs["TrackCollectionCnvTool"] = result.popToolsAndMerge(TrackCollectionCnvToolCfg(
            flags,
            TrackParticleCreator=kwargs["TrackParticleCreator"],
        ))

    if flags.InDet.doTruth:
        kwargs.setdefault("TrackTruthContainerName", f"{TrackContainerName}TruthCollection")
        kwargs.setdefault("AddTruthLink", True)

        if "MCTruthClassifier" not in kwargs:
            from MCTruthClassifier.MCTruthClassifierConfig import MCTruthClassifierCfg
            kwargs["MCTruthClassifier"] = result.popToolsAndMerge(
                MCTruthClassifierCfg(flags))

    else:
        kwargs.setdefault("AddTruthLink", False)
    result.addEventAlgo(CompFactory.xAODMaker.TrackParticleCnvAlg(name, **kwargs))
    return result

def InDetTrackRecoCfg(flags):
    """Configures complete ID tracking """
    result = ComponentAccumulator()

    if flags.Input.Format == "BS":
        from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConfig import PixelRawDataProviderAlgCfg
        result.merge(PixelRawDataProviderAlgCfg(flags))
        from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConfig import SCTRawDataProviderCfg, SCTEventFlagWriterCfg
        result.merge(SCTRawDataProviderCfg(flags))
        result.merge(SCTEventFlagWriterCfg(flags))
        from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConfig import TRTRawDataProviderCfg
        result.merge(TRTRawDataProviderCfg(flags))

    from InDetConfig.SiliconPreProcessing import InDetRecPreProcessingSiliconCfg
    result.merge(InDetRecPreProcessingSiliconCfg(flags))

    from InDetConfig.TrackingSiPatternConfig import TrackingSiPatternCfg
    result.merge(TrackingSiPatternCfg(flags, [], "ResolvedTracks", "SiSPSeededTracks"))

    # TRT extension
    if flags.InDet.doTRTExtension:
        from InDetConfig.TRTExtensionConfig import NewTrackingTRTExtensionCfg
        result.merge(NewTrackingTRTExtensionCfg(flags,
                                                SiTrackCollection="ResolvedTracks",
                                                ExtendedTrackCollection="ExtendedTracks",
                                                ExtendedTracksMap="ExtendedTracksMap",
                                                doPhase=False))
        InputCombinedInDetTracks = ["ExtendedTracks"]

        from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
        result.merge(TRTSegmentFindingCfg(flags,
                                          "",
                                          InputCombinedInDetTracks,
                                          "TRTSegments"))
    else:
        InputCombinedInDetTracks = ["ResolvedTracks"]

    # BackTracking
    if flags.InDet.doBackTracking:
        from InDetConfig.BackTrackingConfig import BackTrackingCfg
        result.merge(BackTrackingCfg(flags,
                                     InputCombinedInDetTracks,
                                     ClusterSplitProbContainer="InDetAmbiguityProcessorSplitProb"))
        InputCombinedInDetTracks += ["ResolvedTRTSeededTracks"]

    # LRT
    if flags.InDet.doLargeD0 or flags.InDet.doR3LargeD0:
        if flags.InDet.doR3LargeD0:
            flagsLRT = flags.cloneAndReplace("InDet.Tracking", "InDet.R3LargeD0Tracking")
        else:
            flagsLRT = flags.cloneAndReplace("InDet.Tracking", "InDet.LargeD0Tracking")
        result.merge(TrackingSiPatternCfg(flagsLRT, InputCombinedInDetTracks, "ResolvedLargeD0Tracks", "SiSpSeededLargeD0Tracks"))
        LRTTrackContainer = "ResolvedLargeD0Tracks"
        if flags.InDet.doTRTExtension:
            result.merge(NewTrackingTRTExtensionCfg(flagsLRT,
                                                    SiTrackCollection="ResolvedLargeD0Tracks",
                                                    ExtendedTrackCollection="ExtendedLargeD0Tracks",
                                                    ExtendedTracksMap="ExtendedLargeD0TracksMap",
                                                    doPhase=False))
            LRTTrackContainer = "ExtendedLargeD0Tracks"
        if flags.InDet.storeSeparateLargeD0Container:
            if flags.InDet.doTruth:
                from InDetConfig.TrackTruthConfig import InDetTrackTruthCfg
                result.merge(InDetTrackTruthCfg(flagsLRT,
                                                Tracks="ExtendedLargeD0Tracks",
                                                DetailedTruth="ExtendedLargeD0TracksDetailedTruth",
                                                TracksTruth="ExtendedLargeD0TracksTruthCollection"))
            result.merge(TrackParticleCnvAlgCfg(flagsLRT,
                                                name="LargeD0TrackParticleCnvAlg",
                                                TrackContainerName=LRTTrackContainer,
                                                OutputTrackParticleContainer="InDetLargeD0TrackParticles"))
        else:
            InputCombinedInDetTracks += [LRTTrackContainer]

    if flags.InDet.doTRTStandalone:
        flagsTRTStandalone = flags.cloneAndReplace("InDet.Tracking", "InDet.TRTStandaloneTracking")
        from InDetConfig.TRTStandaloneConfig import TRTStandaloneCfg
        result.merge(TRTStandaloneCfg(flagsTRTStandalone, "", InputCombinedInDetTracks, "TRTSegments"))
        InputCombinedInDetTracks += ["TRTStandaloneTracks"]

    result.merge(TrackCollectionMergerAlgCfg(flags, InputCombinedTracks=InputCombinedInDetTracks))

    if flags.InDet.doTruth:
        from InDetConfig.TrackTruthConfig import InDetTrackTruthCfg
        result.merge(InDetTrackTruthCfg(flags))

    # TODO add followup algs
    result.merge(TrackParticleCnvAlgCfg(flags))

    if flags.InDet.doVertexFinding:
        from InDetConfig.VertexFindingConfig import primaryVertexFindingCfg
        result.merge(primaryVertexFindingCfg(flags))

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    toAOD = ["xAOD::TrackParticleContainer#InDetTrackParticles", "xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."]
    toESD = []
    result.merge(addToESD(flags, toAOD+toESD))
    result.merge(addToAOD(flags, toAOD))
    return result


### Common InDet/ITk config interfaces
def TrackRecoCfg(flags):
    if flags.Detector.GeometryID:
        return InDetTrackRecoCfg(flags)
    elif flags.Detector.GeometryITk:
        from InDetConfig.ITkTrackRecoConfig import ITkTrackRecoCfg
        return ITkTrackRecoCfg(flags)


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.InDet.doPixelClusterSplitting = True
    ConfigFlags.InDet.doSiSPSeededTrackFinder = True

    # Disable calo for this test
    ConfigFlags.Detector.EnableCalo = False

    # TODO add these flags in future
    # ConfigFlags.addFlag('InDet.doAmbiSolving', True)
    # ConfigFlags.addFlag('InDet.useHolesFromPattern', False)
    # ConfigFlags.addFlag('InDet.holeSearchInGX2Fit', True)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    #######################################################################
    #################### Additional Configuration  ########################
    if "EventInfo" not in ConfigFlags.Input.Collections:
        from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
        top_acc.merge(EventInfoCnvAlgCfg(ConfigFlags))

    if ConfigFlags.Input.isMC:
        from xAODTruthCnv.xAODTruthCnvConfigNew import GEN_AOD2xAODCfg
        top_acc.merge(GEN_AOD2xAODCfg(ConfigFlags))

    top_acc.merge(TrackRecoCfg(ConfigFlags))
    from AthenaCommon.Constants import DEBUG
    top_acc.foreach_component("AthEventSeq/*").OutputLevel=DEBUG
    top_acc.printConfig(withDetails=True, summariseProps=True)
    top_acc.store(open("TrackRecoConfig.pkl", "wb"))

    import sys
    if "--norun" not in sys.argv:
        sc = top_acc.run(1)
        if sc.isFailure():
            sys.exit(-1)
