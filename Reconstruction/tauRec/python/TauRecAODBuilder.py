# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file TauRecAODBuilder.py
#
#@brief Main steering file to rerun parts of tau reconstruction on AODs. 
#
# Only algorithms which don't need cell level informations can be used.
# This is an example of how to run tauRec on AODs and should later be called by AODCalib, AODFix or TauD3PDMaker.
# Copied from TauRecBuilder.py/TauRecBuilder::TauRecVariablesProcessor.
#
# Algorithms schedule here are independent from the standard tauRec chain.
#
#@author Felix Friedrich
#
################################################################################


import os, sys, string

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.AlgSequence import AlgSequence
import traceback

from RecExConfig.Configured import Configured
from TauRecConfigured import TauRecConfigured

################################################################################
## @class TauRecAODProcessor
# Calculate Tau variables and properties on AODs.
################################################################################
class TauRecAODProcessor ( TauRecConfigured ) :
    """Calculate remaining Tau variables and properties. Use informations available also in AODs, so no cell level is needed."""
    
    _outputType = "xAOD::TauJetContainer"
    _outputKey = "TauJets"
    _outputDetailsType = "xAOD::TauJetAuxContainer"
    _outputDetailsKey = "TauJetsAux."
    
    def __init__(self, name = "TauProcessorAODTools", inAODmode=True, doPi0Clus=False, msglevel=3, ignoreExistingDataObject=True, sequence = None):
        self.name = name
        self.doPi0Clus = doPi0Clus
        self.msglevel = msglevel
        self.AODmode = inAODmode
        self.sequence = sequence #not used at the moment
        if sequence is None:
           self.sequence = AlgSequence() 
        TauRecConfigured.__init__(self, name)
    
    
    def configure(self):
        mlog = logging.getLogger ('TauRecAODProcessor::configure:')
        mlog.info('entering')
        
        import tauRec.TauAlgorithmsHolder as taualgs
        
        ########################################################################
        # Tau Modifier Algos 
        ########################################################################
        try:
            from tauRecTools.tauRecToolsConf import TauProcessorTool
            #TauProcessor.OutputLevel = 2
            self._TauProcessorHandle = TauProcessorTool(
                name = self.name,
                TauContainer             = self._outputKey,
                #TauDetailsContainer      = self._outputDetailsKey,
                TauAuxContainer          = self._outputDetailsKey,
                runOnAOD                 = self.AODmode,
                )
        
        except Exception:
            mlog.error("could not get handle to TauProcessor")
            print traceback.format_exc()
            return False
        
        
        tools = []
        try:
            taualgs.setAODmode(True)
            ## ATTENTION ##################################################################################
            # running these tau tools on AODs will lead to inconsistency with standard tau reconstruction
            ###############################################################################################
            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())
            tools.append(taualgs.getCombinedP4FromRecoTaus())
            tools.append(taualgs.getIDPileUpCorrection())

            # TauDiscriminant:
            from tauRec.tauRecFlags import tauFlags
            if tauFlags.doRunTauDiscriminant() :
                import TauDiscriminant.TauDiscriGetter as tauDisc
                tauDiscTools=tauDisc.getTauDiscriminantTools(mlog)
                if len(tauDiscTools)==0:
                    try: import DOESNOTEXIST
                    except Exception:
                        mlog.error("No TauDiscriminantTools appended")
                        print traceback.format_exc()
                        return False
                    pass                
                tools+=tauDiscTools
                pass
            
            
            TauRecConfigured.AddToolsToToolSvc(self, tools)
            self.TauProcessorHandle().Tools = tools
        
        except Exception:
            mlog.error("could not append tools to TauProcessor")
            print traceback.format_exc()
            return False
        
        TauRecConfigured.WrapTauRecToolExecHandle(self, tool=self.TauProcessorHandle())        
        return True    
    
    #############################################################################################
    # Helpers
    #############################################################################################
    
    def TauProcessorHandle(self):
        return self._TauProcessorHandle

