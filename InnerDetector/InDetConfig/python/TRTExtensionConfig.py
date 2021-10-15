# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
import InDetConfig.TrackingCommonConfig         as   TC

#///////////////////////////////////////////////////////////////////////////////////////////////
def TRT_TrackExtensionAlgCfg(flags, name = 'InDetTRT_ExtensionPhase', SiTrackCollection=None, ExtendedTracksMap="ExtendedTracksMap", TrackExtensionTool=None, **kwargs):
    acc = ComponentAccumulator()
    # set output extension map name
    OutputExtendedTracks = ExtendedTracksMap
    if TrackExtensionTool is None:
        TrackExtensionTool = acc.popToolsAndMerge(TC.InDetTRT_ExtensionToolPhaseCfg(flags))
        acc.addPublicTool(TrackExtensionTool)

    kwargs.setdefault("InputTracksLocation", SiTrackCollection)
    kwargs.setdefault("ExtendedTracksLocation", OutputExtendedTracks )
    kwargs.setdefault("TrackExtensionTool", TrackExtensionTool)

    acc.addEventAlgo(CompFactory.InDet.TRT_TrackExtensionAlg(name = name, **kwargs))
    return acc

def CompetingRIOsOnTrackToolCfg(flags, name = 'InDetCompetingRotCreator', **kwargs):
    acc = ComponentAccumulator()
    InDetCompetingTRT_DC_Tool = acc.popToolsAndMerge(TC.InDetCompetingTRT_DC_ToolCfg(flags))
    acc.addPublicTool(InDetCompetingTRT_DC_Tool)

    kwargs.setdefault("ToolForCompPixelClusters", None)
    kwargs.setdefault("ToolForCompSCT_Clusters", None)
    kwargs.setdefault("ToolForCompTRT_DriftCircles", InDetCompetingTRT_DC_Tool)
    acc.setPrivateTools(CompFactory.Trk.CompetingRIOsOnTrackTool(name = name, **kwargs))
    return acc

def DeterministicAnnealingFilterCfg(flags, name = 'InDetDAF', **kwargs):
    acc = ComponentAccumulator()
    InDetCompetingRotCreator = acc.popToolsAndMerge(CompetingRIOsOnTrackToolCfg(flags))
    acc.addPublicTool(InDetCompetingRotCreator)

    from InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))

    from InDetConfig.TrackingCommonConfig import InDetUpdatorCfg
    InDetUpdator = acc.getPrimaryAndMerge(InDetUpdatorCfg(flags))

    kwargs.setdefault("ToolForExtrapolation", InDetExtrapolator)
    kwargs.setdefault("ToolForCompetingROTsCreation", InDetCompetingRotCreator)
    kwargs.setdefault("ToolForUpdating", InDetUpdator)
    kwargs.setdefault("AnnealingScheme", [200., 81., 9., 4., 1., 1., 1.])
    kwargs.setdefault("DropOutlierCutValue", 1.E-7)
    kwargs.setdefault("OutlierCutValue", 0.01)
    acc.setPrivateTools(CompFactory.Trk.DeterministicAnnealingFilter(name = name, **kwargs))
    return acc

