# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import json
from collections import OrderedDict as odict
from TrigConfigSvc.TrigConfigSvcCfg import getHLTMenuFileName

from AthenaCommon.Logging import logging
__log = logging.getLogger( __name__ )


def __generateJSON( chainDicts, chainConfigs, menuName, fileName ):
    """ Generates JSON given the ChainProps and sequences
    """
    menuDict = odict([("filetype", "hltmenu"), ("name", menuName), ("chains", [])])

    from TriggerMenuMT.HLTMenuConfig.Menu import StreamInfo
    for chain in chainDicts:
        streamDicts = []
        streamTags = StreamInfo.getStreamTags(chain["stream"])
        for streamTag in streamTags:
            streamDicts.append({"name": streamTag.name(),
                                "type": streamTag.type(),
                                "obeyLB": streamTag.obeysLumiBlock(),
                                "forceFullEventBuilding": streamTag.forceFullEventBuilding()})

        l1Thresholds  = []
        [ l1Thresholds.append(p['L1threshold']) for p in chain['chainParts'] ]

        # Tim M: This is a non-final example of a way we could save Sequence info
        # This is still under discussion
        # chainConfig = filter(lambda c: c.name == chain["chainName"], chainConfigs)
        # sequences = {}
        # if len(chainConfig) == 0:
        #     __log.warn( "generateJSON cannot find sequence info for %s", chain["chainName"])
        # else:
        #     for theStep in chainConfig[0].steps:
        #         allSeqDict = []
        #         for theSeq in theStep.sequences:
        #             hypo = []
        #             if type(theSeq.hypo) is list:
        #                 [ hypo.append(h.Alg.name()) for h in theSeq.hypo ]
        #             else:
        #                 hypo.append(theSeq.hypo.Alg.name())
        #             allSeqDict.append({"Sequence":theSeq.sequence.Alg.name(), "Hypo":hypo})
        #         sequences[theStep.name] = allSeqDict



        chainDict = odict([ 
            ("counter", chain["chainCounter"]),
            ("name", chain["chainName"]),
            ("nameHash", chain["chainNameHash"]),
            ("l1item", chain["L1item"]),
            ("l1thresholds", l1Thresholds),
            ("groups", chain["groups"]),
            ("streams", streamDicts),
            #("sequences", sequences)
        ])


        menuDict["chains"].append( chainDict )

    __log.info( "Writing trigger menu to %s", fileName )
    with open( fileName, 'w' ) as fp:
        json.dump( menuDict, fp, indent=4, sort_keys=False )

def generateJSON():
    __log.info("Generating HLT JSON config in the rec-ex-common job")
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT


    return __generateJSON( TriggerConfigHLT.dictsList(), TriggerConfigHLT.configsList(), TriggerFlags.triggerMenuSetup(), getHLTMenuFileName() )
    
def generateJSON_newJO( chainDicts, chainConfigs ):
    __log.info("Generating HLT JSON config in the new JO")
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    return __generateJSON( chainDicts, chainConfigs, ConfigFlags.Trigger.triggerMenuSetup, getHLTMenuFileName( ConfigFlags) )
