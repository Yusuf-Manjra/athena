# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Jet.JetDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, RecoFragmentsPool

#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class JetChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

        from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoConfiguration import extractRecoDict
        self.recoDict = extractRecoDict(self.dict["chainParts"])

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        log.debug("Assembling chain " + self.chainName)

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        # Only one step for now, but we might consider adding steps for
        # reclustering and trimming workflows
        steps=[self.getJetChainStep()]

        chainSteps = []
        for step in steps:
            chainSteps+=[step]
    
        myChain = self.buildChain(chainSteps)
        return myChain
        

    # --------------------
    # Configuration of steps
    # --------------------
    def getJetChainStep(self):
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetMenuSequence
        from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoSequences import jetRecoDictToString

        jetDefStr = jetRecoDictToString(self.recoDict)

        stepName = "Step1_jet_"+jetDefStr
        jetSeq1 = RecoFragmentsPool.retrieve( jetMenuSequence, None, **self.recoDict ) # the None will be used for flags in future
        return ChainStep(stepName, [jetSeq1], multiplicity=[1])


        
            
            

        
                
