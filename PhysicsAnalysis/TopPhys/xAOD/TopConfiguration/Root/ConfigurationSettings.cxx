/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopConfiguration/ConfigurationSettings.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>

namespace top {
  ConfigurationSettings* ConfigurationSettings::m_instance = 0;

  ConfigurationSettings::ConfigurationSettings() : m_configured(false) {
    registerParameter("ElectronCollectionName", "Name of the Electron container");
    registerParameter("FwdElectronCollectionName",
                      "Name of the Forward Electrons container, ForwardElectrons or None (default)", "None");
    registerParameter("MuonCollectionName", "Name of the Muon container");
    registerParameter("PhotonCollectionName", "Name of the Photon container");
    registerParameter("JetCollectionName", "Name of the Jet container");
    registerParameter("LargeJetCollectionName", "Name of the large-R Jet container");
    registerParameter("LargeJetSubstructure", "Setup to use when applying grooming on the large-R jet");
    registerParameter("TrackJetCollectionName", "Name of the track Jet container", "None");
    registerParameter("TauCollectionName", "Name of the Tau container");
    registerParameter("JetGhostTrackDecoName", "Name of the jet decoration that holds the ghost-associated track.",
                      "None");

    registerParameter("TruthCollectionName", "Name of the Truth container");
    registerParameter("TruthElectronCollectionName", "Name of the Truth Electron container", "TruthElectrons");
    registerParameter("TruthMuonCollectionName", "Name of the Truth Muon container", "TruthMuons");
    registerParameter("TruthPhotonCollectionName", "Name of the Truth Photon container", "None");
    registerParameter("TruthMETCollectionName", "Name of the Truth MET container", "MET_Truth");
    registerParameter("TruthJetCollectionName", "Name of the Truth Jet container");
    registerParameter("TruthLargeRJetCollectionName", "Name of the Truth Large R Jet container", "None");
    registerParameter("TruthTauCollectionName", "Name of the Truth tau container", "None");

    registerParameter("applyTTVACut", "Decide if to apply lepton z0/d0 cuts", "True");

    registerParameter("GRLDir", "Location of GRL File", "TopAnalysis");
    registerParameter("GRLFile", "Name of GRL File", " ");

    registerParameter("TDPPath", "Path to TopDataPreparation file (accessible via PathResolver)",
                      "dev/AnalysisTop/TopDataPreparation/XSection-MC16-13TeV.data");

    registerParameter("DumpBtagSystsInSystTrees",
                      "Dump systematics-shifted b-tagging SFs in systematic TTrees, True or False (default: False)",
                      "False");
    registerParameter("StorePerJetBtagSFs", "Store per-jet btag SFs", "False");

    registerParameter("ElectronID", "Type of electron. Likelihood LooseAndBLayerLH, MediumLH, TightLH", "TightLH");
    registerParameter("ElectronIDLoose",
                      "Type of electron for background. Likelihood LooseAndBLayerLH, MediumLH, TightLH", "MediumLH");
    registerParameter("ElectronPt", "Electron pT cut for object selection (in MeV). Default 25 GeV.", "25000.");
    registerParameter("EgammaSystematicModel", "Egamma Calibration Systematic model : FULL_v1 , 1NP_v1 (default)",
                      "1NP_v1");
    registerParameter("ElectronEfficiencySystematicModel",
                      "Electron Efficiency Systematic model : FULL, SIMPLIFIED, TOTAL (default)", "TOTAL");
    registerParameter("ElectronEfficiencySystematicModelEtaBinning",
                      "Electron Efficiency Systematic model eta binning (option for SIMPLIFIED model, do not specify to use default; format XXX:YYY:ZZZ, e.g. 0.0:1.37:4.9)",
                      "default");
    registerParameter("ElectronEfficiencySystematicModelEtBinning",
                      "Electron Efficiency Systematic model E_T binning (option for SIMPLIFIED model, do not specify to use default; format XXX:YYY:ZZZ. e.g. 4000:7000:10000:15000:13000000)",
                      "default");
    registerParameter("ElectronIsolation",
                      "Isolation to use : Gradient, FCLoose, FCTight, FCHighPtCaloOnly, (EXPERIMENTAL: HighPtCaloOnly, Loose, Tight, TightTrackOnly, TightTrackOnly_FixedRad, PLVTight, PLVLoose), (DANGEROUS: PflowTight, PflowLoose), None",
                      "Gradient");
    registerParameter("ElectronIsolationLoose",
                      "Isolation to use : Gradient, FCLoose, FCTight, FCHighPtCaloOnly, (EXPERIMENTAL: HighPtCaloOnly, Loose, Tight, TightTrackOnly, TightTrackOnly_FixedRad, PLVTight, PLVLoose), (DANGEROUS: PflowTight, PflowLoose), None",
                      "None");
    registerParameter("ElectronIsolationSF", "Force electron isolation SF (e.g. None). EXPERIMENTAL!", " ");
    registerParameter("ElectronIsolationSFLoose", "Force electron isolation SF (e.g. None). EXPERIMENTAL!", " ");
    registerParameter("ElectronVetoLArCrack", "True/False. Set to False to disable LAr crack veto (not recommended).",
                      "True");
    registerParameter("UseElectronChargeIDSelection",
                      "True/False. Switch on/off electron charge ID selection (Default False).", "False");
    registerParameter("UseEgammaLeakageCorrection",
                      "True/False. Switch on/off leakage correction -- REQUIRES ptag>p3947 (Default True).", "True");

    registerParameter("FwdElectronID", "Type of fwd electron. Loose, Medium, Tight (default)", "Tight");
    registerParameter("FwdElectronIDLoose", "Type of fwd loose electrons. Loose, Medium, Tight (default)", "Tight");
    registerParameter("FwdElectronPt", "Fwd Electron pT cut for object selection (in MeV). Default 25 GeV.", "25000.");
    registerParameter("FwdElectronMinEta", "Fwd Electron lower |eta| cut for object selection. Default 2.5", "2.5");
    registerParameter("FwdElectronMaxEta", "Fwd Electron upper |eta| cut for object selection. Default 4.9", "4.9");
    registerParameter("FwdElectronBCIDCleaningRunRange",
                      "Specify run range for which the BCID cleaning must be applied for fwd el. in data: \"XXX:YYY\"; the cleaning will be applied for XXX<=runNumber<=YYY",
                      "266904:311481");


    registerParameter("PhotonPt", "Photon pT cut for object selection (in MeV). Default 25 GeV.", "25000.");
    registerParameter("PhotonEta", "Absolute Photon eta cut for object selection. Default 2.5.", "2.5");
    registerParameter("PhotonID", "Type of photon. Definition to use : Tight, Loose and None.", "Tight");
    registerParameter("PhotonIDLoose", "Type of photon for background. Definition to use : Tight, Loose, None.",
                      "Loose");
    registerParameter("PhotonIsolation",
                      "Isolation to use : FixedCutTightCaloOnly, FixedCutTight, FixedCutLoose, (EXPERIMENTAL: TightCaloOnly, Tight, Loose), None.",
                      "FixedCutTight");
    registerParameter("PhotonIsolationLoose",
                      "Isolation to use : FixedCutTightCaloOnly, FixedCutTight, FixedCutLoose, (EXPERIMENTAL: TightCaloOnly, Tight, Loose), None.",
                      "FixedCutLoose");
    registerParameter("PhotonUseRadiativeZ", "True/False. Set to True to enable photon radiative Z up to 100 GeV.",
                      "False");

    registerParameter("MuonPt", "Muon pT cut for object selection (in MeV). Default 25 GeV.", "25000");
    registerParameter("MuonEta", "Absolute Muon eta cut for object selection. Default 2.5.", "2.5");
    registerParameter("MuonQuality",
                      "Muon quality cut for object selection. Options are VeryLoose, Loose, Medium (default) and Tight",
                      "Medium");
    registerParameter("MuonQualityLoose",
                      "Muon quality cut for object selection. Options are VeryLoose, Loose, Medium (default) and Tight",
                      "Medium");
    registerParameter("MuonIsolation",
                      "Isolation to use : FCTight, FCLoose, FCTightTrackOnly, FCTightTrackOnly_FixedRad, FCLoose_FixedRad, FCTight_FixedRad, FixedCutPflowTight, FixedCutPflowLoose, None",
                      "FCTight_FixedRad");
    registerParameter("MuonIsolationLoose",
                      "Isolation to use : Tight, Loose, LooseTrackOnly, FixedCutTight, FixedCutTightTrackOnly, FixedCutLoose, FCTight, FCLoose, FCTightTrackOnly, PromptLepton, None",
                      "None");
    registerParameter("MuonIsolationSF", "Force muon isolation SF (e.g. None). EXPERIMENTAL!", " ");
    registerParameter("MuonIsolationSFLoose", "Force muon isolation SF (e.g. None). EXPERIMENTAL!", " ");
    registerParameter("UseAntiMuons", "Use AntiMuons for fake estimate. Default: false", "false");

    registerParameter("UseSoftMuons", "True to use soft muons, False (default) otherwise", "False");
    registerParameter("SoftMuonPt", "Soft Muon pT cut for object selection (in MeV). Default 4 GeV.", "4000");
    registerParameter("SoftMuonEta", "Absolute Soft Muon eta cut for object selection. Default 2.5.", "2.5");
    registerParameter("SoftMuonQuality",
                      "Soft Muon quality cut for object selection. Options are Loose, Medium, Tight (default), LowPt",
                      "Tight");
    registerParameter("SoftMuonDRJet",
                      "Soft Muon maximum dR wrt nearest selected jet. Can be set to 999. to keep all soft muons. Default 0.4",
                      "0.4");
    registerParameter("SoftMuonAdditionalTruthInfo",
                      "Decide if you want to store additional truth information on the particle-level origin for soft muons (see TopParticleLevel/TruthTools.h): True or False (default)",
                      "False");
    registerParameter("SoftMuonAdditionalTruthInfoCheckPartonOrigin",
                      "Decide if you want to store additional truth information on the parton-level origin for soft muons (see TopParticleLevel/TruthTools.h, this makes sense only if also SoftMuonAdditionalTruthInfo is True) : True or False (default)",
                      "False");
    registerParameter("SoftMuonAdditionalTruthInfoDoVerbose",
                      "Debug output for soft muon addition information: True or False (default)",
                      "False");

    registerParameter("JetPt", "Jet pT cut for object selection (in MeV). Default 25 GeV.", "25000.");
    registerParameter("JetEta", "Absolute Jet eta cut for object selection. Default 2.5.", "2.5");
    registerParameter("FwdJetAndMET", "Forward jet selection and corresponding MET calculation."
                                      "Default (does nothing on forward jets), fJVT (No longer recommended), fJVTTight(apply tight fJVT cut if pT<60GeV and |eta|>2.5), Tight (requires pT>30GeV if |eta|>2.5).",
                      "Default");
    registerParameter("JetPtGhostTracks",
                      "Jet pT threshold for ghost track systematic variations calculation (in MeV). Default 19 GeV.",
                      "19000.");
    registerParameter("JetUncertainties_BunchSpacing",
                      "25ns (default) or 50ns - for JetUncertainties",
                      "25ns");
    registerParameter("JetUncertainties_NPModel",
                      "AllNuisanceParameters, CategoryReduction (default), GlobalReduction, StrongReduction - for JetUncertainties",
                      "CategoryReduction");
    registerParameter("JetUncertainties_QGFracFile", "To specify a root file with quark/gluon fractions,"
                                                     " in order to reduce FlavourComposition and response uncertainties."
                                                     " Default: None (i.e. no file is used and default flat 50+/-50% fraction is used).",
                      "None");
    registerParameter("JetUncertainties_QGHistPatterns", "To specify a pattern for the name of the quark/gluon fractions histograms, or a list of DSIDs which will have their specific histogram."
                                                         " Two syntaxes are possible, either a single string or a list of DSIDs separated by commas:"
                                                         "   \"MyQGHisto\" (the histograms with \"MyQGHisto\" in their names will be used for all DSIDs),"
                                                         "   \"410470,410472,345873,345874,345875\" (for the listed DSIDs, histograms with the processed DSID will be used, while the flat 50+/-50% fraction will be used for the other DSIDs)."
                                                         " Default: None (i.e. no specific pattern is looked for in the name of the provided histograms).",
                      "None");
    registerParameter("LargeRSmallRCorrelations",
                      "Do large-small R jet correlation systematics - True or False (default)",
                      "False");
    registerParameter("JetJERSmearingModel",
                      "All (inc. data smearing), All_PseudoData (use MC as pseudo-data), Full (inc. data smearing), Full_PseudoData (use MC as pseudo-data) or Simple (MC only - default)",
                      "Simple");
    registerParameter("JetCalibSequence", "Jet calibaration sequence, GSC (default) or JMS", "GSC");
    registerParameter("StoreJetTruthLabels", "Flag to store truth labels for jets - True (default) or False", "True");
    registerParameter("JVTinMETCalculation",
                      "Perfom a JVT cut on the jets in the MET recalculation? True (default) or False.", "True");
    registerParameter("SaveFailJVTJets", "Save the jets that failed the JVT cut? False (default) or True.", "False");
    registerParameter("JVTWP", "Set JVT WP, default is set to \'Default\' (Tight for PFlow and Medium for Topo).",
                      "Default");

    registerParameter("JSF", "Used for top mass analysis, default is 1.0", "1.0");
    registerParameter("bJSF", "Used for top mass analysis, default is 1.0", "1.0");

    registerParameter("LargeRJetPt", "Track Jet pT cut for object selection (in MeV). Default 7 GeV.", "25000.");
    registerParameter("LargeRJetEta", "Absolute large-R jet eta cut for object selection. Default 2.0.", "2.0");
    registerParameter("LargeRJetUncertainties_NPModel",
                      "AllNuisanceParameters, CategoryReduction (default), GlobalReduction, - for LCTopo Large-R Jet Uncertainties or Scale_TCC_all - for TCC Large-R Jet Uncertainties",
                      "CategoryReduction");
    registerParameter("LargeRJESJMSConfig",
                      "Calibration for large-R JES/JMS. CombMass, CaloMass or TCCMass (default CombMass).",
                      "CombMass");
    registerParameter("BoostedJetTagging",
                      "Boosted jet taggers to use in the analysis, separated by commas or white spaces."
                      " By default, no tagger is used.",
                      " ");

    registerParameter("TrackJetPt", "Track Jet pT cut for object selection (in MeV). Default 10 GeV.", "10000.");
    registerParameter("TrackJetEta", "Absolute Track Jet eta cut for object selection. Default 2.5.", "2.5");

    registerParameter("RCJetPt", "Reclustered Jet pT cut for object selection (in MeV). Default 100000 MeV.",
                      "100000.");
    registerParameter("RCJetEta", "Reclustered Jet eta cut for object selection. Default 2.0.", "2.0");
    registerParameter("RCJetTrim", "Reclustered Jet trimming cut for object selection. Default 0.05.", "0.05");
    registerParameter("RCJetRadius", "Reclustered Jet radius for object selection. Default 1.0", "1.0");
    registerParameter("UseRCJetSubstructure", "Calculate Reclustered Jet Substructure Variables. Default False",
                      "False");
    registerParameter("UseRCJetAdditionalSubstructure",
                      "Calculate Additional Reclustered Jet Substructure Variables. Default False", "False");

    registerParameter("UseRCJets", "Use Reclustered Jets. Default False.", "False");

    registerParameter("VarRCJetPt",
                      "Reclustered Jet (variable-R) pT cut for object selection (in MeV). Default 100000 MeV.",
                      "100000.");
    registerParameter("VarRCJetEta", "Reclustered Jet (variable-R) eta cut for object selection. Default 2.0.", "2.0");
    registerParameter("VarRCJetTrim", "Reclustered Jet (variable-R) trimming cut for object selection. Default 0.05.",
                      "0.05");
    registerParameter("VarRCJetMaxRadius",
                      "Reclustered Jet (variable-R) max. radius cut for object selection. Default 1.0", "1.0");
    registerParameter("VarRCJetRho", "Reclustered Jet (variable-R) list of rho values (). Default 2.", "2");
    registerParameter("VarRCJetMassScale",
                      "Reclustered Jet (variable-R) list of mass scale values (m_w,m_z,m_h,m_t). By default use all.",
                      "m_w,m_z,m_h,m_t");
    registerParameter("UseVarRCJets", "Use Reclustered Jets (Variable-R Jets). Default False.", "False");
    registerParameter("UseVarRCJetSubstructure",
                      "Calculate Variable-R Reclustered Jet Substructure Variables. Default False", "False");
    registerParameter("UseVarRCJetAdditionalSubstructure",
                      "Calculate Additional Variable-R Reclustered Jet Substructure Variables. Default False", "False");

    registerParameter("TauPt",
                      "Pt cut applied to both tight and loose taus (in MeV)."
                      "Default 25 GeV.",
                      "25000");
    registerParameter("TauJetIDWP",
                      "Tau jet IDWP (None, Loose, Medium, Tight, LooseNotMedium, LooseNotTight, MediumNotTight, NotLoose, RNNLoose, RNNMedium, RNNTight)."
                      "Default RNNMedium.",
                      "RNNMedium");
    registerParameter("TauJetIDWPLoose",
                      "Loose Tau jet IDWP (None, Loose, Medium, Tight, LooseNotMedium, LooseNotTight, MediumNotTight, NotLoose)."
                      "Default RNNLoose.",
                      "RNNLoose");
    registerParameter("TauEleBDTWP",
                      "Tau electron BDT WP (None, Loose, Medium, Tight, OldLoose, OldMedium)."
                      "Default Loose.",
                      "Loose");
    registerParameter("TauEleBDTWPLoose",
                      "Loose Tau electron BDT WP (None, Loose, Medium, Tight, OldLoose, OldMedium)."
                      "Default Loose.",
                      "Loose");
    registerParameter("TauEleOLR",
                      "Apply tau-electron overlap removal (True/False)."
                      "Default False",
                      "False");
    registerParameter("TauEleOLRLoose",
                      "Apply loose tau-electron overlap removal (True/False)."
                      "Default False",
                      "False");
    registerParameter("TauJetConfigFile",
                      "Config file to configure tau selection. "
                      "If anything other than 'Default'"
                      "then all cuts are taken from the "
                      "config file rather than other options.",
                      "Default");
    registerParameter("TauJetConfigFileLoose",
                      "Config file to configure loose tau selection. "
                      "If anything other than 'Default'"
                      "then all cuts are taken from the "
                      "config file rather than other options.",
                      "Default");
    registerParameter("ApplyTauMVATES",
                      "Apply new Tau energy calibration based on substructure information and regression. Must be True. Deprecated.",
                      "True");
    registerParameter("TauSFDoRNNID",
                      "Save SF for RNN tau ID True/False",
                      "True");
    registerParameter("TauSFDoBDTID",
                      "Save SF for BDT tau ID True/False",
                      "False");

    registerParameter("Systematics", "What to run? Nominal (just the nominal), All(do all systematics) ", "Nominal");

    registerParameter("LibraryNames", "Names of any libraries that need loading");
    registerParameter("UseAodMetaData", "Whether to read xAOD meta-data from input files (default: True)", "True");
    registerParameter("WriteTrackingData", "Whether to generate and store analysis-tracking data (default: True)",
                      "True");
    registerParameter("ObjectSelectionName", "Code used to define objects, e.g. ObjectLoaderStandardCuts");
    registerParameter("OutputFormat", "Format, can be user defined, e.g. top::EventSaverFlatNtuple");
    registerParameter("OutputEvents",
                      "AllEvents (saves all events + decison bits), SelectedEvents (saves only the events passing your cuts)");
    registerParameter("OutputFilename", "The file that will contain the output histograms and trees");
    registerParameter("OutputFileSetAutoFlushZero",
                      "setAutoFlush(0) on EventSaverFlatNtuple for ANALYSISTO-44 workaround. (default: False)",
                      "False");
    registerParameter("OutputFileNEventAutoFlush",
                      "Set the number of events after which the TTree cache is optimised, ie setAutoFlush(nEvents). (default: 1000)",
                      "1000");
    registerParameter("OutputFileBasketSizePrimitive",
                      "Set the TTree basket size for primitive objects (int, float, ...). (default: 4096)", "4096");
    registerParameter("OutputFileBasketSizeVector", "Set the TTree basket size for vector objects. (default: 40960)",
                      "40960");
    registerParameter("RecomputeCPVariables",
                      "Run the CP tools to force computation of variables that may already exist in derivations? (default: True)",
                      "True");
    registerParameter("EventVariableSaveList", "The list of event variables to save (EventSaverxAODNext only).",
                      "runNumber.eventNumber.eventTypeBitmask.averageInteractionsPerCrossing");
    registerParameter("PhotonVariableSaveList", "The list of photon variables to save (EventSaverxAODNext only).",
                      "pt.eta.phi.m.charge.ptvarcone20.topoetcone20.passPreORSelection");
    registerParameter("ElectronVariableSaveList", "The list of electron variables to save (EventSaverxAODNext only).",
                      "pt.eta.phi.m.charge.ptvarcone20.topoetcone20.passPreORSelection");
    registerParameter("MuonVariableSaveList", "The list of muon variables to save (EventSaverxAODNext only).",
                      "pt.eta.phi.m.author.charge.ptvarcone30.topoetcone20.muonType.passPreORSelection");
    registerParameter("TauVariableSaveList", "The list of tau variables to save (EventSaverxAODNext only).",
                      "pt.eta.phi.m.charge.passPreORSelection");
    registerParameter("JetVariableSaveList", "The list of jet variables to save (EventSaverxAODNext only).",
                      "pt.eta.phi.m.passPreORSelection.btaggingLink.HadronConeExclTruthLabelID");
    registerParameter("BTagVariableSaveList", "The list of b-tag variables to save (EventSaverxAODNext only).",
                      "MV2c20_discriminant");

    registerParameter("NEvents",
                      "The number of events that you want to run on (for testing). If 0 then all events are run.", "0");
    registerParameter("FirstEvent",
                      "The number of events that you want to skip (for testing). If 0 then no events are skipped.",
                      "0");
    registerParameter("PerfStats", " I/O Performance printouts. None, Summary or Full", "None");
    registerParameter("IsAFII", "Define if you are running over a fastsim sample: True or False", " ");
    registerParameter("FilterBranches",
                      "Comma separated list of names of the branches that will be removed from the output", " ");
    registerParameter("FilterTrees",
                      "Comma separated list of names of the trees that will be removed from the output", " ");

    registerParameter("FakesMMWeightsIFF",
                      "Calculate matrix-method weights for fake leptons estimate using FakeBkgTools from IFF: True (calculate weights), False (does nothing)",
                      "False");
    registerParameter("FakesMMConfigIFF",
                      "Configurations for fake leptons estimate using FakeBkgTools from IFF: - default is $ROOTCOREBIN/data/TopFakes/efficiencies.xml:1T:1F[T]. Use as \n <ROOT/XML FILE>:<DEFNINITION>:<PROCESS>;<ROOT/XML FILE 2>:<DEFNINITION 2>:<PROCESS 2>; ...",
                      "$ROOTCOREBIN/data/TopFakes/efficiencies.xml:1T:1F[T]");
    registerParameter("FakesMMIFFDebug",
                      "Enables debug mode for matrix-method weight calculation using FakeBkgTools from IFF: True, False (default)",
                      "False");

    registerParameter("DoTight", "Dumps the normal non-\"*_Loose\" trees : Data, MC, Both (default), False", "Both");
    registerParameter("DoLoose", "Run Loose selection and dumps the Loose trees : Data (default), MC, Both, False",
                      "Data");
    registerParameter("DoSysts", "Run systematics on given selection: Both (default), Tight, Loose", "Both");

    registerParameter("OverlapRemovalLeptonDef",
                      "Special: run overlap removal on : Tight (top default) or Loose (not top default) lepton definitions",
                      "Tight");
    registerParameter("ApplyTightSFsInLooseTree",
                      "Special: in Loose trees, calculate lepton SFs with tight leptons only, and considering they are tight: True or False (default)",
                      "False");

    registerParameter("ApplyElectronInJetSubtraction",
                      "Subtract electrons close to jets for boosted analysis : True or False(top default)", "False");
    registerParameter("TopPartonHistory", "ttbar, tb, Wtb, ttz, ttgamma, tHqtautau, False (default)", "False");

    registerParameter("TopParticleLevel", "Perform particle level selection? True or False", "False");
    registerParameter("DoParticleLevelOverlapRemoval",
                      "Perform overlap removal at particle level? True (default), False, or any combination (comma separated) of MuonJet, ElectronJet, JetPhoton",
                      "True");

    registerParameter("PDFInfo",
                      "Do you want the PDF info? True (in truth tree), Nominal (save to the nominal tree if passes selection) or False (nothing, default)",
                      "False");
    registerParameter("MCGeneratorWeights",
                      "Do you want the OTF-computed MC generator weights (if available)? True (in truth tree), Nominal (save to the nominal tree if passes selection) or False (nothing, default)",
                      "False");
    registerParameter("NominalWeightNames",
                      "List of nominal weight names to attempt to retrieve. Attempts are made in the order as specified. If none of the names can be found, we will crash with error message. Use index instead in such case.",
                      "\" nominal \",\"nominal\",\"Default\",\"Weight\",\"1001\",\" muR=0.10000E+01 muF=0.10000E+01 \",\"\",\" \"");

    registerParameter("TruthBlockInfo", "Do you want to dump the full Truth block info? True or False", "False");

    registerParameter("TruthElectronPt",
                      "Electron pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.",
                      "25000");
    registerParameter("TruthElectronEta",
                      "Absolute electron eta cut for [Particle Level / Truth] object selection. Default 2.5.", "2.5");

    registerParameter("TruthMuonPt",
                      "Muon pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.", "25000");
    registerParameter("TruthMuonEta",
                      "Absolute Muon eta cut for [Particle Level / Truth] object selection. Default 2.5.", "2.5");

    registerParameter("TruthPhotonPt",
                      "Photon pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.",
                      "25000");
    registerParameter("TruthPhotonEta",
                      "Absolute Photon eta cut for [Particle Level / Truth] object selection. Default 2.5.",
                      "2.5");
    registerParameter("TruthPhotonOrigin",
                      "Potential origin of [Particle Level / Truth] photons. Comma separated list of particle origin values as given by MCTruthClassifier (string names).",
                      "WBoson,ZBoson,SinglePhot,Higgs,HiggsMSSM,WZMSSM,PromptPhot,SUSY,UndrPhot,FSRPhot");
    registerParameter("TruthPhotonIsolation",
                      "Configuration option for isolation applied to [Particle Level / Truth] photons. "
                      "This can be False / None (isolation requirement disabled), "
                      "True (use default isolation), "
                      "or a configuration in the format `VAR CUT`, where VAR is one of the isolation variables and CUT is the cut applied as `VAR / pt < CUT`.",
                      "ptcone30 0.1");


    registerParameter("TruthJetPt",
                      "Jet pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.", "25000");
    registerParameter("TruthJetEta", "Absolute Jet eta cut for [Particle Level / Truth] object selection. Default 2.5.",
                      "2.5");

    registerParameter("TruthLargeRJetPt",
                      "Large R Jet pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.",
                      "25000");
    registerParameter("TruthLargeRJetEta",
                      "Absolute Large R Jet eta cut for [Particle Level / Truth] object selection. Default 2.5.",
                      "2.5");

    registerParameter("TruthTauPt",
                      "Tau pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.", "25000");
    registerParameter("TruthTauEta", "Tau eta cut for [Particle Level / Truth] object selection. Default 2.5.", "2.5");

    registerParameter("LHAPDFSets",
                      "List of PDF sets to calculate weights for, seperated by spaces, use LHAPDF names e.g CT10nlo NNPDF30_nlo_as_0118 MMHT2014nlo68cl",
                      " ");
    registerParameter("LHAPDFEventWeights",
                      "Save per event weights for all PDF sets/members: True (lots of info in truth tree!), Nominal (save to the nominal tree if passes selection) or False (nothing, default).",
                      "False");
    registerParameter("LHAPDFBaseSet",
                      "Base PDF set used to recalculate XF1,XF2 values if they are zero. Will be added to LHAPDFSets.",
                      " ");
    registerParameter("BTagCDIPath", "Path to the b-tagging CDI file. Default: Using the hardcoded path.", "Default");

    registerParameter("BTaggingWP",
                      "b-tagging WPs to use in the analysis, separated by commas."
                      " The format should follow the convention of the b-tagging CP group, e.g. FixedCutBEff_60, FlatBEff_77, Continuous, etc."
                      " For fixed-cut WPs, the simpler format 60%, instead of FixedCutBEff_60, is also tolerated."
                      " The specified WPs which are calibrated for all flavours will have scale-factors computed."
                      " By default, no WP is used.",
                      " ");

    registerParameter("BTaggingSystExcludedFromEV",
                      "User-defined list of b-tagging systematics to be dextracted from eigenvector decomposition, separated by semi-colons (none by default)",
                      "none");

    registerParameter("BTaggingCalibrationB",
                      "The calibration to use for the b-tagging SFs (B-jets)."
                      " Default 'default'",
                      "default");

    registerParameter("BTaggingCalibrationC",
                      "The calibration to use for the b-tagging SFs (C- and T-jets)."
                      " Default 'default'",
                      "default");

    registerParameter("BTaggingCalibrationLight",
                      "The calibration to use for the b-tagging SFs (Light-jets)."
                      " Default 'default'",
                      "default");

    registerParameter("PRWConfigFiles",
                      "List of PU config files, seperated by spaces (nothing by default) - Not compatible with FS/AF options",
                      " ");
    registerParameter("PRWConfigFiles_FS",
                      "List of PU config files only for full sim samples, seperated by spaces (nothing by default)",
                      " ");
    registerParameter("PRWConfigFiles_AF",
                      "List of PU config files only for fast sim samples, seperated by spaces (nothing by default)",
                      " ");
    registerParameter("PRWActualMu_FS",
                      "List of actual mu files for full sim samples, seperated by spaces (nothing by default)", " ");
    registerParameter("PRWActualMu_AF",
                      "List of actual mu files only for fast sim samples, seperated by spaces (nothing by default)",
                      " ");
    registerParameter("PRWLumiCalcFiles", "List of PU lumicalc files, seperated by spaces (nothing by default)", " ");
    registerParameter("PRWUseGRLTool", "Pass the GRL tool to the PU reweighting tool (False by default)", "False");
    registerParameter("PRWMuDependent",
                      "Use mu dependent random run numbers for MC. "
                      "True or False (default True)",
                      "True");
    registerParameter("PRWCustomScaleFactor",
                      "Specify custom scale-factor and up/down variations, for specific studies."
                      "Format is \'nominal:up:down\'."
                      "If nothing is set, the default values will be used (recommended).",
                      " ");
    registerParameter("PRWUnrepresentedDataTolerance",
                      "Specify value between 0 and 1 to represent acceptable fraction of unrepresented data in PRW [default: 0.05]",
                      "0.05");
    registerParameter("PRWPeriodAssignments",
                      "Specify period number assignments to run numbers ranges in this form: \"XXX:XXX:XXX\", where XXX are runnumbers, first number is the associated run number, second number is the period block start, the third number is the period block end. You can pass any number of these sets (total number of provided RunNumbers needs to be divisible by 3). Default is used if not specified",
                      " ");

    registerParameter("MuonTriggerSF", "Muon trigger SFs to calculate", "HLT_mu20_iloose_L1MU15_OR_HLT_mu50");

    registerParameter("KLFitterTransferFunctionsPath", "Select the transfer functions to use", "mc12a/akt4_LCtopo_PP6");
    registerParameter("KLFitterOutput", "Select the KLFitter output (FULL, FITTEDTOPS_ONLY, JETPERM_ONLY)", "FULL");
    registerParameter("KLFitterJetSelectionMode",
                      "kLeadingThree , kLeadingFour , kLeadingFive , kLeadingSix , kLeadingSeven , kLeadingEight , kBtagPriorityThreeJets , kBtagPriorityFourJets , kBtagPriorityFiveJets, kBtagPrioritySixJets , kBtagPrioritySevenJets , kBtagPriorityEightJets",
                      "kBtagPriorityFourJets");
    registerParameter("KLFitterBTaggingMethod", "Recommend use kNotag or kVetoNoFit - see KLFitter TWiki", "kNotag");
    registerParameter("KLFitterLH",
                      "Select likelihood depending on signal, ttbar, ttbar_angles, ttH, ttZTrilepton, ttbar_AllHadronic, ttbar_BoostedLJets",
                      "ttbar");
    registerParameter("KLFitterTopMassFixed", "Fix the mass of the top quark? True or False", "True");
    registerParameter("KLFitterSaveAllPermutations",
                      "Save All permutations to the output file (False will save only the best)", "False");
    registerParameter("KLFitterFailOnLessThanXJets",
                      "Fail if kLeadingX or kBtagPriorityXJets is set and the number of jets in the event is less than X (Default is False)",
                      "False");

    registerParameter("DynamicKeys", "Additional dynamic key list seperated by ,", "");

    registerParameter("OverlapRemovalProcedure", "Overlap removal procedure to be used. Options include:"
                                                 " recommended [default], jetmuApplyRelPt, harmonized,"
                                                 " Boosted, BoostedSlidingDREl, BoostedSlidingDRMu, BoostedSlidingDRElMu, noTauJetOLR",
                      "recommended"
                      );

    registerParameter("OverlapRemovalSlidingInnerDRel",
                      "Overlap removal inner radius to be used for electron SlidingDR ,", "0.2");
    registerParameter("OverlapRemovalSlidingInnerDRmu", "Overlap removal inner radius to be used for muon SlidingDR ,",
                      "0.2");

    registerParameter("LargeJetOverlapRemoval",
                      "Perform overlap removal including large-R jets. True or False (default: False).", "False");

    registerParameter("HLLHC",
                      "Set to run HL-LHC studies,"
                      "True or False (default False)",
                      "False");

    registerParameter("HLLHCFakes",
                      "Set to enable Fakes HL-LHC studies,"
                      "True or False (default False)",
                      "False");

    registerParameter("SaveBootstrapWeights", "Set to true in order to save Poisson bootstrap weights,"
                                              "True or False (default False)", "False");

    registerParameter("NumberOfBootstrapReplicas", "Define integer number of replicas to be stored with bootstrapping, "
                                                   "Default 100", "100");

    registerParameter("UseBadBatmanCleaning", "Switch to turn on BadBatman cleanig.", "False");
    registerParameter("BadBatmanCleaningRange",
                      "Set a range of RunNumbers where the cleaning is applied in the form of XXXXX:YYYYY",
                      "276262:311481");
    registerParameter("UseEventLevelJetCleaningTool",
                      "Switch to turn on event-level jet cleaning tool (for testing), True or False (default False)",
                      "False");

    registerParameter("UseGlobalLeptonTriggerSF",
                      "Switch to activate event-level trigger scale factors allowing multiple OR of single-, di-, tri- lepton triggers, True or False (default False)",
                      "False");
    registerParameter("GlobalTriggers",
                      "Trigger list for GlobalLeptonTriggerSF - Format as 2015@trig1,trig2 2016@trig3,trig4 : Separate periods defined with @ using whitespace, triggers with comma (default: None)",
                      "None");
    registerParameter("GlobalTriggersLoose",
                      "Trigger list for GlobalLeptonTriggerSF - Format as 2015@trig1,trig2 2016@trig3,trig4 : Separate periods defined with @ using whitespace, triggers with comma (default: None)",
                      "None");
    registerParameter("ElectronTriggers", "Deprecated, use GlobalTriggers instead.", "None");
    registerParameter("ElectronTriggersLoose", "Deprecated, use GlobalTriggersLoose instead.", "None");
    registerParameter("MuonTriggers", "Deprecated, use GlobalTriggers instead.", "None");
    registerParameter("MuonTriggersLoose", "Deprecated, use GlobalTriggersLoose instead.", "None");

    registerParameter("DemandPrimaryVertex", "Wether at least one primary vertex in event is required. Default True. For debugging purposes only!", "True");

    registerParameter("KillExperimental", "Disable some specific experimental feature.", " ");
    registerParameter("RedifineMCMCMap", "Dictionary for translating the shower names from TopDataPreparation. Format: \"shower1:shower2,shower3:shower4\".", " ");
  }

