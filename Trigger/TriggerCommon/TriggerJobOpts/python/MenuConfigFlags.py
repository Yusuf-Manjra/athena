
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
import six


def createMenuFlags():
    # this flags define which chains are in trgger menu
    # the flags content is not just the name bu contains a few key elements of the chains definition, like the seed, other chain aspects can be added
    # The DF/CF and hypos are then generated using the menu code
    # Should the HLT_ and L1_ prefixes are omitted (DRY principle)?
    # each flag is translated to an independent reconstruction chain
    flags = AthConfigFlags()

    #flags.addFlag('Trigger.menu.muons', [])
    flags.addFlag('Trigger.menu.muon', [])
    flags.addFlag('Trigger.menu.electron', [])
    flags.addFlag('Trigger.menu.photon', [])
    flags.addFlag('Trigger.menu.jet', [])
    flags.addFlag('Trigger.menu.combined', [])

    return flags


class MenuUtils(object):
    @staticmethod
    def toCTPSeedingDict( flags ):
        seeding = {} # HLTChain to L1 item mapping
        for k,v in flags._flagdict.iteritems():
            if "Trigger.menu." in k:
                for chain in v.get():
                    hlt = chain.name                    
                    l1 = hlt[hlt.index("_L1")+1:].replace("L1","L1_", 1)
                    seeding[hlt] = l1 # this is the item name
        return seeding

import unittest
class __MustHaveMenuInTheName(unittest.TestCase):    
    def runTest(self):
        flags = createMenuFlags()
        for fname,fval in six.iteritems (flags._flagdict):
            self.assertTrue( fname.startswith('Trigger.menu.') )

