# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.StreamingChainConfiguration import StreamingChainConfiguration as StreamingChainConfiguration
from TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging import mergeChainDefs


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.generateChainConfigs' )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):

    listOfChainDicts = splitChainDict(chainDict)
    log.debug("Will generate Config for streamer: ",chainDict['chainName'])

    listOfChainDefs = []
        
    #streamers will never have more than one chainPart but this is still
    #needed to move to the correct format [{}]->{}
    for subChainDict in listOfChainDicts:
        
        Streaming = StreamingChainConfiguration(subChainDict).assembleChain() 

        listOfChainDefs += [Streaming]
        log.debug('length of chaindefs %s', len(listOfChainDefs) )

    if len(listOfChainDefs)>1:
        log.warning("This is a streamer with more than one chainPart, is this really intended?")
        theChainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef %s" , theChainDef)

    return theChainDef

