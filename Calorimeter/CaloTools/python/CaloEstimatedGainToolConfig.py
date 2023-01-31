# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
#
# File: CaloTools/python/CaloEstimatedGainToolConfig.py
# Created: Aug 2019, sss
# Purpose: Configure CaloEstimatedGainTool.
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def CaloEstimatedGainToolCfg (flags):
    result = ComponentAccumulator()

    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    result.merge (GeoModelCfg (flags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    
    result.merge(LArGMCfg(flags))
    result.merge(TileGMCfg(flags))

    from LArCabling.LArCablingConfig import LArFebRodMappingCfg, LArCalibIdMappingCfg 
    result.merge(LArFebRodMappingCfg(flags))
    result.merge(LArCalibIdMappingCfg(flags))

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    result.merge (TileInfoLoaderCfg (flags))

    from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
    acc = TileCondToolEmscaleCfg (flags)
    emscaleTool = acc.popPrivateTools()
    result.merge (acc)

    from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
    acc = TileCondToolNoiseSampleCfg (flags)
    noiseSampleTool = acc.popPrivateTools()
    result.merge (acc)

    from LArRecUtils.LArADC2MeVCondAlgConfig import LArADC2MeVCondAlgCfg
    result.merge (LArADC2MeVCondAlgCfg (flags))
    adc2mev = result.getCondAlgo ('LArADC2MeVCondAlg')

    TileCondIdTransforms=CompFactory.TileCondIdTransforms

    CaloEstimatedGainTool=CompFactory.CaloEstimatedGainTool
    tool = CaloEstimatedGainTool ('CaloEstimatedGainTool',
                                  ADC2MeVKey = adc2mev.LArADC2MeVKey,
                                  TileCondIdTransforms = TileCondIdTransforms(),
                                  TileCondToolEmscale = emscaleTool,
                                  TileCondToolNoiseSample = noiseSampleTool)

    result.setPrivateTools (tool)

    return result


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import initConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    flags1 = initConfigFlags()
    flags1.Input.Files = defaultTestFiles.RDO_RUN2
    flags1.lock()
    acc1 = CaloEstimatedGainToolCfg (flags1)
    only = ['CaloEstimatedGainTool']
    acc1.printCondAlgs(summariseProps=True, onlyComponents = only)

    # Print a configurable with properties in sorted order.
    Configurable = CompFactory.CaloEstimatedGainTool.__bases__[0]
    def sorted_repr (c):
        if not isinstance(c, Configurable): return str(c)
        args = [c.name] + ['{}={!r}'.format(item[0], sorted_repr(item[1])) for item in sorted(c._properties.items())]
        return '{}({})'.format(type(c).__name__, ', '.join(args))

    print ('ComponentAccumulator:', sorted_repr (acc1.popPrivateTools()))
    acc1.wasMerged()
