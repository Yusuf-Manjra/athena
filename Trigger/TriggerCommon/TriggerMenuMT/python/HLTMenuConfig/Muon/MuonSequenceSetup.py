# 
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
# 

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND, seqOR
from AthenaConfiguration.AllConfigFlags import ConfigFlags


#-----------------------------------------------------#
### Used the algorithms as Step1 "muFast step" ###
### Load data from Muon detectors ###
#-----------------------------------------------------#
#import MuonRecExample.MuonRecStandaloneOnlySetup
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.doTrackPerformance    = True
muonRecFlags.TrackPerfSummaryLevel = 2
muonRecFlags.TrackPerfDebugLevel   = 5
muonCombinedRecFlags.doCaloTrkMuId = False
muonCombinedRecFlags.printSummary = False
muonCombinedRecFlags.doSiAssocForwardMuons = False
muonCombinedRecFlags.doStatisticalCombination = False
muonCombinedRecFlags.doCombinedFit = True
muonRecFlags.enableErrorTuning = False

from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm, \
  ViewCreatorInitialROITool, ViewCreatorPreviousROITool, ViewCreatorNamedROITool, \
  ViewCreatorFSROITool, ViewCreatorCentredOnIParticleROITool

#muon container names (for RoI based sequences)
from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muonNames
muNames = muonNames().getNames('RoI')
#-----------------------------------------------------#
### ************* Step1  ************* ###
#-----------------------------------------------------#
def muFastAlgSequence(ConfigFlags):

    ### set the EVCreator ###
    l2MuViewsMaker = EventViewCreatorAlgorithm("IMl2Mu")
    #
    l2MuViewsMaker.RoIsLink = "initialRoI" # ROI is from L1
    l2MuViewsMaker.RoITool = ViewCreatorInitialROITool() # ROI is from L1
    #
    l2MuViewsMaker.Views = "MUViewRoIs"
    l2MuViewsMaker.InViewRoIs = "MURoIs"
    #
    l2MuViewsMaker.ViewFallThrough = True

    ### get muFast reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muFastRecoSequence, makeMuonPrepDataAlgs
    viewAlgs_MuonPRD = makeMuonPrepDataAlgs(RoIs=l2MuViewsMaker.InViewRoIs)
    muFastRecoSequence, sequenceOut = muFastRecoSequence( l2MuViewsMaker.InViewRoIs )
    muFastSequence = parOR("muFastRecoSequence", [viewAlgs_MuonPRD, muFastRecoSequence])
    l2MuViewsMaker.ViewNodeName = muFastSequence.name() 
    
    l2muFastSequence = seqAND("l2muFastSequence", [ l2MuViewsMaker, muFastSequence ])
    return (l2muFastSequence, l2MuViewsMaker, sequenceOut)

def muFastSequence():
    
    (l2muFastSequence, l2MuViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muFastAlgSequence, ConfigFlags)

    ### set up MuFastHypo ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoAlg
    trigMufastHypo = TrigMufastHypoAlg("TrigL2MufastHypoAlg")
    trigMufastHypo.MuonL2SAInfoFromMuFastAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromDict

    return MenuSequence( Sequence    = l2muFastSequence,
                         Maker       = l2MuViewsMaker,
                         Hypo        = trigMufastHypo,
                         HypoToolGen = TrigMufastHypoToolFromDict  )


def muFastOvlpRmSequence():
    
    (l2muFastSequence, l2MuViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muFastAlgSequence, ConfigFlags)

    ### set up MuFastHypo ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoAlg
    trigMufastHypo = TrigMufastHypoAlg("TrigL2MufastHypoAlg")
    trigMufastHypo.MuonL2SAInfoFromMuFastAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromDict

    ### set up MuSAOverlapRemoval ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigL2MuonOverlapRemoverMufastAlg
    trigL2MuonOverlapRemover = TrigL2MuonOverlapRemoverMufastAlg("TrigL2MuonOverlapRemoverMufastAlg")
    trigL2MuonOverlapRemover.L2MuonOverlapInfoFromMuFastAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigL2MuonOverlapRemoverMufastToolFromDict
  
    return MenuSequence( Sequence    = l2muFastSequence,
                         Maker       = l2MuViewsMaker,
                         Hypo        = [trigMufastHypo, trigL2MuonOverlapRemover],
                         HypoToolGen = [TrigMufastHypoToolFromDict, TrigL2MuonOverlapRemoverMufastToolFromDict] )


