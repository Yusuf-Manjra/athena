"""Define methods to construct configured RPC Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from RPC_Digitization.RPC_DigitizationConf import RpcDigitizationTool, RPC_Digitizer
from PileUpComps.PileUpCompsConf import PileUpXingFolder
from MuonConfig.MuonByteStreamCnvTestConfig import RpcDigitToRpcRDOCfg, RpcOverlayDigitToRpcRDOCfg
from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg


# The earliest and last bunch crossing times for which interactions will be sent
# to the RpcDigitizationTool.
def RPC_FirstXing():
    return -150


def RPC_LastXing():
    return 125


def RPC_RangeToolCfg(flags, name="RPC_Range", **kwargs):
    """Return a PileUpXingFolder tool configured for RPC"""
    kwargs.setdefault("FirstXing", RPC_FirstXing())
    kwargs.setdefault("LastXing", RPC_LastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0)
    kwargs.setdefault("ItemList", ["RPCSimHitCollection#RPC_Hits"])
    return PileUpXingFolder(name, **kwargs)


def RPC_DigitizationToolCfg(flags, name="RPC_DigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured RpcDigitizationTool"""
    from MuonConfig.MuonCondAlgConfig import RpcCondDbAlgCfg # MT-safe conditions access
    acc = RpcCondDbAlgCfg(flags)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", RPC_FirstXing())
        kwargs.setdefault("LastXing", RPC_LastXing())
    kwargs.setdefault("OutputObjectName", "RPC_DIGITS")
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputSDOName", flags.Overlay.BkgPrefix + "RPC_SDO")
    else:
        kwargs.setdefault("OutputSDOName", "RPC_SDO")
    # config
    kwargs.setdefault("DeadTime", 100)
    kwargs.setdefault("PatchForRpcTime", True)
    # kwargs.setdefault("PatchForRpcTimeShift", 9.6875)
    kwargs.setdefault("PatchForRpcTimeShift", 12.5)
    kwargs.setdefault("turnON_efficiency", True)
    kwargs.setdefault("turnON_clustersize", True)
    kwargs.setdefault("testbeam_clustersize", 0)
    kwargs.setdefault("ClusterSize1_2uncorr", 0)
    kwargs.setdefault("CutProjectedTracks", 100)
    kwargs.setdefault("RPCInfoFromDb", True)
    kwargs.setdefault("Efficiency_fromCOOL", True)
    kwargs.setdefault("EfficiencyPatchForBMShighEta", False)
    kwargs.setdefault("ClusterSize_fromCOOL", True)
    kwargs.setdefault("DumpFromDbFirst", False)
    kwargs.setdefault("PanelId_OFF_fromlist", False)
    kwargs.setdefault("PanelId_OK_fromlist", False)
    kwargs.setdefault("IgnoreRunDependentConfig", False)
    kwargs.setdefault("PrintCalibrationVector",False )
    kwargs.setdefault("PhiAndEtaEff_A",[0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938])
    kwargs.setdefault("OnlyPhiEff_A"  ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022])
    kwargs.setdefault("OnlyEtaEff_A"  ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022])
    kwargs.setdefault("PhiAndEtaEff_C",[0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938])
    kwargs.setdefault("OnlyPhiEff_C"  ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022])
    kwargs.setdefault("OnlyEtaEff_C"  ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022])
    kwargs.setdefault("FracClusterSize1_A",   [0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664])
    kwargs.setdefault("FracClusterSize2_A",   [0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986])
    kwargs.setdefault("FracClusterSizeTail_A",[0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035, 0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035 ])
    kwargs.setdefault("MeanClusterSizeTail_A",[0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598])
    kwargs.setdefault("FracClusterSize1_C",   [0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664])
    kwargs.setdefault("FracClusterSize2_C",   [0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986])
    kwargs.setdefault("FracClusterSizeTail_C",[0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035, 0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035 ])
    kwargs.setdefault("MeanClusterSizeTail_C",[0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598])
    acc.setPrivateTools(RpcDigitizationTool(name, **kwargs))
    return acc


def RPC_OverlayDigitizationToolCfg(flags, name="RPC_DigitizationTool", **kwargs):
    """Return ComponentAccumulator with RpcDigitizationTool configured for Overlay"""
    acc = ComponentAccumulator()
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "RPC_DIGITS")
    if not flags.Overlay.DataOverlay:
        kwargs.setdefault("OutputSDOName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "RPC_SDO")
    acc.setPrivateTools(RpcDigitizationTool(name, **kwargs))
    return acc


def RPC_OutputCfg(flags):
    """Return ComponentAccumulator with Output for RPC. Not standalone."""
    acc = ComponentAccumulator()
    ItemList = ["RpcPadContainer#*"]
    if flags.Digitization.TruthOutput:
        ItemList += ["MuonSimDataCollection#*"]
        acc.merge(TruthDigitizationOutputCfg(flags))
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList))
    return acc


def RPC_DigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for RPC digitization"""
    acc = MuonGeoModelCfg(flags)
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(RPC_DigitizationToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    return acc


def RPC_OverlayDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with RPC Overlay digitization"""
    acc = MuonGeoModelCfg(flags)
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(RPC_OverlayDigitizationToolCfg(flags))
        kwargs["DigitizationTool"] = tool
    acc.addEventAlgo(RPC_Digitizer(**kwargs))
    return acc


# with output defaults
def RPC_DigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for RPC digitization and Output"""
    acc = RPC_DigitizationBasicCfg(flags, **kwargs)
    acc.merge(RPC_OutputCfg(flags))
    return acc


def RPC_OverlayDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator with RPC Overlay digitization and Output"""
    acc = RPC_OverlayDigitizationBasicCfg(flags, **kwargs)
    acc.merge(RPC_OutputCfg(flags))
    return acc


def RPC_DigitizationDigitToRDOCfg(flags):
    """Return ComponentAccumulator with RPC digitization and Digit to RPCPAD RDO"""
    acc = RPC_DigitizationCfg(flags)
    acc.merge(RPCCablingConfigCfg(flags))
    acc.merge(RpcDigitToRpcRDOCfg(flags))
    return acc


def RPC_OverlayDigitizationDigitToRDOCfg(flags):
    """Return ComponentAccumulator with RPC Overlay digitization and Digit to RPCPAD RDO"""
    acc = RPC_OverlayDigitizationCfg(flags)
    acc.merge(RPCCablingConfigCfg(flags))
    acc.merge(RpcOverlayDigitToRpcRDOCfg(flags))
    return acc

