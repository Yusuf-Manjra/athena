# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from RecExConfig.Configured import Configured
from egammaRec import egammaKeys
from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import ToolFactory, PublicToolFactory, AlgFactory, FcnWrapper
from AthenaCommon.BeamFlags import jobproperties
from egammaRec import egammaRecFlags as egRecFlags
egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags
from egammaTools.egammaExtrapolators import egammaCaloExtrapolator
from MCTruthClassifier import MCTruthClassifierConf
import AthenaCommon.CfgMgr as CfgMgr

def getSimBarcodeOffset():
  # "Return the simulation barcode offset for G4 particles from metadata"
  from PyUtils.MetaReaderPeeker import metadata

  offset = 200e3
  try:
    return int(metadata['SimBarcodeOffset'])

  except:
    print ('Could not retrieve SimBarcodeOffset from /Simulation/Parameters, leaving at 200k')
  return int(offset)

def getSimBarcodeOffset1():
  "Return the simulation barcode offset for G4 particles from metadata + 1"
  return getSimBarcodeOffset() + 1

EMClassifierParticleCaloExtensionTool =  PublicToolFactory (CfgMgr.Trk__ParticleCaloExtensionTool, 
                                                            name="EMClassifierParticleCaloExtensionTool",
                                                            Extrapolator = egammaCaloExtrapolator)

EMMCTruthClassifier = ToolFactory( MCTruthClassifierConf.MCTruthClassifier, name = 'EMMCTruthClassifier',
                                   ParticleCaloExtensionTool=EMClassifierParticleCaloExtensionTool,
                                   barcodeG4Shift = FcnWrapper( getSimBarcodeOffset1 )  )


egammaTruthAssociationAlg = AlgFactory( egammaAlgsConf.egammaTruthAssociationAlg,
                                        ClusterContainerName = egammaKeys.outputClusterKey(),
                                        ElectronContainerName = egammaKeys.outputElectronKey(),
                                        PhotonContainerName = egammaKeys.outputPhotonKey(),
                                        FwdElectronContainerName = egammaKeys.outputFwdElectronKey(),
                                        TruthEventContainerName = egammaKeys.truthEventKey(),
                                        TruthParticleContainerName = egammaKeys.truthParticleKey(),                                                
                                        EgammaTruthContainerName = egammaKeys.outputTruthKey(),
                                        MCTruthClassifier = EMMCTruthClassifier,
                                        MatchForwardElectrons = egammaRecFlags.doEgammaForwardSeeded(),
                                        MatchClusters = False,
                                        SimBarcodeOffset = FcnWrapper( getSimBarcodeOffset )
                                        )


class egammaTruthAssociationGetter ( Configured ) :

    def configure(self):
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger ('egammaTruthAssociationGetter::configure:')
        mlog.info('entering')
        

        # configure egammaTruthAssociation here:
        try:
            self._egammaTruthAssociationHandle = egammaTruthAssociationAlg()
        except Exception:
            mlog.error("could not get handle to egammaTruthAssociation")
            import traceback
            traceback.print_exc()
            return False
        return True

    def egammaTruthAssociationHandle(self):
        return self._egammaTruthAssociationHandle

