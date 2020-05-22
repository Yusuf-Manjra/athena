# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

##------------------------------------------------------------------------------
def BCM_ZeroSuppressionCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BcmContainerName", "BCM_RDOs")
    algo = CompFactory.BCM_ZeroSuppression("InDetBCM_ZeroSuppression", **kwargs)
    acc.addEventAlgo(algo, primary = True)
    return acc

##------------------------------------------------------------------------------
def PixelClusterizationCfg(flags, **kwargs) :
    acc = ComponentAccumulator()
    sub_acc = MergedPixelsToolCfg(flags, **kwargs)
    merged_pixels_tool = sub_acc.getPrimary()
    acc.merge(sub_acc)
    sub_acc = PixelGangedAmbiguitiesFinderCfg(flags)
    ambi_finder=sub_acc.getPrimary()
    acc.merge(sub_acc)
    acc.addEventAlgo( CompFactory.InDet.PixelClusterization(   name                    = "InDetPixelClusterization",
                                                    clusteringTool          = merged_pixels_tool,
                                                    gangedAmbiguitiesFinder = ambi_finder,
                                                    DataObjectName          = "PixelRDOs",
                                                    ClustersName            = "PixelClusters" ))
    return acc
##------------------------------------------------------------------------------
def PixelClusterizationPUCfg(flags, **kwargs) :
    kwargs.setdefault("name", "InDetPixelClusterizationPU")
    kwargs.setdefault("DataObjectName", "Pixel_PU_RDOs")
    kwargs.setdefault("ClustersName", "PixelPUClusters")
    kwargs.setdefault("AmbiguitiesMap", "PixelClusterAmbiguitiesMapPU")
    return PixelClusterizationCfg(flags, **kwargs)

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def SCTClusterizationCfg(flags, **kwargs) :
    acc = ComponentAccumulator()

    # Need to get SCT_ConditionsSummaryTool for e.g. SCT_ClusteringTool
    from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(InDetSCT_ConditionsSummaryToolCfg(flags,withFlaggedCondTool=False))

    #### Clustering tool ######
    accbuf = ClusterMakerToolCfg(flags)
    InDetClusterMakerTool = accbuf.getPrimary()
    acc.merge(accbuf)
    InDetSCT_ClusteringTool = CompFactory.InDet.SCT_ClusteringTool(    name              = "InDetSCT_ClusteringTool",
                                                            globalPosAlg     = InDetClusterMakerTool,
                                                            conditionsTool = InDetSCT_ConditionsSummaryToolWithoutFlagged)
    if flags.InDet.selectSCTIntimeHits :
       if flags.InDet.InDet25nsec : 
          InDetSCT_ClusteringTool.timeBins = "01X" 
       else: 
          InDetSCT_ClusteringTool.timeBins = "X1X" 

    acc.addEventAlgo( CompFactory.InDet.SCT_Clusterization(    name                    = "InDetSCT_Clusterization",
                                                    clusteringTool          = InDetSCT_ClusteringTool,
                                                    DataObjectName          = 'SCT_RDOs', ##InDetKeys.SCT_RDOs(),
                                                    ClustersName            = 'SCT_Clusters', ##InDetKeys.SCT_Clusters(),
                                                    conditionsTool          = InDetSCT_ConditionsSummaryToolWithoutFlagged  ) )

    return acc


##------------------------------------------------------------------------------
##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def SCTClusterizationPUCfg(flags, **kwargs) :
    kwargs.setdefault("name", "InDetSCT_ClusterizationPU")
    kwargs.setdefault("DataObjectName", "SCT_PU_RDOs" )   #flags.InDetKeys.SCT_PU_RDOs
    kwargs.setdefault("ClustersName", "SCT_PU_Clusters") #flags.InDetKeys.SCT_PU_Clusters
    return SCTClusterizationCfg(flags, **kwargs)

##------------------------------------------------------------------------------
def PixelGangedAmbiguitiesFinderCfg(flags) :
    acc = ComponentAccumulator()
    InDetPixelGangedAmbiguitiesFinder = CompFactory.InDet.PixelGangedAmbiguitiesFinder( name = "InDetPixelGangedAmbiguitiesFinder")
    acc.addPublicTool( InDetPixelGangedAmbiguitiesFinder, primary=True)
    return acc


