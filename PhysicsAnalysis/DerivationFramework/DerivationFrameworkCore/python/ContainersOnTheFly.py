# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# List of containers that are made on-the-fly by basically all DAOD types and
# can therefore be reasonably added to the NameAndTypes dictionary centrally
# rather than asking each DAOD to do it themselves
ContainersOnTheFly = [
                        ['TruthEvents','xAOD::TruthEventContainer'],
                        ['TruthEventsAux','xAOD::TruthEventAuxContainer'],
                        ['MET_Truth','xAOD::MissingETContainer'],
                        ['MET_TruthAux','xAOD::MissingETAuxContainer'],
                        ['MET_TruthRegions','xAOD::MissingETContainer'],
                        ['MET_TruthRegionsAux','xAOD::MissingETAuxContainer'],
                        ['TruthBSM','xAOD::TruthParticleContainer'],
                        ['TruthBSMAux','xAOD::TruthParticleAuxContainer'],
                        ['TruthBoson','xAOD::TruthParticleContainer'],
                        ['TruthBosonAux','xAOD::TruthParticleAuxContainer'],
                        ['TruthBottom','xAOD::TruthParticleContainer'],
                        ['TruthBottomAux','xAOD::TruthParticleAuxContainer'],
                        ['TruthTop','xAOD::TruthParticleContainer'],
                        ['TruthTopAux','xAOD::TruthParticleAuxContainer'],
                        ["TruthMuons","xAOD::TruthParticleContainer"],
                        ["TruthMuonsAux","xAOD::TruthParticleAuxContainer"],
                        ["TruthElectrons","xAOD::TruthParticleContainer"],
                        ["TruthElectronsAux","xAOD::TruthParticleAuxContainer"],
                        ["TruthPhotons","xAOD::TruthParticleContainer"],
                        ["TruthPhotonsAux","xAOD::TruthParticleAuxContainer"],
                        ["TruthNeutrinos","xAOD::TruthParticleContainer"],
                        ["TruthNeutrinosAux","xAOD::TruthParticleAuxContainer"],
                        ["TruthTaus","xAOD::TruthParticleContainer"],
                        ["TruthTausAux","xAOD::TruthParticleAuxContainer"],
                        ['TruthWbosonWithDecayParticles','xAOD::TruthParticleContainer'],
                        ["TruthWbosonWithDecayParticlesAux",'xAOD::TruthParticleAuxContainer'],
                        ['TruthWbosonWithDecayVertices','xAOD::TruthVertexContainer'],
                        ['TruthWbosonWithDecayVerticesAux','xAOD::TruthVertexAuxContainer'],
                        ["AntiKt2TruthJets","xAOD::JetContainer"],
                        ["AntiKt2TruthJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4TruthJets","xAOD::JetContainer"],
                        ["AntiKt4TruthJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4TruthWZJets","xAOD::JetContainer"],
                        ["AntiKt4TruthWZJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4TruthDressedWZJets","xAOD::JetContainer"],
                        ["AntiKt4TruthDressedWZJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt2PV0TrackJets","xAOD::JetContainer"],
                        ["AntiKt2PV0TrackJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4PV0TrackJets","xAOD::JetContainer"],
                        ["AntiKt4PV0TrackJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackGhostTagJets","xAOD::JetContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackGhostTagJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810","xAOD::JetContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810Aux","xAOD::JetAuxContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTag","xAOD::JetContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTagAux","xAOD::JetAuxContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903GhostTag","xAOD::JetContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903GhostTagAux","xAOD::JetAuxContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903","xAOD::JetContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903Aux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASubJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASubJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASubJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASubJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt2LCTopoJets","xAOD::JetContainer"],
                        ["AntiKt2LCTopoJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4EMTopoCSSKJets","xAOD::JetContainer"],
                        ["AntiKt4EMTopoCSSKJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4EMPFlowCSSKJets","xAOD::JetContainer"],
                        ["AntiKt4EMPFlowCSSKJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4EMTopoJets_BTagging201810","xAOD::JetContainer"],
                        ["AntiKt4EMTopoJets_BTagging201810Aux","xAOD::ShallowAuxContainer"],
                        ["AntiKt4EMPFlowJets_BTagging201903","xAOD::JetContainer"],
                        ["AntiKt4EMPFlowJets_BTagging201903Aux","xAOD::ShallowAuxContainer"],
                        ["AntiKt4EMPFlowJets_BTagging201810","xAOD::JetContainer"],
                        ["AntiKt4EMPFlowJets_BTagging201810Aux","xAOD::ShallowAuxContainer"],
                        ["AntiKt10LCTopoJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10TrackCaloClusterJets","xAOD::JetContainer"],
                        ["AntiKt10TrackCaloClusterJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoCSSKSoftDropBeta100Zcut10JetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoCSSKJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoCSSKJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10UFOCSSKJets","xAOD::JetContainer"],
                        ["AntiKt10UFOCSSKJetsAux","xAOD::JetAuxContainer"],
                        ["Kt4EMPFlowPUSBEventShape","xAOD::EventShape"],
                        ["Kt4EMPFlowPUSBEventShapeAux","xAOD::AuxInfoBase"],

                        ["TrackCaloClustersCombinedAndNeutral","xAOD::TrackCaloClusterContainer"],
                        ["TrackCaloClustersCombinedAndNeutralAux","xAOD::TrackCaloClusterAuxContainer"],
                        ["BTagging_AntiKt4LCTopo","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt4LCTopoAux", "xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt2Track","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt2TrackAux","xAOD::BTaggingAuxContainer"],
                        #["BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag","xAOD::BTaggingContainer"],
                        #["BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTagAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02Track_201810","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02Track_201810Aux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTagAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02Track_201903GhostTag","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02Track_201903GhostTagAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02Track_201903","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02Track_201903Aux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2Sub","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3Sub","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASub","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASubAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASub","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASubAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt4EMTopo_201810","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt4EMTopo_201810Aux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt4EMPFlow","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt4EMPFlowAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt4EMPFlow_201810","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt4EMPFlow_201810Aux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt4EMPFlow_201903","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt4EMPFlow_201903Aux","xAOD::BTaggingAuxContainer"],
                        ["DiTauJetsLowPt","xAOD::DiTauJetContainer"],
                        ["DiTauJetsLowPtAux","xAOD::DiTauJetAuxContainer"],
                        ["MuRmTauJets","xAOD::TauJetContainer"],
                        ["MuRmTauJetsAux","xAOD::TauJetAuxContainer"]
]
