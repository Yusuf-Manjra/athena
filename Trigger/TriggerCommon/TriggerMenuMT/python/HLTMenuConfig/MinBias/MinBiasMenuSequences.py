#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR
from AthenaCommon.CFElements import seqAND
from TrigInDetConfig.InDetSetup import makeInDetAlgs
from TrigInDetConfig.EFIDTracking import makeInDetPatternRecognition
from TrigEDMConfig.TriggerEDMRun3 import recordable
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool
from TrigInDetConfig.ConfigSettings import getInDetTrigConfig


########
# to move into TrigMinBiasHypoConfigMT?

def SPCountHypoToolGen(chainDict):
    from TrigMinBias.TrigMinBiasConf import SPCountHypoTool
    hypo = SPCountHypoTool(chainDict["chainName"])
    if "hmt" in chainDict["chainName"]:
        hypo.sctSP = int(chainDict["chainParts"][0]["hypoL2Info"].strip("sp"))
    if "mb_sptrk" in chainDict["chainName"]:
        hypo.pixCL = 2
        hypo.sctSP = 3
            # will set here thresholds
    return hypo



def TrackCountHypoToolGen(chainDict):
    from TrigMinBias.TrigMinBiasConf import TrackCountHypoTool
    hypo = TrackCountHypoTool(chainDict["chainName"])
    if "hmt" in chainDict["chainName"]:
        hypo.minNtrks = int(chainDict["chainParts"][0]["hypoEFInfo"].strip("trk"))
    if "mb_sptrk" in chainDict["chainName"]:
        hypo.minPt = 0.2
        hypo.maxZ0 = 401
        # will set here cuts
    return hypo


### Now the sequences

def MinBiasSPSequence():
    spAlgsList = []
    from TrigMinBias.TrigMinBiasConf import TrigCountSpacePoints, SPCountHypoAlg

    spInputMakerAlg = EventViewCreatorAlgorithm("IM_SPEventViewCreator")
    spInputMakerAlg.ViewFallThrough = True
    spInputMakerAlg.RoITool = ViewCreatorInitialROITool()
    spInputMakerAlg.InViewRoIs = "InputRoI"
    spInputMakerAlg.Views = "SPView"

    idTrigConfig = getInDetTrigConfig('minBias')
    idAlgs, verifier = makeInDetAlgs(config=idTrigConfig, 
                                     rois=spInputMakerAlg.InViewRoIs, 
                                     viewVerifier='SPViewDataVerifier', 
                                     doFTF=False)
    verifier.DataObjects += [('TrigRoiDescriptorCollection', 'StoreGateSvc+InputRoI'),
                             ('SCT_ID', 'DetectorStore+SCT_ID'),
                             ('PixelID', 'DetectorStore+PixelID'),
                             ('TagInfo', 'DetectorStore+ProcessingTags')]

    # Make sure required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [('SCT_ID', 'DetectorStore+SCT_ID'),
                                       ('PixelID', 'DetectorStore+PixelID'),
                                       ('TagInfo', 'DetectorStore+ProcessingTags')]

#    spAlgsList = idAlgs[:-2]
    spAlgsList = idAlgs


    spCount = TrigCountSpacePoints()
    spCount.SpacePointsKey = recordable("HLT_SpacePointCounts")

    from TrigMinBias.TrigMinBiasMonitoring import SpCountMonitoring
    spCount.MonTool = SpCountMonitoring()

    spRecoSeq = parOR("spRecoSeq", spAlgsList + [spCount])
    spSequence = seqAND("spSequence", [spInputMakerAlg, spRecoSeq])
    spInputMakerAlg.ViewNodeName = spRecoSeq.name()


    spCountHypo =SPCountHypoAlg()
    spCountHypo.SpacePointsKey=recordable("HLT_SpacePointCounts")

    return MenuSequence(Sequence    = spSequence,
                        Maker       = spInputMakerAlg,
                        Hypo        = spCountHypo,
                        HypoToolGen = SPCountHypoToolGen )

