# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def GepMETAlgCfg(
        flags,
        name,
        caloClustersKey,
        outputMETKey,
        OutputLevel=None):
    
    cfg = ComponentAccumulator()

    alg = CompFactory.GepMETAlg(name,
                                caloClustersKey=caloClustersKey,
                                outputMETKey=outputMETKey)
    if OutputLevel is not None:
        alg.OutputLevel = OutputLevel
        
    cfg.addEventAlgo(alg)

    return cfg
    
                     
