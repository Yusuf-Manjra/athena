# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TrackingCommon')

def createAndAddCondAlg(creator, the_name, **kwargs) :
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AlgSequence import AthSequencer
    cond_seq=AthSequencer("AthCondSeq")
    for seq in [AlgSequence(),cond_seq] :
        log.debug('createAndAddCondAlg match ?  %s == %s ? %s ', dir(seq), the_name, hasattr(seq,the_name) )
        if hasattr(seq,the_name) :
            if seq.getName() != "AthCondSeq" :
                raise Exception('Algorithm already in a sequnece but not the conditions seqence')
            return
    cond_seq += creator(**kwargs)

def setDefaults(kwargs, **def_kwargs) :
    def_kwargs.update(kwargs)
    return def_kwargs

def copyArgs(kwargs, copy_list) :
    dict_copy={}
    for elm in copy_list :
        if elm in kwargs :
            dict_copy[elm]=kwargs[elm]
    return dict_copy

def splitDefaultPrefix(name) :
    default_prefix=''
    for prefix in ['InDet','InDetTrig'] :
        if name[0:len(prefix)] == prefix :
            name=name[len(prefix):]
            default_prefix=prefix
            break
    return default_prefix,name

def makeName( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix

def makeNameGetPreAndSuffix( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix,namePrefix,nameSuffix

def getDefaultName(func) :
    # @TODO only works for python 2
    import inspect
    defaults=inspect.getargspec(func).defaults
    if len(defaults)==0 :
        raise Exception("Missing default name for %s" % func.__name__)
    return defaults[0]

def makePublicTool(tool_creator) :
    '''
    If the decorated method gets called the created tool will be added to ToolSvc
    '''
    def createPublicTool(*args,**kwargs):
        from AthenaCommon.AppMgr import ToolSvc
        name=kwargs.pop('name',None)
        private=kwargs.pop("private",False)

        if len(args) == 1 :
            if name is not None :
                raise Exception('Name given as positional and keyword argument')
            name=args[0]

        if name is None :
            name=getDefaultName(tool_creator)
        orig_name = name
        default_prefix,name=splitDefaultPrefix(name)
        the_name =  kwargs.get('namePrefix',default_prefix) + name + kwargs.get('nameSuffix','')
        if private is True or the_name not in ToolSvc :
            if len(args) > 1 :
                raise Exception('Too many positional arguments')
            tool = tool_creator(orig_name, **kwargs)
            if tool is None :
                return None
            if the_name != tool.name() :
                raise Exception('Tool has not the exepected name %s but %s' % (the_name, tool.name()))
            if private is False :
                ToolSvc += tool
            return tool
        else :
            return getattr(ToolSvc, the_name)

    createPublicTool.__name__   = tool_creator.__name__
    createPublicTool.__module__ = tool_creator.__module__
    return createPublicTool

def getPixelRIO_OnTrackErrorScalingDbOverrideCondAlg( **kwargs) :
    '''
    usage:
       createAndAddCondAlg( getPixelRIO_OnTrackErrorScalingDbOverrideCondAlg, 'PixelRIO_OnTrackErrorScalingDbOverrideCondAlg' )
    '''
    the_name=kwargs.pop("name",'PixelRIO_OnTrackErrorScalingDbOverrideCondAlg')
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    # kPixBarrelPhi
    params   = [ 10. , 0.0044]
    # kPixBarrelEta
    params  += [ 10. , 0.0312]
    # kPixEndcapPhi,
    params  += [ 10. , 0.026]
    # kPixEndcapEta,
    params  += [ 10. , 0.0]
    if geoFlags.isIBL() :
        error_scaling_type='PixelRIO_OnTrackErrorScaling'
        # kPixIBLPhi
        params  += [ 10. , 0.0044]
        # kPixIBLEta
        params  += [ 10. , 0.0312]
    else :
        error_scaling_type='PixelRIO_OnTrackErrorScalingRun1'


    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import RIO_OnTrackErrorScalingDbOverrideCondAlg
    return RIO_OnTrackErrorScalingDbOverrideCondAlg( name = the_name,
                                                     **setDefaults( kwargs,
                                                                    ErrorScalingTypeName  = error_scaling_type,
                                                                    WriteKey              = "/Indet/TrkErrorScalingPixel",
                                                                    ErorScalingParameters = params,
                                                                    OutputLevel = 1) )  # VERBOSE

def getRIO_OnTrackErrorScalingCondAlg( **kwargs) :
    the_name=kwargs.pop("name",None)
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    if geoFlags.isIBL() :
        error_scaling_type =   ["PixelRIO_OnTrackErrorScaling"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]
    else :
        error_scaling_type =   ["PixelRIO_OnTrackErrorScalingRun1"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]

    error_scaling_type +=      ["SCTRIO_OnTrackErrorScaling"]
    error_scaling_outkey +=    ["/Indet/TrkErrorScalingSCT"]

    error_scaling_type +=      ["TRTRIO_OnTrackErrorScaling"]
    error_scaling_outkey +=    ["/Indet/TrkErrorScalingTRT"]

    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested( "/Indet/TrkErrorScaling" ):
        log.debug('request /Indet/TrkErrorScaling')
        conddb.addFolderSplitOnline('INDET','/Indet/Onl/TrkErrorScaling','/Indet/TrkErrorScaling', className="CondAttrListCollection")
    else :
        log.debug('folder /Indet/TrkErrorScaling already requested.')
    for elm in conddb.iovdbsvc.Folders :
        log.debug('IOVDbSvc folder %s', elm)


    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import RIO_OnTrackErrorScalingCondAlg
    if the_name is None :
        return RIO_OnTrackErrorScalingCondAlg( **setDefaults(kwargs,
                                                             ReadKey             = "/Indet/TrkErrorScaling",
                                                             ErrorScalingType    = error_scaling_type,
                                                             OutKeys             = error_scaling_outkey) )
    else :
        return RIO_OnTrackErrorScalingCondAlg( the_name,
                                               **setDefaults(kwargs,
                                                             ReadKey             = "/Indet/TrkErrorScaling",
                                                             ErrorScalingType    = error_scaling_type,
                                                             OutKeys             = error_scaling_outkey) )




def getEventInfoKey() :
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.DetFlags    import DetFlags

    isData = (globalflags.DataSource == 'data')

    eventInfoKey = "EventInfo"
    if not isData:
        eventInfoKey = "McEventInfo"
    if globalflags.isOverlay() and isData :
        if DetFlags.overlay.pixel_on() or DetFlags.overlay.SCT_on() or DetFlags.overlay.TRT_on():
            from OverlayCommonAlgs.OverlayFlags import overlayFlags
            eventInfoKey = (overlayFlags.dataStore() + '+' + eventInfoKey).replace("StoreGateSvc+","")
        else :
            eventInfoKey = "McEventInfo"
    return eventInfoKey


def getNeuralNetworkToHistoTool(**kwargs) :
    from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
    name = kwargs.pop('name',"NeuralNetworkToHistoTool")
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,name) :
        return getattr(ToolSvc,name)

    NeuralNetworkToHistoTool=Trk__NeuralNetworkToHistoTool(name,
                                                           **kwargs)
    ToolSvc += NeuralNetworkToHistoTool
    return NeuralNetworkToHistoTool

def getPixelClusterNnCondAlg(**kwargs) :
    track_nn = kwargs.pop('TrackNetwork',False)
    nn_names = [
          "NumberParticles_NoTrack/",
          "ImpactPoints1P_NoTrack/",
          "ImpactPoints2P_NoTrack/",
          "ImpactPoints3P_NoTrack/",
          "ImpactPointErrorsX1_NoTrack/",
          "ImpactPointErrorsX2_NoTrack/",
          "ImpactPointErrorsX3_NoTrack/",
          "ImpactPointErrorsY1_NoTrack/",
          "ImpactPointErrorsY2_NoTrack/",
          "ImpactPointErrorsY3_NoTrack/" ]

    if track_nn :
        nn_names = [ elm.replace('_NoTrack', '')  for elm in nn_names ]

    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested('/PIXEL/PixelClustering/PixelClusNNCalib'):
        # COOL binding
        conddb.addFolder("PIXEL_OFL","/PIXEL/PixelClustering/PixelClusNNCalib",className='CondAttrListCollection')

    kwargs=setDefaults(kwargs,
                       NetworkNames = nn_names,
                       WriteKey     ='PixelClusterNN' if not track_nn else 'PixelClusterNNWithTrack')

    if 'NetworkToHistoTool' not in kwargs :
        kwargs = setDefaults( kwargs,
                              NetworkToHistoTool   = getNeuralNetworkToHistoTool() )

    from SiClusterizationTool.SiClusterizationToolConf import InDet__TTrainedNetworkCondAlg
    return InDet__TTrainedNetworkCondAlg(kwargs.pop("name", 'PixelClusterNnCondAlg'), **kwargs)

def getPixelClusterNnWithTrackCondAlg(**kwargs) :

    kwargs = setDefaults( kwargs,
                          TrackNetwork = True,
                          name         ='PixelClusterNnWithTrackCondAlg')
    return getPixelClusterNnCondAlg( **kwargs )

def getPixelLorentzAngleTool(name='PixelLorentzAngleTool', **kwargs) :
    the_name = makeName( name, kwargs)
    if the_name != "PixelLorentzAngleTool" :
        raise Exception('There should be only one Pixel Lorentz angle tool configuration named "PixelLorentzAngleTool"')
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc, the_name ):
        from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
        PixelLorentzAngleToolSetup()
    return getattr(ToolSvc,the_name)