def MinBiasZVertexFinderSequence():
    import AthenaCommon.CfgMgr as CfgMgr
    vdv = CfgMgr.AthViews__ViewDataVerifier( "VDVZFinderInputs" )
    vdv.DataObjects = [( 'SpacePointContainer' , 'StoreGateSvc+PixelTrigSpacePoints'), ( 'PixelID' , 'DetectorStore+PixelID' ) ]

    from IDScanZFinder.ZFinderAlgConfig import  MinBiasZFinderAlg
    ZVertFindRecoSeq = seqAND("ZVertFindRecoSeq", [ vdv, MinBiasZFinderAlg ])
    
    #idTrigConfig = getInDetTrigConfig('InDetSetup')
    ZVertFindInputMakerAlg = EventViewCreatorAlgorithm("IM_ZVertFinder")
    ZVertFindInputMakerAlg.ViewFallThrough = True
    ZVertFindInputMakerAlg.RoITool = ViewCreatorInitialROITool()
    ZVertFindInputMakerAlg.InViewRoIs = "InputRoI"
    ZVertFindInputMakerAlg.Views = "ZVertFinderView"
    ZVertFindInputMakerAlg.RequireParentView = True 
    ZVertFindInputMakerAlg.ViewNodeName =  ZVertFindRecoSeq.name()
    

    ZVertFindSequence = seqAND("ZVertFindSequence", [ZVertFindInputMakerAlg, ZVertFindRecoSeq])
    
    from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg
    from TrigStreamerHypo.TrigStreamerHypoConfig import StreamerHypoToolGenerator
    ZVertFindHypoAlg = TrigStreamerHypoAlg("ZVertFinderHypoAlg")
    ZVertFindHypoAlg.RuntimeValidation = False #Needed to avoid the ERROR ! Decision has no 'feature' ElementLink
    
    ZVertFindHypoToolGen = StreamerHypoToolGenerator

    return MenuSequence(Sequence    = ZVertFindSequence,
                        Maker       = ZVertFindInputMakerAlg,
                        Hypo        = ZVertFindHypoAlg,
                        HypoToolGen = ZVertFindHypoToolGen)



def MinBiasTrkSequence():
        from TrigMinBias.TrigMinBiasConf import TrackCountHypoAlg

        trkInputMakerAlg = EventViewCreatorAlgorithm("IM_TrkEventViewCreator")
        trkInputMakerAlg.ViewFallThrough = True
        trkInputMakerAlg.RoITool = ViewCreatorInitialROITool()
        trkInputMakerAlg.InViewRoIs = "InputRoI" # contract with the consumer
        trkInputMakerAlg.Views = "TrkView"
        trkInputMakerAlg.RequireParentView = True

        # prepare algorithms to run in views, first,
        # inform scheduler that input data is available in parent view (has to be done by hand)
        idTrigConfig = getInDetTrigConfig('minBias')

        algs,_ = makeInDetPatternRecognition(idTrigConfig, verifier='VDVMinBiasIDTracking')
        trackCountHypo = TrackCountHypoAlg()
        trackCountHypo.trackCountKey = recordable("HLT_TrackCount")
        trackCountHypo.tracksKey = recordable("HLT_IDTrack_MinBias_IDTrig")

        #TODO move a complete configuration of the algs to TrigMinBias package
        from TrigMinBias.TrigMinBiasMonitoring import TrackCountMonitoring
        trackCountHypo.MonTool = TrackCountMonitoring(trackCountHypo) # monitoring tool configures itself using config of the hypo alg

        trkRecoSeq = parOR("TrkRecoSeq", algs)
        trkSequence = seqAND("TrkSequence", [trkInputMakerAlg, trkRecoSeq])
        trkInputMakerAlg.ViewNodeName = trkRecoSeq.name()

        return MenuSequence(Sequence    = trkSequence,
                            Maker       = trkInputMakerAlg,
                            Hypo        = trackCountHypo,
                            HypoToolGen = TrackCountHypoToolGen)

def MinBiasMbtsSequence():
    from TrigMinBias.TrigMinBiasConf import MbtsHypoAlg, MbtsHypoTool
    from TrigMinBias.MbtsConfig import MbtsFexCfg
    fex = MbtsFexCfg(MbtsBitsKey=recordable("HLT_MbtsBitsContainer"))
    MbtsRecoSeq = parOR("MbtsRecoSeq", [fex])

    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
    MbtsInputMakerAlg = InputMakerForRoI("IM_Mbts", 
                                        RoIsLink="initialRoI", 
                                        RoITool = ViewCreatorInitialROITool(),
                                        RoIs='MbtsRoI', # not used in fact
                                        )

    MbtsSequence = seqAND("MbtsSequence", [MbtsInputMakerAlg, MbtsRecoSeq])

    hypo = MbtsHypoAlg("MbtsHypoAlg", MbtsBitsKey=fex.MbtsBitsKey)

    def hypoToolGen(chainDict):
        return MbtsHypoTool(chainDict["chainName"]) # to now no additional settings

    return MenuSequence(Sequence    = MbtsSequence,
                        Maker       = MbtsInputMakerAlg,
                        Hypo        = hypo,
                        HypoToolGen = hypoToolGen)
