# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkVertexFitters package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def AdaptiveMultiVertexFitterCfg(flags, name="AdaptiveMultiVertexFitter", **kwargs):
    acc = ComponentAccumulator()

    if "LinearizedTrackFactory" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import (
            FullLinearizedTrackFactoryCfg)
        kwargs.setdefault("LinearizedTrackFactory", acc.popToolsAndMerge(
            FullLinearizedTrackFactoryCfg(flags)))

    if "ImpactPoint3dEstimator" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import (
            ImpactPoint3dEstimatorCfg)
        kwargs.setdefault("ImpactPoint3dEstimator", acc.popToolsAndMerge(
            ImpactPoint3dEstimatorCfg(flags)))

    if "AnnealingMaker" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import DetAnnealingMakerCfg
        kwargs.setdefault("AnnealingMaker", acc.popToolsAndMerge(
            DetAnnealingMakerCfg(flags)))

    kwargs.setdefault("DoSmoothing", True)

    acc.setPrivateTools(CompFactory.Trk.AdaptiveMultiVertexFitter(name, **kwargs))
    return acc

def SequentialVertexSmootherCfg(flags, name="SequentialVertexSmoother", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.SequentialVertexSmoother())
    return acc

def AdaptiveVertexFitterCfg(flags, name="AdaptiveVertexFitter", **kwargs):
    acc = ComponentAccumulator()

    if "SeedFinder" not in kwargs:
        from TrkConfig.TrkVertexSeedFinderToolsConfig import ZScanSeedFinderCfg
        kwargs.setdefault("SeedFinder", acc.popToolsAndMerge(
            ZScanSeedFinderCfg(flags)))

    if "LinearizedTrackFactory" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import (
            FullLinearizedTrackFactoryCfg)
        kwargs.setdefault("LinearizedTrackFactory", acc.popToolsAndMerge(
            FullLinearizedTrackFactoryCfg(flags)))

    if "ImpactPoint3dEstimator" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import (
            ImpactPoint3dEstimatorCfg)
        kwargs.setdefault("ImpactPoint3dEstimator", acc.popToolsAndMerge(
            ImpactPoint3dEstimatorCfg(flags)))

    if "AnnealingMaker" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import DetAnnealingMakerCfg
        kwargs.setdefault("AnnealingMaker", acc.popToolsAndMerge(
            DetAnnealingMakerCfg(flags)))

    if "VertexSmoother" not in kwargs:
        kwargs.setdefault("VertexSmoother", acc.popToolsAndMerge(
            SequentialVertexSmootherCfg(flags)))

    acc.setPrivateTools(CompFactory.Trk.AdaptiveVertexFitter(name, **kwargs))
    return acc

def TauAdaptiveVertexFitterCfg(flags, name="TauAdaptiveVertexFitter", **kwargs):
    acc = ComponentAccumulator()

    if "SeedFinder" not in kwargs:
        from TrkConfig.TrkVertexSeedFinderToolsConfig import (
            CrossDistancesSeedFinderCfg)
        kwargs.setdefault("SeedFinder", acc.popToolsAndMerge(
            CrossDistancesSeedFinderCfg(flags)))

    if "LinearizedTrackFactory" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import (
            AtlasFullLinearizedTrackFactoryCfg)
        kwargs.setdefault("LinearizedTrackFactory", acc.popToolsAndMerge(
            AtlasFullLinearizedTrackFactoryCfg(flags)))

    if "ImpactPoint3dEstimator" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import (
            AtlasImpactPoint3dEstimatorCfg)
        kwargs.setdefault("ImpactPoint3dEstimator", acc.popToolsAndMerge(
            AtlasImpactPoint3dEstimatorCfg(flags)))

    if "AnnealingMaker" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import TauDetAnnealingMakerCfg
        kwargs.setdefault("AnnealingMaker", acc.popToolsAndMerge(
            TauDetAnnealingMakerCfg(flags)))

    if "VertexSmoother" not in kwargs:
        kwargs.setdefault("VertexSmoother", acc.popToolsAndMerge(
            SequentialVertexSmootherCfg(flags)))

    acc.setPrivateTools(CompFactory.Trk.AdaptiveVertexFitter(name, **kwargs))
    return acc

def AdaptiveVxFitterToolIncSecVtxCfg(flags, name='AdaptiveVxFitterToolIncSecVtx', **kwargs):

    kwargs.setdefault("MaxIterations", 8000)
    kwargs.setdefault("MaxDistToLinPoint", 0.2)
    kwargs.setdefault("InitialError", 0.2)
    
    return AdaptiveVertexFitterCfg(flags, name, **kwargs)
