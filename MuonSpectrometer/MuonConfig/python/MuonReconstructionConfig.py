# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Core configuration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Local
from MuonConfig.MuonSegmentFindingConfig import MuonSegmentFindingCfg
from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg
from xAODTrackingCnv.xAODTrackingCnvConfig import MuonStandaloneTrackParticleCnvAlgCfg


def StandaloneMuonOutputCfg(flags):
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    result = ComponentAccumulator()

    # FIXME! Fix for ATLASRECTS-5151. Remove when better solution found.
    Trk__EventCnvSuperTool = CompFactory.Trk.EventCnvSuperTool
    cnvTool = Trk__EventCnvSuperTool(name='EventCnvSuperTool')
    cnvTool.MuonCnvTool.FixTGCs = True
    result.addPublicTool(cnvTool)

    aod_items = []
    if flags.Detector.EnableMM or flags.Detector.EnablesTGC:
        aod_items += ["xAOD::TrackParticleContainer#EMEO_MuonSpectrometerTrackParticles"]
        aod_items += ["xAOD::TrackParticleAuxContainer#EMEO_MuonSpectrometerTrackParticlesAux."]
        aod_items += ["xAOD::MuonSegmentContainer#xAODNSWSegments"]
        aod_items += ["xAOD::MuonSegmentAuxContainer#xAODNSWSegmentsAux."]
    else:
      aod_items += [ "xAOD::MuonSegmentContainer#NCB_MuonSegments" ]
      aod_items += [ "xAOD::MuonSegmentAuxContainer#NCB_MuonSegmentsAux." ]

    # TrackParticles
    aod_items += ["xAOD::TrackParticleContainer#MuonSpectrometerTrackParticles"]
    aod_items += ["xAOD::TrackParticleAuxContainer#MuonSpectrometerTrackParticlesAux."]
    aod_items += ["xAOD::TrackParticleContainer#MuonSpectrometerOnlyTrackParticles"]
    aod_items += ["xAOD::TrackParticleAuxContainer#MuonSpectrometerOnlyTrackParticlesAux."]

    aod_items += ["xAOD::TrackParticleContainer#MSonlyTracklets"]
    aod_items += ["xAOD::TrackParticleAuxContainer#MSonlyTrackletsAux."]
    aod_items += ["xAOD::VertexContainer#MSDisplacedVertex"]
    aod_items += ["xAOD::VertexAuxContainer#MSDisplacedVertexAux."]

    if flags.Input.isMC:
        # Truth Particle Container
        aod_items += ["xAOD::TruthParticleContainer#MuonTruthParticles"]
        aod_items += ["xAOD::TruthParticleAuxContainer#MuonTruthParticlesAux."]

        # Truth Segment Container
        aod_items += ["xAOD::MuonSegmentContainer#MuonTruthSegments"]
        aod_items += ["xAOD::MuonSegmentAuxContainer#MuonTruthSegmentsAux."]

    if flags.Muon.prdToxAOD:
        aod_items += ["xAOD::TrackMeasurementValidationContainer#MDT_DriftCircles"]
        aod_items += ["xAOD::TrackMeasurementValidationAuxContainer#MDT_DriftCirclesAux."]
        aod_items += ["xAOD::TrackMeasurementValidationContainer#RPC_Measurements"]
        aod_items += ["xAOD::TrackMeasurementValidationAuxContainer#RPC_MeasurementsAux."]
        aod_items += ["xAOD::TrackMeasurementValidationContainer#TGC_MeasurementsAllBCs"]
        aod_items += ["xAOD::TrackMeasurementValidationAuxContainer#TGC_MeasurementsAllBCsAux."]
        if flags.Detector.EnableCSC:
            aod_items += ["xAOD::TrackMeasurementValidationContainer#CSC_Clusters"]
            aod_items += ["xAOD::TrackMeasurementValidationAuxContainer#CSC_ClustersAux."]

    if flags.Muon.rpcRawToxAOD:
        aod_items += ["xAOD::TrackMeasurementValidationContainer#RPC_RDO_Measurements"]
        aod_items += ["xAOD::TrackMeasurementValidationAuxContainer#RPC_RDO_MeasurementsAux."]

    # ESD list includes all AOD items
    esd_items = []
    esd_items += aod_items

    # PRDs
    if flags.Detector.EnableMM:
        esd_items += ["Muon::MMPrepDataContainer#MM_Measurements"]
    if flags.Detector.EnablesTGC:
        esd_items += ["Muon::sTgcPrepDataContainer#STGC_Measurements"]
    if flags.Detector.EnableCSC:
        esd_items += ["Muon::CscPrepDataContainer#CSC_Clusters"]
        esd_items += ["Muon::CscStripPrepDataContainer#CSC_Measurements"]
    esd_items += ["Muon::RpcPrepDataContainer#RPC_Measurements"]
    esd_items += ["Muon::TgcPrepDataContainer#TGC_MeasurementsAllBCs"]
    esd_items += ["Muon::MdtPrepDataContainer#MDT_DriftCircles"]

    # trigger related info for offline DQA
    esd_items += ["Muon::TgcCoinDataContainer#TrigT1CoinDataCollection"]
    esd_items += ["Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionPriorBC"]
    esd_items += ["Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionNextBC"]
    esd_items += ["Muon::RpcCoinDataContainer#RPC_triggerHits"]
    esd_items += ["RpcSectorLogicContainer#RPC_SECTORLOGIC"]

    # Segments
    esd_items += ["Trk::SegmentCollection#NCB_TrackMuonSegments"]

    # Tracks
    esd_items += ["TrackCollection#MuonSpectrometerTracks"]
    if flags.Muon.runCommissioningChain:
        esd_items += ["TrackCollection#EMEO_MuonSpectrometerTracks"]

    # Truth
    if flags.Input.isMC:
        esd_items += ["TrackRecordCollection#MuonEntryLayerFilter"]
        esd_items += ["TrackRecordCollection#MuonExitLayerFilter"]

        esd_items += ["PRD_MultiTruthCollection#MDT_TruthMap",
                      "PRD_MultiTruthCollection#RPC_TruthMap", "PRD_MultiTruthCollection#TGC_TruthMap"]
        if flags.Detector.EnableCSC:
            esd_items += ["PRD_MultiTruthCollection#CSC_TruthMap"]
        if flags.Detector.EnablesTGC:
            esd_items += ["PRD_MultiTruthCollection#STGC_TruthMap"]
        if flags.Detector.EnableMM:
            esd_items += ["PRD_MultiTruthCollection#MM_TruthMap"]

        # Track truth
        esd_items += ["DetailedTrackTruthCollection#MuonSpectrometerTracksTruth"]
        esd_items += ["TrackTruthCollection#MuonSpectrometerTracksTruth"]

        # Since this is only used for debugging, probably okay not to have a writeSDO flag..
        # Leaving this here for the moment in case this turns out to be a mistake.
        # if muonRecFlags.writeSDOs():
        #     if flags.Detector.EnableCSC: esd_items+=["CscSimDataCollection#CSC_SDO"]
        #     esd_items+=["MuonSimDataCollection#MDT_SDO"]
        #     esd_items+=["MuonSimDataCollection#RPC_SDO"]
        #     esd_items+=["MuonSimDataCollection#TGC_SDO"]
        #     if flags.Detector.EnablesTGC: esd_items+=["MuonSimDataCollection#sTGC_SDO"]
        #     if flags.Detector.EnableMM: esd_items+=["MuonSimDataCollection#MM_SDO"]

    if flags.Output.doWriteESD:
        result = OutputStreamCfg(flags, "ESD", esd_items)
    if flags.Output.doWriteAOD:
        result.merge(OutputStreamCfg(flags, "AOD", aod_items))
    return result


