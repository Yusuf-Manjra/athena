# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for SV1Tag
#0;115;0c Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaSV1Tag = { 'IsATagger'         : True,
               'DependsOn'         : ['AtlasExtrapolator',
                                      'BTagTrackToVertexTool',
                                      'InDetVKalVxInJetTool',
                                      'SV1NewLikelihoodTool'],
               'PassByPointer'     : {'LikelihoodTool'  : 'SV1NewLikelihoodTool'},
#               'PassByName'        : {'SecVxFinderName' : 'InDetVKalVxInJetTool' },
               'JetCollectionList' : 'jetCollectionList',
#               'JetWithInfoPlus'   : 'jetWithInfoPlus',
               'ToolCollection'    : 'SV1Tag' }

def toolSV1Tag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SV1Tag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    SVAlgType                           default: "SV1"
    jetCollectionList                   default: BTaggingFlags.Jets
    LikelihoodTool                      default: None
    xAODBaseName                        default: "SV1"
    SecVxFinderName                     default: "SV1"
    UseCHypo                            default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'SVAlgType'                        : 'SV1',
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'xAODBaseName'                     : 'SV1',
                     'SecVxFinderName'                  : 'SV1',
                     'UseCHypo'                         : True,
#                     'jetWithInfoPlus'                  : BTaggingFlags.JetsWithInfoPlus,
#                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'LikelihoodTool'                   : None }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SVTag
    return Analysis__SVTag(**options)

#-----------------------------------------------------------------

metaSV1NewLikelihoodTool = { 'CalibrationFolders' : ['SV1',],
                             'DependsOn'          : ['BTagCalibrationBrokerTool',],
                             'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool' },
                             'ToolCollection'     : 'SV1Tag' }

def toolSV1NewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SV1NewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "SV1"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'taggerName'                          : 'SV1' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
