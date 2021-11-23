# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
from IOVDbSvc.IOVDbSvcConfig                  import addFoldersSplitOnline
from InDetConfig.ITkRecToolConfig             import makeName
import AthenaCommon.SystemOfUnits               as   Units
#######################################################################

# @TODO retire once migration to TrackingGeometry conditions data is complete
from InDetRecExample.TrackingCommon import use_tracking_geometry_cond_alg

def copyArgs(kwargs, copy_list):
    dict_copy={}
    for elm in copy_list :
        if elm in kwargs :
            dict_copy[elm]=kwargs[elm]
    return dict_copy

def stripArgs(kwargs, copy_list) :
    dict_copy = copyArgs(kwargs,copy_list)
    for an_arg in copy_list :
        kwargs.pop(an_arg,None)
    return dict_copy


def ITkEtaDependentCutsSvcCfg(flags, name = 'ITkEtaDependentCutsSvc', **kwargs):
    acc = ComponentAccumulator()
    the_name = name + flags.ITk.Tracking.extension

    cuts = flags.ITk.Tracking

    kwargs.setdefault("etaBins",              cuts.etaBins)
    kwargs.setdefault("etaWidthBrem",         cuts.etaWidthBrem)
    kwargs.setdefault("maxdImpactSSSSeeds",   cuts.maxdImpactSSSSeeds)
    kwargs.setdefault("maxDoubleHoles",       cuts.maxDoubleHoles)
    kwargs.setdefault("maxHoles",             cuts.maxHoles)
    kwargs.setdefault("maxPixelHoles",        cuts.maxPixelHoles)
    kwargs.setdefault("maxPrimaryImpact",     cuts.maxPrimaryImpact)
    kwargs.setdefault("maxSctHoles",          cuts.maxSctHoles)
    kwargs.setdefault("maxShared",            cuts.maxShared)
    kwargs.setdefault("maxZImpact",           cuts.maxZImpact)
    kwargs.setdefault("minClusters",          cuts.minClusters)
    kwargs.setdefault("minPixelHits",         cuts.minPixel)
    kwargs.setdefault("minPT",                cuts.minPT)
    kwargs.setdefault("minPTBrem",            cuts.minPTBrem)
    kwargs.setdefault("minSiNotShared",       cuts.minSiNotShared)
    kwargs.setdefault("maxHolesGapPattern",   cuts.nHolesGapMax)
    kwargs.setdefault("maxHolesPattern",      cuts.nHolesMax)
    kwargs.setdefault("nWeightedClustersMin", cuts.nWeightedClustersMin)
    kwargs.setdefault("phiWidthBrem",         cuts.phiWidthBrem)
    kwargs.setdefault("Xi2max",               cuts.Xi2max)
    kwargs.setdefault("Xi2maxNoAdd",          cuts.Xi2maxNoAdd)

    ITkEtaDependentCutsSvc = CompFactory.InDet.InDetEtaDependentCutsSvc( name = the_name, **kwargs )
    acc.addService(ITkEtaDependentCutsSvc)
    return acc

def ITkTruthClusterizationFactoryCfg(flags, name = 'ITkTruthClusterizationFactory', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName(name, kwargs)

    kwargs.setdefault("InputSDOMap", 'ITkPixelSDO_Map')
    ITkTruthClusterizationFactory = CompFactory.InDet.TruthClusterizationFactory( name = the_name, **kwargs )
    acc.setPrivateTools(ITkTruthClusterizationFactory)
    return acc

def ITkPixelClusterOnTrackToolBaseCfg(flags, name="ITkPixelClusterOnTrackTool", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName(name, kwargs)

    from PixelConditionsAlgorithms.ITkPixelConditionsConfig import ITkPixelOfflineCalibCondAlgCfg
    acc.merge(ITkPixelOfflineCalibCondAlgCfg(flags))

    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    if (flags.Beam.Type == "cosmics"):
        kwargs.setdefault("ErrorStrategy", 0)
        kwargs.setdefault("PositionStrategy", 0)

    kwargs.setdefault("applyNNcorrection", False )
    kwargs.setdefault("SplitClusterAmbiguityMap", 'SplitClusterAmbiguityMap' + split_cluster_map_extension )
    kwargs.setdefault("RunningTIDE_Ambi", flags.ITk.doTIDE_Ambi )

    kwargs.setdefault("PixelErrorScalingKey", "")

    acc.setPrivateTools(CompFactory.ITk.PixelClusterOnTrackTool(the_name, **kwargs))
    return acc

def ITkPixelClusterOnTrackToolDigitalCfg(flags, name='ITkPixelClusterOnTrackToolDigital', **kwargs):
    acc = ComponentAccumulator()

    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleCfg
        ITkPixelLorentzAngleTool = acc.popToolsAndMerge(ITkPixelLorentzAngleCfg(flags))
        kwargs.setdefault("LorentzAngleTool", ITkPixelLorentzAngleTool )

    if flags.ITk.doDigitalROTCreation:
        kwargs.setdefault("applyNNcorrection", False )
        kwargs.setdefault("NNIBLcorrection", False )
        kwargs.setdefault("ErrorStrategy", 2 )
        kwargs.setdefault("PositionStrategy", 1 )
        kwargs.setdefault("SplitClusterAmbiguityMap", "" )
    else :
        kwargs.setdefault("SplitClusterAmbiguityMap", "" )

    ITkPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkPixelClusterOnTrackTool)
    return acc

def ITkPixelClusterOnTrackToolNNSplittingCfg(flags, name='ITkPixelClusterOnTrackToolNNSplitting', **kwargs):
    acc = ComponentAccumulator()

    if flags.ITk.doPixelClusterSplitting and flags.ITk.pixelClusterSplittingType == 'NeuralNet':
        if 'NnClusterizationFactory' not in kwargs :
            ITkTruthClusterizationFactory = acc.popToolsAndMerge(ITkTruthClusterizationFactoryCfg(flags))
            kwargs.setdefault("NnClusterizationFactory", ITkTruthClusterizationFactory) #Truth-based for ITk for now

    ITkPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkPixelClusterOnTrackTool)
    return acc

