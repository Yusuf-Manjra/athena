# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging


def GSFTrackSummaryToolCfg(flags,
                           name="GSFTrackSummaryTool",
                           **kwargs):
    """ The Track Summary for the GSF refitted Tracks/TrackParticles"""

    acc = ComponentAccumulator()

    if "PixelToTPIDTool" not in kwargs:
        kwargs["PixelToTPIDTool"] = CompFactory.InDet.PixelToTPIDTool(
            name="GSFBuildPixelToTPIDTool")

    # TODO what happens to
    # ClusterSplitProbabilityName=
    # TrackingCommon.combinedClusterSplitProbName() ? hard-coded for the time being, so do as std config
    if "InDetSummaryHelperTool" not in kwargs:
        from InDetConfig.TrackingCommonConfig import (
            InDetRecTestBLayerToolCfg)
        testBLTool = acc.popToolsAndMerge(
            InDetRecTestBLayerToolCfg(
                flags,
                name="GSFBuildTestBLayerTool"))

        from InDetConfig.InDetRecToolConfig import (
            InDetTrackSummaryHelperToolCfg)
        kwargs["InDetSummaryHelperTool"] = acc.getPrimaryAndMerge(
            InDetTrackSummaryHelperToolCfg(
                flags,
                name="GSFBuildTrackSummaryHelperTool",
                HoleSearch=None,
                AssoTool=None,
                PixelToTPIDTool=kwargs["PixelToTPIDTool"],
                TestBLayerTool=testBLTool,
                ClusterSplitProbabilityName='InDetTRT_SeededAmbiguityProcessorSplitProb'
            ))

    if "TRT_ElectronPidTool" not in kwargs:
        from InDetConfig.TRT_ElectronPidToolsConfig import (
            TRT_ElectronPidToolCfg)
        kwargs["TRT_ElectronPidTool"] = acc.popToolsAndMerge(
            TRT_ElectronPidToolCfg(
                flags,
                name="GSFBuildTRT_ElectronPidTool",
                CalculateNNPid=False,
                MinimumTrackPtForNNPid=0.))

    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", False)

    summaryTool = CompFactory.Trk.TrackSummaryTool(name, **kwargs)
    acc.setPrivateTools(summaryTool)
    return acc


def EMBremCollectionBuilderCfg(flags,
                               name="EMBremCollectionBuilder",
                               **kwargs):
    """ Algorithm to create the GSF collection
        We need to refit with GSF
        Create a Track Summary
        Create the relevant TrackParticles """

    acc = ComponentAccumulator()

    if "TrackRefitTool" not in kwargs:
        from egammaTrackTools.egammaTrackToolsConfig import (
            egammaTrkRefitterToolCfg)
        kwargs["TrackRefitTool"] = acc.popToolsAndMerge(
            egammaTrkRefitterToolCfg(flags))

    if "TrackParticleCreatorTool" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackToVertexCfg
        gsfTrackParticleCreatorTool = CompFactory.Trk.TrackParticleCreatorTool(
            name="GSFBuildInDetParticleCreatorTool",
            KeepParameters=True,
            TrackToVertex=acc.popToolsAndMerge(TrackToVertexCfg(flags)),
            UseTrackSummaryTool=False,
            BadClusterID=0)
        kwargs["TrackParticleCreatorTool"] = gsfTrackParticleCreatorTool

    if "TrackSlimmingTool" not in kwargs:
        slimmingTool = CompFactory.Trk.TrackSlimmingTool(
            name="GSFBuildInDetTrackSlimmingTool",
            KeepParameters=False,
            KeepOutliers=True)
        kwargs["TrackSlimmingTool"] = slimmingTool

    if "TrackSummaryTool" not in kwargs:
        kwargs["TrackSummaryTool"] = acc.popToolsAndMerge(
            GSFTrackSummaryToolCfg(flags))

    kwargs.setdefault("usePixel", flags.Detector.EnablePixel)
    kwargs.setdefault("useSCT", flags.Detector.EnableSCT)
    kwargs.setdefault("DoTruth", flags.Input.isMC)

    alg = CompFactory.EMBremCollectionBuilder(name, **kwargs)
    acc.addEventAlgo(alg)
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO
    flags.lock()
    acc = MainServicesCfg(flags)
    acc.merge(EMBremCollectionBuilderCfg(flags))
    mlog = logging.getLogger("EMBremCollectionBuilderConfigTest")
    mlog.info("Configuring  EMBremCollectionBuilder: ")
    printProperties(mlog,
                    acc.getEventAlgo("EMBremCollectionBuilder"),
                    nestLevel=1,
                    printDefaults=True)
    with open("embremcollectionbuilder.pkl", "wb") as f:
        acc.store(f)
