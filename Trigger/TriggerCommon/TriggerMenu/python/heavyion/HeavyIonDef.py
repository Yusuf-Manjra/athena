# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for heavy ion chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

logHeavyIonDef = logging.getLogger("TriggerMenu.heavyion.HeavyIonDef")

from AthenaCommon import CfgGetter
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

import re

from TriggerJobOpts.TriggerFlags            import TriggerFlags

from TriggerMenu.heavyion.HeavyIonSliceFlags  import HeavyIonSliceFlags

from TriggerMenu.menu.HltConfig import *

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo

#theTrigEFIDInsideOut_FullScan = TrigEFIDSequence("FullScan","fullScan")

from TrigT2MinBias.TrigT2MinBiasConfig import *
from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
#fexes.efid = TrigEFIDSequence("minBias","minBias","InsideOut").getSequence()
#fexes.efid2P = TrigEFIDSequence("minBias2P","minBias2","InsideOutLowPt").getSequence()

efiddataprep = TrigEFIDSequence("minBias","minBias","DataPrep").getSequence()
efid = TrigEFIDSequence("minBias","minBias","InsideOut").getSequence()
efid_heavyIon = TrigEFIDSequence("heavyIonFS","heavyIonFS","InsideOut").getSequence()
efid2P = TrigEFIDSequence("minBias2P","minBias2","InsideOutLowPt").getSequence()

from TrigMinBias.TrigMinBiasConfig import *


from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyRoI
from TrigGenericAlgs.TrigGenericAlgsConf import PrescaleAlgo
dummyRoI=DummyRoI(name='MinBiasDummyRoI', createRoIDescriptors = True, NumberOfOutputTEs=1)
terminateAlgo = PrescaleAlgo('terminateAlgo')


# for HI
from TrigHIHypo.TrigHIHypoConfig import HIEFTrackHypo_AtLeastOneTrack
atLeastOneTrack = HIEFTrackHypo_AtLeastOneTrack(name='HIEFTrackHypo_AtLeastOneTrack')

from TrigHIHypo.TrigHIHypoConfig import *
#hypos.update(hi_hypos)

#L2 pileup suppression
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_FullScan_ZF_OnlyA  #TrigL2SiTrackFinder_FullScanA_ZF_OnlyA

theL2PileupSup = TrigL2SiTrackFinder_FullScan_ZF_OnlyA()

###########################################################################
#  All min bias
###########################################################################
class L2EFChain_HI(L2EFChainDef):
    
    def __init__(self, chainDict):

        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []
        
        self.chainPart = chainDict['chainParts']
        
        self.chainL1Item = chainDict['L1item']        
        self.chainPartL1Item = self.chainPart['L1item']
        
        self.chainCounter = chainDict['chainCounter']       
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        #self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']
        #self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        
        self.L2InputTE = self.chainPartL1Item or self.chainL1Item
        # cut of L1_, _EMPTY,..., & multiplicity
        if self.L2InputTE is not '':
            self.L2InputTE = self.L2InputTE.replace("L1_","")
            self.L2InputTE = self.L2InputTE.split("_")[0]
            self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE

        if "v2" in self.chainPart['eventShape']:
            self.setup_hi_eventshape()
        elif "v3" in self.chainPart['eventShape']:
            self.setup_hi_eventshape()
        elif "ucc" in self.chainPart['recoAlg']:
            self.setup_hi_ultracentral()
        elif "upc" in self.chainPart['recoAlg']:
            self.setup_hi_ultraperipheral()
        
        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter, self.chainL1Item, self.EFName, self.chainCounter, self.L2InputTE)

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
        self.TErenamingMap = self.TErenamingDict