def MuonReconstructionCfg(flags):
    # https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py
    from MuonConfig.MuonPrepDataConvConfig import MuonPrepDataConvCfg
    from MuonConfig.MuonRecToolsConfig import MuonTrackScoringToolCfg
    from MuonConfig.MuonGeometryConfig import MuonIdHelperSvcCfg
    from MuonConfig.MuonRecToolsConfig import MuonEDMHelperSvcCfg
    from TrkConfig.TrkTrackSummaryToolConfig import MuonTrackSummaryToolCfg
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

    # Many components need these services, so setup once here.
    result = MuonIdHelperSvcCfg(flags)
    result.merge(MuonEDMHelperSvcCfg(flags))

    # Now setup reconstruction steps
    result.merge(MuonPrepDataConvCfg(flags))
    result.merge(MuonSegmentFindingCfg(flags))
    result.merge(MuonTrackBuildingCfg(flags))
    result.merge(MuonStandaloneTrackParticleCnvAlgCfg(flags))
    if flags.Muon.runCommissioningChain:
        result.merge(MuonStandaloneTrackParticleCnvAlgCfg(flags,
                                                          "MuonStandaloneTrackParticleCnvAlg_EMEO",
                                                          TrackContainerName="EMEO_MuonSpectrometerTracks",
                                                          xAODTrackParticlesFromTracksContainerName="EMEO_MuonSpectrometerTrackParticles"))

    # FIXME - this is copied from the old configuration, but I'm not sure it really belongs here. 
    # It's probably better to have as part of TrackBuilding, or Segment building...
    if flags.Input.isMC:
        track_cols = ["MuonSpectrometerTracks"]
        track_colstp = ["MuonSpectrometerTrackParticles"]
        if flags.Muon.runCommissioningChain:
            track_cols += ["EMEO_MuonSpectrometerTracks"]
            track_colstp += ["EMEO_MuonSpectrometerTrackParticles"]

        muondetailedtracktruthmaker = CompFactory.MuonDetailedTrackTruthMaker(name="MuonStandaloneDetailedTrackTruthMaker", TrackCollectionNames=track_cols,
                                                                              PRD_TruthNames=["RPC_TruthMap", "TGC_TruthMap", "MDT_TruthMap"] +
                                                                              (["CSC_TruthMap"] if flags.Detector.EnableCSC else []) +
                                                                              (["MM_TruthMap"]if flags.Detector.EnableMM else []) +
                                                                              (["STGC_TruthMap"] if flags.Detector.EnablesTGC else []))
        result.addEventAlgo(muondetailedtracktruthmaker)
        for i in range(len(track_cols)):
            result.addEventAlgo(CompFactory.TrackTruthSelector(name=track_cols[i] + "Selector",
                                                               DetailedTrackTruthName=track_cols[i] +
                                                               "DetailedTruth",
                                                               OutputName=track_cols[i] + "Truth"))
            result.addEventAlgo(CompFactory.TrackParticleTruthAlg(name=track_cols[i]+"TruthAlg",
                                                                  TrackTruthName=track_cols[i] +
                                                                  "Truth",
                                                                  TrackParticleName=track_colstp[i]))

        # Check if we're making PRDs, i.e. DetFlags.makeRIO.Muon_on(): in old config
        # FIXME - I think we can remove this flag if we shift this to where PRDs are being created. However, this will involve some refactoring, so temporary fix is this.
        if flags.Muon.makePRDs:
            result.addEventAlgo(CompFactory.MuonPRD_MultiTruthMaker())

            from MCTruthClassifier.MCTruthClassifierConfig import MCTruthClassifierCfg

            PRD_TruthMaps = ["RPC_TruthMap","TGC_TruthMap","MDT_TruthMap"]
            SDOs = ["RPC_SDO","TGC_SDO","MDT_SDO"]
            CSCSDOs= "CSC_SDO" 
            if (flags.Detector.EnablesTGC and flags.Detector.EnableMM):
                SDOs+=["MM_SDO","sTGC_SDO"]
                PRD_TruthMaps += ["MM_TruthMap", "STGC_TruthMap"]
            if not flags.Detector.EnableCSC: 
                CSCSDOs = ""
            else: PRD_TruthMaps += ["CSC_TruthMap"]
            result.addEventAlgo( CompFactory.Muon.MuonTruthDecorationAlg(name="MuonTruthDecorationAlg", 
                                                        MCTruthClassifier=result.popToolsAndMerge(MCTruthClassifierCfg(flags)), 
                                                        SDOs=SDOs, 
                                                        CSCSDOs=CSCSDOs,
                                                        PRD_TruthMaps = PRD_TruthMaps,
                                                        Extrapolator=result.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))
                                                        )

    if flags.Muon.doMSVertex:
        msvertexrecotool = CompFactory.Muon.MSVertexRecoTool(
            MyExtrapolator=result.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))
        the_alg = CompFactory.MSVertexRecoAlg(
            name="MSVertexRecoAlg", MSVertexRecoTool=msvertexrecotool)
        # Not explicitly configuring MSVertexTrackletTool
        result.addEventAlgo(the_alg)

    # FIXME - work around to fix unconfigured public MuonTrackScoringTool
    # It wuould be best to find who is using this tool, and add this configuration there
    # But AFAICS the only parent is MuonCombinedFitTagTool, and it's private there, so I'm a bit confused.
    result.addPublicTool(result.popToolsAndMerge(
        MuonTrackScoringToolCfg(flags)))
    # Ditto
    result.addPublicTool(result.popToolsAndMerge(
        MuonTrackSummaryToolCfg(flags)))

    # Setup output
    if flags.Output.doWriteESD or flags.Output.doWriteAOD:
        result.merge(StandaloneMuonOutputCfg(flags))
    return result


