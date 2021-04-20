# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Egamma.ElectronChainConfiguration")


from ..Menu.ChainConfigurationBase import ChainConfigurationBase
from ..CommonSequences.CaloSequences import fastCaloMenuSequence

from .ElectronMenuSequences import fastElectronMenuSequence
from .ElectronMenuSequences_LRT import fastElectronMenuSequence_LRT
from .PrecisionCaloMenuSequences import precisionCaloMenuSequence
from .PrecisionCaloMenuSequences_LRT import precisionCaloMenuSequence_LRT
from .PrecisionElectronMenuSequences import precisionElectronMenuSequence
from .PrecisionElectronMenuSequences_GSF import precisionElectronMenuSequence_GSF
from .PrecisionElectronMenuSequences_LRT import precisionElectronMenuSequence_LRT
from .PrecisionTrackingMenuSequences import precisionTrackingMenuSequence
from .PrecisionTrackingMenuSequences_LRT import precisionTrackingMenuSequence_LRT

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------

def electronFastCaloCfg( flags ):
    return fastCaloMenuSequence("Electron")

def fastElectronSequenceCfg( flags ):
    return fastElectronMenuSequence(do_idperf=False)

def fastElectronSequenceCfg_idperf( flags ):
    return fastElectronMenuSequence(do_idperf=True)

def fastElectronSequenceCfg_lrt( flags ):
    return fastElectronMenuSequence_LRT()

def precisionCaloSequenceCfg( flags ):
    return precisionCaloMenuSequence('Electron')

def precisionCaloSequenceCfg_lrt( flags ):
    return precisionCaloMenuSequence_LRT('Electron')

def precisionTrackingSequenceCfg( flags ):
    return precisionTrackingMenuSequence('Electron')

def precisionTrackingSequenceCfg_lrt( flags ):
    return precisionTrackingMenuSequence_LRT('Electron')

def precisionElectronSequenceCfg( flags ):
    return precisionElectronMenuSequence()

def precisionGSFElectronSequenceCfg( flags ):
    return precisionElectronMenuSequence_GSF()

def precisionElectronSequenceCfg_lrt( flags ):
    return precisionElectronMenuSequence_LRT()

# this must be moved to the HypoTool file:
def diElectronZeeMassComboHypoToolFromDict(chainDict):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaMassHypoTool
    name = chainDict['chainName']
    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [defineHistogram('MassOfAccepted', type='TH1F', path='EXPERT', title="Mass in accepted combinations [MeV]", xbins=75, xmin=0, xmax=150000)]
    tool= TrigEgammaMassHypoTool(name)
    tool.LowerMassElectronClusterCut = 50000
    tool.UpperMassElectronClusterCut = 130000
    monTool.HistPath = 'EgammaMassHypo/'+tool.getName()
    tool.MonTool = monTool
    return tool

def diElectronJpsieeMassComboHypoToolFromDict(chainDict):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaMassHypoTool
    name = chainDict['chainName']
    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [defineHistogram('MassOfAccepted', type='TH1F', path='EXPERT', title="Mass in accepted combinations [MeV]", xbins=75, xmin=0, xmax=150000)]
    tool= TrigEgammaMassHypoTool(name)
    tool.LowerMassElectronClusterCut = 1000
    tool.UpperMassElectronClusterCut = 5000
    monTool.HistPath = 'EgammaMassHypo/'+tool.getName()
    tool.MonTool = monTool
    return tool