def InDetExtensionProcessorCfg(flags, SiTrackCollection=None, ExtendedTrackCollection = None, ExtendedTracksMap = None, doPhase=True, **kwargs):
    acc = ComponentAccumulator()

    ForwardTrackCollection = ExtendedTrackCollection
    # set output extension map name
    OutputExtendedTracks = ExtendedTracksMap

    if flags.InDet.trtExtensionType == 'DAF' :
        #
        # --- DAF Fitter setup
        #
        InDetExtensionFitter = acc.popToolsAndMerge(DeterministicAnnealingFilterCfg(flags, name = 'InDetDAF'+ flags.InDet.Tracking.extension))
        acc.addPublicTool(InDetExtensionFitter)
    else:
        fitter_args = {}
        if True: #flags.InDet.holeSearchInGX2Fit:
            fitter_args.setdefault("DoHoleSearch", True)
            from  InDetConfig.InDetRecToolConfig import InDetBoundaryCheckToolCfg
            InDetBoundaryCheckTool = acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags))
            acc.addPublicTool(InDetBoundaryCheckTool)
            fitter_args.setdefault("BoundaryCheckTool", InDetBoundaryCheckTool)
        if flags.InDet.Tracking.extension != "LowPt":
            InDetExtensionFitter = acc.popToolsAndMerge(TC.InDetTrackFitterCfg(flags, 'InDetTrackFitter_TRTExtension'+flags.InDet.Tracking.extension, **fitter_args))
            acc.addPublicTool(InDetExtensionFitter)
        else:
            InDetExtensionFitter = acc.popToolsAndMerge(TC.InDetTrackFitterLowPt(flags, 'InDetTrackFitter_TRTExtension'+flags.InDet.Tracking.extension, **fitter_args))
            acc.addPublicTool(InDetExtensionFitter)
    #
    # --- load scoring for extension
    #
    if flags.Beam.Type == "cosmics":
        InDetExtenScoringTool = acc.popToolsAndMerge(TC.InDetCosmicExtenScoringToolCfg(flags))
        acc.addPublicTool(InDetExtenScoringTool)
    else:
        InDetExtenScoringTool = acc.popToolsAndMerge(TC.InDetExtenScoringToolCfg(flags))
        acc.addPublicTool(InDetExtenScoringTool)
    #
    # --- get configured track extension processor
    #
    InDetTrackSummaryTool = acc.popToolsAndMerge(TC.InDetTrackSummaryToolCfg(flags))
    acc.addPublicTool(InDetTrackSummaryTool)

    if flags.InDet.materialInteractions:
        kwargs.setdefault("matEffects", flags.InDet.materialInteractionsType)
    else:
        kwargs.setdefault("matEffects", 0)

    kwargs.setdefault("TrackName", SiTrackCollection)
    kwargs.setdefault("ExtensionMap", OutputExtendedTracks)
    kwargs.setdefault("NewTrackName", ForwardTrackCollection)
    kwargs.setdefault("TrackFitter", InDetExtensionFitter)
    kwargs.setdefault("TrackSummaryTool", InDetTrackSummaryTool)
    kwargs.setdefault("ScoringTool", InDetExtenScoringTool)
    kwargs.setdefault("suppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.InDet.doBremRecovery)
    kwargs.setdefault("caloSeededBrem", flags.InDet.doCaloSeededBrem)
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.minPTBrem)
    kwargs.setdefault("RefitPrds", not (flags.InDet.refitROT or (flags.InDet.trtExtensionType == 'DAF')))
    
    if doPhase:
        kwargs.setdefault("Cosmics", True)

        acc.addEventAlgo(CompFactory.InDet.InDetExtensionProcessor(name = "InDetExtensionProcessorPhase" + flags.InDet.Tracking.extension, **kwargs))
    else:
        kwargs.setdefault("Cosmics", flags.Beam.Type == "cosmics")

        acc.addEventAlgo(CompFactory.InDet.InDetExtensionProcessor("InDetExtensionProcessor" + flags.InDet.Tracking.extension, **kwargs))

    return acc

##########################################################################################################################
# ------------------------------------------------------------
#
# ----------- Setup TRT Extension for New tracking
#
# ------------------------------------------------------------
def NewTrackingTRTExtensionCfg(flags, SiTrackCollection = None, ExtendedTrackCollection = None, ExtendedTracksMap = None, doPhase = True):
    acc = ComponentAccumulator()
    #
    # ---------- TRT_TrackExtension
    #
    if flags.InDet.doTRTExtensionNew:
        #
        # Track extension to TRT algorithm
        #
        if doPhase:
            acc.merge(TRT_TrackExtensionAlgCfg( flags,
                                                name = 'InDetTRT_ExtensionPhase' + flags.InDet.Tracking.extension,
                                                SiTrackCollection=SiTrackCollection,
                                                ExtendedTracksMap = ExtendedTracksMap))
        else:
            acc.merge(TRT_TrackExtensionAlgCfg( flags,  
                                                name = 'InDetTRT_Extension' + flags.InDet.Tracking.extension,
                                                SiTrackCollection=SiTrackCollection,
                                                ExtendedTracksMap = ExtendedTracksMap,
                                                TrackExtensionTool = acc.popToolsAndMerge(TC.InDetTRT_ExtensionToolCfg(flags))))
        acc.merge(InDetExtensionProcessorCfg(flags,
                                             SiTrackCollection = SiTrackCollection,
                                             ExtendedTrackCollection = ExtendedTrackCollection,
                                             ExtendedTracksMap = ExtendedTracksMap,
                                             doPhase = doPhase))
    return acc
