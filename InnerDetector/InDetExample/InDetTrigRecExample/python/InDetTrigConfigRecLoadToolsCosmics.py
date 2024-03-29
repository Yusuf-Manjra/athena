# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

""" InDetTrigConfigRecLoadToolsCosmics
    various tools for cosmicsN slice
"""

__author__ = "J. Masik"
__version__= "$Revision: 1.2 $"
__doc__    = "InDetTrigConfigRecLoadToolsCosmics"


from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
from AthenaCommon.Logging import logging 
log = logging.getLogger("InDetTrigConfigRecLoadToolsCosmics.py")

from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsCosmics

from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetCosmicScoringTool
InDetTrigScoringToolCosmics_SiPattern = \
    InDet__InDetCosmicScoringTool(name = 'InDetTrigCosmicScoringTool_SiPattern')

InDetTrigScoringToolCosmics_SiPattern.nWeightedClustersMin = EFIDTrackingCutsCosmics.nWeightedClustersMin()
InDetTrigScoringToolCosmics_SiPattern.minTRTHits = 0
ToolSvc += InDetTrigScoringToolCosmics_SiPattern

if (InDetTrigFlags.doPrintConfigurables()):
  print (     InDetTrigScoringToolCosmics_SiPattern)


from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
import InDetRecExample.TrackingCommon as TrackingCommon

InDetTrigAmbiTrackSelectionToolCosmicsN = InDet__InDetAmbiTrackSelectionTool \
  (name = 'InDetTrigAmbiTrackSelectionToolCosmicsN',
   minHits         = 0,
   minNotShared    = 3,
   maxShared       = 0,
   minTRTHits      = 0, # used for Si only tracking !!!
   Cosmics=True,
   maxTracksPerSharedPRD = 10,
   UseParameterization = False,
   DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
   AssociationTool    = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels())

ToolSvc += InDetTrigAmbiTrackSelectionToolCosmicsN




