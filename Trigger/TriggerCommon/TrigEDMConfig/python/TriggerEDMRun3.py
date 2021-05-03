# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


# Definition of trigger EDM for the Run 3

# Concept of categories is kept similar to TriggerEDMRun2.py, categories are:
#   Bjet, Bphys, Egamma, ID/Tracking (to be concluded if can be merged),
#   Jet, L1, MET, MinBias, Muon, Steer, Tau, Calo

# Please note:
#   Dynamic varialbes/Container slimming: All dyn vars are removed unless explicitly specified to be kept!
#   Please refer to ATR-20275 for discussion about policy/handling of dynamic variables

from AthenaCommon.Logging import logging
__log = logging.getLogger('TriggerEDMRun3Config')

from TrigEDMConfig import DataScoutingInfo

def recordable( name ):
    """
    Verify that the name is in the list of recorded objects and conform to the name convention

    In Run 2 it was a delicate process to configure correctly what got recorded
    as it had to be set in the algorithm that produced it as well in the TriggerEDM.py in a consistent manner.

    For Run 3 every alg input/output key can be crosschecked against the list of objects to record which is defined here.
    I.e. in the configuration alg developer would do this:
    from TriggerEDM.TriggerEDMRun3 import recordable

    alg.outputKey = recordable("SomeKey")
    If the names are correct the outputKey is assigned with SomeKey, if there is a missmatch an exception is thrown.
    """

    if "HLTNav_" in name:
        __log.error( "Don't call recordable({0}), or add any \"HLTNav_\" collection manually to the EDM. See:collectDecisionObjects.".format( name ) )
        pass
    else: #negative filtering
        if not name.startswith( "HLT_" ):
            __log.error( "The collection name {0} does not start with HLT_".format( name ) )
        if "Aux" in name and not name[-1] != ".":
            __log.error( "The collection name {0} is Aux but the name does not end with the '.'".format( name ) )

    for entry in TriggerHLTListRun3:
        if entry[0].split( "#" )[1] == name:
            return name
    msg = "The collection name {0} is not declared to be stored by HLT. Add it to TriggerEDMRun3.py".format( name )
    __log.error("ERROR in recordable() - see following stack trace.")
    raise RuntimeError( msg )


# ------------------------------------------------------------
# Trigger EDM list for Run 3 with all containers that should
# be stored in the specified format as well as the category
# ------------------------------------------------------------
AllowedOutputFormats = ['BS', 'ESD', 'AODFULL', 'AODSLIM', 'AODVERYSLIM', 'AODBLSSLIM', 'AODLARGE', 'AODSMALL', ]
AllowedOutputFormats.extend(DataScoutingInfo.getAllDataScoutingIdentifiers())


JetVarsToKeep = ['ActiveArea', 'ActiveArea4vec_eta', 'ActiveArea4vec_m', 'ActiveArea4vec_phi', 'ActiveArea4vec_pt', 'AlgorithmType',
                 'DetectorEta', 'DetectorPhi', 'EMFrac', 'EnergyPerSampling', 'GhostTrack', 'HECFrac', 'InputType',
                 'JetConstitScaleMomentum_eta', 'JetConstitScaleMomentum_m', 'JetConstitScaleMomentum_phi', 'JetConstitScaleMomentum_pt',
                 'JetPileupScaleMomentum_eta', 'JetPileupScaleMomentum_m', 'JetPileupScaleMomentum_phi', 'JetPileupScaleMomentum_pt',
                 'JetEtaJESScaleMomentum_eta', 'JetEtaJESScaleMomentum_m', 'JetEtaJESScaleMomentum_phi', 'JetEtaJESScaleMomentum_pt',
                 'JetGSCScaleMomentum_eta', 'JetGSCScaleMomentum_m', 'JetGSCScaleMomentum_phi', 'JetGSCScaleMomentum_pt',
                 'Jvt', 'JVFCorr', 'JvtRpt', 'NumTrkPt500', 'NumTrkPt1000', 'SizeParameter', 'SumPtTrkPt500', 'SumPtTrkPt1000', 'TrackWidthPt1000', 'SumPtChargedPFOPt500',
]
JetVars = '.'.join(JetVarsToKeep)

JetCopyVarsToKeep = ['pt', 'eta', 'phi', 'm',
                     'JetPileupScaleMomentum_eta', 'JetPileupScaleMomentum_m', 'JetPileupScaleMomentum_phi', 'JetPileupScaleMomentum_pt',
                     'JetEtaJESScaleMomentum_eta', 'JetEtaJESScaleMomentum_m', 'JetEtaJESScaleMomentum_phi', 'JetEtaJESScaleMomentum_pt',
                     'JetGSCScaleMomentum_eta', 'JetGSCScaleMomentum_m', 'JetGSCScaleMomentum_phi', 'JetGSCScaleMomentum_pt',
                     'Jvt', 'JvtRpt'
                 ]
JetCopyVars = '.'.join(JetCopyVarsToKeep)

