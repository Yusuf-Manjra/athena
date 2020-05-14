#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from __future__ import print_function, absolute_import

from AthenaCommon.Include import include
include.block("InDetTrigRecExample/EFInDetConfig.py")

from AthenaCommon.Logging import logging
log = logging.getLogger("InDetSetup")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags # noqa: F401

if 'InDetTrigFlags' not in dir():
   # --- setup flags with default values
   from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
   InDetTrigFlags.doNewTracking.set_Value_and_Lock(True)
   InDetTrigFlags.primaryVertexSetup = "IterativeFinding"
   InDetTrigFlags.doRefit = True    # switched on for ATR-12226 (z0 uncertainties in bjets)
   InDetTrigFlags.doPixelClusterSplitting = False
   InDetTrigFlags.doPrintConfigurables = False



def makeInDetAlgsNoView( whichSignature='', separateTrackParticleCreator='', rois = 'EMViewRoIs', doFTF = True ):

  viewAlgs, viewVerify = makeInDetAlgs( whichSignature, separateTrackParticleCreator, rois, doFTF, None )
  return viewAlgs

def makeInDetAlgs( whichSignature='', separateTrackParticleCreator='', rois = 'EMViewRoIs', doFTF = True, viewVerifier='IDViewDataVerifier' ):
  #If signature specified add suffix to the algorithms
  signature =  "_" + whichSignature if whichSignature else ''
  if signature != "" and separateTrackParticleCreator == "":
    separateTrackParticleCreator = signature

  #Global keys/names for Trigger collections 
  from .InDetTrigCollectionKeys import  TrigPixelKeys, TrigSCTKeys
  from InDetRecExample.InDetKeys import InDetKeys
  from TrigInDetConfig.TrigInDetConfig import InDetCacheNames
  from AthenaCommon.GlobalFlags import globalflags

  viewAlgs = []

  ViewDataVerifier = None
  if viewVerifier:
    import AthenaCommon.CfgMgr as CfgMgr
    ViewDataVerifier = CfgMgr.AthViews__ViewDataVerifier( viewVerifier + signature )
    ViewDataVerifier.DataObjects = [( 'InDet::PixelClusterContainerCache' , InDetCacheNames.Pixel_ClusterKey ),
                                    ( 'PixelRDO_Cache' , InDetCacheNames.PixRDOCacheKey ),
                                    ( 'InDet::SCT_ClusterContainerCache' , InDetCacheNames.SCT_ClusterKey ),
                                    ( 'SCT_RDO_Cache' , InDetCacheNames.SCTRDOCacheKey ),
                                    ( 'SpacePointCache' , InDetCacheNames.SpacePointCachePix ),
                                    ( 'SpacePointCache' , InDetCacheNames.SpacePointCacheSCT ),
                                    ( 'IDCInDetBSErrContainer_Cache' , InDetCacheNames.SCTBSErrCacheKey ),
                                    ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
    viewAlgs.append( ViewDataVerifier )

    # Make sure required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]

    # Load RDOs if we aren't loading bytestream
    if not globalflags.InputFormat.is_bytestream():
      ViewDataVerifier.DataObjects +=   [( 'PixelRDO_Container' , InDetKeys.PixelRDOs() ),
                                         ( 'SCT_RDO_Container' , InDetKeys.SCT_RDOs() ),
                                         ( 'IDCInDetBSErrContainer' , InDetKeys.SCT_ByteStreamErrs() )]
      topSequence.SGInputLoader.Load += [( 'PixelRDO_Container' , InDetKeys.PixelRDOs() ),
                                         ( 'SCT_RDO_Container' , InDetKeys.SCT_RDOs() ),
                                         ( 'IDCInDetBSErrContainer' , InDetKeys.SCT_ByteStreamErrs() )]

    # This object must be loaded from SG if it's not loaded in conddb (algs request it but ignore)
    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested( "Cond/StatusHT" ):
      ViewDataVerifier.DataObjects += [( 'TRTCond::StrawStatusMultChanContainer' , 'ConditionStore+/TRT/Cond/StatusHT' )]
      topSequence.SGInputLoader.Load += [( 'TRTCond::StrawStatusMultChanContainer' , 'ConditionStore+/TRT/Cond/StatusHT' )]

  from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
  from AthenaCommon.AppMgr import ToolSvc

  #Only add raw data decoders if we're running over raw data
  if globalflags.InputFormat.is_bytestream():
    #Pixel

    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRodDecoder
    InDetPixelRodDecoder = PixelRodDecoder(name = "InDetPixelRodDecoder" + signature)
    ToolSvc += InDetPixelRodDecoder

    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProviderTool
    InDetPixelRawDataProviderTool = PixelRawDataProviderTool(name    = "InDetPixelRawDataProviderTool" + signature,
                                                             Decoder = InDetPixelRodDecoder,
                                                             checkLVL1ID = False)
    ToolSvc += InDetPixelRawDataProviderTool

    if (InDetTrigFlags.doPrintConfigurables()):
      print(InDetPixelRawDataProviderTool) # noqa: ATL901

    # load the PixelRawDataProvider
    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProvider
    InDetPixelRawDataProvider = PixelRawDataProvider(name         = "InDetPixelRawDataProvider"+ signature,
                                                     RDOKey       = InDetKeys.PixelRDOs(),
                                                     ProviderTool = InDetPixelRawDataProviderTool,)
    InDetPixelRawDataProvider.isRoI_Seeded = True
    InDetPixelRawDataProvider.RoIs = rois
    InDetPixelRawDataProvider.RDOCacheKey = InDetCacheNames.PixRDOCacheKey

    viewAlgs.append(InDetPixelRawDataProvider)


    if (InDetTrigFlags.doPrintConfigurables()):
      print(InDetPixelRawDataProvider) # noqa: ATL901


    #SCT
    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCT_RodDecoder
    InDetSCTRodDecoder = SCT_RodDecoder(name          = "InDetSCTRodDecoder" + signature)
    ToolSvc += InDetSCTRodDecoder

    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProviderTool
    InDetSCTRawDataProviderTool = SCTRawDataProviderTool(name    = "InDetSCTRawDataProviderTool" + signature,
                                                         Decoder = InDetSCTRodDecoder)
    ToolSvc += InDetSCTRawDataProviderTool
    if (InDetTrigFlags.doPrintConfigurables()):
      print(InDetSCTRawDataProviderTool) # noqa: ATL901


    # load the SCTRawDataProvider
    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProvider
    InDetSCTRawDataProvider = SCTRawDataProvider(name         = "InDetSCTRawDataProvider" + signature,
                                                 RDOKey       = InDetKeys.SCT_RDOs(),
                                                 ProviderTool = InDetSCTRawDataProviderTool)
    InDetSCTRawDataProvider.isRoI_Seeded = True
    InDetSCTRawDataProvider.RoIs = rois
    InDetSCTRawDataProvider.RDOCacheKey = InDetCacheNames.SCTRDOCacheKey
    InDetSCTRawDataProvider.BSErrCacheKey = InDetCacheNames.SCTBSErrCacheKey

    viewAlgs.append(InDetSCTRawDataProvider)

    # load the SCTEventFlagWriter
    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTEventFlagWriter
    InDetSCTEventFlagWriter = SCTEventFlagWriter(name = "InDetSCTEventFlagWriter"+ signature)

    viewAlgs.append(InDetSCTEventFlagWriter)



  #Pixel clusterisation
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import TrigPixelLorentzAngleTool, TrigSCTLorentzAngleTool

  from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
  PixelConfigCondAlg.UseCalibConditions = False

  from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
  InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool" + signature,
                                                  SCTLorentzAngleTool = TrigSCTLorentzAngleTool,
                                                  PixelLorentzAngleTool = TrigPixelLorentzAngleTool)

  ToolSvc += InDetClusterMakerTool


  from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
  InDetMergedPixelsTool = InDet__MergedPixelsTool(name                    = "InDetMergedPixelsTool" + signature,
                                                  globalPosAlg            = InDetClusterMakerTool,
                                                  MinimalSplitSize        = 0,
                                                  MaximalSplitSize        = 49,
                                                  MinimalSplitProbability = 0,
                                                  DoIBLSplitting = True,
  )
  ToolSvc += InDetMergedPixelsTool

  from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
  InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder" + signature)
  ToolSvc += InDetPixelGangedAmbiguitiesFinder

  from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
  InDetPixelClusterization = InDet__PixelClusterization(name                    = "InDetPixelClusterization" + signature,
                                                        clusteringTool          = InDetMergedPixelsTool,
                                                        gangedAmbiguitiesFinder = InDetPixelGangedAmbiguitiesFinder,
                                                        DataObjectName          = InDetKeys.PixelRDOs(),
                                                        AmbiguitiesMap          = TrigPixelKeys.PixelClusterAmbiguitiesMap,
                                                        ClustersName            = TrigPixelKeys.Clusters)

  InDetPixelClusterization.isRoI_Seeded = True
  InDetPixelClusterization.RoIs = rois
  InDetPixelClusterization.ClusterContainerCacheKey = InDetCacheNames.Pixel_ClusterKey

  viewAlgs.append(InDetPixelClusterization)

  # Create SCT_ConditionsSummaryTool
  from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
  sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryTool" + signature)
  sct_ConditionsSummaryToolSetup.setup()
  InDetSCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool() # noqa: F841
  sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryToolWithoutFlagged" + signature)
  sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
  InDetSCT_ConditionsSummaryToolWithoutFlagged = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()

  # Add conditions tools to SCT_ConditionsSummaryTool
  from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
  sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
  sct_ConfigurationConditionsToolSetup.setToolName("InDetSCT_ConfigurationConditionsTool" + signature)
  sct_ConfigurationConditionsToolSetup.setup()
  InDetSCT_ConditionsSummaryToolWithoutFlagged.ConditionsTools.append(sct_ConfigurationConditionsToolSetup.getTool().getFullName())

  from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
  sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
  sct_ReadCalibDataToolSetup.setToolName("InDetSCT_ReadCalibDataTool" + signature)
  sct_ReadCalibDataToolSetup.setup()
  InDetSCT_ConditionsSummaryToolWithoutFlagged.ConditionsTools.append(sct_ReadCalibDataToolSetup.getTool().getFullName())

  from SCT_ConditionsTools.SCT_ByteStreamErrorsToolSetup import SCT_ByteStreamErrorsToolSetup
  sct_ByteStreamErrorsToolSetup = SCT_ByteStreamErrorsToolSetup()
  sct_ByteStreamErrorsToolSetup.setToolName("InDetSCT_BSErrorTool" + signature)
  sct_ByteStreamErrorsToolSetup.setConfigTool(sct_ConfigurationConditionsToolSetup.getTool())
  sct_ByteStreamErrorsToolSetup.setup()
  InDetSCT_ConditionsSummaryToolWithoutFlagged.ConditionsTools.append(sct_ByteStreamErrorsToolSetup.getTool().getFullName())     

  if (InDetTrigFlags.doPrintConfigurables()):
     print (InDetSCT_ConditionsSummaryToolWithoutFlagged)

  #
  # --- SCT_ClusteringTool
  #
  from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
  InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool" + signature,
                                                      globalPosAlg      = InDetClusterMakerTool,
                                                      conditionsTool    = InDetSCT_ConditionsSummaryToolWithoutFlagged)
  if InDetTrigFlags.doSCTIntimeHits():
     if InDetTrigFlags.InDet25nsec():
        InDetSCT_ClusteringTool.timeBins = "01X"
     else:
        InDetSCT_ClusteringTool.timeBins = "X1X"

  #
  # --- SCT_Clusterization algorithm
  #

  from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
  InDetSCT_Clusterization = InDet__SCT_Clusterization(name                    = "InDetSCT_Clusterization" + signature,
                                                      clusteringTool          = InDetSCT_ClusteringTool,
                                                      # ChannelStatus         = InDetSCT_ChannelStatusAlg,
                                                      DataObjectName          = InDetKeys.SCT_RDOs(),
                                                      ClustersName            = TrigSCTKeys.Clusters,
                                                      #Adding the suffix to flagged conditions
                                                      SCT_FlaggedCondData     = "SCT_FlaggedCondData_TRIG",
                                                      conditionsTool          = InDetSCT_ConditionsSummaryToolWithoutFlagged)
  InDetSCT_Clusterization.isRoI_Seeded = True
  InDetSCT_Clusterization.RoIs = rois
  InDetSCT_Clusterization.ClusterContainerCacheKey = InDetCacheNames.SCT_ClusterKey


  viewAlgs.append(InDetSCT_Clusterization)


  #Space points and FTF

  from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
  InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool" + signature)

  from AthenaCommon.DetFlags import DetFlags
  from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
  from SiSpacePointFormation.InDetOnlineMonitor import InDetMonitoringTool
  InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder" + signature,
                                                                    SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                    PixelsClustersName     = TrigPixelKeys.Clusters,
                                                                    SpacePointsPixelName   = TrigPixelKeys.SpacePoints,
                                                                    SCT_ClustersName	    = TrigSCTKeys.Clusters, 
                                                                    SpacePointsSCTName     = TrigSCTKeys.SpacePoints,
                                                                    SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                    ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                    ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                    ProcessOverlaps        = DetFlags.haveRIO.SCT_on(),
                                                                    SpacePointCacheSCT     = InDetCacheNames.SpacePointCacheSCT,
                                                                    SpacePointCachePix     = InDetCacheNames.SpacePointCachePix,
                                                                    monTool                = InDetMonitoringTool())

  viewAlgs.append(InDetSiTrackerSpacePointFinder)

  # Condition algorithm for SiTrackerSpacePointFinder
  if InDetSiTrackerSpacePointFinder.ProcessSCTs:
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "InDetSiElementPropertiesTableCondAlg"):
      # Setup alignment folders and conditions algorithms
      from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiElementPropertiesTableCondAlg
      condSeq += InDet__SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg")


  #FIXME have a flag for now set for True( as most cases call FTF) but potentially separate
  if doFTF: 
      from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinderBase
      theFTF = TrigFastTrackFinderBase("TrigFastTrackFinder_" + whichSignature, whichSignature)
      theFTF.RoIs = rois
      theFTF.TracksName = "TrigFastTrackFinder_Tracks" + separateTrackParticleCreator
      
      #the following doCloneRemoval modification should be set up in the InDetTrigSliceSettings once legacy trigger not needed
      if whichSignature=="Electron":
         theFTF.doCloneRemoval = True

      viewAlgs.append(theFTF)


      from TrigInDetConf.TrigInDetPostTools import  InDetTrigParticleCreatorToolFTF
      from TrigEDMConfig.TriggerEDMRun3 import recordable
      from InDetTrigParticleCreation.InDetTrigParticleCreationConf import InDet__TrigTrackingxAODCnvMT

      trackCollection = "HLT_IDTrack" + separateTrackParticleCreator + "_FTF"

      theTrackParticleCreatorAlg = InDet__TrigTrackingxAODCnvMT(name = "InDetTrigTrackParticleCreatorAlg" + whichSignature,
                                                                doIBLresidual = False,
                                                                TrackName = "TrigFastTrackFinder_Tracks" + separateTrackParticleCreator,
                                                                TrackParticlesName = recordable( trackCollection ),
                                                                ParticleCreatorTool = InDetTrigParticleCreatorToolFTF)

      theTrackParticleCreatorAlg.roiCollectionName = rois
      viewAlgs.append(theTrackParticleCreatorAlg)


  return viewAlgs, ViewDataVerifier

