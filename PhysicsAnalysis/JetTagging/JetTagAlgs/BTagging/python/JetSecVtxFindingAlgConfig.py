# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.InDetImprovedJetFitterVxFinderConfig import InDetImprovedJetFitterVxFinderCfg
from InDetConfig.InDetVKalVxInJetToolConfig import InDetVKalVxInJetToolCfg, MSV_InDetVKalVxInJetToolCfg


def JetSecVtxFindingAlgCfg(flags, BTagVxSecVertexInfoName, SVAlgName, JetCollection, PrimaryVertexCollectionName="", SVFinder="", TracksToTag="", **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           JetCollection       The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           options:            Python dictionary of options to be passed to the SecVtxTool.
    output: The tool."""

    acc = ComponentAccumulator()

    if SVFinder == 'JetFitter':
        secVtxFinder = acc.popToolsAndMerge(InDetImprovedJetFitterVxFinderCfg(flags, 'JFVxFinder'))
    elif SVFinder == 'JetFitterFlip': #Add the JetFitterFlip tagger (invert sign of signed track impact parameter)
        secVtxFinder = acc.popToolsAndMerge(InDetImprovedJetFitterVxFinderCfg(flags, 'JFVxFinderFlip', 'FLIP_SIGN'))
    elif SVFinder == 'SV1':
        secVtxFinder = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg(flags, "IDVKalVxInJet"))
    elif SVFinder == 'SV1Flip':
        secVtxFinder = acc.popToolsAndMerge(InDetVKalVxInJetToolCfg(flags, "IDVKalVxInJetFlip"))
    elif SVFinder == 'MSV':
        secVtxFinder = acc.popToolsAndMerge(MSV_InDetVKalVxInJetToolCfg(flags, "IDVKalMultiVxInJet"))
    else:
        return acc

    options = {}
    options.setdefault('SecVtxFinder', secVtxFinder)
    options.setdefault('vxPrimaryCollectionName', PrimaryVertexCollectionName)
    options['JetCollectionName'] = JetCollection
    options['TracksToTag'] = TracksToTag
    options['BTagVxSecVertexInfoName'] = BTagVxSecVertexInfoName
    options['name'] = SVAlgName

    # -- create the association algorithm
    acc.addEventAlgo(CompFactory.Analysis.JetSecVtxFindingAlg(**options))

    return acc