def getSCTLorentzAngleTool(name='SCTLorentzAngleTool', **kwargs) :
    the_name = makeName( name, kwargs)
    if the_name != "SCTLorentzAngleTool" :
        raise Exception('There should be only one SCT Lorentz angle tool configuration named "SCTLorentzAngleTool"')
    from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
    return SCTLorentzAngleToolSetup().SCTLorentzAngleTool

@makePublicTool
def getNnClusterizationFactory(name='NnClusterizationFactory', **kwargs) :
    the_name = makeName( name, kwargs)
    from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory

    if 'PixelLorentzAngleTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelLorentzAngleTool = getPixelLorentzAngleTool())

    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as geoFlags
    do_runI = geoFlags.Run() not in ["RUN2", "RUN3"]
    createAndAddCondAlg( getPixelClusterNnCondAlg,         'PixelClusterNnCondAlg',          GetInputsInfo = do_runI)
    createAndAddCondAlg( getPixelClusterNnWithTrackCondAlg,'PixelClusterNnWithTrackCondAlg', GetInputsInfo = do_runI)

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults( kwargs,
                          doRunI                             = do_runI,
                          useToT                             = False if do_runI else InDetFlags.doNNToTCalibration(),
                          useRecenteringNNWithoutTracks      = True  if do_runI else False,  # default,
                          useRecenteringNNWithTracks         = False if do_runI else False,  # default,
                          correctLorShiftBarrelWithoutTracks = 0,
                          correctLorShiftBarrelWithTracks    = 0.030 if do_runI else 0.000,  # default,
                          NnCollectionReadKey                = 'PixelClusterNN',
                          NnCollectionWithTrackReadKey       = 'PixelClusterNNWithTrack')
    return InDet__NnClusterizationFactory(name=the_name, **kwargs)