############################### HELPER FUNCTIONS ##############################
    def setup_hi_eventshape(self):

        EShypo_temp = self.chainPart['extra']
        ESth=EShypo_temp.lstrip('th')
        #print 'igb: ES threshold:', ESth
        VetoHypo = None
        ESHypo=None
        ESDiHypo=None
        if 'v2' == self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V2_th
            chainSuffix = 'v2_th'+ESth
            assert V2_th.has_key(int(ESth)), "Missing V2 configuration for threshold "+ESth
            ESHypo=V2_th[int(ESth)] 
            if self.chainPart['eventShapeVeto'] == 'veto3':
                from TrigHIHypo.VnHypos import V3_th1_veto
                VetoHypo = V3_th1_veto
                
        elif 'v3' == self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V3_th
            chainSuffix = 'v3_th'+ESth
            assert V3_th.has_key(int(ESth)), "Missing V3 configuration for threshold "+ESth         
            ESHypo=V3_th[int(ESth)] 
            if self.chainPart['eventShapeVeto']  == 'veto2':
                from TrigHIHypo.VnHypos import V2_th1_veto
                VetoHypo = V2_th1_veto

        elif 'v23' == self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V3_th,V2_th
            chainSuffix = 'th'+ESth
            th=int(ESth)
            ESDiHypo = [V2_th[th], V3_th[th]]

        print "WTF", ESHypo, VetoHypo, ESDiHypo, " when making chain ", self.chainPart

        from TrigHIHypo.UE import theUEMaker, theFSCellMaker

        ########### Sequence List ##############
        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_hi_step1']] 
        self.EFsequenceList += [[['L2_hi_step1'], 
                                     [theFSCellMaker], 'EF_hi_step1_fs']]

        self.EFsequenceList += [[['EF_hi_step1_fs'], 
                                 [theUEMaker], 'EF_hi_step1_ue']]

        self.L2signatureList += [ [['L2_hi_step1']] ]
        self.EFsignatureList += [ [['EF_hi_step1_fs']] ]
        self.EFsignatureList += [ [['EF_hi_step1_ue']] ]

        if ESHypo:
            self.EFsequenceList += [[['EF_hi_step1_ue'], 
                                     [ESHypo], 'EF_hi_step2']]
            self.EFsignatureList += [ [['EF_hi_step2']] ]

        if VetoHypo:
            self.EFsequenceList += [[['EF_hi_step2'], 
                                     [VetoHypo], 'EF_hi_step_veto']]
            self.EFsignatureList += [ [['EF_hi_step_veto']] ]

        if ESDiHypo:
            self.EFsequenceList += [[['EF_hi_step1_ue'], 
                                     [ESDiHypo[0]], 'EF_hi_step2']]
            self.EFsequenceList += [[['EF_hi_step2'], 
                                     [ESDiHypo[1]], 'EF_hi_step3']]

            self.EFsignatureList += [ [['EF_hi_step2']] ]
            self.EFsignatureList += [ [['EF_hi_step3']] ]

    
        self.TErenamingDict = {
            'L2_hi_step1': mergeRemovingOverlap('L2_hi_step1_', chainSuffix),
            'EF_hi_step1_fs': mergeRemovingOverlap('EF_hi_fs_', chainSuffix),
            'EF_hi_step1_ue': mergeRemovingOverlap('EF_hi_ue_', chainSuffix),
            'EF_hi_step2': mergeRemovingOverlap('EF_hi_', chainSuffix),
            'EF_hi_step3': mergeRemovingOverlap('EF_hi_', '_and_v3_'+chainSuffix),
            'EF_hi_step_veto': mergeRemovingOverlap('EF_hi_veto_', chainSuffix),
            }

