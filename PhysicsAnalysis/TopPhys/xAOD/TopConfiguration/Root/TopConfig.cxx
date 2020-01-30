/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/AodMetaDataAccess.h"
#include "TopConfiguration/ConfigurationSettings.h"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "TopConfiguration/Tokenize.h"

namespace top {
  TopConfig::TopConfig() :
    // Prevent any more configuration
    m_configFixed(false),
    m_makeAllCPTools(true),

    // Use which objects
    m_usePhotons(false),
    m_useElectrons(false),
    m_useFwdElectrons(false),
    m_useMuons(false),
    m_useSoftMuons(false),
    m_useAntiMuons(false),
    m_useTaus(false),
    m_useJets(false),
    m_useLargeRJets(false),
    m_useTrackJets(false),
    m_useRCJets(false),
    m_useVarRCJets(false),
    m_useJetGhostTrack(false),

    m_useTruthParticles(false),
    m_useTruthElectrons(false),
    m_useTruthMuons(false),
    m_useTruthPhotons(false),
    m_useTruthJets(false),
    m_useTruthLargeRJets(false),
    m_useTruthMET(false),

    m_applyTTVACut(true),

    m_demandPriVtx(true),

    m_jetSubstructureName("None"),

    m_recomputeCPvars(true),

    // Do systematics? - this needs many more configuration options
    m_systematics("SetMe"),
    /// special syst config
    m_nominalSystName("Nominal"),
    m_allSystName("All"),

    m_DSID(-1),
    m_MapIndex(0),
    // Is MC
    m_isMC(false),
    // Is AFII
    m_isAFII(false),
    // Generators
    m_generators("SetMe"),
    // AMITag
    m_AMITag("SetMe"),
    // Is Primary xAOD
    m_isPrimaryxAOD(false),
    // Is Truth xAOD
    m_isTruthDxAOD(false),
    // Derivation name
    m_derivationStream("SetMe"),
    // Do fakes MM weights calculation using FakeBkgTools from IFF
    m_doFakesMMWeightsIFF(false),
    // Configurations for MM fake estimate using FakeBkgTools from IFF
    m_FakesMMConfigIFF("$ROOTCOREBIN/data/TopFakes/efficiencies.xml:1T:1F[T]"),
    // Debug level for MM fake estimate using FakeBkgTools from IFF
    m_doFakesMMIFFDebug(false),
    // Apply overlap removal on loose lepton definitons - not the top recommendation, for studies only
    m_doOverlapRemovalOnLooseLeptonDef(false),
    // do overlap removal also with large-R jets
    // (using whatever procedure is used in the official tools)
    m_doLargeJetOverlapRemoval(false),
    // Dumps the normal non-"*_Loose" trees
    m_doTightEvents(true),
    // Runs Loose selection and dumps the "*_Loose" trees
    m_doLooseEvents(false),
    // Runs systematics on the given selection
    m_doTightSysts(true),
    m_doLooseSysts(true),
    // In the *_Loose trees, lepton SFs are calculated considering
    // tight ID and isolation instead of loose
    // Only tight leptons are considered in the event SF calculation
    m_applyTightSFsInLooseTree(false),
    // For boosted analysis
    m_applyElectronInJetSubtraction(false),
    // Write Truth block info
    m_doTruthBlockInfo(false),
    // Write Truth PDF info
    m_doTruthPDFInfo(false),
    m_doTruthPDFInfoInNominalTrees(false),
    // Write MC generator weights
    m_doMCGeneratorWeights(false),
    m_doMCGeneratorWeightsInNominalTrees(false),
    m_nominalWeightNames(),
    m_detectedNominalWeightName("SetMe"),
    m_detectedNominalWeightIndex(0),
    // Top Parton History
    m_doTopPartonHistory(false),
    m_isTopPartonHistoryRegisteredInNtuple(false),

    m_doTopParticleLevel(false),
    m_doParticleLevelOverlapRemovalMuJet(true),
    m_doParticleLevelOverlapRemovalElJet(true),
    m_doParticleLevelOverlapRemovalJetPhoton(false),

    // KLFitter
    m_doKLFitter(false),
    m_KLFitterTransferFunctionsPath("SetMe"),
    m_KLFitterOutput("SetMe"),
    m_KLFitterJetSelectionMode("SetMe"),
    m_KLFitterBTaggingMethod("SetMe"),
    m_KLFitterLH("SetMe"),
    m_KLFitterTopMassFixed(true),
    m_KLFitterSaveAllPermutations(false),
    m_KLFitterFailOnLessThanXJets(false),

    // PseudoTop
    m_doPseudoTop(false),

    // StoreGate / TStore keys
    m_sgKeyDummy("SetMe"),
    m_sgKeyEventInfo("EventInfo"),
    m_sgKeyPrimaryVertices("PrimaryVertices"),
    m_sgKeyPhotons("SetMe"),
    m_sgKeyElectrons("SetMe"),
    m_sgKeyFwdElectrons("SetMe"),
    m_sgKeyMuons("SetMe"),
    m_sgKeySoftMuons("SetMe"),
    m_sgKeyTaus("SetMe"),
    m_sgKeyJets("SetMe"),
    m_sgKeyLargeRJets("SetMe"),
    m_sgKeyTrackJets("SetMe"),
    m_sgKeyMissingEt("MET"),
    m_sgKeyMissingEtLoose("LooseMET"),
    m_sgKeyInDetTrackParticles("InDetTrackParticles"),

    m_sgKeyTruthEvent("TruthEvents"),
    m_sgKeyMCParticle("SetMe"),
    m_sgKeyTruthPhotons("TruthPhotons"),
    m_sgKeyTruthElectrons("TruthElectrons"),
    m_sgKeyTruthMuons("TruthMuons"),
    m_sgKeyTruthTaus("TruthTaus"),
    m_sgKeyTruthJets("SetMe"),
    m_sgKeyTruthLargeRJets("SetMe"),
    m_sgKeyTruthMET("MET_Truth"),
    m_sgKeyTopPartonHistory("TopPartonHistory"),

    m_sgKeyTopSystematicEvents("TopSystematicEvents"),
    m_sgKeyTopSystematicEventsLoose("TopSystematicEventsLoose"),
    m_passEventSelectionDecoration("passEventSelection"),
    m_decoKeyJetGhostTrack("GhostTrack"),

    // special: allow to dump the systematics-shifted b-tagging SFs in the systematics trees
    m_dumpBtagSystsInSystTrees(false),

    m_storePerJetBtagSFs(false),

    // Electron configuration
    m_egammaSystematicModel("1NP_v1"),
    m_electronEfficiencySystematicModel("TOTAL"),
    m_electronEfficiencySystematicModelEtaBinning(""),
    m_electronEfficiencySystematicModelEtBinning(""),
    m_electronID("SetMe"),
    m_electronIDLoose("SetMe"),
    m_electronVetoLArCrack(true),
    m_electronPtcut(25000.),
    m_electronIsolation("SetMe"),
    m_electronIsolationLoose("SetMe"),
    m_electronIsolationSF("SetMe"),
    m_electronIsolationSFLoose("SetMe"),
    m_electronIsoSFs(true),
    m_electronIDDecoration("SetMe"),
    m_electronIDLooseDecoration("SetMe"),
    m_useElectronChargeIDSelection(false),
    m_useEgammaLeakageCorrection(true),

    // Fwd electron configuration
    m_fwdElectronPtcut(25000.),
    m_fwdElectronMinEtacut(2.5),
    m_fwdElectronMaxEtacut(4.9),
    m_fwdElectronID("SetMe"),
    m_fwdElectronIDLoose("SetMe"),
    m_fwdElectronBCIDCleaningMinRun(266904),
    m_fwdElectronBCIDCleaningMaxRun(311481),

    // Muon configuration
    m_muonPtcut(25000.),
    m_muonEtacut(2.5),
    m_muonQuality("SetMe"),
    m_muonQualityLoose("SetMe"),
    m_muonIsolation("SetMe"),
    m_muonIsolationLoose("SetMe"),
    m_muonIsolationSF("SetMe"),
    m_muonIsolationSFLoose("SetMe"),

    // Soft Muon configuration
    m_softmuonPtcut(4000.),
    m_softmuonEtacut(2.5),
    m_softmuonQuality("SetMe"),
    m_softmuonDRJetcut(0.4),
    m_softmuonAdditionalTruthInfo(false),
    m_softmuonAdditionalTruthInfoCheckPartonOrigin(false),
    m_softmuonAdditionalTruthInfoDoVerbose(false),

    // Jet configuration
    m_jetPtcut(25000.),
    m_jetEtacut(2.5),
    m_fwdJetAndMET("Default"),
    m_jetPtGhostTracks(19000.),
    m_jetUncertainties_BunchSpacing("25ns"),
    m_jetUncertainties_NPModel("AllNuisanceParameters"),
    m_jetUncertainties_QGFracFile("None"),
    m_jetUncertainties_QGHistPatterns(),
    m_doMultipleJES(false),
    m_jetJERSmearingModel("Simple"),
    m_jetCalibSequence("GSC"),
    m_jetStoreTruthLabels("True"),
    m_doJVTInMETCalculation(true),
    m_saveFailJVTJets(false),
    m_JVTWP("Default"),

    m_largeRJetPtcut(25000.),
    m_largeRJetEtacut(2.5),
    m_largeRJetUncertainties_NPModel("CategoryReduction"),
    m_largeRJESJMSConfig("SetMe"),

    m_trackJetPtcut(7000.0),
    m_trackJetEtacut(2.5),

    m_JSF(1.0),
    m_bJSF(1.0),

    // Tau configuration
    /**
       m_tauJetID("SetMe"),
       m_tauJetIDBkg("SetMe"),
       m_tauEVetoID("SetMe"),
       m_tauVetoLArCrack(false),
       m_tauPtcut(20000.),
     **/

    // [[[-----------------------------------------------
    // Particle Level / Truth Configuration
    m_truth_electron{25000., 2.5, true, false},
    m_truth_muon{25000., 2.5, true, false},
    m_truth_photon{25000., 2.5, "SET_ME", "SET_ME"},
    m_truth_jet{25000., 2.5},
    // -----------------------------------------------]]]

    // HL LHC studies
    m_HLLHC(false),
    m_HLLHCFakes(false),

    // Selections
    m_allSelectionNames(nullptr),
    // Trigger
    m_allTriggers_Tight(nullptr),
    m_electronTriggers_Tight(nullptr),
    m_muonTriggers_Tight(nullptr),
    m_tauTriggers_Tight(nullptr),
    m_allTriggers_Loose(nullptr),
    m_electronTriggers_Loose(nullptr),
    m_muonTriggers_Loose(nullptr),
    m_tauTriggers_Loose(nullptr),

    // Where the sum of event weights
    // before derivation framework is kept
    m_sumOfEventWeightsMetaData("CutBookkeepersAux.sumOfEventWeights"),

    // Object Selector name
    m_objectSelectionName("SetMe"),
    // Output Format
    m_outputFormat("SetMe"),
    // Output Events
    m_outputEvents("SetMe"),
    m_saveOnlySelectedEvents(true),
    m_outputFileSetAutoFlushZero(false),
    m_outputFileNEventAutoFlush(1000), // 1000 events
    m_outputFileBasketSizePrimitive(4096), // 4kB
    m_outputFileBasketSizeVector(40960),   // 40kB
    // Number of events to run on (only for testing)
    m_numberOfEventsToRun(0),

    // Number of events to skip (only for testing)
    m_numberOfEventsToSkip(0),

    m_aodMetaData(new AodMetaDataAccess()),

    // Systematics
    m_nominalHashValue(0),

    m_systHashPhotons(nullptr),
    m_systHashElectrons(nullptr),
    m_systHashFwdElectrons(nullptr),
    m_systHashMuons(nullptr),
    m_systHashSoftMuons(nullptr),
    m_systHashTaus(nullptr),
    m_systHashJets(nullptr),
    m_systHashLargeRJets(nullptr),
    m_systHashTrackJets(nullptr),
    m_systHashMET(nullptr),

    m_systHashAll(nullptr),
    m_list_systHashAll(nullptr),
    m_systHash_electronInJetSubtraction(nullptr),
    m_list_systHash_electronInJetSubtraction(nullptr),


    m_systMapPhotons(nullptr),
    m_systMapElectrons(nullptr),
    m_systMapFwdElectrons(nullptr),
    m_systMapMuons(nullptr),
    m_systMapSoftMuons(nullptr),
    m_systMapTaus(nullptr),
    m_systMapJets(nullptr),
    m_systMapLargeRJets(nullptr),
    m_systMapTrackJets(nullptr),
    m_systMapMET(nullptr),

    m_systSgKeyMapPhotons(nullptr),
    m_systSgKeyMapElectrons(nullptr),
    m_systSgKeyMapFwdElectrons(nullptr),
    m_systSgKeyMapMuons(nullptr),
    m_systSgKeyMapSoftMuons(nullptr),
    m_systSgKeyMapTaus(nullptr),
    m_systSgKeyMapJets(nullptr),
    m_systSgKeyMapJets_electronInJetSubtraction(nullptr),
    m_systSgKeyMapJetsLoose_electronInJetSubtraction(nullptr),
    m_systSgKeyMapLargeRJets(nullptr),
    m_systSgKeyMapTrackJets(nullptr),
    m_systSgKeyMapMET(nullptr),

    m_systAllSgKeyMapPhotons(nullptr),
    m_systAllSgKeyMapElectrons(nullptr),
    m_systAllSgKeyMapFwdElectrons(nullptr),
    m_systAllSgKeyMapMuons(nullptr),
    m_systAllSgKeyMapSoftMuons(nullptr),
    m_systAllSgKeyMapTaus(nullptr),
    m_systAllSgKeyMapJets(nullptr),
    m_systAllSgKeyMapLargeRJets(nullptr),
    m_systAllSgKeyMapTrackJets(nullptr),
    m_systAllSgKeyMapElectrons_electronInJetSubtraction(nullptr),
    m_systAllSgKeyMapJets_electronInJetSubtraction(nullptr),
    m_systAllSgKeyMapJetsLoose_electronInJetSubtraction(nullptr),

    m_systAllSgKeyMapPhotonsTDS(nullptr),
    m_systAllSgKeyMapPhotonsTDSAux(nullptr),
    m_systAllSgKeyMapElectronsTDS(nullptr),
    m_systAllSgKeyMapElectronsTDSAux(nullptr),
    m_systAllSgKeyMapFwdElectronsTDS(nullptr),
    m_systAllSgKeyMapFwdElectronsTDSAux(nullptr),
    m_systAllSgKeyMapMuonsTDS(nullptr),
    m_systAllSgKeyMapMuonsTDSAux(nullptr),
    m_systAllSgKeyMapSoftMuonsTDS(nullptr),
    m_systAllSgKeyMapSoftMuonsTDSAux(nullptr),
    m_systAllSgKeyMapTausTDS(nullptr),
    m_systAllSgKeyMapTausTDSAux(nullptr),
    m_systAllSgKeyMapJetsTDS(nullptr),
    m_systAllSgKeyMapJetsTDSAux(nullptr),
    m_systAllSgKeyMapLargeRJetsTDS(nullptr),
    m_systAllSgKeyMapLargeRJetsTDSAux(nullptr),
    m_systAllSgKeyMapTrackJetsTDS(nullptr),
    m_systAllSgKeyMapTrackJetsTDSAux(nullptr),
    m_systAllSgKeyMapElectrons_electronInJetSubtractionTDS(nullptr),
    m_systAllSgKeyMapElectrons_electronInJetSubtractionTDSAux(nullptr),
    m_systAllSgKeyMapJets_electronInJetSubtractionTDS(nullptr),
    m_systAllSgKeyMapJets_electronInJetSubtractionTDSAux(nullptr),
    m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDS(nullptr),
    m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDSAux(nullptr),

    m_systSgKeyMapMissingET(nullptr),
    m_systSgKeyMapMissingETLoose(nullptr),

    m_sgKeyKLFitter("KLFitterResults"),
    m_systSgKeyMapKLFitter(nullptr),
    m_systSgKeyMapKLFitterLoose(nullptr),

    m_sgKeyPseudoTop("PseudoTopResult"),
    m_systSgKeyMapPseudoTop(nullptr),
    m_systSgKeyMapPseudoTopLoose(nullptr),
    m_systMapJetGhostTrack(nullptr),
    m_systDecoKeyMapJetGhostTrack(nullptr),

    m_systAllTTreeNames(nullptr),
    m_systPersistantAllTTreeNames(nullptr),
    m_systAllTTreeIndex(nullptr),
    m_systAllTTreeLooseIndex(nullptr),
    m_saveBootstrapWeights(false),
    m_BootstrapReplicas(100),
    m_useBadBatmanCleaning(true),
    m_badBatmanCleaningMin(276262),
    m_badBatmanCleaningMax(311481),
    m_useEventLevelJetCleaningTool(false) {
    m_allSelectionNames = std::shared_ptr<std::vector<std::string> > (new std::vector<std::string> );

    m_systHashPhotons = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_systHashElectrons = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_systHashFwdElectrons = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_systHashMuons = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_systHashSoftMuons = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_systHashTaus = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_systHashJets = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_systHashLargeRJets = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_systHashTrackJets = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_systHashMET = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );

    m_systHashAll = std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_list_systHashAll = std::shared_ptr<std::list<std::size_t> > (new std::list<std::size_t> );
    m_systHash_electronInJetSubtraction =
      std::shared_ptr<std::unordered_set<std::size_t> > (new std::unordered_set<std::size_t> );
    m_list_systHash_electronInJetSubtraction = std::shared_ptr<std::list<std::size_t> > (new std::list<std::size_t> );

