# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
# Configuration of TRT_TrackSegmentsFinder package
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
import AthenaCommon.SystemOfUnits as Units

def TRT_TrackSegmentsFinderCfg(flags, name = 'InDetTRT_TrackSegmentsFinder',
                               extension = '',
                               InputCollections = None,
                               **kwargs):

    from MagFieldServices.MagFieldServicesConfig import (
        AtlasFieldCacheCondAlgCfg)
    acc = AtlasFieldCacheCondAlgCfg(flags)

    if "SegmentsMakerTool" not in kwargs:
        if flags.Beam.Type is BeamType.Cosmics:
            from InDetConfig.TRT_TrackSegmentsToolConfig import (
                TRT_TrackSegmentsMaker_BarrelCosmicsCfg)
            InDetTRT_TrackSegmentsMaker = acc.popToolsAndMerge(
                TRT_TrackSegmentsMaker_BarrelCosmicsCfg(flags, name='InDetTRTSegmentsMaker'+extension))
        else:
            from InDetConfig.TRT_TrackSegmentsToolConfig import (
                TRT_TrackSegmentsMaker_ATLxkCfg)
            InDetTRT_TrackSegmentsMaker = acc.popToolsAndMerge(
                TRT_TrackSegmentsMaker_ATLxkCfg(flags, name = 'InDetTRT_SeedsMaker'+extension, 
                                                extension = extension,
                                                InputCollections = InputCollections))
        kwargs.setdefault("SegmentsMakerTool", InDetTRT_TrackSegmentsMaker)

    if "RoadTool" not in kwargs:
        from InDetConfig.TRT_DetElementsRoadToolConfig import (
            TRT_DetElementsRoadMaker_xkCfg)
        kwargs.setdefault("RoadTool", acc.popToolsAndMerge(
            TRT_DetElementsRoadMaker_xkCfg(flags)))

    if flags.InDet.Tracking.ActiveConfig.RoISeededBackTracking:
        from InDetConfig.InDetCaloClusterROISelectorConfig import (
            CaloClusterROIPhiRZContainerMakerCfg)
        acc.merge(CaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("useCaloSeeds", True)
        kwargs.setdefault("EMROIPhiRZContainer",
                          "InDetCaloClusterROIPhiRZ%.0fGeVUnordered" % \
                          (flags.InDet.Tracking.ActiveConfig.minRoIClusterEt/Units.GeV))

    acc.addEventAlgo(CompFactory.InDet.TRT_TrackSegmentsFinder(name, **kwargs))
    return acc