#-----------------------------------------------------#
### ************* Step2  ************* ###
#-----------------------------------------------------#
def muCombAlgSequence(ConfigFlags):

    ### set the EVCreator ###
    l2muCombViewsMaker = EventViewCreatorAlgorithm("IMl2muComb")
    newRoITool = ViewCreatorCentredOnIParticleROITool()
    newRoITool.RoisWriteHandleKey = "HLT_l2SAMuROIs" # TODO - make this recordable()
    #
    l2muCombViewsMaker.RoIsLink = "initialRoI" # ROI for merging is still from L1, we get exactly one L2 SA muon per L1 ROI
    l2muCombViewsMaker.RoITool = newRoITool # Create a new ROI centred on the L2 SA muon from Step 1 
    #
    l2muCombViewsMaker.Views = "MUCombViewRoIs" #output of the views maker (key in "storegate")
    l2muCombViewsMaker.InViewRoIs = "MUIDRoIs" # Name of the RoI collection inside of the view, holds the single ROI used to seed the View.
    #
    l2muCombViewsMaker.RequireParentView = True
    l2muCombViewsMaker.ViewFallThrough = True #if this needs to access anything from the previous step, from within the view

    ### get ID tracking and muComb reco sequences ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup  import muCombRecoSequence, muonIDFastTrackingSequence
    muFastIDRecoSequence = muonIDFastTrackingSequence( l2muCombViewsMaker.InViewRoIs , "")
    muCombRecoSequence, sequenceOut = muCombRecoSequence( l2muCombViewsMaker.InViewRoIs )
 
    #Filter algorithm to run muComb only if non-Bphysics muon chains are active
    from TrigMuonEF.TrigMuonEFConfig import MuonChainFilterAlg
    muonChainFilter = MuonChainFilterAlg("FilterBphysChains")
    bphysChains =getBphysChainNames()
    muonChainFilter.ChainsToFilter=bphysChains
    if hasattr(l2muCombViewsMaker, "InputMakerOutputDecisions"):
        muonChainFilter.InputDecisions = [ l2muCombViewsMaker.InputMakerOutputDecisions ]
    muonChainFilter.L2MuCombContainer = sequenceOut

    muCombFilterSequence = seqAND("l2muCombFilterSequence", [muonChainFilter, muCombRecoSequence])
    muCombIDSequence = parOR("l2muCombIDSequence", [muFastIDRecoSequence, muCombFilterSequence])

    l2muCombViewsMaker.ViewNodeName = muCombIDSequence.name()

    l2muCombSequence = seqAND("l2muCombSequence", [l2muCombViewsMaker, muCombIDSequence] )

    return (l2muCombSequence, l2muCombViewsMaker, sequenceOut)



