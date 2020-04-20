# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from enum import Enum, unique

from past.builtins import cmp

from AthenaCommon.Logging import logging
log = logging.getLogger("Menu.L1.Base.ThresholdType")

@unique
class ThrType( Enum ):

    # run 3 calo and muon thresholds
    eEM = 1; eTAU = 2; jJ = 3; gJ = 4; gXE = 5; jXE = 6; MU = 7 # noqa: E702

    # NIM thresholds
    BCM = 21; BCMCMB = 22; LUCID = 23; ZDC = 24; BPTX = 25; CALREQ = 26; MBTS = 27; MBTSSI = 28; NIM = 29 # noqa: E702

    # legacy calo thresholds
    EM = 41; TAU = 42; JET = 43; JE = 44; XE = 45; TE = 46; XS = 47 # noqa: E702

    # zero bias threshold on CTPIN
    ZB = 50 

    # topo thresholds
    TOPO = 60; MUTOPO = 61; MULTTOPO = 62; R2TOPO = 63 # noqa: E702

    # ALFA thresholds
    ALFA = 70

    @staticmethod
    def LegacyTypes():
        return [ ThrType.EM, ThrType.TAU, ThrType.JET, ThrType.XE, ThrType.TE, ThrType.XS, ThrType.ZB ]
    
    @staticmethod
    def Run3Types():
        return [ ThrType.MU, ThrType.eEM, ThrType.eTAU, ThrType.jJ, ThrType.gJ, ThrType.gXE, ThrType.jXE ]
    
    @staticmethod
    def NIMTypes():
        return [ ThrType.BCM, ThrType.BCMCMB, ThrType.LUCID, ThrType.ZDC, ThrType.BPTX, ThrType.CALREQ, ThrType.MBTS, ThrType.MBTSSI, ThrType.NIM ]
    
    def __repr__(self):
        return self.name

    def __str__(self):
        return self.name

    def __cmp__(self,other):
        return cmp(self.name, other)
