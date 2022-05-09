# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Configuration of tools shared between Segment Finding and Track Building

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType


def MuonEDMPrinterToolCfg(flags, name="MuonEDMPrinterTool", **kwargs):
    result = ComponentAccumulator()  
    kwargs.setdefault('TgcPrdCollection', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    the_tool = CompFactory.Muon.MuonEDMPrinterTool(name, **kwargs)
    result.setPrivateTools(the_tool)
    result.merge(MuonEDMHelperSvcCfg(flags))
    from MuonConfig.MuonGeometryConfig import MuonIdHelperSvcCfg   
    result.merge(MuonIdHelperSvcCfg(flags))
    return result
    
def MuonEDMHelperSvcCfg(flags, name = "MuonEDMHelperSvc"):
    result = ComponentAccumulator()
    flags.Beam.Type ### Dummy call
    result.addService(CompFactory.Muon.MuonEDMHelperSvc(name), primary = True)
    return result
       
def MuonTrackToSegmentToolCfg(flags,name="MuonTrackToSegmentTool", **kwargs):
    Muon__MuonTrackToSegmentTool=CompFactory.Muon.MuonTrackToSegmentTool
    #MDT conditions information not available online
    result = ComponentAccumulator()
    from MuonConfig.MuonCondAlgConfig import MuonStationIntersectCondAlgCfg
    result.merge(MuonStationIntersectCondAlgCfg(flags))

    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
    atlasRungeKuttaPropagator = result.popToolsAndMerge(RungeKuttaPropagatorCfg(flags))
    result.addPublicTool(atlasRungeKuttaPropagator)
    kwargs.setdefault("Propagator",atlasRungeKuttaPropagator)
    
    # Not bothering to explicitly set IdHelper or EDMHelper    
    muon_track_to_segment_tool = Muon__MuonTrackToSegmentTool(name, **kwargs)
    result.setPrivateTools(muon_track_to_segment_tool)
    return result

def MuonHitSummaryToolCfg(flags, name="MuonHitSummaryTool", **kwargs):
    result = MuonTrackSummaryHelperToolCfg(flags)   
    kwargs.setdefault('MuonTrackSummaryHelperTool', result.getPrimary())
    kwargs.setdefault('Printer', result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags)))
    the_tool = CompFactory.Muon.MuonHitSummaryTool(name, **kwargs)
    result.setPrivateTools(the_tool)
    return result

