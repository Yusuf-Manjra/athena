# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__author__  = 'M.Backes, C.Bernius'
__doc__="Definition of bjet chains" 

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.bjet.generateBjetChainDefs.py")

from TriggerMenu.menu.MenuUtils import *
from TriggerMenu.menu.DictFromChainName import DictFromChainName 
theDictFromChainName = DictFromChainName()

import pprint
pp = pprint.PrettyPrinter(indent=4, depth=8)

###########################################################################
###########################################################################
def generateChainDefs(chainDict):

    from copy import deepcopy
    chainDict_orig = deepcopy(chainDict)
    log.debug("Initial b-jet chainDict: \n %s" % (pp.pformat(chainDict)))

    #----------------------------------------------------------------------------
    # --- preparing the dictionary for jet chain generation ---
    n_chainParts = len(chainDict['chainParts'])
   
    if (n_chainParts >1):
        jetchainDict = _prepareJetChainDict(chainDict)
    elif (n_chainParts == 1):
        log.debug('Only one chain part dictionary for this chain.')
        jetchainDict = chainDict
    else:
        raise RuntimeError('No chain parts found for chain %s' % (chainDict['chainName']))
    #----------------------------------------------------------------------------

    #----------------------------------------------------------------------------
    # --- obtain jet chain def used as inputTE for first btag sequence ---
    thejThrChainDef = get_j35_ChainDef()
    if ('j0_bperf' in chainDict['chainName']):
        thejThrChainDef['L1item'] = 'L1_J10'
        thejThrChainDef['chainName'] = 'j0'
        for part in thejThrChainDef['chainParts']:
            part['chainPartName'] = 'j0'
            part['threshold'] = '0'
        if ('L1MU10' in chainDict['chainName']):
            thejThrChainDef['L1item'] = 'L1_MU10'
        if ('L1RD0_EMPTY' in chainDict['chainName']):
            thejThrChainDef['L1item'] = 'L1_RD0_EMPTY'

    from TriggerMenu.jet.generateJetChainDefs import generateChainDefs as genJetChainDefs
    theBJChainDef =  genJetChainDefs(thejThrChainDef)
    log.debug("ChainDef for b-jet chain: \n %s" % (str(theBJChainDef)))
    #----------------------------------------------------------------------------


    #----------------------------------------------------------------------------
    # --- build the jet chain, then pass JetChainDef and bjetchainDictionaries to build bjet chains ---
    theAllJetChainDef =  genJetChainDefs(jetchainDict)
    log.debug("Jet ChainDef for b-jet chain: \n %s" % (str(theAllJetChainDef)))
    #----------------------------------------------------------------------------


    #----------------------------------------------------------------------------
    # --- now merge chain defs so that last TE is from jet serving as input to bjet seq ---
    # DO NOT CHANGE THE MERGING ORDER PLEASE, OTHERWISE WRONG INPUTTE IS SELECTED
    log.debug("Trying to merge chainDef for b-jet chain: \n %s " % (str(theBJChainDef) ))
    log.debug("AllJetChainDef: \n %s" % str(theAllJetChainDef))
    theJetChainDef = mergeChainDefs([theAllJetChainDef, theBJChainDef], 'serial', -1, False)
    log.debug("Merged chainDef for b-jet chain: \n %s" % (str(theJetChainDef)))
    #----------------------------------------------------------------------------

    
    #----------------------------------------------------------------------------
    # --- filtering b-tagged jets chainParts ---
    listofChainDicts = splitChainDict(chainDict_orig)
    log.debug("Split b-jet chainDict: \n %s" % (pp.pformat(listofChainDicts)))

    bjetchainDicts = [cdict for cdict in listofChainDicts if cdict['chainParts']['bTag']] 
    log.debug("Final b-jet chainDict: \n %s" % (pp.pformat(bjetchainDicts)))

    theListOfChainDefs = []
    for subChainDict in bjetchainDicts:
        theJetChainDef1 = deepcopy(theJetChainDef)
        theBjetChainDef = buildBjetChains(theJetChainDef1, subChainDict, True, len(bjetchainDicts))    
        theListOfChainDefs += [theBjetChainDef] 


    log.debug("----------------- Beginning of final individual chainDefs for b-jet chains printout -----------------")
    for chainDef in theListOfChainDefs:
        log.debug(str(chainDef))        

    log.debug("----------------- End of final individual chainDefs for b-jet chains printout -----------------")
        

    if len(theListOfChainDefs)>1:
        theFinalChainDef = mergeChainDefs(theListOfChainDefs,strategy="parallel",offset=-1,preserveL2EFOrder=True,removeDuplicateTEs=True)
    else:
        theFinalChainDef = theListOfChainDefs[0]

    log.debug("----------------- Beginning of final merged chainDefs for b-jet chains printout -----------------")
    log.debug(str(theFinalChainDef))


    log.debug("----------------- End of final merged chainDefs for b-jet chains printout -----------------")
        

    return theFinalChainDef