##------------------------------------------------------------------------------
def MergedPixelsToolCfg(flags, **kwargs) :
      acc = ComponentAccumulator()
      clusterSplitProbTool = None
      clusterSplitterTool  = None
      # FIXME - should we just not set InDet.doPixelClusterSplitting if flags.InDet.doTIDE_Ambi?
      if flags.InDet.doPixelClusterSplitting and not flags.InDet.doTIDE_Ambi: 
         # --- Neutral Network version
         if flags.InDet.pixelClusterSplittingType == 'NeuralNet':
            useBeamConstraint = flags.InDet.useBeamConstraint

            from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool
 
            # --- new NN prob tool
            NnClusterizationFactory = CompFactory.InDet.NnClusterizationFactory(   name                         = "NnClusterizationFactory",
                                                                        PixelLorentzAngleTool        = PixelLorentzAngleTool( flags ),
                                                                        useToT                       = flags.InDet.doNNToTCalibration,
                                                                        NnCollectionReadKey          = "PixelClusterNN",
                                                                        NnCollectionWithTrackReadKey = "PixelClusterNNWithTrack")
            MultiplicityContent = [1 , 1 , 1]
            if flags.InDet.doSLHC:
                PixelClusterSplitProbTool=CompFactory.InDet.TruthPixelClusterSplitProbTool
            else:
                PixelClusterSplitProbTool=CompFactory.InDet.NnPixelClusterSplitProbTool
            NnPixelClusterSplitProbTool=PixelClusterSplitProbTool(name                     = "NnPixelClusterSplitProbTool",
                                                                           PriorMultiplicityContent = MultiplicityContent,
                                                                           NnClusterizationFactory  = NnClusterizationFactory,
                                                                           useBeamSpotInfo          = useBeamConstraint)
            # --- remember this prob tool  
            clusterSplitProbTool = NnPixelClusterSplitProbTool
            # --- new NN splitter
            if flags.InDet.doSLHC :
                PixelClusterSplitter=CompFactory.InDet.TruthPixelClusterSplitter
            else:
                PixelClusterSplitter=CompFactory.InDet.NnPixelClusterSplitter
            NnPixelClusterSplitter=PixelClusterSplitter(    name                                = "NnPixelClusterSplitter",
                                                            NnClusterizationFactory             = NnClusterizationFactory,
                                                            ThresholdSplittingIntoTwoClusters   = 0.5, ##InDet.pixelClusterSplitProb1, ###0.5, # temp.
                                                            ThresholdSplittingIntoThreeClusters = 0.25, ##InDet.pixelClusterSplitProb2, ###0.25, # temp.
                                                            SplitOnlyOnBLayer                   = False,
                                                            useBeamSpotInfo                     = useBeamConstraint)
            # remember splitter tool  
            clusterSplitterTool = NnPixelClusterSplitter
         # --- Neutral Network version ?
         elif flags.InDet.pixelClusterSplittingType == 'AnalogClus':      
            # new splitter tool & remember splitter tool    
            clusterSplitterTool=CompFactory.InDet.TotPixelClusterSplitter (name = "TotPixelClusterSplitter")
      
      if clusterSplitProbTool is not None: kwargs.setdefault("SplitProbTool", clusterSplitProbTool )
      if clusterSplitterTool is not None: kwargs.setdefault("ClusterSplitter", clusterSplitterTool )
      # --- now load the framework for the clustering
      #InDetClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name                 = "InDetClusterMakerTool")
      #acc.addPublicTool(InDetClusterMakerTool)
      accbuf = ClusterMakerToolCfg(flags)
      InDetClusterMakerTool = accbuf.getPrimary()
      kwargs.setdefault("globalPosAlg", InDetClusterMakerTool )
      acc.merge(accbuf)

      # PixelClusteringToolBase uses PixelConditionsSummaryTool
      from InDetConfig.InDetRecToolConfig import PixelConditionsSummaryToolCfg
      accbuf = PixelConditionsSummaryToolCfg(flags)
      conditionssummarytool = accbuf.popPrivateTools()
      kwargs.setdefault("PixelConditionsSummaryTool", conditionssummarytool ) 
      acc.merge(accbuf)

      kwargs.setdefault("MinimalSplitSize", 0 )
      kwargs.setdefault("MaximalSplitSize", 49 )
      kwargs.setdefault("MinimalSplitProbability", 0 )
      kwargs.setdefault("DoIBLSplitting", True )

      InDetMergedPixelsTool = CompFactory.InDet.MergedPixelsTool(  name = "InDetMergedPixelsTool", **kwargs)
     
      acc.addPublicTool(InDetMergedPixelsTool, primary=True)
      return acc
##------------------------------------------------------------------------------
def ClusterMakerToolCfg(flags, **kwargs) :
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelChargeCalibCondAlgCfg, PixelConfigCondAlgCfg

    acc = ComponentAccumulator()
    # This directly needs the following Conditions data:
    # PixelModuleData & PixelChargeCalibCondData
    acc.merge( PixelChargeCalibCondAlgCfg(flags))
    acc.merge( PixelConfigCondAlgCfg(flags))

    InDetClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name = "InDetClusterMakerTool", **kwargs)

    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    SCTLorentzAngleTool = acc.popToolsAndMerge( SCT_LorentzAngleCfg(flags) )    
    acc.addPublicTool(SCTLorentzAngleTool)
    acc.addPublicTool(InDetClusterMakerTool, primary=True)
    return acc

