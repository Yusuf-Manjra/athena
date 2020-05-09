# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# New configuration for ATLAS extrapolator
# Based heavily on AtlasExtrapolator.py

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

# import the Extrapolator configurable
Trk__Extrapolator=CompFactory.Trk.Extrapolator

# define the class
def AtlasExtrapolatorCfg( flags, name = 'AtlasExtrapolator' ):
       result=ComponentAccumulator()

       acc  = MagneticFieldSvcCfg(flags)
       result.merge(acc)

       # get the correct TrackingGeometry setup
       from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
       acc = TrackingGeometrySvcCfg(flags)
       geom_svc = acc.getPrimary() 
       result.merge(acc)

       # PROPAGATOR DEFAULTS --------------------------------------------------------------------------------------
       
       AtlasPropagators  = []
       
       RkPropagator=CompFactory.Trk.RungeKuttaPropagator
       AtlasRungeKuttaPropagator = RkPropagator(name = 'AtlasRungeKuttaPropagator')
       result.addPublicTool(AtlasRungeKuttaPropagator) #TODO remove one day
       
       AtlasPropagators += [AtlasRungeKuttaPropagator]
       
       STEP_Propagator=CompFactory.Trk.STEP_Propagator 
       AtlasSTEP_Propagator = STEP_Propagator(name = 'AtlasSTEP_Propagator')
       result.addPublicTool(AtlasSTEP_Propagator) #TODO remove one day
       
       AtlasPropagators += [AtlasSTEP_Propagator]

       # UPDATOR DEFAULTS -----------------------------------------------------------------------------------------       
       
       AtlasUpdators    = []

       MaterialEffectsUpdator=CompFactory.Trk.MaterialEffectsUpdator
       AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
       result.addPublicTool(AtlasMaterialEffectsUpdator) #TODO remove one day
       
       AtlasUpdators    += [ AtlasMaterialEffectsUpdator ]

       AtlasMaterialEffectsUpdatorLandau = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdatorLandau')
       AtlasMaterialEffectsUpdatorLandau.LandauMode = True
       result.addPublicTool(AtlasMaterialEffectsUpdatorLandau) #TODO remove one day
       
       AtlasUpdators    += [ AtlasMaterialEffectsUpdatorLandau ]

       # the UNIQUE NAVIGATOR ( === UNIQUE GEOMETRY) --------------------------------------------------------------
       Trk__Navigator=CompFactory.Trk.Navigator
       AtlasNavigator = Trk__Navigator(name = 'AtlasNavigator')
       AtlasNavigator.TrackingGeometrySvc = geom_svc
       result.addPublicTool(AtlasNavigator) #TODO remove one day

       # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE
       
       AtlasSubPropagators = []
       AtlasSubUpdators = []

       # -------------------- set it depending on the geometry ----------------------------------------------------
       # default for Global is (Rk,Mat)
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]

       # default for ID is (Rk,Mat)
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]

       # default for BeamPipe is (STEP,Mat)
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]
       
       # default for Calo is (STEP,Mat)
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]
       
       # default for MS is (STEP,Mat)
       AtlasSubPropagators += [ AtlasSTEP_Propagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]

       # default for Cavern is (Rk,Mat)
       AtlasSubPropagators += [ AtlasRungeKuttaPropagator.name ]
       AtlasSubUpdators    += [ AtlasMaterialEffectsUpdator.name ]
       
       # ----------------------------------------------------------------------------------------------------------          
       
       # call the base class constructor
       Extrapolator = Trk__Extrapolator(name,\
                                  Navigator = AtlasNavigator,\
                                  MaterialEffectsUpdators = AtlasUpdators,\
                                  Propagators = AtlasPropagators,\
                                  SubPropagators = AtlasSubPropagators,\
                                  SubMEUpdators = AtlasSubUpdators
                                  )


       result.setPrivateTools(Extrapolator)

       return result