#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class ElectronChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
  
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):
        chainSteps = []
        log.debug("Assembling chain for %s", self.chainName)

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration
        # --------------------

        stepDictionary = {
                'etcut1step': ['getFastCalo'],
                'vlooseidperf'    : ['getFastCalo', 'getFastElectron_idperf', 'getPrecisionCaloElectron', 'getPrecisionTracking'],
                'looseidperf'    : ['getFastCalo', 'getFastElectron_idperf', 'getPrecisionCaloElectron', 'getPrecisionTracking'],
                'mediumidperf'    : ['getFastCalo', 'getFastElectron_idperf', 'getPrecisionCaloElectron', 'getPrecisionTracking'],
                'tightidperf'    : ['getFastCalo', 'getFastElectron_idperf', 'getPrecisionCaloElectron', 'getPrecisionTracking'],
                'etcut'     : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking'],
                'lhloose'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhvloose'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhmedium'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhtight'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'etcutnoringer'     : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking'],
                'lhloosenoringer'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhvloosenoringer'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhmediumnoringer'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhtightnoringer'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhlooseivarloose'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhlooseivarmedium' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhlooseivartight'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhmediumivarloose' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhmediumivarmedium': ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhmediumivartight' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhtightivarloose'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhtightivarmedium'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhtightivartight'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
        # gsf sequences. For now just settin gup as normal non-gsf chains
                'lhloosegsf'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhvloosegsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhmediumgsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhtightgsf'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhlooseivarloosegsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhlooseivarmediumgsf' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhlooseivartightgsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhmediumivarloosegsf' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhmediumivarmediumgsf': ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhmediumivartightgsf' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhtightivarloosegsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhtightivarmediumgsf' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhtightivartightgsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
          # lrt chains
                'lhlooselrtloose'  : ['getFastCalo', 'getFastElectron_lrt', 'getPrecisionCaloElectron_lrt', 'getPrecisionTracking_lrt', 'getPrecisionElectron_lrt'],
                'lhmediumlrtmedium'  : ['getFastCalo', 'getFastElectron_lrt', 'getPrecisionCaloElectron_lrt', 'getPrecisionTracking_lrt', 'getPrecisionElectron_lrt'],
                'lhtightlrttight'   : ['getFastCalo', 'getFastElectron_lrt', 'getPrecisionCaloElectron_lrt', 'getPrecisionTracking_lrt', 'getPrecisionElectron_lrt'],

                }

        log.debug('electron chain part = %s', self.chainPart)
        key = self.chainPart['extra'] + self.chainPart['IDinfo'] + self.chainPart['isoInfo'] + self.chainPart['trkInfo'] + self.chainPart['lrtInfo']
        addInfo = 'etcut'
        L2IDAlg = 'noringer'

        for addInfo in self.chainPart['addInfo']:
            key+=addInfo
        
        for L2IDAlg in self.chainPart['L2IDAlg']:
            key+=L2IDAlg


        log.debug('electron key = %s', key)
        if key in stepDictionary:
            steps=stepDictionary[key]
        else:
            raise RuntimeError("Chain configuration unknown for electron chain with key: " + key )
        
        for step in steps:
            log.debug('Adding electron trigger step %s', step)
            chainstep = getattr(self, step)()
            chainSteps+=[chainstep]

        myChain = self.buildChain(chainSteps) 
        
        return myChain

    # --------------------
    # Configuration of electron steps
    # --------------------

    def getFastCalo(self):
        stepName       = "FastCalo_electron"
        fastCaloCfg    = electronFastCaloCfg
        return self.getStep(1,stepName,[ fastCaloCfg])

    def getFastElectron(self):
        stepName = "fast_electron"
        return self.getStep(2,stepName,[ fastElectronSequenceCfg])

    def getFastElectron_lrt(self):
        stepName = "fast_electron_lrt"
        return self.getStep(2,stepName,[ fastElectronSequenceCfg_lrt])
   
    def getFastElectron_idperf(self):
        stepName = "fast_electron_idperf"
        return self.getStep(2,stepName,[ fastElectronSequenceCfg_idperf])

    def getPrecisionCaloElectron(self):
        stepName = "precisionCalo_electron"
        return self.getStep(3,stepName,[ precisionCaloSequenceCfg])
    
    def getPrecisionCaloElectron_lrt(self):
        stepName = "precisionCalo_electron_lrt"
        return self.getStep(3,stepName,[ precisionCaloSequenceCfg_lrt])

    def getPrecisionTracking(self):
        stepName = "precisionTracking_electron"
        return self.getStep(4,stepName,[ precisionTrackingSequenceCfg])

    def getPrecisionTracking_lrt(self):
        stepName = "precisionTracking_electron_lrt"
        return self.getStep(4,stepName,[ precisionTrackingSequenceCfg_lrt])

    def getPrecisionElectron(self):

        isocut = self.chainPart['isoInfo']
        log.debug(' isolation cut = %s', isocut)

        if "Zee" in self.chainName:
            stepName = "precision_topoelectron"+str(isocut)
            return self.getStep(5,stepName,sequenceCfgArray=[precisionElectronSequenceCfg], comboTools=[diElectronZeeMassComboHypoToolFromDict])
        elif "Jpsiee" in self.chainName:
            stepName = "precision_topoelectron"+str(isocut)
            return self.getStep(5,stepName,sequenceCfgArray=[precisionElectronSequenceCfg], comboTools=[diElectronJpsieeMassComboHypoToolFromDict])
        else:
            stepName = "precision_electron"+str(isocut)
            return self.getStep(5,stepName,[ precisionElectronSequenceCfg])

    def getPrecisionGSFElectron(self):

        isocut = self.chainPart['isoInfo']
        log.debug(' isolation cut = ' + str(isocut))

        if "Zee" in self.chainName:
            stepName = "precision_topoelectron_GSF"+str(isocut)
            return self.getStep(5,stepName,sequenceCfgArray=[precisionGSFElectronSequenceCfg], comboTools=[diElectronZeeMassComboHypoToolFromDict])
        if "Jpsiee" in self.chainName:
            stepName = "precision_topoelectron_GSF"+str(isocut)
            return self.getStep(5,stepName,sequenceCfgArray=[precisionGSFElectronSequenceCfg], comboTools=[diElectronJpsieeMassComboHypoToolFromDict])
        else:
            stepName = "precision_electron_GSF"+str(isocut)
            return self.getStep(5,stepName,[ precisionGSFElectronSequenceCfg])

    def getPrecisionElectron_lrt(self):

        isocut = self.chainPart['isoInfo']
        log.debug(' isolation cut = ' + str(isocut))

        stepName = "precision_electron_lrt"+str(isocut)
        return self.getStep(5,stepName,[ precisionElectronSequenceCfg_lrt])
