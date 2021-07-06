"""Define functions for LAr Digitization with ComponentAccumulator

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
# utilities
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
# for PileUpTool
from LArGeoAlgsNV.LArGMConfig import LArGMCfg
from LArRecUtils.LArADC2MeVCondAlgConfig import LArADC2MeVCondAlgCfg
from LArRecUtils.LArRecUtilsConfig import LArAutoCorrNoiseCondAlgCfg
from LArBadChannelTool.LArBadChannelConfig import LArBadFebCfg,LArBadChannelCfg
from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg
# for Digitization
from LArROD.LArRawChannelBuilderAlgConfig import LArRawChannelBuilderAlgCfg
from LArROD.LArDigitThinnerConfig import LArDigitThinnerCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
# for Trigger Tower
from CaloConditions.CaloConditionsConfig import CaloTriggerTowerCfg
from SGComps.AddressRemappingConfig import InputRenameCfg, InputOverwriteCfg

def useLArFloat(flags):
    """Return bool for simplified transient LArHit with float E,time"""
    # temporary, remapping to LArHitFloat does not seeem to work
    # with this scheme... => larger memory usage
    if flags.Digitization.DoXingByXingPileUp or flags.Common.ProductionStep == ProductionStep.Overlay:
        return False
    # check for fast chain, running digitisation from hits in memory
    if flags.Sim.DoFullChain:
        return False
    return True


def LArRangeEMCfg(flags, name="LArRangeEM", **kwargs):
    """Return a PileUpXingFoldertool for ElectroMagnetic calorimeter"""
    # bunch crossing range in ns
    kwargs.setdefault("FirstXing", -751)
    kwargs.setdefault("LastXing", 101)
    if useLArFloat(flags):
        kwargs.setdefault("ItemList", ["LArHitFloatContainer#LArHitEMB",
                                       "LArHitFloatContainer#LArHitEMEC"])
    else:
        kwargs.setdefault("ItemList", ["LArHitContainer#LArHitEMB",
                                       "LArHitContainer#LArHitEMEC"])
    PileUpXingFolder = CompFactory.PileUpXingFolder
    return PileUpXingFolder(name, **kwargs)


def LArRangeHECCfg(flags, name="LArRangeHEC", **kwargs):
    """Return a PileUpXingFoldertool for Hadronic End-Cap"""
    # bunch crossing range in ns
    kwargs.setdefault("FirstXing", -676)
    kwargs.setdefault("LastXing", 101)
    if useLArFloat(flags):
        kwargs.setdefault("ItemList", ["LArHitFloatContainer#LArHitHEC"])
    else:
        kwargs.setdefault("ItemList", ["LArHitContainer#LArHitHEC"])
    PileUpXingFolder = CompFactory.PileUpXingFolder
    return PileUpXingFolder(name, **kwargs)


def LArRangeFCALCfg(flags, name="LArRangeFCAL", **kwargs):
    """Return a PileUpXingFoldertool for Forward Calorimeters"""
    # bunch crossing range in ns
    kwargs.setdefault("FirstXing", -551)
    kwargs.setdefault("LastXing", 101)
    if useLArFloat(flags):
        kwargs.setdefault("ItemList", ["LArHitFloatContainer#LArHitFCAL"])
    else:
        kwargs.setdefault("ItemList", ["LArHitContainer#LArHitFCAL"])
    PileUpXingFolder = CompFactory.PileUpXingFolder
    return PileUpXingFolder(name, **kwargs)


def LArPileUpToolCfg(flags, name="LArPileUpTool", **kwargs):
    """Return ComponentAccumulator with configured LArPileUpTool"""
    acc = LArGMCfg(flags)

    #The LArPileupTool needs: Noise, fSampl, Pedestal,Shape ADC2MeV
    # AutoCorrNoise, the list of bad FEBs and the cabling
    acc.merge(LArADC2MeVCondAlgCfg(flags))
    acc.merge(LArBadFebCfg(flags))
    acc.merge(LArBadChannelCfg(flags))
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("ShapeKey", "LArShape")
    if not flags.Digitization.DoCaloNoise:
        requiredConditons=["fSampl", "Pedestal", "Shape"]
    else:
        requiredConditons=["Noise", "fSampl", "Pedestal", "Shape"]
    acc.merge(LArElecCalibDbCfg(flags,requiredConditons))

    if flags.Common.ProductionStep != ProductionStep.Overlay:
        acc.merge(LArAutoCorrNoiseCondAlgCfg(flags))

    if "ProblemsToMask" not in kwargs:
        kwargs["ProblemsToMask"] = ["deadReadout", "deadPhys"]
    # defaults
    kwargs.setdefault("NoiseOnOff", flags.Digitization.DoCaloNoise)
    kwargs.setdefault("DoDigiTruthReconstruction", flags.Digitization.DoDigiTruth)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", -751)
        kwargs.setdefault("LastXing", 101)
    if (not flags.Digitization.HighGainFCal) and (flags.Common.ProductionStep != ProductionStep.Overlay):
        kwargs.setdefault("HighGainThreshFCAL", 0)
    if (not flags.Digitization.HighGainEMECIW) and (flags.Common.ProductionStep != ProductionStep.Overlay):
        kwargs.setdefault("HighGainThreshEMECIW", 0)
    kwargs.setdefault("RndmEvtOverlay", flags.Common.ProductionStep == ProductionStep.Overlay)
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("DigitContainer", flags.Overlay.BkgPrefix + "LArDigitContainer_MC")
    else:
        kwargs.setdefault("DigitContainer", "LArDigitContainer_MC") # FIXME - should not be hard-coded
    # if doing MC+MC overlay
    if flags.Common.ProductionStep == ProductionStep.Overlay and flags.Input.isMC:
          kwargs.setdefault("isMcOverlay", True)
    kwargs.setdefault("Nsamples", flags.LAr.ROD.nSamples)
    kwargs.setdefault("firstSample", flags.LAr.ROD.FirstSample)
    # cosmics digitization
    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("UseTriggerTime", True)
        CosmicTriggerTimeTool = CompFactory.CosmicTriggerTimeTool
        kwargs.setdefault("TriggerTimeToolName", CosmicTriggerTimeTool())
    # pileup configuration "algorithm" way
    if not flags.Digitization.DoXingByXingPileUp:
        if flags.Digitization.PileUp or flags.Common.ProductionStep == ProductionStep.Overlay:
            kwargs.setdefault("PileUp", True)
    kwargs.setdefault("useLArFloat", useLArFloat(flags))
    if useLArFloat(flags):
        acc.merge(InputOverwriteCfg("LArHitContainer","LArHitEMB","LArHitFloatContainer","LArHitEMB"))
        acc.merge(InputOverwriteCfg("LArHitContainer","LArHitEMEC","LArHitFloatContainer","LArHitEMEC"))
        acc.merge(InputOverwriteCfg("LArHitContainer","LArHitHEC","LArHitFloatContainer","LArHitHEC"))
        acc.merge(InputOverwriteCfg("LArHitContainer","LArHitFCAL","LArHitFloatContainer","LArHitFCAL"))
        kwargs.setdefault("LArHitContainers", [])
    else:
        kwargs.setdefault("LArHitFloatContainers", [])
    if flags.Common.ProductionStep == ProductionStep.Overlay:
        kwargs.setdefault("OnlyUseContainerName", False)
        if flags.Overlay.DataOverlay:
            kwargs.setdefault("InputDigitContainer", flags.Overlay.BkgPrefix + "FREE")
        else:
            kwargs.setdefault("InputDigitContainer", flags.Overlay.BkgPrefix + "LArDigitContainer_MC")
    LArPileUpTool = CompFactory.LArPileUpTool
    acc.setPrivateTools(LArPileUpTool(name, **kwargs))
    return acc


def LArOutputCfg(flags):
    """Return ComponentAccumulator with Output for LAr. Not standalone."""
    acc = ComponentAccumulator()
    if flags.Output.doWriteRDO:
        ItemList = []
        if flags.Digitization.AddCaloDigi:
            ItemList.append("LArDigitContainer#*")
        elif flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
            ItemList.append("LArDigitContainer#" + flags.Overlay.BkgPrefix + "LArDigitContainer_MC")
        elif flags.Digitization.AddCaloDigiThinned:
            ItemList.append("LArDigitContainer#LArDigitContainer_MC_Thinned")
        if flags.Common.ProductionStep != ProductionStep.PileUpPresampling:
            ItemList.append("LArRawChannelContainer#LArRawChannels")
        if flags.Digitization.TruthOutput:
            ItemList.append("CaloCalibrationHitContainer#*")
            acc.merge(TruthDigitizationOutputCfg(flags))
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList))
    return acc


def LArDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with LAr digitization configuration"""
    acc = ComponentAccumulator()
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(LArPileUpToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    acc.merge(LArRawChannelBuilderAlgCfg(flags))
    if flags.Digitization.AddCaloDigiThinned:
        acc.merge(LArDigitThinnerCfg(flags))
    return acc


def LArDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator with LAr digitization and Output"""
    acc = LArDigitizationBasicCfg(flags, **kwargs)
    acc.merge(LArOutputCfg(flags))
    return acc


def LArOverlayDigitizationBasicCfg(flags, name="digitmaker1", **kwargs):
    """Return ComponentAccumulator with LAr Overlay digitization configuration"""
    acc = ComponentAccumulator()

    if flags.Overlay.DataOverlay:
        from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
        acc.merge(LArRawDataReadingCfg(flags))

    if "LArPileUpTool" not in kwargs:
        kwargs["LArPileUpTool"] = acc.popToolsAndMerge(LArPileUpToolCfg(flags))

    if flags.Concurrency.NumThreads > 0:
        kwargs.setdefault('Cardinality', flags.Concurrency.NumThreads)

    LArDigitMaker = CompFactory.LArDigitMaker
    acc.addEventAlgo(LArDigitMaker(name, **kwargs))

    acc.merge(LArRawChannelBuilderAlgCfg(flags))
    if flags.Digitization.AddCaloDigiThinned:
        acc.merge(LArDigitThinnerCfg(flags))
    return acc


def LArOverlayCfg(flags, **kwargs):
    """Return ComponentAccumulator with LAr overlay and Output"""
    acc = LArOverlayDigitizationBasicCfg(flags, **kwargs)
    acc.merge(LArOutputCfg(flags))
    return acc


def LArTriggerDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for LAr Trigger Tower"""
    acc = LArDigitizationBasicCfg(flags)
    acc.merge(CaloTriggerTowerCfg(flags))

    kwargs.setdefault("NoiseOnOff", flags.Digitization.DoCaloNoise)
    kwargs.setdefault("PileUp", flags.Digitization.PileUp)
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("EmTTL1ContainerName", flags.Overlay.BkgPrefix + "LArTTL1EM")
        kwargs.setdefault("HadTTL1ContainerName", flags.Overlay.BkgPrefix + "LArTTL1HAD")
    LArTTL1Maker = CompFactory.LArTTL1Maker
    acc.addEventAlgo(LArTTL1Maker(**kwargs))
    return acc


def LArTriggerDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for LAr Trigger Tower and Output"""
    acc = LArTriggerDigitizationBasicCfg(flags)
    acc.merge(LArOutputCfg(flags))
    acc.merge(OutputStreamCfg(flags, "RDO", ["LArTTL1Container#*"]))
    return acc


def LArOverlayTriggerDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with LAr Overlay Trigger Tower"""
    acc = LArOverlayDigitizationBasicCfg(flags)
    acc.merge(CaloTriggerTowerCfg(flags))

    kwargs.setdefault("NoiseOnOff", flags.Digitization.DoCaloNoise)
    kwargs.setdefault("PileUp", True)
    kwargs.setdefault("EmTTL1ContainerName", flags.Overlay.SigPrefix + "LArTTL1EM")
    kwargs.setdefault("HadTTL1ContainerName", flags.Overlay.SigPrefix + "LArTTL1HAD")

    if flags.Concurrency.NumThreads > 0:
        kwargs.setdefault('Cardinality', flags.Concurrency.NumThreads)

    LArTTL1Maker = CompFactory.LArTTL1Maker
    acc.addEventAlgo(LArTTL1Maker(**kwargs))
    return acc


def LArHitFilterCfg(flags, **kwargs):
    """ Return ComponentAccumulator with LArHitFilter """
    acc = ComponentAccumulator()
    acc.merge(LArGMCfg(flags))
    acc.merge(InputRenameCfg("LArHitContainer","LArHitEMB","LArHitEMBOLD"))
    acc.merge(InputRenameCfg("LArHitContainer","LArHitEMEC","LArHitEMECOLD"))
    acc.merge(InputRenameCfg("LArHitContainer","LArHitHEC","LArHitHECOLD"))
    acc.merge(InputRenameCfg("LArHitContainer","LArHitFCAL","LArHitFCALOLD"))
    LArHitFilter = CompFactory.LArHitFilter
    acc.addEventAlgo(LArHitFilter("LArHitFilter"))
    return acc
