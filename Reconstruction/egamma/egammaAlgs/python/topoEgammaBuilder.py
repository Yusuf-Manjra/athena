# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate the
two supercluster builders with default configuration"""
__author__ = "Jovan Mitrevski"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from egammaRec import egammaKeys
from RecExConfig.RecFlags import rec
from egammaTools.egammaToolsFactories import EGammaAmbiguityTool

topoEgammaBuilder = AlgFactory(
    egammaAlgsConf.xAODEgammaBuilder,
    name='topoEgammaBuilder',
    InputElectronRecCollectionName=egammaKeys.ElectronSuperRecKey(),
    InputPhotonRecCollectionName=egammaKeys.PhotonSuperRecKey(),
    ElectronOutputName=egammaKeys.outputElectronKey(),
    PhotonOutputName=egammaKeys.outputPhotonKey(),
    AmbiguityTool=EGammaAmbiguityTool,
    isTruth=rec.doTruth()
)
