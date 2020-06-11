/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCPTools/TopEgammaCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Egamma include(s):
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include "ElectronEfficiencyCorrection/ElectronChargeEfficiencyCorrectionTool.h"
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeTool.h"
#include "PhotonEfficiencyCorrection/AsgPhotonEfficiencyCorrectionTool.h"
#include "Root/EGSelectorConfigurationMapping.h"

namespace top {
  EgammaCPTools::EgammaCPTools(const std::string& name) :
    asg::AsgTool(name),
    m_electronEffTriggerFile("SetMe"),
    m_electronEffTriggerLooseFile("SetMe"),
    m_electronEffSFTriggerFile("SetMe"),
    m_electronEffSFTriggerLooseFile("SetMe"),
    m_electronEffSFRecoFile("SetMe"),
    m_electronEffSFIDFile("SetMe"),
    m_electronEffSFIDLooseFile("SetMe"),
    m_electronEffSFIsoFile("SetMe"),
    m_electronEffSFIsoLooseFile("SetMe"),
    m_electronEffSFChargeIDFile("SetMe"),
    m_electronEffSFChargeIDLooseFile("SetMe"),
    m_electronEffSFChargeMisIDFile("SetMe"),
    m_electronEffSFChargeMisIDLooseFile("SetMe"),
    m_fwdElectronEffSFIDFile("SetMe"),
    m_fwdElectronEffSFIDLooseFile("SetMe") {
    declareProperty("config", m_config);

    declareProperty("EgammaCalibrationAndSmearingTool", m_egammaCalibrationAndSmearingTool);
    declareProperty("ElectronEffTrigger", m_electronEffTrigger);
    declareProperty("ElectronEffTriggerLoose", m_electronEffTriggerLoose);
    declareProperty("ElectronEffSFTrigger", m_electronEffSFTrigger);
    declareProperty("ElectronEffSFTriggerLoose", m_electronEffSFTriggerLoose);
    declareProperty("ElectronEffReco", m_electronEffSFReco);
    declareProperty("ElectronEffID", m_electronEffSFID);
    declareProperty("ElectronEffIDLoose", m_electronEffSFIDLoose);
    declareProperty("ElectronEffIso", m_electronEffSFIso);
    declareProperty("ElectronEffIsoLoose", m_electronEffSFIsoLoose);
    declareProperty("ElectronEffChargeID", m_electronEffSFChargeID);
    declareProperty("ElectronEffChargeIDLoose", m_electronEffSFChargeIDLoose);

    declareProperty("ElectronEffTriggerCorrModel", m_electronEffTriggerCorrModel);
    declareProperty("ElectronEffTriggerLooseCorrModel", m_electronEffTriggerLooseCorrModel);
    declareProperty("ElectronEffSFTriggerCorrModel", m_electronEffSFTriggerCorrModel);
    declareProperty("ElectronEffSFTriggerLooseCorrModel", m_electronEffSFTriggerLooseCorrModel);
    declareProperty("ElectronEffRecoCorrModel", m_electronEffSFRecoCorrModel);
    declareProperty("ElectronEffIDCorrModel", m_electronEffSFIDCorrModel);
    declareProperty("ElectronEffIDLooseCorrModel", m_electronEffSFIDLooseCorrModel);
    declareProperty("ElectronEffIsoCorrModel", m_electronEffSFIsoCorrModel);
    declareProperty("ElectronEffIsoLooseCorrModel", m_electronEffSFIsoLooseCorrModel);

    declareProperty("PhotonIsEMSelectorLoose", m_photonLooseIsEMSelector);
    declareProperty("PhotonIsEMSelectorMedium", m_photonMediumIsEMSelector);
    declareProperty("PhotonIsEMSelectorTight", m_photonTightIsEMSelector);

    declareProperty("FwdElectronSelector", m_fwdElectronSelector);
    declareProperty("FwdElectronSelectorLoose", m_fwdElectronSelectorLoose);
    declareProperty("FwdElectronEffIDLoose", m_fwdElectronEffSFIDLoose);
    declareProperty("WebBunchCrossingTool", m_webBunchCrossingTool);
  }

  StatusCode EgammaCPTools::initialize() {
    ATH_MSG_INFO("top::EgammaCPTools initialize...");

    if (m_config->isTruthDxAOD()) {
      ATH_MSG_INFO("top::EgammaCPTools: no need to initialise anything on truth DxAOD");
      return StatusCode::SUCCESS;
    }

    if (m_config->usePhotons() || m_config->useElectrons() || m_config->useFwdElectrons()) {
      if (m_config->makeAllCPTools()) {// skiping calibrations on mini-xAODs
        top::check(setupCalibration(), "Failed to setup Egamma calibration tools");
      }
      if (m_config->useFwdElectrons() && m_config->makeAllCPTools()) {
        top::check(setupSelectors(), "Failed to setup Fwd electrons selectors tools");
      }
      if (m_config->isMC()) {// scale-factors are only for MC
        top::check(setupScaleFactors(), "Failed to setup Egamma scale-factor tools");
      }
    } else {
      ATH_MSG_INFO(
        "top::EgammaCPTools: no need to initialise anything since using neither electrons nor fwd electrons nor photons");
    }

    return StatusCode::SUCCESS;
  }

