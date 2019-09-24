# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigInDetRecoTools.TrigInDetRecoToolsConf import TrigL2PattRecoStrategyC
from TrigInDetRecoTools.ConfiguredSettings import SettingsForStrategyC
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPixelConditionsSummaryTool

from AthenaCommon.Logging import logging  
log = logging.getLogger("FactoryForStrategyC")

class FactoryForStrategyC() :
    __slots__=[]
    def __init__(self, name = 'FactoryForStrategyC') :
        self.strategy = None
        self.settings = SettingsForStrategyC()
    
    def createStrategy(self, instName) :
        if instName in self.settings.allowedInstanceNames :
             self.strategy = TrigL2PattRecoStrategyC("StrategyC_"+instName)
             from AthenaCommon.DetFlags    import DetFlags
             from AthenaCommon.AppMgr import ToolSvc
             from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
             from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
             InDetTrigCutValues = EFIDTrackingCuts
             from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_Trigger
             SeedMaker_C = InDet__SiSpacePointsSeedMaker_Trigger( name = 'InDetTrigSiSpacePointsSeedMaker_C_'+instName,
                                                                  pTmin = self.settings[('pTmin',instName)],
                                                                  maxdImpact = 5.0,
                                                                  maxZ = InDetTrigCutValues.maxZImpact(),
                                                                  minZ = -InDetTrigCutValues.maxZImpact(),
                                                                  useOverlapSpCollection   = False,
                                                                  #useOverlapSpCollection   = InDetTrigFlags.doOverlapSP(), #testing
                                                                  usePixel = DetFlags.haveRIO.pixel_on(),
                                                                  useSCT   = DetFlags.haveRIO.SCT_on(),
                                                                  SpacePointsSCTName = 'SCT_TempSpacePoints',
                                                                  SpacePointsPixelName = 'PixelTempSpacePoints',
                                                                  radMax = InDetTrigCutValues.radMax(),
                                                                  #maxNumberVertices=5,    #not fully tested
                                                                  )
             ToolSvc += SeedMaker_C
             print SeedMaker_C
             from SiZvertexTool_xk.SiZvertexTool_xkConf import InDet__SiZvertexMaker_xk
             ZvertexMaker_C = InDet__SiZvertexMaker_xk( name = 'InDetTrigZvertexMaker_C_'+instName,
                                                        Zmax = InDetTrigCutValues.maxZImpact(),
                                                        Zmin = -InDetTrigCutValues.maxZImpact(),
                                                        #SeedSize = 3,
                                                        #minContent = 10,
                                                        minRatio = 0.17 # not default
                                                        )
             ZvertexMaker_C.SeedMakerTool = SeedMaker_C
             ToolSvc += ZvertexMaker_C

             print ZvertexMaker_C

             # SCT and Pixel detector elements road builder
             from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPixelConditionsSummaryTool, InDetTrigSCTConditionsSummaryTool
             from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker
             InDetTrigSiDetElementsRoadMaker_C = InDetTrigSiDetElementsRoadMaker.clone('InDetTrigSiDetElementsRoadMaker_C')
             InDetTrigSiDetElementsRoadMaker_C.RoadWidth = self.settings[('RoadWidth',instName)]

             if DetFlags.haveRIO.SCT_on():
                 from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
                 from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool
                 InDetTrigSCTConditionsSummaryTool = SCT_ConditionsSummaryTool(SCT_ConditionsSetup.instanceName("InDetSCT_ConditionsSummaryTool"))
             else:
                 InDetTrigSCTConditionsSummaryTool = None
             from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
             from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool, InDetTrigPatternPropagator
             from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPatternUpdator, InDetTrigRotCreator
             
             TrackFinder_C = InDet__SiCombinatorialTrackFinder_xk(name = 'InDetTrigSiComTrackFinder_C_'+instName,
                                                                  PropagatorTool = InDetTrigPatternPropagator,
                                                                  UpdatorTool    = InDetTrigPatternUpdator,
                                                                  RIOonTrackTool   = InDetTrigRotCreator,
                                                                  usePixel         = DetFlags.haveRIO.pixel_on(),
                                                                  useSCT           = DetFlags.haveRIO.SCT_on(),
                                                                  PixelClusterContainer = 'PixelClusterCache',
                                                                  SCT_ClusterContainer = 'SCT_ClusterCache',
                                                                  PixelSummaryTool = InDetTrigPixelConditionsSummaryTool,
                                                                  SctSummaryTool = InDetTrigSCTConditionsSummaryTool)
             if DetFlags.haveRIO.pixel_on():
                 # Condition algorithm for SiCombinatorialTrackFinder_xk
                 from AthenaCommon.AlgSequence import AthSequencer
                 condSeq = AthSequencer("AthCondSeq")
                 if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksPixelCondAlg"):
                     from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
                     condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                           ReadKey = "PixelDetectorElementCollection",
                                                                           WriteKey = "PixelDetElementBoundaryLinks_xk")
             if DetFlags.haveRIO.SCT_on():
                 # Condition algorithm for SiCombinatorialTrackFinder_xk
                 from AthenaCommon.AlgSequence import AthSequencer
                 condSeq = AthSequencer("AthCondSeq")
                 if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksSCTCondAlg"):
                     from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
                     condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                           ReadKey = "SCT_DetectorElementCollection",
                                                                           WriteKey = "SCT_DetElementBoundaryLinks_xk")
             
             from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk

             TrackMaker_C = InDet__SiTrackMaker_xk( name = 'InDetTrigSiTrackMaker_C_'+instName,
                                                    RoadTool       = InDetTrigSiDetElementsRoadMaker_C,
                                                    CombinatorialTrackFinder = TrackFinder_C,
                                                    pTmin          = self.settings[('pTmin',instName)],
                                                    nClustersMin   = self.settings[('nClustersMin',instName)],
                                                    nHolesMax      = self.settings[('nHolesMax',instName)],
                                                    nHolesGapMax   = self.settings[('nHolesGapMax',instName)],
                                                    SeedsFilterLevel = InDetTrigCutValues.seedFilterLevel(),
                                                    Xi2max         = 15.0,
                                                    Xi2maxNoAdd    = 15.0,
                                                    nWeightedClustersMin= 8,
                                                    UseAssociationTool       = False)
             ToolSvc += TrackMaker_C
             print TrackMaker_C
             self.strategy.SeedsTool = SeedMaker_C
             self.strategy.ZvertexTool = ZvertexMaker_C
             #strategy.useZvertexTool = InDetTrigFlags.useZvertexTool()
             self.strategy.useZvertexTool = self.settings[('useZvertexTool',instName)]
             #self.useZvertexTool = False
             self.strategy.TrackTool = TrackMaker_C
             return self.strategy
        else :
            print "Instance "+instName+" of the Strategy C is not supported !"
            return None


    
