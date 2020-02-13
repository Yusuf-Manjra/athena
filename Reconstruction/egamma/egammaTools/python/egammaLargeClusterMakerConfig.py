# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure egammaLargeClusterMaker, which chooses cells to store in the AOD" 
__author__ = "Jovan Mitrevski"

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
egammaLargeClusterMaker=CompFactory.egammaLargeClusterMaker

def egammaLargeClusterMakerCfg(flags, **kwargs):

    acc = ComponentAccumulator
    
    kwargs.setdefault("CellsName", flags.Egamma.Keys.Input.CaloCells)
    kwargs.setdefault("InputClusterCollection", flags.Egamma.Keys.Output.CaloClusters)

    acc.setPrivateTools(egammaLargeClusterMaker(**kwargs))

    return acc

