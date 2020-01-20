# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate EMTrackMatchBuilder with default configuration"
__author__ = "Bruno Lenzi"

from egammaTools import egammaToolsConf
from egammaRec.Factories import FcnWrapper, ToolFactory
from egammaRec import egammaKeys
from RecExConfig.RecFlags import rec
from AthenaCommon.BeamFlags import jobproperties
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools, EMExtrapolationToolsCache

EMTrackMatchBuilder = ToolFactory( egammaToolsConf.EMTrackMatchBuilder,
      TrackParticlesName = egammaKeys.outputTrackParticleKey(),
      ExtrapolationTool  = EMExtrapolationToolsCache,
      broadDeltaEta      = 0.1, #candidate match is done in 2 times this  so +- 0.2
      broadDeltaPhi      = 0.15,  #candidate match is done in 2 times this  so +- 0.3
      useCandidateMatch  = True,
      useScoring         = True,
      SecondPassRescale  = True,
      UseRescaleMetric   = True,
      isCosmics          = (jobproperties.Beam.beamType()=="cosmics")
)
