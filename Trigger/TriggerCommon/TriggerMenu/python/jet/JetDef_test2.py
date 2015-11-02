# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
from cStringIO import StringIO
import copy
import unittest
from mock import MagicMock
from JetDef import generateHLTChainDef, usage
from test_functions import run_strawman_test

from ChainConfigMaker import JetAttributes

from MC_pp_V5_dicts import MC_pp_V5_dicts

class TestJetDef2(unittest.TestCase):

    def setUp(self):
        self.n_ErrorChainDefs = 0
        self.n_ChainDefs = 0
        
    def test_0(self):
        """Create ChainDef instances with full instantiation,
        diagnostics off"""

        # turn off diagonostics if set
        jetdef_test_env = os.environ.get('JETDEF_TEST', None)
        if jetdef_test_env is not None:
            del os.environ['JETDEF_TEST']

        chain_defs = run_strawman_test()

        # reset diagonostics on if this was the previous state.
        if jetdef_test_env:
            os.environ['JETDEF_TEST'] = jetdef_test_env

        self.n_ErrorChainDefs = len([c for c in chain_defs if
                                     c.__class__.__name__ == 'ErrorChainDef'])
        self.n_ChainDefs = len([c for c in chain_defs if
                                c.__class__.__name__ == 'ChainDef'])

        self.assertTrue(self.n_ChainDefs + self.n_ErrorChainDefs ==
                        len(chain_defs))
        self.assertFalse(self.n_ErrorChainDefs)

    def test_1(self):
        """Create ChainDef instances with full instantiation,
        diagnostics on"""

        # turn on diagonostics if not set
        jetdef_test_env = os.environ.get('JETDEF_TEST', None)
        if jetdef_test_env is None:
            os.environ['JETDEF_TEST'] = '1'

        chain_defs = run_strawman_test()

        # reset diagonostics off if this was the previous state.
        if jetdef_test_env is None:
            del os.environ['JETDEF_TEST']

        self.n_ErrorChainDefs = len([c for c in chain_defs if
                                     c.__class__.__name__ == 'ErrorChainDef'])
        self.n_ChainDefs = len([c for c in chain_defs if
                                c.__class__.__name__ == 'ChainDef'])

        self.assertTrue(self.n_ChainDefs + self.n_ErrorChainDefs ==
                        len(chain_defs))
        self.assertFalse(self.n_ErrorChainDefs)

if __name__ == '__main__':
    unittest.main()
