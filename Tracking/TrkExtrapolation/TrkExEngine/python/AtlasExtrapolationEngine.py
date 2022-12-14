# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

######################################################
# AtlasExtrapolationEngine module
#
# it inherits from Trk__Extrapolator and uses
# the AtlasTrackingGeometrySvc
#
######################################################

# import the ExtrapolationEngine configurable
from TrkExEngine.TrkExEngineConf import Trk__ExtrapolationEngine as ExEngine
from InDetRecExample.TrackingCommon import createAndAddCondAlg
from InDetRecExample import TrackingCommon

# define the class
class AtlasExtrapolationEngine( ExEngine ):
    # constructor
    def __init__(self,name = 'Extrapolation', nameprefix = 'Atlas', ToolOutputLevel = None, TrackingGeometrySvc = None):
       
        # get the correct TrackingGeometry setup
        AtlasTrackingGeometrySvc=None
        AtlasTrackingGeometryCondAlg=None
        if not TrackingGeometrySvc :
            if TrackingCommon.use_tracking_geometry_cond_alg :
                AtlasTrackingGeometryCondAlg = createAndAddCondAlg(TrackingCommon.getTrackingGeometryCondAlg, "AtlasTrackingGeometryCondAlg", name="AtlasTrackingGeometryCondAlg")
            else :
                from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
                from AthenaCommon.AppMgr import ServiceMgr as svcMgr
                AtlasTrackingGeometrySvc = svcMgr.AtlasTrackingGeometrySvc  # noqa: F811
        else :
            AtlasTrackingGeometrySvc = TrackingGeometrySvc

        # import the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir() :
            ToolSvc = ToolSvc()

        # load the RungeKutta Propagator
        from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator
        RungeKuttaPropagator = Trk__RungeKuttaPropagator(name = nameprefix+'RungeKuttaPropagator')
        if ToolOutputLevel :
            RungeKuttaPropagator.OutputLevel     = ToolOutputLevel
        ToolSvc += RungeKuttaPropagator
        
        # from the Propagator create a Propagation engine to handle path length
        from TrkExEngine.TrkExEngineConf import Trk__PropagationEngine
        StaticPropagator = Trk__PropagationEngine(name = nameprefix+'StaticPropagation')
        # give the tools it needs 
        StaticPropagator.Propagator               = RungeKuttaPropagator
        # configure output formatting               
        StaticPropagator.OutputPrefix             = '[SP] - '
        StaticPropagator.OutputPostfix            = ' - '
        if ToolOutputLevel : 
            StaticPropagator.OutputLevel          = ToolOutputLevel
        # add to tool service
        ToolSvc += StaticPropagator
        
        # load the material effects engine
        from TrkExEngine.TrkExEngineConf import Trk__MaterialEffectsEngine
        MaterialEffectsEngine = Trk__MaterialEffectsEngine(name = nameprefix+'MaterialEffects')
        # configure output formatting               
        MaterialEffectsEngine.OutputPrefix        = '[ME] - '
        MaterialEffectsEngine.OutputPostfix       = ' - '
        #MaterialEffectsEngine.EnergyLossCorrection = False
        if ToolOutputLevel : 
            MaterialEffectsEngine.OutputLevel     = ToolOutputLevel
        # add to tool service
        ToolSvc += MaterialEffectsEngine
        
        # load the static navigation engine
        from TrkExEngine.TrkExEngineConf import Trk__StaticNavigationEngine
        StaticNavigator = Trk__StaticNavigationEngine(name = nameprefix+'StaticNavigation')
        # give the tools it needs 
        StaticNavigator.PropagationEngine        = StaticPropagator
        StaticNavigator.MaterialEffectsEngine    = MaterialEffectsEngine
        # Geometry name
        StaticNavigator.TrackingGeometryReadKey     = AtlasTrackingGeometryCondAlg.TrackingGeometryWriteKey  if AtlasTrackingGeometryCondAlg is not None else ''
        # configure output formatting               
        StaticNavigator.OutputPrefix             = '[SN] - '
        StaticNavigator.OutputPostfix            = ' - '
        if ToolOutputLevel : 
            StaticNavigator.OutputLevel              = ToolOutputLevel
        # add to tool service
        ToolSvc += StaticNavigator
        
        
        # load the Static ExtrapolationEngine
        from TrkExEngine.TrkExEngineConf import Trk__StaticEngine
        StaticExtrapolator = Trk__StaticEngine(name = nameprefix+'StaticExtrapolation')
        # give the tools it needs 
        StaticExtrapolator.PropagationEngine        = StaticPropagator
        StaticExtrapolator.MaterialEffectsEngine    = MaterialEffectsEngine
        StaticExtrapolator.NavigationEngine         = StaticNavigator
        # configure output formatting               
        StaticExtrapolator.OutputPrefix             = '[SE] - '
        StaticExtrapolator.OutputPostfix            = ' - '
        if ToolOutputLevel : 
            StaticExtrapolator.OutputLevel              = ToolOutputLevel
        # add to tool service
        ToolSvc += StaticExtrapolator
       
        # call the base class constructor
        ExEngine.__init__(self, name=nameprefix+'Extrapolation',
                          ExtrapolationEngines   = [ StaticExtrapolator ],
                          PropagationEngine      = StaticPropagator,
                          NavigationEngine       = StaticNavigator,
                          TrackingGeometrySvc    = ''      if AtlasTrackingGeometryCondAlg is not None else AtlasTrackingGeometrySvc,
                          TrackingGeometryReadKey= AtlasTrackingGeometryCondAlg.TrackingGeometryWriteKey  if AtlasTrackingGeometryCondAlg is not None else '',
                          OutputPrefix           = '[ME] - ',
                          OutputPostfix          = ' - ')
        # set the output level
        if ToolOutputLevel :
            self.OutputLevel = ToolOutputLevel
