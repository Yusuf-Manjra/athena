from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestComboHypoAlg
from AthenaCommon.Constants import VERBOSE,DEBUG


from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
def InputMakerAlg(name):
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, LinkName="initialRoI")


# here define the sequences from the signatures
# signatures do this:
# - declare all the RecoAlg and the HypoAlg -> create the Sequence
# - creates the InputMaker, without the inputs

from TrigUpgradeTest.MenuComponents import NodeSequence, MenuSequence
from AthenaCommon.CFElements import parOR, seqAND, stepSeq


UseThisLinkName="initialRoI"
#UseThisLinkName="feature"

#### muon signatures
#####################

def TestHypoTool(name, prop, threshold_value):
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
    value  =  int(threshold_value)*1000
    h = HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=value, Property=prop, LinkName=UseThisLinkName)
    return h

def MuTestHypoTool(name):
    threshold = name.replace("HLT_mu","")
    return TestHypoTool(name,prop="pt", threshold_value=threshold)

def muMSRecAlg(name, FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def MuHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName, OutputLevel = DEBUG)

def MuStep1HypoTool():
   return "MuTestHypoTool"

def MuStep2HypoTool():
   return "MuTestHypoTool"


muIM= InputMakerAlg(name="Step1MuInputMaker")
muIM.Output='muIM_out'

muAlg = muMSRecAlg(name="muMSRecAlg", FileName="msmu.dat")
muAlg.Output='muAlg_out'

muHypo = MuHypo(name="Step1MuHypo")
muAlg.Input  = muIM.Output
muHypo.Input = muAlg.Output

mustep1_sequence = seqAND("muSeqStep1", [muIM, muAlg])
nodeSequence_mu = NodeSequence("muNodeSeq1",  Maker=muIM, Sequence=mustep1_sequence,  Seed="L1MU", )

def muStep1Sequence():
    return MenuSequence("muStep1", nodeSeqList=[nodeSequence_mu], Hypo=muHypo,HypoToolClassName=MuStep1HypoTool())

# mu step2
muIM2= InputMakerAlg(name="Step2MuInputMaker")
muIM2.Output='muIM2_out'

muAlg2 = muMSRecAlg(name="muMSRecAlg2", FileName="msmu.dat")
muAlg2.Output='muAlg2_out'

muHypo2 = MuHypo(name="Step2MuHypo")
muAlg2.Input  = muIM2.Output
muHypo2.Input = muAlg2.Output

mustep2_sequence = seqAND("muSeqStep2", [muIM2, muAlg2])
nodeSequence2_mu = NodeSequence("muNodeSeq2", Sequence=mustep2_sequence, Maker=muIM2, Seed="L1MU")

def muStep2Sequence():
    return MenuSequence("muStep2", nodeSeqList=[nodeSequence2_mu], Hypo=muHypo2, HypoToolClassName=MuStep2HypoTool())


## ##### electron signatures
## ##########################


def ElTestHypoTool(name):
    threshold = name.replace("HLT_e","")
    return TestHypoTool(name,prop="et", threshold_value=threshold)

def CaloClustering(name,  FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def ElGamHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName, OutputLevel = DEBUG)


def ElStep1HypoTool():
   return "ElTestHypoTool"

def ElStep2HypoTool():
   return "ElTestHypoTool"



elIM= InputMakerAlg(name="Step1ElInputMaker")
elIM.Output='elIM_out'

elAlg = CaloClustering(name="CaloClustering", FileName="emclusters.dat")
elAlg.Output='elAlg_out'

elHypo = ElGamHypo(name="Step1ElHypo")
elAlg.Input  = elIM.Output
elHypo.Input = elAlg.Output

elstep1_sequence = seqAND("elSeqStep1", [elIM, elAlg])
nodeSequence_el = NodeSequence("elNodeSeq1",  Maker=elIM, Sequence=elstep1_sequence, Seed="L1EM")

def elStep1Sequence():
    return MenuSequence("elStep1", nodeSeqList=[nodeSequence_el],  Hypo=elHypo, HypoToolClassName=ElStep1HypoTool())

#step2
elIM2= InputMakerAlg(name="Step2ElInputMaker")
elIM2.Output='elIM2_out'

elAlg2 = CaloClustering(name="CaloClustering2", FileName="emclusters.dat")
elAlg2.Output='elAlg2_out'

elHypo2 = ElGamHypo(name="Step2ElHypo")
elAlg2.Input  = elIM2.Output
elHypo2.Input = elAlg2.Output

elstep2_sequence = seqAND("elSeqStep2", [elIM2, elAlg2])
nodeSequence2_el = NodeSequence("elNodeSeq2",  Maker=elIM2, Sequence=elstep2_sequence,  Seed="L1EM")

def elStep2Sequence():
    return MenuSequence("elStep2", nodeSeqList=[nodeSequence2_el], Hypo=elHypo2, HypoToolClassName=ElStep2HypoTool())


# combined Hypo #
#################

class ComboTestHypoTool(HLTTest__TestHypoTool):
    def __init__(self,name):
        #doing nothing
        HLTTest__TestHypoTool.__init__(self,name, OutputLevel=DEBUG, Property="pt")

def ComboMuEHypo(name):
    return HLTTest__TestComboHypoAlg(name=name, OutputLevel = DEBUG, Property1="pt", Property2="et")

def ComboStep1HypoTool():
   return "ComboTestHypoTool"

def ComboStep2HypoTool():
   return "ComboTestHypoTool"

comboAlg = ComboMuEHypo("Step1ComboMuElHypo")
comboAlg.Input1=muAlg.Output
comboAlg.Input2=elAlg.Output

nodeSequence_muComb = NodeSequence("Combo1NodeSeq1",  Maker=muIM, Sequence=mustep1_sequence, Seed="L1MU")
nodeSequence_elComb = NodeSequence("Combo2NodeSeq1",  Maker=elIM, Sequence=elstep1_sequence, Seed="L1EM")


def combStep1Sequence():
    return MenuSequence("ComboStep1", nodeSeqList=[nodeSequence_muComb,nodeSequence_elComb],  Hypo=comboAlg, HypoToolClassName=ComboStep1HypoTool())

# step2

comboAlg2 = ComboMuEHypo("Step2ComboMuElHypo")
comboAlg2.Input1=muAlg2.Output
comboAlg2.Input2=elAlg2.Output

nodeSequence2_muComb = NodeSequence("Combo1NodeSeq2",  Maker=muIM2, Sequence=mustep2_sequence,  Seed="L1MU")
nodeSequence2_elComb = NodeSequence("Combo2NodeSeq2",  Maker=elIM2, Sequence=elstep2_sequence,  Seed="L1EM")

def combStep2Sequence():
    return MenuSequence("ComboStep2", nodeSeqList=[nodeSequence2_muComb,nodeSequence2_elComb], Hypo=comboAlg2,HypoToolClassName=ComboStep2HypoTool())
