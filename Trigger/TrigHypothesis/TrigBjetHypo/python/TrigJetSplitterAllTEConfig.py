# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigJetSplitterAllTE

from AthenaCommon.SystemOfUnits import GeV


def getJetSplitterAllTEInstance( ):
    return JetSplitterAllTE( name="JetSplitterAllTE" )

def getJetSplitterFTKAllTEInstance( ):
    thejetSplitterFTK = JetSplitterAllTE( name="JetSplitterFTKAllTE" )
    thejetSplitterFTK.PriVtxKey    = "HLT_PrimVertexFTK"
    return thejetSplitterFTK

class JetSplitterAllTE (TrigJetSplitterAllTE):
    __slots__ = []
    
    def __init__(self, name):
        super( JetSplitterAllTE, self ).__init__( name )
        
        self.JetInputKey  = "TrigJetRec"
        self.JetOutputKey = "SplitJet"
        self.EtaHalfWidth = 0.4
        self.PhiHalfWidth = 0.4
        self.PriVtxKey    = "xPrimVx"

