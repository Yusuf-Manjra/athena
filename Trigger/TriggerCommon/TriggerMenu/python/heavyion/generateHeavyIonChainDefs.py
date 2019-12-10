# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of min bias signatures"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)


from TriggerMenu.heavyion.HeavyIonDef import *
from TriggerMenu.menu.MenuUtils import *


HeavyIon = []

##########################################################################################
##########################################################################################


def generateChainDefs(chainDict):

    print ('generateHeavyIonChainDefs, input dict', chainDict)

    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []
    
    for subChainDict in listOfChainDicts:      
        HeavyIon = L2EFChain_HI(subChainDict)
        
        listOfChainDefs += [HeavyIon.generateHLTChainDef()]
        
    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]        
    
    return theChainDef