def ITkPixelClusterOnTrackToolCfg(flags, name='ITkPixelClusterOnTrackTool', **kwargs):
    acc = ComponentAccumulator()

    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleCfg
        ITkPixelLorentzAngleTool = acc.popToolsAndMerge(ITkPixelLorentzAngleCfg(flags))
        kwargs.setdefault("LorentzAngleTool", ITkPixelLorentzAngleTool )

    ITkPixelClusterOnTrackTool = None
    if flags.ITk.doDigitalROTCreation:
        ITkPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolDigitalCfg(flags, name=name, **kwargs))
    else:
        ITkPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolNNSplittingCfg(flags, name=name, **kwargs))

    acc.setPrivateTools(ITkPixelClusterOnTrackTool)
    return acc

def ITkStripClusterOnTrackToolCfg(flags, name='ITkStrip_ClusterOnTrackTool', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName(name, kwargs)

    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleCfg
        ITkStripLorentzAngleTool = acc.popToolsAndMerge( ITkStripLorentzAngleCfg(flags) )
        kwargs.setdefault("LorentzAngleTool", ITkStripLorentzAngleTool )

    kwargs.setdefault("CorrectionStrategy", 0 ) # do correct position bias
    kwargs.setdefault("ErrorStrategy", 0 ) # use width / sqrt(12)

    kwargs.setdefault("ErrorScalingKey", "")

    acc.setPrivateTools(CompFactory.ITk.StripClusterOnTrackTool(the_name, **kwargs))
    return acc

def ITkBroadStripClusterOnTrackToolCfg(flags, name='ITkBroadStripClusterOnTrackTool', **kwargs):
    kwargs.setdefault("ErrorStrategy", 0)
    return ITkStripClusterOnTrackToolCfg(flags, name=name, **kwargs)

def ITkBroadPixelClusterOnTrackToolCfg(flags, name='ITkBroadPixelClusterOnTrackTool', **kwargs):
    kwargs.setdefault("ErrorStrategy", 0)
    return ITkPixelClusterOnTrackToolCfg(flags, name=name, **kwargs)

def ITk_RIO_OnTrackErrorScalingCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    the_name=kwargs.pop("name", None)

    if flags.GeoModel.Run == "RUN1":
        error_scaling_type   = ["PixelRIO_OnTrackErrorScalingRun1"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]
    else:  # Run 2 and 3
        error_scaling_type   = ["PixelRIO_OnTrackErrorScaling"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]
    # TODO: cover Run 4 and beyond

    error_scaling_type      += ["SCTRIO_OnTrackErrorScaling"]
    error_scaling_outkey    += ["/Indet/TrkErrorScalingSCT"]

    acc.merge(addFoldersSplitOnline(flags, 'INDET','/Indet/Onl/TrkErrorScaling','/Indet/TrkErrorScaling', className="CondAttrListCollection"))

    kwargs.setdefault("ReadKey", "/Indet/TrkErrorScaling")
    kwargs.setdefault("ErrorScalingType", error_scaling_type)
    kwargs.setdefault("OutKeys", error_scaling_outkey)

    if the_name is not None:
        kwargs.setdefault("name", the_name)
    acc.addCondAlgo(CompFactory.RIO_OnTrackErrorScalingCondAlg(**kwargs))
    return acc

def ITkRotCreatorCfg(flags, name='ITkRotCreator', **kwargs):
    acc = ComponentAccumulator()
    strip_args=['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix']
    pix_cluster_on_track_args = copyArgs(kwargs,strip_args)
    the_name = makeName(name, kwargs)

    for an_arg in  strip_args:
        kwargs.pop(an_arg, None)

    use_broad_cluster_pix = flags.ITk.useBroadPixClusterErrors
    use_broad_cluster_strip = flags.ITk.useBroadStripClusterErrors

    if 'ToolPixelCluster' not in kwargs :
        if use_broad_cluster_pix :
            ToolPixelCluster = acc.popToolsAndMerge(ITkBroadPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args))
        else:
            ToolPixelCluster = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault("ToolPixelCluster", ToolPixelCluster)

    if 'ToolSCT_Cluster' not in kwargs :
        if use_broad_cluster_strip :
            ToolStripCluster = acc.popToolsAndMerge(ITkBroadStripClusterOnTrackToolCfg(flags))
        else :
            ToolStripCluster = acc.popToolsAndMerge(ITkStripClusterOnTrackToolCfg(flags))
        kwargs.setdefault("ToolSCT_Cluster", ToolStripCluster)

    kwargs.setdefault("ToolTRT_DriftCircle", None)
    kwargs.setdefault('Mode', 'indet')
    acc.setPrivateTools(CompFactory.Trk.RIO_OnTrackCreator(name=the_name, **kwargs))
    return acc

def ITkPRDtoTrackMapToolGangedPixelsCfg(flags, name='ITkPRDtoTrackMapToolGangedPixels', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'ITkPixelClusterAmbiguitiesMap')
    kwargs.setdefault("addTRToutliers", False)
    acc.setPrivateTools(CompFactory.InDet.InDetPRDtoTrackMapToolGangedPixels( name=the_name, **kwargs))
    return acc

