#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#           Setup of offline pattern recognition tracking for ID Trigger
#Heavily inspired by the offline version:
#https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetExample/InDetRecExample/share/ConfiguredNewTrackingSiPattern.py

from __future__ import print_function

from AthenaCommon.Include import include
include.block("InDetTrigRecExample/EFInDetConfig.py")
include("InDetTrigRecExample/InDetTrigRec_jobOptions.py") # this is needed to get InDetTrigFlags

from AthenaCommon.Logging import logging 
log = logging.getLogger("EFIDTracking")


#Create a view verifier for necessary data collections
def get_idtrig_view_verifier(name):
   import AthenaCommon.CfgMgr as CfgMgr
   from AthenaCommon.GlobalFlags import globalflags
   from .InDetTrigCollectionKeys import  TrigPixelKeys, TrigSCTKeys
   from InDetRecExample.InDetKeys import InDetKeys
   from TrigInDetConfig.TrigInDetConfig import InDetCacheNames
   viewDataVerifier = CfgMgr.AthViews__ViewDataVerifier( name )
   viewDataVerifier.DataObjects = [
                                    ( 'SpacePointContainer',           TrigSCTKeys.SpacePoints ),
                                    ( 'SpacePointContainer',           TrigPixelKeys.SpacePoints ),
                                    ( 'SpacePointOverlapCollection',   'StoreGateSvc+OverlapSpacePoints' ),
                                    ( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+TrigPixelClusterAmbiguitiesMap' )
                                  ]

   #FIXME:
   #Having these (clusters) uncommented breaks cosmic when data preparation is right before offline pattern rec
   #Probably it tries to fetch the data before the actual alg producing them runs?
   #Not case in other signatures where data preparation and offline patern recognition are in different views
   if 'cosmics' not in name:
      viewDataVerifier.DataObjects += [
                                       ( 'InDet::SCT_ClusterContainer',   TrigSCTKeys.Clusters ),
                                       ( 'InDet::PixelClusterContainer',  TrigPixelKeys.Clusters ),
                                      ]
   
   #FIXME:
   #Align with the data preparation, are all of them  really needed in the EFID ?
   viewDataVerifier.DataObjects += [ ( 'InDet::PixelClusterContainerCache' , InDetCacheNames.Pixel_ClusterKey ),
                                     ( 'PixelRDO_Cache' , InDetCacheNames.PixRDOCacheKey ),
                                     ( 'InDet::SCT_ClusterContainerCache' , InDetCacheNames.SCT_ClusterKey ),
                                     ( 'SCT_RDO_Cache' , InDetCacheNames.SCTRDOCacheKey ),
                                     ( 'SpacePointCache' , InDetCacheNames.SpacePointCachePix ),
                                     ( 'SpacePointCache' , InDetCacheNames.SpacePointCacheSCT ),
                                     ( 'IDCInDetBSErrContainer_Cache' , InDetCacheNames.PixBSErrCacheKey ),
                                     ( 'IDCInDetBSErrContainer_Cache' , InDetCacheNames.SCTBSErrCacheKey ),
                                     ( 'IDCInDetBSErrContainer_Cache' , InDetCacheNames.SCTFlaggedCondCacheKey ),
                                     ( 'IDCInDetBSErrContainer',        'StoreGateSvc+SCT_FlaggedCondData' ),
                                     ( 'IDCInDetBSErrContainer',        'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                     ( 'IDCInDetBSErrContainer',        'StoreGateSvc+SCT_ByteStreamErrs' ),
                                     ( 'IDCInDetBSErrContainer',        'StoreGateSvc+PixelByteStreamErrs' ),
                                     ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                     ( 'TagInfo' , 'DetectorStore+ProcessingTags' )]
                                   # ]

   
  # Load RDOs if we aren't loading bytestream
   from AthenaCommon.AlgSequence import AlgSequence
   topSequence = AlgSequence()
  
   topSequence.SGInputLoader.Load += [ ( 'TagInfo' , 'DetectorStore+ProcessingTags' ) ]
   
   if not globalflags.InputFormat.is_bytestream():
     viewDataVerifier.DataObjects +=   [( 'PixelRDO_Container' , InDetKeys.PixelRDOs() ),
                                        ( 'SCT_RDO_Container' , InDetKeys.SCT_RDOs() ),
                                        ( 'IDCInDetBSErrContainer' , InDetKeys.PixelByteStreamErrs() ),
                                        ( 'IDCInDetBSErrContainer' , InDetKeys.SCT_ByteStreamErrs() ), 
                                        ( 'IDCInDetBSErrContainer',  'StoreGateSvc+SCT_FlaggedCondData' ),
                                        ]
     topSequence.SGInputLoader.Load += [( 'PixelRDO_Container' , InDetKeys.PixelRDOs() ),
                                         ( 'SCT_RDO_Container' , InDetKeys.SCT_RDOs() ),
                                         ( 'IDCInDetBSErrContainer' , InDetKeys.PixelByteStreamErrs() ),
                                         ( 'IDCInDetBSErrContainer' , InDetKeys.SCT_ByteStreamErrs() )]
   
   return viewDataVerifier


#Temporary fix before we port offline cuts to trigger code, we are using offline configuration
def remapToOffline( name ):
   if name == 'cosmics':
      return 'Cosmic'
   else:
       return name

def makeInDetPatternRecognition( config, verifier = 'IDTrigViewDataVerifier'  ):
      viewAlgs = [] #list of all algs running in this module

      #Load necessary data
      dataVerifier = None
      #FIXME: Should not be necessary
      if verifier:
         dataVerifier = get_idtrig_view_verifier(verifier+config.name)
         viewAlgs.append( dataVerifier )


      #FIXME: For now id setup but eventually port parameters into ConfigSetting in TrigInDetConfig pkg
      from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
      offName = remapToOffline( config.name )
      trackingCuts = ConfiguredNewTrackingCuts( offName ) #FIXME: replace cosmic 
      trackingCuts.__indetflags = InDetTrigFlags

      #TODO: to be taken from config info
      #prefix     = 'InDetTrigMT'
      #suffix     = '_%s'%whichSignature if whichSignature else '' 

      #outEFIDTracks             = "HLT_IDTrkTrack_%s_%s"         %( whichSignature, 'EFID')
      #outEFIDTrackParticles     = "HLT_IDTrack_%s_%s"            %( whichSignature, 'EFID')


      # --- decide if use the association tool
      #FIXME: Make the same decision as offline (based on the tracking cuts)?
      #Are all of these needed?
      #if (len(InputCollections) > 0) and (trackingCuts.mode() == "LowPt" or trackingCuts.mode() == "VeryLowPt" or trackingCuts.mode() == "LargeD0" or trackingCuts.mode() == "LowPtLargeD0" or trackingCuts.mode() == "BeamGas" or trackingCuts.mode() == "ForwardTracks" or trackingCuts.mode() == "ForwardSLHCTracks"  or trackingCuts.mode() == "Disappearing" or trackingCuts.mode() == "VeryForwardSLHCTracks" or trackingCuts.mode() == "SLHCConversionFinding"):
      #usePrdAssociationTool = True
      #else:
      usePrdAssociationTool = False #Keep false for now
      #Do we actually need it?
      if usePrdAssociationTool:
         from .InDetTrigCommon import prdAssociation_builder
         print ('Running SiSPseedTrackFinder!')
         InputCollections = None #Dummy atm
         prdAssociation = prdAssociation_builder( InputCollections )
         viewAlgs.append( prdAssociation )


      #-----------------------------------------------------------------------------
      #                      Track building stage


      #FIXME? use trigger flags?
      # What are the instances when we don't need this?
      #if InDetFlags.doSiSPSeededTrackFinder():
      doSiSPSeededTrackFinder = True #True by default to test this
      if doSiSPSeededTrackFinder:
         print ('Running SiSPseedTrackFinder!')

         #FIXME: do we need this covered by detflag condition?
         #from AthenaCommon.DetFlags import DetFlags 
         # --- Loading Pixel, SCT conditions
         if True:#DetFlags.haveRIO.pixel_on():
            from AthenaCommon.AlgSequence import AthSequencer
            condSeq = AthSequencer("AthCondSeq")
            if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksPixelCondAlg"):
               from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
               condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name     = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                     ReadKey  = "PixelDetectorElementCollection",
                                                                     WriteKey = "PixelDetElementBoundaryLinks_xk",)



         if True:#trackingCuts.useSCT():
            from AthenaCommon.AlgSequence import AthSequencer
            condSeq = AthSequencer("AthCondSeq")
            if not hasattr(condSeq, "InDet__SiDetElementsRoadCondAlg_xk"):
               from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadCondAlg_xk
               condSeq += InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk")

            if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksSCTCondAlg"):
               from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
               condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name     = "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                  ReadKey  = "SCT_DetectorElementCollection",
                                                                  WriteKey = "SCT_DetElementBoundaryLinks_xk")
            #-------------------------------------------------------


         from .InDetTrigCommon import siSPSeededTrackFinder_builder, get_full_name
         siSPSeededTrackFinder = siSPSeededTrackFinder_builder( name                  = get_full_name( 'siSPSeededTrackFinder', config.name ),
                                                                outputTracks          = config.PT.trkTracksPT(),  # config.EFID.trkTracksEFID(), ##outEFIDTracks, 
                                                                trackingCuts          = trackingCuts,
                                                                usePrdAssociationTool = usePrdAssociationTool,
                                                                nameSuffix            = config.name )

         print(siSPSeededTrackFinder)
         viewAlgs.append( siSPSeededTrackFinder )
      #-----------------------------------------------------------------------------
      #                      Track particle conversion algorithm (for pattern rec.)
      #                        atm disabled but might be useful later for debugging
      #
      #from .InDetTrigCommon import trackParticleCnv_builder
      #trackParticleCnvAlg = trackParticleCnv_builder(name                 = get_full_name( 'xAODParticleCreatorAlg',config.name + '_EFID' ), 
      #                                               config               = config,
      #                                               inTrackCollectionKey = config.PT.trkTracksPT(),#config.EFID.trkTracksEFID(),
      #                                               outTrackParticlesKey = config.EFID.tracksEFID( doRecord = config.isRecordable),
      #                                               )

      #-----------------------------------------------------------------------------
      #                      Precision algorithms

      #Verifier should not be necessary when both patt. rec. and PT runs in the same view -> None
      #Also provides particle cnv alg inside
      precisionAlgs = makePrecisionInDetPatternRecognition(config      = config,
                                                           inputTracks = config.PT.trkTracksPT(), #config.EFID.trkTracksEFID(),
                                                           verifier    = None )


      viewAlgs += precisionAlgs


      return  viewAlgs, dataVerifier


