# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RTTTestCase import RTTTestCase
import unittest
import os

class TestCase(RTTTestCase):

    def testCMLComp(self): self.rttTestXMLComp()

    def testKeepfileDiff(self): self.rttKeepfileDiff()

    def testNKeepFiles(self): self.rttTestNTags(self.dom, 'keepfile_newstyle', 17)
        
if __name__ == '__main__':
    from runTestCase import runTestCase
    runTestCase(TestCase)
