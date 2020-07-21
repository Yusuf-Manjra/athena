# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Content included in addition to the smart slimming content

# ==========================================================================================================================
# Extra content
# ==========================================================================================================================

ExtraContentTracks               = ["InDetTrackParticles.eProbabilityHT"]

ExtraContentPhotons              = ["Photons.Loose.Medium.Tight"]
ExtraContentPhotonsTruth         = [ ]

ExtraContentElectrons            = ["Electrons.etcone20.etcone30.etcone40.ptcone20.ptcone30.ptcone40.Loose.Medium.Tight"]
ExtraContentElectronsTruth       = [ ]

ExtraContentMuons                = ["Muons.etcone20.etcone30.etcone40"]
ExtraContentMuonsTruth           = [ ]

ExtraContentTaus                 = [   "TauJets.jetLink."
                                       + "caloIso."
                                       + "isolFrac."
                                       + "IsTruthMatched."
                                       + "truthJetLink."
                                       + "ptDetectorAxis."
                                       + "etaDetectorAxis."
                                       + "phiDetectorAxis."
                                       + "mDetectorAxis."
                                       + "ptIntermediateAxis."
                                       + "etaIntermediateAxis."
                                       + "phiIntermediateAxis."
                                       + "mIntermediateAxis."
                                       + "leadTrkPt."
                                       + "centFrac."
                                       #+ "centFracCorrected."
                                       + "ChPiEMEOverCaloEME."
                                       #+ "ChPiEMEOverCaloEMECorrected."
                                       + "dRmax."
                                       #+ "dRmaxCorrected."
                                       + "etOverPtLeadTrk."
                                       #+ "etOverPtLeadTrkCorrected."
                                       + "EMPOverTrkSysP."
                                       #+ "EMPOverTrkSysPCorrected."
                                       + "innerTrkAvgDist."
                                       #+ "innerTrkAvgDistCorrected."
                                       + "ipSigLeadTrk."
                                       #+ "ipSigLeadTrkCorrected."
                                       + "massTrkSys."
                                       #+ "massTrkSysCorrected."
                                       + "mEflowApprox."
                                       #+ "mEflowApproxCorrected."
                                       + "ptRatioEflowApprox."
                                       #+ "ptRatioEflowApproxCorrected."
                                       + "SumPtTrkFrac."
                                       #+ "SumPtTrkFracCorrected."
                                       + "trFlightPathSig."
                                       #+ "trFlightPathSigCorrected."
                                       + "PSSFraction."
                                       + "etEMAtEMScale."
                                       + "etHadAtEMScale."
                                       + "sumEMCellEtOverLeadTrkPt."
                                       + "hadLeakEt."
                                       + "secMaxStripEt."
                                       + "EMFracFixed."
                                       + "etHotShotWinOverPtLeadTrk."
                                       + "hadLeakFracFixed."
                                       + "leadTrackProbHT."
                                       + "ClustersMeanCenterLambda."
                                       + "ClustersMeanFirstEngDens."
                                       + "ClustersMeanSecondLambda."
                                       + "ClustersMeanPresamplerFrac."
                                       + "ClustersMeanEMProbability."
                                       + "PFOEngRelDiff."
                                       + "GhostMuonSegmentCount."
                                       + "ptPanTauCellBased."
                                       + "ptPanTauCellBasedProto."
                                       + "etaPanTauCellBased."
                                       + "etaPanTauCellBasedProto."
                                       + "phiPanTauCellBased."
                                       + "phiPanTauCellBasedProto."
                                       + "mPanTauCellBased."
                                       + "mPanTauCellBasedProto."
                                       + "clusterLinks."
                                       + "hadronicPFOLinks."
                                       + "shotPFOLinks."
                                       + "chargedPFOLinks."
                                       + "neutralPFOLinks."
                                       + "pi0PFOLinks."
                                       + "protoChargedPFOLinks."
                                       + "protoNeutralPFOLinks."
                                       + "protoPi0PFOLinks."
                                       + "eflowRec_Charged_PFOLinks."
                                       + "eflowRec_Neutral_PFOLinks."
                                       + "eflowRec_Pi0_PFOLinks."
                                       + "shot_PFOLinks."
                                       + "charged_PFOLinks."
                                       + "neutral_PFOLinks."
                                       + "pi0_PFOLinks."
                                       + "pi0Links."
                                       + "cellBased_Charged_PFOLinks."
                                       + "cellBased_Neutral_PFOLinks."
                                       + "cellBased_Pi0_PFOLinks."
                                       + "PanTau_isPanTauCandidate.PanTau_DecayModeProto.PanTau_BDTValue_1p0n_vs_1p1n.PanTau_BDTValue_1p1n_vs_1pXn.PanTau_BDTValue_3p0n_vs_3pXn.PanTau_BDTVar_Basic_NNeutralConsts.PanTau_BDTVar_Charged_JetMoment_EtDRxTotalEt.PanTau_BDTVar_Charged_StdDev_Et_WrtEtAllConsts.PanTau_BDTVar_Neutral_HLV_SumM.PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_1.PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_2.PanTau_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts.PanTau_BDTVar_Neutral_Ratio_EtOverEtAllConsts.PanTau_BDTVar_Neutral_Shots_NPhotonsInSeed.PanTau_BDTVar_Combined_DeltaR1stNeutralTo1stCharged.PanTau_DecayModeExtended",
                                       "TauChargedParticleFlowObjects",
                                       "TauNeutralParticleFlowObjects",
                                       "TauHadronicParticleFlowObjects",
                                       "TauShotParticleFlowObjects",
                                       "TauPi0Clusters"   ]
