# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from InDetRecExample.InDetKeys import InDetKeys

def egammaCaloClusterSelectorCfg(ConfigFlags, name="caloClusterROISelector", **kwargs):
    result = ComponentAccumulator()
    if "egammaCheckEnergyDepositTool" not in kwargs:
        kwargs["egammaCheckEnergyDepositTool"] = CompFactory.egammaCheckEnergyDepositTool()
    kwargs.setdefault("EMEtCut", 2250.)
    kwargs.setdefault("EMEtSplittingFraction", 0.7)
    kwargs.setdefault("EMFCut", 0.7)
    kwargs.setdefault("RetaCut", 0.65)
    kwargs.setdefault("HadLeakCut", 0.15)
    result.setPrivateTools(CompFactory.egammaCaloClusterSelector(name, **kwargs))
    return result


def CaloClusterROI_SelectorCfg(ConfigFlags, name="InDetCaloClusterROISelector", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("InputClusterContainerName",  ConfigFlags.Egamma.Keys.Internal.EgammaTopoClusters) #TODO synhronise InDetKeys.CaloClusterContainer())
    kwargs.setdefault("OutputClusterContainerName", InDetKeys.CaloClusterROIContainer())
    if "CaloClusterROIBuilder" not in kwargs:
        kwargs["CaloClusterROIBuilder"] = CompFactory.InDet.CaloClusterROI_Builder(
            "InDetCaloClusterROIBuilder",
            EMEnergyOnly=True,
        )
    if "egammaCaloClusterSelector" not in kwargs:
        kwargs["egammaCaloClusterSelector"] = result.popToolsAndMerge(egammaCaloClusterSelectorCfg(ConfigFlags))
    result.addEventAlgo(CompFactory.InDet.CaloClusterROI_Selector(name, **kwargs), primary=True)
    return result