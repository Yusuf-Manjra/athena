#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from .Algorithm_HLT_TableConstructor import Algorithm_HLT_TableConstructor

'''
@file Chain_Algorithm_HLT_TableConstructor.py
@brief Contains TableConstructor classes per Chain_Algorithm_HLTtable. It should
    have the same behaviour as Algorithm_HLT_TableConstructor but with own data 
    (own ROOT directory)
'''

''' 
@brief Class representing Chain_Algorithm_HLT table
'''
Chain_Algorithm_HLT_TableConstructor = Algorithm_HLT_TableConstructor