if __name__ == "__main__":
    # To run this, do e.g.
    # python -m MuonConfig.MuonReconstructionConfig --run --threads=1
    from MuonConfig.MuonConfigUtils import SetupMuonStandaloneArguments, SetupMuonStandaloneConfigFlags, SetupMuonStandaloneCA

    args = SetupMuonStandaloneArguments()
    ConfigFlags = SetupMuonStandaloneConfigFlags(args)
    cfg = SetupMuonStandaloneCA(args, ConfigFlags)

    # Run the actual test.
    acc = MuonReconstructionCfg(ConfigFlags)
    cfg.merge(acc)

    if args.threads > 1 and args.forceclone:
        from AthenaCommon.Logging import log
        log.info(
            'Forcing track building cardinality to be equal to '+str(args.threads))
        # We want to force the algorithms to run in parallel (eventually the algorithm will be marked as cloneable in the source code)
        AlgResourcePool = CompFactory.AlgResourcePool
        cfg.addService(AlgResourcePool(OverrideUnClonable=True))
        track_builder = acc.getPrimary()
        track_builder.Cardinality = args.threads

    from SGComps.AddressRemappingConfig import InputRenameCfg
    cfg.merge(InputRenameCfg("TrackCollection",
              "MuonSpectrometerTracks", "MuonSpectrometerTracks_old"))

    cfg.printConfig(withDetails=True)
    # drop faulty remapping
    # the evaluation of MuonSegmentNameFixCfg should happen conditionally instead
    # this is hack that is functioning only because this is top level CA
    oldRemaps = cfg.getService("AddressRemappingSvc").TypeKeyRenameMaps
    cfg.getService("AddressRemappingSvc").TypeKeyRenameMaps = [
        remap for remap in oldRemaps if "Trk::SegmentCollection" not in remap]

    f = open("MuonReconstruction.pkl", "wb")
    cfg.store(f)
    f.close()

    if args.run:
        sc = cfg.run(20)
        if not sc.isSuccess():
            import sys
            sys.exit("Execution failed")
