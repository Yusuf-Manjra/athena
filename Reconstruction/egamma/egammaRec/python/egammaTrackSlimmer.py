# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

# specifies egamma"standard"
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
import traceback
import EventKernel.ParticleDataType
from egammaRec.egammaRecFlags import jobproperties
from egammaRec import egammaKeys
from InDetRecExample.InDetKeys import InDetKeys
from RecExConfig.Configured import Configured



class egammaTrackSlimmer ( Configured ) :
 
  def configure(self):
    mlog = logging.getLogger ('egammaTrackSlimmer.py::configure:')
    mlog.info('entering')
 
    try:
      from egammaAlgs.egammaAlgsConf import egammaTrackSlimmer
      theEgammaTrackSlimmer = egammaTrackSlimmer(
        "egammaTrackSlimmer",
        StreamName = 'StreamAOD',
        InputElectronContainerName=egammaKeys.outputElectronKey(),
        InputPhotonContainerName=egammaKeys.outputPhotonKey(),
        TrackParticleContainerName= egammaKeys.outputTrackParticleKey(), 
        VertexContainerName= egammaKeys.outputConversionKey(), 
        InDetTrackParticleContainerName=InDetKeys.xAODTrackParticleContainer(), 
        doThinning=True
        )
      print (theEgammaTrackSlimmer)
    except Exception:
      mlog.error("could not get handle to egammaTrackSlimmer")
      traceback.print_exc()
      return False
    
    # add to topsequence 
    mlog.info("now adding to topSequence")
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += theEgammaTrackSlimmer
 
 
    return True