  ConfigurationSettings* ConfigurationSettings::get() {
    if (!m_instance) m_instance = new ConfigurationSettings();

    return m_instance;
  }

  void ConfigurationSettings::loadFromFile(const std::string& filename) {
    std::ifstream input(filename.c_str());

    if (!input) {
      std::cout << "Configuration file does not exist, " << filename << std::endl;
      std::cout << "Can't continue" << std::endl;
      exit(1);
    }

    struct SelectionData {
      std::string name;
      bool isSub;
      std::vector<std::string> cuts;
    };
    std::string line;
    std::vector<SelectionData> selections;

    //for the key-value pairs
    while (std::getline(input, line)) {
      std::string newstring(line);
      //std::cout << newstring << '\n';

      if (newstring.find("#") != std::string::npos) newstring = newstring.substr(0, newstring.find("#"));

      // remove (multiple) spaces hanging around relevant information
      boost::algorithm::trim_all(newstring);
      if (newstring.empty()) continue;

      // handle start of a (sub)selection (implies end of key-value section)
      if (boost::algorithm::starts_with(newstring, "SELECTION ")) {
        selections.push_back({newstring.substr(10), false, {}});
        continue;
      }
      if (boost::algorithm::starts_with(newstring, "SUB ")) {
        selections.push_back({newstring.substr(4), true, {}});
        continue;
      }

      if (!selections.empty()) {
        // read body of (sub)selection
        auto& sel = selections.back();
        if (boost::algorithm::starts_with(newstring, ". ")) {
          // source another (sub)selection here
          auto subselName = newstring.substr(2);
          auto subselIt = std::find_if(selections.rbegin(), selections.rend(),
                                       [subselName](SelectionData const& sel) {
            return(subselName == sel.name);
          });
          if (subselIt == selections.rend()) throw std::invalid_argument(
                    "ConfigurationSettings: unknown selection: " + subselName);
          sel.cuts.insert(sel.cuts.end(), subselIt->cuts.begin(), subselIt->cuts.end());
        } else {
          sel.cuts.push_back(newstring);
        }
        continue;
      }

      // parse key-value pair
      std::istringstream liness(newstring);
      std::string key;
      std::string value;

      std::getline(liness, key, ' '); //to the space
      std::getline(liness, value); //to the end of the line

      auto its = strings_.find(key);
      if (its != strings_.end()) {
        its->second.m_data = value;
        its->second.m_set = true;
      }

      //// add dynamic keys
      if ("DynamicKeys" == key) {
        std::vector<std::string> listofkeys;
        std::string separator = ",";
        std::string::size_type start = 0, end = 0;
        while ((end = value.find(separator, start)) != std::string::npos) {
          std::string token = value.substr(start, end - start);
          if (token.size()) listofkeys.push_back(token);
          start = end + 1;
        }

        std::string lasttoken = value.substr(start);
        if (lasttoken.size()) listofkeys.push_back(lasttoken);

        for (auto par : listofkeys) {
          registerParameter(par, "Dynamic parameter", "");
        }
      }
    }

    for (auto& sel : selections) {
      if (sel.isSub) continue;
      m_selections.push_back({sel.name, sel.cuts});
    }

    {
      auto const& it = strings_.find("KillExperimental");
      m_killedFeatures.clear();
      if (it != strings_.end() && it->second.m_set) {
        std::string strValue(it->second.m_data);
        boost::trim(strValue);
        boost::split(m_killedFeatures, strValue, boost::is_any_of(" "));
      }
    }

    input.close();
    m_configured = true;
  }