@makePublicTool
def getInDetPixelClusterOnTrackToolBase(name, **kwargs) :
    the_name                    = makeName( name, kwargs)
    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    from InDetRecExample.InDetJobProperties import InDetFlags
    from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool

    if InDetFlags.doCosmics() or InDetFlags.doDBMstandalone():
        kwargs = setDefaults(kwargs,
                             ErrorStrategy    = 0,
                             PositionStrategy = 0)

    from InDetRecExample.InDetKeys import InDetKeys
    kwargs = setDefaults(kwargs,
                         DisableDistortions       = (InDetFlags.doFatras() or InDetFlags.doDBMstandalone()),
                         applyNNcorrection        = ( InDetFlags.doPixelClusterSplitting() and
                                                      InDetFlags.pixelClusterSplittingType() == 'NeuralNet' and not InDetFlags.doSLHC()),
                         NNIBLcorrection          = ( InDetFlags.doPixelClusterSplitting() and
                                                      InDetFlags.pixelClusterSplittingType() == 'NeuralNet' and not InDetFlags.doSLHC()),
                         SplitClusterAmbiguityMap = InDetKeys.SplitClusterAmbiguityMap() + split_cluster_map_extension,
                         RunningTIDE_Ambi         = InDetFlags.doTIDE_Ambi())

    return InDet__PixelClusterOnTrackTool(the_name , **kwargs)

def getInDetPixelClusterOnTrackToolNNSplitting(name='InDetPixelClusterOnTrackToolNNSplitting', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.doPixelClusterSplitting() and InDetFlags.pixelClusterSplittingType() == 'NeuralNet':
        if 'NnClusterizationFactory' not in kwargs :
            kwargs = setDefaults(kwargs, NnClusterizationFactory  = getNnClusterizationFactory())

        if InDetFlags.doTIDE_RescalePixelCovariances() :
            kwargs = setDefaults(kwargs, applydRcorrection = True)
    return getInDetPixelClusterOnTrackToolBase(name=name, **kwargs)

def getInDetPixelClusterOnTrackTool(name='InDetPixelClusterOnTrackTool', **kwargs) :
    if 'LorentzAngleTool' not in kwargs :
        kwargs = setDefaults(kwargs, LorentzAngleTool = getPixelLorentzAngleTool())

    return getInDetPixelClusterOnTrackToolNNSplitting(name=name, **kwargs)

def getInDetPixelClusterOnTrackToolPattern(name='InDetPixelClusterOnTrackToolPattern', **kwargs) :
    return getInDetPixelClusterOnTrackToolNNSplitting(name=name, **kwargs)

def getInDetPixelClusterOnTrackToolDigital(name='InDetPixelClusterOnTrackToolDigital', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if 'LorentzAngleTool' not in kwargs :
        kwargs = setDefaults(kwargs, LorentzAngleTool = getPixelLorentzAngleTool())

    if InDetFlags.doDigitalROTCreation():
        kwargs = setDefaults(kwargs,
                             applyNNcorrection = False,
                             NNIBLcorrection   = False,
                             ErrorStrategy     = 2,
                             PositionStrategy  = 1)
    else :
        kwargs = setDefaults(kwargs,
                             SplitClusterAmbiguityMap = "")

    return getInDetPixelClusterOnTrackToolBase(name=name, **kwargs)

def getInDetPixelClusterOnTrackToolDBM(name='InDetPixelClusterOnTrackToolDBM', **kwargs) :
    return getInDetPixelClusterOnTrackToolBase(name=name, **setDefaults(kwargs,
                                                                        DisableDistortions = True,
                                                                        applyNNcorrection  = False,
                                                                        NNIBLcorrection    = False,
                                                                        RunningTIDE_Ambi   = False,
                                                                        ErrorStrategy      = 0,
                                                                        PositionStrategy   = 0))

def getInDetBroadPixelClusterOnTrackTool(name='InDetBroadPixelClusterOnTrackTool', **kwargs) :
    return getInDetPixelClusterOnTrackTool(name=name, **setDefaults(kwargs, ErrorStrategy  = 0))

@makePublicTool
def getInDetSCT_ClusterOnTrackTool(name='InDetSCT_ClusterOnTrackTool', **kwargs) :
    the_name = makeName( name, kwargs)
    if 'LorentzAngleTool' not in kwargs :
        kwargs = setDefaults(kwargs, LorentzAngleTool = getSCTLorentzAngleTool())

    kwargs = setDefaults(kwargs,
        # CorrectionStrategy = -1,  # no position correction (test for bug #56477)
        CorrectionStrategy = 0,  # do correct position bias
        ErrorStrategy      = 2  # do use phi dependent errors
        )
    from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
    return InDet__SCT_ClusterOnTrackTool (the_name, **kwargs)

def getInDetBroadSCT_ClusterOnTrackTool(name='InDetBroadSCT_ClusterOnTrackTool', **kwargs) :
    return getInDetSCT_ClusterOnTrackTool(name=name, **setDefaults( kwargs, ErrorStrategy  = 0) )


@makePublicTool
def getInDetBroadTRT_DriftCircleOnTrackTool(name='InDetBroadTRT_DriftCircleOnTrackTool', **kwargs) :
    the_name = makeName( name, kwargs)
    from TRT_DriftCircleOnTrackTool.TRT_DriftCircleOnTrackToolConf import InDet__TRT_DriftCircleOnTrackNoDriftTimeTool
    return InDet__TRT_DriftCircleOnTrackNoDriftTimeTool(the_name)

# @TODO rename to InDetTRT_DriftCircleOnTrackTool ?
def getInDetTRT_DriftCircleOnTrackTool(name='TRT_DriftCircleOnTrackTool', **kwargs) :
    createAndAddCondAlg(getRIO_OnTrackErrorScalingCondAlg,'RIO_OnTrackErrorScalingCondAlg')
    kwargs = setDefaults(kwargs,
                         EventInfoKey       = getEventInfoKey(),
                         TRTErrorScalingKey = '/Indet/TrkErrorScalingTRT')
    return getInDetBroadTRT_DriftCircleOnTrackTool(name = name, **kwargs)

default_ScaleHitUncertainty = 2.5

@makePublicTool
def getInDetTRT_DriftCircleOnTrackUniversalTool(name='InDetTRT_RefitRotCreator',**kwargs) :
    the_name = makeName( name, kwargs)
    if 'RIOonTrackToolDrift' not in kwargs :
        kwargs = setDefaults(kwargs, RIOonTrackToolDrift = getInDetTRT_DriftCircleOnTrackTool())
    if 'RIOonTrackToolTube' not in kwargs :
        kwargs = setDefaults(kwargs, RIOonTrackToolTube  = getInDetBroadTRT_DriftCircleOnTrackTool())

    from TRT_DriftCircleOnTrackTool.TRT_DriftCircleOnTrackToolConf import InDet__TRT_DriftCircleOnTrackUniversalTool
    return InDet__TRT_DriftCircleOnTrackUniversalTool(name = the_name, **setDefaults(kwargs,
                                                                                     ScaleHitUncertainty = default_ScaleHitUncertainty))

@makePublicTool
def getInDetRotCreator(name='InDetRotCreator', **kwargs) :
    the_name = makeName( name, kwargs)
    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    from InDetRecExample.InDetJobProperties import InDetFlags
    use_broad_cluster_pix = InDetFlags.useBroadPixClusterErrors() and (not InDetFlags.doDBMstandalone())
    use_broad_cluster_sct = InDetFlags.useBroadSCTClusterErrors() and (not InDetFlags.doDBMstandalone())

    if 'ToolPixelCluster' not in kwargs :
        if use_broad_cluster_pix :
            kwargs = setDefaults( kwargs,
                                  ToolPixelCluster = getInDetBroadPixelClusterOnTrackTool(nameSuffix               = split_cluster_map_extension,
                                                                                          SplitClusterMapExtension = split_cluster_map_extension))
        else :
            kwargs = setDefaults( kwargs,
                                  ToolPixelCluster = getInDetPixelClusterOnTrackTool(nameSuffix               = split_cluster_map_extension,
                                                                                     SplitClusterMapExtension = split_cluster_map_extension))

    if 'ToolSCT_Cluster' not in kwargs :
        if use_broad_cluster_sct :
            kwargs = setDefaults( kwargs, ToolSCT_Cluster = getInDetBroadSCT_ClusterOnTrackTool())
        else :
            kwargs = setDefaults( kwargs, ToolSCT_Cluster = getInDetSCT_ClusterOnTrackTool())

    kwargs = setDefaults( kwargs, Mode             = 'indet')

    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
    return Trk__RIO_OnTrackCreator(name=the_name, **kwargs)

def getInDetRotCreatorPattern(name='InDetRotCreatorPattern', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        split_cluster_map_extension = kwargs.get('SplitClusterMapExtension','')
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster = getInDetPixelClusterOnTrackToolPattern(nameSuffix               = split_cluster_map_extension,
                                                                                       SplitClusterMapExtension = split_cluster_map_extension))
    return getInDetRotCreator(name=name, **kwargs)


def getInDetRotCreatorDBM(name='InDetRotCreatorDBM', **kwargs) :
    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    if 'ToolPixelCluster' not in kwargs :
        from InDetRecExample.InDetJobProperties import InDetFlags
        from AthenaCommon.DetFlags              import DetFlags
        if InDetFlags.loadRotCreator() and DetFlags.haveRIO.pixel_on():
            kwargs = setDefaults(kwargs,
                                 ToolPixelCluster = getInDetPixelClusterOnTrackToolDBM(nameSuffix               = split_cluster_map_extension,
                                                                                       SplitClusterMapExtension = split_cluster_map_extension))
        else :
            kwargs = setDefaults(kwargs,
                                 ToolPixelCluster = getInDetPixelClusterOnTrackTool(nameSuffix               = split_cluster_map_extension,
                                                                                    SplitClusterMapExtension = split_cluster_map_extension))
    return getInDetRotCreator(name=name, **kwargs)

def getInDetRotCreatorDigital(name='InDetRotCreatorDigital', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        split_cluster_map_extension = kwargs.get('SplitClusterMapExtension','')
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster = getInDetPixelClusterOnTrackToolDigital(nameSuffix               = split_cluster_map_extension,
                                                                                       SplitClusterMapExtension = split_cluster_map_extension))
    return getInDetRotCreator(name=name, **kwargs)

# @TODO rename to InDetBroadRotCreator
def getInDetBroadRotCreator(name='InDetBroadInDetRotCreator', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        split_cluster_map_extension = kwargs.get('SplitClusterMapExtension','')
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster    = getInDetBroadPixelClusterOnTrackTool(nameSuffix               = split_cluster_map_extension,
                                                                                        SplitClusterMapExtension = split_cluster_map_extension))
    if 'ToolSCT_Cluster' not in kwargs :
        kwargs = setDefaults(kwargs,
                             ToolSCT_Cluster     = getInDetBroadSCT_ClusterOnTrackTool())

    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.haveRIO.TRT_on():
        if 'ToolTRT_DriftCircle' not in kwargs :
            kwargs = setDefaults(kwargs,
                                 ToolTRT_DriftCircle = getInDetBroadTRT_DriftCircleOnTrackTool())

    return getInDetRotCreator(name=name, **kwargs)


def getInDetRefitRotCreator(name='InDetRefitRotCreator', **kwargs) :
    ScaleHitUncertainty = kwargs.pop('ScaleHitUncertainty',default_ScaleHitUncertainty)
    from InDetRecExample.InDetJobProperties import InDetFlags
    from AthenaCommon.DetFlags              import DetFlags

    if InDetFlags.redoTRT_LR():
        if DetFlags.haveRIO.TRT_on():
            if 'ToolTRT_DriftCircle' not in kwargs :
                kwargs = setDefaults(kwargs,
                                     ToolTRT_DriftCircle = getInDetTRT_DriftCircleOnTrackUniversalTool(ScaleHitUncertainty = ScaleHitUncertainty))

    return getInDetRotCreator(name = name, **kwargs)

@makePublicTool
def getInDetGsfMaterialUpdator(name='InDetGsfMaterialUpdator', **kwargs) :
    the_name = makeName( name, kwargs)
    if 'MultiComponentStateMerger' not in kwargs :
        kwargs=setDefaults(kwargs, MultiComponentStateMerger = getInDetGsfComponentReduction())

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMaterialMixtureConvolution
    return Trk__GsfMaterialMixtureConvolution (name = the_name, **kwargs)

@makePublicTool
def getInDetGsfComponentReduction(name='InDetGsfComponentReduction', **kwargs) :
    the_name = makeName( name, kwargs)
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__QuickCloseComponentsMultiStateMerger
    return Trk__QuickCloseComponentsMultiStateMerger (name = the_name, **setDefaults(kwargs, MaximumNumberOfComponents = 12))

@makePublicTool
def getInDetGsfExtrapolator(name='InDetGsfExtrapolator', **kwargs) :
    the_name = makeName(name,kwargs)
    if 'Propagators' not in kwargs :
        from AthenaCommon.AppMgr import ToolSvc
        kwargs=setDefaults(kwargs, Propagators = [ ToolSvc.InDetPropagator ] )

    if 'Navigator' not in kwargs :
        from AthenaCommon.AppMgr import ToolSvc
        kwargs=setDefaults(kwargs, Navigator   =  ToolSvc.InDetNavigator)

    if 'GsfMaterialConvolution' not in kwargs :
        kwargs=setDefaults(kwargs, GsfMaterialConvolution        = getInDetGsfMaterialUpdator())

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfExtrapolator
    return Trk__GsfExtrapolator(name = the_name, **setDefaults(kwargs,
                                                               SearchLevelClosestParameters  = 10,
                                                               StickyConfiguration           = True,
                                                               SurfaceBasedMaterialEffects   = False ))

@makePublicTool
def getPRDtoTrackMapTool(name='PRDtoTrackMapTool',**kwargs) :
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetKeys                       import InDetKeys
    from TrkAssociationTools.TrkAssociationToolsConf import Trk__PRDtoTrackMapTool
    return Trk__PRDtoTrackMapTool( name=the_name, **kwargs)

@makePublicTool
def getInDetPRDtoTrackMapToolGangedPixels(name='PRDtoTrackMapToolGangedPixels',**kwargs) :
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetKeys                       import InDetKeys
    kwargs = setDefaults( kwargs,
                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                          addTRToutliers                 = True)

    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRDtoTrackMapToolGangedPixels
    return InDet__InDetPRDtoTrackMapToolGangedPixels( name=the_name, **kwargs)


def getInDetTrigPRDtoTrackMapToolGangedPixels(name='InDetTrigPRDtoTrackMapToolGangedPixels',**kwargs) :
    return getInDetPRDtoTrackMapToolGangedPixels(name,
                                                 **setDefaults(kwargs,
                                                               PixelClusterAmbiguitiesMapName = "TrigPixelClusterAmbiguitiesMap",
                                                               addTRToutliers                 = False))

def getInDetTrackPRD_Association(name='InDetTrackPRD_Association', **kwargs) :
    the_name,prefix,suffix=makeNameGetPreAndSuffix(name,kwargs)

    if kwargs.get('TracksName',None) is None :
        raise Exception('Not TracksName argument provided')

    kwargs = setDefaults( kwargs,
                          AssociationTool    = getInDetPRDtoTrackMapToolGangedPixels() \
                                                    if 'AssociationTool' not in kwargs else None,
                          AssociationMapName = prefix+'PRDtoTrackMap'+suffix)

    from InDetTrackPRD_Association.InDetTrackPRD_AssociationConf import InDet__InDetTrackPRD_Association
    alg = InDet__InDetTrackPRD_Association(name = the_name, **kwargs)
    return alg


@makePublicTool
def getConstPRD_AssociationTool(name='ConstPRD_AssociationTool',**kwargs) :
    the_name,prefix,suffix=makeNameGetPreAndSuffix(name,kwargs)

    kwargs = setDefaults( kwargs,
                          SetupCorrect     = True,
                          MuonIdHelperTool = "",
                          PRDtoTrackMap    = prefix+'PRDtoTrackMap'+suffix)

    from TrkAssociationTools.TrkAssociationToolsConf import Trk__PRD_AssociationTool
    return Trk__PRD_AssociationTool(name=the_name, **kwargs)

@makePublicTool
def getInDetPrdAssociationTool(name='InDetPrdAssociationTool',**kwargs) :
    '''
    Provide an instance for all clients in which the tool is only set in c++
    '''
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetKeys                       import InDetKeys
    kwargs = setDefaults( kwargs,
                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                          addTRToutliers                 = True)

    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    return InDet__InDetPRD_AssociationToolGangedPixels(name=the_name, **kwargs)

def getInDetTrigPrdAssociationTool(name='InDetTrigPrdAssociationTool_setup',**kwargs) :
    return getInDetPrdAssociationTool(name,
                                      **setDefaults(kwargs,
                                                    PixelClusterAmbiguitiesMapName = "TrigPixelClusterAmbiguitiesMap",
                                                    addTRToutliers                 = False))


def getInDetPrdAssociationTool_setup(name='InDetPrdAssociationTool_setup',**kwargs) :
    '''
    Provide an instance for all clients which set the tool explicitely
    '''
    return getInDetPrdAssociationTool(name, **setDefaults(kwargs, SetupCorrect                   = True) )

def getInDetPixelConditionsSummaryTool() :
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
    from InDetRecExample.InDetJobProperties import InDetFlags

    from PixelConditionsTools.PixelConditionsSummaryToolSetup import PixelConditionsSummaryToolSetup
    pixelConditionsSummaryToolSetup = PixelConditionsSummaryToolSetup()
    pixelConditionsSummaryToolSetup.setUseConditions(True)
    pixelConditionsSummaryToolSetup.setUseDCSState((globalflags.DataSource=='data') and InDetFlags.usePixelDCS())
    pixelConditionsSummaryToolSetup.setUseByteStream((globalflags.DataSource=='data'))
    pixelConditionsSummaryToolSetup.setUseTDAQ(athenaCommonFlags.isOnline())
    pixelConditionsSummaryToolSetup.setUseDeadMap((not athenaCommonFlags.isOnline()))
    pixelConditionsSummaryToolSetup.setup()
    return pixelConditionsSummaryToolSetup.getTool()

@makePublicTool
def getInDetTestPixelLayerTool(name = "InDetTestPixelLayerTool", **kwargs) :
    the_name = makeName( name, kwargs)
    if 'PixelSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelSummaryTool = getInDetPixelConditionsSummaryTool())

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults( kwargs,
                          CheckActiveAreas = InDetFlags.checkDeadElementsOnTrack(),
                          CheckDeadRegions = InDetFlags.checkDeadElementsOnTrack(),
                          CheckDisabledFEs = InDetFlags.checkDeadElementsOnTrack())

    from InDetTestPixelLayer.InDetTestPixelLayerConf import InDet__InDetTestPixelLayerTool
    return InDet__InDetTestPixelLayerTool(name = the_name, **kwargs)

@makePublicTool
def getInDetNavigator(name='InDetNavigator', **kwargs) :
    the_name = makeName( name, kwargs)
    if 'TrackingGeometrySvc' not in kwargs :
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
        kwargs = setDefaults(kwargs, TrackingGeometrySvc = AtlasTrackingGeometrySvc)

    from TrkExTools.TrkExToolsConf import Trk__Navigator
    return Trk__Navigator(name=the_name,**kwargs )


@makePublicTool
def getInDetMaterialEffectsUpdator(name = "InDetMaterialEffectsUpdator", **kwargs) :
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetJobProperties import InDetFlags
    if not InDetFlags.solenoidOn():
        import AthenaCommon.SystemOfUnits as Units
        kwargs = setDefaults(kwargs,
                             EnergyLoss          = False,
                             ForceMomentum       = True,
                             ForcedMomentumValue = 1000*Units.MeV )
    from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
    return Trk__MaterialEffectsUpdator(name = the_name, **kwargs)

@makePublicTool
def getInDetPropagator(name='InDetPropagator',**kwargs) :
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.propagatorType() == "STEP":
        from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as Propagator
    else:
        from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator

    if InDetFlags.propagatorType() == "RungeKutta":
        kwargs=setDefaults(kwargs,
                           AccuracyParameter = 0.0001,
                           MaxStraightLineStep = .004 ) # Fixes a failed fit
    return Propagator(name = the_name, **kwargs)

# # set up the propagator for outside ID (A.S. needed as a fix for 14.5.0 )
# @makePublicTool
# def getInDetStepPropagator(name='InDetStepPropagator',**kwargs) :
#     from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as StepPropagator
#     return StepPropagator(name = name, **kwargs)

@makePublicTool
def getInDetMultipleScatteringUpdator(name='InDetMultipleScatteringUpdator', **kwargs) :
    the_name = makeName( name, kwargs)
    from TrkExTools.TrkExToolsConf import Trk__MultipleScatteringUpdator
    return Trk__MultipleScatteringUpdator(name = the_name, **setDefaults( kwargs, UseTrkUtils = False))

@makePublicTool
def getInDetExtrapolator(name='InDetExtrapolator', **kwargs) :
    the_name = makeName( name, kwargs)

    if 'Propagators' not in kwargs :
        kwargs = setDefaults( kwargs, Propagators = [ getInDetPropagator() ] ) # [ InDetPropagator, InDetStepPropagator ],
    propagator= kwargs.get('Propagators')[0].name() if  kwargs.get('Propagators',None) is not None and len(kwargs.get('Propagators',None))>0 else None

    if 'MaterialEffectsUpdators' not in kwargs :
        kwargs = setDefaults( kwargs, MaterialEffectsUpdators = [ getInDetMaterialEffectsUpdator() ] )
    material_updator= kwargs.get('MaterialEffectsUpdators')[0].name() if  kwargs.get('MaterialEffectsUpdators',None) is not None and len(kwargs.get('MaterialEffectsUpdators',None))>0  else None

    if 'Navigator' not in kwargs :
        kwargs = setDefaults( kwargs, Navigator               = getInDetNavigator())

    sub_propagators = []
    sub_updators    = []

    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for ID is (Rk,Mat)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for Calo is (Rk,MatLandau)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for MS is (STEP,Mat)
    #  sub_propagators += [ InDetStepPropagator.name() ]
    sub_updators    += [ material_updator ]
    # @TODO should check that all sub_propagators and sub_updators are actually defined.

    kwargs = setDefaults( kwargs,
                          SubPropagators          = sub_propagators,
                          SubMEUpdators           = sub_updators)

    from TrkExTools.TrkExToolsConf import Trk__Extrapolator
    return Trk__Extrapolator(name = the_name, **kwargs)

def_InDetSCT_ConditionsSummaryTool=None
def getInDetSCT_ConditionsSummaryTool() :
    return def_InDetSCT_ConditionsSummaryTool

@makePublicTool
def getInDetHoleSearchTool(name = 'InDetHoleSearchTool', **kwargs) :
    the_name = makeName( name, kwargs)
    from AthenaCommon.DetFlags    import DetFlags
    from InDetRecExample.InDetJobProperties import InDetFlags

    if 'Extrapolator' not in kwargs :
        kwargs = setDefaults( kwargs, Extrapolator     = getInDetExtrapolator())

    if 'PixelSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelSummaryTool = getInDetPixelConditionsSummaryTool() if DetFlags.haveRIO.pixel_on() else None)

    if 'SctSummaryTool' not in kwargs :
        from AthenaCommon.AppMgr import ToolSvc
        kwargs = setDefaults( kwargs, SctSummaryTool   = getInDetSCT_ConditionsSummaryTool()  if DetFlags.haveRIO.SCT_on()   else None)

    if 'PixelLayerTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelLayerTool   = getInDetTestPixelLayerTool())

    if InDetFlags.doCosmics :
        kwargs = setDefaults( kwargs, Cosmics = True)

    kwargs = setDefaults( kwargs,
                          usePixel                     = DetFlags.haveRIO.pixel_on(),
                          useSCT                       = DetFlags.haveRIO.SCT_on(),
                          CountDeadModulesAfterLastHit = True)

    from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
    return InDet__InDetTrackHoleSearchTool(name = the_name, **kwargs)


@makePublicTool
def getInDetPixelToTPIDTool(name = "InDetPixelToTPIDTool", **kwargs) :
    the_name = makeName( name, kwargs)

    from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixelToTPIDTool
    return InDet__PixelToTPIDTool(name = the_name, **kwargs)

@makePublicTool
def getInDetRecTestBLayerTool(name='InDetRecTestBLayerTool', **kwargs) :
    the_name = makeName( name, kwargs)
    from AthenaCommon.DetFlags    import DetFlags
    if not DetFlags.haveRIO.pixel_on() :
        return None

    if 'Extrapolator' not in kwargs :
        kwargs = setDefaults( kwargs, Extrapolator     = getInDetExtrapolator())

    if 'PixelSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelSummaryTool = getInDetPixelConditionsSummaryTool())

    from InDetTestBLayer.InDetTestBLayerConf import InDet__InDetTestBLayerTool
    return InDet__InDetTestBLayerTool(name=the_name, **kwargs)


@makePublicTool
def getInDetTRT_LocalOccupancy(name ="InDet_TRT_LocalOccupancy", **kwargs) :
    the_name = makeName( name, kwargs)
    if 'TRTCalDbTool' not in kwargs :
        import InDetRecExample.TRTCommon as TRTCommon
        kwargs = setDefaults( kwargs, TRTCalDbTool = TRTCommon.getInDetTRTCalDbTool() )

    if 'TRTStrawStatusSummaryTool' not in kwargs :
        import InDetRecExample.TRTCommon as TRTCommon
        kwargs = setDefaults( kwargs, TRTStrawStatusSummaryTool = TRTCommon.getInDetTRTStrawStatusSummaryTool() )

    from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
    return InDet__TRT_LocalOccupancy(name=the_name, **setDefaults( kwargs, isTrigger = False) )

@makePublicTool
def getInDetTRT_ElectronPidTool(name = "InDetTRT_ElectronPidTool", **kwargs) :
    the_name = makeName( name, kwargs)
    from AthenaCommon.DetFlags    import DetFlags
    from InDetRecExample.InDetJobProperties import InDetFlags
    if not DetFlags.haveRIO.TRT_on() or  InDetFlags.doSLHC() or  InDetFlags.doHighPileup() \
            or  InDetFlags.useExistingTracksAsInput(): # TRT_RDOs (used byt the TRT_LocalOccupancy tool) are not present in ESD
        return None

    if 'Extrapolator' not in kwargs :
        kwargs = setDefaults( kwargs, TRT_LocalOccupancyTool    = getInDetTRT_LocalOccupancy())

    if 'Extrapolator' not in kwargs :
        import InDetRecExample.TRTCommon as TRTCommon
        kwargs = setDefaults( kwargs, TRTStrawSummaryTool       = TRTCommon.getInDetTRTStrawStatusSummaryTool())

    from AthenaCommon.GlobalFlags import globalflags
    kwargs = setDefaults( kwargs, isData = (globalflags.DataSource == 'data'))

    from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_ElectronPidToolRun2
    return InDet__TRT_ElectronPidToolRun2(name = the_name, **kwargs)



@makePublicTool
def getInDetSummaryHelper(name='InDetSummaryHelper',**kwargs) :
    the_name = makeName( name, kwargs)
    isHLT=kwargs.pop("isHLT",False)
    # Configrable version of loading the InDetTrackSummaryHelperTool
    #
    if 'AssoTool' not in kwargs :
        kwargs = setDefaults( kwargs, AssoTool = getInDetPrdAssociationTool_setup()   if not isHLT else   getInDetTrigPrdAssociationTool())
    if 'HoleSearch' not in kwargs :
        kwargs = setDefaults( kwargs, HoleSearch = getInDetHoleSearchTool()) # @TODO trigger specific hole search tool ?

    from AthenaCommon.DetFlags    import DetFlags
    if not DetFlags.haveRIO.TRT_on() :
        kwargs = setDefaults( kwargs, TRTStrawSummarySvc = "")

    from InDetRecExample.InDetJobProperties import InDetFlags
    from AthenaCommon.DetFlags              import DetFlags
    kwargs = setDefaults( kwargs,
                          PixelToTPIDTool = None,         # we don't want to use those tools during pattern
                          TestBLayerTool  = None,         # we don't want to use those tools during pattern
                          # PixelToTPIDTool = InDetPixelToTPIDTool,
                          # TestBLayerTool  = InDetRecTestBLayerTool,
                          RunningTIDE_Ambi = InDetFlags.doTIDE_Ambi(),
                          DoSharedHits    = False,
                          usePixel        = DetFlags.haveRIO.pixel_on(),
                          useSCT          = DetFlags.haveRIO.SCT_on(),
                          useTRT          = DetFlags.haveRIO.TRT_on())

    from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
    return InDet__InDetTrackSummaryHelperTool(name = the_name,**kwargs)

def getInDetSummaryHelperNoHoleSearch(name='InDetSummaryHelperNoHoleSearch',**kwargs) :
    if 'HoleSearch' not in kwargs :
        kwargs = setDefaults( kwargs, HoleSearch = None)
    return getInDetSummaryHelper(name,**kwargs)


def getInDetSummaryHelperSharedHits(name='InDetSummaryHelperSharedHits',**kwargs) :

    if 'PixelToTPIDTool' not in kwargs :
        kwargs = setDefaults( kwargs,          PixelToTPIDTool = getInDetPixelToTPIDTool())

    if 'TestBLayerTool' not in kwargs :
        kwargs = setDefaults( kwargs,          TestBLayerTool  = getInDetRecTestBLayerTool())

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults( kwargs,     DoSharedHits    = InDetFlags.doSharedHits())

    from AthenaCommon.DetFlags    import DetFlags
    if DetFlags.haveRIO.TRT_on() :
        from InDetRecExample.InDetJobProperties import InDetFlags
        kwargs = setDefaults( kwargs, DoSharedHitsTRT = InDetFlags.doSharedHits())

    return getInDetSummaryHelper(name, **kwargs)


@makePublicTool
def getInDetTrackSummaryTool(name='InDetTrackSummaryTool',**kwargs) :
    # makeName will remove the namePrefix in suffix from kwargs, so copyArgs has to be first
    hlt_args = copyArgs(kwargs,['isHLT','namePrefix'])
    kwargs.pop('isHLT',None)
    the_name = makeName( name, kwargs)
    do_holes=kwargs.get("doHolesInDet",True)
    if 'InDetSummaryHelperTool' not in kwargs :
        kwargs = setDefaults( kwargs, InDetSummaryHelperTool = getInDetSummaryHelper(**hlt_args) if do_holes else getInDetSummaryHelperNoHoleSearch())

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs = setDefaults(kwargs,
                         doSharedHits           = False,
                         doHolesInDet           = do_holes,
                         TRT_ElectronPidTool    = None,         # we don't want to use those tools during pattern
                         TRT_ToT_dEdxTool       = None,         # dito
                         PixelToTPIDTool        = None)         # we don't want to use those tools during pattern
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    return Trk__TrackSummaryTool(name = the_name, **kwargs)

def getInDetTrackSummaryToolNoHoleSearch(name='InDetTrackSummaryToolNoHoleSearch',**kwargs) :
    return getInDetTrackSummaryTool(name, **setDefaults(kwargs, doHolesInDet           = False))

def getInDetTrackSummaryToolSharedHits(name='InDetTrackSummaryToolSharedHits',**kwargs) :

    if 'InDetSummaryHelperTool' not in kwargs :
        kwargs = setDefaults( kwargs, InDetSummaryHelperTool = getInDetSummaryHelperSharedHits())

    if 'TRT_ElectronPidTool' not in kwargs :
        kwargs = setDefaults( kwargs, TRT_ElectronPidTool    = getInDetTRT_ElectronPidTool())

    if 'TRT_ToT_dEdxTool' not in kwargs :
        import InDetRecExample.TRTCommon as TRTCommon
        kwargs = setDefaults( kwargs, TRT_ToT_dEdxTool       = TRTCommon.getInDetTRT_dEdxTool())

    if 'PixelToTPIDTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelToTPIDTool        = getInDetPixelToTPIDTool())

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults(kwargs,
                         doSharedHits           = InDetFlags.doSharedHits(),
                         TRTdEdx_DivideByL      = True, # default is True
                         TRTdEdx_useHThits      = True, # default is True
                         TRTdEdx_corrected      = True, # default is True
                         minTRThitsForTRTdEdx   = 1)    # default is 1

    return getInDetTrackSummaryTool( name, **kwargs)

def getInDetTrigTrackSummaryTool(name='InDetTrackSummaryTool',**kwargs) :
    return getInDetTrackSummaryTool(name,**setDefaults(kwargs,
                                                       namePrefix = "InDetTrig",
                                                       isHLT      = True))