ExtraContentTausTruth               = [ ]
ExtraContentJetsTruth               = ["AntiKt4TruthJets.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID"]

# ==========================================================================================================================

ExtraContentTAUP1              =   ExtraContentPhotons                  \
                                 + ExtraContentElectrons                \
                                 + ExtraContentMuons                    \
                                 + ExtraContentTaus                     \
                                 + ExtraContentTracks

# ExtraContentTAUP2              =   ExtraContentTaus

ExtraContentTAUP3              =   ExtraContentPhotons                  \
                                 + ExtraContentElectrons                \
                                 + ExtraContentMuons                    \
                                 + ExtraContentTaus                     \
                                 + ExtraContentTracks

# ExtraContentTAUP4              =   ExtraContentPhotons                  \
#                                  + ExtraContentElectrons                \
#                                  + ExtraContentMuons
#                                  + ExtraContentTaus

ExtraContentTAUP5              =   ExtraContentPhotons                  \
                                 + ExtraContentElectrons                \
                                 + ExtraContentMuons                    

ExtraContentTruthTAUP1         =   ExtraContentPhotonsTruth             \
                                 + ExtraContentElectronsTruth           \
                                 + ExtraContentMuonsTruth               \
                                 + ExtraContentTausTruth                \
                                 + ExtraContentJetsTruth

ExtraContentTruthTAUP2         =   ExtraContentElectronsTruth           \
                                 + ExtraContentMuonsTruth               \
                                 + ExtraContentTausTruth                \
                                 + ExtraContentJetsTruth

ExtraContentTruthTAUP3         =   ExtraContentPhotonsTruth             \
                                 + ExtraContentElectronsTruth           \
                                 + ExtraContentMuonsTruth               \
                                 + ExtraContentTausTruth                \
                                 + ExtraContentJetsTruth

# ExtraContentTruthTAUP4       =   ExtraContentPhotonsTruth             \
#                                + ExtraContentElectronsTruth           \
#                                + ExtraContentMuonsTruth               \
#                                + ExtraContentTausTruth

ExtraContentTruthTAUP5         =   ExtraContentPhotonsTruth             \
                                 + ExtraContentElectronsTruth           \
                                 + ExtraContentMuonsTruth               \
                                 + ExtraContentTausTruth                \
                                 + ExtraContentJetsTruth

# ==========================================================================================================================
# Extra containers
# ==========================================================================================================================

ExtraContainersElectrons         = []  #    ["ForwardElectrons"] -- removed for rel21, not needed

ExtraContainersJets              = ["CaloCalTopoClusters"]

ExtraContainersTrigger           = ["LVL1EmTauRoIs",
                                    "LVL1JetRoIs",
                                    "HLT_TrigRoiDescriptorCollection_forID",
                                    "HLT_TrigRoiDescriptorCollection_forID1",
                                    "HLT_TrigRoiDescriptorCollection_forID2",
                                    "HLT_TrigRoiDescriptorCollection_forID3",
                                    "HLT_TrigRoiDescriptorCollection_forMS",
                                    "HLT_TrigRoiDescriptorCollection_initialRoI",
                                    "HLT_TrigRoiDescriptorCollection_secondaryRoI_EF",
                                    "HLT_TrigRoiDescriptorCollection_secondaryRoI_HLT",
                                    "HLT_TrigRoiDescriptorCollection_secondaryRoI_L2",
                                    "HLT_xAOD__EmTauRoIContainer_L1TopoEM",
                                    "HLT_xAOD__EmTauRoIContainer_L1TopoTau",
                                    "HLT_xAOD__JetContainer_TrigTauJet",
                                    "HLT_xAOD__TauJetContainer_TrigTauRecPreselection",
                                    "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_EFID",
                                    "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF",
                                    "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_IDTrig",
                                    "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTF",
                                    "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF"]

