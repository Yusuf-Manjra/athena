# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ROOT import egammaPID
from AthenaCommon.Logging import logging

log = logging.getLogger(__name__)

# Add ONNX into app service mgr (in rec-ex common style), but only in Run 2 conf style
from AthenaCommon.Configurable import Configurable
if not Configurable.configurableRun3Behavior:
    from AthOnnxruntimeService.AthOnnxruntimeServiceConf import AthONNX__ONNXRuntimeSvc
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += AthONNX__ONNXRuntimeSvc()

from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
from TrigEDMConfig.TriggerEDMRun3 import recordable
from .TrigEgammaKeys import getTrigEgammaKeys
from egammaRec.egammaRecFlags import jobproperties
from egammaRec.Factories import ToolFactory, ServiceFactory
from egammaMVACalib import egammaMVACalibConf
from xAODEgamma.xAODEgammaParameters import xAOD

log = logging.getLogger(__name__)



class TrigEgammaKeys_LRT(object):
      """Static class to collect all string manipulation in Electron_LRT sequences """
      outputElectronKey_LRT = recordable('HLT_egamma_Electrons_LRT')
      IDTrigConfig_LRT = getInDetTrigConfig('electronLRT')
      TrigElectronTracksCollectionName_LRT = IDTrigConfig_LRT.tracks_IDTrig()

class TrigEgammaKeys_GSF(object):
      """Static class to collect all string manipulation in Electron sequences """
      outputElectronKey_GSF = recordable('HLT_egamma_Electrons_GSF')
      outputTrackKey_GSF = 'HLT_IDTrkTrack_Electron_GSF'
      outputTrackParticleKey_GSF = recordable('HLT_IDTrack_Electron_GSF')

#
# Electron DNN Selectors
#
def TrigEgammaPrecisionElectronDNNSelectorCfg(name='TrigEgammaPrecisionElectronDNNSelector', ConfigFilePath=None):
    acc = ComponentAccumulator()
    # We should include the DNN here
    TrigEgammaKeys = getTrigEgammaKeys() # default configuration 
    if not ConfigFilePath:
      ConfigFilePath = 'ElectronPhotonSelectorTools/trigger/'+TrigEgammaKeys.dnnVersion
  
    import collections
    SelectorNames = collections.OrderedDict({
          'dnntight'  :'AsgElectronDNNTightSelector',
          'dnnmedium' :'AsgElectronDNNMediumSelector',
          'dnnloose'  :'AsgElectronDNNLooseSelector',
          })

    ElectronToolConfigFile = collections.OrderedDict({
          'dnntight'  :'ElectronDNNMulticlassTight.conf',
          'dnnmedium' :'ElectronDNNMulticlassMedium.conf',
          'dnnloose'  :'ElectronDNNMulticlassLoose.conf',
          })

    log.debug('Configuring electron DNN' )
    for dnnname, name in SelectorNames.items():
      SelectorTool = CompFactory.AsgElectronSelectorTool(name)
      SelectorTool.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile[dnnname]
      SelectorTool.skipDeltaPoverP = True
      acc.addPublicTool(SelectorTool)

    return acc