def MuonSeededSegmentFinderCfg(flags,name="MuonSeededSegmentFinder", **kwargs):
    Muon__MuonSeededSegmentFinder=CompFactory.Muon.MuonSeededSegmentFinder
    from MuonConfig.MuonSegmentFindingConfig import DCMathSegmentMakerCfg, MdtMathSegmentFinderCfg # FIXME - should really shift this to RecTools then.
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg
    result = ComponentAccumulator()
    
    mdt_segment_finder = result.popToolsAndMerge( MdtMathSegmentFinderCfg(flags, name="MCTBMdtMathSegmentFinder", UseChamberTheta = False, AssociationRoadWidth = 1.5) )
    result.addPublicTool(mdt_segment_finder)
    
    if "SegmentMaker" not in kwargs or "SegmentMakerNoHoles" not in kwargs:
        seg_maker=""
        if flags.Beam.Type is BeamType.Collisions:
            seg_maker = result.popToolsAndMerge ( DCMathSegmentMakerCfg( flags, name = "MCTBDCMathSegmentMaker", MdtSegmentFinder = mdt_segment_finder, SinAngleCut = 0.04, DoGeometry = True))
        else:  # cosmics or singlebeam
            seg_maker = result.popToolsAndMerge (DCMathSegmentMakerCfg( flags, name = "MCTBDCMathSegmentMaker", MdtSegmentFinder = mdt_segment_finder, SinAngleCut = 0.1,  DoGeometry = False, AddUnassociatedPhiHits= True ))
        kwargs.setdefault("SegmentMaker", seg_maker)
        kwargs.setdefault("SegmentMakerNoHoles", seg_maker) #FIXME. Just remove one.
    if not flags.Detector.GeometryCSC:
        kwargs.setdefault("CscPrepDataContainer","")
    if not flags.Detector.GeometrysTGC:
        kwargs.setdefault("sTgcPrepDataContainer","")
    if not flags.Detector.GeometryMM:
        kwargs.setdefault("MMPrepDataContainer","")
    
    kwargs.setdefault("Printer", result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags)) )
    driftCreator = result.popToolsAndMerge (MdtDriftCircleOnTrackCreatorCfg(flags))
    kwargs.setdefault("MdtRotCreator", driftCreator)
  
    kwargs.setdefault('TgcPrepDataContainer', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    
    muon_seeded_segment_finder = Muon__MuonSeededSegmentFinder(name, **kwargs)
    result.setPrivateTools(muon_seeded_segment_finder)
    return result
        
        
def MuonSegmentMomentumFromFieldCfg(flags, name="MuonSegmentMomentumFromField", **kwargs):
    MuonSegmentMomentumFromField=CompFactory.MuonSegmentMomentumFromField
    result = ComponentAccumulator()
    
    from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasNavigatorCfg
    navigator = result.popToolsAndMerge (AtlasNavigatorCfg(flags))
    result.addPublicTool(navigator)
    kwargs.setdefault("NavigatorTool", navigator)
    
    from TrkConfig.TrkExSTEP_PropagatorConfig import AtlasSTEP_PropagatorCfg
    muon_prop =  result.popToolsAndMerge(AtlasSTEP_PropagatorCfg(flags, name="MuonSTEP_Propagator"))
    kwargs.setdefault("PropagatorTool", muon_prop)
        
    muon_seg_mom_from_field = MuonSegmentMomentumFromField(name=name, **kwargs)
    result.setPrivateTools(muon_seg_mom_from_field)
    return result
    
def MuonTrackSummaryHelperToolCfg(flags, name="MuonTrackSummaryHelperTool", **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    
    result = AtlasExtrapolatorCfg(flags)
    extrap = result.getPrimary()
    result.addPublicTool(extrap)
    kwargs.setdefault("Extrapolator", extrap)

    kwargs.setdefault("CalculateCloseHits", True)

    Muon__MuonTrackSummaryHelperTool=CompFactory.Muon.MuonTrackSummaryHelperTool
    result.setPrivateTools(Muon__MuonTrackSummaryHelperTool(name=name,**kwargs))
    return result

def MuonTrackSummaryToolCfg(flags, name="MuonTrackSummaryTool", **kwargs):
    Trk__TrackSummaryTool=CompFactory.Trk.TrackSummaryTool    
    result = ComponentAccumulator()
    track_summary_helper = result.popToolsAndMerge(MuonTrackSummaryHelperToolCfg(flags))
    kwargs.setdefault("MuonSummaryHelperTool", track_summary_helper )
    kwargs.setdefault("doSharedHits", False )
    kwargs.setdefault("AddDetailedMuonSummary", True )
    result.setPrivateTools(Trk__TrackSummaryTool(name=name,**kwargs))
    return result

def MuonTrackScoringToolCfg(flags, name="MuonTrackScoringTool", **kwargs):
    Muon__MuonTrackScoringTool=CompFactory.Muon.MuonTrackScoringTool
    # m_trkSummaryTool("Trk::TrackSummaryTool"),    
    result = ComponentAccumulator()
    track_summary = result.getPrimaryAndMerge(MuonTrackSummaryToolCfg(flags)) 
    kwargs.setdefault('SumHelpTool', track_summary)
    kwargs.setdefault("EDMPrinter", result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags)) )
    result.setPrivateTools(Muon__MuonTrackScoringTool(name=name,**kwargs))
    return result

def MuonAmbiProcessorCfg(flags, name="MuonAmbiProcessor", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault('DropDouble', False)
    scoring_tool =  result.getPrimaryAndMerge(MuonTrackScoringToolCfg( flags ))
    kwargs.setdefault('ScoringTool', scoring_tool )
    result.addPublicTool(scoring_tool)
    muon_edm_printer = result.getPrimaryAndMerge(MuonEDMPrinterToolCfg( flags ))
    # muon_edm_printer = result.getPrimaryAndMerge(MuonEDMPrinterToolCfg( flags, TgcPrdCollection="TGC_Measurements" )) # FIXME Hack to get wrapping working. Keep in for now, to aid debugging

    muon_ami_selection_tool = CompFactory.Muon.MuonAmbiTrackSelectionTool(name="MuonAmbiSelectionTool", Printer=muon_edm_printer)
    result.addPublicTool(muon_ami_selection_tool)
    kwargs.setdefault('SelectionTool', muon_ami_selection_tool)
    result.setPrivateTools(CompFactory.Trk.TrackSelectionProcessorTool(name=name,**kwargs))
    return result

def MuonTrackCleanerCfg(flags, name="MuonTrackCleaner", **kwargs):
    Muon__MuonTrackCleaner=CompFactory.Muon.MuonTrackCleaner
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg, TriggerChamberClusterOnTrackCreatorCfg
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

    result=ComponentAccumulator()
    
    mdt_creator= result.getPrimaryAndMerge(MdtDriftCircleOnTrackCreatorCfg(flags))
    kwargs.setdefault("MdtRotCreator", mdt_creator)
    
    muon_cluster_creator= result.getPrimaryAndMerge(TriggerChamberClusterOnTrackCreatorCfg(flags))
    kwargs.setdefault("CompRotCreator", muon_cluster_creator)
    
    # For PullCalculator, just let it get default for moment.
    
    extrapolator = result.getPrimaryAndMerge(AtlasExtrapolatorCfg(flags)) 
    kwargs.setdefault("Extrapolator", extrapolator)

    slfitter = result.getPrimaryAndMerge(MCTBSLFitterMaterialFromTrackCfg(flags))
    kwargs.setdefault("SLFitter", slfitter)

    fitter = result.getPrimaryAndMerge(MCTBFitterCfg(flags, name = "MCTBFitterMaterialFromTrack", GetMaterialFromTrack=True))
    kwargs.setdefault("Fitter", fitter)

    kwargs.setdefault("Printer", result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags)) )
    # kwargs.setdefault("Printer", result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags, TgcPrdCollection="TGC_Measurements" )) ) # FIXME Hack to get wrapping working. Keep in for now, to aid debugging

    kwargs.setdefault("MaxAvePullSumPerChamber", 6)
    kwargs.setdefault("Chi2Cut", flags.Muon.Chi2NDofCut)
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("Iterate", False)
        kwargs.setdefault("RecoverOutliers", False)

    # FIXME - do remaining tools

    
    result.setPrivateTools(Muon__MuonTrackCleaner(name, **kwargs))
    
    return result 