#TODO: potentially  unify with makeInDetPrecisionTracking in the InDetPT.py?
#TODO better name?
def makePrecisionInDetPatternRecognition( config, inputTracks,verifier = None ):
   ptAlgs = [] #List containing all the precision tracking algorithms hence every new added alg has to be appended to the list
   
   #-----------------------------------------------------------------------------
   #                        Verifying input data for the algorithms
   if verifier:
     verifier.DataObjects += [ #( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+' + TrigPixelKeys.PixelClusterAmbiguitiesMap ),
                               ( 'TrackCollection' , 'StoreGateSvc+' + inputTracks )]
   
   
   #-----------------------------------------------------------------------------
   #                        Ambiguity solving stage
   from .InDetTrigCommon import ambiguityScoreAlg_builder, ambiguitySolverAlg_builder, get_full_name, get_scoremap_name
   
   ambiguityScoreAlg = ambiguityScoreAlg_builder( name                  = get_full_name(  core = 'TrkAmbiguityScore', suffix  = config.name ),
                                                  config                = config,
                                                  inputTrackCollection  = inputTracks,
                                                  outputTrackScoreMap   = get_scoremap_name( config.name ), #Map of tracks and their scores
                                                 )
   ptAlgs.append( ambiguityScoreAlg )
   
   #FIXME: these alg internally don't expect EFID setting (but FTF), have to take into consideration
   ambiguitySolverAlg = ambiguitySolverAlg_builder( name                  = get_full_name( core = 'TrkAmbiguitySolver', suffix = config.name ),
                                                    config                = config,
                                                    inputTrackScoreMap    = get_scoremap_name( config.name ), #Map of tracks and their scores, 
                                                    outputTrackCollection = config.PT.trkTracksPT()+"_Amb" )  #FIXME: for now keep PT but if TRT added this will ahve to become intermediate collection

   ptAlgs.append( ambiguitySolverAlg )
   
   #-----------------------------------------------------------------------------
   #                      Track particle conversion algorithm
   from .InDetTrigCommon import trackParticleCnv_builder
   trackParticleCnvAlg = trackParticleCnv_builder(name                 = get_full_name( 'xAODParticleCreatorAlg',config.name + '_IDTrig' ), 
                                                  config               = config,
                                                  inTrackCollectionKey = config.PT.trkTracksPT()+"_Amb",
                                                  outTrackParticlesKey = config.PT.tracksPT( doRecord = config.isRecordable),
                                                  )
   
   ptAlgs.append( trackParticleCnvAlg )

   return ptAlgs
