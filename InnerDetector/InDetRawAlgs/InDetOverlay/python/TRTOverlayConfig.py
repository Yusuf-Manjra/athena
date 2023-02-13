"""Define methods to construct configured TRT overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TRTDataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with TRT data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConfig import TRTOverlayRawDataProviderAlgCfg
    acc.merge(TRTOverlayRawDataProviderAlgCfg(flags))

    return acc


def TRTOverlayAlgCfg(flags, name="TRTOverlay", **kwargs):
    """Return a ComponentAccumulator for TRTOverlay algorithm"""
    acc = ComponentAccumulator()
    from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
    acc.merge(TRT_ReadoutGeometryCfg(flags))

    kwargs.setdefault("SortBkgInput", flags.Overlay.DataOverlay)
    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}TRT_RDOs")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}TRT_RDOs")
    kwargs.setdefault("SignalInputSDOKey", f"{flags.Overlay.SigPrefix}TRT_SDO_Map")
    kwargs.setdefault("OutputKey", "TRT_RDOs")

    if not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'TRT_RDO_Container#{kwargs["BkgInputKey"]}']))

    # HT hit correction fraction
    kwargs.setdefault("TRT_HT_OccupancyCorrectionBarrel", 0.110)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionEndcap", 0.090)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionBarrelNoE", 0.060)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionEndcapNoE", 0.050)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionBarrelAr", 0.040)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionEndcapAr", 0.041)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionBarrelArNoE", 0.033)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionEndcapArNoE", 0.042)

    from InDetConfig.TRT_ElectronPidToolsConfig import TRT_OverlayLocalOccupancyCfg
    kwargs.setdefault("TRT_LocalOccupancyTool", acc.popToolsAndMerge(TRT_OverlayLocalOccupancyCfg(flags)))

    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)

    # Do TRT overlay
    acc.addEventAlgo(CompFactory.TRTOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "TRT_RDO_Container#TRT_RDOs"
        ]))

        if flags.Overlay.DataOverlay:
            acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
                "TRT_BSErrContainer#TRT_ByteStreamErrs"
            ]))
  
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"TRT_RDO_Container#{flags.Overlay.SigPrefix}TRT_RDOs"
        ]))

    if flags.Overlay.doTrackOverlay:
    #for track overlay, write out the signal RDOs because reco tracking will only run on them
            acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            f"TRT_RDO_Container#{flags.Overlay.SigPrefix}TRT_RDOs"]))

    return acc


def TRTTruthOverlayCfg(flags, name="TRTSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the TRT SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background TRT SDOs for data overlay
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}TRT_SDO_Map")

    if kwargs["BkgInputKey"]:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'InDetSimDataCollection#{kwargs["BkgInputKey"]}']))

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}TRT_SDO_Map")
    kwargs.setdefault("OutputKey", "TRT_SDO_Map")

    # Do TRT truth overlay
    acc.addEventAlgo(CompFactory.InDetSDOOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDetSimDataCollection#TRT_SDO_Map"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"InDetSimDataCollection#{flags.Overlay.SigPrefix}TRT_SDO_Map"
        ]))

    return acc


def TRTOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for TRT overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(TRTDataOverlayExtraCfg(flags))

    # Add TRT overlay digitization algorithm
    from TRT_Digitization.TRT_DigitizationConfig import TRT_OverlayDigitizationBasicCfg
    acc.merge(TRT_OverlayDigitizationBasicCfg(flags))
    # Add TRT overlay algorithm
    acc.merge(TRTOverlayAlgCfg(flags))
    # Add TRT truth overlay
    if flags.Digitization.EnableTruth:
        acc.merge(TRTTruthOverlayCfg(flags))

    return acc