###########################################################################
###########################################################################
def buildBjetChains(jchaindef,bjetdict,doAtL2AndEF=True,numberOfSubChainDicts=1):
    inputTEsEF = jchaindef.signatureList[-1]['listOfTriggerElements'][0]

    L2ChainName = "L2_" + bjetdict['chainName']
    EFChainName = "EF_bjet_" + bjetdict['chainName']
    HLTChainName = "HLT_" + bjetdict['chainName']   
    topoAlgs = bjetdict["topo"]

    bjetparts = bjetdict['chainParts']

    if ('split' in bjetparts['bConfig']):
        theBjetChainDef = myBjetConfig_split(jchaindef, bjetdict, inputTEsEF,numberOfSubChainDicts) 
        theBjetChainDef.chain_name = 'HLT_'+bjetdict['chainName']
    else:
        theBjetChainDef = myBjetConfig1(jchaindef, bjetdict, inputTEsEF,numberOfSubChainDicts) 
        theBjetChainDef.chain_name = 'HLT_'+bjetdict['chainName']

    return theBjetChainDef


###################################################################################
###################################################################################

def myBjetConfig_split(theChainDef, chainDict, inputTEsEF,numberOfSubChainDicts=1):
    
    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_bjet_" + chainDict['chainName']
    HLTChainName = "HLT_" + chainDict['chainName']   

    chainParts = chainDict['chainParts']
    btagthresh = chainParts['threshold']
    btagmult = chainParts['multiplicity']
    btagcut = chainParts['bTag']
    btagcut = btagcut[1:]
    btracking = chainParts['bTracking']
    bmatching = chainParts['bMatching']

    print "JK chainParts[bTracking] is ",chainParts['bTracking']
    ftk=""
    if 'FTKVtx' in chainParts['bTracking'] or 'FTK' in chainParts['bTracking']  or 'FTKrefit' in chainParts['bTracking']:
        ftk="FTKVtx"

    print "JK ftk is ",ftk

    #-----------------------------------------------------------------------------------
    # Import of algs
    #-----------------------------------------------------------------------------------

    algoInstance = "EF"

    #--------------------
    
    # super ROI building
    from TrigBjetHypo.TrigSuperRoiBuilderAllTEConfig import getSuperRoiBuilderAllTEInstance
    theSuperRoi=getSuperRoiBuilderAllTEInstance()

    #--------------------

    # jet splitting
    from TrigBjetHypo.TrigJetSplitterAllTEConfig import getJetSplitterAllTEInstance
    theJetSplit=getJetSplitterAllTEInstance()

    #-------------------- 
    
    #find jets far away from a muon (for mu-jet chains with b-jet requirements only)
    if any('antimatchdr' in bM for bM in chainParts['bMatching'] ):

        # Extract the dR value from the chain name here.. the deltaR value has to consist of 2 numbers
       deltaR = -1
       for anti_match_part in chainParts['bMatching']:
           if 'dr' in anti_match_part:
               deltaR=anti_match_part.split('dr')[1][0:2]
       if deltaR == -1: logCombined.error("No DeltaR cut could be extracted!")
        
       from TrigBjetHypo.TrigFarawayJetFinderAllTEConfig import getFarawayJetFinderAllTEInstance
       theFarawayJet=getFarawayJetFinderAllTEInstance(str(deltaR))

       algoInstance = "MuJetChain"

    #--------------------
    
    # Et hypo (for b-tagging)
    from TrigBjetHypo.TrigBjetEtHypoConfig import getBjetEtHypoInstance
    theBjetEtHypo   = getBjetEtHypoInstance(algoInstance, "Btagging", btagthresh+"GeV" )

    #--------------------

    # tracking