def MuonChi2TrackFitterCfg(flags, name='MuonChi2TrackFitter', **kwargs):    
    result = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import MuonExtrapolatorCfg
    extrapolator= result.popToolsAndMerge(MuonExtrapolatorCfg(flags))
    from TrkConfig.TrkRIO_OnTrackCreatorConfig import MuonRotCreatorCfg
    rotcreator=result.popToolsAndMerge(MuonRotCreatorCfg(flags))
    
    from TrkConfig.TrkMeasurementUpdatorConfig import KalmanUpdatorCfg
    measurement_updater = result.popToolsAndMerge(KalmanUpdatorCfg(flags, name="MuonMeasUpdator"))
    

    kwargs.setdefault("ExtrapolationTool"    , extrapolator)
    kwargs.setdefault("RotCreatorTool"       , rotcreator)
    kwargs.setdefault("MeasurementUpdateTool", measurement_updater)
    kwargs.setdefault("StraightLine"         , False)
    kwargs.setdefault("OutlierCut"           , 3.0)
    kwargs.setdefault("GetMaterialFromTrack" , False)
    kwargs.setdefault("RejectLargeNScat"     , True)

    # take propagator and navigator from the extrapolator
    kwargs.setdefault("PropagatorTool", extrapolator.Propagators[0] if len(extrapolator.Propagators)>0 else "")
    kwargs.setdefault("NavigatorTool",  extrapolator.Navigator)
    kwargs.setdefault("EnergyLossTool", extrapolator.EnergyLossUpdators[0] if len(extrapolator.EnergyLossUpdators)>0 else "" )
    ### We need to include the tracking geometry conditions 
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    geom_cond_key = result.getPrimaryAndMerge(TrackingGeometryCondAlgCfg(flags)).TrackingGeometryWriteKey
    kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)
    fitter = CompFactory.Trk.GlobalChi2Fitter(name=name, **kwargs)
    result.setPrivateTools(fitter)
    # print fitter
    return result

def MCTBSLFitterMaterialFromTrackCfg(flags, name='MCTBSLFitterMaterialFromTrack', **kwargs):
    result = ComponentAccumulator()
    kwargs["StraightLine"] = True       # always set
    kwargs["GetMaterialFromTrack"]=True # always set
    from TrkConfig.AtlasExtrapolatorConfig import MuonStraightLineExtrapolatorCfg
    extrapolator = result.popToolsAndMerge(MuonStraightLineExtrapolatorCfg(flags))
    result.addPublicTool(extrapolator)
    kwargs.setdefault("ExtrapolationTool", extrapolator)

    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
    propagator = result.popToolsAndMerge(RungeKuttaPropagatorCfg(flags, name="MuonRK_Propagator"))
    kwargs["PropagatorTool"]=propagator

    result.setPrivateTools(result.popToolsAndMerge(MCTBFitterCfg(flags, name, **kwargs)))
    return result

