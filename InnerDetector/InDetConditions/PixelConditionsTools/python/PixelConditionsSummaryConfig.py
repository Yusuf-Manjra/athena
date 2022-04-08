"""Define a function to configure PixelConditionsSummaryCfg

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format

from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg, PixelDeadMapCondAlgCfg
)
from PixelReadoutGeometry.PixelReadoutGeometryConfig import PixelReadoutManagerCfg

def PixelConditionsSummaryCfg(flags, name="PixelConditionsSummary", **kwargs):
    """Return configured ComponentAccumulator with tool for Pixel Conditions"""
    acc = PixelReadoutManagerCfg(flags)
    acc.merge(PixelDCSCondStateAlgCfg(flags))
    acc.merge(PixelDCSCondStatusAlgCfg(flags))
    acc.merge(PixelDeadMapCondAlgCfg(flags))

    kwargs.setdefault("PixelReadoutManager", acc.getPrimary())
    kwargs.setdefault("UseByteStreamFEI4", not flags.Input.isMC and not flags.Overlay.DataOverlay and flags.Input.Format is Format.BS)
    kwargs.setdefault("UseByteStreamFEI3", not flags.Input.isMC and not flags.Overlay.DataOverlay and flags.Input.Format is Format.BS)

    if flags.InDet.usePixelDCS:
        kwargs.setdefault("IsActiveStates", [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ])
        kwargs.setdefault("IsActiveStatus", [ 'OK', 'WARNING', 'ERROR', 'FATAL' ])

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementCondAlgCfg
    acc.merge(PixelDetectorElementCondAlgCfg(flags))
    kwargs.setdefault( "PixelDetEleCollKey", "PixelDetectorElementCollection")

    acc.setPrivateTools(CompFactory.PixelConditionsSummaryTool(name=name + "Tool", **kwargs))
    return acc