  StatusCode EgammaCPTools::setupSelectors() {
    ATH_MSG_INFO("top::EgammaCPTools setupSelectors..");
    if (m_config->useFwdElectrons()) {
      m_fwdElectronSelector = new AsgForwardElectronLikelihoodTool("CP::FwdElectronSelector");
      top::check(m_fwdElectronSelector->setProperty("ConfigFile",
                                                    EgammaSelectors::ForwardLHPointToConfFile.at(
                                                      m_config->fwdElectronID() + "LHForwardElectron")),
                 "Failed to set config for AsgElectronFwdLikelihoodTool");
      top::check(m_fwdElectronSelector->initialize(), "Couldn't initialise Forward Electron LH ID Tool");

      m_fwdElectronSelectorLoose = new AsgForwardElectronLikelihoodTool("CP::FwdElectronSelectorLoose");
      top::check(m_fwdElectronSelectorLoose->setProperty("ConfigFile",
                                                         EgammaSelectors::ForwardLHPointToConfFile.at(
                                                           m_config->fwdElectronIDLoose() + "LHForwardElectron")),
                 "Failed to set config for AsgElectronFwdLikelihoodTool");
      top::check(m_fwdElectronSelectorLoose->initialize(), "Couldn't initialise Forward Electron LH ID Loose Tool");

      if (!m_config->isMC()) {
        ATH_MSG_INFO("top::EgammaCPTools setting up web bunch crossing tool");
        m_webBunchCrossingTool = new Trig::WebBunchCrossingTool("CP::WebBunchCrossingTool");
        top::check(m_webBunchCrossingTool->setProperty("OutputLevel",
                                                       MSG::INFO), "failed to set propert for WebBunchCrossingTool");
        top::check(m_webBunchCrossingTool->setProperty("ServerAddress",
                                                       "atlas-trigconf.cern.ch"),
                   "failed to set propert for WebBunchCrossingTool");
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode EgammaCPTools::setupCalibration() {
    // Setup electron and photon calibration tools
    // List of tools include:
    // - EgammaCalibrationAndSmearingTool
    // - Electron Charge ID Selector tool
    // - Photon shower shape fudge tool
    // - Photon efficiency correction tool
    using IEgammaCalibTool = CP::IEgammaCalibrationAndSmearingTool;
    const std::string egamma_calib_name = "CP::EgammaCalibrationAndSmearingTool";
    if (asg::ToolStore::contains<IEgammaCalibTool>(egamma_calib_name)) {
      m_egammaCalibrationAndSmearingTool = asg::ToolStore::get<IEgammaCalibTool>(egamma_calib_name);
    } else {
      IEgammaCalibTool* egammaCalibrationAndSmearingTool = new CP::EgammaCalibrationAndSmearingTool(egamma_calib_name);
      top::check(asg::setProperty(egammaCalibrationAndSmearingTool,
                                  "ESModel", "es2018_R21_v0"),
                 "Failed to set ESModel for " + egamma_calib_name);
      top::check(asg::setProperty(egammaCalibrationAndSmearingTool,
                                  "decorrelationModel",
                                  m_config->egammaSystematicModel()),
                 "Failed to set decorrelationModel for " + egamma_calib_name);

      if (m_config->isAFII()) {
        top::check(asg::setProperty(egammaCalibrationAndSmearingTool,
                                    "useAFII", 1),
                   "Failed to useAFII to true for" + egamma_calib_name);
      }
      top::check(egammaCalibrationAndSmearingTool->initialize(),
                 "Failed to initialize " + egamma_calib_name);
      m_egammaCalibrationAndSmearingTool = egammaCalibrationAndSmearingTool;
    }

    // The terribly named ElectronPhotonShowerShapeFudgeTool...
    // We apply this only to photons to correct the shower shape
    // This should only be applied on MC
    using IFudgeTool = IElectronPhotonShowerShapeFudgeTool;
    const std::string fudgeName = "PhotonFudgeTool";
    if (asg::ToolStore::contains<IFudgeTool>(fudgeName)) {
      m_photonFudgeTool = asg::ToolStore::get<IFudgeTool>(fudgeName);
    } else {
      IFudgeTool* fudge_tool = new ElectronPhotonShowerShapeFudgeTool(fudgeName);
      // Set Preselection to 22
      // for MC15 samples, which are based on a geometry derived from GEO-21 from 2015+2016 data
      top::check(asg::setProperty(fudge_tool, "Preselection", 22),
                 "Failed to set " + fudgeName + " property: Preselection");
      top::check(asg::setProperty(fudge_tool, "FFCalibFile",
                                  "ElectronPhotonShowerShapeFudgeTool/v2/PhotonFudgeFactors.root"),
                 "Failed to set ElectronPhotonShowerShapeFudgeTool");
      top::check(fudge_tool->initialize(),
                 "Failed to initialize PhotonFudgeTool");
      m_photonFudgeTool = fudge_tool;
    }

    // The photon efficiency SF tool
    bool af2 = m_config->isAFII();
    int data_type = 0; // Data
    if (m_config->isMC()) {
      if (af2) {
        ATH_MSG_WARNING("PhotonEfficiencies - Currently there are no recommendations for FastSimulation photons");
        ATH_MSG_WARNING("PhotonEfficiencies - Therefore we are advised to treat FastSim as FullSim for configuration");
        //data_type = 3; // AF2
        data_type = 1; // AF2 masquerading as full sim
      } else {
        data_type = 1; // Full sim
      }
    }

    using IPhotonEffTool = IAsgPhotonEfficiencyCorrectionTool;
    const std::string photonSFName = "AsgPhotonEfficiencyCorrectionTool"; // to retrieve ID Eff scale factors
    if (asg::ToolStore::contains<IPhotonEffTool>(photonSFName)) {
      m_photonEffSF = asg::ToolStore::get<IPhotonEffTool>(photonSFName);
    } else {
      if (m_config->isMC()) {  // Seem to only be able to setup the tool for MC
        IPhotonEffTool* photonEffSF = new AsgPhotonEfficiencyCorrectionTool(photonSFName);
        top::check(asg::setProperty(photonEffSF, "ForceDataType", data_type),
                   "Failed to set ForceDataType for " + photonSFName);
        top::check(photonEffSF->initialize(),
                   "Failed to initialize " + photonSFName);
        m_photonEffSF = photonEffSF;
      }
    }

    // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/IsolationSF2016Moriond#Radiative_Z_low_ET
    // To retrieve Isolation Eff scale factors
    // N.B. Naming for isolation working points for AsgPhotonEfficiencyCorrectionTool isolation scale factors
    // are different than those for the IsolationCorrectionTool (preceded by FixedCut)
    std::set<std::string> photon_isolations = {
      "Tight",
      "Loose",
      "TightCaloOnly"
    };
    for (const std::string& isoWP : photon_isolations) {
      std::string photonIsoSFName = "AsgPhotonEfficiencyCorrectionTool_IsoSF" + isoWP;
      if (!asg::ToolStore::contains<IPhotonEffTool>(photonIsoSFName)) {
        if (m_config->isMC()) {
          IPhotonEffTool* photonIsoSFTool = new AsgPhotonEfficiencyCorrectionTool(photonIsoSFName);
          top::check(asg::setProperty(photonIsoSFTool, "ForceDataType", data_type),
                     "Failed to set ForceDataType for " + photonIsoSFName);
          top::check(asg::setProperty(photonIsoSFTool, "IsoKey", isoWP),
                     "Failed to set IsoKey for " + photonIsoSFName);
          top::check(photonIsoSFTool->initialize(),
                     "Failed to initialize " + photonIsoSFName);
          m_photonIsoSFTools.push_back(photonIsoSFTool);
        }
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode EgammaCPTools::setupScaleFactors() {
    // Don't need for data, return SUCCESS straight away
    if (!m_config->isMC()) return StatusCode::SUCCESS;

    ///-- Scale factors --///
    std::string electron_data_dir = "ElectronEfficiencyCorrection/";

    // Define the data type variable - 0 : Data, 1 : MC FullSim, 3 : MC AFII
    int dataType(0);
    if (m_config->isMC()) {
      dataType = (m_config->isAFII()) ? 3 : 1;
    }

    ///-- Reco SFs doesn't depend on WP --///
    std::string electronID = m_config->electronID();
    if (electronID.find("LH") != std::string::npos) electronID.replace(electronID.find("LH"), 2, "LLH"); // that way
                                                                                                         // people do
                                                                                                         // not have to
                                                                                                         // change their
                                                                                                         // cuts file
    std::string electronIDLoose = m_config->electronIDLoose();
    if (electronIDLoose.find("LH") != std::string::npos) electronIDLoose.replace(electronIDLoose.find("LH"), 2, "LLH"); // that
                                                                                                                        // way
                                                                                                                        // people
                                                                                                                        // do
                                                                                                                        // not
                                                                                                                        // have
                                                                                                                        // to
                                                                                                                        // change
                                                                                                                        // their
                                                                                                                        // cuts
                                                                                                                        // file
    std::string electronIsolation = mapWorkingPoints(m_config->electronIsolationSF()); // temporary fix: we can
                                                                                       // hopefully remove the map soon!
    std::string electronIsolationLoose = mapWorkingPoints(m_config->electronIsolationSFLoose());

    // Retrieve full path to maps for different types of tool
    m_electronEffSFRecoFile = electronSFMapFilePath("reco");
    // - Tight
    m_electronEffSFIDFile = electronSFMapFilePath("ID");
    m_electronEffSFTriggerFile = electronSFMapFilePath("trigger");
    m_electronEffTriggerFile = electronSFMapFilePath("trigger");
    m_electronEffSFIsoFile = electronSFMapFilePath("isolation");
    // - Loose
    m_electronEffSFIDLooseFile = electronSFMapFilePath("ID");
    m_electronEffSFTriggerLooseFile = electronSFMapFilePath("trigger");
    m_electronEffTriggerLooseFile = electronSFMapFilePath("trigger");
    m_electronEffSFIsoLooseFile = electronSFMapFilePath("isolation");

    // Define the trigger string for scale factors
    const std::string trigger_string = "SINGLE_E_2015_e24_lhmedium_L1EM20VH_"
                                       "OR_e60_lhmedium_"
                                       "OR_e120_lhloose_"
                                       "2016_2018_e26_lhtight_nod0_ivarloose_"
                                       "OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0";



    // Define the tool prefix name
    const std::string elSFPrefix = "AsgElectronEfficiencyCorrectionTool_";

    ATH_MSG_INFO("Setting up Electrons SF tool for TOTAL correlation model");
    // Configure the tools with the maps - Name, map, reco_key, ID_key, iso_key, trigger_key, data_type, for the TOTAL
    // correlation model
    // Reco SFs
    m_electronEffSFReco = setupElectronSFToolWithMap(elSFPrefix + "Reco", m_electronEffSFRecoFile, "Reconstruction", "",
                                                     "", "", dataType, "TOTAL", "", "");
    // ID SFs
    m_electronEffSFID = setupElectronSFToolWithMap(elSFPrefix + "ID", m_electronEffSFIDFile, "", electronID, "", "",
                                                   dataType, "TOTAL", "", "");
    m_electronEffSFIDLoose = setupElectronSFToolWithMap(elSFPrefix + "IDLoose", m_electronEffSFIDLooseFile, "",
                                                        electronIDLoose, "", "", dataType, "TOTAL", "", "");
    // Trigger SFs
    m_electronEffSFTrigger = setupElectronSFToolWithMap(elSFPrefix + "TriggerSF", m_electronEffSFTriggerFile, "",
                                                        electronID, electronIsolation, trigger_string, dataType,
                                                        "TOTAL", "", "");
    m_electronEffSFTriggerLoose = setupElectronSFToolWithMap(elSFPrefix + "TriggerSFLoose",
                                                             m_electronEffSFTriggerLooseFile, "", electronIDLoose,
                                                             electronIsolationLoose, trigger_string, dataType, "TOTAL",
                                                             "", "");
    // Trigger Efficiencies
    m_electronEffTrigger = setupElectronSFToolWithMap(elSFPrefix + "Trigger", m_electronEffTriggerFile, "", electronID,
                                                      electronIsolation, "Eff_" + trigger_string, dataType, "TOTAL", "",
                                                      "");
    m_electronEffTriggerLoose = setupElectronSFToolWithMap(elSFPrefix + "TriggerLoose", m_electronEffTriggerLooseFile,
                                                           "", electronIDLoose, electronIsolationLoose,
                                                           "Eff_" + trigger_string, dataType, "TOTAL", "", "");
    // Isolation SFs
    m_electronEffSFIso = setupElectronSFToolWithMap(elSFPrefix + "Iso", m_electronEffSFIsoFile, "", electronID,
                                                    electronIsolation, "", dataType, "TOTAL", "", "");
    m_electronEffSFIsoLoose = setupElectronSFToolWithMap(elSFPrefix + "IsoLoose", m_electronEffSFIsoLooseFile, "",
                                                         electronID, electronIsolationLoose, "", dataType, "TOTAL", "",
                                                         "");

    ATH_MSG_INFO(
      "Requested Electrons SF tool for " << m_config->electronEfficiencySystematicModel() << " correlation model");

    if (m_config->electronEfficiencySystematicModel() != "TOTAL") {
      ATH_MSG_INFO(
        "Setting up Electrons SF tool for " << m_config->electronEfficiencySystematicModel() << " correlation model");
      const std::string elSFPrefixCorrModel = elSFPrefix + "CorrModel_";
      // Reco SFs
      m_electronEffSFRecoCorrModel = setupElectronSFToolWithMap(elSFPrefixCorrModel + "Reco", m_electronEffSFRecoFile,
                                                                "Reconstruction", "", "", "", dataType,
                                                                m_config->electronEfficiencySystematicModel(),
                                                                m_config->electronEfficiencySystematicModelEtaBinning(),
                                                                m_config->electronEfficiencySystematicModelEtBinning());
      // ID SFs
      m_electronEffSFIDCorrModel = setupElectronSFToolWithMap(elSFPrefixCorrModel + "ID", m_electronEffSFIDFile, "",
                                                              electronID, "", "", dataType,
                                                              m_config->electronEfficiencySystematicModel(),
                                                              m_config->electronEfficiencySystematicModelEtaBinning(),
                                                              m_config->electronEfficiencySystematicModelEtBinning());
      m_electronEffSFIDLooseCorrModel = setupElectronSFToolWithMap(elSFPrefixCorrModel + "IDLoose",
                                                                   m_electronEffSFIDLooseFile, "", electronIDLoose, "",
                                                                   "", dataType,
                                                                   m_config->electronEfficiencySystematicModel(),
                                                                   m_config->electronEfficiencySystematicModelEtaBinning(),
                                                                   m_config->electronEfficiencySystematicModelEtBinning());
      // Trigger SFs
      m_electronEffSFTriggerCorrModel = setupElectronSFToolWithMap(elSFPrefixCorrModel + "TriggerSF",
                                                                   m_electronEffSFTriggerFile, "", electronID,
                                                                   electronIsolation, trigger_string, dataType,
                                                                   m_config->electronEfficiencySystematicModel(),
                                                                   m_config->electronEfficiencySystematicModelEtaBinning(),
                                                                   m_config->electronEfficiencySystematicModelEtBinning());
      m_electronEffSFTriggerLooseCorrModel = setupElectronSFToolWithMap(elSFPrefixCorrModel + "TriggerSFLoose",
                                                                        m_electronEffSFTriggerLooseFile, "",
                                                                        electronIDLoose, electronIsolationLoose,
                                                                        trigger_string, dataType,
                                                                        m_config->electronEfficiencySystematicModel(),
                                                                        m_config->electronEfficiencySystematicModelEtaBinning(),
                                                                        m_config->electronEfficiencySystematicModelEtBinning());
      // Trigger Efficiencies
      m_electronEffTriggerCorrModel = setupElectronSFToolWithMap(elSFPrefixCorrModel + "Trigger",
                                                                 m_electronEffTriggerFile, "", electronID,
                                                                 electronIsolation, "Eff_" + trigger_string, dataType,
                                                                 m_config->electronEfficiencySystematicModel(),
                                                                 m_config->electronEfficiencySystematicModelEtaBinning(),
                                                                 m_config->electronEfficiencySystematicModelEtBinning());
      m_electronEffTriggerLooseCorrModel = setupElectronSFToolWithMap(elSFPrefixCorrModel + "TriggerLoose",
                                                                      m_electronEffTriggerLooseFile, "",
                                                                      electronIDLoose, electronIsolationLoose,
                                                                      "Eff_" + trigger_string, dataType,
                                                                      m_config->electronEfficiencySystematicModel(),
                                                                      m_config->electronEfficiencySystematicModelEtaBinning(),
                                                                      m_config->electronEfficiencySystematicModelEtBinning());
      // Isolation SFs
      m_electronEffSFIsoCorrModel = setupElectronSFToolWithMap(elSFPrefixCorrModel + "Iso", m_electronEffSFIsoFile, "",
                                                               electronID, electronIsolation, "", dataType,
                                                               m_config->electronEfficiencySystematicModel(),
                                                               m_config->electronEfficiencySystematicModelEtaBinning(),
                                                               m_config->electronEfficiencySystematicModelEtBinning());
      m_electronEffSFIsoLooseCorrModel = setupElectronSFToolWithMap(elSFPrefixCorrModel + "IsoLoose",
                                                                    m_electronEffSFIsoLooseFile, "", electronID,
                                                                    electronIsolationLoose, "", dataType,
                                                                    m_config->electronEfficiencySystematicModel(),
                                                                    m_config->electronEfficiencySystematicModelEtaBinning(),
                                                                    m_config->electronEfficiencySystematicModelEtBinning());
    }

    if (m_config->useFwdElectrons()) {
      ATH_MSG_INFO("Setting up forward Electrons SF tool");

      m_fwdElectronEffSFIDFile = electronSFMapFilePath("FWDID");
      m_fwdElectronEffSFIDLooseFile = electronSFMapFilePath("FWDID");
      m_fwdElectronEffSFID = setupElectronSFToolWithMap("AsgFwdElectronEfficiencyCorrectionTool_ID",
                                                        m_fwdElectronEffSFIDFile, "",
                                                        "Fwd" + m_config->fwdElectronID(), "", "", dataType, "TOTAL", "",
                                                        "");
      m_fwdElectronEffSFIDLoose = setupElectronSFToolWithMap("AsgFwdElectronEfficiencyCorrectionTool_IDLoose",
                                                             m_fwdElectronEffSFIDLooseFile, "",
                                                             "Fwd" + m_config->fwdElectronIDLoose(), "", "", dataType, "TOTAL", "",
                                                             "");

      ATH_MSG_INFO("Finished setting up forward Electrons SF tool");
    }


    // Charge ID cannot use maps at the moment so we default to the old method
    if (m_config->useElectronChargeIDSelection()) { // We need to update the implementation according to new
                                                    // recommendations
      ATH_MSG_INFO("Setting up Electrons ChargeID SF tool");
      // Charge ID file (no maps)
      m_electronEffSFChargeIDFile = electronSFFilePath("ChargeID", electronID, electronIsolation);
      if (m_config->applyTightSFsInLooseTree()) // prevent crash on-supported loose electron WPs with ECIDS
        m_electronEffSFChargeIDLooseFile = electronSFFilePath("ChargeID", electronID, electronIsolation);
      else m_electronEffSFChargeIDLooseFile = electronSFFilePath("ChargeID", electronIDLoose, electronIsolationLoose);
      // The tools want the files in vectors: remove this with function
      std::vector<std::string> inChargeID {
        m_electronEffSFChargeIDFile
      };
      std::vector<std::string> inChargeIDLoose {
        m_electronEffSFChargeIDLooseFile
      };
      // Charge Id efficiency scale factor
      m_electronEffSFChargeID = setupElectronSFTool(elSFPrefix + "ChargeID", inChargeID, dataType);
      m_electronEffSFChargeIDLoose = setupElectronSFTool(elSFPrefix + "ChargeIDLoose", inChargeIDLoose, dataType);
    }
    // Charge flip correction: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EgammaChargeMisIdentificationTool
    CP::ElectronChargeEfficiencyCorrectionTool* ChargeMisIDCorrections = new CP::ElectronChargeEfficiencyCorrectionTool(
      "ElectronChargeEfficiencyCorrection");
    CP::ElectronChargeEfficiencyCorrectionTool* ChargeMisIDCorrectionsLoose =
      new CP::ElectronChargeEfficiencyCorrectionTool("ElectronChargeEfficiencyCorrectionLoose");
    m_electronEffSFChargeMisIDFile = electronSFFilePath("ChargeMisID", electronID, electronIsolation);
    m_electronEffSFChargeMisIDLooseFile = electronSFFilePath("ChargeMisID", electronIDLoose, electronIsolationLoose);
    top::check(ChargeMisIDCorrections->setProperty("CorrectionFileName",
                                                   m_electronEffSFChargeMisIDFile), "Failed to setProperty");
    top::check(ChargeMisIDCorrections->initialize(), "Failed to setProperty");
    top::check(ChargeMisIDCorrectionsLoose->setProperty("CorrectionFileName",
                                                        m_electronEffSFChargeMisIDLooseFile), "Failed to setProperty");
    top::check(ChargeMisIDCorrectionsLoose->initialize(), "Failed to setProperty");

    return StatusCode::SUCCESS;
  }

  IAsgElectronEfficiencyCorrectionTool*
  EgammaCPTools::setupElectronSFTool(const std::string& name, const std::vector<std::string>& file_list,
                                     const int& data_type) {
    IAsgElectronEfficiencyCorrectionTool* tool = nullptr;

    if (asg::ToolStore::contains<IAsgElectronEfficiencyCorrectionTool>(name)) {
      tool = asg::ToolStore::get<IAsgElectronEfficiencyCorrectionTool>(name);
    } else {
      if (!file_list.empty()) {  // If the file list is empty do nothing
        tool = new AsgElectronEfficiencyCorrectionTool(name);
        top::check(asg::setProperty(tool, "CorrectionFileNameList", file_list),
                   "Failed to set CorrectionFileNameList to " + name);
        top::check(asg::setProperty(tool, "ForceDataType", data_type),
                   "Failed to set ForceDataType to " + name);
        top::check(asg::setProperty(tool, "CorrelationModel", "TOTAL"),
                   "Failed to set CorrelationModel to " + name);
        top::check(tool->initialize(), "Failed to initialize " + name);
      }
    }
    return tool;
  }

  void EgammaCPTools::setCorrelationModelBinning(IAsgElectronEfficiencyCorrectionTool* tool,
                                                 const std::string& binningName, const std::string& binning) {
    std::vector<std::string> tokens;
    top::tokenize(binning, tokens, ":");
    if (tokens.size() < 1) {
      ATH_MSG_ERROR(
        "EgammaCPTools::setupElectronSFToolWithMap, correlation model " << binningName <<
          " binning must be in the form XXX:YYY:WWW:ZZZ...");
    }
    std::vector<float> bins;
    for (unsigned int i = 0; i < tokens.size(); i++) {
      std::string token = tokens[i];
      float value = 0.;
      try{
        value = std::stof(token);
      }
      catch (...) {
        throw std::invalid_argument {
                "EgammaCPTools::setupElectronSFToolWithMap, correlation model " + binningName +
                " binning must be in the for XXX:YYY:WWW:ZZZ, couldn't convert correctly to float"
        };
      }
      bins.push_back(value);
    }
    ATH_MSG_INFO(" ---> electron SF tools will use " << binningName << " bins:");
    for (unsigned int i = 0; i < bins.size(); i++) ATH_MSG_INFO("       " << bins[i]);
    top::check(asg::setProperty(tool, binningName,
                                bins), "Failed to set correlation model " + binningName + " binning to " + binning);
  }

  IAsgElectronEfficiencyCorrectionTool*
  EgammaCPTools::setupElectronSFToolWithMap(const std::string& name, const std::string& map_path,
                                            const std::string& reco_key, const std::string& ID_key,
                                            const std::string& iso_key, const std::string& trigger_key,
                                            const int& data_type, const std::string& correlation_model,
                                            const std::string& correlationModelEtaBinning,
                                            const std::string& correlationModelEtBinning) {
    std::string infoStr = "Configuring : name=" + name + " map=" + map_path + " reco_key=" + reco_key + " ID_key=" +
                          ID_key + " iso_key=" + iso_key + " trigger_key=" + trigger_key + "data_type=" +
                          std::to_string(data_type) +
                          " correlation_model=" + correlation_model + " etaBinning=" + correlationModelEtaBinning +
                          " etBinning=" +
                          correlationModelEtBinning;
    ATH_MSG_INFO(infoStr);
    IAsgElectronEfficiencyCorrectionTool* tool = nullptr;
    if (asg::ToolStore::contains<IAsgElectronEfficiencyCorrectionTool>(name)) {
      tool = asg::ToolStore::get<IAsgElectronEfficiencyCorrectionTool>(name);
    } else {
      tool = new AsgElectronEfficiencyCorrectionTool(name);
      // Give the full map path
      top::check(asg::setProperty(tool, "MapFilePath", map_path), "Failed to set MapFilePath to " + name);
      // Set the data type for all tools
      top::check(asg::setProperty(tool, "ForceDataType", data_type), "Failed to set ForceDataType to " + name);
      // Set the correlation model for all tools
      top::check(asg::setProperty(tool, "CorrelationModel",
                                  correlation_model), "Failed to set CorrelationModel to " + name);

      if (correlationModelEtaBinning != "" && correlationModelEtaBinning != "default") this->setCorrelationModelBinning(
          tool, "UncorrEtaBinsUser", correlationModelEtaBinning);
      if (correlationModelEtBinning != "" && correlationModelEtBinning != "default") this->setCorrelationModelBinning(
          tool, "UncorrEtBinsUser", correlationModelEtBinning);

      // Set the keys which configure the tool options (empty string means we do not include this key)
      if (reco_key != "" && reco_key != "None") {
        ATH_MSG_INFO(" Adding RecoKey    : " + reco_key);
        top::check(asg::setProperty(tool, "RecoKey", reco_key), "Failed to set RecoKey to " + name);
      }
      if (ID_key != "" && ID_key != "None") {
        std::string id_key = mapWorkingPoints(ID_key);
        ATH_MSG_INFO(" Adding IDKey      : " + id_key);
        top::check(asg::setProperty(tool, "IdKey", id_key), "Failed to set IdKey to " + name);
      }
      if (iso_key != "" && iso_key != "None") {
        ATH_MSG_INFO(" Adding IsoKey     : " + iso_key);
        top::check(asg::setProperty(tool, "IsoKey", iso_key), "Failed to set IsoKey to " + name);
      }
      if (trigger_key != "" && trigger_key != "None") {
        ATH_MSG_INFO(" Adding TriggerKey : " + trigger_key);
        top::check(asg::setProperty(tool, "TriggerKey", trigger_key), "Failed to set TriggerKey to " + name);
      }
      // Initialise this tool
      top::check(tool->initialize(), "Failed to initialize " + name);
    }
    return tool;
  }

  std::string EgammaCPTools::electronSFFilePath(const std::string& type, const std::string& ID,
                                                const std::string& ISO) {
    const std::string el_calib_path = "ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/";

    std::string file_path;

    if (type == "reco") {
      ATH_MSG_ERROR("Moved to using egamma maps for configuring scale factor tools - electronSFMapFilePath");
    } else if (type == "ID") {
      ATH_MSG_ERROR("Moved to using egamma maps for configuring scale factor tools - electronSFMapFilePath");
    } else if (type == "triggerSF") {
      ATH_MSG_ERROR("Moved to using egamma maps for configuring scale factor tools - electronSFMapFilePath");
    } else if (type == "triggerEff") {
      ATH_MSG_ERROR("Moved to using egamma maps for configuring scale factor tools - electronSFMapFilePath");
    } else if (type == "ChargeID") {
      if (ID != "MediumLLH" && ID != "TightLLH") ATH_MSG_ERROR(
          "The requested ID WP (" + ID +
        ") is not supported for electron ChargeID SFs! Try TightLH or MediumLH instead.");
      if (ISO != "FCTight" && ISO != "Gradient") ATH_MSG_ERROR("The requested ISO WP (" + ISO + ") is not supported for electron ChargeID SFs! Try FCTight or Gradient instead.");
      file_path += "additional/efficiencySF.ChargeID.";
      file_path += ID;
      file_path += "_d0z0_v13_";
      file_path += ISO;
      file_path += "_ECIDSloose.root";
      file_path = el_calib_path + file_path;
    } else if (type == "ChargeMisID") {
      // Protect against "None" Iso key
      std::string iso = ISO;
      if (iso == "None") iso = "";
      // Protect against Loose ID + any Iso
      if (ID == "LooseAndBLayerLLH") iso = "";
      file_path = "charge_misID/";
      file_path += "chargeEfficiencySF.";
      file_path += ID;
      file_path += "_d0z0_v13";
      if (iso != "") file_path += "_" + iso;
      if (m_config->useElectronChargeIDSelection()) {
        if (ID != "MediumLLH" && ID != "TightLLH") ATH_MSG_WARNING("The requested ID WP (" + ID + ") is not supported for electron ECIDS+ChargeMisID SFs! Try TightLH or MediumLH instead. Will now switch to regular ChargeMisID SFs.");
        else if (iso != "FCTight" && iso != "Gradient") ATH_MSG_WARNING("The requested ISO WP (" + iso + ") is not supported for electron ECIDS+ChargeMisID SFs! Try FCTight or Gradient instead. Will now switch to regular ChargeMisID SFs.");
        else file_path += "_ECIDSloose";
      }
      file_path += ".root";
      file_path = el_calib_path + file_path;
    } else {
      ATH_MSG_ERROR("Unknown electron SF type");
    }
    return PathResolverFindCalibFile(file_path);
  }

  std::string EgammaCPTools::electronSFMapFilePath(const std::string& type) {
    // Store here the paths to maps which may be updated with new recommendations
    // Currently can use maps for reco, id, iso, trigger but not ChargeID
    const std::string el_calib_path = "ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/";

    std::string file_path;
    if (type == "reco") {
      file_path = "map3.txt";
    } else if (type == "ID") {
      file_path = "map3.txt";
    } else if (type == "FWDID") {
      file_path = "map3.txt";
    } else if (type == "isolation") {
      file_path = "map3.txt";
    } else if (type == "trigger") {
      file_path = "map3.txt";
    } else if (type == "ChargeID") {
      ATH_MSG_ERROR("Use electronSFFilePath method until ChargeID is supported by maps");
    } else if (type == "ChargeMisID") {
      ATH_MSG_ERROR("Use electronSFFilePath method until ChargeMisID is supported by maps");
    } else {
      ATH_MSG_ERROR("Unknown electron SF type");
    }
    return PathResolverFindCalibFile(el_calib_path + file_path);
  }

  std::string EgammaCPTools::mapWorkingPoints(const std::string& type) {
    // Ian Connelly - 27 Sept 2017
    // When moving to the maps, the working points are converted to a nicer format
    // We will provide a mapping from the names used by analysers and the map WP names to prevent cutfiles breaking
    // See :
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/XAODElectronEfficiencyCorrectionTool#Configuration_of_the_tool_using
    std::string working_point = "";
    // ID
    if (type == "LooseAndBLayerLLH" || type == "LooseBLayer") {
      working_point = "LooseBLayer";
    }
    if (type == "MediumLLH" || type == "Medium") {
      working_point = "Medium";
    }
    if (type == "TightLLH" || type == "Tight") {
      working_point = "Tight";
    }
    if (type == "FwdLoose" || type == "FwdMedium" || type == "FwdTight") working_point = type;

    // Temporary ISO map to handle the mess that is EGamma+IFF right now...
    if (type.find("Pflow") != std::string::npos) {
      ATH_MSG_WARNING("You selected a Pflow isolation WP for at least one of your electron collections - BE WARNED THAT THESE ARE NOT YET READY TO BE RELEASED FOR USE IN PHYSICS ANALYSES AND OF COURSE DON'T HAVE ASSOCIATED SCALE FACTORS YET!!!");
      if (type == "PflowLoose") working_point = "FCLoose";
      if (type == "PflowTight") working_point = "FCTight";
    }
    if (type == "Tight") working_point = "FCTight";
    if (type == "Loose") working_point = "FCLoose";
    if (type == "HighPtCaloOnly") working_point = "FCHighPtCaloOnly";
    if (type == "TightTrackOnly") working_point = "Gradient";
    if (type == "TightTrackOnly_FixedRad") working_point = "Gradient";
    if (type == "FCTight" || type == "FCLoose" || type == "FCHighPtCaloOnly" || type == "Gradient") working_point = type;

    return working_point;
  }
}  // namespace top