    m_systMapPhotons =
      std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (new std::unordered_map<std::size_t,
                                                                                                   CP::SystematicSet> );
    m_systMapElectrons =
      std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (new std::unordered_map<std::size_t,
                                                                                                   CP::SystematicSet> );
    m_systMapFwdElectrons = std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (
      new std::unordered_map<std::size_t, CP::SystematicSet> );
    m_systMapMuons =
      std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (new std::unordered_map<std::size_t,
                                                                                                   CP::SystematicSet> );
    m_systMapSoftMuons =
      std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (new std::unordered_map<std::size_t,
                                                                                                   CP::SystematicSet> );
    m_systMapTaus =
      std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (new std::unordered_map<std::size_t,
                                                                                                   CP::SystematicSet> );
    m_systMapJets =
      std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (new std::unordered_map<std::size_t,
                                                                                                   CP::SystematicSet> );
    m_systMapLargeRJets =
      std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (new std::unordered_map<std::size_t,
                                                                                                   CP::SystematicSet> );
    m_systMapTrackJets =
      std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (new std::unordered_map<std::size_t,
                                                                                                   CP::SystematicSet> );
    m_systMapMET =
      std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (new std::unordered_map<std::size_t,
                                                                                                   CP::SystematicSet> );

    m_systSgKeyMapPhotons =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapElectrons =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapFwdElectrons =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapMuons =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapSoftMuons =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapTaus =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapJets =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapJets_electronInJetSubtraction = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systSgKeyMapJetsLoose_electronInJetSubtraction = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systSgKeyMapLargeRJets =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapTrackJets =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapMET =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );

    m_systAllSgKeyMapPhotons =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapElectrons =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapFwdElectrons = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapMuons =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapSoftMuons =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapTaus =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapJets =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapLargeRJets = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapTrackJets =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );

    m_systAllSgKeyMapElectrons_electronInJetSubtraction
      = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                               std::string> );
    m_systAllSgKeyMapJets_electronInJetSubtraction
      = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                               std::string> );
    m_systAllSgKeyMapJetsLoose_electronInJetSubtraction
      = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                               std::string> );

    m_systAllSgKeyMapPhotonsTDS = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapPhotonsTDSAux = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapElectronsTDS = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapElectronsTDSAux = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapFwdElectronsTDS = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapFwdElectronsTDSAux = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapMuonsTDS =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapMuonsTDSAux = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapSoftMuonsTDS = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapSoftMuonsTDSAux = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapTausTDS =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapTausTDSAux = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapJetsTDS =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapJetsTDSAux = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapLargeRJetsTDS = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapLargeRJetsTDSAux = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapTrackJetsTDS = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapTrackJetsTDSAux = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapElectrons_electronInJetSubtractionTDS =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapElectrons_electronInJetSubtractionTDSAux =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapJets_electronInJetSubtractionTDS = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllSgKeyMapJets_electronInJetSubtractionTDSAux =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDS =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDSAux =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );


    m_systSgKeyMapMissingET =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapMissingETLoose = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );

    m_systSgKeyMapKLFitter =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapKLFitterLoose = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );

    m_systSgKeyMapPseudoTop =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systSgKeyMapPseudoTopLoose = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );

    m_systMapJetGhostTrack = std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > (
      new std::unordered_map<std::size_t, CP::SystematicSet> );
    m_systDecoKeyMapJetGhostTrack = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );

    m_systAllTTreeNames =
      std::shared_ptr<std::unordered_map<std::size_t, std::string> > (new std::unordered_map<std::size_t,
                                                                                             std::string> );
    m_systPersistantAllTTreeNames = std::shared_ptr<std::unordered_map<std::size_t, std::string> > (
      new std::unordered_map<std::size_t, std::string> );
    m_systAllTTreeIndex =
      std::shared_ptr<std::unordered_map<std::size_t, unsigned int> > (new std::unordered_map<std::size_t,
                                                                                              unsigned int> );
    m_systAllTTreeLooseIndex =
      std::shared_ptr<std::unordered_map<std::size_t, unsigned int> > (new std::unordered_map<std::size_t,
                                                                                              unsigned int> );

    // Nominal has value
    CP::SystematicSet nominal;
    m_nominalHashValue = nominal.hash();
    
    m_treeFilter = std::make_shared<TreeFilter>();
  }

  void TopConfig::setConfigSettings(top::ConfigurationSettings* const& settings) {
    // Selections
    const std::vector<top::SelectionConfigurationData> allSelections = settings->selections();

    for (auto sel : allSelections) {
      m_allSelectionNames->push_back(sel.m_name);
    }

    // set GRL file
    this->setGrlDir(settings->value("GRLDir"));
    this->setGrlFile(settings->value("GRLFile"));

    // Set TDP file name
    this->setTDPPath(settings->value("TDPPath"));

    m_treeFilter->init(settings->value("FilterTrees"));


    //we need storegate keys so people can pick different collections / met / jets etc.
    this->sgKeyPhotons(settings->value("PhotonCollectionName"));
    this->sgKeyElectrons(settings->value("ElectronCollectionName"));
    this->sgKeyFwdElectrons(settings->value("FwdElectronCollectionName"));
    this->sgKeyMuons(settings->value("MuonCollectionName"));
    //small trick for the soft muons
    if (settings->value("UseSoftMuons") == "True") this->sgKeySoftMuons("Soft" + settings->value("MuonCollectionName"));

    else this->sgKeySoftMuons("None");
    this->sgKeyTaus(settings->value("TauCollectionName"));
    this->sgKeyJets(settings->value("JetCollectionName"));
    this->sgKeyLargeRJets(settings->value("LargeJetCollectionName"));
    this->sgKeyTrackJets(settings->value("TrackJetCollectionName"));
    this->jetSubstructureName(settings->value("LargeJetSubstructure"));
    this->decoKeyJetGhostTrack(settings->value("JetGhostTrackDecoName"));

    // check that small-R jets use tagged collectio name for new derivations
    // this is due to b-tagging since AthDerivation-21.2.72.0
    if (m_aodMetaData->valid()) {
      try {
        std::string deriv_rel_name = m_aodMetaData->get("/TagInfo", "AtlasRelease_AODtoDAOD");
        size_t pos = deriv_rel_name.find('-');
        if (pos != std::string::npos) {
          deriv_rel_name = deriv_rel_name.substr(pos + 1);
          if (deriv_rel_name >= "21.2.72.0") { // release where we need tagged jet collection
            if (this->sgKeyJets() == this->sgKeyJetsType()) { // jet collection is NOT tagged
              throw std::runtime_error(
                      "TopConfig: You are using derivation with release 21.2.72.0 or newer and did not specify tagged small-R jet collection, e.g. \"AntiKt4EMPFlowJets_BTagging201903\". This is necessary for b-tagging to work!");
            }
          } else { // release does NOT have tagged jet collection
            if (this->sgKeyJets() != this->sgKeyJetsType()) { // jet collection is NOT tagged
              throw std::runtime_error(
                      "TopConfig: You are using derivation with release older than 21.2.72.0 so you cannot use tagged jet containers as you specified: \"" + this->sgKeyJets() + "\". Use \"" + this->sgKeyJetsType() +
                      "\" instead.");
            }
          }
        } else {
          std::cout <<
            "WARNING: Could not parse derivation release from the file metadata. We cannot check that correct jet collection is used for b-tagging. You are on your own."
                    << std::endl;
        }
        // try to parse the derivation release, we need the release number
      } catch (std::logic_error& e) {
        std::cout << e.what() << std::endl;
        std::cout <<
          "WARNING: Could not obtain derivation release from the file metadata. We cannot check that correct jet collection is used for b-tagging. You are on your own."
                  << std::endl;
      }
    }

    // ROOTCORE/Analysis release series
    this->setReleaseSeries();

    if (settings->value("applyTTVACut") == "False") this->m_applyTTVACut = false;

    if (this->isMC()) {
      this->sgKeyMCParticle(settings->value("TruthCollectionName"));
      this->sgKeyTruthElectrons(settings->value("TruthElectronCollectionName"));
      this->sgKeyTruthMuons(settings->value("TruthMuonCollectionName"));
      this->sgKeyTruthPhotons(settings->value("TruthPhotonCollectionName"));
      this->sgKeyTruthMET(settings->value("TruthMETCollectionName"));
      this->sgKeyTruthJets(settings->value("TruthJetCollectionName"));
      this->sgKeyTruthLargeRJets(settings->value("TruthLargeRJetCollectionName"));
      this->sgKeyTruthTaus(settings->value("TruthTauCollectionName"));

      // Dump truth block
      if (settings->value("TruthBlockInfo") == "True") this->setTruthBlockInfo();

      // Save the Truth PDF information
      if (settings->value("PDFInfo") == "True") {
        this->setTruthPDFInfo();
      } else if (settings->value("PDFInfo") == "Nominal") {
        // Save the Truth PDF information in the reco-level tree instead of the truth-level one
        this->setTruthPDFInfo();
        this->setTruthPDFInfoInNominalTrees();
      }

      // Save the MC generator weights
      if (settings->value("MCGeneratorWeights") == "True") {
        this->setMCGeneratorWeights();
      } else if (settings->value("MCGeneratorWeights") == "Nominal") {
        // Save the Truth PDF information in the reco-level tree instead of the truth-level one
        this->setMCGeneratorWeights();
        this->setMCGeneratorWeightsInNominalTrees();
      }

      // load the nominal weight names that we should try to get the real nominal weight name
      boost::split(m_nominalWeightNames, settings->value("NominalWeightNames"), boost::is_any_of(","));
      for (std::string &weight : m_nominalWeightNames) {
        boost::trim(weight);
        boost::trim_if(weight, boost::is_any_of("\"\'"));
      }

      // Save the Top Parton History
      if (this->useTruthParticles() && settings->value("TopPartonHistory") != "False") this->setTopPartonHistory();

      // Perform particle-level selection and save particle level objects
      if (settings->value("TopParticleLevel") == "True") {
        this->setTopParticleLevel();
      }
      // Particle-level OR
      if (settings->value("DoParticleLevelOverlapRemoval") == "True") {
        // Value True -> Do all ORs
        this->setParticleLevelOverlapRemovalElJet(true);
        this->setParticleLevelOverlapRemovalMuJet(true);
        this->setParticleLevelOverlapRemovalJetPhoton(true);
      } else if (settings->value("DoParticleLevelOverlapRemoval") == "False") {
        // Value False -> Do not perform any OR.
        this->setParticleLevelOverlapRemovalElJet(false);
        this->setParticleLevelOverlapRemovalMuJet(false);
        this->setParticleLevelOverlapRemovalJetPhoton(false);
      } else {
        // Explicitly specify the possible OR values.
        std::vector<std::string> tokens;
        tokenize(settings->value("DoParticleLevelOverlapRemoval"), tokens, ",");
        while (tokens.size()) {
          const auto& token = tokens.back();

          if (token == "MuonJet") {
            this->setParticleLevelOverlapRemovalMuJet(true);
          } else if (token == "ElectronJet") {
            this->setParticleLevelOverlapRemovalElJet(true);
          } else if (token == "JetPhoton") {
            this->setParticleLevelOverlapRemovalJetPhoton(true);
          } else {
            throw std::runtime_error {
                    "TopConfig: Option DoParticleLevelOverlapRemoval: Invalid Token"
            };
          }

          // Remove the last token in the container.
          tokens.pop_back();
        }
      }

      // check if you are running over AFII samples
      // only check the configuration file if the AodMetaData is not instatiated
      if (m_aodMetaData->valid()) {
        try{
          auto simulatorName = m_aodMetaData->get("/Simulation/Parameters", "Simulator");
          bool aodMetaDataIsAFII = m_aodMetaData->isAFII();
          std::cout << "AodMetaData :: Simulation Type " << simulatorName << " -> " << "Setting IsAFII to " <<
            aodMetaDataIsAFII << std::endl;
          this->setIsAFII(aodMetaDataIsAFII);
          auto generatorsName = m_aodMetaData->get("/TagInfo", "generators");
          std::cout << "AodMetaData :: Generators Type " << generatorsName << std::endl;
          this->setGenerators(generatorsName);
          auto AMITagName = m_aodMetaData->get("/TagInfo", "AMITag");
          std::cout << "AodMetaData :: AMITag " << AMITagName << std::endl;
          this->setAMITag(AMITagName);
        }
        catch (const std::logic_error& aodMetaDataError) {
          std::cout << "An error was encountered handling AodMetaData : " << aodMetaDataError.what() << std::endl;
          std::cout << "We will attempt to read the IsAFII flag from your config." << std::endl;
          this->ReadIsAFII(settings);
          std::cout << "Unfortunately, we can not read MC generators and AMITag without valid MetaData." << std::endl;
          this->setGenerators("unknown");
          this->setAMITag("unknown");
        }
      } else {
        this->ReadIsAFII(settings);
      }
    }

    // Get list of branches to be filtered
    if (settings->value("FilterBranches") != " ") {
      std::vector<std::string> branches;
      tokenize(settings->value("FilterBranches"), branches, ",");

      if (branches.size() == 0) {
        std::cout <<
          "WARNING: You provided \"Filterbranches\" option but you did not provide any meaningful values. Ignoring" <<
          std::endl;
      }
      this->setFilterBranches(branches);
    }

    // Force recomputation of CP variables?
    if (settings->value("RecomputeCPVariables") == "False") m_recomputeCPvars = false;

    // Bootstrapping weights (permitted in MC and Data)
    if (settings->value("SaveBootstrapWeights") == "True") {
      this->setSaveBootstrapWeights(true);
      this->setNumberOfBootstrapReplicas(std::atoi(settings->value("NumberOfBootstrapReplicas").c_str()));
    }


    if (this->isMC()) {
      m_doLooseEvents = (settings->value("DoLoose") == "MC" || settings->value("DoLoose") == "Both");
      m_doTightEvents = (settings->value("DoTight") == "MC" || settings->value("DoTight") == "Both");
      m_doLooseSysts = (settings->value("DoSysts") == "Loose" || settings->value("DoSysts") == "Both") &&
                       m_doLooseEvents;
      m_doTightSysts = (settings->value("DoSysts") == "Tight" || settings->value("DoSysts") == "Both") &&
                       m_doTightEvents;
    } else {
      m_doLooseEvents = (settings->value("DoLoose") == "Data" || settings->value("DoLoose") == "Both");
      if (m_doLooseEvents) {
        if (settings->value("FakesMMWeightsIFF") == "True") {
          this->setFakesMMWeightsCalculationIFF();
          std::string configIFF = settings->value("FakesMMConfigIFF");
          if (configIFF != "") {
            this->setFakesMMConfigIFF(configIFF);
          }
          if (settings->value("FakesMMIFFDebug") == "True") {
            this->setFakesMMIFFDebug();
          }
        }
      }
      m_doTightEvents = (settings->value("DoTight") == "Data" || settings->value("DoTight") == "Both");
    }

    // Switch to set event BadBatman cleaning
    if (settings->value("UseBadBatmanCleaning") == "False") {
      this->setUseBadBatmanCleaning(false);
    } else if (settings->value("UseBadBatmanCleaning") == "True") {
      this->setUseBadBatmanCleaning(true);
    } else {
      throw std::invalid_argument {
              "TopConfig: Option UseBadBatmanCleaning unknown value, only True or False (default) is allowed"
      };
    }

    // now check the ranges of the batman cleaning
    {
      std::vector<std::string> tokens;
      tokenize(settings->value("BadBatmanCleaningRange"), tokens, ":");
      if (tokens.size() != 2) {
        throw std::runtime_error {
                "TopConfig: Option BadBatmanCleaningRange should be of the form \'RunNumber1:RunNumber2\'"
        };
      }
      unsigned int minRunNumber = 999999;
      unsigned int maxRunNumber = 0;
      try { // convert the values from string to unsigned int
        minRunNumber = std::stoul(tokens.at(0));
        maxRunNumber = std::stoul(tokens.at(1));
      } catch (...) {
        throw std::invalid_argument {
                "TopConfig: Option BadBatmanCleaningRange cannot convert the RunNumbers into unsigned int"
        };
      }

      // check if the first value is not larger than the second value
      if (maxRunNumber < minRunNumber) {
        throw std::invalid_argument {
                "TopConfig: Option BadBatmanCleaningRange: the first RunNumber cannot be larger than the second!"
        };
      }

      // check if there is an overlap with data 2017 as this option should not be used for this period
      static const unsigned int data17_begin = 325713;
      static const unsigned int data17_end = 348835;

      if (std::max(minRunNumber, data17_begin) <= std::min(maxRunNumber, data17_end)) {
        throw std::invalid_argument {
                "TopConfig: Option BadBatmanCleaningRange cannot include RunNumbers from 2017 data taking (325713-348835)"
        };
      }

      this->setBadBatmanCleaningMin(minRunNumber);
      this->setBadBatmanCleaningMax(maxRunNumber);
    }

    // Switch to set event level jet cleaning tool [false by default]
    if (settings->value("UseEventLevelJetCleaningTool") == "True") {
      this->setUseEventLevelJetCleaningTool(true);
    }

    // Object Selection Name
    this->objectSelectionName(settings->value("ObjectSelectionName"));
    // Output Format
    this->outputFormat(settings->value("OutputFormat"));
    // Output Events
    this->outputEvents(settings->value("OutputEvents"));
    // SetAutoFlush(0) on EventSaverFlatNtuple for ANALYSISTO-44 workaround
    m_outputFileSetAutoFlushZero = false;
    if (settings->value("OutputFileSetAutoFlushZero") != "False") {
      std::cout << "OutputFileSetAutoFlushZero is deprecated in favour of more custom memory options" << std::endl;
    }
    // Configurable TTree options (ANALYSISTO-463)
    if (settings->value("OutputFileNEventAutoFlush") != "") {
      m_outputFileNEventAutoFlush = std::stoi(settings->value("OutputFileNEventAutoFlush"));
    }
    if (settings->value("OutputFileBasketSizePrimitive") != "") {
      m_outputFileBasketSizePrimitive = std::stoi(settings->value("OutputFileBasketSizePrimitive"));
    }
    if (settings->value("OutputFileBasketSizeVector") != "") {
      m_outputFileBasketSizeVector = std::stoi(settings->value("OutputFileBasketSizeVector"));
    }

    // The systematics want much much more configuration options.....
    this->systematics(settings->value("Systematics"));

    // special: allow to dump the systematics-shifted b-tagging SFs in the systematics trees
    if (settings->value("DumpBtagSystsInSystTrees") == "True") this->dumpBtagSystsInSystTrees(true);

    // special: store per-jet btag SFs
    if (settings->value("StorePerJetBtagSFs") == "True") this->storePerJetBtagSFs(true);

    // Electron configuration
    this->egammaSystematicModel(settings->value("EgammaSystematicModel"));
    this->electronEfficiencySystematicModel(settings->value("ElectronEfficiencySystematicModel"));
    this->electronEfficiencySystematicModelEtaBinning(settings->value("ElectronEfficiencySystematicModelEtaBinning"));
    this->electronEfficiencySystematicModelEtBinning(settings->value("ElectronEfficiencySystematicModelEtBinning"));
    this->electronID(settings->value("ElectronID"));
    this->electronIDLoose(settings->value("ElectronIDLoose"));
    {
      std::string const& cut_wp = settings->value("ElectronIsolation");
      std::string const& sf_wp = settings->value("ElectronIsolationSF");
      this->electronIsolation(cut_wp);
      this->electronIsolationSF(sf_wp == " " ? cut_wp : sf_wp);
    }
    {
      std::string const& cut_wp = settings->value("ElectronIsolationLoose");
      std::string const& sf_wp = settings->value("ElectronIsolationSFLoose");
      this->electronIsolationLoose(cut_wp);
      this->electronIsolationSFLoose(sf_wp == " " ? cut_wp : sf_wp);
    }
    this->useElectronChargeIDSelection(settings->value("UseElectronChargeIDSelection"));
    this->useEgammaLeakageCorrection(settings->value("UseEgammaLeakageCorrection"));
    this->electronPtcut(std::stof(settings->value("ElectronPt")));



    m_electronIDDecoration = "AnalysisTop_" + m_electronID;
    m_electronIDLooseDecoration = "AnalysisTop_" + m_electronIDLoose;

    //Fwd electron configuration
    this->fwdElectronID(settings->value("FwdElectronID"));
    this->fwdElectronIDLoose(settings->value("FwdElectronIDLoose"));
    double fwdElPtCut = 99999999.;
    try{
      fwdElPtCut = std::stof(settings->value("FwdElectronPt"));
    }
    catch (...) {
      throw std::invalid_argument {
              "TopConfig: cannot convert Option FwdElectronPt into float"
      };
    }
    this->fwdElectronPtcut(fwdElPtCut);
    double fwdElMinEtaCut = 2.5;
    try{
      fwdElMinEtaCut = std::stof(settings->value("FwdElectronMinEta"));
    }
    catch (...) {
      throw std::invalid_argument {
              "TopConfig: cannot convert Option FwdElectronMinEta into float"
      };
    }
    this->fwdElectronMinEtacut(fwdElMinEtaCut);
    double fwdElMaxEtaCut = 2.5;
    try{
      fwdElMaxEtaCut = std::stof(settings->value("FwdElectronMaxEta"));
    }
    catch (...) {
      throw std::invalid_argument {
              "TopConfig: cannot convert Option FwdElectronMaxEta into float"
      };
    }
    this->fwdElectronMaxEtacut(fwdElMaxEtaCut);

    int fwdElectronBCIDCleaningMinRun = 0;
    int fwdElectronBCIDCleaningMaxRun = 0;
    std::vector<std::string> fwd_bcid_tokens;
    tokenize(settings->value("FwdElectronBCIDCleaningRunRange"), fwd_bcid_tokens, ":");
    if (fwd_bcid_tokens.size() != 2) {
      throw std::invalid_argument(
              "TopConfig: Option FwdElectronBCIDCleaningRunRange requires values in the form of \'XXX:YYY\'. The number of values needs to be exactly 2.");
    }
    try{
      fwdElectronBCIDCleaningMinRun = std::stoi(fwd_bcid_tokens[0]);
      fwdElectronBCIDCleaningMaxRun = std::stoi(fwd_bcid_tokens[1]);
    }
    catch (...) {
      throw std::invalid_argument(
              "TopConfig: Cannot convert the strings into integers for the run numbers in Option FwdElectronBCIDCleaningRunRange");
    }

    this->fwdElectronBCIDCleaningMinRun(fwdElectronBCIDCleaningMinRun);
    this->fwdElectronBCIDCleaningMaxRun(fwdElectronBCIDCleaningMaxRun);

    // Photon configuration
    this->photonPtcut(std::stof(settings->value("PhotonPt")));
    this->photonEtacut(std::stof(settings->value("PhotonEta")));
    this->photonIdentification(settings->value("PhotonID"));
    this->photonIdentificationLoose(settings->value("PhotonIDLoose"));
    this->photonIsolation(settings->value("PhotonIsolation"));
    this->photonIsolationLoose(settings->value("PhotonIsolationLoose"));
    if (settings->value("PhotonUseRadiativeZ") == "True") this->m_photon_configuration.useRadiativeZ = true;

    // Muon configuration
    this->muonPtcut(std::stof(settings->value("MuonPt")));
    this->muonEtacut(std::stof(settings->value("MuonEta")));
    this->muonQuality(settings->value("MuonQuality"));
    this->muonQualityLoose(settings->value("MuonQualityLoose"));
    {
      std::string const& cut_wp = settings->value("MuonIsolation");
      std::string const& sf_wp = settings->value("MuonIsolationSF");
      this->muonIsolation(cut_wp);
      this->muonIsolationSF(sf_wp == " " ? cut_wp : sf_wp);
    }
    {
      std::string const& cut_wp = settings->value("MuonIsolationLoose");
      std::string const& sf_wp = settings->value("MuonIsolationSFLoose");
      this->muonIsolationLoose(cut_wp);
      this->muonIsolationSFLoose(sf_wp == " " ? cut_wp : sf_wp);
    }

    if (settings->value("UseAntiMuons") == "True") this->m_useAntiMuons = true;

    // Soft Muon configuration
    this->softmuonPtcut(readFloatOption(settings, "SoftMuonPt"));
    this->softmuonEtacut(readFloatOption(settings, "SoftMuonEta"));
    this->softmuonQuality(settings->value("SoftMuonQuality"));
    this->softmuonDRJetcut(readFloatOption(settings, "SoftMuonDRJet"));
    if (settings->value("SoftMuonAdditionalTruthInfo") == "True") this->softmuonAdditionalTruthInfo(true);
    else this->softmuonAdditionalTruthInfo(false);
    if (settings->value("SoftMuonAdditionalTruthInfoCheckPartonOrigin") == "True") this->softmuonAdditionalTruthInfoCheckPartonOrigin(true);
    else this->softmuonAdditionalTruthInfoCheckPartonOrigin(false);
    if (settings->value("SoftMuonAdditionalTruthInfoDoVerbose") == "True") this->softmuonAdditionalTruthInfoDoVerbose(true);
    else this->softmuonAdditionalTruthInfoDoVerbose(false);

    //tau configuration
    this->tauPtcut(std::stof(settings->value("TauPt")));
    this->tauJetIDWP(settings->value("TauJetIDWP"));
    this->tauJetIDWPLoose(settings->value("TauJetIDWPLoose"));
    this->tauEleBDTWP(settings->value("TauEleBDTWP"));
    this->tauEleBDTWPLoose(settings->value("TauEleBDTWPLoose"));
    this->tauEleOLR((settings->value("TauEleOLR") == "True"));
    this->tauEleOLRLoose((settings->value("TauEleOLRLoose") == "True"));
    this->tauSFDoRNNID((settings->value("TauSFDoRNNID") == "True"));
    this->tauSFDoBDTID((settings->value("TauSFDoBDTID") == "True"));
    this->tauJetConfigFile(settings->value("TauJetConfigFile"));
    this->tauJetConfigFileLoose(settings->value("TauJetConfigFileLoose"));
    if (settings->value("ApplyTauMVATES") != "True") throw std::runtime_error {
              "TopConfig: ApplyTauMVATES must be True"
      };

    // Jet configuration
    this->jetPtcut(std::stof(settings->value("JetPt")));
    this->jetEtacut(std::stof(settings->value("JetEta")));
    this->fwdJetAndMET(settings->value("FwdJetAndMET"));
    this->jetPtGhostTracks(std::stof(settings->value("JetPtGhostTracks")));
    this->jetUncertainties_BunchSpacing(settings->value("JetUncertainties_BunchSpacing"));
    this->jetUncertainties_NPModel(settings->value("JetUncertainties_NPModel"));
    this->jetUncertainties_QGFracFile(settings->value("JetUncertainties_QGFracFile"));
    this->jetUncertainties_QGHistPatterns(settings->value("JetUncertainties_QGHistPatterns"));
    this->jetJERSmearingModel(settings->value("JetJERSmearingModel"));
    this->jetCalibSequence(settings->value("JetCalibSequence"));
    this->doJVTinMET((settings->value("JVTinMETCalculation") == "True" ? true : false));
    this->saveFailJVTJets((settings->value("SaveFailJVTJets") == "True" ? true : false));
    this->setJVTWP(settings->value("JVTWP"));
    this->m_largeRSmallRCorrelations = settings->value("LargeRSmallRCorrelations") == "True" ? true : false;

    this->largeRJetPtcut(std::stof(settings->value("LargeRJetPt")));
    this->largeRJetEtacut(std::stof(settings->value("LargeRJetEta")));
    this->largeRJetUncertainties_NPModel(settings->value("LargeRJetUncertainties_NPModel"));
    this->largeRJESJMSConfig(settings->value("LargeRJESJMSConfig"));

    this->trackJetPtcut(std::stof(settings->value("TrackJetPt")));
    this->trackJetEtacut(std::stof(settings->value("TrackJetEta")));

    // Jet configuration reclustered jets
    this->RCJetPtcut(std::stof(settings->value("RCJetPt")));
    this->RCJetEtacut(std::stof(settings->value("RCJetEta")));
    this->RCJetTrimcut(std::stof(settings->value("RCJetTrim")));
    this->RCJetRadius(std::stof(settings->value("RCJetRadius")));
    if (settings->value("UseRCJets") == "True" || settings->value("UseRCJets") == "true") this->m_useRCJets = true;
    if (settings->value("UseRCJetSubstructure") == "True" ||
        settings->value("UseRCJetSubstructure") == "true") this->m_useRCJetSubstructure = true;
    else this->m_useRCJetSubstructure = false;

    if (settings->value("UseRCJetAdditionalSubstructure") == "True" ||
        settings->value("UseRCJetAdditionalSubstructure") == "true") this->m_useRCJetAdditionalSubstructure = true;
    else this->m_useRCJetAdditionalSubstructure = false;

    this->VarRCJetPtcut(std::stof(settings->value("VarRCJetPt")));
    this->VarRCJetEtacut(std::stof(settings->value("VarRCJetEta")));
    this->VarRCJetTrimcut(std::stof(settings->value("VarRCJetTrim")));
    this->VarRCJetMaxRadius(std::stof(settings->value("VarRCJetMaxRadius")));
    this->VarRCJetRho(settings->value("VarRCJetRho"));
    this->VarRCJetMassScale(settings->value("VarRCJetMassScale"));
    if (settings->value("UseVarRCJets") == "True" ||
        settings->value("UseVarRCJets") == "true") this->m_useVarRCJets = true;
    if (settings->value("UseVarRCJetSubstructure") == "True" ||
        settings->value("UseVarRCJetSubstructure") == "true") this->m_useVarRCJetSubstructure = true;
    else this->m_useVarRCJetSubstructure = false;
    if (settings->value("UseVarRCJetAdditionalSubstructure") == "True" ||
        settings->value("UseVarRCJetAdditionalSubstructure") ==
        "true") this->m_useVarRCJetAdditionalSubstructure = true;
    else this->m_useVarRCJetAdditionalSubstructure = false;

    if (settings->value("StoreJetTruthLabels") == "False") {
      this->jetStoreTruthLabels(false);
    } else if (settings->value("StoreJetTruthLabels") == "True") {
      this->jetStoreTruthLabels(true);
    } else {
      std::cout <<
        "WARNING TopConfig::setConfigSettings: Unrecognized option for \"StoreJetTruthLabels\", assuming True" <<
        std::endl;
      this->jetStoreTruthLabels(true);
    }

    // for top mass analysis, per default set to 1.0!
    m_JSF = std::stof(settings->value("JSF"));
    m_bJSF = std::stof(settings->value("bJSF"));

    // If parameter exists remove electron crack veto
    if (settings->value("ElectronVetoLArCrack") == "False") this->removeElectronVetoLArCrack();

    // By default the top group does overlap removal on the tight lepton definitions
    // If you use this you are going off piste and need to report
    // your findings to the top reconstruction meeting
    if (settings->value("OverlapRemovalLeptonDef") == "Loose") this->setOverlapRemovalOnLooseLeptonDef();

    m_overlap_removal_procedure = settings->value("OverlapRemovalProcedure");

    // do overlap removal also with large-R jets
    // (using whatever procedure is used in the official tools)
    if (settings->value("LargeJetOverlapRemoval") == "True") {
      this->setLargeJetOverlapRemoval();// only usefull in case of MC
    }

    // In the *_Loose trees, lepton SFs are calculated considering
    // tight ID and isolation instead of loose
    // Only tight leptons are considered in the event SF calculation
    // only usefull in case of MC
    if (settings->value("ApplyTightSFsInLooseTree") == "True" && this->isMC()) this->setApplyTightSFsInLooseTree();

    // Apply Electron In Jet Subtraction - boosted analysis
    if (settings->value("ApplyElectronInJetSubtraction") == "True") this->setApplyElectronInJetSubtraction();

    // Set Number of events to run on (for testing)
    this->numberOfEventsToRun(std::stoi(settings->value("NEvents")));

    // Set Number of the first Event to run skip (for testing)
    this->numberOfEventsToSkip(std::stoi(settings->value("FirstEvent")));

    // [[[-----------------------------------------------
    //; Particle Level / Truth Configuration
    this->truth_electron_PtCut(std::stof(settings->value("TruthElectronPt")));
    this->truth_electron_EtaCut(std::stof(settings->value("TruthElectronEta")));

    this->truth_muon_PtCut(std::stof(settings->value("TruthMuonPt")));
    this->truth_muon_EtaCut(std::stof(settings->value("TruthMuonEta")));

    this->truth_photon_PtCut(std::stof(settings->value("TruthPhotonPt")));
    this->truth_photon_EtaCut(std::stof(settings->value("TruthPhotonEta")));
    // The TruthPhoton Origin and Isolation parameters are validated inside the
    // TopParticleLevel Loader Class.
    this->truth_photon_Origin(settings->value("TruthPhotonOrigin"));
    this->truth_photon_Isolation(settings->value("TruthPhotonIsolation"));

    this->truth_jet_PtCut(std::stof(settings->value("TruthJetPt")));
    this->truth_jet_EtaCut(std::stof(settings->value("TruthJetEta")));

    this->truth_jet_largeR_PtCut(std::stof(settings->value("TruthLargeRJetPt")));
    this->truth_jet_largeR_EtaCut(std::stof(settings->value("TruthLargeRJetEta")));

    this->truth_tau_PtCut(std::stof(settings->value("TruthTauPt")));
    this->truth_tau_EtaCut(std::stof(settings->value("TruthTauEta")));

    // -----------------------------------------------]]]

    // Upgrade studies
    if (settings->value("HLLHC") == "True") {
      this->HLLHC(true);
      if (settings->value("TDPPath").compare("dev/AnalysisTop/TopDataPreparation/XSection-MC15-13TeV.data") == 0) {
        std::cout << "TopConfig::setConfigSettings  HLLHC is set to True, but the TDPPath is set to default " <<
          settings->value("TDPPath") << ". Changing to dev/AnalysisTop/TopDataPreparation/XSection-MC15-14TeV.data" <<
          std::endl;
        this->setTDPPath("dev/AnalysisTop/TopDataPreparation/XSection-MC15-14TeV.data");
      }
    }
    if (settings->value("HLLHCFakes") == "True") this->HLLHCFakes(true);

    // LHAPDF Reweighting configuration
    std::istringstream lha_pdf_ss(settings->value("LHAPDFSets"));
    std::copy(std::istream_iterator<std::string>(lha_pdf_ss),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_lhapdf_options.pdf_set_names));

    // if provided, it will be used to recompute XF1,XF2 (if any is zero)
    const std::string LHAPDFBase = settings->value("LHAPDFBaseSet");
    if (LHAPDFBase.find_first_not_of(' ') != std::string::npos) {
      // should only set one base PDF set
      if (LHAPDFBase.find(' ') != std::string::npos) std::cout << "LHAPDFBaseSet: " << LHAPDFBase
                                                               <<
          " <<<<< only one PDF set allowed for recomputing XF1,XF2 !!!" << std::endl;
      m_lhapdf_options.baseLHAPDF = LHAPDFBase;
    }
    // if not already present, add to the list of PDF sets
    if (!m_lhapdf_options.baseLHAPDF.empty() &&
        !(std::find(m_lhapdf_options.pdf_set_names.begin(),
                    m_lhapdf_options.pdf_set_names.end(),
                    m_lhapdf_options.baseLHAPDF) !=
          m_lhapdf_options.pdf_set_names.end())) m_lhapdf_options.pdf_set_names.push_back(m_lhapdf_options.baseLHAPDF);

    if (m_lhapdf_options.pdf_set_names.size() && this->isMC()) m_lhapdf_options.doLHAPDF = true;

    if (m_lhapdf_options.pdf_set_names.size() && this->isMC() && settings->value("LHAPDFEventWeights") == "True") {
      m_lhapdf_options.save_per_event = true;
    } else if (m_lhapdf_options.pdf_set_names.size() && this->isMC() &&
               settings->value("LHAPDFEventWeights") == "Nominal") {
      m_lhapdf_options.save_per_event = true;
      m_lhapdf_options.doLHAPDFInNominalTrees = true;
    }

    // now get all Boosted jet taggers from the config file.
    std::string str_boostedJetTagger = settings->value("BoostedJetTagging");
    std::vector<std::string> helpvec_str;
    tokenize(str_boostedJetTagger, helpvec_str, ",");

    std::vector<std::string> vec_boostedJetTaggers;

    for (const std::string& x : helpvec_str) {
      std::istringstream istr_boostedJetTaggers(x);
      std::copy(std::istream_iterator<std::string>(istr_boostedJetTaggers),
                std::istream_iterator<std::string>(), std::back_inserter(vec_boostedJetTaggers));
    }

    for (const std::string& tagger : vec_boostedJetTaggers) {
      std::vector<std::string> helpvec;
      tokenize(tagger, helpvec, ":");
      if (helpvec.size() != 2) throw std::runtime_error {
                "TopConfig: Options in BoostedJetTagging should be of the form \'x:y\' where x is tagging type and y is shortened tagger name."
        };
      m_chosen_boostedJetTaggers.push_back(std::make_pair(helpvec[0], helpvec[1]));
    }

    m_btagging_cdi_path = settings->value("BTagCDIPath");

    // now get all Btagging WP from the config file, and store them properly in a map.
    // Need function to compare the cut value with the WP and vice versa

    std::istringstream str_btagging_WP(settings->value("BTaggingWP"));

    std::vector<std::string> all_btagging_WP;
    std::copy(std::istream_iterator<std::string>(str_btagging_WP),
              std::istream_iterator<std::string>(),
              std::back_inserter(all_btagging_WP));

    // loop through all btagging WPs requested
    for (auto AlgTag : all_btagging_WP) {
      std::vector<std::string> btagAlg_btagWP;
      tokenize(AlgTag, btagAlg_btagWP, ":");
      // DEFAULT algorithm - May remove in future
      std::string alg = "MV2c10";
      std::string tag = "";
      // If no ':' delimiter, assume we want default algorithm, and take the WP from the option
      if (btagAlg_btagWP.size() == 2) {
        alg = btagAlg_btagWP.at(0);
        tag = btagAlg_btagWP.at(1);
      } else if (btagAlg_btagWP.size() == 1) {
        tag = btagAlg_btagWP.at(0);
      } else {
        std::cerr << "Error with btag ALGORITHM_NAME:WP. Incorrect format." << std::endl;
        continue;
      }

      std::cout << "TopConfig ==========================================================> " << alg << ", " << tag <<
        std::endl;
      std::string formatedWP = FormatedWP(tag);
      std::pair<std::string, std::string> alg_tag = std::make_pair(alg, tag);
      // take care that no WP is taken twice
      if (std::find(m_chosen_btaggingWP.begin(), m_chosen_btaggingWP.end(), alg_tag) == m_chosen_btaggingWP.end()) {
        m_chosen_btaggingWP.push_back(alg_tag);
        std::cout << "chosen btag alg, WP  ===============================================> " << alg_tag.first <<
          ", " << alg_tag.second << std::endl;
      } else {
        std::cout << "alg, WP " << alg_tag.first << " " << alg_tag.second << " already choosen" << std::endl;
      }
    }

    m_btagging_calibration_B = settings->value("BTaggingCalibrationB");
    m_btagging_calibration_C = settings->value("BTaggingCalibrationC");
    m_btagging_calibration_Light = settings->value("BTaggingCalibrationLight");
    m_bTagSystsExcludedFromEV = settings->value("BTaggingSystExcludedFromEV");

    // Set translatio ndictionary for MCMC maps
    if (settings->value("RedifineMCMCMap") != " ") {
      std::vector<std::string> tmp;
      tokenize(settings->value("RedifineMCMCMap"), tmp, ",");
      for (const std::string& dictionaries : tmp) {
        std::vector<std::string> dictionary;
        tokenize(dictionaries, dictionary, ":");
        if (dictionary.size() != 2) {
          throw std::invalid_argument{"Wrong input argument for RedifineMCMCMap. Expected format is: \"shower1:shower2,shower3:shower4\""};
        }

        m_showerMCMCtranslator.insert({dictionary.at(0), dictionary.at(1)});
      }
    }

    /************************************************************
     *
     * Loop through all suplied config files and make into a
     * vector if we have at least one config and at least one
     * lumi calc file (and is MC) then apply PU reweighting
     * Note: PathResolving done in TopCPTools.
     *
     ***********************************************************/

    std::istringstream pileup_config_ss(settings->value("PRWConfigFiles"));
    std::copy(std::istream_iterator<std::string>(pileup_config_ss),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_pileup_reweighting.config_files));

    std::istringstream pileup_lumi_ss(settings->value("PRWLumiCalcFiles"));
    std::copy(std::istream_iterator<std::string>(pileup_lumi_ss),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_pileup_reweighting.lumi_calc_files));

    std::istringstream pileup_config_FS_ss(settings->value("PRWConfigFiles_FS"));
    std::copy(std::istream_iterator<std::string>(pileup_config_FS_ss),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_pileup_reweighting.config_files_FS));

    std::istringstream pileup_config_AF_ss(settings->value("PRWConfigFiles_AF"));
    std::copy(std::istream_iterator<std::string>(pileup_config_AF_ss),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_pileup_reweighting.config_files_AF));

    std::istringstream actual_mu_FS_ss(settings->value("PRWActualMu_FS"));
    std::copy(std::istream_iterator<std::string>(actual_mu_FS_ss),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_pileup_reweighting.actual_mu_FS));

    std::istringstream actual_mu_AF_ss(settings->value("PRWActualMu_AF"));
    std::copy(std::istream_iterator<std::string>(actual_mu_AF_ss),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_pileup_reweighting.actual_mu_AF));

    m_pileup_reweighting.unrepresented_data_tol = std::stof(settings->value("PRWUnrepresentedDataTolerance"));

    m_pileup_reweighting.mu_dependent = (settings->value("PRWMuDependent") == "True");

    // now even if the user don't provide a PRWConfigFiles, PRW is done on MC, using the default calibration file
    if (m_pileup_reweighting.lumi_calc_files.size() > 0) {
      m_pileup_reweighting.apply = true;
    }

    if (m_pileup_reweighting.apply && m_grlFile.size() > 0 &&
        settings->value("PRWUseGRLTool") == "True") m_pileup_reweighting.use_grl_tool = true;

    // if provided, using custom Data Scale-Factors for pile-up reweighting
    // nominal:up:down
    // also allowing to specify under this form: 1/1.2:1.0:1/1.4
    if (m_pileup_reweighting.apply && settings->value("PRWCustomScaleFactor") != " ") {
      std::vector<std::string> SFs_tokens;
      tokenize(settings->value("PRWCustomScaleFactor"), SFs_tokens, ":");
      if (SFs_tokens.size() != 3) throw std::runtime_error {
                "TopConfig: Option PRWCustomScaleFactor should be of the form \'nominal:up:down\'"
        };
      try {
        std::vector<std::string> nomSF_tokens;
        tokenize(SFs_tokens[0], nomSF_tokens, "/");
        if (nomSF_tokens.size() == 1) m_pileup_reweighting.custom_SF.push_back(std::stof(nomSF_tokens[0]));
        else if (nomSF_tokens.size() == 2) m_pileup_reweighting.custom_SF.push_back(std::stof(
                                                                                      nomSF_tokens[0]) /
                                                                                    std::stof(nomSF_tokens[1]));
        else throw std::runtime_error {
                  "TopConfig: issue in division"
          };
      } catch (...) {
        throw std::runtime_error {
                "TopConfig: can't convert provided PRW nominal Data SF into float"
        };
      }
      try {
        std::vector<std::string> upSF_tokens;
        tokenize(SFs_tokens[1], upSF_tokens, "/");
        if (upSF_tokens.size() == 1) m_pileup_reweighting.custom_SF.push_back(std::stof(upSF_tokens[0]));
        else if (upSF_tokens.size() == 2) m_pileup_reweighting.custom_SF.push_back(std::stof(
                                                                                     upSF_tokens[0]) /
                                                                                   std::stof(upSF_tokens[1]));
        else throw std::runtime_error {
                  "TopConfig: issue in division"
          };
      } catch (...) {
        throw std::runtime_error {
                "TopConfig: can't convert provided PRW up Data SF into float"
        };
      }
      try {
        std::vector<std::string> downSF_tokens;
        tokenize(SFs_tokens[2], downSF_tokens, "/");
        if (downSF_tokens.size() == 1) m_pileup_reweighting.custom_SF.push_back(std::stof(downSF_tokens[0]));
        else if (downSF_tokens.size() == 2) m_pileup_reweighting.custom_SF.push_back(std::stof(
                                                                                       downSF_tokens[0]) /
                                                                                     std::stof(downSF_tokens[1]));
        else throw std::runtime_error {
                  "TopConfig: issue in division"
          };
      } catch (...) {
        throw std::runtime_error {
                "TopConfig: can't convert provided PRW down Data SF into float"
        };
      }
      std::cout << "Custom PRW scale-factors - nominal:" << SFs_tokens[0] << "=" << m_pileup_reweighting.custom_SF[0] <<
        " up:" << SFs_tokens[1] << "=" << m_pileup_reweighting.custom_SF[1] << " down:" << SFs_tokens[2] << "=" <<
        m_pileup_reweighting.custom_SF[2] << std::endl;
    }

    if (m_pileup_reweighting.apply && settings->value("PRWPeriodAssignments") != " ") {
      std::vector<std::string> period_tokens;
      tokenize(settings->value("PRWPeriodAssignments"), period_tokens, ":");
      if (period_tokens.size() % 3 != 0) {
        throw std::invalid_argument(
                "TopConfig: Option PRWPeriodAssignments requires values in the form of \'value:value:value\'. The number of values needs to be divisible by 3.");
      }
      try {
        for (const std::string& per : period_tokens) {
          m_pileup_reweighting.periodAssignments.emplace_back(std::stoi(per));
        }
      } catch (...) {
        throw std::invalid_argument(
                "TopConfig: Cannot convert the strings into integers for the run numbers in Option PRWPeriodAssignments");
      }
    }

    // TRUTH derivations do not contain pile-up weights
    if (m_isTruthDxAOD) m_pileup_reweighting.apply = false;

    /************************************************************
    *
    * Muon trigger SF configuration
    * see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15#How_to_retrieve_the_SF
    * for the various trigger strings allowed
    *
    ************************************************************/

    m_muon_trigger_SF = settings->value("MuonTriggerSF");

    if (settings->value("DemandPrimaryVertex") == "False")
      m_demandPriVtx = false;

    ///-- KLFitter settings --///
    m_KLFitterTransferFunctionsPath = settings->value("KLFitterTransferFunctionsPath");
    m_KLFitterOutput = settings->value("KLFitterOutput");
    m_KLFitterJetSelectionMode = settings->value("KLFitterJetSelectionMode");
    m_KLFitterBTaggingMethod = settings->value("KLFitterBTaggingMethod");
    m_KLFitterLH = settings->value("KLFitterLH");

    if (settings->value("KLFitterTopMassFixed") == "True") m_KLFitterTopMassFixed = true;
    if (settings->value("KLFitterTopMassFixed") == "False") m_KLFitterTopMassFixed = false;
    if (settings->value("KLFitterSaveAllPermutations") == "True") m_KLFitterSaveAllPermutations = true;
    if (settings->value("KLFitterSaveAllPermutations") == "False") m_KLFitterSaveAllPermutations = false;
    if (settings->value("KLFitterFailOnLessThanXJets") == "True") m_KLFitterFailOnLessThanXJets = true;
    if (settings->value("KLFitterFailOnLessThanXJets") == "False") m_KLFitterFailOnLessThanXJets = false;

    //--- Check for configuration on the global lepton triggers ---//
    if (settings->value("UseGlobalLeptonTriggerSF") == "True") {
      auto parseTriggerString =
        [settings](std::unordered_map<std::string, std::vector<std::string> >& triggersByPeriod,
                   std::string const& key) {
          /* parse a string of the form "2015@triggerfoo,triggerbar,... 2016@triggerfoo,triggerbaz,... ..." */
          std::unordered_map<std::string, std::vector<std::string> > result;
          std::vector<std::string> pairs;
          boost::split(pairs, settings->value(key), boost::is_any_of(" "));
          for (std::string const& pair : pairs) {
            if (pair.empty() || pair == "None") continue;
            auto i = pair.find('@');
            if (!(i != std::string::npos &&
                  pair.find('@', i + 1) == std::string::npos)) throw std::invalid_argument(
                      std::string() + "Malformed trigger list in configuration item `" + key + "'");
            auto&& period = pair.substr(0, i), triggerstr = pair.substr(i + 1);
            auto&& triggers = result[period];
            if (!triggers.empty()) throw std::invalid_argument(
                      std::string() + "Period `" + period + "' appears multiple times in configuration item `" + key +
                      "'");
            boost::split(triggers, triggerstr, boost::is_any_of(","));
          }
          /* merge trigger map from this configuration line into triggersByPeriod */
          for (auto&& kv : result) {
            auto&& src = kv.second;
            auto&& dst = triggersByPeriod[kv.first];
            for (std::string const& trigger : src) {
              if (std::find(dst.begin(), dst.end(),
                            trigger) != dst.end()) throw std::invalid_argument(
                        std::string() + "Trigger `" + trigger + "' was specified multiple times");
              dst.push_back(trigger);
            }
          }
        };
      m_trigGlobalConfiguration.isActivated = true;
      parseTriggerString(m_trigGlobalConfiguration.trigger, "ElectronTriggers");
      parseTriggerString(m_trigGlobalConfiguration.trigger_loose, "ElectronTriggersLoose");
      parseTriggerString(m_trigGlobalConfiguration.trigger, "MuonTriggers");
      parseTriggerString(m_trigGlobalConfiguration.trigger_loose, "MuonTriggersLoose");
      parseTriggerString(m_trigGlobalConfiguration.trigger, "GlobalTriggers");
      parseTriggerString(m_trigGlobalConfiguration.trigger_loose, "GlobalTriggersLoose");
    }
  }

  float TopConfig::readFloatOption(top::ConfigurationSettings* const& settings, std::string instring) const {
    float invalue = 99999999.;

    try{
      invalue = std::stof(settings->value(instring));
    }
    catch (...) {
      throw std::invalid_argument {
              "TopConfig: cannot convert Option " + instring + "into float"
      };
    }

    return invalue;
  }

  void TopConfig::setGrlDir(const std::string& s) {
    if (!m_configFixed) {
      m_grlDir = s;
      m_grlDir.erase(remove_if(m_grlDir.begin(), m_grlDir.end(), isspace), m_grlDir.end());
    }
  }

  void TopConfig::setGrlFile(const std::string& s) {
    if (!m_configFixed) {
      std::istringstream grl_files_ss(s);
      std::vector<std::string> temp_grls = {};
      std::copy(std::istream_iterator<std::string>(grl_files_ss),
                std::istream_iterator<std::string>(),
                std::back_inserter(temp_grls));
      // m_grlFile = s;
      for (std::string grl : temp_grls) {
        grl.erase(remove_if(grl.begin(), grl.end(), isspace), grl.end());
        m_grlFile.push_back(grl);
      }
    }
  }

  void TopConfig::setTDPPath(const std::string& s) {
    if (!m_configFixed) {
      m_topDataPreparationPath = s;
    }
  }

  void TopConfig::jetUncertainties_NPModel(const std::string& s) {
    if (!m_configFixed) {
      m_jetUncertainties_NPModel = s;
      m_doMultipleJES = false;
      if (m_jetUncertainties_NPModel == "StrongReduction") {
        m_doMultipleJES = true;
      }
    }
  }

  void TopConfig::jetUncertainties_QGFracFile(const std::string& s) {
    if (!m_configFixed) {
      m_jetUncertainties_QGFracFile = s;
    }
  }

  void TopConfig::jetUncertainties_QGHistPatterns(const std::string& s) {
    if (!m_configFixed) {
      std::vector<std::string> outVector;
      if (s.find(" ") != std::string::npos) {
        throw std::runtime_error {
                "TopConfig: jetUncertainties_QGHistPatterns string can't contain white spaces"
        };
      }
      if (s != "None") {
        tokenize(s, outVector, ","); // list of DSIDs separated by commas
        if (outVector.size() != 1) // if size is !=1, we need to check if these are DSIDs
          for (auto s : outVector) {
            int i = std::atoi(s.c_str());
            if (i < 300000 || i >= 1000000) throw std::runtime_error {
                      "TopConfig: jetUncertainties_QGHistPatterns string doesn't look like a list of DISDs! You can either specify a single string pattern or a list of DSIDs separated by commas."
              };
          }
      }
      m_jetUncertainties_QGHistPatterns = outVector;
    }
  }

  void TopConfig::sgKeyMCParticle(const std::string& s) {
    if (!m_configFixed) {
      m_useTruthParticles = false;
      if (s != "None") m_useTruthParticles = true;

      m_sgKeyMCParticle = s;
    }
  }

  void TopConfig::sgKeyPhotons(const std::string& s) {
    if (!m_configFixed) {
      m_usePhotons = false;
      if (s != "None") m_usePhotons = true;

      m_sgKeyPhotons = s;
    }
  }

  void TopConfig::sgKeyElectrons(const std::string& s) {
    if (!m_configFixed) {
      m_useElectrons = false;
      if (s != "None") m_useElectrons = true;

      m_sgKeyElectrons = s;
    }
  }

  void TopConfig::sgKeyFwdElectrons(const std::string& s) {
    if (!m_configFixed) {
      m_useFwdElectrons = false;
      if (s != "None") m_useFwdElectrons = true;

      m_sgKeyFwdElectrons = s;
    }
  }

  void TopConfig::sgKeyMuons(const std::string& s) {
    if (!m_configFixed) {
      m_useMuons = false;
      if (s != "None") m_useMuons = true;

      m_sgKeyMuons = s;
    }
  }

  void TopConfig::sgKeySoftMuons(const std::string& s) {
    if (!m_configFixed) {
      m_useSoftMuons = false;
      if (s != "None") m_useSoftMuons = true;

      m_sgKeySoftMuons = s;
    }
  }

  void TopConfig::sgKeyTaus(const std::string& s) {
    if (!m_configFixed) {
      m_useTaus = false;
      if (s != "None") m_useTaus = true;

      m_sgKeyTaus = s;
    }
  }

  void TopConfig::sgKeyJets(const std::string& s) {
    if (!m_configFixed) {
      m_useJets = false;
      if (s != "None") m_useJets = true;

      size_t delim_pos = s.find('_');
      // for b-tagging shallow copies, e,g.
      // AntiKt4EMPFlowJets_BTagging20181003
      // we want to have quick access  to base collection name
      m_sgKeyJetsType = s.substr(0, delim_pos);

      // If anti-kt4 pflow jets then...
      if (m_sgKeyJetsType == "AntiKt4EMPFlowJets") m_useParticleFlowJets = true;

      m_sgKeyJets = s;
    }
  }

  void TopConfig::sgKeyLargeRJets(const std::string& s) {
    if (!m_configFixed) {
      m_useLargeRJets = false;
      if (s != "None") m_useLargeRJets = true;

      m_sgKeyLargeRJets = s;
    }
  }

  void TopConfig::sgKeyTrackJets(const std::string& s) {
    if (!m_configFixed) {
      m_useTrackJets = false;
      if (s != "None") m_useTrackJets = true;

      m_sgKeyTrackJets = s;
    }
  }

  void TopConfig::sgKeyTruthElectrons(const std::string& s) {
    if (!m_configFixed) {
      m_useTruthElectrons = false;
      if (s != "None") m_useTruthElectrons = true;

      m_sgKeyTruthElectrons = s;
    }
  }

  void TopConfig::sgKeyTruthMuons(const std::string& s) {
    if (!m_configFixed) {
      m_useTruthMuons = false;
      if (s != "None") m_useTruthMuons = true;

      m_sgKeyTruthMuons = s;
    }
  }

  void TopConfig::sgKeyTruthPhotons(const std::string& s) {
    if (!m_configFixed) {
      m_useTruthPhotons = false;
      if (s != "None") {
        m_useTruthPhotons = true;
      }

      m_sgKeyTruthPhotons = s;
    }
  }

  void TopConfig::sgKeyTruthMET(const std::string& s) {
    if (!m_configFixed) {
      m_useTruthMET = false;
      if (s != "None") m_useTruthMET = true;

      m_sgKeyTruthMET = s;
    }
  }

  void TopConfig::sgKeyTruthJets(const std::string& s) {
    if (!m_configFixed) {
      m_useTruthJets = false;
      if (s != "None") m_useTruthJets = true;

      m_sgKeyTruthJets = s;
    }
  }

  void TopConfig::sgKeyTruthLargeRJets(const std::string& s) {
    if (!m_configFixed) {
      m_useTruthLargeRJets = false;
      if (s != "None") m_useTruthLargeRJets = true;

      m_sgKeyTruthLargeRJets = s;
    }
  }

  void TopConfig::sgKeyTruthTaus(const std::string& s) {
    if (!m_configFixed) {
      m_useTruthTaus = false;
      if (s != "None") m_useTruthTaus = true;

      m_sgKeyTruthTaus = s;
    }
  }

  void TopConfig::decoKeyJetGhostTrack(const std::string& key) {
    if (!m_configFixed) {
      m_useJetGhostTrack = false;
      if (key != "None") m_useJetGhostTrack = true;

      m_decoKeyJetGhostTrack = key;
    }
  }

  // setting the run periods for ghost track
  // even if configuration is fixed - could be changed later
  void TopConfig::runPeriodJetGhostTrack(const std::vector<std::uint32_t>& vect) {
    if (m_useJetGhostTrack == true) m_jetGhostTrackRunPeriods = vect;
  }

  void TopConfig::setBTaggingSFSysts(std::string WP, const std::set<std::string>& btagging_SF_names, bool isTrackJet) {
    //this avoids code duplication
    std::unordered_map<std::string,
                       std::set<std::string> >& base_names = isTrackJet ? bTag_base_names_trkJet : bTag_base_names;
    std::unordered_map<std::string,
                       std::set<std::string> >& named_systs = isTrackJet ? bTag_named_systs_trkJet : bTag_named_systs;
    std::unordered_map<std::string, unsigned int>& eigen_B = isTrackJet ? bTag_eigen_B_trkJet : bTag_eigen_B;
    std::unordered_map<std::string, unsigned int>& eigen_C = isTrackJet ? bTag_eigen_C_trkJet : bTag_eigen_C;
    std::unordered_map<std::string,
                       unsigned int>& eigen_light = isTrackJet ? bTag_eigen_light_trkJet : bTag_eigen_light;

    //names of all systematics
    base_names[WP] = btagging_SF_names;
    //initialise named systematics to empty set
    named_systs[WP] = { };
    // initialise number of eigenvectors
    eigen_B[WP] = 0;
    eigen_C[WP] = 0;
    eigen_light[WP] = 0;
    // loop on systematic names
    for (const auto& SF_name : base_names.at(WP)) {
      if (SF_name.find("_Eigen_B_") != std::string::npos) eigen_B.at(WP)++;
      else if (SF_name.find("_Eigen_C_") != std::string::npos) eigen_C.at(WP)++;
      else if (SF_name.find("_Eigen_Light_") != std::string::npos) eigen_light.at(WP)++;
      else named_systs[WP].insert(SF_name);
    }
  }

  void TopConfig::setCalibBoostedJetTagger(const std::string& WP, const std::string& SFname) {
    m_boostedTaggerSFnames[WP] = SFname;
  }

  std::string TopConfig::FormatedWP(std::string raw_WP) {
    // just to have some backward compatibility...
    if (raw_WP == "60%") return "FixedCutBEff_60";
    else if (raw_WP == "70%") return "FixedCutBEff_70";
    else if (raw_WP == "77%") return "FixedCutBEff_77";
    else if (raw_WP == "85%") return "FixedCutBEff_85";
    else return raw_WP;
  }

  void TopConfig::setBTagWP_available(std::string btagging_WP) {
    m_available_btaggingWP.push_back(btagging_WP);
  }

  void TopConfig::setBTagWP_available_trkJet(std::string btagging_WP) {
    m_available_btaggingWP_trkJet.push_back(btagging_WP);
  }

  void TopConfig::setBTagWP_calibrated(std::string btagging_WP) {
    m_calibrated_btaggingWP.push_back(btagging_WP);
  }

  void TopConfig::setBTagWP_calibrated_trkJet(std::string btagging_WP) {
    m_calibrated_btaggingWP_trkJet.push_back(btagging_WP);
  }

  void TopConfig::addLHAPDFResult(const std::string& pdf_name,
                                  std::vector<float> sum_of_weights) {
    m_lhapdf_options.pdf_results[ pdf_name ] = sum_of_weights;
  }

  void TopConfig::outputEvents(const std::string& s) {
    if (!m_configFixed) {
      m_outputEvents = s;
      if (m_outputEvents == "AllEvents") {
        m_saveOnlySelectedEvents = false;
      }
    }
  }

  /// syst helper functions
  bool TopConfig::isSystNominal(const std::string& syst) const {
    return boost::iequals(syst, m_nominalSystName);
  }

  bool TopConfig::isSystAll(const std::string& syst) const {
    return boost::iequals(syst, m_allSystName);
  }

  bool TopConfig::getSystematicsList(const std::string& syststr, std::set<std::string>& systList) const {
    if (isSystAll(syststr) || isSystNominal(syststr)) return true;

    if (syststr.find(" ") != std::string::npos) {
      std::cout << "ERROR getSystematicsList: systematic string can't contain white spaces" << std::endl;
      return false;
    }

    std::string separator = ",";
    std::string::size_type start = 0, end = 0;
    while ((end = syststr.find(separator, start)) != std::string::npos) {
      std::string token = syststr.substr(start, end - start);
      if (token.size()) systList.insert(token);
      start = end + 1;
    }
    std::string lasttoken = syststr.substr(start);
    if (lasttoken.size()) systList.insert(lasttoken);

    return true;
  }

  bool TopConfig::contains(const std::set<std::string>& systList, const std::string& name) const {
    return systList.find(name) != systList.end();
  }

  // set systematics
  void TopConfig::systematicsPhotons(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      for (auto s : syst) {
        m_systHashPhotons->insert(s.hash());
        m_list_systHashAll->push_back(s.hash());
        m_systMapPhotons->insert(std::make_pair(s.hash(), s));
        m_systSgKeyMapPhotons->insert(std::make_pair(s.hash(), m_sgKeyPhotons + "_" + s.name()));
      }
      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
    }
  }

  void TopConfig::systematicsElectrons(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      for (auto s : syst) {
        m_systHashElectrons->insert(s.hash());
        m_list_systHashAll->push_back(s.hash());
        m_list_systHash_electronInJetSubtraction->push_back(s.hash());
        m_systMapElectrons->insert(std::make_pair(s.hash(), s));
        m_systSgKeyMapElectrons->insert(std::make_pair(s.hash(), m_sgKeyElectrons + "_" + s.name()));
      }
      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
      m_list_systHash_electronInJetSubtraction->sort();
      m_list_systHash_electronInJetSubtraction->unique();
    }
  }

  void TopConfig::systematicsFwdElectrons(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      for (const auto& s : syst) {	
        m_systHashFwdElectrons->insert(s.hash());
        m_list_systHashAll->push_back(s.hash());
        m_systMapFwdElectrons->insert(std::make_pair(s.hash(), s));
        m_systSgKeyMapFwdElectrons->insert(std::make_pair(s.hash(), m_sgKeyFwdElectrons + "_" + s.name()));
      }
      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
    }
  }

  void TopConfig::systematicsMuons(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      for (auto s : syst) {	
        m_systHashMuons->insert(s.hash());
        m_list_systHashAll->push_back(s.hash());
        m_systMapMuons->insert(std::make_pair(s.hash(), s));
        m_systSgKeyMapMuons->insert(std::make_pair(s.hash(), m_sgKeyMuons + "_" + s.name()));
      }
      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
    }
  }

  void TopConfig::systematicsSoftMuons(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      for (const CP::SystematicSet& s : syst) {	
        m_systHashSoftMuons->insert(s.hash());
        m_list_systHashAll->push_back(s.hash());
        m_systMapSoftMuons->insert(std::make_pair(s.hash(), s));
        m_systSgKeyMapSoftMuons->insert(std::make_pair(s.hash(), m_sgKeySoftMuons + "_" + s.name()));
      }
      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
    }
  }

  void TopConfig::systematicsTaus(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      for (auto s : syst) {	
        m_systHashTaus->insert(s.hash());
        m_list_systHashAll->push_back(s.hash());
        m_systMapTaus->insert(std::make_pair(s.hash(), s));
        m_systSgKeyMapTaus->insert(std::make_pair(s.hash(), m_sgKeyTaus + "_" + s.name()));
      }
      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
    }
  }

  void TopConfig::systematicsJets(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      for (auto s : syst) {	
        m_systHashJets->insert(s.hash());
        m_list_systHashAll->push_back(s.hash());
        m_list_systHash_electronInJetSubtraction->push_back(s.hash());
        m_systMapJets->insert(std::make_pair(s.hash(), s));
        m_systSgKeyMapJets->insert(std::make_pair(s.hash(), m_sgKeyJets + "_" + s.name()));
      }
      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
      m_list_systHash_electronInJetSubtraction->sort();
      m_list_systHash_electronInJetSubtraction->unique();
    }
  }

  void TopConfig::systematicsLargeRJets(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      for (auto s : syst) {	
        m_systHashLargeRJets->insert(s.hash());
        m_list_systHashAll->push_back(s.hash());
        m_systMapLargeRJets->insert(std::make_pair(s.hash(), s));
        m_systSgKeyMapLargeRJets->insert(std::make_pair(s.hash(), m_sgKeyLargeRJets + "_" + s.name()));
      }
      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
    }
  }

  void TopConfig::systematicsTrackJets(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      for (auto s : syst) {
	m_systHashTrackJets->insert(s.hash());
        m_list_systHashAll->push_back(s.hash());
        m_systMapTrackJets->insert(std::make_pair(s.hash(), s));
        m_systSgKeyMapTrackJets->insert(std::make_pair(s.hash(), m_sgKeyTrackJets + "_" + s.name()));
      }
      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
    }
  }

  void TopConfig::systematicsMET(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      for (auto s : syst) {	
        m_systHashMET->insert(s.hash());
        m_list_systHashAll->push_back(s.hash());
        m_systMapMET->insert(std::make_pair(s.hash(), s));
        m_systSgKeyMapMET->insert(std::make_pair(s.hash(), m_sgKeyLargeRJets + "_" + s.name()));
      }
      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
    }
  }

  void TopConfig::systematicsJetGhostTrack(const std::list<CP::SystematicSet>& syst) {
    if (!m_configFixed) {
      // Add the nominal (for reporting purposes).
      (*m_systMapJetGhostTrack)[m_nominalHashValue] = {};
      (*m_systDecoKeyMapJetGhostTrack)[m_nominalHashValue] = m_decoKeyJetGhostTrack + "_";
      m_jetGhostTrackSystematics.push_back("");

      for (auto s : syst) {	
        (*m_systMapJetGhostTrack)[s.hash()] = s;
        (*m_systDecoKeyMapJetGhostTrack)[s.hash()] = m_decoKeyJetGhostTrack + "_" + s.name();
        m_list_systHashAll->push_back(s.hash());
        m_jetGhostTrackSystematics.push_back(s.name());
      }

      // Ensure that elements are unique.
      std::sort(m_jetGhostTrackSystematics.begin(),
                m_jetGhostTrackSystematics.end());
      auto last = std::unique(m_jetGhostTrackSystematics.begin(),
                              m_jetGhostTrackSystematics.end());
      m_jetGhostTrackSystematics.erase(last,
                                       m_jetGhostTrackSystematics.end());

      m_list_systHashAll->sort();
      m_list_systHashAll->unique();
    }
  }

  void TopConfig::fixConfiguration() {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "TopConfig::fixConfiguration()" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    // Prevent the user from changing anything
    // Yes, this is deliberate
    m_configFixed = true;

    // copy hash values in m_systHashAll_list
    // into m_systHashAll
    // The std::unordered_set is faster
    for (std::list<std::size_t>::const_iterator i = m_list_systHashAll->begin(); i != m_list_systHashAll->end(); ++i) {
      m_systHashAll->insert((*i));
    }
    // same for m_systHash_electronInJetSubtraction
    for (std::list<std::size_t>::const_iterator i = m_list_systHash_electronInJetSubtraction->begin();
         i != m_list_systHash_electronInJetSubtraction->end(); ++i) {
      m_systHash_electronInJetSubtraction->insert((*i));
    }


    // Setup StoreGate keys for TopEvent/SingleSystEvent
    // Let's do ALL string manipulation here, never do it per event
    std::string nominalPhotons("SetMe");
    std::string nominalElectrons("SetMe");
    std::string nominalFwdElectrons("SetMe");
    std::string nominalMuons("SetMe");
    std::string nominalSoftMuons("SetMe");
    std::string nominalTaus("SetMe");
    std::string nominalJets("SetMe");
    std::string nominalLargeRJets("SetMe");
    std::string nominalTrackJets("SetMe");
    std::string nominal("nominal");
    std::string tds("TDS");
    std::string tdsAux("TDSAux.");

    // Figure out the nominal object collection names
    std::unordered_map<std::size_t, std::string>::const_iterator ph = m_systSgKeyMapPhotons->find(m_nominalHashValue);
    if (ph != m_systSgKeyMapPhotons->end()) {
      nominalPhotons = (*ph).second;
    }

    std::unordered_map<std::size_t, std::string>::const_iterator el = m_systSgKeyMapElectrons->find(m_nominalHashValue);
    if (el != m_systSgKeyMapElectrons->end()) {
      nominalElectrons = (*el).second;
    }

    std::unordered_map<std::size_t, std::string>::const_iterator fwdel = m_systSgKeyMapFwdElectrons->find(
      m_nominalHashValue);
    if (fwdel != m_systSgKeyMapFwdElectrons->end()) {
      nominalFwdElectrons = (*fwdel).second;
    }

    std::unordered_map<std::size_t, std::string>::const_iterator mu = m_systSgKeyMapMuons->find(m_nominalHashValue);
    if (mu != m_systSgKeyMapMuons->end()) {
      nominalMuons = (*mu).second;
    }

    std::unordered_map<std::size_t, std::string>::const_iterator softmu = m_systSgKeyMapSoftMuons->find(
      m_nominalHashValue);
    if (softmu != m_systSgKeyMapSoftMuons->end()) {
      nominalSoftMuons = (*softmu).second;
    }

    std::unordered_map<std::size_t, std::string>::const_iterator tau = m_systSgKeyMapTaus->find(m_nominalHashValue);
    if (tau != m_systSgKeyMapTaus->end()) {
      nominalTaus = (*tau).second;
    }

    std::unordered_map<std::size_t, std::string>::const_iterator jet = m_systSgKeyMapJets->find(m_nominalHashValue);
    if (jet != m_systSgKeyMapJets->end()) {
      nominalJets = (*jet).second;
    }

    std::unordered_map<std::size_t, std::string>::const_iterator largeRJet = m_systSgKeyMapLargeRJets->find(
      m_nominalHashValue);
    if (largeRJet != m_systSgKeyMapLargeRJets->end()) {
      nominalLargeRJets = (*largeRJet).second;
    }

    std::unordered_map<std::size_t, std::string>::const_iterator trackJet = m_systSgKeyMapTrackJets->find(
      m_nominalHashValue);
    if (trackJet != m_systSgKeyMapTrackJets->end()) {
      nominalTrackJets = (*trackJet).second;
    }

    for (std::unordered_set<std::size_t>::const_iterator i = m_systHashAll->begin(); i != m_systHashAll->end(); ++i) {
      std::unordered_map<std::size_t, std::string>::const_iterator ph = m_systSgKeyMapPhotons->find(*i);
      if (ph != m_systSgKeyMapPhotons->end()) {
        m_systAllSgKeyMapPhotons->insert(std::make_pair((*i), (*ph).second));
      }
      if (ph == m_systSgKeyMapPhotons->end()) {
        m_systAllSgKeyMapPhotons->insert(std::make_pair((*i), nominalPhotons));
      }

      std::unordered_map<std::size_t, std::string>::const_iterator el = m_systSgKeyMapElectrons->find(*i);
      if (el != m_systSgKeyMapElectrons->end()) {
        m_systAllSgKeyMapElectrons->insert(std::make_pair((*i), (*el).second));
      }
      if (el == m_systSgKeyMapElectrons->end()) {
        m_systAllSgKeyMapElectrons->insert(std::make_pair((*i), nominalElectrons));
      }

      std::unordered_map<std::size_t, std::string>::const_iterator fwdel = m_systSgKeyMapFwdElectrons->find(*i);
      if (fwdel != m_systSgKeyMapFwdElectrons->end()) {
        m_systAllSgKeyMapFwdElectrons->insert(std::make_pair((*i), (*fwdel).second));
      }
      if (fwdel == m_systSgKeyMapFwdElectrons->end()) {
        m_systAllSgKeyMapFwdElectrons->insert(std::make_pair((*i), nominalFwdElectrons));
      }

      std::unordered_map<std::size_t, std::string>::const_iterator mu = m_systSgKeyMapMuons->find(*i);
      if (mu != m_systSgKeyMapMuons->end()) {
        m_systAllSgKeyMapMuons->insert(std::make_pair((*i), (*mu).second));
      }
      if (mu == m_systSgKeyMapMuons->end()) {
        m_systAllSgKeyMapMuons->insert(std::make_pair((*i), nominalMuons));
      }

      std::unordered_map<std::size_t, std::string>::const_iterator softmu = m_systSgKeyMapSoftMuons->find(*i);
      if (softmu != m_systSgKeyMapSoftMuons->end()) {
        m_systAllSgKeyMapSoftMuons->insert(std::make_pair((*i), (*softmu).second));
      }
      if (softmu == m_systSgKeyMapSoftMuons->end()) {
        m_systAllSgKeyMapSoftMuons->insert(std::make_pair((*i), nominalSoftMuons));
      }

      std::unordered_map<std::size_t, std::string>::const_iterator tau = m_systSgKeyMapTaus->find(*i);
      if (tau != m_systSgKeyMapTaus->end()) {
        m_systAllSgKeyMapTaus->insert(std::make_pair((*i), (*tau).second));
      }
      if (tau == m_systSgKeyMapTaus->end()) {
        m_systAllSgKeyMapTaus->insert(std::make_pair((*i), nominalTaus));
      }

      std::unordered_map<std::size_t, std::string>::const_iterator jet = m_systSgKeyMapJets->find(*i);
      if (jet != m_systSgKeyMapJets->end()) {
        m_systAllSgKeyMapJets->insert(std::make_pair((*i), (*jet).second));
      }
      if (jet == m_systSgKeyMapJets->end()) {
        m_systAllSgKeyMapJets->insert(std::make_pair((*i), nominalJets));
      }

      std::unordered_map<std::size_t, std::string>::const_iterator largeRJet = m_systSgKeyMapLargeRJets->find(*i);
      if (largeRJet != m_systSgKeyMapLargeRJets->end()) {
        m_systAllSgKeyMapLargeRJets->insert(std::make_pair((*i), (*largeRJet).second));
      }
      if (largeRJet == m_systSgKeyMapLargeRJets->end()) {
        m_systAllSgKeyMapLargeRJets->insert(std::make_pair((*i), nominalLargeRJets));
      }

      std::unordered_map<std::size_t, std::string>::const_iterator trackJet = m_systSgKeyMapTrackJets->find(*i);
      if (trackJet != m_systSgKeyMapTrackJets->end()) {
        m_systAllSgKeyMapTrackJets->insert(std::make_pair((*i), (*trackJet).second));
      }
      if (trackJet == m_systSgKeyMapTrackJets->end()) {
        m_systAllSgKeyMapTrackJets->insert(std::make_pair((*i), nominalTrackJets));
      }
    } // Loop over all systematic hash values


    if (m_applyElectronInJetSubtraction) {
      // Loop over all systematic hash values again, this time for electronInJetSubtraction
      std::string electronInJetSubtraction_Electrons("electronInJetSubtraction_Electrons_USING_");
      std::string electronInJetSubtraction_Jets("electronInJetSubtraction_Jets_USING_");
      std::string electronInJetSubtraction_JetsLoose("electronInJetSubtraction_JetsLoose_USING_");
      std::string electronInJetSubtraction_nominalElectrons = electronInJetSubtraction_Electrons + nominalElectrons +
                                                              "_AND_" + nominalJets;
      std::string electronInJetSubtraction_nominalJets = electronInJetSubtraction_Jets + nominalElectrons + "_AND_" +
                                                         nominalJets;
      std::string electronInJetSubtraction_nominalJetsLoose = electronInJetSubtraction_JetsLoose + nominalElectrons +
                                                              "_AND_" + nominalJets;

      for (std::unordered_set<std::size_t>::const_iterator i = m_systHashAll->begin(); i != m_systHashAll->end(); ++i) {
        std::unordered_map<std::size_t, std::string>::const_iterator el = m_systSgKeyMapElectrons->find(*i);
        std::unordered_map<std::size_t, std::string>::const_iterator jet = m_systSgKeyMapJets->find(*i);

        if (el != m_systSgKeyMapElectrons->end()) {
          m_systAllSgKeyMapElectrons_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                     electronInJetSubtraction_Electrons
                                                                                     + (*el).second + "_AND_" +
          nominalJets));
          m_systAllSgKeyMapJets_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                electronInJetSubtraction_Jets +
                                                                                (*el).second + "_AND_" + nominalJets));
          m_systSgKeyMapJets_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                             electronInJetSubtraction_Jets +
                                                                             (*el).second + "_AND_" + nominalJets));

          m_systAllSgKeyMapJetsLoose_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                     electronInJetSubtraction_JetsLoose
                                                                                     + (*el).second + "_AND_" +
          nominalJets));
          m_systSgKeyMapJetsLoose_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                  electronInJetSubtraction_JetsLoose +
                                                                                  (*el).second + "_AND_" +
          nominalJets));
        }
        if (jet != m_systSgKeyMapJets->end()) {
          m_systAllSgKeyMapElectrons_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                     electronInJetSubtraction_Electrons
                                                                                     + nominalElectrons + "_AND_" +
          (*jet).second));
          m_systAllSgKeyMapJets_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                electronInJetSubtraction_Jets +
                                                                                nominalElectrons + "_AND_" +
          (*jet).second));
          m_systSgKeyMapJets_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                             electronInJetSubtraction_Jets +
                                                                             nominalElectrons + "_AND_" +
          (*jet).second));

          m_systAllSgKeyMapJetsLoose_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                     electronInJetSubtraction_JetsLoose
                                                                                     + nominalElectrons + "_AND_" +
          (*jet).second));
          m_systSgKeyMapJetsLoose_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                  electronInJetSubtraction_JetsLoose +
                                                                                  nominalElectrons + "_AND_" +
          (*jet).second));
        }
        if (el == m_systSgKeyMapElectrons->end() && jet == m_systSgKeyMapJets->end()) {
          m_systAllSgKeyMapElectrons_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                     electronInJetSubtraction_nominalElectrons));
          m_systAllSgKeyMapJets_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                electronInJetSubtraction_nominalJets));
          m_systAllSgKeyMapJetsLoose_electronInJetSubtraction->insert(std::make_pair((*i),
                                                                                     electronInJetSubtraction_nominalJetsLoose));
        }
      } // Loop over all systematic hash values
    } // electron in jet Subtraction


    // Tempory data store names
    typedef std::unordered_map<std::size_t, std::string>::const_iterator Itr2;
    for (Itr2 i = m_systAllSgKeyMapPhotons->begin(); i != m_systAllSgKeyMapPhotons->end(); ++i) {
      m_systAllSgKeyMapPhotonsTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapPhotonsTDSAux->insert(std::make_pair((*i).first, (*i).second + tdsAux));
    }

    for (Itr2 i = m_systAllSgKeyMapElectrons->begin(); i != m_systAllSgKeyMapElectrons->end(); ++i) {
      m_systAllSgKeyMapElectronsTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapElectronsTDSAux->insert(std::make_pair((*i).first, (*i).second + tdsAux));
    }

    for (const auto& i : *(m_systAllSgKeyMapFwdElectrons)) {
      m_systAllSgKeyMapFwdElectronsTDS->insert(std::make_pair(i.first, i.second + tds));
      m_systAllSgKeyMapFwdElectronsTDSAux->insert(std::make_pair(i.first, i.second + tdsAux));
    }

    for (Itr2 i = m_systAllSgKeyMapMuons->begin(); i != m_systAllSgKeyMapMuons->end(); ++i) {
      m_systAllSgKeyMapMuonsTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapMuonsTDSAux->insert(std::make_pair((*i).first, (*i).second + tdsAux));
    }

    for (Itr2 i = m_systAllSgKeyMapSoftMuons->begin(); i != m_systAllSgKeyMapSoftMuons->end(); ++i) {
      m_systAllSgKeyMapSoftMuonsTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapSoftMuonsTDSAux->insert(std::make_pair((*i).first, (*i).second + tdsAux));
    }

    for (Itr2 i = m_systAllSgKeyMapTaus->begin(); i != m_systAllSgKeyMapTaus->end(); ++i) {
      m_systAllSgKeyMapTausTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapTausTDSAux->insert(std::make_pair((*i).first, (*i).second + tdsAux));
    }

    for (Itr2 i = m_systAllSgKeyMapJets->begin(); i != m_systAllSgKeyMapJets->end(); ++i) {
      m_systAllSgKeyMapJetsTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapJetsTDSAux->insert(std::make_pair((*i).first, (*i).second + tdsAux));
    }

    for (Itr2 i = m_systAllSgKeyMapLargeRJets->begin(); i != m_systAllSgKeyMapLargeRJets->end(); ++i) {
      m_systAllSgKeyMapLargeRJetsTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapLargeRJetsTDSAux->insert(std::make_pair((*i).first, (*i).second + tdsAux));
    }

    for (Itr2 i = m_systAllSgKeyMapElectrons_electronInJetSubtraction->begin();
         i != m_systAllSgKeyMapElectrons_electronInJetSubtraction->end(); ++i) {
      m_systAllSgKeyMapElectrons_electronInJetSubtractionTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapElectrons_electronInJetSubtractionTDSAux->insert(std::make_pair((*i).first,
                                                                                       (*i).second + tdsAux));
    }

    for (Itr2 i = m_systAllSgKeyMapTrackJets->begin(); i != m_systAllSgKeyMapTrackJets->end(); ++i) {
      m_systAllSgKeyMapTrackJetsTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapTrackJetsTDSAux->insert(std::make_pair((*i).first, (*i).second + tdsAux));
    }

    for (Itr2 i = m_systAllSgKeyMapJets_electronInJetSubtraction->begin();
         i != m_systAllSgKeyMapJets_electronInJetSubtraction->end(); ++i) {
      m_systAllSgKeyMapJets_electronInJetSubtractionTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapJets_electronInJetSubtractionTDSAux->insert(std::make_pair((*i).first, (*i).second + tdsAux));
    }

    for (Itr2 i = m_systAllSgKeyMapJetsLoose_electronInJetSubtraction->begin();
         i != m_systAllSgKeyMapJetsLoose_electronInJetSubtraction->end(); ++i) {
      m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDS->insert(std::make_pair((*i).first, (*i).second + tds));
      m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDSAux->insert(std::make_pair((*i).first,
                                                                                       (*i).second + tdsAux));
    }


    // TTree names
    m_systAllTTreeNames->insert(std::make_pair(m_nominalHashValue, nominal));
    typedef std::unordered_map<std::size_t, CP::SystematicSet>::const_iterator Itr;

    if (m_usePhotons) {
      for (Itr i = m_systMapPhotons->begin(); i != m_systMapPhotons->end(); ++i) {
        m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
      }
    }
    if (m_useElectrons) {
      for (Itr i = m_systMapElectrons->begin(); i != m_systMapElectrons->end(); ++i) {
        m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
      }
    }
    if (m_useFwdElectrons) {
      for (Itr i = m_systMapFwdElectrons->begin(); i != m_systMapFwdElectrons->end(); ++i) {
        m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
      }
    }
    if (m_useMuons) {
      for (Itr i = m_systMapMuons->begin(); i != m_systMapMuons->end(); ++i) {
        m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
      }
    }
    if (m_useSoftMuons) {
      for (Itr i = m_systMapSoftMuons->begin(); i != m_systMapSoftMuons->end(); ++i) {
        m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
      }
    }
    if (m_useTaus) {
      for (Itr i = m_systMapTaus->begin(); i != m_systMapTaus->end(); ++i) {
        m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
      }
    }
    if (m_useJets) {
      for (Itr i = m_systMapJets->begin(); i != m_systMapJets->end(); ++i) {
        m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
      }
    }
    if (m_useLargeRJets) {
      for (Itr i = m_systMapLargeRJets->begin(); i != m_systMapLargeRJets->end(); ++i) {
        m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
      }
    }
    if (m_useTrackJets) {
      for (Itr i = m_systMapTrackJets->begin(); i != m_systMapTrackJets->end(); ++i) {
        m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
      }
    }
    if (m_useJetGhostTrack) {
      for (Itr i = m_systMapJetGhostTrack->begin(); i != m_systMapJetGhostTrack->end(); ++i) {
        m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
      }
    }
    for (Itr i = m_systMapMET->begin(); i != m_systMapMET->end(); ++i) {
      m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second.name()));
    }



    // Missing ET from TTree names
    // KLFitter from TTree names

    for (Itr2 i = m_systAllTTreeNames->begin(); i != m_systAllTTreeNames->end(); ++i) {
      if (m_doTightEvents) m_systSgKeyMapMissingET->insert(std::make_pair((*i).first,
                                                                          m_sgKeyMissingEt + "_" + (*i).second));
      if (m_doLooseEvents) m_systSgKeyMapMissingETLoose->insert(std::make_pair((*i).first,
                                                                               m_sgKeyMissingEt + "_Loose_" +
                                                                               (*i).second));

      if (m_doTightEvents) m_systSgKeyMapKLFitter->insert(std::make_pair((*i).first,
                                                                         m_sgKeyKLFitter + "_" + (*i).second));
      if (m_doLooseEvents) m_systSgKeyMapKLFitterLoose->insert(std::make_pair((*i).first,
                                                                              m_sgKeyKLFitter + "_Loose_" +
                                                                              (*i).second));

      if (m_doTightEvents) m_systSgKeyMapPseudoTop->insert(std::make_pair((*i).first,
                                                                          m_sgKeyPseudoTop + "_" + (*i).second));
      if (m_doLooseEvents) m_systSgKeyMapPseudoTopLoose->insert(std::make_pair((*i).first,
                                                                               m_sgKeyPseudoTop + "_Loose_" +
                                                                               (*i).second));
    }


    // TTree index
    unsigned int TTreeIndex(0);
    if (m_doTightEvents) {
      for (Itr2 i = m_systAllTTreeNames->begin(); i != m_systAllTTreeNames->end(); ++i) {
        if ((*i).second == "nominal" || m_doTightSysts) {
          m_systAllTTreeIndex->insert(std::make_pair((*i).first, TTreeIndex));
          ++TTreeIndex;
        }
      }
    }
    if (m_doLooseEvents) {
      for (Itr2 i = m_systAllTTreeNames->begin(); i != m_systAllTTreeNames->end(); ++i) {
        if ((*i).second == "nominal" || m_doLooseSysts) {
          m_systAllTTreeLooseIndex->insert(std::make_pair((*i).first, TTreeIndex));
          ++TTreeIndex;
        }
      }
    }

    if (m_makeAllCPTools) {
      for (Itr2 i = m_systAllTTreeNames->begin(); i != m_systAllTTreeNames->end(); ++i) {
        m_systPersistantAllTTreeNames->insert(std::make_pair((*i).first, (*i).second));
      }
    }
  }

  const std::string& TopConfig::sgKeyPhotons(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapPhotons->find(hash);
    if (key != m_systAllSgKeyMapPhotons->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyPhotonsTDS(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapPhotonsTDS->find(hash);
    if (key != m_systAllSgKeyMapPhotonsTDS->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyPhotonsTDSAux(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapPhotonsTDSAux->find(hash);
    if (key != m_systAllSgKeyMapPhotonsTDSAux->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyElectrons(const std::size_t hash) const {
    if (!m_applyElectronInJetSubtraction) {
      std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapElectrons->find(hash);
      if (key != m_systAllSgKeyMapElectrons->end()) {
        return (*key).second;
      }
    }

    if (m_applyElectronInJetSubtraction) {
      std::unordered_map<std::size_t,
                         std::string>::const_iterator key = m_systAllSgKeyMapElectrons_electronInJetSubtraction->find(
        hash);
      if (key != m_systAllSgKeyMapElectrons_electronInJetSubtraction->end()) {
        return (*key).second;
      }
    }

    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyElectronsTDS(const std::size_t hash) const {
    if (!m_applyElectronInJetSubtraction) {
      std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapElectronsTDS->find(hash);
      if (key != m_systAllSgKeyMapElectronsTDS->end()) {
        return (*key).second;
      }
    }

    if (m_applyElectronInJetSubtraction) {
      std::unordered_map<std::size_t,
                         std::string>::const_iterator key =
        m_systAllSgKeyMapElectrons_electronInJetSubtractionTDS->find(hash);
      if (key != m_systAllSgKeyMapElectrons_electronInJetSubtractionTDS->end()) {
        return (*key).second;
      }
    }

    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyElectronsTDSAux(const std::size_t hash) const {
    if (!m_applyElectronInJetSubtraction) {
      std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapElectronsTDSAux->find(hash);
      if (key != m_systAllSgKeyMapElectronsTDSAux->end()) {
        return (*key).second;
      }
    }

    if (m_applyElectronInJetSubtraction) {
      std::unordered_map<std::size_t,
                         std::string>::const_iterator key =
        m_systAllSgKeyMapElectrons_electronInJetSubtractionTDSAux->find(hash);
      if (key != m_systAllSgKeyMapElectrons_electronInJetSubtractionTDSAux->end()) {
        return (*key).second;
      }
    }

    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyElectronsStandAlone(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapElectrons->find(hash);
    if (key != m_systAllSgKeyMapElectrons->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyFwdElectrons(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapFwdElectrons->find(hash);
    if (key != m_systAllSgKeyMapFwdElectrons->end()) {
      return (*key).second;
    }

    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyFwdElectronsTDS(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapFwdElectronsTDS->find(hash);
    if (key != m_systAllSgKeyMapFwdElectronsTDS->end()) {
      return (*key).second;
    }

    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyFwdElectronsTDSAux(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapFwdElectronsTDSAux->find(hash);
    if (key != m_systAllSgKeyMapFwdElectronsTDSAux->end()) {
      return (*key).second;
    }

    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyFwdElectronsStandAlone(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapFwdElectrons->find(hash);
    if (key != m_systAllSgKeyMapFwdElectrons->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyMuons(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapMuons->find(hash);
    if (key != m_systAllSgKeyMapMuons->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyMuonsTDS(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapMuonsTDS->find(hash);
    if (key != m_systAllSgKeyMapMuonsTDS->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyMuonsTDSAux(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapMuonsTDSAux->find(hash);
    if (key != m_systAllSgKeyMapMuonsTDSAux->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeySoftMuons(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapSoftMuons->find(hash);
    if (key != m_systAllSgKeyMapSoftMuons->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeySoftMuonsTDS(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapSoftMuonsTDS->find(hash);
    if (key != m_systAllSgKeyMapSoftMuonsTDS->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeySoftMuonsTDSAux(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapSoftMuonsTDSAux->find(hash);
    if (key != m_systAllSgKeyMapSoftMuonsTDSAux->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyTaus(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapTaus->find(hash);
    if (key != m_systAllSgKeyMapTaus->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyTausTDS(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapTausTDS->find(hash);
    if (key != m_systAllSgKeyMapTausTDS->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyTausTDSAux(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapTausTDSAux->find(hash);
    if (key != m_systAllSgKeyMapTausTDSAux->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyJets(const std::size_t hash, const bool useLooseLeptonJets) const {
    if (!m_applyElectronInJetSubtraction) {
      std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapJets->find(hash);
      if (key != m_systAllSgKeyMapJets->end()) {
        return (*key).second;
      }
    }

    if (m_applyElectronInJetSubtraction) {
      if (!useLooseLeptonJets) {
        std::unordered_map<std::size_t,
                           std::string>::const_iterator key =
          m_systAllSgKeyMapJets_electronInJetSubtraction->find(hash);
        if (key != m_systAllSgKeyMapJets_electronInJetSubtraction->end()) {
          return (*key).second;
        }
      }
      if (useLooseLeptonJets) {
        std::unordered_map<std::size_t,
                           std::string>::const_iterator key = m_systAllSgKeyMapJetsLoose_electronInJetSubtraction->find(
          hash);
        if (key != m_systAllSgKeyMapJetsLoose_electronInJetSubtraction->end()) {
          return (*key).second;
        }
      }
    }

    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyJetsTDS(const std::size_t hash, const bool useLooseLeptonJets) const {
    if (!m_applyElectronInJetSubtraction) {
      std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapJetsTDS->find(hash);
      if (key != m_systAllSgKeyMapJetsTDS->end()) {
        return (*key).second;
      }
    }

    if (m_applyElectronInJetSubtraction) {
      if (!useLooseLeptonJets) {
        std::unordered_map<std::size_t,
                           std::string>::const_iterator key = m_systAllSgKeyMapJets_electronInJetSubtractionTDS->find(
          hash);
        if (key != m_systAllSgKeyMapJets_electronInJetSubtractionTDS->end()) {
          return (*key).second;
        }
      }
      if (useLooseLeptonJets) {
        std::unordered_map<std::size_t,
                           std::string>::const_iterator key =
          m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDS->find(hash);
        if (key != m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDS->end()) {
          return (*key).second;
        }
      }
    }

    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyJetsTDSAux(const std::size_t hash, const bool useLooseLeptonJets) const {
    if (!m_applyElectronInJetSubtraction) {
      std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapJetsTDSAux->find(hash);
      if (key != m_systAllSgKeyMapJetsTDSAux->end()) {
        return (*key).second;
      }
    }

    if (m_applyElectronInJetSubtraction) {
      if (!useLooseLeptonJets) {
        std::unordered_map<std::size_t,
                           std::string>::const_iterator key =
          m_systAllSgKeyMapJets_electronInJetSubtractionTDSAux->find(hash);
        if (key != m_systAllSgKeyMapJets_electronInJetSubtractionTDSAux->end()) {
          return (*key).second;
        }
      }
      if (useLooseLeptonJets) {
        std::unordered_map<std::size_t,
                           std::string>::const_iterator key =
          m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDSAux->find(hash);
        if (key != m_systAllSgKeyMapJetsLoose_electronInJetSubtractionTDSAux->end()) {
          return (*key).second;
        }
      }
    }

    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyJetsStandAlone(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapJets->find(hash);
    if (key != m_systAllSgKeyMapJets->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyLargeRJets(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapLargeRJets->find(hash);
    if (key != m_systAllSgKeyMapLargeRJets->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyLargeRJetsTDS(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapLargeRJetsTDS->find(hash);
    if (key != m_systAllSgKeyMapLargeRJetsTDS->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyLargeRJetsTDSAux(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapLargeRJetsTDSAux->find(hash);
    if (key != m_systAllSgKeyMapLargeRJetsTDSAux->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyTrackJets(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapTrackJets->find(hash);
    if (key != m_systAllSgKeyMapTrackJets->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyTrackJetsTDS(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapTrackJetsTDS->find(hash);
    if (key != m_systAllSgKeyMapTrackJetsTDS->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyTrackJetsTDSAux(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systAllSgKeyMapTrackJetsTDSAux->find(hash);
    if (key != m_systAllSgKeyMapTrackJetsTDSAux->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyMissingEt(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systSgKeyMapMissingET->find(hash);
    if (key != m_systSgKeyMapMissingET->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyMissingEtLoose(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systSgKeyMapMissingETLoose->find(hash);
    if (key != m_systSgKeyMapMissingETLoose->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyKLFitter(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systSgKeyMapKLFitter->find(hash);
    if (key != m_systSgKeyMapKLFitter->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyKLFitterLoose(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systSgKeyMapKLFitterLoose->find(hash);
    if (key != m_systSgKeyMapKLFitterLoose->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyPseudoTop(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systSgKeyMapPseudoTop->find(hash);
    if (key != m_systSgKeyMapPseudoTop->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::sgKeyPseudoTopLoose(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systSgKeyMapPseudoTopLoose->find(hash);
    if (key != m_systSgKeyMapPseudoTopLoose->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  const std::string& TopConfig::decoKeyJetGhostTrack(const std::size_t hash) const {
    // If it's in the map, use the mapped value, otherwise use the nominal.
    auto it = m_systDecoKeyMapJetGhostTrack->find(hash);

    if (it != m_systDecoKeyMapJetGhostTrack->end()) {
      return it->second;
    } else {
      it = m_systDecoKeyMapJetGhostTrack->find(m_nominalHashValue);
      if (it == m_systDecoKeyMapJetGhostTrack->end()) throw std::runtime_error(
                "TopConfig: Failed to retrieve decoKeyJetGhostTrack.");
      return it->second;
    }
  }

  const std::string& TopConfig::systematicName(const std::size_t hash) const {
    std::unordered_map<std::size_t, std::string>::const_iterator key = m_systPersistantAllTTreeNames
                                                                        ->find(hash);
    if (key != m_systPersistantAllTTreeNames->end()) {
      return (*key).second;
    }
    return m_sgKeyDummy;
  }

  std::shared_ptr<std::unordered_map<std::size_t, std::string> > TopConfig::systSgKeyMapJets(
    const bool useLooseLeptonJets) const {
    if (m_applyElectronInJetSubtraction) {
      if (useLooseLeptonJets) {
        return m_systSgKeyMapJetsLoose_electronInJetSubtraction;
      }
      return m_systSgKeyMapJets_electronInJetSubtraction;
    }
    return m_systSgKeyMapJets;
  }

  // TTree index
  unsigned int TopConfig::ttreeIndex(const std::size_t hash) const {
    unsigned int index(99999); // This will crash the job if not set properly

    std::unordered_map<std::size_t, unsigned int>::const_iterator Itr = m_systAllTTreeIndex->find(hash);
    if (Itr != m_systAllTTreeIndex->end()) {
      index = (*Itr).second;
    }
    if (index == 99999) {
      std::cout << "ttreeIndex is crazy, something has gone wrong with the hash value = " << hash << std::endl;
    }
    return index;
  }

  unsigned int TopConfig::ttreeIndexLoose(const std::size_t hash) const {
    unsigned int index(99999); // This will crash the job if not set properly

    std::unordered_map<std::size_t, unsigned int>::const_iterator Itr = m_systAllTTreeLooseIndex->find(hash);
    if (Itr != m_systAllTTreeLooseIndex->end()) {
      index = (*Itr).second;
    }
    if (index == 99999) {
      std::cout << "ttreeIndex is crazy, something has gone wrong with the hash value = " << hash << std::endl;
    }
    return index;
  }

  const std::vector<std::string>& TopConfig::allTriggers_Tight(const std::string& selection) const {
    std::unordered_map<std::string,
                       std::vector<std::string> >::const_iterator key = m_allTriggers_Tight->find(selection);
    if (key != m_allTriggers_Tight->end()) {
      return (*key).second;
    }
    return m_dummyTrigger;
  }

  const std::vector<std::string>& TopConfig::electronTriggers_Tight(const std::string& selection) const {
    std::unordered_map<std::string, std::vector<std::string> >::const_iterator key = m_electronTriggers_Tight->find(
      selection);
    if (key != m_electronTriggers_Tight->end()) {
      return (*key).second;
    }
    return m_dummyTrigger;
  }

  const std::vector<std::string>& TopConfig::muonTriggers_Tight(const std::string& selection) const {
    std::unordered_map<std::string, std::vector<std::string> >::const_iterator key = m_muonTriggers_Tight->find(
      selection);
    if (key != m_muonTriggers_Tight->end()) {
      return (*key).second;
    }
    return m_dummyTrigger;
  }

  const std::vector<std::string>& TopConfig::tauTriggers_Tight(const std::string& selection) const {
    std::unordered_map<std::string,
                       std::vector<std::string> >::const_iterator key = m_tauTriggers_Tight->find(selection);
    if (key != m_tauTriggers_Tight->end()) {
      return (*key).second;
    }
    return m_dummyTrigger;
  }

  const std::vector<std::string>& TopConfig::allTriggers_Loose(const std::string& selection) const {
    std::unordered_map<std::string,
                       std::vector<std::string> >::const_iterator key = m_allTriggers_Loose->find(selection);
    if (key != m_allTriggers_Loose->end()) {
      return (*key).second;
    }
    return m_dummyTrigger;
  }

  const std::vector<std::string>& TopConfig::electronTriggers_Loose(const std::string& selection) const {
    std::unordered_map<std::string, std::vector<std::string> >::const_iterator key = m_electronTriggers_Loose->find(
      selection);
    if (key != m_electronTriggers_Loose->end()) {
      return (*key).second;
    }
    return m_dummyTrigger;
  }

  const std::vector<std::string>& TopConfig::muonTriggers_Loose(const std::string& selection) const {
    std::unordered_map<std::string, std::vector<std::string> >::const_iterator key = m_muonTriggers_Loose->find(
      selection);
    if (key != m_muonTriggers_Loose->end()) {
      return (*key).second;
    }
    return m_dummyTrigger;
  }

  const std::vector<std::string>& TopConfig::tauTriggers_Loose(const std::string& selection) const {
    std::unordered_map<std::string,
                       std::vector<std::string> >::const_iterator key = m_tauTriggers_Loose->find(selection);
    if (key != m_tauTriggers_Loose->end()) {
      return (*key).second;
    }
    return m_dummyTrigger;
  }

  ///-- top::TopPersistentSettings allows us to take a snapshot of top::TopConfig --///
  ///-- and save it to a mini-xAOD. We can then re-create the top::TopConfig from file --///
  top::TopPersistentSettings* TopConfig::createPersistentSettings() {
    top::TopPersistentSettings* out = new top::TopPersistentSettings();

    out->m_isMC = m_isMC;
    out->m_isAFII = m_isAFII;
    out->m_applyElectronInJetSubtraction = m_applyElectronInJetSubtraction;
    out->m_doOverlapRemovalOnLooseLeptonDef = m_doOverlapRemovalOnLooseLeptonDef;
    out->m_doKLFitter = m_doKLFitter;
    out->m_doPseudoTop = m_doPseudoTop;

    out->m_sgKeyTopSystematicEvents = m_sgKeyTopSystematicEvents;
    out->m_sgKeyTopSystematicEventsLoose = m_sgKeyTopSystematicEventsLoose;

    out->m_sgKeyEventInfo = m_sgKeyEventInfo;
    out->m_sgKeyPrimaryVertices = m_sgKeyPrimaryVertices;
    out->m_sgKeyPhotons = m_sgKeyPhotons;
    out->m_sgKeyElectrons = m_sgKeyElectrons;
    out->m_sgKeyFwdElectrons = m_sgKeyFwdElectrons;
    out->m_sgKeyMuons = m_sgKeyMuons;
    out->m_sgKeySoftMuons = m_sgKeySoftMuons;
    out->m_sgKeyTaus = m_sgKeyTaus;
    out->m_sgKeyJets = m_sgKeyJets;
    out->m_sgKeyLargeRJets = m_sgKeyLargeRJets;
    out->m_sgKeyTrackJets = m_sgKeyTrackJets;
    out->m_sgKeyMissingEt = m_sgKeyMissingEt;
    out->m_sgKeyMissingEtLoose = m_sgKeyMissingEtLoose;

    out->m_electronID = m_electronID;
    out->m_electronIDLoose = m_electronIDLoose;
    out->m_electronIsolation = m_electronIsolation;
    out->m_electronIsolationLoose = m_electronIsolationLoose;
    out->m_useElectronChargeIDSelection = m_useElectronChargeIDSelection;
    out->m_useEgammaLeakageCorrection = m_useEgammaLeakageCorrection;

    out->m_fwdElectronID = m_fwdElectronID;

    out->m_muon_trigger_SF = m_muon_trigger_SF;
    out->m_muonQuality = m_muonQuality;
    out->m_muonQualityLoose = m_muonQualityLoose;
    out->m_muonIsolation = m_muonIsolation;
    out->m_muonIsolationLoose = m_muonIsolationLoose;

    out->m_softmuonQuality = m_softmuonQuality;

    typedef std::unordered_map<std::size_t, std::string>::const_iterator Itr;

    for (std::vector<std::pair<std::string, std::string> >::const_iterator i = m_chosen_btaggingWP.begin();
         i != m_chosen_btaggingWP.end(); ++i)
      out->m_chosen_btaggingWP.push_back(*i);

    for (Itr i = m_systSgKeyMapPhotons->begin(); i != m_systSgKeyMapPhotons->end(); ++i)
      out->m_systSgKeyMapPhotons.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapElectrons->begin(); i != m_systSgKeyMapElectrons->end(); ++i)
      out->m_systSgKeyMapElectrons.insert(std::make_pair((*i).first, (*i).second));

    for (const auto& i : *(m_systSgKeyMapFwdElectrons))
      out->m_systSgKeyMapFwdElectrons.insert(std::make_pair(i.first, i.second));

    for (Itr i = m_systSgKeyMapMuons->begin(); i != m_systSgKeyMapMuons->end(); ++i)
      out->m_systSgKeyMapMuons.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapSoftMuons->begin(); i != m_systSgKeyMapSoftMuons->end(); ++i)
      out->m_systSgKeyMapSoftMuons.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapTaus->begin(); i != m_systSgKeyMapTaus->end(); ++i)
      out->m_systSgKeyMapTaus.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapJets->begin(); i != m_systSgKeyMapJets->end(); ++i)
      out->m_systSgKeyMapJets.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapJets_electronInJetSubtraction->begin();
         i != m_systSgKeyMapJets_electronInJetSubtraction->end(); ++i)
      out->m_systSgKeyMapJets_electronInJetSubtraction.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapJetsLoose_electronInJetSubtraction->begin();
         i != m_systSgKeyMapJetsLoose_electronInJetSubtraction->end(); ++i)
      out->m_systSgKeyMapJetsLoose_electronInJetSubtraction.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapLargeRJets->begin(); i != m_systSgKeyMapLargeRJets->end(); ++i)
      out->m_systSgKeyMapLargeRJets.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapTrackJets->begin(); i != m_systSgKeyMapTrackJets->end(); ++i)
      out->m_systSgKeyMapTrackJets.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapMissingET->begin(); i != m_systSgKeyMapMissingET->end(); ++i)
      out->m_systSgKeyMapMissingET.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapMissingETLoose->begin(); i != m_systSgKeyMapMissingETLoose->end(); ++i)
      out->m_systSgKeyMapMissingETLoose.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapKLFitter->begin(); i != m_systSgKeyMapKLFitter->end(); ++i)
      out->m_systSgKeyMapKLFitter.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapKLFitterLoose->begin(); i != m_systSgKeyMapKLFitterLoose->end(); ++i)
      out->m_systSgKeyMapKLFitterLoose.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapPseudoTop->begin(); i != m_systSgKeyMapPseudoTop->end(); ++i)
      out->m_systSgKeyMapPseudoTop.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systSgKeyMapPseudoTopLoose->begin(); i != m_systSgKeyMapPseudoTopLoose->end(); ++i)
      out->m_systSgKeyMapPseudoTopLoose.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systDecoKeyMapJetGhostTrack->begin(); i != m_systDecoKeyMapJetGhostTrack->end(); ++i)
      out->m_systDecoKeyMapJetGhostTrack.insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = m_systPersistantAllTTreeNames->begin(); i != m_systPersistantAllTTreeNames->end(); ++i)
      out->m_systPersistantAllTTreeNames.insert(std::make_pair((*i).first, (*i).second));

    for (std::list<std::size_t>::const_iterator i = m_list_systHashAll->begin(); i != m_list_systHashAll->end(); ++i)
      out->m_list_systHashAll.push_back(*i);

    for (std::list<std::size_t>::const_iterator i = m_list_systHash_electronInJetSubtraction->begin();
         i != m_list_systHash_electronInJetSubtraction->end(); ++i)
      out->m_list_systHash_electronInJetSubtraction.push_back(*i);

    for (std::vector<std::string>::const_iterator i = m_allSelectionNames->begin(); i != m_allSelectionNames->end();
         ++i)
      out->m_allSelectionNames.push_back(*i);

    for (auto i : *m_allTriggers_Tight) {
      out->m_allTriggers_Tight.insert(i);
    }
    for (auto i : *m_electronTriggers_Tight) {
      out->m_electronTriggers_Tight.insert(i);
    }
    for (auto i : *m_muonTriggers_Tight) {
      out->m_tauTriggers_Tight.insert(i);
    }

    for (auto i : *m_allTriggers_Loose) {
      out->m_allTriggers_Loose.insert(i);
    }
    for (auto i : *m_electronTriggers_Loose) {
      out->m_electronTriggers_Loose.insert(i);
    }
    for (auto i : *m_muonTriggers_Loose) {
      out->m_muonTriggers_Loose.insert(i);
    }
    for (auto i : *m_tauTriggers_Loose) {
      out->m_tauTriggers_Loose.insert(i);
    }

    return out;
  }

  TopConfig::TopConfig(const top::TopPersistentSettings* settings) :
    // delegate to the default ctor - this is a nice C++11 feature, John likes this a lot
    TopConfig() {
    m_makeAllCPTools = false;
    m_isMC = settings->m_isMC;
    m_isAFII = settings->m_isAFII;
    m_applyElectronInJetSubtraction = settings->m_applyElectronInJetSubtraction;
    m_doOverlapRemovalOnLooseLeptonDef = settings->m_doOverlapRemovalOnLooseLeptonDef;
    m_doKLFitter = settings->m_doKLFitter;
    m_doPseudoTop = settings->m_doPseudoTop;

    m_sgKeyTopSystematicEvents = settings->m_sgKeyTopSystematicEvents;
    m_sgKeyTopSystematicEventsLoose = settings->m_sgKeyTopSystematicEventsLoose;

    m_sgKeyEventInfo = settings->m_sgKeyEventInfo;
    m_sgKeyPrimaryVertices = settings->m_sgKeyPrimaryVertices;
    sgKeyPhotons(settings->m_sgKeyPhotons);
    sgKeyElectrons(settings->m_sgKeyElectrons);
    sgKeyFwdElectrons(settings->m_sgKeyFwdElectrons);
    sgKeyMuons(settings->m_sgKeyMuons);
    sgKeySoftMuons(settings->m_sgKeySoftMuons);
    sgKeyTaus(settings->m_sgKeyTaus);
    sgKeyJets(settings->m_sgKeyJets);
    sgKeyLargeRJets(settings->m_sgKeyLargeRJets);
    sgKeyTrackJets(settings->m_sgKeyTrackJets);
    m_sgKeyMissingEt = settings->m_sgKeyMissingEt;
    m_sgKeyMissingEtLoose = settings->m_sgKeyMissingEtLoose;

    m_electronID = settings->m_electronID;
    m_electronIDLoose = settings->m_electronIDLoose;
    m_electronIsolation = settings->m_electronIsolation;
    m_electronIsolationLoose = settings->m_electronIsolationLoose;
    m_useElectronChargeIDSelection = settings->m_useElectronChargeIDSelection;
    m_useEgammaLeakageCorrection = settings->m_useEgammaLeakageCorrection;

    m_fwdElectronID = settings->m_fwdElectronID;

    m_muon_trigger_SF = settings->m_muon_trigger_SF;
    m_muonQuality = settings->m_muonQuality;
    m_muonQualityLoose = settings->m_muonQualityLoose;
    m_muonIsolation = settings->m_muonIsolation;
    m_muonIsolationLoose = settings->m_muonIsolationLoose;

    m_softmuonQuality = settings->m_softmuonQuality;

    for (std::vector<std::pair<std::string, std::string> >::const_iterator i = settings->m_chosen_btaggingWP.begin();
         i != settings->m_chosen_btaggingWP.end(); ++i)
      m_chosen_btaggingWP.push_back(*i);

    typedef std::map<std::size_t, std::string>::const_iterator Itr;

    for (Itr i = settings->m_systSgKeyMapPhotons.begin(); i != settings->m_systSgKeyMapPhotons.end(); ++i)
      m_systSgKeyMapPhotons->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapElectrons.begin(); i != settings->m_systSgKeyMapElectrons.end(); ++i)
      m_systSgKeyMapElectrons->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapFwdElectrons.begin(); i != settings->m_systSgKeyMapFwdElectrons.end(); ++i)
      m_systSgKeyMapFwdElectrons->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapMuons.begin(); i != settings->m_systSgKeyMapMuons.end(); ++i)
      m_systSgKeyMapMuons->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapSoftMuons.begin(); i != settings->m_systSgKeyMapSoftMuons.end(); ++i)
      m_systSgKeyMapSoftMuons->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapTaus.begin(); i != settings->m_systSgKeyMapTaus.end(); ++i)
      m_systSgKeyMapTaus->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapJets.begin(); i != settings->m_systSgKeyMapJets.end(); ++i)
      m_systSgKeyMapJets->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapJets_electronInJetSubtraction.begin();
         i != settings->m_systSgKeyMapJets_electronInJetSubtraction.end(); ++i)
      m_systSgKeyMapJets_electronInJetSubtraction->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapJetsLoose_electronInJetSubtraction.begin();
         i != settings->m_systSgKeyMapJetsLoose_electronInJetSubtraction.end(); ++i)
      m_systSgKeyMapJetsLoose_electronInJetSubtraction->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapLargeRJets.begin(); i != settings->m_systSgKeyMapLargeRJets.end(); ++i)
      m_systSgKeyMapLargeRJets->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapTrackJets.begin(); i != settings->m_systSgKeyMapTrackJets.end(); ++i)
      m_systSgKeyMapTrackJets->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapMissingET.begin(); i != settings->m_systSgKeyMapMissingET.end(); ++i)
      m_systSgKeyMapMissingET->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapMissingETLoose.begin(); i != settings->m_systSgKeyMapMissingETLoose.end(); ++i)
      m_systSgKeyMapMissingETLoose->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapKLFitter.begin(); i != settings->m_systSgKeyMapKLFitter.end(); ++i)
      m_systSgKeyMapKLFitter->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapKLFitterLoose.begin(); i != settings->m_systSgKeyMapKLFitterLoose.end(); ++i)
      m_systSgKeyMapKLFitterLoose->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapPseudoTop.begin(); i != settings->m_systSgKeyMapPseudoTop.end(); ++i)
      m_systSgKeyMapPseudoTop->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systSgKeyMapPseudoTopLoose.begin(); i != settings->m_systSgKeyMapPseudoTopLoose.end(); ++i)
      m_systSgKeyMapPseudoTopLoose->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systDecoKeyMapJetGhostTrack.begin(); i != settings->m_systDecoKeyMapJetGhostTrack.end();
         ++i)
      m_systDecoKeyMapJetGhostTrack->insert(std::make_pair((*i).first, (*i).second));

    for (Itr i = settings->m_systPersistantAllTTreeNames.begin(); i != settings->m_systPersistantAllTTreeNames.end();
         ++i) {
      m_systPersistantAllTTreeNames->insert(std::make_pair((*i).first, (*i).second));
      m_systAllTTreeNames->insert(std::make_pair((*i).first, (*i).second));
    }

    for (std::vector<std::size_t>::const_iterator i = settings->m_list_systHashAll.begin();
         i != settings->m_list_systHashAll.end(); ++i)
      m_list_systHashAll->push_back(*i);

    for (std::vector<std::size_t>::const_iterator i = settings->m_list_systHash_electronInJetSubtraction.begin();
         i != settings->m_list_systHash_electronInJetSubtraction.end(); ++i)
      m_list_systHash_electronInJetSubtraction->push_back(*i);

    for (std::vector<std::string>::const_iterator i = settings->m_allSelectionNames.begin();
         i != settings->m_allSelectionNames.end(); ++i)
      m_allSelectionNames->push_back(*i);

    m_allTriggers_Tight = std::make_shared<std::unordered_map<std::string, std::vector<std::string> > >();
    m_electronTriggers_Tight = std::make_shared<std::unordered_map<std::string, std::vector<std::string> > >();
    m_muonTriggers_Tight = std::make_shared<std::unordered_map<std::string, std::vector<std::string> > >();
    m_tauTriggers_Tight = std::make_shared<std::unordered_map<std::string, std::vector<std::string> > >();

    m_allTriggers_Loose = std::make_shared<std::unordered_map<std::string, std::vector<std::string> > >();
    m_electronTriggers_Loose = std::make_shared<std::unordered_map<std::string, std::vector<std::string> > >();
    m_muonTriggers_Loose = std::make_shared<std::unordered_map<std::string, std::vector<std::string> > >();
    m_tauTriggers_Loose = std::make_shared<std::unordered_map<std::string, std::vector<std::string> > >();

    for (auto i : settings->m_allTriggers_Tight) {
      m_allTriggers_Tight->insert(i);
    }
    for (auto i : settings->m_electronTriggers_Tight) {
      m_electronTriggers_Tight->insert(i);
    }
    for (auto i : settings->m_muonTriggers_Tight) {
      m_muonTriggers_Tight->insert(i);
    }
    for (auto i : settings->m_tauTriggers_Tight) {
      m_tauTriggers_Tight->insert(i);
    }

    for (auto i : settings->m_allTriggers_Loose) {
      m_allTriggers_Loose->insert(i);
    }
    for (auto i : settings->m_electronTriggers_Loose) {
      m_electronTriggers_Loose->insert(i);
    }
    for (auto i : settings->m_muonTriggers_Loose) {
      m_muonTriggers_Loose->insert(i);
    }
    for (auto i : settings->m_tauTriggers_Loose) {
      m_tauTriggers_Loose->insert(i);
    }

    fixConfiguration();
  }

  AodMetaDataAccess& TopConfig::aodMetaData() {
    return *m_aodMetaData;
  }

  // Place into a private function to allow use without replication of code
  void TopConfig::ReadIsAFII(top::ConfigurationSettings* const& settings) {
    if (settings->value("IsAFII") == "True") this->setIsAFII(true);
    else if (settings->value("IsAFII") == "False") this->setIsAFII(false);
    else if (settings->value("IsAFII") != " ") throw std::runtime_error(
              "TopConfig: option IsAFII must be either True or False");
    else if (this->isMC()) throw std::runtime_error("TopConfig: option IsAFII not set");
  }

  // Function to set the release series (this method may change so refactor)
  void TopConfig::setReleaseSeries() {
    // Method taken from TopCPTools
    std::string release_series = "";
    const char* rel_temp = std::getenv("ROOTCORE_RELEASE_SERIES");
    if (rel_temp) {
      release_series = std::string(rel_temp);
    } else {
      release_series = "";
    }

    if (release_series == "23") {
      m_release_series = 23; // 2.3
    } else if (release_series == "24") {
      m_release_series = 24; // 2.4 R20.7
    } else if (release_series == "25") {
      m_release_series = 25; // 2.6 R21
    } else {
      // Default to R21 because this build has moved away from ROOTCORE
      m_release_series = 25;
    }
    return;
  }

  void TopConfig::setAmiTag(std::string const& amiTag) {
    assert(!m_configFixed);
    if (m_amiTagSet == 0) {
      m_amiTag = amiTag;
      m_amiTagSet = 1;
    } else if (m_amiTagSet > 0 && m_amiTag != amiTag) {
      m_amiTag.clear();
      m_amiTagSet = -1;
    }
  }

  std::string const& TopConfig::getAmiTag() const {
    assert(m_configFixed);
    return m_amiTag;
  }

  // Function to return the year of data taking based on either run number (data) or random run number (MC)
  const std::string TopConfig::getYear(unsigned int runnumber) {
    // 2015 : 266904 - 284484
    if (runnumber >= 266904 && runnumber <= 284484) return "2015";

    // 2016 : 296939 - 311481
    if (runnumber >= 296939 && runnumber <= 311481) return "2016";

    // 2017 : 324320 - 999999
    if (runnumber >= 324320) return "2017";

    return "ERROR";
  }

  void TopConfig::setGlobalTriggerConfiguration(std::vector<std::string> electron_trigger_systematics,
                                                std::vector<std::string> muon_trigger_systematics,
                                                std::vector<std::string> electron_tool_names,
                                                std::vector<std::string> muon_tool_names) {
    m_trigGlobalConfiguration.electron_trigger_systematics = electron_trigger_systematics;
    m_trigGlobalConfiguration.muon_trigger_systematics = muon_trigger_systematics;
    m_trigGlobalConfiguration.electron_trigger_tool_names = electron_tool_names;
    m_trigGlobalConfiguration.muon_trigger_tool_names = muon_tool_names;
    m_trigGlobalConfiguration.isConfigured = true;
    return;
  }
}



std::ostream& operator << (std::ostream& os, const top::TopConfig& config) {
  typedef std::shared_ptr<std::unordered_map<std::size_t, CP::SystematicSet> > map_t;
  typedef std::unordered_map<std::size_t, CP::SystematicSet>::const_iterator Itr;

  if (config.useJetGhostTrack()) {
    for (const auto& item : config.systematicsJetGhostTrack()) {
      os << " Jet Ghost Track Systematic\t\t :: " << item << " \n";
    }
  }

  os << "\n";
  os << "top::TopConfig has identified the following analysis release series : " << config.getReleaseSeries() << "\n";
  os << "top::TopConfig will evaluate the following systematics (saved as TTrees in your ntuple) \n";
  os << "A blank systematic means \"Nominal\" in xAOD. All Nominal calibrations go into the Nominal TTree. \n";
  os << "\n";
  if (config.usePhotons()) {
    map_t syst = config.systMapPhotons();
    for (Itr i = syst->begin(); i != syst->end(); ++i) {
      os << " Photon systematic\t :: " << (*i).second.name() << " \n";
    }
  }

  if (config.useElectrons()) {
    map_t syst = config.systMapElectrons();
    for (Itr i = syst->begin(); i != syst->end(); ++i) {
      os << " Electron systematic\t :: " << (*i).second.name() << " \n";
    }
  }

  if (config.useFwdElectrons()) {
    map_t syst = config.systMapFwdElectrons();
    for (Itr i = syst->begin(); i != syst->end(); ++i) {
      os << " Fwd Electron systematic\t :: " << (*i).second.name() << " \n";
    }
  }

  if (config.useMuons()) {
    map_t syst = config.systMapMuons();
    for (Itr i = syst->begin(); i != syst->end(); ++i) {
      os << " Muon systematic\t :: " << (*i).second.name() << " \n";
    }
  }

  if (config.useSoftMuons()) {
    map_t syst = config.systMapSoftMuons();
    for (Itr i = syst->begin(); i != syst->end(); ++i) {
      os << " Soft Muon systematic\t :: " << (*i).second.name() << " \n";
    }
  }

  if (config.useTaus()) {
    map_t syst = config.systMapTaus();
    for (Itr i = syst->begin(); i != syst->end(); ++i) {
      os << " Tau systematic\t :: " << (*i).second.name() << " \n";
    }
  }

  if (config.useJets()) {
    map_t syst = config.systMapJets();
    for (Itr i = syst->begin(); i != syst->end(); ++i) {
      os << " Jet systematic\t\t :: " << (*i).second.name() << " \n";
    }
  }

  if (config.useLargeRJets()) {
    map_t syst = config.systMapLargeRJets();
    for (Itr i = syst->begin(); i != syst->end(); ++i) {
      os << " Large-R Jet systematic\t\t :: " << (*i).second.name() << " \n";
    }
  }

  os << "\n";
  return os;
}