def MCTBFitterCfg(flags, name='MCTBFitter', **kwargs):
    # didn't bother with MCTBSLFitter, since this seems redundant. Just set "StraightLine" = True since the kwargs are passed on to MuonChi2TrackFitterCfg  
    # Ditto with MCTBFitterMaterialFromTrack. Just set "GetMaterialFromTrack" = True
    result = ComponentAccumulator()
    
    from TrkConfig.AtlasExtrapolatorConfig import MCTBExtrapolatorCfg
    mctbExtrapolator = result.popToolsAndMerge (MCTBExtrapolatorCfg(flags))
    
    kwargs.setdefault("ExtrapolationTool", mctbExtrapolator)
    kwargs.setdefault("GetMaterialFromTrack", True)
    kwargs.setdefault("Momentum", flags.Muon.straightLineFitMomentum)    
    mctbfitter = result.popToolsAndMerge(MuonChi2TrackFitterCfg(flags, name=name, **kwargs)) 
    result.setPrivateTools(mctbfitter)
    return result

def MuonPhiHitSelector(flags, name="MuonPhiHitSelector",**kwargs):
    MuonPhiHitSelector=CompFactory.MuonPhiHitSelector
    kwargs.setdefault("MakeClusters", True)
    kwargs.setdefault("CompetingRios", True)
    kwargs.setdefault("DoCosmics", flags.Beam.Type is BeamType.Cosmics)

    return MuonPhiHitSelector(name,**kwargs)


def MuPatHitToolCfg(flags, name="MuPatHitTool",**kwargs):
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg
    result = MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_creator=result.getPrimary()
    kwargs.setdefault("MdtRotCreator", mdt_creator)

    if flags.Detector.GeometryCSC:
        from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import CscClusterOnTrackCreatorCfg
        kwargs.setdefault("CscRotCreator", result.popToolsAndMerge(CscClusterOnTrackCreatorCfg(flags)))
    else:
        kwargs.setdefault("CscRotCreator", "")

    result.setPrivateTools(CompFactory.Muon.MuPatHitTool(name,**kwargs))
    return result


def MuonTrackExtrapolationToolCfg(flags, name="MuonTrackExtrapolationTool", **kwargs):
    # FIXME - it seems like this tool needs a lot of configuration still.
    # But perhaps it can be simplified first?
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg, MuonExtrapolatorCfg
    result = ComponentAccumulator()
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    condAlg = result.getPrimaryAndMerge(TrackingGeometryCondAlgCfg(flags))
    geom_cond_key = condAlg.TrackingGeometryWriteKey
    kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)
    kwargs.setdefault("Cosmics", flags.Beam.Type is BeamType.Cosmics)

    kwargs.setdefault("AtlasExtrapolator", result.popToolsAndMerge( AtlasExtrapolatorCfg(flags) ) )
    kwargs.setdefault("MuonExtrapolator",  result.popToolsAndMerge( MuonExtrapolatorCfg(flags) ) )
    kwargs.setdefault("MuonExtrapolator2", result.popToolsAndMerge( MuonExtrapolatorCfg(flags) ) ) # Reported in ATLASRECTS-6658
    kwargs.setdefault('EDMPrinter', result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags)))

    result.setPrivateTools(
        CompFactory.Muon.MuonTrackExtrapolationTool(name, **kwargs))
    return result

def MuonRefitToolCfg(flags, name="MuonRefitTool", **kwargs):
    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import MdtDriftCircleOnTrackCreatorCfg, TriggerChamberClusterOnTrackCreatorCfg

    # FIXME - many tools are not yet explicitly configured here.
    result = ComponentAccumulator()
    kwargs.setdefault("Fitter", 
                      result.popToolsAndMerge(MCTBFitterCfg(flags, name = "MCTBFitterMaterialFromTrack", GetMaterialFromTrack=True)))
    from TrkConfig.AtlasExtrapolatorConfig import MuonExtrapolatorCfg
    kwargs.setdefault("MuonExtrapolator", result.popToolsAndMerge( MuonExtrapolatorCfg(flags) ) )
    kwargs.setdefault("MdtRotCreator", result.popToolsAndMerge( MdtDriftCircleOnTrackCreatorCfg(flags) ) )
    kwargs.setdefault("CompClusterCreator", result.popToolsAndMerge( TriggerChamberClusterOnTrackCreatorCfg(flags) ) )
    kwargs.setdefault("MuonEntryExtrapolationTool", result.popToolsAndMerge(MuonTrackExtrapolationToolCfg(flags)) )
    kwargs.setdefault('Printer', result.getPrimaryAndMerge(MuonEDMPrinterToolCfg(flags)))

    if flags.IOVDb.DatabaseInstance == 'COMP200' or \
                'HLT'  in flags.IOVDb.GlobalTag or flags.Common.isOnline or flags.Muon.MuonTrigger:
        kwargs["AlignmentErrorTool"] = None
    result.setPrivateTools(CompFactory.Muon.MuonRefitTool(name, **kwargs))
    return result