##########################################################################################################################

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads

    ConfigFlags.Detector.GeometryPixel = True 
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.Detector.GeometryTRT   = True

    ConfigFlags.addFlag('InDet.doTRTExtension', True)
    ConfigFlags.addFlag('InDet.doExtensionProcessor', True)
    ConfigFlags.addFlag('InDet.useHolesFromPattern', False)
    ConfigFlags.addFlag('InDet.holeSearchInGX2Fit', True)

    # SiliconPreProcessing
    ConfigFlags.InDet.doPixelClusterSplitting = True
    ConfigFlags.InDet.doSiSPSeededTrackFinder = True

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    top_acc.merge(PixelReadoutGeometryCfg(ConfigFlags))
    top_acc.merge(SCT_ReadoutGeometryCfg(ConfigFlags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
    top_acc.merge(TRT_ReadoutGeometryCfg( ConfigFlags ))

    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg, MuonIdHelperSvcCfg
    top_acc.merge(MuonGeoModelCfg(ConfigFlags))
    top_acc.merge(MuonIdHelperSvcCfg(ConfigFlags))

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    top_acc.merge(BeamSpotCondAlgCfg(ConfigFlags))

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDistortionAlgCfg
    top_acc.merge(PixelDistortionAlgCfg(ConfigFlags))

    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTActiveCondAlgCfg
    top_acc.merge(TRTActiveCondAlgCfg(ConfigFlags))
    top_acc.merge(TC.TRT_DetElementsRoadCondAlgCfg())

    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool, PixelLorentzAngleCfg
    top_acc.addPublicTool(PixelLorentzAngleTool(ConfigFlags))
    top_acc.addPublicTool(top_acc.popToolsAndMerge(PixelLorentzAngleCfg(ConfigFlags)))

    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    top_acc.addPublicTool(top_acc.popToolsAndMerge(SCT_LorentzAngleCfg(ConfigFlags)))

    from PixelConditionsAlgorithms.PixelConditionsConfig import (PixelChargeCalibCondAlgCfg, PixelConfigCondAlgCfg, PixelDeadMapCondAlgCfg, PixelCablingCondAlgCfg, PixelReadoutSpeedAlgCfg, PixelOfflineCalibCondAlgCfg, PixelDistortionAlgCfg)
    top_acc.merge(PixelConfigCondAlgCfg(ConfigFlags))
    top_acc.merge(PixelDeadMapCondAlgCfg(ConfigFlags))
    top_acc.merge(PixelChargeCalibCondAlgCfg(ConfigFlags))
    top_acc.merge(PixelCablingCondAlgCfg(ConfigFlags))
    top_acc.merge(PixelReadoutSpeedAlgCfg(ConfigFlags))
    top_acc.merge(PixelOfflineCalibCondAlgCfg(ConfigFlags))
    top_acc.merge(PixelDistortionAlgCfg(ConfigFlags))

    top_acc.merge(TC.PixelClusterNnCondAlgCfg(ConfigFlags))
    top_acc.merge(TC.PixelClusterNnWithTrackCondAlgCfg(ConfigFlags))

    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    top_acc.merge(BeamPipeGeometryCfg(ConfigFlags))

    InputCollections = []
    
    InDetSpSeededTracksKey    = 'SiSPSeededTracks'  # InDetKeys.SiSpSeededTracks()
    SiSPSeededTrackCollectionKey = InDetSpSeededTracksKey

    ExtendedTrackCollection = 'ExtendedTracksPhase' # InDetKeys.ExtendedTracksPhase
    ExtendedTracksMap = 'ExtendedTracksMapPhase'    # InDetKeys.ExtendedTracksMapPhase

    #################### Additional Configuration  ########################
    #######################################################################
    ################# TRTPreProcessing Configuration ######################
    from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
    if not ConfigFlags.InDet.doDBMstandalone:
        top_acc.merge(TRTPreProcessingCfg(ConfigFlags,(not ConfigFlags.InDet.doTRTPhaseCalculation or ConfigFlags.Beam.Type =="collisions"),False))

    ################ TRTSegmentFinding Configuration ######################
    from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
    top_acc.merge(TRTSegmentFindingCfg( ConfigFlags,
                                        extension = "",
                                        InputCollections = InputCollections,
                                        BarrelSegments = 'TRTSegments', # InDetKeys.TRT_Segments
                                        doPhase = False))

    ####################### TrackingSiPattern #############################
    if ConfigFlags.InDet.doSiSPSeededTrackFinder:
        from InDetConfig.TrackingSiPatternConfig import SiSPSeededTrackFinderCfg
        top_acc.merge(SiSPSeededTrackFinderCfg( ConfigFlags,
                                                InputCollections = InputCollections, 
                                                SiSPSeededTrackCollectionKey = InDetSpSeededTracksKey))

    ########################## Clusterization #############################
    from InDetConfig.ClusterizationConfig import InDetClusterizationAlgorithmsCfg
    top_acc.merge(InDetClusterizationAlgorithmsCfg(ConfigFlags))

    ######################## PixelByteStreamErrs ##########################
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelHitDiscCnfgAlgCfg
    top_acc.merge(PixelHitDiscCnfgAlgCfg(ConfigFlags))

    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConfig import PixelRawDataProviderAlgCfg
    top_acc.merge(PixelRawDataProviderAlgCfg(ConfigFlags))
    ########################### TRTExtension  #############################
    top_acc.merge(NewTrackingTRTExtensionCfg(ConfigFlags,
                                             SiTrackCollection=InDetSpSeededTracksKey,
                                             ExtendedTrackCollection = ExtendedTrackCollection, 
                                             ExtendedTracksMap = ExtendedTracksMap,
                                             doPhase = False))
    #######################################################################

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5
    #
    top_acc.printConfig()
    top_acc.run(25)
    top_acc.store(open("test_TRTExtensionConfig.pkl", "wb"))
