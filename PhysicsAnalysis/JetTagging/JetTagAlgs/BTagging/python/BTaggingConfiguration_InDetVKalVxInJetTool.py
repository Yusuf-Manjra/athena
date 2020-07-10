# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Configuration functions for InDetVKalVxInJetTool
# Author: Wouter van den Wollenberg (2013-2014)
# Note: This tool has its own file because it is common to many different taggers
#       so deletion of the one tagger that has this function won't break the others.
from BTagging.BTaggingFlags import BTaggingFlags
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags

metaInDetVKalVxInJetTool = { 'IsAVertexFinder'          : True,
                             'VertexFinderxAODBaseName' : 'SV1',
                             'OneInTotal'               : True,
                             'DependsOn'                : ['BTagFullLinearizedTrackFactory',
                                                           'BTagTrackToVertexIPEstimator',
                                                           'BTagTrackToVertexTool',
                                                           'AtlasExtrapolator'] }

def toolInDetVKalVxInJetTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetVKalVxInJetTool tool and returns it.

    The following options have BTaggingFlags defaults:

    TrackSummaryTool                    default: None (not needed for b-tagging)
    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output. Note however
    that this tool also needs to be added to the main B-tagging tool."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'      : BTaggingFlags.OutputLevel,
                     'ExistIBL'         : geoFlags.isIBL(),
                     'IsPhase2'         : commonGeoFlags.Run() == "RUN4",
#                     'TrackSummaryTool' : None }
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])

        if(commonGeoFlags.Run()=="RUN4"):
            from BTagging.BTaggingConfiguration_InDetEtaDependentCutsSvc import IDEtaDepCutsSvc
            InDetEtaDependentCutsSvc = IDEtaDepCutsSvc_InDetVKalVxInJetFinder( "IDEtaDepCutsSvc_" + name )
            InDetEtaDependentCutsSvc.minStripHits = [0]
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr += InDetEtaDependentCutsSvc
            options['InDetEtaDependentCutsSvc'] = InDetEtaDependentCutsSvc

    options['name'] = name
    from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool
    return InDet__InDetVKalVxInJetTool(**options)
