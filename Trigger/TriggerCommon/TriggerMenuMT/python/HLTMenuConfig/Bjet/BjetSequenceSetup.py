#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# menu components
from AthenaCommon.CFElements import parOR, seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
from TrigEDMConfig.TriggerEDMRun3 import recordable

#from AthenaCommon.Constants import DEBUG

# ====================================================================================================  
#    Get MenuSequences
# ==================================================================================================== 

def getBJetSequence( step ):
    if step == "j":
        return bJetStep1Sequence()
    if step == "jALLTE":
        return bJetStep1SequenceALLTE()
    if step == "gsc":
        return bJetStep2Sequence()
    if step == "gscALLTE":
        return bJetStep2SequenceALLTE()
    if step == "btag":
        return bJetStep3Sequence()
    return None

# ==================================================================================================== 
#    step 1: jet reco and cut on 'j' threshold 
# ==================================================================================================== 

def bJetStep1Sequence():
    jetsKey = "HLT_AntiKt4EMTopoJets_subjesgscIS_ftf"
    prmVtxKey = "HLT_EFHistoPrmVtx"
    
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
    InputMakerAlg = InputMakerForRoI("IMstep1bjet", RoIsLink="initialRoI")
    InputMakerAlg.RoIs = "BjetInputRoI"

    # Shortlist of jets
    from TrigBjetHypo.TrigBjetHypoConf import TrigJetSplitterMT
    jetSplitter = TrigJetSplitterMT("TrigJetSplitterMT")
    jetSplitter.ImposeZconstraint = True
    jetSplitter.Jets = jetsKey
    jetSplitter.OutputJets = recordable("HLT_SplitJet")
    jetSplitter.OutputRoi = "SplitJets"
    jetSplitter.InputVertex = prmVtxKey

    bJetEtSequence = seqAND("bJetEtSequence",[ jetSplitter] )

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlgMT_step1")
    hypo.RoILink = "step1RoI" # To be used in following step EventView
    hypo.PrmVtxLink = "xPrimVx"
    hypo.Jets = jetSplitter.OutputJets
    hypo.RoIs = jetSplitter.OutputRoi
    hypo.PrmVtx = prmVtxKey

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step1",[InputMakerAlg,bJetEtSequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict )

def bJetStep1SequenceALLTE():

    # Construct jets
    from TrigUpgradeTest.jetMenuHelper import jetCFSequenceFromString
    (recoSequence, InputMakerAlg, sequenceOut) = jetCFSequenceFromString("a4_tc_em_subjes")

    # Start with b-jet-specific algo sequence
    # Construct RoI. Needed input for Fast Tracking
    from TrigBjetHypo.TrigBjetHypoConf import TrigRoIFromJetsMT
    RoIBuilder = TrigRoIFromJetsMT("TrigRoIFromJetsMT")
    RoIBuilder.JetInputKey = sequenceOut
    RoIs="EMViewRoIs" # Default for Fast Tracking Algs
    RoIBuilder.RoIOutputKey = RoIs


    # Fast Tracking 
    viewAlgs = makeInDetAlgs(whichSignature='FS',separateTrackParticleCreator="_FS",rois=RoIs)


    # Primary Vertex 
    from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxAllTEMTConfig import EFHistoPrmVtxAllTEMT_Jet
    prmVtx = EFHistoPrmVtxAllTEMT_Jet( "EFHistoPrmVtxAllTEMT_Jet" )
    prmVtx.InputRoIsKey = RoIs
    prmVtx.InputTracksKey = "HLT_xAODTracks_FS"
    prmVtx.OutputVertexKey = "EFHistoPrmVtx"
    
    # Shortlis of jets
    from TrigBjetHypo.TrigBjetHypoConf import TrigJetSplitterMT
    jetSplitter = TrigJetSplitterMT("TrigJetSplitterMT")
    jetSplitter.ImposeZconstraint = True 
    jetSplitter.Jets = sequenceOut
    jetSplitter.InputVertex = prmVtx.OutputVertexKey
    jetSplitter.OutputJets = "SplitJets"
    jetSplitter.OutputRoi = "SplitJets"

    fastTrackingSequence = parOR("fastTrackingSequence",viewAlgs)
    bJetEtSequence = seqAND("bJetEtSequence",[ RoIBuilder,fastTrackingSequence,prmVtx,jetSplitter] )

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlgMT_step1_ALLTE")
    hypo.RoILink = "step1RoI" # To be used in following step EventView
    hypo.PrmVtxLink = "xPrimVx"
    hypo.Jets = jetSplitter.OutputJets
    hypo.RoIs = jetSplitter.OutputRoi
    hypo.PrmVtx = prmVtx.OutputVertexKey

    # Sequence     
    BjetAthSequence = seqAND("BjetAthSequence_step1_ALLTE", [InputMakerAlg,recoSequence,bJetEtSequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict )



# ==================================================================================================== 
#    step 2: precision tracking, gsc calibration and cut on gsc-corrected threshold
# ==================================================================================================== 

def bJetStep2Sequence():

    # Event View Creator Algorithm
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithmWithJets
    InputMakerAlg = EventViewCreatorAlgorithmWithJets("IMBJet_step2", RoIsLink="initialRoI")
    InputMakerAlg.ViewFallThrough = True # Access Store Gate for retrieving data
    #InputMakerAlg.ViewPerRoI = True # If True it creates one view per RoI. NOTE: REMOVING AS NOT IMPLEMENTED
    InputMakerAlg.Views = "BJetViews" # Name of output view
    # RoIs
    InputMakerAlg.InViewRoIs = "InViewRoIs" # Name RoIs are inserted in the view
    InputMakerAlg.RoIsLink = "step1RoI" # RoIs linked to previous decision
    # Jets
    InputMakerAlg.InViewJets = "InViewJets" # Name Jets are inserted in the view

    # Precision Tracking
    #def makeInDetPrecisionTracking( whichSignature, verifier = False, inputFTFtracks='TrigFastTrackFinder_Tracks', outputTrackPrefixName = "InDetTrigPT" ):
    from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "bjets", inputFTFtracks="TrigFastTrackFinder_Tracks_FS" )

    # gsc correction
    from TrigBjetHypo.TrigGSCFexMTConfig import getGSCFexSplitInstance
    theGSC = getGSCFexSplitInstance("GSCFexSplitInstance")
    theGSC.RoIs = InputMakerAlg.InViewRoIs
    theGSC.JetKey = InputMakerAlg.InViewJets
    theGSC.TrackKey = PTTrackParticles[0]
    theGSC.PriVtxKey = "HLT_EFHistoPrmVtx"
    theGSC.JetOutputKey = recordable("HLT_GSCJet") 

    step2Sequence = seqAND("step2Sequence",[theGSC])
    InputMakerAlg.ViewNodeName = "step2Sequence"

    # to debug the View contnets  
    import AthenaCommon.CfgMgr as CfgMgr
    step2Sequence += CfgMgr.AthViews__ViewTestAlg("view_testBjet2")
    
    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgEVMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict
    hypo = TrigBjetEtHypoAlgEVMT("TrigBjetEtHypoAlg_step2")
    # no links this time, only for tracks
    hypo.TracksLink = "tracks"
    hypo.RoIs = "SplitJets"
    hypo.Jets = theGSC.JetOutputKey
    hypo.PrmVtx = theGSC.PriVtxKey
    hypo.Tracks = PTTrackParticles[0]

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step2",[InputMakerAlg] + PTAlgs + [step2Sequence])

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict )


def bJetStep2SequenceALLTE():
    # input maker
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
    InputMakerAlg = InputMakerForRoI("IMBJet_step2_ALLTE", RoIsLink="initialRoI")

    from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "bjets", inputFTFtracks="TrigFastTrackFinder_Tracks_FS" )

    # gsc correction
    from TrigBjetHypo.TrigGSCFexMTConfig import getGSCFexSplitInstance
    theGSC = getGSCFexSplitInstance("GSCFexSplitInstance_ALLTE")
    theGSC.TrackKey = PTTrackParticles[0]
    theGSC.JetKey = "SplitJets"
    theGSC.JetOutputKey = "GSCJets"
    theGSC.PriVtxKey = "EFHistoPrmVtx"

    # hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoAlgMT
    from TrigBjetHypo.TrigBjetEtHypoTool import TrigBjetEtHypoToolFromDict
    hypo = TrigBjetEtHypoAlgMT("TrigBjetEtHypoAlg_step2ALLTE")
    hypo.RoIs = "step1RoI"
    hypo.Jets = theGSC.JetOutputKey
    hypo.PrmVtx = theGSC.PriVtxKey

    hypo.TracksLink = "tracks__COLL"
    hypo.Tracks = PTTrackParticles[0] #"InDetTrigPTTrackParticles_bjets"

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step2ALLTE",[InputMakerAlg] + PTAlgs + [theGSC] )

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetEtHypoToolFromDict )

# ==================================================================================================== 
#    step 3: secondary vertex and b-tagging
# ==================================================================================================== 


def bJetStep3Sequence():

   # Event View Creator Algorithm
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithmWithJets
    InputMakerAlg = EventViewCreatorAlgorithmWithJets("IMBJet_step3")
    InputMakerAlg.ViewFallThrough = True # Access Store Gate for retrieving data
    #InputMakerAlg.ViewPerRoI = True # If True it creates one view per RoI. NOTE: REMOVING AS NOT IMPLEMENTED
    InputMakerAlg.Views = "BTagViews" # Name of output view
    # RoIs
    InputMakerAlg.InViewRoIs = "InViewRoIs" # Name RoIs are inserted in the view
    InputMakerAlg.RoIsLink = "step1RoI" # RoIs linked to previous decision
    # Jets
    InputMakerAlg.InViewJets = "InViewJets" # Name Jets are inserted in the view

    from TrigBjetHypo.TrigBtagFexMTConfig import getBtagFexSplitInstance
    bTagFex = getBtagFexSplitInstance( "EF","2012","" )
    bTagFex.JetKey = "InViewJets"
    bTagFex.PriVtxKey = "HLT_EFHistoPrmVtx"
    bTagFex.TracksKey = "InDetTrigPTTrackParticles_bjets"
    bTagFex.OutputBTagging = "HLT_BTagging"

    bTaggingSequence = seqAND("bTaggingSequence",[bTagFex] )
    InputMakerAlg.ViewNodeName = "bTaggingSequence"

    # Hypo
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetBtagHypoAlgMT
    from TrigBjetHypo.TrigBjetBtagHypoTool import TrigBjetBtagHypoToolFromDict
    hypo = TrigBjetBtagHypoAlgMT("TrigBjetBtagHypoAlg_step3")
    hypo.BTagging = bTagFex.OutputBTagging

    # Sequence
    BjetAthSequence = seqAND("BjetAthSequence_step3", [InputMakerAlg,bTaggingSequence] )

    return MenuSequence( Sequence    = BjetAthSequence,
                         Maker       = InputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigBjetBtagHypoToolFromDict )

