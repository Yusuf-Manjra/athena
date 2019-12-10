
def _args( kwargs, **extra_kwargs) :
    import copy
    _kwargs = copy.copy(kwargs)
    for kw in extra_kwargs :
        _kwargs.setdefault(kw,extra_kwargs[kw])
    return _kwargs

def getInDetTRTStrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool", **kwargs) :
    from AthenaCommon.GlobalFlags import globalflags
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
    return TRT_StrawStatusSummaryTool(name = name, **_args( kwargs, isGEANT4=(globalflags.DataSource == 'geant4')))

def getInDetTRTCalDbTool(name = "TRT_CalDbTool", **kwargs) :
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
    return TRT_CalDbTool(name = name, **kwargs)

def getInDetTRT_dEdxTool(**kwargs) :
    InDetTRT_dEdxTool = None


    from InDetRecExample.InDetJobProperties import InDetFlags
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.GlobalFlags           import globalflags
    if DetFlags.haveRIO.TRT_on() and not InDetFlags.doSLHC() and not InDetFlags.doHighPileup() :
        from AthenaCommon.AppMgr import ToolSvc
        the_name=kwargs.pop('name','InDetTRT_dEdxTool')
        if hasattr(ToolSvc,the_name) :
            return getattr(ToolSvc,the_name)

        from TRT_ToT_Tools.TRT_ToT_ToolsConf import  TRT_ToT_dEdx
        InDetTRT_dEdxTool = TRT_ToT_dEdx(name=the_name,
                                         **_args(kwargs,
                                                 TRT_dEdx_whichToTEstimatorAlgo=2,  # default is 2
                                                 TRT_dEdx_useTrackPartWithGasType=3, # default is 3
                                                 TRT_dEdx_toolScenario=0, # default is 0
                                                 TRT_dEdx_applyMimicToXeCorrection=False, # default is False
                                                 TRT_dEdx_trackConfig_maxRtrack=1.9, # default is 1.9
                                                 TRT_dEdx_trackConfig_minRtrack=0.01, # default is 0.01
                                                 TRT_dEdx_useZeroRHitCut=True,
                                                 TRT_dEdx_isData=globalflags.DataSource != 'geant4')  # default is True
                                         )
        ToolSvc += InDetTRT_dEdxTool

    return InDetTRT_dEdxTool