ExtraContainersPions             = ["finalTauPi0s"]

# ==========================================================================================================================

ExtraContainersTAUP1            =   ExtraContainersElectrons    \
                                  + ExtraContainersJets         \
                                  + ExtraContainersTrigger      \
                                  + ExtraContainersPions

ExtraContainersTAUP2            =   ExtraContainersTrigger      \
                                  + ["HLT_xAOD__TauJetContainer_TrigTauRecMerged",
#                                     "TauJets", # do not add this, it is in TauJetsCPContent and duplication causes merging to crash
                                     "TauNeutralParticleFlowObjects",
                                     "TauHadronicParticleFlowObjects",
                                     "TauShotParticleFlowObjects",
                                     "TauChargedParticleFlowObjects",
                                     "PrimaryVertices",
                                     "AntiKt4LCTopoJets",
                                     "CaloCalTopoClusters",
                                     "InDetTrackParticles"]

ExtraContainersTAUP3            =   ExtraContainersElectrons   \
                                  + ExtraContainersJets        \
                                  + ExtraContainersTrigger    \
                                  + ExtraContainersPions       \
                                  + ["HLT_xAOD__TauJetContainer_TrigTauRecMerged",
                                     "HLT_xAOD__MuonRoIContainer_L1TopoMuon",
                                     "HLT_xAOD__JetRoIContainer_L1TopoJet",
                                     "HLT_xAOD__TrigCompositeContainer_L1TopoMET",
                                     "HLT_xAOD__TrigCompositeContainer_L1TopoComposite"]

ExtraContainersTAUP4            =   ExtraContainersTrigger      \
                                  + ["TauJets",
                                     "TauNeutralParticleFlowObjects",
                                     "TauHadronicParticleFlowObjects",
                                     "TauShotParticleFlowObjects",
                                     "TauChargedParticleFlowObjects",
                                     "PrimaryVertices",
                                     "AntiKt4LCTopoJets",
                                     "CaloCalTopoClusters",
                                     "InDetTrackParticles",
                                     "HLT_xAOD__TauJetContainer_TrigTauRecMerged",
                                     "HLT_xAOD__JetRoIContainer_L1TopoJet",
                                     "HLT_xAOD__TrigCompositeContainer_L1TopoComposite"]

ExtraContainersTAUP5            =   ExtraContainersElectrons   

ExtraContainersTruthTAUP1      = ["TruthEvents",
                                  "TruthParticles",
                                  "TruthVertices",
                                  "TruthTaus"
                                 #"AntiKt4TruthJets",
                                 #"BTagging_AntiKt4Truth", JRC TEMPORARILY COMMENTED
                                 #"AntiKt4TruthWZJets"
                                 ]

ExtraContainersTruthTAUP2      = ["TruthEvents",
                                  "TruthParticles",
                                  "TruthVertices"]
#                                 "AntiKt4TruthJets",
                                 #"BTagging_AntiKt4Truth", JRC TEMPORARILY COMMENTED
#                                 "AntiKt4TruthWZJets"]

ExtraContainersTruthTAUP3      = ["TruthEvents",
                                  "TruthParticles",
                                  "TruthVertices",
                                  "TruthTaus"
                                 #"AntiKt4TruthJets",
                                 #"BTagging_AntiKt4Truth", JRC TEMPORARILY COMMENTED
                                 #"AntiKt4TruthWZJets"
                                 ]

# ExtraContainersTruthTAUP4    = ["TruthEvents",
#                                 "TruthParticles",
#                                 "TruthVertices",
#                                 "AntiKt4TruthJets",
#                                #"BTagging_AntiKt4Truth", JRC TEMPORARILY COMMENTED
#                                 "AntiKt4TruthWZJets"]
ExtraContainersTruthTAUP5      = ["TruthEvents",
                                  "TruthParticles",
                                  "TruthVertices",
                                  "AntiKt4TruthJets",
                                 #"BTagging_AntiKt4Truth", JRC TEMPORARILY COMMENTED
                                  "AntiKt4TruthWZJets"
                                 ]
