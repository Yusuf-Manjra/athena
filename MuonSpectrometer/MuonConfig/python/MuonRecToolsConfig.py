# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Configuration of tools shared between Segment Finding and Track Building

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# Tracking
from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg

def MuonSeededSegmentFinderCfg(flags,name="MuonSeededSegmentFinder", **kwargs):
    from MuonTrackFinderTools.MuonTrackFinderToolsConf import Muon__MuonSeededSegmentFinder
    from MuonSegmentFindingConfig import DCMathSegmentMakerCfg, MdtMathSegmentFinder # FIXME - should really shift this to RecTools then.
    result = ComponentAccumulator()
    
    mdt_segment_finder = MdtMathSegmentFinder(flags, name="MCTBMdtMathSegmentFinder", UseChamberTheta = False, AssociationRoadWidth = 1.5)
    result.addPublicTool(mdt_segment_finder)
    
    if "SegmentMaker" not in kwargs or "SegmentMakerNoHoles" not in kwargs:
        seg_maker=""
        acc={}
        if flags.Beam.Type == 'collisions':
            acc = DCMathSegmentMakerCfg( flags, MdtSegmentFinder = mdt_segment_finder, SinAngleCut = 0.04, DoGeometry = True)
        else:  # cosmics or singlebeam
            acc = DCMathSegmentMakerCfg( flags, MdtSegmentFinder = mdt_segment_finder, SinAngleCut = 0.1,  DoGeometry = False, AddUnassociatedPhiHits= True )
        seg_maker = acc.getPrimary()
        result.merge(acc)
        result.addPublicTool(seg_maker)
        kwargs.setdefault("SegmentMaker", seg_maker)
        kwargs.setdefault("SegmentMakerNoHoles", seg_maker) #FIXME. Just remove one.
    if not flags.Detector.GeometryCSC:
        kwargs.setdefault("CscPrepDataContainer","")
    else:
        kwargs.setdefault("sTgcPrepDataContainer","")
        kwargs.setdefault("MMPrepDataContainer","")
    
    muon_seeded_segment_finder = Muon__MuonSeededSegmentFinder(name, **kwargs)
    result.setPrivateTools(muon_seeded_segment_finder)
    return result
        
        
def MuonSegmentMomentumFromFieldCfg(flags, name="MuonSegmentMomentumFromField", **kwargs):
    from MuonSegmentMomentum.MuonSegmentMomentumConf import MuonSegmentMomentumFromField
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    
    result = ComponentAccumulator()
    acc  = MagneticFieldSvcCfg(flags) 
    magfieldsvc = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MagFieldSvc", magfieldsvc)
    
    navigator_ca = MuonNavigatorCfg(flags)
    navigator = navigator_ca.popPrivateTools()
    result.addPublicTool(navigator)
    result.merge(navigator_ca)
    kwargs.setdefault("NavigatorTool", navigator)
    
    acc = MuonSTEP_PropagatorCfg(flags)
    muon_prop = acc.getPrimary()
    result.merge(acc)
    result.addPublicTool(muon_prop)
    kwargs.setdefault("PropagatorTool", muon_prop)
        
    muon_seg_mom_from_field = MuonSegmentMomentumFromField(name=name, **kwargs)
    result.setPrivateTools(muon_seg_mom_from_field)
    return result
    
def MuonTrackSummaryHelperToolCfg(flags, name="MuonTrackSummaryHelperTool", **kwargs):
    # m_muonTgTool("MuonHolesOnTrack"),
    #   m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    #   m_edmHelperTool("Muon::MuonEDMHelperSvc/MuonEDMHelperSvc"),
    #   m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
    #   m_slExtrapolator("Trk::Extrapolator/MuonStraightLineExtrapolator"),
    
    result = ComponentAccumulator()
    acc  = TrackingGeometrySvcCfg(flags)
    
    result.merge(acc)
    kwargs.setdefault("TrackingGeometryName", 'AtlasTrackingGeometry') # FIXME - get this from somewhere?
    
    acc = MuonExtrapolatorCfg(flags)
    extrap = acc.getPrimary()
    acc.addPublicTool(extrap)
    result.merge(acc)
    kwargs.setdefault("Extrapolator", extrap)

    from MuonTGRecTools.MuonTGRecToolsConf import Muon__MuonHolesOnTrackTool
    holetool = Muon__MuonHolesOnTrackTool (ExtrapolatorName = extrap,
                                           TrackingGeometryName = 'MuonStandaloneTrackingGeometry')
    
    kwargs.setdefault("DoHolesOnTrack", False)
    kwargs.setdefault("CalculateCloseHits", True)
    kwargs.setdefault("HoleOnTrackTool", holetool)

    from MuonTrackSummaryHelperTool.MuonTrackSummaryHelperToolConf import Muon__MuonTrackSummaryHelperTool
    result.setPrivateTools(Muon__MuonTrackSummaryHelperTool(name=name,**kwargs))
    return result

