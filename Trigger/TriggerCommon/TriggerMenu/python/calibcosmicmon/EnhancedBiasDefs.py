# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#########################################################################
#
# SliceDef file for HLT signatures used for EnhancedBias Slice
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger('TriggerMenu.calibcosmon.EnhancedBiasDef')

from TriggerMenu.menu.HltConfig import L2EFChainDef

#########################################################################

##USUAL STUPID HARDCODING OF THE ITEM LISTS DEPENDING ON THE MENU     
l1_seeds ={ 
    
    # Low threshold prescaled L1 items - slected at HLT based on TBP bit from L1 in random-seeded events        
    'low'  : ['L1_MU6', 'L1_EM10VH', 
              'L1_J30.0ETA49_2J20.0ETA49', 'L1_TAU12',
              'L1_MU4_J12', 'L1_2EM7',
              'L1_2MU4', 'L1_XS30', 
              'L1_J15', 'L1_ZB'],

    # High(er) threshold prescaled L1 items - slected at HLT based on TBP bit from L1 in random-seeded events 
    'high' : ['L1_3J15','L1_EM18VH',
              'L1_EM20VHI',  'L1_2EM10VH', 
              'L1_J50', 'L1_TAU30',
              'L1_MU15','L1_TAU20_2TAU12', 
              'L1_XE35', 'L1_MU6_2MU4',
              'L1_MU6_J20', 'L1_EM7_MU10',
              'L1_EM15VH_3EM7', 'L1_2EM15',
              'L1_EM15I_MU4', 'L1_2MU6',
              'L1_TAU20_2TAU12_XE35', 
              'L1_DR-TAU20ITAU12I', 'L1_DY-BOX-2MU6', # Topo
              'L1_DY-DR-2MU4', # Topo
              'L1_2MU4-B', 'L1_BTAG-MU4J15', # Topo
              'L1_MJJ-100', 'L1_J4-MATCH', # Topo
              'L1_HT190-J15.ETA21', #Topo,
              'L1_2MU4_J20_XE30_DPHI-J20s2XE30',  # 2017
              'L1_MU4_J20_XE30_DPHI-J20s2XE30', # 2017
              'L1_J40_XE50_DPHI-J20s2XE50', # 2017
              'L1_2MU4_J40_XE20', # 2017
              'L1_MU4_J50_XE40'] # 2017

    }

#########################################################################

class L2EFChain_EnhancedBiasTemplate(L2EFChainDef):
    def __init__(self, chainDict):
        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []
        
        self.chainName = chainDict['chainName']
        self.chainPart = chainDict['chainParts']  
        self.chainL1Item = chainDict['L1item']       
        self.chainCounter = chainDict['chainCounter']       
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        self.chainPartName = self.chainPart['chainPartName']             
        self.algType  = self.chainPart['algType']
        
        self.L2InputTE = self.chainL1Item 
        newL1items = self.L2InputTE 


        # sequence/signature/TErenaming
        from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo        
        from TrigHypoCommonTools.TrigHypoCommonToolsConf import L1InfoHypo
        
        self.dummyAlg = PESA__DummyUnseededAllTEAlgo("EF_DummyFEX_%s" %(self.algType))
        self.dummyAlg.createRoIDescriptors  = False
        self.dummyAlg.NumberOfOutputTEs     = 1
            
        self.hypoEB = L1InfoHypo("EF_Hypo_%s" %(self.algType))
        
        if self.algType in l1_seeds:
            self.hypoEB.L1ItemNames = l1_seeds[self.algType]        
            self.hypoEB.AlwaysPass                = False
            self.hypoEB.InvertSelection           = False
            self.hypoEB.InvertL1ItemNameSelection = False
            self.hypoEB.L1TriggerBitMask          = 0
            self.hypoEB.TriggerTypeBitMask        = 0
            self.hypoEB.UseBeforePrescaleBit      = True

            self.EFsequenceList += [['',
                                     [self.dummyAlg, self.hypoEB],
                                     'EF_eb']]            
            self.EFsignatureList += [[['EF_eb']]]
            self.TErenamingDict = {
                'EF_eb':     'EF_' + self.chainName,
                }
        
        else:
            log.error("No configuration exist for EB chain: %s" %(self.algType))
            self.hypoEB.L1ItemNames = []


        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter,
                              newL1items, self.EFName, self.chainCounter, self.L2InputTE)

            
    def defineSequences(self):   
        for sequence in self.L2sequenceList:
            self.addL2Sequence(*sequence)            
        for sequence in self.EFsequenceList:
            self.addEFSequence(*sequence)
          
    def defineSignatures(self):
        for signature in self.L2signatureList:
            self.addL2Signature(*signature)
        for signature in self.EFsignatureList:
            self.addEFSignature(*signature)          
           
    def defineTErenaming(self):
        self.TErenamingMap=self.TErenamingDict


        