def ITkTrackPRD_AssociationCfg(flags, name='ITkTrackPRD_Association', **kwargs):
    acc = ComponentAccumulator()
    from InDetConfig.ITkRecToolConfig import makeNameGetPreAndSuffix
    the_name,prefix,suffix=makeNameGetPreAndSuffix(name,kwargs)

    if kwargs.get('TracksName', None) is None :
        raise Exception('Not TracksName argument provided')

    AssociationTool = acc.popToolsAndMerge(ITkPRDtoTrackMapToolGangedPixelsCfg(flags))
    kwargs.setdefault("AssociationTool", AssociationTool \
                      if 'AssociationTool' not in kwargs else None )
    kwargs.setdefault("AssociationMapName", prefix+'PRDtoTrackMap'+suffix )
    acc.addEventAlgo(CompFactory.InDet.InDetTrackPRD_Association(name = the_name, **kwargs))
    return acc

def ITkSummaryHelperNoHoleSearchCfg(flags, name='ITkSummaryHelperNoHoleSearch', **kwargs):
    if 'HoleSearch' not in kwargs :
        kwargs.setdefault("HoleSearch", None)
    from  InDetConfig.ITkRecToolConfig import ITkTrackSummaryHelperToolCfg
    return ITkTrackSummaryHelperToolCfg(flags, name = name, **kwargs)

def ITkTrackSummaryToolCfg(flags, name='ITkTrackSummaryTool', **kwargs):
    acc = ComponentAccumulator()
    # makeName will remove the namePrefix in suffix from kwargs, so copyArgs has to be first
    hlt_args = copyArgs(kwargs,['isHLT','namePrefix'])
    id_helper_args = copyArgs(kwargs,['ClusterSplitProbabilityName','namePrefix','nameSuffix']) if 'ClusterSplitProbabilityName' in kwargs else {}
    kwargs.pop('ClusterSplitProbabilityName',None)
    kwargs.pop('isHLT',None)
    the_name = makeName( name, kwargs)
    do_holes=kwargs.get("doHolesInDet",True)
    if do_holes :
        id_helper_args.update(hlt_args)

    if 'InDetSummaryHelperTool' not in kwargs :
        if do_holes:
            from  InDetConfig.ITkRecToolConfig import ITkTrackSummaryHelperToolCfg
            ITkSummaryHelperTool = acc.getPrimaryAndMerge(ITkTrackSummaryHelperToolCfg(flags, **id_helper_args))
        else:
            ITkSummaryHelperTool = acc.getPrimaryAndMerge(ITkSummaryHelperNoHoleSearchCfg(flags, **id_helper_args))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperTool)

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", do_holes)
    kwargs.setdefault("TRT_ElectronPidTool", None) # we don't want to use those tools during pattern
    kwargs.setdefault("PixelToTPIDTool", None) # we don't want to use those tools during pattern
    acc.addPublicTool(CompFactory.Trk.TrackSummaryTool(name = the_name, **kwargs), primary=True)
    return acc

def ITkSummaryHelperSharedHitsCfg(flags, name='ITkSummaryHelperSharedHits', **kwargs):
    kwargs.setdefault("PixelToTPIDTool", None)
    kwargs.setdefault("TestBLayerTool", None)
    kwargs.setdefault("DoSharedHits", flags.ITk.doSharedHits)

    from  InDetConfig.ITkRecToolConfig import ITkTrackSummaryHelperToolCfg
    return ITkTrackSummaryHelperToolCfg(flags, name = name, **kwargs)

def ITkTrackSummaryToolSharedHitsCfg(flags, name='ITkTrackSummaryToolSharedHits',**kwargs):
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs :
        copy_args=['ClusterSplitProbabilityName','namePrefix','nameSuffix']
        do_holes=kwargs.get("doHolesInDet",True)
        if do_holes :
            copy_args += ['isHLT']
        id_helper_args = copyArgs(kwargs,copy_args) if 'ClusterSplitProbabilityName' in kwargs else {}
        kwargs.pop('ClusterSplitProbabilityName',None)

        ITkSummaryHelperSharedHits = acc.getPrimaryAndMerge(ITkSummaryHelperSharedHitsCfg(flags, **id_helper_args))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperSharedHits)

    kwargs.setdefault( "PixelToTPIDTool", None)
    kwargs.setdefault( "doSharedHits", flags.ITk.doSharedHits)

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags, name, **kwargs))
    acc.addPublicTool(ITkTrackSummaryTool, primary=True)
    return acc

def ITkMultipleScatteringUpdatorCfg(flags, name = "ITkMultipleScatteringUpdator", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    kwargs.setdefault( "UseTrkUtils", False)
    MultipleScatteringUpdator = CompFactory.Trk.MultipleScatteringUpdator(name = the_name, **kwargs)
    acc.setPrivateTools(MultipleScatteringUpdator)
    return acc

def ITkMeasRecalibSTCfg(flags, name='ITkMeasRecalibST', **kwargs) :
    acc = ComponentAccumulator()

    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

    if 'BroadPixelClusterOnTrackTool' not in kwargs :
        ITkBroadPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkBroadPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('BroadPixelClusterOnTrackTool', ITkBroadPixelClusterOnTrackTool)
    if 'BroadSCT_ClusterOnTrackTool' not in kwargs :
        ITkBroadStripClusterOnTrackTool = acc.popToolsAndMerge(ITkBroadStripClusterOnTrackToolCfg(flags))
        kwargs.setdefault('BroadSCT_ClusterOnTrackTool', ITkBroadStripClusterOnTrackTool)
    if 'CommonRotCreator' not in kwargs :
        ITkRefitRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, 
                                                                   name = 'ITkRefitRotCreator',
                                                                   **pix_cluster_on_track_args))
        kwargs.setdefault('CommonRotCreator', ITkRefitRotCreator)

    MeasRecalibSteeringTool = CompFactory.Trk.MeasRecalibSteeringTool(name, **kwargs)
    acc.setPrivateTools(MeasRecalibSteeringTool)
    return acc