#
# Electron LH Selectors
#
def TrigEgammaPrecisionElectronLHSelectorCfg( name='TrigEgammaPrecisionElectronLHSelector', ConfigFilePath=None):

    # Configure the LH selectors
    acc = ComponentAccumulator()
    TrigEgammaKeys = getTrigEgammaKeys() # default configuration 
    #TrigEgammaKeys.pidVersion.set_On()
    if not ConfigFilePath:
        ConfigFilePath = 'ElectronPhotonSelectorTools/trigger/'+TrigEgammaKeys.pidVersion

    import collections
    SelectorNames = collections.OrderedDict({
          'lhtight'       :'AsgElectronLHTightSelector',
          'lhmedium'      :'AsgElectronLHMediumSelector',
          'lhloose'       :'AsgElectronLHLooseSelector',
          'lhvloose'      :'AsgElectronLHVLooseSelector',
          'lhtight_nopix' :'AsgElectronLHTightSelectorNoPix',
          'lhmedium_nopix':'AsgElectronLHMediumSelectorNoPix',
          'lhloose_nopix' :'AsgElectronLHLooseSelectorNoPix',
          'lhvloose_nopix':'AsgElectronLHVLooseSelectorNoPix',
          })
     
    ElectronToolConfigFile = collections.OrderedDict({
          'lhtight'         :'ElectronLikelihoodTightTriggerConfig.conf',
          'lhmedium'        :'ElectronLikelihoodMediumTriggerConfig.conf',
          'lhloose'         :'ElectronLikelihoodLooseTriggerConfig.conf',
          'lhvloose'        :'ElectronLikelihoodVeryLooseTriggerConfig.conf',
          'lhtight_nopix'   :'ElectronLikelihoodTightTriggerConfig_NoPix.conf',
          'lhmedium_nopix'  :'ElectronLikelihoodMediumTriggerConfig_NoPix.conf',
          'lhloose_nopix'   :'ElectronLikelihoodLooseTriggerConfig_NoPix.conf',
          'lhvloose_nopix'  :'ElectronLikelihoodVeryLooseTriggerConfig_NoPix.conf',
          })

    log.debug('Configuring electron PID' )
    for pidname, name in SelectorNames.items():
      SelectorTool = CompFactory.AsgElectronLikelihoodTool(name)
      SelectorTool.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile[pidname]
      SelectorTool.usePVContainer = False 
      SelectorTool.skipDeltaPoverP = True
      acc.addPublicTool(SelectorTool)
    return acc


#
# Electron CB Selectors
#

def TrigEgammaPrecisionElectronCBSelectorCfg(name='TrigEgammaPrecisionElectronCBSelector', ConfigFilePath=None):
    acc = ComponentAccumulator()
    TrigEgammaKeys = getTrigEgammaKeys() # default configuration 
    from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import BitDefElectron

    ElectronLooseHI = (0
            | 1 << BitDefElectron.ClusterEtaRange_Electron
            | 1 << BitDefElectron.ClusterHadronicLeakage_Electron
            | 1 << BitDefElectron.ClusterMiddleEnergy_Electron
            | 1 << BitDefElectron.ClusterMiddleEratio37_Electron
            | 1 << BitDefElectron.ClusterMiddleWidth_Electron
            | 1 << BitDefElectron.ClusterStripsWtot_Electron
    )

    ElectronMediumHI = (ElectronLooseHI
            | 1 << BitDefElectron.ClusterMiddleEratio33_Electron
            | 1 << BitDefElectron.ClusterBackEnergyFraction_Electron
            | 1 << BitDefElectron.ClusterStripsEratio_Electron
            | 1 << BitDefElectron.ClusterStripsDeltaEmax2_Electron
            | 1 << BitDefElectron.ClusterStripsDeltaE_Electron
            | 1 << BitDefElectron.ClusterStripsFracm_Electron
            | 1 << BitDefElectron.ClusterStripsWeta1c_Electron
    )

    if not ConfigFilePath:
        ConfigFilePath = 'ElectronPhotonSelectorTools/trigger/'+TrigEgammaKeys.pidVersion

    from collections import OrderedDict
    SelectorNames = OrderedDict({
          'medium': 'AsgElectronIsEMSelectorHIMedium',
          'loose': 'AsgElectronIsEMSelectorHILoose',
          'mergedtight'  : 'AsgElectronIsEMSelectorMergedTight',
    })

    ElectronToolConfigFile = {
          'medium': 'ElectronIsEMMediumSelectorCutDefs.conf',
          'loose': 'ElectronIsEMLooseSelectorCutDefs.conf',
          'mergedtight'  : 'ElectronIsEMMergedTightSelectorCutDefs.conf',
    }

    ElectronMaskBits = {
          'medium': ElectronMediumHI,
          'loose': ElectronLooseHI,
          'mergedtight'  : egammaPID.ElectronTightHLT,
    }

    for sel, name in SelectorNames.items():
        SelectorTool = CompFactory.AsgElectronIsEMSelector(name)
        SelectorTool.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile[sel]
        SelectorTool.isEMMask = ElectronMaskBits[sel]
        acc.addPublicTool(SelectorTool)
    
    return acc


