# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.BTagTrackToVertexIPEstimatorConfig import BTagTrackToVertexIPEstimatorCfg
from JetTagTools.NewLikelihoodToolConfig import NewLikelihoodToolCfg
from JetTagTools.MuonSelectorToolConfig import MuonSelectorToolCfg

# import the SoftMuonTag configurable
Analysis__SoftMuonTag=CompFactory.Analysis.SoftMuonTag

def SoftMuonTagCfg( flags, name = 'SoftMu', useBTagFlagsDefaults = True, **options ):
    """Sets up a SoftMuonTag tool and returns it.

    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTagging.RunModus
    jetCollectionList                   default: BTaggingFlags.Jets
    originalMuCollectionName            default: BTaggingFlags.MuonCollectionName
    BTagJetEtamin                       default: 2.5 (only if BTagging.RunModus == 'reference')
    MuonQuality                         default: xAOD::Muon::Medium

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        trackToVertexIPEstimator = acc.popToolsAndMerge(BTagTrackToVertexIPEstimatorCfg(flags, 'TrkToVxIPEstimator'))
        muonSelectorTool = acc.popToolsAndMerge(MuonSelectorToolCfg('MuonSelectorTool'))
        likelihood = acc.popToolsAndMerge(NewLikelihoodToolCfg(flags, 'SoftMuonTagNewLikelihoodTool', 'SMT'))
        defaults = {
                     'Runmodus'                         : flags.BTagging.RunModus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'originalMuCollectionName'         : BTaggingFlags.MuonCollectionName,
                     'MuonQuality'                      : 2,
                     'muonSelectorTool'                 : muonSelectorTool,
                     'LikelihoodTool'                   : likelihood,
                     'TrackToVertexIPEstimator'         : trackToVertexIPEstimator, }
        if(flags.BTagging.RunModus == 'reference'):
            defaults['BTagJetEtamin'] = 2.5
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(Analysis__SoftMuonTag( **options))

    return acc
