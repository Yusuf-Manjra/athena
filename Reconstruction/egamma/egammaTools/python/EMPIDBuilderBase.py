# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the EMPIDBuilder
from AthenaCommon.Logging import logging
from AthenaCommon.DetFlags import DetFlags

import traceback

from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu
from ElectronPhotonSelectorTools.PhotonIsEMSelectorMapping import photonPIDmenu

#import base class
from egammaTools.egammaToolsConf import EMPIDBuilder

import PyCintex
PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import egammaPID
from ROOT import LikeEnum

class EMPIDBuilderElectronBase ( EMPIDBuilder ) :
    __slots__ = ()

    def __init__(self, name="EMPIDBuilderElectronBase"):
        EMPIDBuilder.__init__(self,name)
        mlog = logging.getLogger(name+'::__init__')
        mlog.debug("entering")

        from AthenaCommon.AppMgr import ToolSvc

       # Electron Selectors
        try:
            # Cut based 
            from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
            LooseElectronSelector = ConfiguredAsgElectronIsEMSelector("LooseElectronSelector", egammaPID.ElectronIDLoosePP)
            ToolSvc+=LooseElectronSelector
            MediumElectronSelector = ConfiguredAsgElectronIsEMSelector("MediumElectronSelector", egammaPID.ElectronIDMediumPP)
            ToolSvc+=MediumElectronSelector
            TightElectronSelector = ConfiguredAsgElectronIsEMSelector("TightElectronSelector", egammaPID.ElectronIDTightPP)
            ToolSvc+=TightElectronSelector


            # Likelihood
            from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool
            LooseLHSelector = ConfiguredAsgElectronLikelihoodTool("LooseLHSelector", LikeEnum.Loose)
            LooseLHSelector.primaryVertexContainer="PrimaryVertices" 
            ToolSvc+=LooseLHSelector
            MediumLHSelector = ConfiguredAsgElectronLikelihoodTool("MediumLHSelector", LikeEnum.Medium)
            MediumLHSelector.primaryVertexContainer="PrimaryVertices"
            ToolSvc+=MediumLHSelector
            TightLHSelector = ConfiguredAsgElectronLikelihoodTool("TightLHSelector", LikeEnum.Tight)
            TightLHSelector.primaryVertexContainer="PrimaryVertices"
            ToolSvc+=TightLHSelector

            # Multi Lepton
            from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronMultiLeptonSelector
            MultiLeptonSelector=AsgElectronMultiLeptonSelector("MultiLeptonSelector")
            ToolSvc+=MultiLeptonSelector

        except:
            mlog.error("could not get configure tools")
            print traceback.format_exc()
            return False

        electronSelectors = [LooseElectronSelector, MediumElectronSelector, TightElectronSelector,
                            LooseLHSelector, MediumLHSelector, TightLHSelector, MultiLeptonSelector]
        electronSelectorNames = ["Loose", "Medium", "Tight", "LHLoose","LHMedium","LHTight","MultiLepton"]
            
        self.selectors=electronSelectors
        self.selectorResultNames=electronSelectorNames

class EMPIDBuilderPhotonBase ( EMPIDBuilder ) :
    __slots__ = ()

    def __init__(self, name="EMPIDBuilderPhotonBase"):
        EMPIDBuilder.__init__(self,name)
        mlog = logging.getLogger(name+'::__init__')
        mlog.debug("entering")

        from AthenaCommon.AppMgr import ToolSvc

        # photon Selectors
        try:
            from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
            LoosePhotonSelector = ConfiguredAsgPhotonIsEMSelector("LoosePhotonSelector", egammaPID.PhotonIDLoose)
            ToolSvc+=LoosePhotonSelector
            TightPhotonSelector = ConfiguredAsgPhotonIsEMSelector("TightPhotonSelector", egammaPID.PhotonIDTight)
            ToolSvc+=TightPhotonSelector


        except:
            mlog.error("could not get configure tools")
            print traceback.format_exc()
            return False

        photonSelectors = [LoosePhotonSelector, TightPhotonSelector]
        photonSelectorNames = ["Loose", "Tight"]
            
        self.selectors=photonSelectors
        self.selectorResultNames=photonSelectorNames
