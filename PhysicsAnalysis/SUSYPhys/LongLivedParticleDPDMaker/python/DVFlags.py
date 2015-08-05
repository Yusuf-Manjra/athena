# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class DV_containerFlags(JobProperty):
    statusOn = True
    photonCollectionName='Photons'
    electronCollectionName='Electrons'
    muonCollectionName='Muons'
    jetCollectionName="AntiKt4EMTopoJets"
    METCollectionName="MET_LocHadTopo"
    pass
primRPVLLDESDM.add_JobProperty(DV_containerFlags)

class DV_MultiJetTriggerFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    triggers = ["HLT_4j100","HLT_5j85","HLT_5j85_lcw","HLT_5j75_0eta250","EF_6j70","HLT_6j45_0eta240","HLT_7j45"]
    pass
primRPVLLDESDM.add_JobProperty(DV_MultiJetTriggerFlags)

### multi-jet filter just to verify trigger - 4j80 OR 5j55 OR 6j45

class DV_4JetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=100.0*Units.GeV
    nPassed=4
    pass
primRPVLLDESDM.add_JobProperty(DV_4JetFilterFlags)

class DV_5JetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=75.0*Units.GeV
    nPassed=5
    pass
primRPVLLDESDM.add_JobProperty(DV_5JetFilterFlags)

class DV_6JetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=50.0*Units.GeV
    nPassed=6
    pass
primRPVLLDESDM.add_JobProperty(DV_6JetFilterFlags)

class DV_7JetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=45.0*Units.GeV
    nPassed=7
    pass
primRPVLLDESDM.add_JobProperty(DV_7JetFilterFlags)

class DV_MuonFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    cutEtMin=60.0*Units.GeV
    triggers=["HLT_mu60_0eta105_msonly"]
    nPassed=1
    pass
primRPVLLDESDM.add_JobProperty(DV_MuonFilterFlags)

class DV_PhotonFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue= True
    cutEtMin=140.0*Units.GeV
    triggers=["HLT_g140_loose"]
    nPassed=1
    pass
primRPVLLDESDM.add_JobProperty(DV_PhotonFilterFlags)


class DV_METFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    cutMetMin=100.0*Units.GeV
    deltaPhiCut=0.1
    triggers=["HLT_xe100","HLT_xe100_tc_lcw","HLT_xe100_tc_lcw_wEFMu","HLT_xe100_wEFMu"]
    pass
primRPVLLDESDM.add_JobProperty(DV_METFilterFlags)

class DV_SingleTracklessJetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    cutEtMin=50.0*Units.GeV
    cutEtaMax=2.5
    cutSumPtTrkMax=5.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(DV_SingleTracklessJetFilterFlags)

class DV_DoubleTracklessJetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    cutEtMin=50.0*Units.GeV
    cutEtaMax=2.5
    cutSumPtTrkMax=5.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(DV_DoubleTracklessJetFilterFlags)

class DV_MeffFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutMeffMin=1.0*Units.TeV ##
    cutMEToverMeffMin=0.3   ## note that these two cuts are ORed in the code!
    cutJetPtMin=40.0*Units.GeV
    cutJetEtaMax=2.5
    cutMETMin=80.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(DV_MeffFilterFlags)

class DV_PrescalerFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    prescale=10
    pass
primRPVLLDESDM.add_JobProperty(DV_PrescalerFlags)