def ITkKalmanTrackFitterBaseCfg(flags, name='ITkKalmanTrackFitterBase',**kwargs) :
    acc = ComponentAccumulator()
    nameSuffix=kwargs.pop('nameSuffix','')
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName'])
    if len(pix_cluster_on_track_args)>0 and len(nameSuffix)>0 :
        pix_cluster_on_track_args['nameSuffix']=nameSuffix

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    acc.addPublicTool(extrapolator)  # TODO: migrate to private?
    kwargs.setdefault("ExtrapolatorHandle", extrapolator)

    if 'RIO_OnTrackCreatorHandle' not in kwargs :
        ITkRefitRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, 
                                                                   name = 'ITkRefitRotCreator',
                                                                   **pix_cluster_on_track_args))
        kwargs.setdefault("RIO_OnTrackCreatorHandle", ITkRefitRotCreator)

    from  InDetConfig.ITkRecToolConfig import ITkUpdatorCfg
    ITkUpdator = acc.popToolsAndMerge(ITkUpdatorCfg(flags))
    ITkBKS = acc.popToolsAndMerge(ITkBKSCfg(flags))
    ITkKOL = acc.popToolsAndMerge(ITkKOLCfg(flags))

    kwargs.setdefault('MeasurementUpdatorHandle', ITkUpdator)
    kwargs.setdefault('KalmanSmootherHandle', ITkBKS)
    kwargs.setdefault('KalmanOutlierLogicHandle', ITkKOL)
    kwargs.setdefault('DynamicNoiseAdjustorHandle', None)
    kwargs.setdefault('BrempointAnalyserHandle', None)
    kwargs.setdefault('AlignableSurfaceProviderHandle',None)

    if len(pix_cluster_on_track_args)>0 :
        if 'RecalibratorHandle' not in kwargs :
            the_tool_name = 'ITkMeasRecalibST'
            ITkMeasRecalibST = acc.popToolsAndMerge(ITkMeasRecalibSTCfg(flags, name=the_tool_name+nameSuffix, **pix_cluster_on_track_args))
            kwargs.setdefault('RecalibratorHandle', ITkMeasRecalibST)
    else :
        ITkMeasRecalibST = acc.popToolsAndMerge(ITkMeasRecalibSTCfg(flags))
        kwargs.setdefault('RecalibratorHandle', ITkMeasRecalibST)

    KalmanFitter = CompFactory.Trk.KalmanFitter(name=name, **kwargs)
    acc.setPrivateTools(KalmanFitter)
    return acc

def ITkDNAdjustorCfg(flags, name='ITkDNAdjustor', **kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.InDetDynamicNoiseAdjustment(name=name,**kwargs))
    return acc

def ITkDNASeparatorCfg(flags, name='ITkDNASeparator',**kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.InDet.InDetDNASeparator(name=name,**kwargs))
    return acc

