# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# AUTHOR:   Marcin.Wolter@cern.ch
# CREATED:  20 March 2008
# 
# 23 Nov 2010: cleaning up (Noel Dawe)

from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

from AthenaCommon.AppMgr import ToolSvc

import traceback

from TrigTauDiscriminant.TrigTauDiscriminantConf import TrigTauDiscriBuilder

def singleton(cls):

    log = logging.getLogger('%s::__init__'% cls.__name__) 
    instances = {}
    def getinstance(*args, **kwargs):
        if cls in instances:
            log.warning("Attempting to construct more than one %s. Returning the singleton."% cls.__name__)
            return instances[cls]
        obj = cls(*args, **kwargs)
        instances[cls] = obj
        return obj
    return getinstance

#@singleton

class TrigTauDiscriGetter(TrigTauDiscriBuilder):
    __slots__ = [ '_mytools']
    def __init__(self, name = "TrigTauDiscriminant"):
         super( TrigTauDiscriGetter , self ).__init__( name )

         #monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
         from TrigTauDiscriminant.TrigTauDiscriminantMonitoring import TrigTauDiscriminantValidationMonitoring, TrigTauDiscriminantOnlineMonitoring 
         validation = TrigTauDiscriminantValidationMonitoring()        
         online     = TrigTauDiscriminantOnlineMonitoring()
         
         from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
         time = TrigTimeHistToolConfig("Time")
         
         self.AthenaMonTools = [ time, validation, online ]
         
         self.Tools = [self.LLHtoolset(), self.BDTtoolset()]
    
    def LLHtoolset(self):
        tools = []
        mlog = logging.getLogger('TrigTauDiscriGetter::configure LLH:')
        from TauDiscriminant.TauDiscriminantConf import TauLLH
        tauLLH = TauLLH()
        tauLLH.FileNameTauPDF = "pdfs_tau_trigger.root"
        tauLLH.FileNameJetPDF = "pdfs_jet_trigger.root"
        return tauLLH


    def BDTtoolset(self):
        from TauDiscriminant.TauDiscriminantConf import TauJetBDT
        return TauJetBDT(jetBDT = "trigger.jet.BDT.bin",
                         jetSigBits = "trigger.sig.bits.jet.BDT.txt")

class TrigTauDiscriGetter2015(TrigTauDiscriBuilder):
    __slots__ = [ '_mytools']
    def __init__(self, name = "TrigTauDiscriminant2015"):
         super( TrigTauDiscriGetter2015 , self ).__init__( name )

         #monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
         from TrigTauDiscriminant.TrigTauDiscriminantMonitoring import TrigTauDiscriminantValidationMonitoring, TrigTauDiscriminantOnlineMonitoring 
         validation = TrigTauDiscriminantValidationMonitoring()        
         online     = TrigTauDiscriminantOnlineMonitoring()
         
         from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
         time = TrigTimeHistToolConfig("Time")
         
         self.AthenaMonTools = [ time, validation, online ]
         
         self.Tools = [self.LLHtoolset(), self.BDTtoolset()]
    
    def LLHtoolset(self):
        tools = []
        mlog = logging.getLogger('TrigTauDiscriGetter2015::configure LLH:')
        from TauDiscriminant.TauDiscriminantConf import TauLLH
        tauLLH = TauLLH()
        tauLLH.FileNameTauPDF = "pdfs_tau_trigger.root"
        tauLLH.FileNameJetPDF = "pdfs_jet_trigger.root"
        return tauLLH


    def BDTtoolset(self):
        from TauDiscriminant.TauDiscriminantConf import TauJetBDT
        return TauJetBDT(jetBDT = "offline.jet.BDT.MC15.bin",
                         jetSigBits = "trigger.sig.bits.jet.BDT.MC15.ver1.txt")