#
# Photon IsEM selectors
#
def createTrigEgammaPrecisionPhotonSelectors(ConfigFilePath=None):
    from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
    TrigEgammaKeys = getTrigEgammaKeys() # default configuration 

    if not ConfigFilePath:
      ConfigFilePath = 'ElectronPhotonSelectorTools/trigger/'+TrigEgammaKeys.pidVersion

    import collections
    # Configure the IsEM selectors
    SelectorNames = collections.OrderedDict( {
            'tight'  : 'TightPhotonSelector',
            'medium' : 'MediumPhotonSelector',
            'loose'  : 'LoosePhotonSelector',
            } )
    SelectorPID = {
            'loose'  : egammaPID.PhotonIDLoose,
            'medium' : egammaPID.PhotonIDMedium,
            'tight'  : egammaPID.PhotonIDTight,
            }
    PhotonToolConfigFile = {
            'loose'  : 'PhotonIsEMLooseSelectorCutDefs.conf', 
            'medium' : 'PhotonIsEMMediumSelectorCutDefs.conf', 
            'tight'  : 'PhotonIsEMTightSelectorCutDefs.conf',
            } 
    PhotonIsEMBits = {
            'loose'  : egammaPID.PhotonLoose,
            'medium' : egammaPID.PhotonMedium,
            'tight'  : egammaPID.PhotonTight,
            }
    selectors = []
    for sel, name in SelectorNames.items():
        log.debug('Configuring photon PID for %s', sel)
        SelectorTool = ConfiguredAsgPhotonIsEMSelector(name, SelectorPID[sel])
        ConfigFile = ConfigFilePath + '/' + PhotonToolConfigFile[sel] 
        log.debug('Configuration file: %s', ConfigFile)
        SelectorTool.ConfigFile = ConfigFile
        SelectorTool.ForceConvertedPhotonPID = True
        SelectorTool.isEMMask = PhotonIsEMBits[sel] 
        selectors.append(SelectorTool)

    return selectors




def createTrigEgammaFastCaloSelectors(ConfigFilePath=None):

    import collections
    TrigEgammaKeys = getTrigEgammaKeys() # default configuration 

    # We should include the ringer here
    if not ConfigFilePath:
      ConfigFilePath = 'RingerSelectorTools/'+TrigEgammaKeys.ringerVersion

  
    SelectorNames = collections.OrderedDict({
            'tight'    : 'AsgElectronFastCaloRingerTightSelectorTool',
            'medium'   : 'AsgElectronFastCaloRingerMediumSelectorTool',
            'loose'    : 'AsgElectronFastCaloRingerLooseSelectorTool',
            'vloose'   : 'AsgElectronFastCaloRingerVeryLooseSelectorTool',
            })
        

    ToolConfigFile = collections.OrderedDict({
          #                               ET < 15 GeV                                                  ET >= 15 GeV
          'tight'   :['ElectronJpsieeRingerTightTriggerConfig_RingsOnly.conf'     , 'ElectronZeeRingerTightTriggerConfig_RingsOnly.conf'    ],
          'medium'  :['ElectronJpsieeRingerMediumTriggerConfig_RingsOnly.conf'    , 'ElectronZeeRingerMediumTriggerConfig_RingsOnly.conf'   ],
          'loose'   :['ElectronJpsieeRingerLooseTriggerConfig_RingsOnly.conf'     , 'ElectronZeeRingerLooseTriggerConfig_RingsOnly.conf'    ],
          'vloose'  :['ElectronJpsieeRingerVeryLooseTriggerConfig_RingsOnly.conf' , 'ElectronZeeRingerVeryLooseTriggerConfig_RingsOnly.conf'],
          })
    
    selectors = []    
    for pidname , name in SelectorNames.items():
      log.debug('Configuring electron ringer PID for %s', pidname)
      SelectorTool=CompFactory.Ringer.AsgRingerSelectorTool(name)
      SelectorTool.ConfigFiles = [ (ConfigFilePath+'/'+path) for path in ToolConfigFile[pidname] ]
      selectors.append(SelectorTool)
    return selectors