BTagOutput = ['jetLink','BTagTrackToJetAssociator','Muons',]
BTagOutput_IP2D = ['IP2D_TrackParticleLinks','IP2D_gradeOfTracks','IP2D_weightBofTracks','IP2D_weightCofTracks','IP2D_weightUofTracks','IP2D_sigD0wrtPVofTracks','IP2D_flagFromV0ofTracks','IP2D_nTrks','IP2D_isDefaults','IP2D_cu','IP2D_bu','IP2D_bc',]
BTagOutput_IP3D = ['IP3D_TrackParticleLinks','IP3D_gradeOfTracks','IP3D_weightBofTracks','IP3D_weightCofTracks','IP3D_weightUofTracks','IP3D_valD0wrtPVofTracks','IP3D_sigD0wrtPVofTracks','IP3D_valZ0wrtPVofTracks','IP3D_sigZ0wrtPVofTracks','IP3D_flagFromV0ofTracks','IP3D_nTrks','IP3D_isDefaults','IP3D_cu','IP3D_bu','IP3D_bc',]
BTagOutput_SV1 = ['SV1_TrackParticleLinks','SV1_vertices','SV1_isDefaults','SV1_NGTinSvx','SV1_masssvx','SV1_N2Tpair','SV1_efracsvx','SV1_deltaR','SV1_Lxy','SV1_L3d','SV1_significance3d','SV1_energyTrkInJet','SV1_dstToMatLay','SV1_badTracksIP','SV1_normdist',]
BTagOutput_JetFitter = [
    'JetFitter_deltaeta','JetFitter_deltaphi','JetFitter_fittedPosition','JetFitter_JFvertices','JetFitter_nVTX','JetFitter_nSingleTracks','JetFitter_isDefaults','JetFitter_deltaR',
    'JetFitterSecondaryVertex_isDefaults','JetFitterSecondaryVertex_nTracks','JetFitterSecondaryVertex_mass','JetFitterSecondaryVertex_energy','JetFitterSecondaryVertex_energyFraction','JetFitterSecondaryVertex_displacement3d','JetFitterSecondaryVertex_displacement2d',
    'JetFitterSecondaryVertex_minimumTrackRelativeEta',
    'JetFitterSecondaryVertex_maximumTrackRelativeEta',
    'JetFitterSecondaryVertex_averageTrackRelativeEta',
    'JetFitterSecondaryVertex_minimumAllJetTrackRelativeEta',
    'JetFitterSecondaryVertex_maximumAllJetTrackRelativeEta',
    'JetFitterSecondaryVertex_averageAllJetTrackRelativeEta',
    'JetFitter_mass','JetFitter_energyFraction','JetFitter_significance3d','JetFitter_nTracksAtVtx','JetFitter_N2Tpair','JetFitter_fittedCov','JetFitter_tracksAtPVchi2','JetFitter_tracksAtPVndf','JetFitter_tracksAtPVlinks','JetFitter_massUncorr','JetFitter_chi2','JetFitter_ndof','JetFitter_dRFlightDir',]
BTagOutput_rnnip = ['rnnip_isDefaults','rnnip_pu','rnnip_pc','rnnip_pb','rnnip_ptau',]
BTagOutput_highLevelTaggers = ['MV2c10_discriminant','DL1_pu','DL1_pc','DL1_pb','DL1r_pu','DL1r_pc','DL1r_pb',]

BTagOutput += BTagOutput_IP2D + BTagOutput_IP3D + BTagOutput_SV1 + BTagOutput_JetFitter + BTagOutput_rnnip + BTagOutput_highLevelTaggers
BTagVars = '.'.join(BTagOutput)


