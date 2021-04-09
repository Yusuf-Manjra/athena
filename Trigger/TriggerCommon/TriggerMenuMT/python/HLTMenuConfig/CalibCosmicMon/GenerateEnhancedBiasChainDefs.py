# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from ..Menu.ChainDictTools import splitChainDict
from ..Menu.ChainMerging import mergeChainDefs
from .EnhancedBiasChainConfiguration import EnhancedBiasChainConfiguration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.generateChainConfigs' )
log.info("Importing %s",__name__)


def generateChainConfigs( chainDict ):
    log.debug("Generate EnhancedBias...")
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        EnhancedBiasChain = EnhancedBiasChainConfiguration(subChainDict).assembleChain()
        listOfChainDefs += [EnhancedBiasChain]

    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef %s" , theChainDef)

    return theChainDef