#
# Electron/Photon ringer NN selectors
#
def createTrigEgammaFastElectronSelectors(ConfigFilePath=None):

    import collections
    TrigEgammaKeys = getTrigEgammaKeys() # default configuration 

    # We should include the ringer here
    if not ConfigFilePath:
      ConfigFilePath = 'RingerSelectorTools/'+TrigEgammaKeys.ringerVersion
  

    SelectorNames = collections.OrderedDict({
            'tight'    : 'AsgElectronFastElectronRingerTightSelectorTool',
            'medium'   : 'AsgElectronFastElectronRingerMediumSelectorTool',
            'loose'    : 'AsgElectronFastElectronRingerLooseSelectorTool',
            'vloose'   : 'AsgElectronFastElectronRingerVeryLooseSelectorTool',
            })
        
    ToolConfigFile = collections.OrderedDict({
          #                               ET < 15 GeV                                       ET >= 15 GeV
          'tight'   :['ElectronJpsieeRingerTightTriggerConfig.conf'     , 'ElectronZeeRingerTightTriggerConfig.conf'    ],
          'medium'  :['ElectronJpsieeRingerMediumTriggerConfig.conf'    , 'ElectronZeeRingerMediumTriggerConfig.conf'   ],
          'loose'   :['ElectronJpsieeRingerLooseTriggerConfig.conf'     , 'ElectronZeeRingerLooseTriggerConfig.conf'    ],
          'vloose'  :['ElectronJpsieeRingerVeryLooseTriggerConfig.conf' , 'ElectronZeeRingerVeryLooseTriggerConfig.conf'],
          })
    selectors = []    
    for pidname , name in SelectorNames.items():
      log.debug('Configuring electron ringer PID for %s', pidname)
      SelectorTool=CompFactory.Ringer.AsgRingerSelectorTool(name)
      SelectorTool.ConfigFiles = [ (ConfigFilePath+'/'+path) for path in ToolConfigFile[pidname] ]
      selectors.append(SelectorTool)
    return selectors


#
# Ringer fast photon selectors
#
def createTrigEgammaFastPhotonSelectors(ConfigFilePath=None):

    import collections
    TrigEgammaKeys = getTrigEgammaKeys() # default configuration 
    # We should include the ringer here
    if not ConfigFilePath:
      ConfigFilePath = 'RingerSelectorTools/'+TrigEgammaKeys.ringerVersion

    SelectorNames = collections.OrderedDict( {
      'tight'  : 'AsgPhotonFastCaloRingerTightSelectorTool' ,
      'medium' : 'AsgPhotonFastCaloRingerMediumSelectorTool',
      'loose'  : 'AsgPhotonFastCaloRingerLooseSelectorTool' ,
    } )
    
    ToolConfigFile = collections.OrderedDict({
      'tight' : ['PhotonRingerTightTriggerConfig.conf' ],
      'medium': ['PhotonRingerMediumTriggerConfig.conf'],
      'loose' : ['PhotonRingerLooseTriggerConfig.conf' ],
    })

    selectors = []
    for pidname , name in SelectorNames.items():
      log.debug('Configuring electron ringer PID for %s', pidname)
      SelectorTool=CompFactory.Ringer.AsgRingerSelectorTool(name)
      SelectorTool.ConfigFiles = [ (ConfigFilePath+'/'+path) for path in ToolConfigFile[pidname] ]
      selectors.append(SelectorTool)
    return selectors



def createTrigEgammaMVASvc( ConfigFilePath=None ):

    if not ConfigFilePath:
      ConfigFilePath = jobproperties.egammaRecFlags.calibMVAVersion()

    trigElectronMVATool = ToolFactory(
        egammaMVACalibConf.egammaMVACalibTool,
        name="TrigElectronMVATool",
        ParticleType=xAOD.EgammaParameters.electron,
        folder=ConfigFilePath )
    trigUnconvPhotonMVATool = ToolFactory(
        egammaMVACalibConf.egammaMVACalibTool,
        name="TrigUnconvPhotonMVATool",
        ParticleType=xAOD.EgammaParameters.unconvertedPhoton,
        folder=ConfigFilePath )
    trigConvertedPhotonMVATool = ToolFactory(
        egammaMVACalibConf.egammaMVACalibTool,
        name="TrigConvertePhotonMVATool",
        ParticleType=xAOD.EgammaParameters.convertedPhoton,
        folder=ConfigFilePath)
    trigEgammaMVASvc = ServiceFactory(
        egammaMVACalibConf.egammaMVASvc,
        name = "TrigEgammaMVASvc",
        ElectronTool=trigElectronMVATool,
        ConvertedPhotonTool=trigConvertedPhotonMVATool,
        UnconvertedPhotonTool=trigUnconvPhotonMVATool)
    return trigEgammaMVASvc