#    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence # new
#    [trkvtx, trkftf, trkprec] = TrigInDetSequence("Bjet", "bjet", "IDTrig", "2step").getSequence() # new

    if 'FTKVtx' in chainParts['bTracking']:
        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence # new
        [trkvtx, trkftf, trkprec] = TrigInDetSequence("Bjet", "bjet", "IDTrig", "2step").getSequence() # new
    elif 'FTKrefit' in chainParts['bTracking']:
        from TrigInDetConf.TrigInDetFTKSequence import TrigInDetFTKSequence # new
        [trkvtx, trkftf] = TrigInDetFTKSequence("Bjet", "bjet", sequenceFlavour=["refit","2step"]).getSequence() # new
        trkprec=[""]
    elif 'FTK' in chainParts['bTracking']:
        from TrigInDetConf.TrigInDetFTKSequence import TrigInDetFTKSequence # new
        [trkvtx, trkftf] = TrigInDetFTKSequence("Bjet", "bjet", sequenceFlavour=["2step"]).getSequence() # new
        trkprec=[""]
    else:
        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence # new
        [trkvtx, trkftf, trkprec] = TrigInDetSequence("Bjet", "bjet", "IDTrig", "2step").getSequence() # new


    print "JK trkvtx: ", trkvtx 
    print "JK trkftf: ", trkftf 
    print "JK trkprec: ", trkprec 

    # for b-tagging
    theBjetTracks = trkftf+trkprec
    # for vertexing
    theVertexTracks = trkvtx  

    #--------------------

    # primary vertexing
    from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxAllTEConfig import EFHistoPrmVtxAllTE_Jet
    from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxComboConfig import EFHistoPrmVtxCombo_Jet
    # JK FTK vertex
    from TrigFTK_RecAlgs.TrigFTK_RecAlgs_Config import TrigFTK_VxPrimary_EF
    from InDetTrigRecExample.InDetTrigConfigRecPostProcessing import *

    #--------------------

    # secondary vertexing
    from InDetTrigVxSecondary.InDetTrigVxSecondary_LoadTools import TrigVxSecondaryCombo_EF
    theVxSecondary = TrigVxSecondaryCombo_EF()

    #--------------------

    # bjet fex
    if ('boffperf' in chainParts['bTag'] or 'bmv2c20' in chainParts['bTag']):
        # Offline taggers
        from TrigBjetHypo.TrigBtagFexConfig import getBtagFexSplitInstance 
        theBjetFex = getBtagFexSplitInstance(algoInstance,"2012","EFID") 
    else:
        # Run 1 style taggers
        from TrigBjetHypo.TrigBjetFexConfig  import getBjetFexSplitInstance
        theBjetFex = getBjetFexSplitInstance(algoInstance,"2012","EFID")

    #--------------------

    # bjet hypo 
    if ('bperf' in chainParts['bTag'] or 'boffperf' in chainParts['bTag']):
        # Performance chains (run 1 and run 2 style)
        # Runs in NoCut mode
        from TrigBjetHypo.TrigBjetHypoConfig import getBjetHypoSplitNoCutInstance
        theBtagReq = getBjetHypoSplitNoCutInstance(algoInstance)
    elif ('bmv2c20' in chainParts['bTag']):
        # MV2c20 tagger series
        from TrigBjetHypo.TrigBjetHypoConfig import getBjetHypoSplitInstance
        theBtagReq = getBjetHypoSplitInstance(algoInstance,"2015", btagcut)
    else:
        # Run 1 style chains
        from TrigBjetHypo.TrigBjetHypoConfig import getBjetHypoSplitInstance
        theBtagReq = getBjetHypoSplitInstance(algoInstance,"2012", btagcut)

    #-----------------------------------------------------------------------------------
    # TE naming
    #-----------------------------------------------------------------------------------
    
    tracking        = "IDTrig"
    if 'FTKVtx' in chainParts['bTracking']:
        tracking    = "IDtrig"
    elif 'FTKrefit' in chainParts['bTracking']:
        tracking    = "FTKrefit"
    elif 'FTK' in chainParts['bTracking']:
        tracking    = "FTK"

    print "JK , tracking is ",tracking

    jetEtHypoTE     = "HLT_j"+btagthresh+ftk
    jetHypoTE       = "HLT_j"+btagthresh+ftk+"_eta"
    jetSplitTE      = jetHypoTE+"_jsplit"
    jetFarawayTE    = jetSplitTE+"_faraway"
    jetTrackTE      = jetSplitTE+"_"+tracking

    print "JK , jetTrackTE ",jetTrackTE

    if any('antimatch' in bM for bM in chainParts['bMatching'] ) and  any('mu' in bM for bM in chainParts['bMatching'] ):

        # extract muon threshold from chainname
        allChainParts = chainDict['chainName'].split('_')
        muonthr=-1
        for cp in allChainParts:
            if 'mu' in cp and not 'antimatch' in cp:
                muonthr = cp.split('mu')[-1] # assume the last bit is the threshold
                break
        #print 'muon thr for antimatch:' + str(muonthr) 
        muonTE      = "EF_SuperEF_mu{0}_MU{1}".format(muonthr,muonthr)
        jetEtHypoTE = jetEtHypoTE+'_antimatchmu{0}'.format(muonthr)
        jetHypoTE   = jetHypoTE+'_antimatchmu{0}'.format(muonthr)
        jetTrackTE  = jetTrackTE +'_antimatchmu{0}'.format(muonthr)

    superTE         = "HLT_super"
    superTrackingTE = superTE+tracking
    prmVertexTE     = superTrackingTE+"_prmVtx"
    comboPrmVtxTE   = prmVertexTE+"Combo"
    secVtxTE        = jetTrackTE+"__"+"superVtx"
    lastTEout       = "HLT_bjet_"+chainParts['chainPartName'] if numberOfSubChainDicts>1 else EFChainName

    if 'FTKVtx' in chainParts['bTracking'] or 'FTK' in chainParts['bTracking']  or 'FTKrefit' in chainParts['bTracking']:
        comboPrmVtxTE="HLT_FTKVtx_Combo"
 

    print "JK comboPrmVtx : ",comboPrmVtxTE

    topoThresh = chainDict['topoThreshold']
    topoStartFrom = setupTopoStartFrom(topoThresh,theChainDef) if topoThresh else None

    if topoStartFrom:
        lastTEout = lastTEout+'_tsf'
    
    #-----------------------------------------------------------------------------------
    # sequence assembling
    #-----------------------------------------------------------------------------------

    # Vertexing part of the chain
    if 'FTKVtx' in chainParts['bTracking'] or 'FTK' in chainParts['bTracking'] or 'FTKrefit' in chainParts['bTracking'] :
        theChainDef.addSequence([TrigFTK_VxPrimary_EF("TrigFTK_VxPrimary_Bjet_IDTrig","bjetVtx"), InDetTrigVertexxAODCnv_EF("InDetTrigVertexxAODCnv_Bjet_FTK","bjetVtx")] , inputTEsEF, comboPrmVtxTE)
    else:
        theChainDef.addSequence(theSuperRoi,      inputTEsEF,   superTE)
        theChainDef.addSequence(theVertexTracks,  superTE,      superTrackingTE) 
        theChainDef.addSequence([EFHistoPrmVtxAllTE_Jet()], superTrackingTE, prmVertexTE)
        theChainDef.addSequence([EFHistoPrmVtxCombo_Jet()], [superTrackingTE,prmVertexTE], comboPrmVtxTE)

    # b-tagging part of the chain (requires PV)
    theChainDef.addSequence(theJetSplit,    [inputTEsEF, comboPrmVtxTE], jetSplitTE)

    #theChainDef.addSequence(theFarawayJet,  [muonTE, jetSplitTE], jetFarawayTE)
    #theChainDef.addSequence(theBjetEtHypo,  jetFarawayTE,  jetEtHypoTE)
    #theChainDef.addSequence(theBjetEtHypo,  jetSplitTE,  jetEtHypoTE)
    if any('antimatch' in bM for bM in chainParts['bMatching'] ) and  any('mu' in bM for bM in chainParts['bMatching'] ):
        theChainDef.addSequence(theFarawayJet,  [muonTE, jetSplitTE], jetFarawayTE)
        theChainDef.addSequence(theBjetEtHypo,  jetFarawayTE,  jetEtHypoTE)
    else :
        theChainDef.addSequence(theBjetEtHypo,  jetSplitTE,  jetEtHypoTE)

    theChainDef.addSequence(theBjetTracks,  jetEtHypoTE, jetTrackTE)   
    theChainDef.addSequence(theVxSecondary, [jetTrackTE, comboPrmVtxTE], secVtxTE)
    theChainDef.addSequence([theBjetFex, theBtagReq], secVtxTE, lastTEout, topo_start_from = topoStartFrom)

    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [lastTEout]*int(btagmult))

    return theChainDef

