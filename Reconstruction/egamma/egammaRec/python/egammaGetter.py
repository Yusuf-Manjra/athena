# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# specifies egamma"standard"
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

from AthenaCommon.DetFlags import DetFlags
from RecExConfig.Configured import Configured
#
from egammaRec import egammaKeys
from egammaRec import egammaRecConf
from egammaRec.Factories import AlgFactory, FcnWrapper
from egammaRec import egammaRecFlags as egRecFlags
egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags

def doSuperclusters():
  return egammaRecFlags.doSuperclusters()

from egammaTools.egammaToolsFactories import \
    EMBremCollectionBuilder, EMTrackMatchBuilder,\
    EMVertexBuilder, EMConversionBuilder, EMAmbiguityTool,\
    EMClusterTool, EMFourMomBuilder, EMShowerBuilder, egammaOQFlagsBuilder, \
    ElectronPIDBuilder, PhotonPIDBuilder

if doSuperclusters() : 
  from egammaTools.egammaToolsFactories import \
      egammaSuperClusterBuilder, egammaClusterOverlapMarker

def doConversions() :
  return DetFlags.detdescr.ID_on() and egammaRecFlags.doConversions()

def getTopoSeededCollectionName():
  if egammaRecFlags.doTopoCaloSeeded() :
    from CaloRec import CaloRecFlags
    from CaloRec.CaloRecTopoEM35Flags import jobproperties
    jobproperties.CaloRecTopoEM35Flags.EtSeedCut =0.8 * GeV # The cut on the Fixed size is 1.5 GeV later on 
    from CaloRec.CaloClusterTopoEMFixedSizeGetter import CaloClusterTopoEMFixedSizeGetter
    theCaloClusterTopoEMFixedSizeGetter = CaloClusterTopoEMFixedSizeGetter()
    return theCaloClusterTopoEMFixedSizeGetter.outputKey()
  else:
    return ""

def doTopoCaloSeeded():
  return egammaRecFlags.doTopoCaloSeeded() and getTopoSeededCollectionName()

def egammaDecorationTools():
  "Return a list with the tools that decorate both electrons and photons"
  if doSuperclusters() :
    return [EMFourMomBuilder(), EMShowerBuilder(), egammaOQFlagsBuilder()]
  else:
    return [EMClusterTool(), EMFourMomBuilder(), EMShowerBuilder(), egammaOQFlagsBuilder()]

def electronDecorationTools():
  "Return a list with the tools that decorate only electrons"
  return [ ElectronPIDBuilder() ]

def photonDecorationTools():
  "Return a list with the tools that decorate only photons"
  return [ PhotonPIDBuilder() ]

class egammaGetter ( Configured ) :

    def configure(self):
        mlog = logging.getLogger ('egammaGetter.py::configure:')
        mlog.info('entering')        

        egammaBuilder = AlgFactory(egammaRecConf.topoEgammaBuilder if doSuperclusters() else egammaRecConf.egammaBuilder, name = 'egamma',
                                   # Keys
                                   ElectronOutputName = egammaKeys.outputElectronKey(),
                                   PhotonOutputName = egammaKeys.outputPhotonKey(),
                                   TopoSeededClusterContainerName = getTopoSeededCollectionName(),
                           
                                   # Builder tools
                                   BremCollectionBuilderTool = EMBremCollectionBuilder,
                                   TrackMatchBuilderTool = EMTrackMatchBuilder,
                                   VertexBuilder = EMVertexBuilder if doConversions() else None,
                                   ConversionBuilderTool = EMConversionBuilder if doConversions() else None,
                                   AmbiguityTool = EMAmbiguityTool,
                                   
                                   # Decoration tools
                                   egammaTools = FcnWrapper(egammaDecorationTools),
                                   ElectronTools = FcnWrapper(electronDecorationTools),
                                   PhotonTools = FcnWrapper(photonDecorationTools),
                                   
                                   # Flags and other properties
                                   # Brem Collection building and track matching depending if ID is on/off
                                   doBremCollection= DetFlags.detdescr.ID_on(), 
                                   doTrackMatching = DetFlags.detdescr.ID_on(),
                                   clusterEnergyCut = 10*MeV,
                                   doConversions = doConversions(), # conversions building/matching depending if ID is on/off
                                   doTopoSeededPhotons = egammaRecFlags.doTopoCaloSeeded()
                                   )        

        # Configure extra tools if we're using supercluster algorithm.
        if doSuperclusters() : 
          egammaBuilder.SuperClusterBuilder = egammaSuperClusterBuilder
          egammaBuilder.OverlapMarker = egammaClusterOverlapMarker
        
        # configure egamma here:
        try:
            self._egammaBuilderHandle = egammaBuilder()
        except Exception:
            mlog.error("could not get handle to egamma")
            import traceback
            print traceback.format_exc()
            return False
        
        print self._egammaBuilderHandle
        return True

    def egammaBuilderHandle(self):
        return self._egammaBuilderHandle
