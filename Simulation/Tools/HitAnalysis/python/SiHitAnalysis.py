# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def SiHitAnalysisOutputCfg(flags):
    acc = ComponentAccumulator()

    svc = CompFactory.THistSvc(name="THistSvc")
    svc.Output = ["SiHitAnalysis DATAFILE='SiHitAnalysis.root' OPT='RECREATE'"]
    acc.addService(svc)

    return acc


def ITkPixelHitAnalysisCfg(flags):
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelSimulationGeometryCfg
    acc = ITkPixelSimulationGeometryCfg(flags)

    alg = CompFactory.SiHitAnalysis('ITkPixelHitAnalysis')
    alg.CollectionName = 'ITkPixelHits'
    alg.HistPath='/SiHitAnalysis/Histos/'
    alg.NtupleFileName='/SiHitAnalysis/Ntuples/'
    acc.addEventAlgo(alg)

    acc.merge(SiHitAnalysisOutputCfg(flags))

    return acc


def ITkStripHitAnalysisCfg(flags):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripSimulationGeometryCfg
    acc = ITkStripSimulationGeometryCfg(flags)

    alg = CompFactory.SiHitAnalysis('ITkStripHitAnalysis')
    alg.CollectionName = 'ITkStripHits'
    alg.HistPath='/SiHitAnalysis/Histos/'
    alg.NtupleFileName='/SiHitAnalysis/Ntuples/'
    acc.addEventAlgo(alg)

    acc.merge(SiHitAnalysisOutputCfg(flags))

    return acc


def HGTD_HitAnalysisCfg(flags):
    from HGTD_GeoModel.HGTD_GeoModelConfig import HGTD_GeometryCfg
    acc = HGTD_GeometryCfg(flags)

    alg = CompFactory.SiHitAnalysis('HGTD_HitAnalysis')
    alg.CollectionName = 'HGTD_Hits'
    alg.HistPath='/SiHitAnalysis/Histos/'
    alg.NtupleFileName='/SiHitAnalysis/Ntuples/'
    acc.addEventAlgo(alg)

    acc.merge(SiHitAnalysisOutputCfg(flags))

    return acc


def SiHitAnalysisCfg(flags):
    acc = ComponentAccumulator()

    if flags.Detector.EnableITkPixel:
        acc.merge(ITkPixelHitAnalysisCfg(flags))
    
    if flags.Detector.EnableITkStrip:
        acc.merge(ITkStripHitAnalysisCfg(flags))

    if flags.Detector.EnableHGTD:
        acc.merge(HGTD_HitAnalysisCfg(flags))

    return acc
