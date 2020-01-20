# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure e/gamma object quality"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
egammaOQFlagsBuilder=CompFactory.egammaOQFlagsBuilder
CaloAffectedTool=CompFactory.CaloAffectedTool


def egammaOQFlagsBuilderCfg(flags, name='egammaOQFlagsBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    kwargs.setdefault("CellsName", flags.Egamma.Keys.Input.CaloCells)
    kwargs.setdefault("affectedTool", CaloAffectedTool())

    tool = egammaOQFlagsBuilder(name, **kwargs)

    acc.setPrivateTools(tool)
    return acc