def ITkAnnealBKSCfg(flags, name='ITkAnnealBKS',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault('InitialCovarianceSeedFactor',200.)
    acc.setPrivateTools(CompFactory.Trk.KalmanSmoother(name=name,**kwargs))
    return acc

def ITkKalmanCompetingPixelClustersToolCfg(flags, name='ITkKalmanCompetingPixelClustersTool',**kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault('WeightCutValueBarrel',5.5)
    kwargs.setdefault('WeightCutValueEndCap',5.5)

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
        acc.addPublicTool(extrapolator)  # TODO: migrate to private?
        kwargs.setdefault("Extrapolator", extrapolator)

    acc.setPrivateTools(CompFactory.InDet.CompetingPixelClustersOnTrackTool(name=name, **kwargs))
    return acc

def ITkKalmanCompetingStripClustersToolCfg(flags, name='ITkKalmanCompetingStripClustersTool',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault('WeightCutValueBarrel',5.5)
    kwargs.setdefault('WeightCutValueEndCap',5.5)

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
        acc.addPublicTool(extrapolator)  # TODO: migrate to private?
        kwargs.setdefault("Extrapolator", extrapolator)

    acc.setPrivateTools(CompFactory.InDet.CompetingSCT_ClustersOnTrackTool(name=name,**kwargs))
    return acc

def ITkKalmanCompetingRIOsToolCfg(flags, name='ITkKalmanCompetingRIOsTool',**kwargs) :
    acc = ComponentAccumulator()

    ITkKalmanCompetingPixelClustersTool = acc.popToolsAndMerge(ITkKalmanCompetingPixelClustersToolCfg(flags))
    ITkKalmanCompetingStripClustersTool = acc.popToolsAndMerge(ITkKalmanCompetingStripClustersToolCfg(flags))    
    kwargs.setdefault('ToolForCompPixelClusters', ITkKalmanCompetingPixelClustersTool)
    kwargs.setdefault('ToolForCompSCT_Clusters', ITkKalmanCompetingStripClustersTool)
    acc.setPrivateTools(CompFactory.Trk.CompetingRIOsOnTrackTool(name=name,**kwargs))
    return acc

def ITkKalmanInternalDAFCfg(flags, name='ITkKalmanInternalDAF',**kwargs) :
    acc = ComponentAccumulator()

    ITkKalmanCompetingRIOsTool = acc.popToolsAndMerge(ITkKalmanCompetingRIOsToolCfg(flags))
    kwargs.setdefault('CompetingRIOsOnTrackCreator',ITkKalmanCompetingRIOsTool)

    if 'BackwardSmoother' not in kwargs:
        ITkAnnealBKS = acc.popToolsAndMerge(ITkAnnealBKSCfg(flags))
        kwargs.setdefault('BackwardSmoother',ITkAnnealBKS)

    acc.setPrivateTools(CompFactory.Trk.KalmanPiecewiseAnnealingFilter(name=name,**kwargs))
    return acc

def ITkAnnealFKFCfg(flags, name='ITkAnnealFKF',**kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.ForwardKalmanFitter(name=name,**kwargs))
    return acc

def ITkFKFRefCfg(flags, name='ITkFKFRef',**kwargs) :
    kwargs.setdefault('StateChi2PerNDFPreCut',30.0)
    return ITkAnnealFKFCfg(name=name,**kwargs)

def ITkKalmanInternalDAFRefCfg(flags, name='ITkKalmanInternalDAFRef',**kwargs) :
    acc = ComponentAccumulator()
    
    ITkAnnealFKF = acc.popToolsAndMerge(ITkAnnealFKFCfg(flags))
    ITkBKS = acc.popToolsAndMerge(ITkBKSCfg(flags))
    kwargs.setdefault('ForwardFitter', ITkAnnealFKF)
    kwargs.setdefault('BackwardSmoother', ITkBKS)

    ITkKalmanInternalDAF = acc.popToolsAndMerge(ITkKalmanInternalDAFCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkKalmanInternalDAF)
    return acc

def ITkBroadRotCreatorCfg(flags, name='ITkBroadRotCreator', **kwargs) :
    acc = ComponentAccumulator()

    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
        ITkBroadPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkBroadPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('ToolPixelCluster', ITkBroadPixelClusterOnTrackTool)

    if 'ToolSCT_Cluster' not in kwargs :
        ITkBroadStripClusterOnTrackTool = acc.popToolsAndMerge(ITkBroadStripClusterOnTrackToolCfg(flags))
        kwargs.setdefault('ToolSCT_Cluster', ITkBroadStripClusterOnTrackTool)

    ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, name = name, **kwargs))
    acc.setPrivateTools(ITkRotCreator)
    return acc

def ITkKalmanFitterCfg(flags, name='ITkKalmanFitter',**kwargs) :
    acc = ComponentAccumulator()

    ITkFKF = acc.popToolsAndMerge(ITkFKFCfg(flags))
    kwargs.setdefault('ForwardKalmanFitterHandle', ITkFKF)

    if flags.ITk.doBremRecovery:
        
        ITkDNAdjustor = acc.popToolsAndMerge(ITkDNAdjustorCfg(flags))
        ITkDNASeparator = acc.popToolsAndMerge(ITkDNASeparatorCfg(flags))

        kwargs.setdefault('DynamicNoiseAdjustorHandle', ITkDNAdjustor)
        kwargs.setdefault('BrempointAnalyserHandle', ITkDNASeparator)
        kwargs.setdefault('DoDNAForElectronsOnly', True)
    else:
        kwargs.setdefault('DynamicNoiseAdjustorHandle', None)
        kwargs.setdefault('BrempointAnalyserHandle', None)

    ITkKalmanInternalDAF = acc.popToolsAndMerge(ITkKalmanInternalDAFCfg(flags))
    kwargs.setdefault('InternalDAFHandle', ITkKalmanInternalDAF)

    ITkKalmanTrackFitter = acc.popToolsAndMerge(ITkKalmanTrackFitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkKalmanTrackFitter)
    return acc

def ITkKalmanDNAFitterCfg(flags, name='ITkKalmanDNAFitter',**kwargs) :
    acc = ComponentAccumulator()

    ITkFKF = acc.popToolsAndMerge(ITkFKFCfg(flags))
    ITkDNAdjustor = acc.popToolsAndMerge(ITkDNAdjustorCfg(flags))
    ITkDNASeparator = acc.popToolsAndMerge(ITkDNASeparatorCfg(flags))
    ITkKalmanInternalDAF = acc.popToolsAndMerge(ITkKalmanInternalDAFCfg(flags))

    kwargs.setdefault('ForwardKalmanFitterHandle', ITkFKF)
    kwargs.setdefault('DynamicNoiseAdjustorHandle', ITkDNAdjustor)
    kwargs.setdefault('BrempointAnalyserHandle', ITkDNASeparator)
    kwargs.setdefault('InternalDAFHandle', ITkKalmanInternalDAF)

    ITkKalmanTrackFitter = acc.popToolsAndMerge(ITkKalmanTrackFitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkKalmanTrackFitter)
    return acc

def ITkReferenceKalmanFitterCfg(flags, name='ITkReferenceKalmanFitter',**kwargs) :
    acc = ComponentAccumulator()

    ITkFKFRef = acc.popToolsAndMerge(ITkFKFRefCfg(flags))
    ITkKalmanInternalDAFRef = acc.popToolsAndMerge(ITkKalmanInternalDAFRefCfg(flags))

    kwargs.setdefault('ForwardKalmanFitterHandle', ITkFKFRef)
    kwargs.setdefault('InternalDAFHandle', ITkKalmanInternalDAFRef)

    ITkKalmanTrackFitter = acc.popToolsAndMerge(ITkKalmanTrackFitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkKalmanTrackFitter)
    return acc

def ITkDistributedKalmanFilterCfg(flags, name="ITkDistributedKalmanFilter", **kwargs) :
    acc = ComponentAccumulator()

    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

    if 'ExtrapolatorTool' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs.setdefault('ExtrapolatorTool', acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    if 'ROTcreator' not in kwargs:
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('ROTcreator', ITkRotCreator)

    DistributedKalmanFilter = CompFactory.Trk.DistributedKalmanFilter(name = name, **kwargs)
    acc.setPrivateTools(DistributedKalmanFilter)
    return acc

def ITkGlobalChi2FitterCfg(flags, name='ITkGlobalChi2Fitter', **kwargs) :
    acc = ComponentAccumulator()

    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
    # PHF cut during fit iterations to save CPU time
    kwargs.setdefault('MinPHFCut', flags.ITk.Tracking.minTRTPrecFrac)

    if 'RotCreatorTool' not in kwargs :
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('RotCreatorTool', ITkRotCreator)

    use_broad_cluster_any = flags.ITk.useBroadClusterErrors

    if 'BroadRotCreatorTool' not in kwargs and  not flags.ITk.doRefit:
        ITkBroadRotCreator = acc.popToolsAndMerge(ITkBroadRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('BroadRotCreatorTool', ITkBroadRotCreator)

    if flags.ITk.doRefit or use_broad_cluster_any is True:
        kwargs.setdefault('RecalibrateSilicon', False)

    if flags.ITk.doRefit:
        kwargs.setdefault('BroadRotCreatorTool', None)
        kwargs.setdefault('ReintegrateOutliers', False)
        kwargs.setdefault('RecalibrateTRT', False)

    if flags.ITk.doRobustReco:
        kwargs.setdefault('OutlierCut', 10.0)
        kwargs.setdefault('TrackChi2PerNDFCut', 20)

    if flags.ITk.doRobustReco or flags.Beam.Type == 'cosmics':
        kwargs.setdefault('MaxOutliers', 99)

    if flags.Beam.Type == 'cosmics' or flags.ITk.doBeamHalo:
        kwargs.setdefault('Acceleration', False)

    if flags.ITk.materialInteractions and not flags.BField.solenoidOn:
        kwargs.setdefault('Momentum', 1000.*Units.MeV)

    ITkGlobalChi2Fitter = acc.popToolsAndMerge(ITkGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkGlobalChi2Fitter)
    return acc

def ITkGsfMaterialUpdatorCfg(flags, name='ITkGsfMaterialUpdator', **kwargs) :
    the_name = makeName( name, kwargs)
    acc = ComponentAccumulator()

    if 'MaximumNumberOfComponents' not in kwargs :
        kwargs.setdefault('MaximumNumberOfComponents', 12)

    GsfMaterialMixtureConvolution = CompFactory.Trk.GsfMaterialMixtureConvolution (name = the_name, **kwargs)
    acc.setPrivateTools(GsfMaterialMixtureConvolution)
    return acc

def ITkGsfExtrapolatorCfg(flags, name='ITkGsfExtrapolator', **kwargs) :
    the_name = makeName(name,kwargs)
    acc = ComponentAccumulator()

    if 'Propagators' not in kwargs :
        from TrkConfig.AtlasExtrapolatorToolsConfig import ITkPropagatorCfg
        ITkPropagator = acc.getPrimaryAndMerge(ITkPropagatorCfg(flags))
        kwargs.setdefault('Propagators', [ ITkPropagator ])

    if 'Navigator' not in kwargs :
        from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasNavigatorCfg
        Navigator = acc.getPrimaryAndMerge(AtlasNavigatorCfg(flags))
        kwargs.setdefault('Navigator', Navigator)

    if 'GsfMaterialConvolution' not in kwargs :
        ITkGsfMaterialUpdato = acc.popToolsAndMerge(ITkGsfMaterialUpdatorCfg(flags))
        kwargs.setdefault('GsfMaterialConvolution', ITkGsfMaterialUpdato)

    kwargs.setdefault('SearchLevelClosestParameters', 10)
    kwargs.setdefault('StickyConfiguration', True)
    kwargs.setdefault('SurfaceBasedMaterialEffects', False)

    GsfExtrapolator = CompFactory.Trk.GsfExtrapolator(name = the_name, **kwargs)
    acc.setPrivateTools(GsfExtrapolator)
    return acc

def ITkGaussianSumFitterCfg(flags, name='ITkGaussianSumFitter', **kwargs) :
    acc = ComponentAccumulator()

    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

    if 'ToolForROTCreation' not in kwargs :
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('ToolForROTCreation', ITkRotCreator)

    if 'ToolForExtrapolation' not in kwargs :
        ITkGsfExtrapolator = acc.popToolsAndMerge(ITkGsfExtrapolatorCfg(flags))
        kwargs.setdefault('ToolForExtrapolation', ITkGsfExtrapolator)

    kwargs.setdefault('ReintegrateOutliers', False)
    kwargs.setdefault('MakePerigee', True)
    kwargs.setdefault('RefitOnMeasurementBase', True)
    kwargs.setdefault('DoHitSorting', True)

    GaussianSumFitter = CompFactory.Trk.GaussianSumFitter(name = name, **kwargs)
    acc.setPrivateTools(GaussianSumFitter)
    return acc

def ITkTrackFitterCfg(flags, name='ITkTrackFitter', **kwargs) :
    return {
            'KalmanFitter'            : ITkKalmanFitterCfg,
            'KalmanDNAFitter'         : ITkKalmanDNAFitterCfg,
            'ReferenceKalmanFitter'   : ITkReferenceKalmanFitterCfg,
            'DistributedKalmanFilter' : ITkDistributedKalmanFilterCfg,
            'GlobalChi2Fitter'        : ITkGlobalChi2FitterCfg,
            'GaussianSumFilter'       : ITkGaussianSumFitterCfg
    }[flags.ITk.trackFitterType](flags, name=name, **kwargs)

def ITkGlobalChi2FitterBaseCfg(flags, name='ITkGlobalChi2FitterBase', **kwargs) :
    acc = ComponentAccumulator()

    if 'TrackingGeometrySvc' not in kwargs :
        if not use_tracking_geometry_cond_alg :
            from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
            acc.merge(TrackingGeometrySvcCfg(flags))
            kwargs.setdefault("TrackingGeometrySvc", acc.getService('AtlasTrackingGeometrySvc') )

    if 'TrackingGeometryReadKey' not in kwargs :
        if use_tracking_geometry_cond_alg :
            from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
            acc.merge( TrackingGeometryCondAlgCfg(flags) )
            # @TODO howto get the TrackingGeometryKey from the TrackingGeometryCondAlgCfg ?
            kwargs.setdefault("TrackingGeometryReadKey", 'AtlasTrackingGeometry')

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasNavigatorCfg, ITkPropagatorCfg, ITkMaterialEffectsUpdatorCfg
    from InDetConfig.ITkRecToolConfig import ITkUpdatorCfg

    Extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    Navigator = acc.getPrimaryAndMerge(AtlasNavigatorCfg(flags))
    ITkPropagator = acc.getPrimaryAndMerge(ITkPropagatorCfg(flags))
    ITkUpdator = acc.popToolsAndMerge(ITkUpdatorCfg(flags))
    ITkMultipleScatteringUpdator = acc.popToolsAndMerge(ITkMultipleScatteringUpdatorCfg(flags))
    ITkMaterialEffectsUpdator = acc.getPrimaryAndMerge(ITkMaterialEffectsUpdatorCfg(flags))

    kwargs.setdefault("ExtrapolationTool", Extrapolator)
    kwargs.setdefault("NavigatorTool", Navigator)
    kwargs.setdefault("PropagatorTool", ITkPropagator)
    kwargs.setdefault("MultipleScatteringTool", ITkMultipleScatteringUpdator)
    kwargs.setdefault("MeasurementUpdateTool", ITkUpdator)
    kwargs.setdefault("MaterialUpdateTool", ITkMaterialEffectsUpdator)
    kwargs.setdefault("StraightLine", not flags.BField.solenoidOn)
    kwargs.setdefault("OutlierCut", 4)
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("ReintegrateOutliers", True)
    kwargs.setdefault("RecalibrateSilicon", True)
    kwargs.setdefault("MaxIterations", 40)
    kwargs.setdefault("Acceleration", True)
    kwargs.setdefault("RecalculateDerivatives", flags.ITk.doMinBias or flags.Beam.Type == 'cosmics' or flags.ITk.doBeamHalo)
    kwargs.setdefault("TrackChi2PerNDFCut", 7)

    GlobalChi2Fitter = CompFactory.Trk.GlobalChi2Fitter(name=name, **kwargs)
    acc.setPrivateTools(GlobalChi2Fitter)
    return acc

def ITkGlobalChi2FitterLowPtCfg(flags, name='ITkGlobalChi2FitterLowPt', **kwargs) :
    acc = ComponentAccumulator()
    # @TODO TrackingGeometrySvc was not set but is set now
    #       RotCreatorTool and BroadRotCreatorTool not set
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

    if 'RotCreatorTool' not in kwargs :
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('RotCreatorTool', ITkRotCreator)

    if 'BroadRotCreatorTool' not in kwargs and  not flags.ITk.doRefit:
        ITkBroadRotCreator = acc.popToolsAndMerge(ITkBroadRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('BroadRotCreatorTool', ITkBroadRotCreator)

    kwargs.setdefault('OutlierCut', 5.0)
    kwargs.setdefault('Acceleration', False)
    kwargs.setdefault('RecalculateDerivatives', True)
    kwargs.setdefault('TrackChi2PerNDFCut', 10)

    ITkGlobalChi2Fitter = acc.popToolsAndMerge(ITkGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkGlobalChi2Fitter)
    return acc

def ITkTrackFitterLowPtCfg(flags, name='ITkTrackFitter', **kwargs) :
    acc = ComponentAccumulator()
    ITkTrackFitter = None
    if flags.ITk.trackFitterType != 'GlobalChi2Fitter' :
        ITkTrackFitter = acc.popToolsAndMerge(ITkTrackFitterCfg(flags, name, **kwargs))
    else :
        ITkTrackFitter = acc.popToolsAndMerge(ITkGlobalChi2FitterLowPtCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkTrackFitter)
    return acc

def ITkGlobalChi2FitterBTCfg(flags, name='ITkGlobalChi2FitterBT', **kwargs):
    acc = ComponentAccumulator()
    '''
    Global Chi2 Fitter for backtracking
    '''
    kwargs.setdefault("MinPHFCut", 0.)
    ITkGlobalChi2Fitter = acc.popToolsAndMerge(ITkGlobalChi2FitterCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkGlobalChi2Fitter)
    return acc

def ITkTrackFitterBTCfg(flags, name='ITkTrackFitterBT', **kwargs) :
    acc = ComponentAccumulator()

    ITkTrackFitter = None
    if flags.ITk.trackFitterType != 'GlobalChi2Fitter' :
        ITkTrackFitter = acc.popToolsAndMerge(ITkTrackFitterCfg(flags, name, **kwargs))
    else :
        ITkTrackFitter = acc.popToolsAndMerge(ITkGlobalChi2FitterBTCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkTrackFitter)
    return acc

def ITkFKFCfg(flags, name='ITkFKF', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("StateChi2PerNDFPreCut", 30.0)
    PublicFKF = CompFactory.Trk.ForwardKalmanFitter(name = name, **kwargs)
    acc.setPrivateTools(PublicFKF)
    return acc

def ITkBKSCfg(flags, name='ITkBKS', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InitialCovarianceSeedFactor", 200.)
    PublicBKS = CompFactory.Trk.KalmanSmoother(name = name, **kwargs)
    acc.setPrivateTools(PublicBKS)
    return acc

def ITkKOLCfg(flags, name = 'ITkKOL', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackChi2PerNDFCut", 17.0)
    kwargs.setdefault("StateChi2PerNDFCut", 12.5)
    PublicKOL = CompFactory.Trk.KalmanOutlierLogic(name = name, **kwargs)
    acc.setPrivateTools(PublicKOL)
    return acc

#############################################################################################
# BackTracking
#############################################################################################
def ITkRotCreatorDigitalCfg(flags, name='ITkRotCreatorDigital', **kwargs) :
    acc = ComponentAccumulator()
    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

        ToolPixelCluster = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolDigitalCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)

    ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, name=name, **kwargs))
    acc.addPublicTool(ITkRotCreator, primary=True)
    return acc

def ITkTrackSummaryToolNoHoleSearchCfg(flags, name='ITkTrackSummaryToolNoHoleSearch',**kwargs) :
    kwargs.setdefault('doHolesInDet', False)
    return ITkTrackSummaryToolCfg(flags, name=name, **kwargs)

def ITkROIInfoVecCondAlgCfg(flags, name='ITkROIInfoVecCondAlg', **kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("InputEmClusterContainerName", 'ITkCaloClusterROIs')
    kwargs.setdefault("WriteKey", kwargs.get("namePrefix","") +"ROIInfoVec"+ kwargs.get("nameSuffix","") )
    kwargs.setdefault("minPtEM", 5000.0) #in MeV
    acc.setPrivateTools(CompFactory.ROIInfoVecAlg(name = name,**kwargs))
    return acc

def ITkAmbiScoringToolBaseCfg(flags, name='ITkAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags))

    from AthenaCommon.DetFlags  import DetFlags
    have_calo_rois = flags.ITk.doBremRecovery and flags.ITk.doCaloSeededBrem and DetFlags.detdescr.Calo_allOn()
    if have_calo_rois :
        alg = acc.popToolsAndMerge(ITkROIInfoVecCondAlgCfg(flags))
        kwargs.setdefault("CaloROIInfoName", alg.WriteKey )
    kwargs.setdefault("SummaryTool", ITkTrackSummaryTool )
    kwargs.setdefault("DriftCircleCutTool", None )
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxEta", flags.ITk.Tracking.maxEta )
    kwargs.setdefault("usePixel", flags.ITk.Tracking.usePixel )
    kwargs.setdefault("useSCT", flags.ITk.Tracking.useSCT )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )
    kwargs.setdefault("useITkAmbigFcn", True )
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )

    if 'InDetEtaDependentCutsSvc' not in kwargs :
        acc.merge(ITkEtaDependentCutsSvcCfg(flags))
        kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.extension))

    the_name = name + flags.ITk.Tracking.extension
    acc.setPrivateTools(CompFactory.InDet.InDetAmbiScoringTool(name = the_name, **kwargs))
    return acc

def ITkCosmicsScoringToolBaseCfg(flags, name='ITkCosmicsScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    the_name=makeName(name, kwargs)

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags))

    kwargs.setdefault("nWeightedClustersMin", flags.ITk.Tracking.nWeightedClustersMin )
    kwargs.setdefault("minTRTHits", 0 )
    kwargs.setdefault("SummaryTool", ITkTrackSummaryTool )

    acc.setPrivateTools( CompFactory.InDet.InDetCosmicScoringTool(name = the_name, **kwargs ) )
    return acc

def ITkCosmicExtenScoringToolCfg(flags, name='ITkCosmicExtenScoringTool',**kwargs) :
    kwargs.setdefault("nWeightedClustersMin", 0)
    kwargs.setdefault("minTRTHits", 0 )
    return ITkCosmicsScoringToolBaseCfg(flags, name = 'ITkCosmicExtenScoringTool', **kwargs)

def ITkAmbiScoringToolCfg(flags, name='ITkAmbiScoringTool', **kwargs) :
    return ITkAmbiScoringToolBaseCfg(flags, name = name + flags.InDet.Tracking.extension, **kwargs)


#############################################################################################
#TrackingSiPatternConfig
#############################################################################################

def ITkPRDtoTrackMapToolCfg(flags, name='ITkPRDtoTrackMapTool',**kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    acc.setPrivateTools(CompFactory.Trk.PRDtoTrackMapTool( name=the_name, **kwargs))
    return acc

def ITkNNScoringToolBaseCfg(flags, name='ITkNNScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    the_name=makeName(name,kwargs)

    from AthenaCommon.DetFlags  import DetFlags
    have_calo_rois = flags.ITk.doBremRecovery and flags.ITk.doCaloSeededBrem and DetFlags.detdescr.Calo_allOn()
    if have_calo_rois :
        alg = acc.popToolsAndMerge(ITkROIInfoVecCondAlgCfg(flags))
        kwargs.setdefault("CaloROIInfoName", alg.WriteKey )

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    Extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags))

    kwargs.setdefault("nnCutConfig", "dev/TrackingCP/LRTAmbiNetwork/20200727_225401/nn-config.json" )
    kwargs.setdefault("nnCutThreshold", flags.ITk.nnCutLargeD0Threshold )
    kwargs.setdefault("Extrapolator", Extrapolator )
    kwargs.setdefault("SummaryTool", ITkTrackSummaryTool )
    kwargs.setdefault("DriftCircleCutTool", None )
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxZImp", flags.ITk.Tracking.maxZImpact )
    kwargs.setdefault("maxEta", flags.ITk.Tracking.maxEta )
    kwargs.setdefault("usePixel", flags.ITk.Tracking.usePixel )
    kwargs.setdefault("useSCT", flags.ITk.Tracking.useSCT )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )

    acc.setPrivateTools(CompFactory.InDet.InDetNNScoringTool(name = the_name, **kwargs ))
    return acc

def ITkNNScoringToolCfg(flags, name='ITkNNScoringTool', **kwargs) :
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )
    kwargs.setdefault("minPt", flags.ITk.Tracking.minPT )
    kwargs.setdefault("maxRPhiImp", flags.ITk.Tracking.maxPrimaryImpact )
    kwargs.setdefault("minSiClusters", flags.ITk.Tracking.minClusters )
    kwargs.setdefault("minPixel", flags.ITk.Tracking.minPixel )
    kwargs.setdefault("maxSiHoles", flags.ITk.Tracking.maxHoles )
    kwargs.setdefault("maxPixelHoles", flags.ITk.Tracking.maxPixelHoles )
    kwargs.setdefault("maxSCTHoles", flags.ITk.Tracking.maxSctHoles )
    kwargs.setdefault("maxDoubleHoles", flags.ITk.Tracking.maxDoubleHoles)

    return ITkNNScoringToolBaseCfg(flags, name=name+flags.ITk.Tracking.extension, **kwargs )

def ITkCosmicsScoringToolCfg(flags, name='ITkCosmicsScoringTool', **kwargs) :
    return ITkCosmicsScoringToolBaseCfg(flags,
                                        name=name+flags.ITk.Tracking.extension)