  void ConfigurationSettings::registerParameter(const std::string& name, const std::string& message,
                                                const std::string& default_val) {
    StringData data;

    data.m_data = default_val;
    data.m_human_explanation = message;
    data.m_set = (default_val.empty() ? false : true);
    strings_[name] = data;
  }

  const std::string& ConfigurationSettings::value(const std::string& key) const {
    //This class never has loadFromFile called
    if (!m_configured) {
      std::string message = "ConfigurationSettings: Not correctly configured\n";
      message.append("You need to call top::ConfigurationSettings::get()->loadFromFile(filename)\n");
      message.append("Early in your program\n");
      throw std::invalid_argument(message);
    }

    std::map<std::string, StringData>::const_iterator its = strings_.find(key);
    //The string is not in the map
    if (its == strings_.end()) {
      throw std::invalid_argument("ConfigurationSettings: The variable doesn't exist in the code " + key);
    }

    //In the map, but never set to anything
    if (!its->second.m_set) {
      throw std::invalid_argument("ConfigurationSettings: You never set a value for " + key);
    }

    return its->second.m_data;
  }

  bool ConfigurationSettings::configured() const {
    return m_configured;
  }

  const std::map<std::string, StringData>& ConfigurationSettings::stringData() const {
    return strings_;
  }

