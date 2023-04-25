# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetSegmentDriftCircleAssValidation package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SegmentDriftCircleAssValidationCfg(
        flags, name="InDetSegmentDriftCircleAssValidation", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("TRT_DriftCirclesName", 'TRT_DriftCircles')
    kwargs.setdefault("Pseudorapidity", 2.1) # end of TRT
    kwargs.setdefault("RadiusMin", 0.)
    kwargs.setdefault("RadiusMax", 600.)
    kwargs.setdefault("pTmin", flags.Tracking.ActiveConfig.minSecondaryPt)
    kwargs.setdefault("MinNumberDCs",
                      flags.Tracking.ActiveConfig.minSecondaryTRTonTrk)

    acc.addEventAlgo(
        CompFactory.InDet.SegmentDriftCircleAssValidation(name, **kwargs))
    return acc

def SegmentDriftCircleAssValidation_TrackSegments_Cfg(
        flags, name="InDetSegmentDriftCircleAssValidation_TrackSegments", **kwargs):
    kwargs.setdefault("MinNumberDCs", flags.Tracking.ActiveConfig.minTRTonly)
    kwargs.setdefault("pTmin", flags.Tracking.ActiveConfig.minPT)
    return SegmentDriftCircleAssValidationCfg(flags, name, **kwargs)
