# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetSegmentDriftCircleAssValidation package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SegmentDriftCircleAssValidationCfg(flags, name = "InDetSegmentDriftCircleAssValidation",
                                       extension = '',
                                       **kwargs):
    acc = ComponentAccumulator()

    #
    # --- cut values
    #
    if extension == "_TRT":
        # TRT Subdetector segment finding
        MinNumberDCs = flags.InDet.Tracking.ActiveConfig.minTRTonly
        pTmin        = flags.InDet.Tracking.ActiveConfig.minPT
    else:
        # TRT-only/back-tracking segment finding
        MinNumberDCs = flags.InDet.Tracking.ActiveConfig.minSecondaryTRTonTrk
        pTmin        = flags.InDet.Tracking.ActiveConfig.minSecondaryPt

    kwargs.setdefault("TRT_DriftCirclesName", 'TRT_DriftCircles')
    kwargs.setdefault("pTmin", pTmin)
    kwargs.setdefault("Pseudorapidity", 2.1) # end of TRT
    kwargs.setdefault("RadiusMin", 0.)
    kwargs.setdefault("RadiusMax", 600.)
    kwargs.setdefault("MinNumberDCs", MinNumberDCs)

    acc.addEventAlgo(CompFactory.InDet.SegmentDriftCircleAssValidation(name, **kwargs))
    return acc
