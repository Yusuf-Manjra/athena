#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#           Setup of precision tracking

from __future__ import print_function

from AthenaCommon.Logging import logging 
log = logging.getLogger("InDetPrecisionTracking")




# FIXME: eventually the rois should go into the slice setings
def makeInDetPrecisionTracking( config=None, verifier=False, rois='EMViewRoIs', prefix="InDetTrigMT" ) :      
    
    log.info( "makeInDetPRecisionTracking:: {} {} {} ".format(  config.input_name, config.name, config.doTRT ) )
    
    ptAlgs = [] # List containing all the precision tracking algorithms hence every new added alg has to be appended to the list
    
    # Expects configuration  
    if config is None:
        raise ValueError('PrecisionTracking No configuration provided!')
        
    doTRT = config.doTRT

    doTRT = True
        
    # Add suffix to the algorithms
    signature =  "_{}".format( config.input_name )
    
    # Name settings for output Tracks/TrackParticles
    outTrkTracks        = config.trkTracks_IDTrig() # Final output Track collection
    outTrackParticles   = config.tracks_IDTrig() # Final output xAOD::TrackParticle collection
    ambiTrackCollection = config.trkTracks_IDTrig()+"_Amb"  # Ambiguity solver tracks
    
    if not doTRT:
        ambiTrackCollection = outTrkTracks

    #  Verifying input data for the algorithms
  
    # If run in views need to check data dependancies!
    # NOTE: this seems necessary only when PT is called from a different view than FTF otherwise causes stalls
    if verifier:
        from .InDetTrigCollectionKeys import TrigPixelKeys
        verifier.DataObjects += [( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+' + TrigPixelKeys.PixelClusterAmbiguitiesMap ),
                                 ( 'TrackCollection' , 'StoreGateSvc+' + config.trkTracks_FTF() )]
        
    summaryTool     = trackSummaryTool_builder( signature, config, prefix )

    ambiSolvingAlgs = ambiguitySolver_builder( signature, config, summaryTool, outputTrackName=ambiTrackCollection, prefix=prefix )

    #Loading the alg to the sequence
    ptAlgs.extend( ambiSolvingAlgs )
    
    if doTRT:
        # do the TRT extension if requested
        trtAlgs = trtExtension_builder( signature, config, rois, summaryTool, inputTracks=ambiTrackCollection, outputTracks=outTrkTracks, prefix=prefix ) 
        ptAlgs.extend( trtAlgs )
  

    #
    #  Track particle conversion algorithm
    #
    from .InDetTrigCommon import trackParticleCnv_builder

    trackParticleCnvAlg = trackParticleCnv_builder( name                 = prefix+'xAODParticleCreatorAlg'+config.input_name+'_IDTrig',
                                                    config               = config,
                                                    inTrackCollectionKey = outTrkTracks,
                                                    outTrackParticlesKey = outTrackParticles )
    
    log.debug(trackParticleCnvAlg)
    ptAlgs.append(trackParticleCnvAlg)
    
    # ToolSvc.InDetTrigHoleSearchTool.SctSummaryTool.InDetTrigInDetSCT_FlaggedConditionTool.SCT_FlaggedCondData = "SCT_FlaggedCondData_TRIG"
    
    # Potentialy other algs with more collections? 
    # Might Drop the list in the end and keep just one output key
    nameTrackCollections =[ outTrkTracks ]
    nameTrackParticles =  [ outTrackParticles ]
    
    # Return list of Track keys, TrackParticle keys, and PT algs
    return  nameTrackCollections, nameTrackParticles, ptAlgs






def trackSummaryTool_builder( signature, config, prefix="InDetTrigMT" ) : 
  
    doTRT = config.doTRT

    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool

    if doTRT:

        from AthenaCommon.AppMgr import ToolSvc
        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigTrackSummaryHelperToolSharedHits
    
        trigTrackSummaryTool = Trk__TrackSummaryTool(name = "%sTrackSummaryToolSharedHitsWithTRT%s"%( prefix, signature ),
                                                     InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolSharedHits,
                                                     doSharedHits           = True,
                                                     doHolesInDet           = True )
        
        if "electron" in config.name  or "tau" in config.name:
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigTRT_ElectronPidTool
            trigTrackSummaryTool.TRT_ElectronPidTool = InDetTrigTRT_ElectronPidTool
            
        summaryTool = trigTrackSummaryTool
        ToolSvc += summaryTool
            
    else:
        summaryTool = InDetTrigTrackSummaryTool

    return summaryTool




def ambiguitySolver_builder( signature, config, summaryTool, outputTrackName=None, prefix="InDetTrigMT" ) :
    
    #-----------------------------------------------------------------------------
    #                        Ambiguity solving stage
    from .InDetTrigCommon import ambiguityScoreAlg_builder
    from .InDetTrigCommon import ambiguitySolverAlg_builder
    
    # Map of tracks and their scores
    scoreAlg = ambiguityScoreAlg_builder( name                  = prefix+'TrkAmbiguityScore'+config.input_name,
                                          config                = config,
                                          inputTrackCollection  = config.trkTracks_FTF(),
                                          outputTrackScoreMap   = 'ScoreMap'+config.input_name ) 
    
  
    solverAlg = ambiguitySolverAlg_builder( name                  = prefix+'TrkAmbiguitySolver'+config.input_name,
                                            config                = config,
                                            summaryTool           = summaryTool,
                                            inputTrackScoreMap    = 'ScoreMap'+config.input_name,
                                            outputTrackCollection = outputTrackName )
    
    return [ scoreAlg, solverAlg ] 




def trtExtension_builder( signature, config, rois, summaryTool, inputTracks, outputTracks, prefix="InDetTrigMT" ): 

    trtRIOMaker           = trtRIOMaker_builder( signature, config, rois, prefix  )
    trtExtensionAlg       = trtExtensionAlg_builder( signature, config, inputTracks, prefix )
    trtExtensionProcessor = trtExtensionProcessor_builder( signature, config, summaryTool, inputTracks, outputTracks, prefix )

    return [ trtRIOMaker, trtExtensionAlg, trtExtensionProcessor]
    




def trtRIOMaker_builder( signature, config, rois, prefix="InDetTrigMT" ): 
    
    log.info( "trtRIOMaker_builder: {} {} {} {}".format(  signature, config.input_name, config.name, prefix ) )

    algs = []

    TRT_RDO_Key = "TRT_RDOs"

    from .InDetTrigCollectionKeys import TrigTRTKeys
    from AthenaCommon.GlobalFlags import globalflags
    
    # Only add raw data decoders if we're running over raw data
    if globalflags.InputFormat.is_bytestream():
        #Global keys/names for collections 
        TRT_RDO_Key = TrigTRTKeys.RDOs
        trtDataProvider = trtDataProvider_builder( signature, config, TRT_RDO_Key, rois )
        algs.append( trtDataProvider )
        
        
    #-----------------------------------------------------------------------------
    #                        TRT extension
    # Keep track that this needs to have a switch between DAF and XK
    # trkExtensionType = 'XK'
    # if InDetTrigFlags.trtExtensionType() is 'DAF' :
    
    from InDetTrigRecExample.InDetTrigCommonTools import  InDetTrigTRT_DriftCircleTool
    
    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__TRT_RIO_Maker
    trtRIOMaker = InDet__TRT_RIO_Maker( name = "%sTRTDriftCircleMaker%s"%(prefix, signature),
                                        TRTRIOLocation = TrigTRTKeys.DriftCircles,
                                        TRTRDOLocation = TRT_RDO_Key,
                                        # FIXME:
                                        # EtaHalfWidth = config.etaHalfWidth
                                        # PhiHalfWidth = config.phiHalfWidth
                                        # doFullScan   = config.doFullScan
                                        TRT_DriftCircleTool = InDetTrigTRT_DriftCircleTool )
    
    trtRIOMaker.isRoI_Seeded = True
    trtRIOMaker.RoIs = rois

    from RegionSelector.RegSelToolConfig import makeRegSelTool_TRT
    trtRIOMaker.RegSelTool = makeRegSelTool_TRT()

    algs.append( trtRIOMaker )

    return algs
  
 




def trtDataProvider_builder( signature, config, TRT_RDO_Key, rois, prefix="InDetTrigMT" ) :
  
    from AthenaCommon.AppMgr import ToolSvc

    from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRT_RodDecoder
    trtRodDecoder = TRT_RodDecoder( name="%sTRTRodDecoder%s"%(prefix, signature),
                                       LoadCompressTableDB=True )
    ToolSvc += trtRodDecoder
  
    from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProviderTool
    trtRawDataProviderTool = TRTRawDataProviderTool( name="%sTRTRawDataProviderTool%s"%(prefix, signature),
                                                     Decoder=trtRodDecoder )
    
    ToolSvc += trtRawDataProviderTool
    
    # load the TRTRawDataProvider
    from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProvider
    trtRawDataProvider = TRTRawDataProvider(name         = "%sTRTRawDataProvider%s"%(prefix, signature),
                                            RDOKey       = TRT_RDO_Key,
                                            ProviderTool = trtRawDataProviderTool)
    
    from RegionSelector.RegSelToolConfig import makeRegSelTool_TRT
    
    trtRawDataProvider.RegSelTool = makeRegSelTool_TRT()
    trtRawDataProvider.isRoI_Seeded = True
    trtRawDataProvider.RoIs = rois
    
    return  trtRawDataProvider 








def trtExtensionTool_builder( signature, config, prefix="InDetTrigMT" ): 

    from AthenaCommon.AppMgr import ToolSvc

    # Condition algorithm for InDet__TRT_DetElementsRoadMaker_xk
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    
    trtRoadAlgName = "%sTRT_DetElementsRoadCondAlg_xk"%(prefix)
    
    if not hasattr( condSeq, trtRoadAlgName ):
        from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadCondAlg_xk
    
        # condSeq += InDet__TRT_DetElementsRoadCondAlg_xk(name = "%sTRT_DetElementsRoadCondAlg_xk%s"%(prefix, signature) )
        condSeq += InDet__TRT_DetElementsRoadCondAlg_xk( name = trtRoadAlgName )


    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPatternPropagator
    
    from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadMaker_xk
    
    trtRoadMaker =  InDet__TRT_DetElementsRoadMaker_xk( name   = '%sTRTRoadMaker%s'%(prefix, signature),
                                                        #DetectorStoreLocation = 'DetectorStore',
                                                        #TRTManagerLocation    = 'TRT',
                                                        MagneticFieldMode     = 'MapSolenoid',
                                                        PropagatorTool        = InDetTrigPatternPropagator )

    ToolSvc += trtRoadMaker



    # TODO implement new configuration of circle cut

    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
    from .InDetTrigCollectionKeys import TrigTRTKeys

    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPatternUpdator 
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
    
    from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
    
    trtExtensionTool = InDet__TRT_TrackExtensionTool_xk ( name = "%sTrackExtensionTool%s"%(prefix,signature),
                                                          MagneticFieldMode     = 'MapSolenoid',      # default
                                                          TRT_ClustersContainer = TrigTRTKeys.DriftCircles, # default
                                                          TrtManagerLocation    = 'TRT',              # default
                                                          PropagatorTool = InDetTrigPatternPropagator,
                                                          UpdatorTool    = InDetTrigPatternUpdator,
                                                          # RIOonTrackToolYesDr = # default for now
                                                          # RIOonTrackToolNoDr  = # default for now
                                                          RoadTool            = trtRoadMaker,
                                                          DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                                          MinNumberDriftCircles = EFIDTrackingCuts.minTRTonTrk(),
                                                          ScaleHitUncertainty   = 2.,
                                                          RoadWidth             = 20.,
                                                          UseParameterization   = EFIDTrackingCuts.useParameterizedTRTCuts() )
               
    ToolSvc += trtExtensionTool

    return trtExtensionTool


 

def trtExtensionAlg_builder( signature, config, inputTracks, prefix="InDetTrigMT" ): 
 
    extensionTool = trtExtensionTool_builder( signature, config ) 
 
    from TRT_TrackExtensionAlg.TRT_TrackExtensionAlgConf import InDet__TRT_TrackExtensionAlg
    extensionAlg = InDet__TRT_TrackExtensionAlg( name = "%sTrackExtensionAlg%s"%(prefix, signature),
                                                    InputTracksLocation    = inputTracks,
                                                    TrackExtensionTool     = extensionTool,
                                                    ExtendedTracksLocation = 'ExtendedTrackMap' )

    return extensionAlg




def trtExtensionProcessor_builder( signature, config, summaryTool, inputTracks, outputTracks, prefix="InDetTrigMT" ):   

    from AthenaCommon.AppMgr import ToolSvc

    #-----------------------------------------------------------------------------
    #                        TRT processor
    
    # TODO: do I need a new fitter for this? Or can I use the same one?
    # TODO In Run2 option for cosmic
    # InDetTrigExtensionFitter = InDetTrigTrackFitter
    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts

    cutValues = EFIDTrackingCuts
    
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigExtrapolator
    
    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut

    scoringTool = InDet__InDetAmbiScoringTool(name               = '%sExtScoringTool%s'%(prefix, signature),
                                              Extrapolator       = InDetTrigExtrapolator,
                                              SummaryTool        = summaryTool,
                                              useAmbigFcn        = True,     # this is NewTracking  
                                              maxRPhiImp         = cutValues.maxPrimaryImpact(),
                                              maxZImp            = cutValues.maxZImpact(),
                                              maxEta             = cutValues.maxEta(),
                                              minSiClusters      = cutValues.minClusters(),
                                              maxSiHoles         = cutValues.maxHoles(),
                                              maxDoubleHoles     = cutValues.maxDoubleHoles(),
                                              usePixel           = cutValues.usePixel(),
                                              useSCT             = cutValues.useSCT(),
                                              doEmCaloSeed       = False,
                                              minTRTonTrk        = cutValues.minTRTonTrk(),
                                              #useSigmaChi2   = False # tuning from Thijs
                                              DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                              minPt              = config.pTmin )
    
    ToolSvc += scoringTool


    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter
    from InDetExtensionProcessor.InDetExtensionProcessorConf import InDet__InDetExtensionProcessor   
    trtExtensionProcessor = InDet__InDetExtensionProcessor (name               = "%sExtensionProcessor%s"%(prefix, signature),
                                                            TrackName          = inputTracks,
                                                            #Cosmics           = InDetFlags.doCosmics(),
                                                            ExtensionMap       = 'ExtendedTrackMap',
                                                            NewTrackName       = outputTracks,
                                                            TrackFitter        = InDetTrigTrackFitter,
                                                            TrackSummaryTool   = summaryTool,
                                                            ScoringTool        = scoringTool, #TODO do I provide the same tool as for ambiguity solver?
                                                            suppressHoleSearch = False )  # does not work properly
                                                            # Check these option after DAF is implemented
                                                            # tryBremFit         = InDetFlags.doBremRecovery(),
                                                            # caloSeededBrem     = InDetFlags.doCaloSeededBrem(),
                                                            # pTminBrem          = NewTrackingCuts.minPTBrem() )
                                                            # RefitPrds          = not (InDetFlags.refitROT() or (InDetFlags.trtExtensionType() is 'DAF')))
    
    return trtExtensionProcessor