TriggerHLTListRun3 = [

    # framework/steering
    ('xAOD::TrigDecision#xTrigDecision' ,                    'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.',          'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigConfKeys#TrigConfKeys' ,                     'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#HLT_EBWeight',            'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#HLT_EBWeightAux.EBWeight.EBUnbiased', 'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigConfKeys#TrigConfKeysOnline' ,               'BS ESD AODFULL AODSLIM', 'Steer'),

    ('TrigRoiDescriptorCollection#HLT_EMRoIs',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_eEMRoIs',                  'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_MURoIs',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_METRoI',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_JETRoI',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_TAURoI',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_FSRoI',                    'BS ESD AODFULL AODSLIM',  'Steer'),

    ('xAOD::TrigCompositeContainer#HLT_TrigCostContainer',   'CostMonDS ESD', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#HLT_TrigCostContainerAux.alg.store.view.thread.thash.slot.roi.start.stop', 'CostMonDS ESD', 'Steer'),
    ('xAOD::TrigCompositeContainer#HLT_TrigCostROSContainer',   'CostMonDS ESD', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#HLT_TrigCostROSContainerAux.alg_idx.lvl1ID.robs_id.robs_size.robs_history.robs_status.start.stop', 'CostMonDS ESD', 'Steer'),

    # Run-2 L1 (temporary)
    ('xAOD::MuonRoIContainer#LVL1MuonRoIs' ,                 'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux.' ,          'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EmTauRoIContainer#LVL1EmTauRoIs' ,               'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EmTauRoIAuxContainer#LVL1EmTauRoIsAux.' ,        'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::JetRoIContainer#LVL1JetRoIs' ,                   'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::JetRoIAuxContainer#LVL1JetRoIsAux.' ,            'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::JetEtRoI#LVL1JetEtRoI' ,                         'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::JetEtRoIAuxInfo#LVL1JetEtRoIAux.' ,              'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EnergySumRoI#LVL1EnergySumRoI' ,                 'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EnergySumRoIAuxInfo#LVL1EnergySumRoIAux.',       'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),

    #Run-3 L1

    ("CaloCellContainer#SCell",                                'BS ESD', 'L1'),
    
    ('xAOD::EnergySumRoI#jXENOISECUTPerf' ,                 'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EnergySumRoIAuxInfo#jXENOISECUTPerfAux.',       'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),

    ('xAOD::EnergySumRoI#jXERHOPerf' ,                      'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EnergySumRoIAuxInfo#jXERHOPerfAux.' ,           'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),

    ('xAOD::EnergySumRoI#gXENOISECUTPerf' ,                  'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EnergySumRoIAuxInfo#gXENOISECUTPerfAux.' ,       'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),

    ('xAOD::EnergySumRoI#gXERHOPerf' ,                 'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EnergySumRoIAuxInfo#gXERHOPerfAux.',       'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    
    ('xAOD::EnergySumRoI#gXEJWOJPerf' ,                     'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EnergySumRoIAuxInfo#gXEJWOJPerfAux.',           'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),

    ("xAOD::TrigEMClusterContainer#eElesPerf" ,             'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ("xAOD::TrigEMClusterAuxContainer#eElesPerfAux." ,      'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
     
    ('xAOD::EmTauRoIContainer#eTausPerf',                   'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EmTauRoIAuxContainer#eTausPerfAux.',            'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),


    # Egamma
    ('xAOD::TrigEMClusterContainer#HLT_FastCaloEMClusters',           'BS ESD AODFULL', 'Egamma', 'inViews:EMCaloViews'), # last arg specifies in which view container the fragments are, look into the proprty of View maker alg for it
    ('xAOD::TrigEMClusterAuxContainer#HLT_FastCaloEMClustersAux.',    'BS ESD AODFULL', 'Egamma'),
    ('xAOD::TrigRingerRingsContainer#HLT_FastCaloRinger',             'BS ESD AODFULL', 'Egamma', 'inViews:EMCaloViews'), #Ringer
    ('xAOD::TrigRingerRingsAuxContainer#HLT_FastCaloRingerAux.',      'BS ESD AODFULL', 'Egamma'), #Ringer

    ('xAOD::TrigPhotonContainer#HLT_FastPhotons',                     'BS ESD AODFULL', 'Egamma', 'inViews:EMPhotonViews'),
    ('xAOD::TrigPhotonAuxContainer#HLT_FastPhotonsAux.',              'BS ESD AODFULL', 'Egamma'),
    ('xAOD::TrigElectronContainer#HLT_FastElectrons',                 'BS ESD AODFULL', 'Egamma', 'inViews:EMElectronViews'),
    ('xAOD::TrigElectronAuxContainer#HLT_FastElectronsAux.',          'BS ESD AODFULL', 'Egamma'),

    ('xAOD::TrigElectronContainer#HLT_FastElectrons_LRT',                 'BS ESD AODFULL', 'Egamma', 'inViews:EMElectronViews_LRT'),
    ('xAOD::TrigElectronAuxContainer#HLT_FastElectrons_LRTAux.',          'BS ESD AODFULL', 'Egamma'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_Electron_FTF',        'BS ESD AODFULL', 'Egamma', 'inViews:EMElectronViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Electron_FTFAux.', 'BS ESD AODFULL', 'Egamma'),
   
    ('xAOD::TrackParticleContainer#HLT_IDTrack_ElectronLRT_FTF',        'BS ESD AODFULL', 'Egamma', 'inViews:EMElectronViews_LRT'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_ElectronLRT_FTFAux.', 'BS ESD AODFULL', 'Egamma'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_Electron_IDTrig',        'BS ESD AODFULL', 'Egamma', 'inViews:precisionEtcutViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Electron_IDTrigAux.', 'BS ESD AODFULL', 'Egamma'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_ElectronLRT_IDTrig',        'BS ESD AODFULL', 'Egamma', 'inViews:precisionTrackViews_LRT'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_ElectronLRT_IDTrigAux.', 'BS ESD AODFULL', 'Egamma'),
   
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Electron_GSF',               'BS ESD AODFULL', 'Egamma', 'inViews:precisionElectronViews_GSF'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Electron_GSFAux.',           'BS ESD AODFULL', 'Egamma'),

    # these two corresponds to the output of the precisionCalo step
    ('xAOD::CaloClusterContainer#HLT_CaloEMClusters',               'BS ESD AODFULL', 'Egamma', 'inViews:precisionCaloViews'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_CaloEMClustersAux.',    'BS ESD AODFULL', 'Egamma'),

    ('xAOD::CaloClusterContainer#HLT_CaloEMClusters_LRT',               'BS ESD AODFULL', 'Egamma', 'inViews:precisionCaloViews_LRT'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_CaloEMClusters_LRTAux.',    'BS ESD AODFULL', 'Egamma'),

    # This variant needed by TrigUpgradeTest/egammaRinger.py
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClusters',             'BS ESD AODFULL', 'Egamma'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersAux.',  'BS ESD AODFULL', 'Egamma'),
    #
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersLCFS',                               'BS ESD AODFULL', 'Jet', 'alias:CaloClusterContainerShallowCopy'), # special argument indicating that this collection has a different Aux
    ('xAOD::ShallowAuxContainer#HLT_TopoCaloClustersLCFSAux.calE.calEta.calPhi',         'BS ESD AODFULL', 'Jet'),

    # Not sure we need these two...
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersRoI',          'BS ESD AODFULL', 'Egamma', 'inViews:precisionCaloViews'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersRoIAux.nCells', 'BS ESD AODFULL', 'Egamma'),

    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersRoI_LRT',          'BS ESD AODFULL', 'Egamma', 'inViews:precisionCaloViews_LRT'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersRoI_LRTAux.nCells', 'BS ESD AODFULL', 'Egamma'),

    # These are for precision photon and precision Electron Keeping same names as in Run2
    ('xAOD::ElectronContainer#HLT_egamma_Electrons',                'BS ESD AODFULL', 'Egamma', 'inViews:precisionElectronViews'),
    ('xAOD::ElectronAuxContainer#HLT_egamma_ElectronsAux.',     'BS ESD AODFULL', 'Egamma'),
    ('xAOD::ElectronContainer#HLT_egamma_Electrons_GSF',                'BS ESD AODFULL', 'Egamma', 'inViews:precisionElectronViews_GSF'),
    ('xAOD::ElectronAuxContainer#HLT_egamma_Electrons_GSFAux.',     'BS ESD AODFULL', 'Egamma'),
    ('xAOD::ElectronContainer#HLT_egamma_Electrons_LRT',                'BS ESD AODFULL', 'Egamma', 'inViews:precisionElectronViews_LRT'),
    ('xAOD::ElectronAuxContainer#HLT_egamma_Electrons_LRTAux.',     'BS ESD AODFULL', 'Egamma'),
    # enable when iso chains are present
    # ('xAOD::ElectronContainer#HLT_egamma_Iso_Electrons',            'BS ESD AODFULL', 'Egamma', 'inViews:precisionIsoElectronViews'),
    # ('xAOD::ElectronTrigAuxContainer#HLT_egamma_Iso_ElectronsAux.', 'BS ESD AODFULL', 'Egamma'),

    ('xAOD::PhotonContainer#HLT_egamma_Photons',                    'BS ESD AODFULL', 'Egamma', 'inViews:precisionPhotonViews'),
    ('xAOD::PhotonAuxContainer#HLT_egamma_PhotonsAux.',         'BS ESD AODFULL', 'Egamma'),
    # enable when iso chains are present
    # ('xAOD::PhotonContainer#HLT_egamma_Iso_Photons',                'BS ESD AODFULL', 'Egamma', 'inViews:precisionIsoPhotonViews'),
    # ('xAOD::PhotonTrigAuxContainer#HLT_egamma_Iso_PhotonsAux.',     'BS ESD AODFULL', 'Egamma'),

    ('TrigRoiDescriptorCollection#HLT_Roi_FastElectron',            'BS ESD AODFULL', 'Egamma'),
    ('TrigRoiDescriptorCollection#HLT_Roi_FastElectron_LRT',            'BS ESD AODFULL', 'Egamma'),
    ('TrigRoiDescriptorCollection#HLT_Roi_FastPhoton',              'BS ESD AODFULL', 'Egamma'),

    # Muon

    # Id track particles
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Muon_FTF',                 'BS ESD AODFULL', 'Muon', 'inViews:MUCombViewRoIs,MUCBFSViews,MUEFLATEViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Muon_FTFAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_Muon_IDTrig',                 'BS ESD AODFULL AODSLIM', 'Muon', 'inViews:MUEFCBViewRoIs,MUCBFSViews,MUEFLATEViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Muon_IDTrigAux.',          'BS ESD AODFULL AODSLIM', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonLRT_IDTrig',                 'BS ESD AODFULL AODSLIM', 'Muon', 'inViews:MUEFCBLRTViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonLRT_IDTrigAux.',          'BS ESD AODFULL AODSLIM', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonIso_FTF',                 'BS ESD AODFULL', 'Muon', 'inViews:MUEFIsoViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonIso_FTFAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonIso_IDTrig',              'BS ESD AODFULL', 'Muon', 'inViews:MUEFIsoViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonIso_IDTrigAux.',       'BS ESD AODFULL', 'Muon'),


    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonLRT_FTF',                 'BS ESD AODFULL', 'Muon', 'inViews:MUCombLRTViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonLRT_FTFAux.',          'BS ESD AODFULL', 'Muon'),


# These extra muon collections have all be removed now, as they all have identical 
# reconstruction as the standard Muon collections so unique collections names should
# not be needed. Once we are sure that there are no affected clients these commented
# collections can be removed 

#    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonFS_FTF',                 'BS ESD AODFULL', 'Muon', 'inViews:MUCBFSViews'),
#    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonFS_FTFAux.',          'BS ESD AODFULL', 'Muon'),

#    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonFS_IDTrig',              'BS ESD AODFULL', 'Muon', 'inViews:MUCBFSViews'),
#    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonFS_IDTrigAux.',       'BS ESD AODFULL', 'Muon'),

#    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonLate_FTF',                 'BS ESD AODFULL', 'Muon', 'inViews:MUEFLATEViewRoIs'),
#    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonLate_FTFAux.',          'BS ESD AODFULL', 'Muon'),

#    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonLate_IDTrig',              'BS ESD AODFULL', 'Muon', 'inViews:MUEFLATEViewRoIs'),
#    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonLate_IDTrigAux.',       'BS ESD AODFULL', 'Muon'),


    # Bphysics Dimuon chains
    ('xAOD::TrigBphysContainer#HLT_DimuEF',                                 'BS ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'Bphys'),
    ('xAOD::TrigBphysAuxContainer#HLT_DimuEFAux.',                          'BS ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'Bphys'),
    # Bphysics Bmumux chains
    ('xAOD::TrigBphysContainer#HLT_Bmumux',                                 'BS ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'Bphys'),
    ('xAOD::TrigBphysAuxContainer#HLT_BmumuxAux.',                          'BS ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'Bphys'),
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Bmumux_FTF',                 'BS ESD AODFULL', 'Bphys', 'inViews:BmumuxViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Bmumux_FTFAux.',          'BS ESD AODFULL', 'Bphys'),
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Bmumux_IDTrig',              'BS ESD AODFULL', 'Bphys', 'inViews:BmumuxViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Bmumux_IDTrigAux.',       'BS ESD AODFULL', 'Bphys'),
    ('TrigRoiDescriptorCollection#HLT_Roi_Bmumux',                          'BS ESD AODFULL', 'Bphys'),

    # xAOD muons (msonly (x2: roi+FS), combined (x2: FS+RoI)
    ('xAOD::MuonContainer#HLT_Muons_RoI',                                       'BS ESD AODFULL', 'Muon', 'inViews:MUEFSAViewRoIs'),
    ('xAOD::MuonAuxContainer#HLT_Muons_RoIAux.',                                'BS ESD AODFULL', 'Muon'),

    ('xAOD::MuonContainer#HLT_Muons_FS',                                        'BS ESD AODFULL', 'Muon', 'inViews:MUFSViewRoI'),
    ('xAOD::MuonAuxContainer#HLT_Muons_FSAux.',                                 'BS ESD AODFULL', 'Muon'),

    ('xAOD::MuonContainer#HLT_MuonsCB_RoI',                                     'BS ESD AODFULL', 'Muon', 'inViews:MUEFCBViewRoIs'),
    ('xAOD::MuonAuxContainer#HLT_MuonsCB_RoIAux.',                              'BS ESD AODFULL', 'Muon'),

    ('xAOD::MuonContainer#HLT_MuonsCB_LRT',                                     'BS ESD AODFULL', 'Muon', 'inViews:MUEFCBLRTViewRoIs'),
    ('xAOD::MuonAuxContainer#HLT_MuonsCB_LRTAux.',                              'BS ESD AODFULL', 'Muon'),

    ('xAOD::MuonContainer#HLT_MuonsCB_FS',                                      'BS ESD AODFULL', 'Muon', 'inViews:MUCBFSViews'),
    ('xAOD::MuonAuxContainer#HLT_MuonsCB_FSAux.',                               'BS ESD AODFULL', 'Muon'),

    ('TrigRoiDescriptorCollection#MuonCandidates_FS_ROIs',                      'BS ESD AODFULL', 'Muon'),

    # xAOD isolated muon
    ('xAOD::MuonContainer#HLT_MuonsIso',                                'BS ESD AODFULL', 'Muon', 'inViews:MUEFIsoViewRoIs'),
    ('xAOD::MuonAuxContainer#HLT_MuonsIsoAux.ptcone02.ptcone03',                         'BS ESD AODFULL', 'Muon'),

    # Muon track particle containers (combined (x2: FS+RoI), extrapolated (x2: FS+RoI), MSonly (x1: FS))
    ('xAOD::TrackParticleContainer#HLT_CBCombinedMuon_RoITrackParticles',                     'BS ESD AODFULL', 'Muon', 'inViews:MUEFCBViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_CBCombinedMuon_RoITrackParticlesAux.',              'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_CBCombinedMuon_FSTrackParticles',                      'BS ESD AODFULL', 'Muon', 'inViews:MUCBFSViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_CBCombinedMuon_FSTrackParticlesAux.',               'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_MSExtrapolatedMuons_RoITrackParticles',                'BS ESD AODFULL', 'Muon', 'inViews:MUEFSAViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_MSExtrapolatedMuons_RoITrackParticlesAux.',         'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_MSExtrapolatedMuons_FSTrackParticles',                 'BS ESD AODFULL', 'Muon', 'inViews:MUFSViewRoI'),
    ('xAOD::TrackParticleAuxContainer#HLT_MSExtrapolatedMuons_FSTrackParticlesAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_MSOnlyExtrapolatedMuons_FSTrackParticles',             'BS ESD AODFULL', 'Muon', 'inViews:MUFSViewRoI'),
    ('xAOD::TrackParticleAuxContainer#HLT_MSOnlyExtrapolatedMuons_FSTrackParticlesAux.',      'BS ESD AODFULL', 'Muon'),

    #xAOD L2 muons (SA, CB, isolation)
    ('xAOD::L2StandAloneMuonContainer#HLT_MuonL2SAInfo',        'BS ESD AODFULL', 'Muon', 'inViews:MUViewRoIs'),
    ('xAOD::L2StandAloneMuonAuxContainer#HLT_MuonL2SAInfoAux.', 'BS ESD AODFULL', 'Muon'),

    ('xAOD::L2StandAloneMuonContainer#HLT_MuonL2SAInfoIOmode',        'BS ESD AODFULL', 'Muon', 'inViews:MUCombViewRoIs'),
    ('xAOD::L2StandAloneMuonAuxContainer#HLT_MuonL2SAInfoIOmodeAux.', 'BS ESD AODFULL', 'Muon'),
    
    ('xAOD::L2StandAloneMuonContainer#HLT_MuonL2SAInfol2mtmode',        'BS ESD AODFULL', 'Muon', 'inViews:MUViewRoIs'),
    ('xAOD::L2StandAloneMuonAuxContainer#HLT_MuonL2SAInfol2mtmodeAux.', 'BS ESD AODFULL', 'Muon'),

    ('xAOD::L2CombinedMuonContainer#HLT_MuonL2CBInfo',          'BS ESD AODFULL', 'Muon', 'inViews:MUCombViewRoIs'),
    ('xAOD::L2CombinedMuonAuxContainer#HLT_MuonL2CBInfoAux.',   'BS ESD AODFULL', 'Muon'),

    ('xAOD::L2CombinedMuonContainer#HLT_MuonL2CBInfoLRT',          'BS ESD AODFULL', 'Muon', 'inViews:MUCombLRTViewRoIs'),
    ('xAOD::L2CombinedMuonAuxContainer#HLT_MuonL2CBInfoLRTAux.',   'BS ESD AODFULL', 'Muon'),

    ('xAOD::L2CombinedMuonContainer#HLT_MuonL2CBInfoIOmode',          'BS ESD AODFULL', 'Muon', 'inViews:MUCombViewRoIs'),
    ('xAOD::L2CombinedMuonAuxContainer#HLT_MuonL2CBInfoIOmodeAux.',   'BS ESD AODFULL', 'Muon'),

    ('xAOD::L2CombinedMuonContainer#HLT_MuonL2CBInfol2mtmode',          'BS ESD AODFULL', 'Muon', 'inViews:MUCombViewRoIs'),
    ('xAOD::L2CombinedMuonAuxContainer#HLT_MuonL2CBInfol2mtmodeAux.',   'BS ESD AODFULL', 'Muon'),
    
    ('xAOD::L2IsoMuonContainer#HLT_MuonL2ISInfo',               'BS ESD AODFULL', 'Muon', 'inViews:MUIsoViewRoIs'),
    ('xAOD::L2IsoMuonAuxContainer#HLT_MuonL2ISInfoAux.',        'BS ESD AODFULL', 'Muon'),

    ('TrigRoiDescriptorCollection#HLT_Roi_L2SAMuon',                   'BS ESD AODFULL', 'Muon'),
    ('TrigRoiDescriptorCollection#HLT_Roi_L2SAMuon_LRT',                   'BS ESD AODFULL', 'Muon'),
    ('TrigRoiDescriptorCollection#HLT_Roi_L2SAMuonForEF',              'BS ESD AODFULL', 'Muon'),
    ('TrigRoiDescriptorCollection#HLT_Roi_MuonIso',                    'BS ESD AODFULL', 'Muon'),

    # Tau

    ('xAOD::TrackParticleContainer#HLT_IDTrack_TauCore_FTF',                 'BS ESD AODFULL', 'Tau', 'inViews:TAUFTFCoreViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_TauCore_FTFAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_TauIso_FTF',                 'BS ESD AODFULL', 'Tau', 'inViews:TAUFTFIsoViews,TAUEFViews,TAUFTFIsoBDTViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_TauIso_FTFAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_Tau_FTF',                 'BS ESD AODFULL', 'Tau', 'inViews:TAUFTFIdViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Tau_FTFAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_Tau_IDTrig',                 'BS ESD AODFULL', 'Tau', 'inViews:TAUFTFIdViews,TAUPrecIsoViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Tau_IDTrigAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::VertexContainer#HLT_IDVertex_Tau',                  'BS ESD AODFULL', 'Tau', 'inViews:TAUFTFIdViews,TAUPrecIsoViews'),
    ('xAOD::VertexAuxContainer#HLT_IDVertex_TauAux.',           'BS ESD AODFULL', 'Tau'),

    ('TrigRoiDescriptorCollection#HLT_Roi_Tau',              'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_Roi_TauCore',             'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_Roi_TauIso',             'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_Roi_TauIsoBDT',             'BS ESD AODFULL AODSLIM',  'Steer'),

    ('xAOD::JetContainer#HLT_jet_seed',                         'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUCaloViews,TAUCaloMVAViews'),
    ('xAOD::JetAuxContainer#HLT_jet_seedAux.',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    # Jet
    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjesIS',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMTopoJets_subjesISAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_nojcalib_ftf',                  'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_nojcalib_ftfAux.'+JetVars,   'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjesIS_ftf',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMTopoJets_subjesIS_ftfAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjesgscIS_ftf',                        'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMTopoJets_subjesgscIS_ftfAux.'+JetCopyVars, 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjesgsc_ftf',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMTopoJets_subjesgsc_ftfAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subresjesgscIS_ftf',                        'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMTopoJets_subresjesgscIS_ftfAux.'+JetCopyVars, 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subresjesgsc_ftf',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMTopoJets_subresjesgsc_ftfAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjes',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMTopoJets_subjesAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_nojcalib',                      'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_nojcalibAux.'+JetVars,       'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoSKJets_nojcalib',                    'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoSKJets_nojcalibAux.'+JetVars,     'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoCSSKJets_nojcalib',                  'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoCSSKJets_nojcalibAux.'+JetVars,   'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoJets_subjes',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt10LCTopoJets_subjesAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoJets_nojcalib',                     'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10LCTopoJets_nojcalibAux.'+JetVars,      'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10EMTopoRCJets_subjesIS',                      'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10EMTopoRCJets_subjesISAux.'+JetVars,       'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes',                'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jesAux.'+JetVars, 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_nojcalib',                'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_nojcalibAux.'+JetVars, 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoSoftDropBeta100Zcut10Jets_nojcalib',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10LCTopoSoftDropBeta100Zcut10Jets_nojcalibAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10EMPFlowJets_nojcalib_ftf',                       'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10EMPFlowJets_nojcalib_ftfAux.'+JetVars,        'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10EMPFlowSoftDropBeta100Zcut10Jets_nojcalib_ftf',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10EMPFlowSoftDropBeta100Zcut10Jets_nojcalib_ftfAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10EMPFlowCSSKJets_nojcalib_ftf',                       'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10EMPFlowCSSKJets_nojcalib_ftfAux.'+JetVars,        'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_nojcalib_ftf',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_nojcalib_ftfAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_jes_ftf',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_jes_ftfAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowJets_subjesIS_ftf',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMPFlowJets_subjesIS_ftfAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowJets_subjesgscIS_ftf',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMPFlowJets_subjesgscIS_ftfAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowJets_subjesgsc_ftf',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMPFlowJets_subjesgsc_ftfAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftfAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowJets_subresjesgsc_ftf',                        'BS ESD AODFULL', 'Jet', 'alias:JetContainerShallowCopy'),
    ('xAOD::ShallowAuxContainer#HLT_AntiKt4EMPFlowJets_subresjesgsc_ftfAux.'+JetCopyVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowJets_nojcalib_ftf',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMPFlowJets_nojcalib_ftfAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowCSSKJets_nojcalib_ftf',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMPFlowCSSKJets_nojcalib_ftfAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    # TLA jets

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjesIS_TLA',                      'BS JetDS ESD', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_subjesIS_TLAAux.'+JetVars,       'BS JetDS ESD', 'Jet'),

    #TLA Photons
    ('xAOD::PhotonContainer#HLT_egamma_Photons_TLA',                                    'BS PhotonDS ESD', 'Egamma', 'inViews:TLAPhotonsView'),
    ('xAOD::PhotonAuxContainer#HLT_egamma_TLAAux',                                      'BS PhotonDS ESD', 'Egamma'),


    # FS tracks
    ('xAOD::TrackParticleContainer#HLT_IDTrack_FS_FTF',                 'BS ESD AODFULL', 'Jet'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_FS_FTFAux.passPFTrackPresel.muonCaloTag.muonScore.ptCone20.etConeCore.trackIso.RErr.EOverP.caloIso.trkPtFraction.tagFakeTrack.tagMuonTrack.tagIsoTrack',          'BS ESD AODFULL', 'Jet'),

    # FS vertices
    ('xAOD::VertexContainer#HLT_IDVertex_FS',                  'BS ESD AODFULL', 'Jet'),
    ('xAOD::VertexAuxContainer#HLT_IDVertex_FSAux.',          'BS ESD AODFULL', 'Jet'),

    ('xAOD::VertexContainer#HLT_IDVertex_FSJet',                  'BS ESD AODFULL', 'Jet'),
    ('xAOD::VertexAuxContainer#HLT_IDVertex_FSJetAux.',           'BS ESD AODFULL', 'Jet'),

    #FSLRT
    ('xAOD::TrackParticleContainer#HLT_IDTrack_FSLRT_FTF',                  'BS ESD AODFULL', 'Jet'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_FSLRT_FTFAux.',          'BS ESD AODFULL', 'Jet'),

    ('xAOD::VertexContainer#HLT_IDVertex_FSLRT',                  'BS ESD AODFULL', 'Jet'),
    ('xAOD::VertexAuxContainer#HLT_IDVertex_FSLRTAux.',          'BS ESD AODFULL', 'Jet'),

    ('xAOD::TrigCompositeContainer#HLT_FSLRT_TrackCount',                                             'BS ESD AODFULL AODSLIM', 'Jet'),
    ('xAOD::TrigCompositeAuxContainer#HLT_FSLRT_TrackCountAux.ntrks.pTcuts.z0cuts.counts',            'BS ESD AODFULL AODSLIM', 'Jet'),

    # custom BeamSpot tracks - we don't want to write these out in general so this
    # is commented, if we want to write them out at some point, then these lines
    # should be uncommented and they should get written out
    #    ('xAOD::TrackParticleContainer#HLT_IDTrack_BeamSpot_FTF',         'BS ESD AODFULL', 'ID', 'inViews:beamspotViewRoIs' ),
    #    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_BeamSpot_FTFAux.',  'BS ESD AODFULL', 'ID', 'inViews:beamspotViewRoIs' ),

    # MET
    ('xAOD::TrigMissingETContainer#HLT_MET_cell',                               'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_cellAux.',                        'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_mht',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_mhtAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_tcpufit',                       'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_tcpufitAux.',                'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_tc',                            'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_tcAux.',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_tc_em',                            'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_tc_emAux.',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_trkmht',                        'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_trkmhtAux.',                 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_pfsum',                         'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_pfsumAux.',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_pfsum_cssk',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_pfsum_csskAux.',             'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_pfsum_vssk',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_pfsum_vsskAux.',             'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_pfopufit',                      'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_pfopufitAux.',               'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_cvfpufit',                      'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_cvfpufitAux.',               'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_mhtpufit_pf_subjesgscIS',       'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_mhtpufit_pf_subjesgscISAux.', 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_mhtpufit_em_subjesgscIS',       'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_mhtpufit_em_subjesgscISAux.', 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersFS',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersFSAux.nCells', 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    # tau
    # will enable when needed
    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_CaloOnly',                         'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUCaloViews,TAUCaloMVAViews'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_CaloOnlyAux.',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    #('xAOD::TauJetContainer#HLT_TrigTauRecMerged_CaloOnlyMVA',                         'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUCaloMVAViews'),
    #('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_CaloOnlyMVAAux.',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_MVA',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUMVAViews'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_MVAAux.',              'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_Precision',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUFTFIdViews,TAUFTFTrackViews,TAUTrackTwoViews,TAUEFViews'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_PrecisionAux.',              'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_Presel',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUFTFTrackViews'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_PreselAux.',              'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    # tau calo clusters
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersLC',                  'BS ESD AODFULL', 'Tau', 'inViews:TAUCaloViews'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersLCAux.nCells' ,'BS ESD AODFULL', 'Tau'),

    # tau tracks
    ('xAOD::TauTrackContainer#HLT_tautrack_MVA',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUMVAViews'),
    ('xAOD::TauTrackAuxContainer#HLT_tautrack_MVAAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),
    ('xAOD::TauTrackContainer#HLT_tautrack_Precision',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUFTFIdViews,TAUFTFTrackViews,TAUEFViews'),
    ('xAOD::TauTrackAuxContainer#HLT_tautrack_PrecisionAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),
    ('xAOD::TauTrackContainer#HLT_tautrack_Presel',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUFTFTrackViews'),
    ('xAOD::TauTrackAuxContainer#HLT_tautrack_PreselAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    # bjet RoI Descriptor used for EventView creation
    ('TrigRoiDescriptorCollection#HLT_Roi_Bjet',                   'BS ESD AODFULL', 'Bjet'),

    # bjet Second Stage Fast tracks
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Bjet_FTF',        'BS ESD AODFULL', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Bjet_FTFAux.', 'BS ESD AODFULL', 'Bjet'),



    # bjet Second Stage Precision tracks
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Bjet_IDTrig',        'BS ESD AODFULL', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Bjet_IDTrigAux.btagIp_d0.btagIp_d0Uncertainty.btagIp_trackDisplacement.btagIp_trackMomentum.btagIp_z0SinTheta.btagIp_z0SinThetaUncertainty', 'BS ESD AODFULL', 'Bjet'),

    # FIXME: add vertex tracks

    # bjet jets
    ('xAOD::JetContainer#HLT_bJets',                             'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::JetAuxContainer#HLT_bJetsAux.btaggingLink',          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),


    # secvertex for b-jets
    ('xAOD::VertexContainer#HLT_BTaggingSecVtx',                          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::VertexAuxContainer#HLT_BTaggingSecVtxAux.',                   'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),

    # btagvertex for b-jets
    ('xAOD::BTagVertexContainer#HLT_BTaggingJFVtx',                          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::BTagVertexAuxContainer#HLT_BTaggingJFVtxAux.',                   'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),

    # bjet b-tagging
    ('xAOD::BTaggingContainer#HLT_BTagging',                          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::BTaggingAuxContainer#HLT_BTaggingAux.'+BTagVars,          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),

    # MinBias

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MinBias_IDTrig',                 'BS ESD AODFULL', 'MinBias', 'inViews:TrkView'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MinBias_IDTrigAux.',          'BS ESD AODFULL', 'MinBias'),

    ('xAOD::TrigT2MbtsBitsContainer#HLT_MbtsBitsContainer',                 'BS ESD AODFULL', 'MinBias'),
    ('xAOD::TrigT2MbtsBitsAuxContainer#HLT_MbtsBitsContainerAux.',          'BS ESD AODFULL', 'MinBias'),


    # Cosmic
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Cosmic_FTF',                 'BS ESD AODFULL', 'Cosmic'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Cosmic_FTFAux.',             'BS ESD AODFULL', 'Cosmic'),

    #FullScan version from offline patern recognition (these will potentially be removed,, as the ones from the precision tracking might be sufficient)
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Cosmic_EFID',                 'BS ESD AODFULL', 'Cosmic'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Cosmic_EFIDAux.',          'BS ESD AODFULL', 'Cosmic'),
    #And their respective precision tracks
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Cosmic_IDTrig',               'BS ESD AODFULL', 'Cosmic'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Cosmic_IDTrigAux.',        'BS ESD AODFULL', 'Cosmic'),


    ('ROIB::RoIBResult#*',                         'ESD', 'Misc'),

    ('xAOD::TrigCompositeContainer#HLT_SpacePointCounts',            'BS ESD AODFULL AODSLIM', 'MinBias', 'inViews:SPView'),
    ('xAOD::TrigCompositeAuxContainer#HLT_SpacePointCountsAux.pixCL.pixCL_1.pixCL_2.pixCLmin3.pixCLBarrel.pixCLEndcapA.pixCLEndcapC.sctSP.sctSPBarrel.sctSPEndcapA.sctSPEndcapC',     'BS ESD AODFULL AODSLIM', 'MinBias'),

    ('xAOD::TrigCompositeContainer#HLT_TrackCount',                                             'BS ESD AODFULL AODSLIM', 'MinBias'),
    ('xAOD::TrigCompositeAuxContainer#HLT_TrackCountAux.ntrks.pTcuts.z0cuts.counts',            'BS ESD AODFULL AODSLIM', 'MinBias'),

    # UTT
    ('xAOD::TrigCompositeContainer#HLT_HitDVSeed',           'BS ESD AODFULL', 'ID'),
    ('xAOD::TrigCompositeAuxContainer#HLT_HitDVSeedAux.seed_eta.seed_phi.seed_et_small.seed_et_large',  'BS ESD AODFULL', 'ID'),
    ('xAOD::TrigCompositeContainer#HLT_HitDVTrk',            'BS ESD AODFULL', 'ID'),
    ('xAOD::TrigCompositeAuxContainer#HLT_HitDVTrkAux.trk_id.trk_pt.trk_eta.trk_phi.trk_n_hits_innermost.trk_n_hits_inner.trk_n_hits_pix.trk_n_hits_sct.trk_a0beam',     'BS ESD AODFULL', 'ID'),
    ('xAOD::TrigCompositeContainer#HLT_HitDVSP',             'BS ESD AODFULL', 'ID'),
    ('xAOD::TrigCompositeAuxContainer#HLT_HitDVSPAux.sp_eta.sp_r.sp_phi.sp_z.sp_layer.sp_isPix.sp_isSct.sp_usedTrkId',      'BS ESD AODFULL', 'ID'),
    ('xAOD::TrigCompositeContainer#HLT_dEdxTrk',             'BS ESD AODFULL', 'ID'),
    ('xAOD::TrigCompositeAuxContainer#HLT_dEdxTrkAux.trk_id.trk_pt.trk_eta.trk_dedx.trk_dedx_n_usedhits.trk_n_hits_innermost.trk_n_hits_inner.trk_n_hits_pix.trk_n_hits_sct.trk_a0beam',      'BS ESD AODFULL', 'ID'),
    ('xAOD::TrigCompositeContainer#HLT_dEdxHit',             'BS ESD AODFULL', 'ID'),
    ('xAOD::TrigCompositeAuxContainer#HLT_dEdxHitAux.hit_trkid.hit_dedx.hit_tot.hit_trkchi2.hit_trkndof.hit_iblovf.hit_loc.hit_layer',      'BS ESD AODFULL', 'ID'),
    ('xAOD::TrigCompositeContainer#HLT_HPtdEdx',             'BS ESD AODFULL AODSLIM', 'ID'),
    ('xAOD::TrigCompositeAuxContainer#HLT_dEdxTrkAux.trk_pt.trk_eta.trk_a0beam.trk_dedx.trk_n_hdedx_hits.trk_n_hits_innermost.trk_n_hits_inner.trk_n_hits_pix.trk_n_hits_sct',      'BS ESD AODFULL AODSLIM', 'ID'),
]

# HLTNav_* object list is built dynamically during job configuration, here we only define its output targets
HLTNavEDMTargets = 'BS ESD AODFULL AODSLIM'

#-------------------------------------------------------------------------------
# EDM details list to store the transient-persistent version
#-------------------------------------------------------------------------------
EDMDetailsRun3 = {}

# xAOD versions are auto-detected at serialisation, but T/P classes need to specify P version here
EDMDetailsRun3[ "TrigRoiDescriptorCollection" ]     = {'persistent':"TrigRoiDescriptorCollection_p3"}

EDMDetailsRun3[ "xAOD::TrigDecisionAuxInfo" ]         = {'parent':"xAOD::TrigDecision"}
EDMDetailsRun3[ "xAOD::EnergySumRoIAuxInfo" ]         = {'parent':"xAOD::EnergySumRoI"}
EDMDetailsRun3[ "xAOD::JetEtRoIAuxInfo" ]             = {'parent':"xAOD::JetEtRoI"}
EDMDetailsRun3[ "xAOD::CaloClusterTrigAuxContainer" ] = {'parent':"xAOD::CaloClusterContainer"}


#-------------------------------------------------------------------------------
# Helper functions
#-------------------------------------------------------------------------------
def persistent( transient ):
    """
    Persistent EDM class, for xAOD it is the actual class version

    Uses list defined above. If absent assumes v1
    """
    if transient in EDMDetailsRun3:
        if 'persistent' in EDMDetailsRun3[transient]:
            return EDMDetailsRun3[transient]['persistent']
    return transient


def tpMap():
    """
    List
    """
    l = {}
    for tr in EDMDetailsRun3.keys():
        if "xAOD" in tr:
            continue
        l[tr] = persistent(tr)
    return l


def addHLTNavigationToEDMList(edmList, allDecisions, hypoDecisions):
    """
    Extend TriggerHLTListRun3 with HLT Navigation objects
    """
    for decisionCollection in allDecisions:
        dynamic = '.-' # Exclude dynamic
        if decisionCollection in hypoDecisions:
            # Include dynamic
            dynamic = '.remap_linkColIndices.remap_linkColKeys'
            if 'PEBInfoWriter' in decisionCollection:
                dynamic += '.PEBROBList.PEBSubDetList'
        typeName = 'xAOD::TrigCompositeContainer#{:s}'.format(decisionCollection)
        typeNameAux = 'xAOD::TrigCompositeAuxContainer#{:s}Aux{:s}'.format(decisionCollection, dynamic)

        # Cost monitoring only requires a sub-set of the navigation collections.
        # (And CANNOT use any slimmed/merged collection, as the container names are important)
        thisCollectionHLTNavEDMTargets = HLTNavEDMTargets
        if decisionCollection.startswith("HLTNav_FStep") or decisionCollection == "HLTNav_Summary" or decisionCollection.startswith("HLTNav_L1"):
            thisCollectionHLTNavEDMTargets += ' CostMonDS'

        edmList.extend([
            (typeName,    thisCollectionHLTNavEDMTargets, 'Steer'),
            (typeNameAux, thisCollectionHLTNavEDMTargets, 'Steer')])

def addExtraCollectionsToEDMList(edmList, extraList):
    """
    Extend edmList with extraList, keeping track whether a completely new
    collection is being added, or a dynamic variable is added to an existing collection.
    The format of extraList is the same as those of TriggerHLTListRun3.
    """
    existing_collections = [(c[0].split("#")[1]).split(".")[0] for c in edmList]
    for item in extraList:
        colname = (item[0].split("#")[1]).split(".")[0]
        if colname not in existing_collections:
            # a new collection is added
            edmList.append(item)
            __log.info("added new item to Trigger EDM: {}".format(item))
        else:
            if "Aux." in item[0]:
                # probably an extra dynamic variable is added
                # new variables to add:
                dynVars = (item[0].split("#")[1]).split(".")[1:]
                # find the index of the existing item
                existing_item_nr = [i for i,s in enumerate(edmList) if colname == (s[0].split("#")[1]).split(".")[0]]
                if len(existing_item_nr) != 1:
                    __log.error("Found {} existing edm items corresponding to new item {}, but it must be exactly one!".format(len(existing_item_nr), item))
                # merge lists of variables
                existing_dynVars = (edmList[existing_item_nr[0]][0].split("#")[1]).split(".")[1:]
                dynVars.extend(existing_dynVars)
                # remove duplicates:
                dynVars = list(dict.fromkeys(dynVars))
                newVars = '.'.join(dynVars)
                typename = item[0].split("#")[0]
                __log.info("old item in Trigger EDM: {}".format(edmList[existing_item_nr[0]]))
                targets = edmList[existing_item_nr[0]][1]
                signature = edmList[existing_item_nr[0]][2]
                edmList.pop(existing_item_nr[0])
                edmList.insert(existing_item_nr[0] , (typename + "#" + colname + "." + newVars, targets, signature))
                __log.info("updated item in Trigger EDM: {}".format(edmList[existing_item_nr[0]]))
            else:
                # asking to add some collection which is already in the list - do nothing
                pass
