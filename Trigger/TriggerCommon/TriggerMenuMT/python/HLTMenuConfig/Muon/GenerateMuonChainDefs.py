# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Muon chains
###########################################################################

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Muon.generateChainConfigs' )
logging.getLogger().info("Importing %s",__name__)

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.Muon.MuonDef import MuonChainConfiguration as MuonChainConfiguration
from TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging import mergeChainDefs


def generateChainConfigs(chainDict):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs=[]

    for subChainDict in listOfChainDicts:
        
        Muon = MuonChainConfiguration(subChainDict).assembleChain() 

        listOfChainDefs += [Muon]
        log.debug('length of chaindefs %s', len(listOfChainDefs) )
        

    if len(listOfChainDefs)>1:
        if 'noL1' in chainDict['chainName']:
            theChainDef = mergeSerial(listOfChainDefs)
        else:
            theChainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef %s" , theChainDef)

    return theChainDef

def mergeSerial(listOfChainDefs):

    chaindef = listOfChainDefs[0]
    listOfChainDefs.pop(0)
    steps = chaindef.steps

    for cdef in listOfChainDefs:
        csteps = cdef.steps 
        for step in csteps:
            steps.append(step)

    return chaindef



