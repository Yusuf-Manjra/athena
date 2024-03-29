# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Adding SiHitValidation for whichever parts of ITk are running
def ITkHitAnalysis(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from HitAnalysis.SiHitAnalysis import ITkPixelHitAnalysisCfg, ITkStripHitAnalysisCfg, PLR_HitAnalysisCfg

    result = ComponentAccumulator()

    if flags.Detector.EnableITkPixel:
        result.merge(ITkPixelHitAnalysisCfg(flags))

    if flags.Detector.EnableITkStrip:
        result.merge(ITkStripHitAnalysisCfg(flags))

    if flags.Detector.EnablePLR:
        result.merge(PLR_HitAnalysisCfg(flags))

    return result

def HGTDHitAnalysis(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from HitAnalysis.SiHitAnalysis import HGTD_HitAnalysisCfg

    result = ComponentAccumulator()

    if flags.Detector.EnableHGTD:
        result.merge(HGTD_HitAnalysisCfg(flags))

    return result
