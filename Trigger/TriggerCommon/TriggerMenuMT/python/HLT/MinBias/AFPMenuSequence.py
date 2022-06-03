# 
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
#
from AthenaConfiguration.AllConfigFlags import ConfigFlags 
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence,algorithmCAToGlobalWrapper
from AthenaCommon.CFElements import parOR, seqAND
from AthenaConfiguration.ComponentFactory import CompFactory
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
from TriggerMenuMT.HLT.Config.MenuComponents import RecoFragmentsPool


def AFPTrkRecoBaseSequence(ConfigFlags):
    # Create inputs using input maker
    AFPInputMaker = InputMakerForRoI("IM_AFPTrackingFS")
    AFPInputMaker.RoITool = ViewCreatorInitialROITool()
    AFPInputMaker.RoIs = "AFPRoIs"

    from AthenaCommon.GlobalFlags import globalflags

    if globalflags.InputFormat.is_bytestream():
        # bytestream convertor
        AFP_Raw=CompFactory.AFP_RawDataProvider("AFP_RawDataProvider")

        # digitalization
        AFP_R2D=CompFactory.AFP_Raw2Digi("AFP_Raw2Digi")

    #cluster reconstruction

    from AFP_SiClusterTools.AFP_SiClusterTools import AFP_SiClusterTools_HLT
    AFP_SiCl= algorithmCAToGlobalWrapper(AFP_SiClusterTools_HLT,ConfigFlags)
    
    # tracks reconstruction
    from AFP_LocReco.AFP_LocReco import AFP_LocReco_SiD_HLT, AFP_LocReco_TD_HLT
    AFP_SID = algorithmCAToGlobalWrapper(AFP_LocReco_SiD_HLT,ConfigFlags)
    AFP_TD  = algorithmCAToGlobalWrapper( AFP_LocReco_TD_HLT,ConfigFlags)

    
    # protons reconstruction
    from AFP_GlobReco.AFP_GlobReco import AFP_GlobReco_HLT
    AFP_Pr= algorithmCAToGlobalWrapper(AFP_GlobReco_HLT,ConfigFlags)
  
    # vertex reconstruction
    from AFP_VertexReco.AFP_VertexReco import AFP_VertexReco_HLT
    AFP_Vtx=algorithmCAToGlobalWrapper(AFP_VertexReco_HLT,ConfigFlags)
    
    if globalflags.InputFormat.is_bytestream():
        AFPRecoSeq = parOR("AFPRecoSeq", [AFP_Raw, AFP_R2D, AFP_SiCl, AFP_SID, AFP_TD, AFP_Pr, AFP_Vtx])
    else:
        AFPRecoSeq = parOR("AFPRecoSeq", [AFP_SiCl, AFP_SID, AFP_TD, AFP_Pr, AFP_Vtx])
    
    return (AFPRecoSeq, AFPInputMaker)


def AFPTrkRecoSequence():

    # Retrieve input maker and reco seq
    (AFPPassThroughRecoSeq, AFPPassThroughInputMaker) = RecoFragmentsPool.retrieve(AFPTrkRecoBaseSequence,ConfigFlags)
    AFPPassThroughSequence = seqAND("AFPPassThroughSequence", [AFPPassThroughInputMaker, AFPPassThroughRecoSeq])

    # Edit pass-through hypo to use new ToF hypo
    def trigStreamerAFPHypoTool(chain_dict):
        from TrigAFPHypo.TrigAFPHypoConf import TrigAFPToFHypoTool
        hypoTool = TrigAFPToFHypoTool(chain_dict["chainName"])
        return hypoTool

    # HypoAlg
    from TrigAFPHypo.TrigAFPHypoConf import TrigAFPToFHypoAlg
    hypoAlg = TrigAFPToFHypoAlg("TrigAFPToFHypoAlg",
                                      AFPVertexContainer = 'HLT_AFPVertexContainer',
                                      VertexContainer = 'HLT_IDVertex_FS')
        
    trigHypoToolGen = trigStreamerAFPHypoTool

    return MenuSequence(Sequence = AFPPassThroughSequence,
                        Maker    = AFPPassThroughInputMaker,
                        Hypo     = hypoAlg,
                        HypoToolGen = trigHypoToolGen)

