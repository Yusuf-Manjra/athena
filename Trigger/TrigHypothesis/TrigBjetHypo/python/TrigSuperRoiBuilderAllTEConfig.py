# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigSuperRoiBuilderAllTE

from AthenaCommon.SystemOfUnits import GeV


def getSuperRoiBuilderAllTEInstance( ):
    return SuperRoiBuilderAllTE( name="SuperRoiBuilderAllTE" )


class SuperRoiBuilderAllTE (TrigSuperRoiBuilderAllTE):
    __slots__ = []
    
    def __init__(self, name):
        super( SuperRoiBuilderAllTE, self ).__init__( name )
        
        self.JetInputKey  = "TrigJetRec"
        self.JetOutputKey = "SuperRoi"
        self.EtaHalfWidth = 0.2
        self.PhiHalfWidth = 0.2
        self.JetMinEt     = 30.0    