def MuonTrackSummaryToolCfg(flags, name="MuonTrackSummaryTool", **kwargs):
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    
    result = ComponentAccumulator()
    acc = MuonTrackSummaryHelperToolCfg(flags)
    track_summary_helper = acc.getPrimary()
    acc.addPublicTool(track_summary_helper)
    result.merge(acc)
    kwargs.setdefault("MuonSummaryHelperTool", track_summary_helper )
    kwargs.setdefault("doSharedHits", False )
    kwargs.setdefault("AddDetailedMuonSummary", True )
    result.setPrivateTools(Trk__TrackSummaryTool(name=name,**kwargs))
    return result

def MuonTrackScoringToolCfg(flags, name="MuonTrackScoringTool", **kwargs):
    from MuonTrackFinderTools.MuonTrackFinderToolsConf import Muon__MuonTrackScoringTool
    
    # m_trkSummaryTool("Trk::TrackSummaryTool"),    
    # m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    result = ComponentAccumulator()
    acc = MuonTrackSummaryToolCfg(flags)
    track_summary = acc.getPrimary( )
    acc.addPublicTool(track_summary)
    result.merge(acc)
    kwargs.setdefault('SumHelpTool', track_summary)
    result.setPrivateTools(Muon__MuonTrackScoringTool(name=name,**kwargs))
    return result

def MuonAmbiProcessorCfg(flags, name="MuonAmbiProcessor", **kwargs):
    from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__TrackSelectionProcessorTool
    from MuonAmbiTrackSelectionTool.MuonAmbiTrackSelectionToolConf import Muon__MuonAmbiTrackSelectionTool
    
    #m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"), 
    #m_selectionTool("InDet::InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool")
    result = ComponentAccumulator()
    kwargs.setdefault('DropDouble', False)
    acc  = MuonTrackScoringToolCfg( flags )
    scoring_tool = acc.getPrimary()
    acc.addPublicTool(scoring_tool)
    result.merge(acc)
    kwargs.setdefault('ScoringTool', scoring_tool )
    muon_ami_selection_tool = Muon__MuonAmbiTrackSelectionTool()
    result.addPublicTool(muon_ami_selection_tool)
    kwargs.setdefault('SelectionTool', muon_ami_selection_tool)
    result.setPrivateTools(Trk__TrackSelectionProcessorTool(name=name,**kwargs))
    return result