  const std::vector<SelectionConfigurationData> ConfigurationSettings::selections() const {
    return m_selections;
  }

  bool ConfigurationSettings::retrieve(std::string const& key, bool& value) const {
    using boost::trim;
    using boost::equals;
    using boost::iequals;
    auto stringValue = ConfigurationSettings::get()->value(key);
    trim(stringValue);
    if (equals(stringValue, "")) {
      return false;
    }
    if (iequals(stringValue, "false") or iequals(stringValue, "0") or iequals(stringValue, "n") or iequals(stringValue,
                                                                                                           "no") or
        iequals(stringValue, "off")) {
      value = false;
      return true;
    }
    if (iequals(stringValue, "true") or iequals(stringValue, "1") or iequals(stringValue, "y") or iequals(stringValue,
                                                                                                          "yes") or
        iequals(stringValue, "on")) {
      value = true;
      return true;
    }
    throw std::invalid_argument(std::string("expected boolean value for configuration setting ") + key);
  }

  bool ConfigurationSettings::feature(std::string const& name) const {
    /* We search a list of strings, not a particularly efficient implementation.
     * If need be, we could abuse the aux registry and use integers instead of
     * strings. Anyhow, in most cases, the list should be empty. */
    return(m_killedFeatures.empty() ||
           std::find(m_killedFeatures.begin(), m_killedFeatures.end(), name) == m_killedFeatures.end());
  }
}

std::ostream& operator << (std::ostream& os, const top::SelectionConfigurationData& data) {
  os << " - " << data.m_name << "\n";
  for (const auto& cutname : data.m_cutnames)
    os << "    " << cutname << "\n";

  return os;
}

std::ostream& operator << (std::ostream& os, const top::ConfigurationSettings& settings) {
  for (std::map<std::string, top::StringData >::const_iterator its = settings.stringData().begin();
       its != settings.stringData().end(); ++its) {
    std::stringstream s;
    s << "\"" << its->first << "\"";

    std::stringstream s2;
    s2 << "\"" << its->second.m_data << "\"";
    os << std::setw(40) << std::left << s.str() << " : " << std::setw(35) << s2.str() << " - " << std::right <<
      its->second.m_human_explanation << "\n";
  }

  //for (const auto& selection : settings.selections())
  //    os << selection << "\n";

  return os;
}
