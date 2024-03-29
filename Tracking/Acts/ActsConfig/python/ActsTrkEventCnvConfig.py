# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def ActsToTrkConverterToolCfg(flags, name="ActsToTrkConverterTool", **kwargs):
    from ActsConfig.ActsTrkGeometryConfig import ActsTrackingGeometryToolCfg
    result = ComponentAccumulator()
    kwargs.setdefault("TrackingGeometryTool", result.popToolsAndMerge(
        ActsTrackingGeometryToolCfg(flags)))  # PrivateToolHandle
    result.setPrivateTools(
        CompFactory.ActsTrk.ActsToTrkConverterTool(name, **kwargs))
    return result


def TrkToActsConvertorAlgCfg(flags, name="", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ConvertorTool", result.popToolsAndMerge(
        ActsToTrkConverterToolCfg(flags)))  # PrivateToolHandle
    result.addEventAlgo(
        CompFactory.ActsTrk.TrkToActsConvertorAlg(name, **kwargs))
    return result

def RunConversion():
    from AthenaConfiguration.AllConfigFlags import initConfigFlags
    from TrkConfig.TrackCollectionReadConfig import TrackCollectionReadCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    flags = initConfigFlags()
    args = flags.fillFromArgs()

    flags.Input.Files = [
        '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/ESD/ATLAS-P2-RUN4-01-01-00/ESD.ttbar_mu0.pool.root']
    flags.IOVDb.GlobalTag = "OFLCOND-MC15c-SDR-14-05"
    flags.Scheduler.ShowDataDeps = True
    flags.Scheduler.ShowDataFlow = True
    flags.Scheduler.CheckDependencies = True
    flags.lock()
    flags.dump()

    if not args.config_only:
        from AthenaConfiguration.MainServicesConfig import MainServicesCfg
        cfg = MainServicesCfg(flags)
    else:
        cfg = ComponentAccumulator()

    # Set up to read ESD and tracks
    track_collections = ['CombinedITkTracks']
    cfg.merge(PoolReadCfg(flags))
    for collection in track_collections:
        cfg.merge(TrackCollectionReadCfg(flags, collection))

    # Needed to read tracks
    from TrkEventCnvTools.TrkEventCnvToolsConfigCA import TrkEventCnvSuperToolCfg
    cfg.merge(TrkEventCnvSuperToolCfg(flags))

    # Muon geometry not yet in ActsTrackingGeometrySvcCfg
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    cfg.merge(MuonGeoModelCfg(flags))

    # Now setup the convertor
    acc = TrkToActsConvertorAlgCfg(
        flags, OutputLevel=1, TrackCollectionKeys=track_collections)
    cfg.merge(acc)

    # Let's dump the input tracks, and also the output ACTS tracks
    from DumpEventDataToJSON.DumpEventDataToJSONConfig import DumpEventDataToJSONAlgCfg
    acc = DumpEventDataToJSONAlgCfg(
        flags, doExtrap=False, OutputLevel=1,
        TrackCollectionKeys=track_collections,
        CscPrepRawDataKey="",
        MMPrepRawDataKey="",
        sTgcPrepRawDataKey="",
        MdtPrepRawDataKey="",
        RpcPrepRawDataKey="",
        TgcPrepRawDataKey="",
        PixelPrepRawDataKey="",
        SctPrepRawDataKey="",
        TrtPrepRawDataKey="",
        CaloCellContainerKey=[""],
        CaloClusterContainerKeys=[""],
        MuonContainerKeys=[""],
        JetContainerKeys=[""],
        TrackParticleContainerKeys=[""],
        OutputLocation="dump.json",
    )
    print(acc.getEventAlgo('DumpEventDataToJsonAlg'))
    cfg.merge(acc)
    cfg.printConfig(withDetails=True, summariseProps=True)

    if not args.config_only:
        sc = cfg.run()
        if not sc.isSuccess():
            import sys
            sys.exit("Execution failed")


if __name__ == "__main__":
    # To run this, do e.g.
    # python -m ActsTrkEventCnv.ActsTrkEventCnvConfig --threads=1
    RunConversion()