def MuonTrackCleanerCfg(flags, **kwargs):
    from MuonTrackFinderTools.MuonTrackFinderToolsConf import Muon__MuonTrackCleaner
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg, TriggerChamberClusterOnTrackCreatorCfg
    # declareProperty("IdHelper",m_idHelper);
    # declareProperty("Helper",m_edmHelperSvc);
    # declareProperty("Printer",m_printer);
    # declareProperty("MdtRotCreator",  m_mdtRotCreator );
    # declareProperty("CompRotCreator", m_compRotCreator );
    # declareProperty("PullCalculator", m_pullCalculator );
    # declareProperty("Extrapolator",   m_extrapolator );
    # declareProperty("MagFieldSvc",    m_magFieldSvc );
    # declareProperty("MeasurementUpdator",   m_measurementUpdator );
    result=ComponentAccumulator()
    
    acc = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_creator=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MdtRotCreator", mdt_creator)
    
    acc = TriggerChamberClusterOnTrackCreatorCfg(flags)
    muon_cluster_creator=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("CompRotCreator", muon_cluster_creator)
    
    # For PullCalculator, just let it get default for moment. FIXME
    
    extrapolator_CA = MuonExtrapolatorCfg(flags)
    extrapolator = extrapolator_CA.getPrimary()
    acc.addPublicTool( extrapolator ) # TODO remove
    result.merge( extrapolator_CA )
    kwargs.setdefault("Extrapolator", extrapolator)

    acc = MCTBFitterCfg(flags, name = "SLFitter", StraightLine=True)
    slfitter = acc.getPrimary()
    acc.addPublicTool(slfitter)
    result.merge(acc)
    kwargs.setdefault("SLFitter", slfitter)

    acc = MCTBFitterCfg(flags, name = "CurvedFitter")
    fitter = acc.getPrimary()
    
    acc.addPublicTool(fitter)
    result.merge(acc)
    kwargs.setdefault("Fitter", fitter)    

    # kwargs.setdefault("MagFieldSvc", mag_field_svc) Default for moment

    # FIXME - do remaining tools
    
    result.setPrivateTools(Muon__MuonTrackCleaner(**kwargs))
    
    return result 

def MuonStationIntersectSvcCfg(flags, name='MuonStationIntersectSvc',**kwargs):
    from MuonStationIntersectSvc.MuonStationIntersectSvcConf import MuonStationIntersectSvc
    # Has dependency IdHelperTool (which we ignore for now)
    result = ComponentAccumulator()
    muon_station_intersect_svc = MuonStationIntersectSvc(name=name, **kwargs)
    result.addService(muon_station_intersect_svc, primary=True)
    return result

# default muon navigator
def MuonNavigatorCfg(flags, name="MuonNavigator", **kwargs):
    from TrkExTools.TrkExToolsConf import Trk__Navigator
    
    result = ComponentAccumulator()

    acc  = TrackingGeometrySvcCfg(flags)
    result.merge(acc)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary() )

    navigator = Trk__Navigator(name=name, **kwargs)
    result.setPrivateTools(navigator)   
    return result     

def MuonExtrapolatorCfg(flags,name = "MuonExtrapolator", **kwargs):
    from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator, Trk__EnergyLossUpdator, Trk__MultipleScatteringUpdator
    
    from TrkExTools.TrkExToolsConf import Trk__Extrapolator 
    result = ComponentAccumulator()
    
    energy_loss_updator = Trk__EnergyLossUpdator() # Not really sure these should be tools...
    result.addPublicTool(energy_loss_updator) # TODO remove 

    # This one has a dependency on RndNumberService
    mult_scat_updator = Trk__MultipleScatteringUpdator()
    result.addPublicTool(mult_scat_updator) # TODO remove 
    
    material_effects_updator = Trk__MaterialEffectsUpdator( EnergyLossUpdator=energy_loss_updator, MultipleScatteringUpdator=mult_scat_updator)
    result.addPublicTool(material_effects_updator)
    kwargs.setdefault("MaterialEffectsUpdators", [material_effects_updator])
    
    navigator_ca = MuonNavigatorCfg(flags)
    navigator = navigator_ca.popPrivateTools()
    result.addPublicTool(navigator)
    result.merge(navigator_ca)
    kwargs.setdefault("Navigator", navigator)
    
    acc = MuonSTEP_PropagatorCfg(flags, Tolerance = 0.00001, MaterialEffects=True, IncludeBgradients=True)
    muon_prop = acc.getPrimary()
    result.merge(acc)
    
    result.addPublicTool(muon_prop)
    
    kwargs.setdefault("Propagators", [muon_prop])
    kwargs.setdefault("ResolveMuonStation", True)
    kwargs.setdefault("Tolerance", 0.0011)  # must be > 1um to avoid missing MTG intersections
    extrap = Trk__Extrapolator(name=name, **kwargs)
    result.setPrivateTools(extrap)
    return result