def muCombSequence():
   
    (l2muCombSequence, l2muCombViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muCombAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigmuCombHypoAlg
    #trigmuCombHypo = TrigmuCombHypoAlg("L2muCombHypoAlg") # avoid to have "Comb" string in the name due to HLTCFConfig.py. 
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBHypoAlg")
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigmuCombHypoToolFromDict

    return MenuSequence( Sequence    = l2muCombSequence,
                         Maker       = l2muCombViewsMaker,
                         Hypo        = trigmuCombHypo,
                         HypoToolGen = TrigmuCombHypoToolFromDict )
  


def muCombOvlpRmSequence():
   
    (l2muCombSequence, l2muCombViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muCombAlgSequence, ConfigFlags)

    ### set up muCombHypo algorithm ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigmuCombHypoAlg
    trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBHypoAlg")
    trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigmuCombHypoToolFromDict
    
    ### set up MuSAOverlapRemoval ###
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigL2MuonOverlapRemoverMucombAlg
    trigL2MuonOverlapRemover = TrigL2MuonOverlapRemoverMucombAlg("TrigL2MuonOverlapRemoverMucombAlg")
    trigL2MuonOverlapRemover.L2MuonOverlapInfoFromMuCombAlg = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigL2MuonOverlapRemoverMucombToolFromDict

    return MenuSequence( Sequence    = l2muCombSequence,
                         Maker       = l2muCombViewsMaker,
                         Hypo        = [trigmuCombHypo, trigL2MuonOverlapRemover],
                         HypoToolGen = [TrigmuCombHypoToolFromDict, TrigL2MuonOverlapRemoverMucombToolFromDict] )
 


######################
###  EFSA step ###
######################
def muEFSAAlgSequence(ConfigFlags):

    efsaViewsMaker = EventViewCreatorAlgorithm("IMefsa")
    #
    efsaViewsMaker.RoIsLink = "roi" # Merge based on L2SA muon
    efsaViewsMaker.RoITool = ViewCreatorPreviousROITool() # Spawn EventViews on L2SA muon ROI 
    #
    efsaViewsMaker.Views = "MUEFSAViewRoIs"
    efsaViewsMaker.InViewRoIs = "MUEFSARoIs"
    #
    efsaViewsMaker.RequireParentView = True
    efsaViewsMaker.ViewFallThrough = True

    ### get EF reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muEFSARecoSequence
    muEFSARecoSequence, sequenceOut = muEFSARecoSequence( efsaViewsMaker.InViewRoIs, 'RoI' )
 
    efsaViewsMaker.ViewNodeName = muEFSARecoSequence.name()
        
    muonEFSAonlySequence = seqAND( "muonEFSAonlySequence", [efsaViewsMaker, muEFSARecoSequence ] )

    return (muonEFSAonlySequence, efsaViewsMaker, sequenceOut)

def muEFSASequence():
    
    (muonEFSAonlySequence, efsaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFSAAlgSequence, ConfigFlags)

    # setup EFSA hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoAlg
    trigMuonEFSAHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFSAHypoAlg" )
    trigMuonEFSAHypo.MuonDecisions = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoToolFromDict
    
    return MenuSequence( Sequence    = muonEFSAonlySequence,
                         Maker       = efsaViewsMaker,
                         Hypo        = trigMuonEFSAHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict )



def muEFMSSequence():
    
    (muonEFSAonlySequence, efsaViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFSAAlgSequence, ConfigFlags)

    # setup EFSA hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoAlg
    trigMuonEFSAHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFMSonlyHypoAlg" )
    trigMuonEFSAHypo.MuonDecisions = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoToolFromDict
    
    return MenuSequence( Sequence    = muonEFSAonlySequence,
                         Maker       = efsaViewsMaker,
                         Hypo        = trigMuonEFSAHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict )


######################
###  EFCB seq ###
######################
def muEFCBAlgSequence(ConfigFlags):

    #By default the EFCB sequence will run both outside-in and 
    #(if zero muons are found) inside-out reconstruction
    from TrigMuonEF.TrigMuonEFConfig import MuonFilterAlg, MergeEFMuonsAlg
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muEFCBRecoSequence, muEFInsideOutRecoSequence
    
    efcbViewsMaker = EventViewCreatorAlgorithm("IMefcbtotal")
    #
    efcbViewsMaker.RoIsLink = "roi" # Merge based on L2SA muon
    efcbViewsMaker.RoITool = ViewCreatorPreviousROITool() # Spawn EventViews on L2SA muon ROI 
    #
    efcbViewsMaker.Views = "MUEFCBViewRoIs"
    efcbViewsMaker.InViewRoIs = "MUEFCBRoIs"
    #
    efcbViewsMaker.RequireParentView = True
    efcbViewsMaker.ViewFallThrough = True

    #outside-in reco sequence
    muEFCBRecoSequence, sequenceOutCB = muEFCBRecoSequence( efcbViewsMaker.InViewRoIs, "RoI" )

    #Algorithm to filter events with no muons
    muonFilter = MuonFilterAlg("FilterZeroMuons")
    muonFilter.MuonContainerLocation = sequenceOutCB
 
    #inside-out reco sequence - runs only if filter is passed
    muonEFInsideOutRecoSequence, sequenceOutInsideOut = muEFInsideOutRecoSequence( efcbViewsMaker.InViewRoIs, "RoI")
    muonInsideOutSequence = seqAND("muonEFInsideOutSequence", [muonFilter,muonEFInsideOutRecoSequence])

    #combine outside-in and inside-out sequences
    muonRecoSequence = parOR("muonEFCBandInsideOutRecoSequence", [muEFCBRecoSequence, muonInsideOutSequence])

    #Merge muon containers from outside-in and inside-out reco
    muonMerger = MergeEFMuonsAlg("MergeEFMuons")
    muonMerger.MuonCBContainerLocation = sequenceOutCB
    muonMerger.MuonInsideOutContainerLocation = sequenceOutInsideOut
    muonMerger.MuonOutputLocation = muNames.EFCBName
    sequenceOut = muonMerger.MuonOutputLocation

    #Add merging alg in sequence with reco sequences
    mergeSequence = seqOR("muonCBInsideOutMergingSequence", [muonRecoSequence, muonMerger])

    #Final sequence running in view
    efcbViewsMaker.ViewNodeName = mergeSequence.name()    
    muonSequence = seqAND("muonEFCBandInsideOutSequence", [efcbViewsMaker, mergeSequence])

    return (muonSequence, efcbViewsMaker, sequenceOut)

def muEFCBSequence():

    (muonEFCBSequence, efcbViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFCBAlgSequence, ConfigFlags)

    # setup EFCB hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoAlg
    trigMuonEFCBHypo = TrigMuonEFCombinerHypoAlg( "TrigMuonEFCombinerHypoAlg" )
    trigMuonEFCBHypo.MuonDecisions = sequenceOut
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoToolFromDict

    return MenuSequence( Sequence    = muonEFCBSequence,
                         Maker       = efcbViewsMaker,
                         Hypo        = trigMuonEFCBHypo,
                         HypoToolGen = TrigMuonEFCombinerHypoToolFromDict )

########################
### EF CB with dimuon ##
### mass cuts         ##
########################
def muEFCBInvMassSequence():

    from AthenaCommon import CfgMgr
    invMassRecoSequence = parOR("muInvMViewNode")
    
    invMViewsMaker = EventViewCreatorAlgorithm("IMmuinvm")
    #
    invMViewsMaker.RoIsLink = "roi" # Merge based on L2SA muon
    invMViewsMaker.RoITool = ViewCreatorPreviousROITool() # Spawn EventViews on L2SA muon ROI 
    #
    invMViewsMaker.Views = "muInvMViewRoIs"
    invMViewsMaker.InViewRoIs = "muInvMRoIs"
    #
    invMViewsMaker.RequireParentView = True
    invMViewsMaker.ViewFallThrough = True
    
    ViewVerifyEFCB = CfgMgr.AthViews__ViewDataVerifier("muInvMViewDataVerifier")
    ViewVerifyEFCB.DataObjects = [( 'xAOD::MuonContainer' , 'StoreGateSvc+'+muNames.EFCBName )]
    invMassRecoSequence += ViewVerifyEFCB
    invMassSequence = seqAND( "muInvMSequence", [invMViewsMaker, invMassRecoSequence] )

    invMViewsMaker.ViewNodeName = invMassRecoSequence.name()

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFInvMassHypoAlg, TrigMuonEFInvMassHypoToolFromDict

    trigMuonEFInvMHypo = TrigMuonEFInvMassHypoAlg( "TrigMuonEFInvMassHypoAlg" )
    trigMuonEFInvMHypo.MuonDecisions = muNames.EFCBName
    return MenuSequence( Sequence    = invMassSequence,
                         Maker       = invMViewsMaker,
                         Hypo        = trigMuonEFInvMHypo,
                         HypoToolGen = TrigMuonEFInvMassHypoToolFromDict )

######################
### EF SA full scan ###
######################
def muEFSAFSAlgSequence(ConfigFlags):

    efsafsInputMaker = EventViewCreatorAlgorithm("IMMuonFS")
    fsRoiTool = ViewCreatorFSROITool()
    fsRoiTool.RoisWriteHandleKey = "MuonFS_RoIs"
    #
    efsafsInputMaker.RoIsLink = "initialRoI" # Only expect to get in one FS RI
    efsafsInputMaker.RoITool = fsRoiTool # Use new FS roi (note: the ViewCreatorInitialROITool should work excactly the same here)
    #
    efsafsInputMaker.Views = "MUFSViewRoI"
    efsafsInputMaker.InViewRoIs = "MUFSRoIs"
    #
    efsafsInputMaker.ViewFallThrough=True

    ### get EF reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muEFSARecoSequence, makeMuonPrepDataAlgs
    viewAlgs_MuonPRD = makeMuonPrepDataAlgs(RoIs=efsafsInputMaker.InViewRoIs, forFullScan=True)
    muEFSAFSRecoSequence, sequenceOut = muEFSARecoSequence( efsafsInputMaker.InViewRoIs,'FS' )
 
    muEFFSRecoSequence = parOR("muEFSAFSRecoSequence",[viewAlgs_MuonPRD, muEFSAFSRecoSequence])
    efsafsInputMaker.ViewNodeName = muEFFSRecoSequence.name()

    muonEFSAFSSequence = seqAND( "muonEFSAFSSequence", [efsafsInputMaker, muEFFSRecoSequence ] )

    return (muonEFSAFSSequence, efsafsInputMaker, sequenceOut)

def muEFSAFSSequence():

    (muonEFSAFSSequence, efsafsInputMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFSAFSAlgSequence, ConfigFlags)

    # setup EFSA hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoAlg
    trigMuonEFSAFSHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFSAFSHypoAlg" )
    trigMuonEFSAFSHypo.MuonDecisions = sequenceOut
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFMSonlyHypoToolFromName
    
    return MenuSequence( Sequence    = muonEFSAFSSequence,
                         Maker       = efsafsInputMaker,
                         Hypo        = trigMuonEFSAFSHypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromName )

######################
### EF CB full scan ###
######################
def muEFCBFSAlgSequence(ConfigFlags):
    efcbfsInputMaker = EventViewCreatorAlgorithm("IMEFCBFS")
    newRoITool = ViewCreatorCentredOnIParticleROITool()
    newRoITool.RoisWriteHandleKey = "MuonCandidates_FS_ROIs"
    #
    efcbfsInputMaker.mergeUsingFeature = True
    efcbfsInputMaker.RoITool = newRoITool
    #
    efcbfsInputMaker.Views = "MUCBFSViews"
    efcbfsInputMaker.InViewRoIs = "MUCBFSRoIs"
    #
    efcbfsInputMaker.RequireParentView = True
    efcbfsInputMaker.ViewFallThrough = True
    # Muon specific
    efcbfsInputMaker.PlaceMuonInView = True
    efcbfsInputMaker.InViewMuons = "InViewMuons"
    efcbfsInputMaker.InViewMuonCandidates = "MuonCandidates_FS"

    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muEFCBRecoSequence
    muEFCBFSRecoSequence, sequenceOut = muEFCBRecoSequence( efcbfsInputMaker.InViewRoIs, "FS" )
 
    efcbfsInputMaker.ViewNodeName = muEFCBFSRecoSequence.name()
        
    muonEFCBFSSequence = seqAND( "muonEFFSCBSequence", [efcbfsInputMaker, muEFCBFSRecoSequence] )

    return (muonEFCBFSSequence, efcbfsInputMaker, sequenceOut)

def muEFCBFSSequence():

    (muonEFCBFSSequence, efcbfsInputMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFCBFSAlgSequence, ConfigFlags)

    # setup EFCB hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoAlg
    trigMuonEFCBFSHypo = TrigMuonEFCombinerHypoAlg( "TrigMuonEFFSCombinerHypoAlg" )
    trigMuonEFCBFSHypo.MuonDecisions = sequenceOut

    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoToolFromName

    return MenuSequence( Sequence    = muonEFCBFSSequence,
                         Maker       = efcbfsInputMaker,
                         Hypo        = trigMuonEFCBFSHypo,
                         HypoToolGen = TrigMuonEFCombinerHypoToolFromName )

def efLateMuRoIAlgSequence(ConfigFlags):

    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import efLateMuRoISequence
    
    eflateViewsMaker = EventViewCreatorAlgorithm("IMeflatemuroi") 
    #
    eflateViewsMaker.RoIsLink = "initialRoI"
    eflateViewsMaker.RoITool = ViewCreatorInitialROITool()
    #
    eflateViewsMaker.Views = "MULATEViewRoIs"
    eflateViewsMaker.InViewRoIs = "MULATERoIs"
    #
    eflateViewsMaker.ViewFallThrough = True

    #Get Late Muon RoIs
    efLateMuRoISequence, sequenceOut = efLateMuRoISequence()

    #Final sequence running in view
    eflateViewsMaker.ViewNodeName = efLateMuRoISequence.name()
    muonSequence = seqAND("lateMuonRoISequence", [eflateViewsMaker, efLateMuRoISequence])

    return (muonSequence, eflateViewsMaker, sequenceOut)

def efLateMuRoISequence():

    (muonEFLateSequence, eflateViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(efLateMuRoIAlgSequence, ConfigFlags)

    # setup EFCB hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonLateMuRoIHypoAlg
    trigMuonEFLateHypo = TrigMuonLateMuRoIHypoAlg( "TrigMuonLateMuRoIHypoAlg" )
    trigMuonEFLateHypo.LateRoIs = sequenceOut
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonLateMuRoIHypoToolFromDict

    return MenuSequence( Sequence    = muonEFLateSequence,
                         Maker       = eflateViewsMaker,
                         Hypo        = trigMuonEFLateHypo,
                         HypoToolGen = TrigMuonLateMuRoIHypoToolFromDict )


def efLateMuAlgSequence(ConfigFlags):

    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muEFInsideOutRecoSequence, makeMuonPrepDataAlgs, muonIDFastTrackingSequence
    eflateViewsMaker = EventViewCreatorAlgorithm("IMeflatemu")
    roiTool = ViewCreatorNamedROITool() # Use an existing ROI which is linked to the navigation with a custom name.
    roiTool.ROILinkName = "feature" # The ROI is actually linked as Step 1's feature. So the custom name is "feature".
    #
    eflateViewsMaker.mergeUsingFeature = True # Expect to have efLateMuRoIAlgSequence produce one Decision Object per lateROI, keep these distinct in the merging 
    eflateViewsMaker.RoITool = roiTool
    #
    eflateViewsMaker.Views = "MUEFLATEViewRoIs"
    eflateViewsMaker.InViewRoIs = "MUEFLATERoIs"
    #
    eflateViewsMaker.ViewFallThrough = True

    #decode data in these RoIs
    viewAlgs_MuonPRD = makeMuonPrepDataAlgs(RoIs=eflateViewsMaker.InViewRoIs)
    #ID fast tracking
    muFastIDRecoSequence = muonIDFastTrackingSequence( eflateViewsMaker.InViewRoIs,"Late" )
    #inside-out reco sequence 
    muonEFInsideOutRecoSequence, sequenceOut = muEFInsideOutRecoSequence( eflateViewsMaker.InViewRoIs, "LateMu")

    lateMuRecoSequence = parOR("lateMuonRecoSequence", [viewAlgs_MuonPRD, muFastIDRecoSequence, muonEFInsideOutRecoSequence])

    #Final sequence running in view
    eflateViewsMaker.ViewNodeName = lateMuRecoSequence.name()
    muonSequence = seqAND("lateMuonOutSequence", [eflateViewsMaker, lateMuRecoSequence])

    return (muonSequence, eflateViewsMaker, sequenceOut)

def efLateMuSequence():

    (muonEFLateSequence, eflateViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(efLateMuAlgSequence, ConfigFlags)

    # setup EFCB hypo
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoAlg
    trigMuonEFLateHypo = TrigMuonEFCombinerHypoAlg( "TrigMuonEFCombinerLateMuHypoAlg" )
    trigMuonEFLateHypo.MuonDecisions = sequenceOut
    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFCombinerHypoToolFromDict

    return MenuSequence( Sequence    = muonEFLateSequence,
                         Maker       = eflateViewsMaker,
                         Hypo        = trigMuonEFLateHypo,
                         HypoToolGen = TrigMuonEFCombinerHypoToolFromDict )



######################
### l2Muiso step ###
######################
def muIsoAlgSequence(ConfigFlags):

    l2muIsoViewsMaker = EventViewCreatorAlgorithm("IMl2muIso")
    #
    l2muIsoViewsMaker.RoIsLink = "roi" # Merge based on L2SA muon
    l2muIsoViewsMaker.RoITool = ViewCreatorPreviousROITool() # Spawn EventViews on L2SA muon ROI 
    #
    l2muIsoViewsMaker.Views = "MUIsoViewRoIs"
    l2muIsoViewsMaker.InViewRoIs = "MUIsoRoIs"
    #
    l2muIsoViewsMaker.RequireParentView = True
    l2muIsoViewsMaker.ViewFallThrough = True

    ### get EF reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup  import l2muisoRecoSequence
    l2muisoRecoSequence, sequenceOut = l2muisoRecoSequence( l2muIsoViewsMaker.InViewRoIs )
 
    l2muIsoViewsMaker.ViewNodeName = l2muisoRecoSequence.name()
     
    ### Define a Sequence to run for muIso ### 
    l2muIsoSequence = seqAND("l2muIsoSequence", [ l2muIsoViewsMaker, l2muisoRecoSequence ] )

    return (l2muIsoSequence, l2muIsoViewsMaker, sequenceOut)

def muIsoSequence():
 
    (l2muIsoSequence, l2muIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muIsoAlgSequence, ConfigFlags)

    # set up hypo    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuisoHypoAlg
    trigmuIsoHypo = TrigMuisoHypoAlg("L2MuisoHypoAlg")
    trigmuIsoHypo.MuonL2ISInfoName = sequenceOut
   
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuisoHypoToolFromDict

    return MenuSequence( Sequence    = l2muIsoSequence,
                         Maker       = l2muIsoViewsMaker,
                         Hypo        = trigmuIsoHypo,
                         HypoToolGen = TrigMuisoHypoToolFromDict )



######################
### efMuiso step ###
######################
def muEFIsoAlgSequence(ConfigFlags):
    efmuIsoViewsMaker = EventViewCreatorAlgorithm("IMefmuIso")
    newRoITool = ViewCreatorCentredOnIParticleROITool()
    newRoITool.RoIEtaWidth=0.15
    newRoITool.RoIPhiWidth=0.15
    newRoITool.RoisWriteHandleKey = "IsoMuRoIs"
    #
    efmuIsoViewsMaker.mergeUsingFeature = True
    efmuIsoViewsMaker.RoITool = newRoITool
    #
    efmuIsoViewsMaker.Views = "MUEFIsoViewRoIs"
    efmuIsoViewsMaker.InViewRoIs = "MUEFIsoRoIs"
    #
    efmuIsoViewsMaker.ViewFallThrough = True
    # Muon specific
    # TODO - this should be deprecated here and removed in the future, now that we mergeUsingFeature, each parent View should only have one muon.
    # therefore the xAOD::Muon should be got via ViewFallThrough, rather than being copied in here as "IsoViewMuons"
    efmuIsoViewsMaker.PlaceMuonInView = True
    efmuIsoViewsMaker.InViewMuonCandidates = "IsoMuonCandidates"
    efmuIsoViewsMaker.InViewMuons = "IsoViewMuons"

    ### get EF reco sequence ###    
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup  import efmuisoRecoSequence
    efmuisoRecoSequence, sequenceOut = efmuisoRecoSequence( efmuIsoViewsMaker.InViewRoIs, efmuIsoViewsMaker.InViewMuons )
 
    efmuIsoViewsMaker.ViewNodeName = efmuisoRecoSequence.name()
     
    ### Define a Sequence to run for muIso ### 
    efmuIsoSequence = seqAND("efmuIsoSequence", [ efmuIsoViewsMaker, efmuisoRecoSequence ] )

    return (efmuIsoSequence, efmuIsoViewsMaker, sequenceOut)

def muEFIsoSequence():
 
    (efmuIsoSequence, efmuIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muEFIsoAlgSequence, ConfigFlags)

    # set up hypo    
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFTrackIsolationHypoAlg
    trigmuefIsoHypo = TrigMuonEFTrackIsolationHypoAlg("EFMuisoHypoAlg")
    trigmuefIsoHypo.EFMuonsName = sequenceOut
   
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFTrackIsolationHypoToolFromDict

    return MenuSequence( Sequence    = efmuIsoSequence,
                         Maker       = efmuIsoViewsMaker,
                         Hypo        = trigmuefIsoHypo,
                         HypoToolGen = TrigMuonEFTrackIsolationHypoToolFromDict )
  
  
######################
### TrkMaterialProvider ###
######################
def TMEF_TrkMaterialProviderTool(name='TMEF_TrkMaterialProviderTool',**kwargs):
    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    kwargs.setdefault("UseCaloEnergyMeasurement", False)
    return Trk__TrkMaterialProviderTool(name,**kwargs)

##############################
### Get Bphysics triggers to # 
### filter chains where we   #
### don't want to run muComb #
##############################

def getBphysChainNames():

    from TriggerJobOpts.TriggerFlags import TriggerFlags
    bphysSlice = TriggerFlags.BphysicsSlice.signatures()
    chains =[]
    if bphysSlice:
        for chain in bphysSlice:
            chains.append(chain.name)
    return chains
