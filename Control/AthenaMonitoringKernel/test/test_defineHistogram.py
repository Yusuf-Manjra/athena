#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
import unittest

from AthenaMonitoringKernel.GenericMonitoringTool import defineHistogram

class Test( unittest.TestCase ):
   def test_1D( self ):
      s = defineHistogram('var', 'TH1F', 'EXPERT', 'title', '', 10, 0.0, 10.0)
      self.assertEqual(s, 'EXPERT, TH1F, , , var, title, 10, 0.000000, 10.000000')

   def test_1D_label( self ):
      s = defineHistogram('var', 'TH1F', 'EXPERT', 'title', '', 10, 0.0, 10.0, labels=['a','b'])
      self.assertEqual(s, 'EXPERT, TH1F, , , var, title, 10, 0.000000, 10.000000, a:b')

   def test_1D_opt( self ):
      s = defineHistogram('var', 'TH1F', 'EXPERT', 'title', '', 10, 0.0, 10.0, opt='myopt')
      self.assertEqual(s, 'EXPERT, TH1F, , , var, title, 10, 0.000000, 10.000000, myopt')

   def test_1D_weight( self ):
      s = defineHistogram('var', 'TH1F', 'EXPERT', 'title', 'weight', 10, 0.0, 10.0)
      self.assertEqual(s, 'EXPERT, TH1F, weight, , var, title, 10, 0.000000, 10.000000')

   def test_2D( self ):
      s = defineHistogram('var1,var2', 'TH2F', 'EXPERT', 'title', '', 10, 0.0, 10.0, 20, 0.0, 20.0)
      self.assertEqual(s, 'EXPERT, TH2F, , , var1,var2, title, 10, 0.000000, 10.000000, 20, 0.000000, 20.000000')

   def test_3D( self ):
      s = defineHistogram('var1,var2,var3', 'TProfile2D', 'EXPERT', 'title', '', 10, 0.0, 10.0, 20, 0.0, 20.0, -1.0, 1.0)
      self.assertEqual(s, 'EXPERT, TProfile2D, , , var1,var2,var3, title, 10, 0.000000, 10.000000, 20, 0.000000, 20.000000, -1.000000, 1.000000')

   def test_enforcePath( self ):
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      athenaCommonFlags.isOnline=True
      with self.assertRaises(AssertionError):
         defineHistogram('var', 'TH1F')

   def test_labelType( self ):
      with self.assertRaises(AssertionError):
         defineHistogram('var', 'TH1F', path='EXPERT', labels='l1:l2')

   def test_efficiency( self ):
      s = defineHistogram('var', 'TEfficiency', 'EXPERT', 'title', '', 10, 0.0, 10.0)
      self.assertEqual(s, 'EXPERT, TEfficiency, , , var, title, 10, 0.000000, 10.000000')

   def test_1D_array( self ):
      s = defineHistogram('var', 'TH1F', 'EXPERT', 'title', '', [0,1,2,4,8])
      self.assertEqual(s, 'EXPERT, TH1F, , , var, title, 0:1:2:4:8')

   def test_2D_array( self ):
      s = defineHistogram('var1,var2', 'TH2F', 'EXPERT', 'title', '', [0,1,2], ybins=[1,2,3,7])
      self.assertEqual(s, 'EXPERT, TH2F, , , var1,var2, title, 0:1:2, 1:2:3:7')

   def test_nameConvention( self ):
      s = defineHistogram('var', 'TH1F', 'path', 'title', '', 10, 0.0, 10.0, convention='OFFLINE:lowStat')
      self.assertEqual(s, 'path, TH1F, , OFFLINE:lowStat, var, title, 10, 0.000000, 10.000000')

if __name__ == '__main__':
   unittest.main()