def MuonChi2TrackFitterCfg(flags, name='MuonChi2TrackFitter', **kwargs):
    from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdator
    from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
    import MuonConfig.MuonRIO_OnTrackCreatorConfig # Trying to avoid circular dependencies here
    
    result = ComponentAccumulator()

    extrapolator_CA = MuonExtrapolatorCfg(flags)
    extrapolator= extrapolator_CA.getPrimary()
    
    result.addPublicTool(extrapolator) # TODO remove
    result.merge(extrapolator_CA)
    
    acc = MuonConfig.MuonRIO_OnTrackCreatorConfig.MuonRotCreatorCfg(flags)
    rotcreator=acc.getPrimary()
    result.merge(acc)
    
    measurement_updater = Trk__KalmanUpdator()
    result.addPublicTool(measurement_updater) #FIXME

    kwargs.setdefault("ExtrapolationTool"    , extrapolator)
    kwargs.setdefault("RotCreatorTool"       , rotcreator)
    kwargs.setdefault("MeasurementUpdateTool", measurement_updater)
    kwargs.setdefault("StraightLine"         , False)
    kwargs.setdefault("OutlierCut"           , 3.0)
    kwargs.setdefault("GetMaterialFromTrack" , False)
    kwargs.setdefault("RejectLargeNScat"     , True)

    # take propagator and navigator from the extrapolator
    kwargs.setdefault("PropagatorTool", extrapolator.Propagators[0])
    kwargs.setdefault("NavigatorTool",  extrapolator.Navigator)

    fitter = Trk__GlobalChi2Fitter(name=name, **kwargs)
    result.setPrivateTools(fitter)
    # print fitter
    return result
    
def MuonSTEP_PropagatorCfg(flags, name='MuonSTEP_Propagator', **kwargs):
    # Really there should be a central configuration for the STEP propagator. FIXME
    # In the old ConfigDb this was named MuonStraightLinePropagator (!)
    from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result = ComponentAccumulator()
    
    acc  = MagneticFieldSvcCfg(flags) 
    magfieldsvc = acc.getPrimary()
    result.merge(acc)
    
    kwargs.setdefault("Tolerance", 0.00001 )
    kwargs.setdefault("MaterialEffects", True  )
    kwargs.setdefault("IncludeBgradients", True  )
    kwargs.setdefault("MagFieldSvc", magfieldsvc  )
    
    propagator = Trk__STEP_Propagator(name=name, **kwargs)
    result.setPrivateTools(propagator)
    return result

def MCTBExtrapolatorCfg(flags, name='MCTBExtrapolator',**kwargs):
    result = ComponentAccumulator()
    
    acc = MuonSTEP_PropagatorCfg(flags)
    prop = acc.getPrimary()
    result.addPublicTool(prop) 
    result.merge(acc)
    kwargs.setdefault("Propagators", [ prop ]) 
    kwargs.setdefault("ResolveMuonStation", False)
    acc = MuonExtrapolatorCfg(flags, name=name)
    result.setPrivateTools(acc.getPrimary())
    result.merge(acc)
    
    return result

def MCTBFitterCfg(flags, name='MCTBFitter', **kwargs):
    # didn't bother with MCTBSLFitter, since this seems redundant. Just set "StraightLine" = True since the kwargs are passed on to MuonChi2TrackFitterCfg  
    result = ComponentAccumulator()
    
    acc = MCTBExtrapolatorCfg(flags)
    mctbExtrapolator = acc.getPrimary()
    result.merge(acc)
    
    kwargs.setdefault("ExtrapolationTool", mctbExtrapolator)
    kwargs.setdefault("GetMaterialFromTrack", True)
    kwargs.setdefault("Momentum", flags.Muon.straightLineFitMomentum)
    
    extra_kwargs = {}
    if 'StraightLine' in kwargs:
      # Pass this on! Can't safely just pass on kwargs, because MuonChi2TrackFitterCfg also has a property ExtrapolationTool
      extra_kwargs.setdefault('StraightLine', kwargs['StraightLine'])
    acc = MuonChi2TrackFitterCfg(flags, name=name, **extra_kwargs)
    mctbfitter = acc.getPrimary()
    result.merge(acc)
    # print mctbfitter
    result.setPrivateTools(mctbfitter)
    return result