###################################################################################

def myBjetConfig1(theChainDef, chainDict, inputTEsEF,numberOfSubChainDicts=1):
    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_bjet_" + chainDict['chainName']
    HLTChainName = "HLT_" + chainDict['chainName']   

    chainParts = chainDict['chainParts']
    btagthresh = chainParts['threshold']
    btagmult = chainParts['multiplicity']
    btagcut = chainParts['bTag']
    btagcut = btagcut[1:]
    btracking = chainParts['bTracking']

    #import fexes/hypos
    from TrigBjetHypo.TrigEFBjetSequenceAllTEConfig import getEFBjetAllTEInstance
    ef_bjetSequence=getEFBjetAllTEInstance()

    from TrigBjetHypo.TrigBjetEtHypoConfig          import getBjetEtHypoInstance
    if   ( int(btagthresh) == 0) : ef_ethypo_startseq = getBjetEtHypoInstance("EF","StartSequence","0GeV")
    elif ( int(btagthresh) < 35) : ef_ethypo_startseq = getBjetEtHypoInstance("EF","StartSequence","15GeV")
    else                         : ef_ethypo_startseq = getBjetEtHypoInstance("EF","StartSequence","35GeV")

    # tracking
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence # new
    [trkvtx, trkftf, trkprec] = TrigInDetSequence("Bjet", "bjet", "IDTrig", "2step").getSequence() 
    ef_bjet_tracks = trkftf+trkprec

    from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxAllTEConfig import EFHistoPrmVtxAllTE_Jet
    from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxComboConfig import EFHistoPrmVtxCombo_Jet

    from InDetTrigVxSecondary.InDetTrigVxSecondary_LoadTools import TrigVxSecondary_EF
    ef_VxSecondary_EF = TrigVxSecondary_EF()

    from TrigBjetHypo.TrigBjetEtHypoConfig          import getBjetEtHypoInstance
    ef_EtHypo_Btagging = getBjetEtHypoInstance("EF","Btagging", btagthresh+"GeV")

    # B-tagging
    if ('boffperf' in chainParts['bTag'] or 'bmv2c20' in chainParts['bTag']):
        from TrigBjetHypo.TrigBtagFexConfig import getBtagFexInstance
        ef_bjet = getBtagFexInstance("EF","2012","EFID")
    else:
        from TrigBjetHypo.TrigBjetFexConfig  import getBjetFexInstance
        ef_bjet = getBjetFexInstance("EF","2012","EFID")

    # Hypo testing
    if ('bperf' in chainParts['bTag'] or 'boffperf' in chainParts['bTag']):
        from TrigBjetHypo.TrigBjetHypoConfig import getBjetHypoNoCutInstance
        ef_hypo = getBjetHypoNoCutInstance("EF")
    elif ('bmv2c20' in chainParts['bTag']):
        from TrigBjetHypo.TrigBjetHypoConfig import getBjetHypoInstance
        ef_hypo = getBjetHypoInstance("EF","2015", btagcut)
    else:
        from TrigBjetHypo.TrigBjetHypoConfig import getBjetHypoInstance
        ef_hypo = getBjetHypoInstance("EF","2012", btagcut)

    #------- 2012 EF Sequences based on j35 intput TE-------
    # TE naming
    ef2 ='HLT_Bj'
    ef3 ='HLT_Bj_EtCut%s' % btagthresh
    if ('EFID' in chainParts['bTracking']):
        ef4 ='HLT_Bj_EtCut%s_EFID'  % btagthresh
        ef5 ='HLT_Bj_EtCut%s_AllTEPrmVtx_EFID'  % btagthresh
        ef6 ='HLT_Bj_EtCut%s_ComboPrmVtx_EFID'  % btagthresh
    else:
        ef4 ='HLT_Bj_EtCut%s_IDTrig'  % btagthresh
        ef5 ='HLT_Bj_EtCut%s_AllTEPrmVtx_IDTrig'  % btagthresh
        ef6 ='HLT_Bj_EtCut%s_ComboPrmVtx_IDTrig'  % btagthresh
    if (btagmult == '1'):
        ef7 = 'EF_b%s_%s_%s_SecVxBhypo' % (btagthresh, btagcut, chainParts['chainPartName'].replace("_"+chainParts['bTracking'],""), )
    else:
        ef7 = 'EF_%sb%s_%s_%s_SecVxBhypo' % (btagmult, btagthresh, btagcut, chainParts['chainPartName'].replace("_"+chainParts['bTracking'],""))

    lastTEout = "EF_bj_"+chainParts['chainPartName'] if numberOfSubChainDicts>1 else EFChainName


    topoThresh = chainDict['topoThreshold']
    topoStartFrom = setupTopoStartFrom(topoThresh,theChainDef) if topoThresh else None
    if topoStartFrom:
        lastTEout = lastTEout+'_tsf'



    theChainDef.addSequence([ef_bjetSequence], inputTEsEF, ef2)
    theChainDef.addSequence(ef_ethypo_startseq, ef2, ef3)
    theChainDef.addSequence(ef_bjet_tracks, ef3 ,ef4)
    theChainDef.addSequence([EFHistoPrmVtxAllTE_Jet()], ef4, ef5)
    theChainDef.addSequence([EFHistoPrmVtxCombo_Jet()], [ef4, ef5], ef6)
    #theChainDef.addSequence([ef_EtHypo_Btagging], ef6, ef7) 
    theChainDef.addSequence([ef_VxSecondary_EF,ef_EtHypo_Btagging], ef6, ef7) 
    theChainDef.addSequence([ef_bjet, ef_hypo], ef7, lastTEout, topo_start_from = topoStartFrom)
    theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [lastTEout]*int(btagmult))
    

    return theChainDef




