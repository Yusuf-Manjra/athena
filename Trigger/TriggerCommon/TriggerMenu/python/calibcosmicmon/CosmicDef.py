# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
#
# SliceDef file for HLT signatures used exclusively in the cosmic slice
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger( 'TriggerMenu.calibcosmon.CosmicDef' )

from TriggerMenu.menu.HltConfig import *

from AthenaCommon.Include import include
from AthenaCommon.SystemOfUnits import GeV
from TriggerJobOpts.TriggerFlags  import TriggerFlags


###################################################################################
def getInputTEfromL1Item(item):
    L1Map = {'L1_CALREQ2':        ['NIM30'],
             #'L1_RD0_EMPTY':      [''],
             'L1_TAU8_EMPTY':      ['HA8'],
             }

    if item in L1Map:
        return L1Map[item]
    else: 
        TE = item.replace("L1_","")
        TE = TE.split("_")[0]
        TE = TE[1:] if TE[0].isdigit() else TE
        return TE



###################################################################################
class L2EFChain_CosmicTemplate(L2EFChainDef):

    def __init__(self, chainDict):
        mlog = logging.getLogger( 'CosmicDef.py:L2EFChain_CosmicTemplate' )

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
        self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']


        self.L2InputL1Item = self.chainPartL1Item or self.chainL1Item
        if self.L2InputL1Item:
            self.L2InputTE = getInputTEfromL1Item(self.L2InputL1Item)
        else:
            self.L2InputTE = ''

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % self.chainName

        mlog.verbose('in L2EFChain_CosmicTemplate constructor for %s' % self.chainName)

        # SPECIFICATIONS
        if ('tilecalib' in  self.chainPart['purpose']) \
                & ('laser' in self.chainPart['addInfo']):
            self.setupCosmicTileCalibration()

        elif 'larps' in self.chainPart['purpose']:
            self.setupCosmicLArPreSNoise()

        elif ('pixel' in self.chainPart['purpose']) \
                | ('sct' in self.chainPart['purpose']):
            self.setupCosmicIDNoiseCalibration()

        elif ('id' in  self.chainPart['purpose']):
            self.setupCosmicAllTeChains()

        else:
            log.error('Chain %s could not be assembled' % (self.chainPartName))
            return False      
        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter,
                            self.chainL1Item, self.EFName, self.chainCounter, self.L2InputTE)

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
      

    ##################################################################
    def setupCosmicTileCalibration(self):
        
        # Configuration
        from TrigDetCalib.TrigDetCalibConfig import TileSubDetListWriter
        self.l2_tileSubDetListWriter = TileSubDetListWriter("L2_Cosmic"+self.chainName+"TileSubDetListWriter")
        self.l2_tileSubDetListWriter.Subdetectors = "Tile"
        self.l2_tileSubDetListWriter.MaxRoIsPerEvent = 1
        self.l2_tileSubDetListWriter.addL1Calo = True
        
        from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
        self.EF_Dummy = DummyFEX("EF_%s" % self.chainName)
        
        # Sequences
        self.L2sequenceList += [[ self.L2InputTE, 
                                  [self.l2_tileSubDetListWriter],
                                  'L2_step1']]
        self.EFsequenceList += [[ ['L2_step1'], [self.EF_Dummy], 'EF_step1']]
        
        # Signatures
        self.L2signatureList += [ [['L2_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_step1']*self.mult] ]
        # TE renaming
        self.TErenamingDict = {
            'L2_step1': mergeRemovingOverlap('L2_', 'Cosmic_'+self.chainName+'_TileSubDetListWriter'),
            'EF_step1': mergeRemovingOverlap('EF_', self.chainName),
            }               	         


    ##################################################################
    def setupCosmicLArPreSNoise(self):
        #L2 sequence setup
        if 'L1_EM' in self.chainL1Item:            
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_AllEm
            theTrigT2CaloEgammaFex = T2CaloEgamma_AllEm()
        else:   # ('L1_J' in self.chainL1Item) | ('L1_TAU' in self.chainL1Item):
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_All
            theTrigT2CaloEgammaFex = T2CaloEgamma_All()
            
        from TrigEgammaHypo.TrigL2CaloHypoConfig import TrigL2CaloLayersHypo_PreS_080
        self.L2sequenceList += [[ self.L2InputTE, 
                                  [theTrigT2CaloEgammaFex,
                                   TrigL2CaloLayersHypo_PreS_080('TrigL2CaloLayersHypo_'+self.chainName+'_080')],
                                  'L2_step1']]

        self.L2signatureList+=[ [['L2_step1']*self.mult] ]
        self.TErenamingDict = {
            'L2_step1': mergeRemovingOverlap('L2_','TrigL2CaloLayersHypo_'+self.chainName+'_080'),
            }



            
    ##################################################################
    def setupCosmicIDNoiseCalibration(self):

        from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter

        if 'pixel' in self.chainPart['purpose']:

            if 'noise' in self.chainPart['addInfo']:
                l2_pixelSubDetListWriter = TrigSubDetListWriter("CosmicPixelNoiseSubDetListWriter")
                l2_pixelSubDetListWriter.Subdetectors = "Pixel"
                l2_pixelSubDetListWriter.extraROBs = []
                theRobWriter = [l2_pixelSubDetListWriter]

            elif 'beam' in self.chainPart['addInfo']:
                l2_pixelSCTSubDetListWriter = TrigSubDetListWriter("CosmicPixelSCTNoiseSubDetListWriter")
                l2_pixelSCTSubDetListWriter.Subdetectors = "Pixel,SCT"
                l2_pixelSCTSubDetListWriter.extraROBs = []            
                theRobWriter = [l2_pixelSCTSubDetListWriter]
                
        elif ('sct' in self.chainPart['purpose']) \
                & ('noise' in self.chainPart['addInfo']):
            l2_SCTSubDetListWriter = TrigSubDetListWriter("CosmicSCTNoiseSubDetListWriter")
            l2_SCTSubDetListWriter.Subdetectors = "SCT"
            l2_SCTSubDetListWriter.extraROBs = []
            theRobWriter= [l2_SCTSubDetListWriter]


        self.L2sequenceList += [[ '', theRobWriter,  'L2_step1']]
        self.L2signatureList+=[ [['L2_step1']*self.mult] ]

        self.TErenamingDict = {
            'L2_step1': mergeRemovingOverlap('L2_','Calib'+self.chainName),
            }

    ##################################################################
    def setupCosmicAllTeChains(self):
        # common L2 sequence
        from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo
        theAllTEDummyFakeROI = PESA__DummyUnseededAllTEAlgo("Cosmic"+self.chainName+"AllTEDummy")

        from TrigHypoCommonTools.TrigHypoCommonToolsConf import L1InfoHypo
        theL1InfoHypo = L1InfoHypo("L1InfoHypo"+self.chainName)
        theL1InfoHypo.TriggerTypeBitMask=0x80
        theL1InfoHypo.TriggerTypeBit=0x80
        theL1InfoHypo.InvertSelection=False
    
        theL1InfoHypoRNDMReject = L1InfoHypo("L1InfoHypoRNDMReject"+self.chainName) 
        theL1InfoHypoRNDMReject.InvertSelection=True
        theL1InfoHypoRNDMReject.TriggerTypeBit = 0x81
        theL1InfoHypoRNDMReject.TriggerTypeBitMask=0xff
        
        self.L2sequenceList += [['',
                                [theAllTEDummyFakeROI, theL1InfoHypo, theL1InfoHypoRNDMReject],
                                'L2_ih']]
        self.L2signatureList+=[ [['L2_ih']*self.mult] ]

 
            
        if any('trtxk' in x for x in self.chainPart['trackingAlg']):
            from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics_NewAlgo
            thetrtsegm  = TrigTRTSegFinder_Cosmics_NewAlgo("Cosmic"+self.chainName+"TrigTRTSegFinder")
            thetrtsegm.SegmentsMakerTool.IsMagneticFieldOn = True
            thetrtsegm.RoIhalfWidthDeltaPhi = 3.14
            thetrtsegm.RoIhalfWidthDeltaEta = 3.

            from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic
            theTrthypoCosmics = CosmicTrtHypo_Cosmic("Cosmic"+self.chainName+"TrtHypo") 
            theTrthypoCosmics.NTrthitsCut = 15
            thetrtsegm.SegmentsMakerTool.MinimalNumberOfTRTHits = 15
            thetrtsegm.pTmin = 100.0
            if ('central' in self.chainPart['addInfo']):
                theTrthypoCosmics.TrtSegD0Cut=250.0
                
   
            self.L2sequenceList += [[['L2_ih'], [thetrtsegm, theTrthypoCosmics], 'L2_seg']]
            self.L2signatureList += [ [['L2_seg']*self.mult] ]
 

            self.TErenamingDict = {
                'L2_ih':   mergeRemovingOverlap('L2_','Cosmic'+self.chainName+"AllTEDummy"),
                'L2_seg':  mergeRemovingOverlap('L2_', 'Cosmic'+self.chainName+"TrigTRTSegFinder"),
                }


        else:    # former CosmicsAllTeEFID chain        
            from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_CosmicsN
            theEFIDTracking=TrigEFIDInsideOut_CosmicsN()
            from TrigMinBias.TrigMinBiasConfig import MbTrkFex_1, MbTrkHypo_1
            thetrackcnt =  MbTrkFex_1("MbTrkFex_"+self.chainName)
            thetrackcnt.InputTrackContainerName = "InDetTrigTrackSlimmerIOTRT_CosmicsN_EFID"
            theefidcosmhypo = MbTrkHypo_1("MbTrkHypo_"+self.chainName)
            theefidcosmhypo.AcceptAll_EF=False
            theefidcosmhypo.Required_ntrks=1
            theefidcosmhypo.Max_z0=1000.0
        
            self.EFsequenceList += [[['L2_ih'],theEFIDTracking.getSequence()+[thetrackcnt,  theefidcosmhypo], 'EF_efid']]
            self.EFsignatureList +=  [ [['EF_efid']*self.mult] ]

            self.TErenamingDict = {
                'L2_ih':    "L2_Cosmic"+self.chainName+"AllTEDummy",
                'EF_efid':  "EF_TrigEFIDInsideOut_CosmicsN",
                }



        # if any('idscan' in x for x in self.chainPart['trackingAlg']) | any('sitrack' in x for x in self.chainPart['trackingAlg']):
        #     theTEsuffix = ''
        #     if any('idscan' in x for x in self.chainPart['trackingAlg']):
        #         from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Cosmics            
        #         theIdScanCosmics = TrigIDSCAN_Cosmics("CosmicIDSCANSequence"+self.chainName)
        #         theIdScanCosmics.doTRTpropagation = True
        #         if '4hits' in self.chainPart['hits']:
        #             theIdScanCosmics.MinHits = 4
        #             theIdScanCosmics.TrigHitFilter.LayerThreshold = 3.5
        #         else:
        #             theIdScanCosmics.MinHits = 3
        #             theIdScanCosmics.TrigHitFilter.LayerThreshold = 2.5

        #         theTEsuffix = 'TrigIDSCAN'
        #         theTrackingCosmics = theIdScanCosmics

        #     else: #if any('SiTrack' in x for x in self.chainPart['purpose']):
        #         from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Cosmics    
        #         theSiTrackCosmics = TrigSiTrack_Cosmics("Cosmic"+self.chainName+"TrigSiTrack") # here we created an instance
        #         if '4hits' in self.chainPart['hits']:            
        #             theSiTrackCosmics.Extension_SpacePoints = 4
        #         else:
        #             theSiTrackCosmics.Extension_SpacePoints = 3

        #         theTEsuffix = 'TrigSiTrack'
        #         theTrackingCosmics = theSiTrackCosmics
                    
                           
        #     from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic                  
        #     theTrkhypoCosmics = CosmicTrtHypo_Cosmic("Cosmic"+self.chainName+"TrkHypo") 
        #     theTrkhypoCosmics.NTrthitsCut = -1
        #     self.L2sequenceList += [[['L2_ih'], [theTrackingCosmics, theTrkhypoCosmics],  'L2_track']]
        #     self.L2signatureList+=[ [['L2_track']*self.mult] ]

    
        #     self.TErenamingDict = {
        #         'L2_ih'   : mergeRemovingOverlap('L2_','Cosmic'+self.chainName+"AllTEDummy"),
        #         'L2_track': mergeRemovingOverlap('L2_','Cosmic_'+self.chainName+'_'+theTEsuffix),                
        #         }

 