def TestTrigAFPDijetHypoToolGen(chainDict):
    from TrigAFPHypo.TrigAFPHypoConf import TestTrigAFPDijetHypoTool
    hypo = TestTrigAFPDijetHypoTool(chainDict["chainName"])

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool("MonTool_AFP_Hypo")
    monTool.defineHistogram('DijetMass', type='TH1F', path='EXPERT', title="Dijet mass", xbins=100, xmin=0, xmax=2000)
    monTool.defineHistogram('DijetRapidity', type='TH1F', path='EXPERT', title="Dijet rapidity", xbins=100, xmin=-5, xmax=5)

    monTool.defineHistogram('XiJet1', type='TH1F', path='EXPERT', title="Jet 1 xi", xbins=100, xmin=0, xmax=1)
    monTool.defineHistogram('XiJet2', type='TH1F', path='EXPERT', title="Jet 2 x1", xbins=100, xmin=0, xmax=1)

    monTool.defineHistogram('PredictProtonAEnergy', type='TH1F', path='EXPERT', title="Predicted proton energy A", xbins=100, xmin=0, xmax=10000)
    monTool.defineHistogram('PredictProtonCEnergy', type='TH1F', path='EXPERT', title="Predicted proton energy C", xbins=100, xmin=0, xmax=10000)

    monTool.defineHistogram('SideA_predictX', type='TH1F', path='EXPERT', title="Predicted X side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideA_predictY', type='TH1F', path='EXPERT', title="Predicted Y side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_predictX', type='TH1F', path='EXPERT', title="Predicted X side C", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_predictY', type='TH1F', path='EXPERT', title="Predicted Y side C", xbins=100, xmin=-100, xmax=100)

    monTool.defineHistogram('XDiff', type='TH1F', path='EXPERT', title="X difference", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('YDiff', type='TH1F', path='EXPERT', title="Y difference", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('distance', type='TH1F', path='EXPERT', title="distance", xbins=100, xmin=0, xmax=50)
    
    monTool.defineHistogram('SideA_trackX', type='TH1F', path='EXPERT', title="Track X side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideA_trackY', type='TH1F', path='EXPERT', title="Track Y side A", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideA_diffX', type='TH1F', path='EXPERT', title="Track X diff side A", xbins=100, xmin=-50, xmax=50)
    monTool.defineHistogram('SideA_diffY', type='TH1F', path='EXPERT', title="Track Y diff side A", xbins=100, xmin=-50, xmax=50)
    
    monTool.defineHistogram('SideC_trackX', type='TH1F', path='EXPERT', title="Track X side C", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_trackY', type='TH1F', path='EXPERT', title="Track Y side C", xbins=100, xmin=-100, xmax=100)
    monTool.defineHistogram('SideC_diffX', type='TH1F', path='EXPERT', title="Track X diff side C", xbins=100, xmin=-50, xmax=50)
    monTool.defineHistogram('SideC_diffY', type='TH1F', path='EXPERT', title="Track Y diff side C", xbins=100, xmin=-50, xmax=50)
    
    #monToolHypo.HistPath = 'AFPComboHypo/'+tool.getName()
    hypo.MonTool = monTool

    return hypo

def AFPTrkRecoHypoSequence():

    (AFPRecoSeqHypo, AFPInputMakerHypo) = RecoFragmentsPool.retrieve(AFPTrkRecoBaseSequence,ConfigFlags)
    AFPSequenceHypo = seqAND("AFPSequenceHypo", [AFPInputMakerHypo, AFPRecoSeqHypo])
    
    # Hypo
    from TrigAFPHypo.TrigAFPHypoConf import TestTrigAFPDijetHypoAlg
    hypoAlg = TestTrigAFPDijetHypoAlg("TestTrigAFPDijetHypoAlg",
                                   AFPTrackContainer = 'HLT_AFPTrackContainer',
                                   InputJetCollection = 'HLT_AntiKt4EMTopoJets_nojcalib')
    

    return MenuSequence(Sequence = AFPSequenceHypo,
                        Maker    = AFPInputMakerHypo,
                        Hypo     = hypoAlg,
                        HypoToolGen = TestTrigAFPDijetHypoToolGen)