###########################################################################
###########################################################################
# !!!!!!PLEASE DO NOT CHANGE THIS UNLESS NECESSARY!!!!
# This funciton checks the chainDict of the chain and merges chain parts 
# if only different in multiplicity and b-tag
# to then send off to the jet slice
##########################################################################
def _prepareJetChainDict(cdict):
    
    # -- collecting thresholds in a list to check if there are duplicates -- 
    thresholds = [part['threshold'] for part in cdict['chainParts']]
    from collections import Counter
    counts = Counter(thresholds)
    duplicates = [val for val, count in counts.items() if count > 1]

    if duplicates:
        log.info('Duplicated thresholds in the jet and b-jet part')
        
        # -- splitting chainparts dictioanries according to bjet and jet -- 
        bjetchainParts =[] 
        jetchainParts  = []
        for part in cdict['chainParts']:
            if not part['bTag']:
                jetchainParts.append(part)
            elif part['bTag']:
                bjetchainParts.append(part)
            else: 
                raise RuntimeError('I do not know what to do with this chain part %s' %(part['chainPartName']))
        
        # -- Loop over jet and bjet chain parts
        # -- pick out the ones with common thresholds
        # -- compare the jet building parts and if nothing differnet, adapt the multiplicity and chainPartName
        mergedbjetpart = []
        for jpart in jetchainParts:
            for bjindex, bjpart in enumerate(bjetchainParts):
                if (int(jpart['threshold']) == int(bjpart['threshold'])):                    
                    # -- check that all jet properties are the same as bjet 
                    # -- except for multiplicity, chainPartName and bTag
                    # -- this is bad hardcoding better clean up ...not
                    allowedDiff1 = set(['multiplicity', 'chainPartName', 'bTag','bTracking'])
                    allowedDiff2 = set(['multiplicity', 'chainPartName', 'bTag'])
                    allowedDiff3 = set(['multiplicity', 'chainPartName', 'bTag','bConfig'])
                    allowedDiff1_noMult = set(['chainPartName', 'bTag','bTracking'])
                    allowedDiff2_noMult = set(['chainPartName', 'bTag'])
                    allowedDiff3_noMult = set(['chainPartName', 'bTag','bConfig'])
                    
                    s1 = set(jpart)
                    s2 = set(bjpart)
                    diff = set(k for k in s1 & s2 if jpart[k] != bjpart[k])

                    if ((diff == allowedDiff1) or (diff == allowedDiff1_noMult) or (diff == allowedDiff2) or (diff == allowedDiff2_noMult) or (diff == allowedDiff3) or (diff == allowedDiff3_noMult)):
                        jpart['multiplicity'] = str(int(jpart['multiplicity'])+int(bjpart['multiplicity']))
                        jpart['chainPartName'] = jpart['multiplicity']+jpart['trigType']+jpart['threshold']
                        mergedbjetpart.append(bjindex)
                    else:
                        log.info("Jet and underlying jet chain from bjet are not the same despite the same thresholds. Ignore and keep separate dictionaries. The difference is %s " % str(diff))
                        continue



        # -- remove those bjet chainParts that have been merged with the jets
        for index in mergedbjetpart:
            bjetchainParts.pop(index)
        
        # -- modify the chainParts of the dictionary by creating a new one with 
        # -- the updated jetchainParts and remaining bjetchainParts
        cdict['chainParts'] = []
        for jpart in jetchainParts:
            cdict['chainParts'].append(jpart) 
        for bjpart in bjetchainParts:
            cdict['chainParts'].append(bjpart)


    else: 
        log.info('No duplicated thresholds in the jet/b-jet chain')
    
    log.debug("Prepared b-jet chainDict: \n %s" % (pp.pformat(cdict)))

    return cdict



