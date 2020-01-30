# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT6SmartContent = [
    "Electrons",
    "Muons",
    "Photons",
    "InDetTrackParticles",
    "PrimaryVertices",
    "MET_Reference_AntiKt4EMTopo",
    "MET_Reference_AntiKt4EMPFlow",
    "AntiKt4EMPFlowJets",
    "AntiKt4EMPFlowJets_BTagging201810",
    "AntiKt4EMPFlowJets_BTagging201903",
    "AntiKt4EMTopoJets",
    "AntiKt4EMTopoJets_BTagging201810",
    "AntiKt4TruthJets",
    "TauJets",
    "BTagging_AntiKt4EMTopo_201810",
    "BTagging_AntiKt4EMPFlow_201810",
    "BTagging_AntiKt4EMPFlow_201903"
]

EXOT6AllVariablesContent = [
    "TruthVertices",
    "MuonTruthParticles",
    "TruthParticles",
    "MET_Truth",
    "MET_LocHadTopo"
]

EXOT6ExtraVariables = [ 
  "Electrons.author.Medium.Tight.Loose.charge.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z.isEMLoose.zvertex.errz.etap.depth",
    "Photons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z.isEMLoose.zvertex.errz.etap.depth",
    "egammaClusters.time",
    "TruthEvents.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.Q.XF1.XF2",
    "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",  
    "TauNeutralParticleFlowObjects.pt.eta.phi.m",
    "TauChargedParticleFlowObjects.pt.eta.phi.m",
    "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.GhostTruth.GhostTruthAssociationLink.GhostPartons.GhostPartonsPt.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.GhostTruthCount.NumTrkPt500.PartonTruthLabelID.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_eta",
  "HLT_xAOD__JetContainer_a4tcemsubjesISFS.pt.eta.phi.m", 
  "HLT_xAOD__PhotonContainer_egamma_Photons.pt.eta.phi",

"AntiKt4EMPFlowJets.GhostTruth.GhostTruthAssociationLink.GhostPartons.GhostPartonsPt.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.GhostTruthCount.NumTrkPt500.PartonTruthLabelID.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_eta"
]

EXOT6UnslimmedContent = [
    'xAOD::TruthParticleContainer#TruthTaus',
    'xAOD::TruthParticleAuxContainer#TruthTausAux.'
]
