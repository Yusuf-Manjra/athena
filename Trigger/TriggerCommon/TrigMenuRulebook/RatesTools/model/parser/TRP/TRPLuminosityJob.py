# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jul 8, 2011

@author: galster
'''

from model.parser.TRP.TRPJob import TRPJob  

class TRPLuminosityJob(TRPJob):
    def __init__(self):
        TRPJob.__init__(self)