###########################################################################

def get_j35_ChainDef():
    return theDictFromChainName.getChainDict( ['j35', 'L1_J20', [], ["Main"], ['RATE:SingleJet',  'BW:Jets'], -1],) 

# def get_j35_ChainDef():
#     # HACK TO GET j35 chains!!!
#     j35_chainDict = { 'EBstep': 1, 
#                       'L1item': 'L1_J20', 
#                       'chainCounter': 21, 
#                       'chainName': 'j35', 
#                       'chainParts': [ { 'L1item': '', 
#                                         'addInfo': [], 
#                                         'bTag': 'bmedium', 
#                                         'calib': 'em', 
#                                         'jetCalib': 'subjes', 
#                                         'chainPartName': 'j35', 
#                                         'dataType': 'tc', 
#                                         'etaRange': '0eta320', 
#                                         'extra': '', 
#                                         'multiplicity': '1', 
#                                         'recoAlg': 'a4', 
#                                         'scan': 'FS', 
#                                         'signature': 'Jet', 
#                                         'threshold': '35', 
#                                         'topo': [], 
#                                         'trigType': 'j'}, ], 
#                       'groups': ['RATE:MultiJet', 'BW:Jets'], 
#                       'signature': 'Jet', 
#                       'signatures': '', 
#                       'stream': ['Jet'], 
#                       'topo': []} 
# 
#    return j35_chainDict

def get_lastTE_j35(ChainDef):
    inputTEsEF = ChainDef.signatureList[-1]['listOfTriggerElements']
    return inputTEsEF
###########################################################################
def getMultThreshBtagInfo(cDict):
    allInfo = []
    chainPartInfo = []
    for cpart in cDict['chainParts']:
        if cpart['signature'] == 'Jet':
            chainPartInfo.append(cpart['multiplicity'])        
            chainPartInfo.append(cpart['threshold'])        
            chainPartInfo.append(cpart['bTag'])
            allInfo.append(chainPartInfo)

    return allInfo
###########################################################################
