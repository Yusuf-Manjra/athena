# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTagSecVertexingConfig import BTagSecVtxToolCfg
from BTagging.BTagTrackAssociationConfig import BTagTrackAssociationCfg
from BTagging.BTagToolConfig import BTagToolCfg

# import the JetBTaggerAlg configurable
from BTagging.BTaggingConf import Analysis__JetBTaggerAlg as JetBTaggerAlg

def JetBTaggerAlgCfg(ConfigFlags, JetCollection="", TaggerList=[], SetupScheme="", **options):

    acc=ComponentAccumulator()
    jetcol = JetCollection

    # setup the Analysis__BTagTrackAssociation tool
    options.setdefault('BTagTrackAssocTool', acc.popToolsAndMerge(BTagTrackAssociationCfg(ConfigFlags, 'TrackAssociation'+ ConfigFlags.BTagging.GeneralToolSuffix, jetcol, TaggerList )))
    
    options.setdefault('BTagTool', acc.popToolsAndMerge(BTagToolCfg(ConfigFlags, jetcol, TaggerList)))

    timestamp = options.get('TimeStamp', None)
    if not timestamp:
        timestamp = ['']
    else:
        del options['TimeStamp']
     
    for ts in timestamp:
        # setup the secondary vertexing tool
        options['BTagSecVertexing'] = acc.popToolsAndMerge(BTagSecVtxToolCfg(ConfigFlags, 'SecVx'+ConfigFlags.BTagging.GeneralToolSuffix, jetcol, TimeStamp = ts, **options))

        btagname = ConfigFlags.BTagging.OutputFiles.Prefix + jetcol
        if ts:
            btagname += '_'+ts
        # Set remaining options
        options['name'] = (btagname + ConfigFlags.BTagging.GeneralToolSuffix).lower()
        options['JetCollectionName']= jetcol.replace('Track','PV0Track') + "Jets"
        options['JetCalibrationName']= jetcol.replace('Track','PV0Track')
        options['BTaggingCollectionName'] = btagname
        options['BTaggingLink'] = '.btaggingLink'+ts

        # -- create main BTagging algorithm
        acc.addEventAlgo(JetBTaggerAlg(**options))
            
    return acc