###########################
    def setup_hi_ultracentral(self):

        if 'perfzdc' in self.chainPart['extra']:
            from TrigHIHypo.UltraCentralHypos import UltraCentral_PT
            from TrigT2MinBias.TrigT2MinBiasConfig import L2MbZdcFex_LG, L2MbZdcFex_HG, L2MbZdcHypo_PT
            theL2Fex1  = L2MbZdcFex_LG
            theL2Fex2  = L2MbZdcFex_HG
            theL2Hypo1 = L2MbZdcHypo_PT
            chainSuffix = 'perfzdc_ucc'
            UChypo=UltraCentral_PT("UltraCentralHypo_PT")
        elif 'perf'  in self.chainPart['extra']:
            from TrigHIHypo.UltraCentralHypos import UltraCentral_PT
            chainSuffix = 'perf_ucc'
            UChypo=UltraCentral_PT("UltraCentralHypo_PT")
        else:
            from TrigHIHypo.UltraCentralHypos import UCC_th
            threshold = self.chainPart['extra']
            UChypo=UCC_th[threshold]
            chainSuffix = threshold
        from TrigHIHypo.UE import theUEMaker, theFSCellMaker

        ########### Sequence List ##############
        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_hi_step1']] 
        self.EFsequenceList += [[['L2_hi_step1'], 
                                     [theFSCellMaker], 'EF_hi_step1_fs']]

        self.EFsequenceList += [[['EF_hi_step1_fs'], 
                                 [theUEMaker], 'EF_hi_step1_ue']]

        self.EFsequenceList += [[['EF_hi_step1_ue'], 
                                 [UChypo], 'EF_hi_step2']]
        if 'perfzdc' in self.chainPart['extra']:
            self.EFsequenceList += [[['EF_hi_step2'],
                                     [theL2Fex1], 'EF_hi_step3']]
            self.EFsequenceList += [[['EF_hi_step3'],
                                     [theL2Fex2, theL2Hypo1], 'EF_hi_step4']]


        ########### Signatures ###########
        self.L2signatureList += [ [['L2_hi_step1']] ]
        self.EFsignatureList += [ [['EF_hi_step1_fs']] ]
        self.EFsignatureList += [ [['EF_hi_step1_ue']] ]
        self.EFsignatureList += [ [['EF_hi_step2']] ]
        if 'perfzdc' in self.chainPart['extra']:
            self.EFsignatureList += [ [['EF_hi_step3']] ]
            self.EFsignatureList += [ [['EF_hi_step4']] ]
    
        self.TErenamingDict = {
            'L2_hi_step1': mergeRemovingOverlap('L2_hi_step1_', chainSuffix),
            'EF_hi_step1_fs': mergeRemovingOverlap('EF_hi_fs_', chainSuffix),
            'EF_hi_step1_ue': mergeRemovingOverlap('EF_hi_ue_', chainSuffix),
            'EF_hi_step2': mergeRemovingOverlap('EF_hi_', chainSuffix),
            }

        if 'perfzdc' in self.chainPart['extra']:
            self.TErenamingDict = {
                'L2_hi_step1': mergeRemovingOverlap('L2_hi_step1_', chainSuffix),
                'EF_hi_step1_fs': mergeRemovingOverlap('EF_hi_fs_', chainSuffix),
                'EF_hi_step1_ue': mergeRemovingOverlap('EF_hi_ue_', chainSuffix),
                'EF_hi_step2': mergeRemovingOverlap('EF_hi_', chainSuffix),
                'EF_hi_step3': mergeRemovingOverlap('EF_hi_hg_', chainSuffix),
                'EF_hi_step4': mergeRemovingOverlap('EF_hi_perfzdc_', chainSuffix),
                }

###########################
    def setup_hi_ultraperipheral(self):

        theL2MbtsFex=L2MbMbtsFex
        theL2MbtsHypo=MbMbtsHypo("L2MbMbtsHypo_1_1_inn_veto")
        theL2PixelFex  = L2MbSpFex

        if 'loose' in self.chainPart['hypoL2Info']:
            minPixel=6
            maxPixel=40
            chainSuffix = 'loose_upc'
        if 'medium' in self.chainPart['hypoL2Info']:
            minPixel=6
            maxPixel=30
            chainSuffix = 'medium_upc'
        if 'tight' in self.chainPart['hypoL2Info']:
            minPixel=6
            maxPixel=25
            chainSuffix = 'tight_upc'
        if 'gg' in self.chainPart['hypoL2Info']:
            minPixel=0
            maxPixel=25
            chainSuffix = 'gg_upc'

        theL2PixelHypo  = L2MbSpUPC("MbPixelSpUPC_min"+str(minPixel)+'_max'+str(maxPixel), minPixel, maxPixel)
        
        ########### Sequence List ##############
        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_hi_step1']] 
        self.L2sequenceList += [[['L2_hi_step1'], 
                                     [theL2MbtsFex, theL2MbtsHypo], 'L2_hi_mbtsveto']]
        self.L2sequenceList += [['L2_hi_mbtsveto',
                                     efiddataprep,
                                 'L2_hi_iddataprep']]
        self.L2sequenceList += [[['L2_hi_iddataprep'],
                                 [theL2PixelFex, theL2PixelHypo],
                                 'L2_hi_pixel']]



        ########### Signatures ###########
        self.L2signatureList += [ [['L2_hi_step1']] ]
        self.L2signatureList += [ [['L2_hi_mbtsveto']] ]
        self.L2signatureList += [ [['L2_hi_iddataprep']] ]
        self.L2signatureList += [ [['L2_hi_pixel']] ]
    
        self.TErenamingDict = {
            'L2_hi_step1': mergeRemovingOverlap('L2_hi_step1_', chainSuffix),
            'L2_hi_mbtsveto': mergeRemovingOverlap('EF_hi_mbtsveto_', chainSuffix),
            'L2_hi_iddataprep': mergeRemovingOverlap('EF_hi_iddataprep_', chainSuffix),
            'L2_hi_pixel': mergeRemovingOverlap('EF_hi_pixel_', chainSuffix),
            }

#####################################################################
    
#if __name__ == '__main__':
#    triggerPythonConfig = TriggerPythonConfig('hlt.xml', None)
#    for m in Muons:
#        m.generateMenu(triggerPythonConfig)
#    triggerPythonConfig.writeConfigFiles()
    
