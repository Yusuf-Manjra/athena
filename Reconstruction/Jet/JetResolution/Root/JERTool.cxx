/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STD includes
#include <sstream>

// ROOT includes
#include "TROOT.h"
#include "TSystem.h"

// EDM includes
#include "xAODJet/Jet.h"

// Non-local includes
#include "PathResolver/PathResolver.h"

// Local includes
#include "JetResolution/JERTool.h"

// Just some constants for unit conversion
const double GeV = 1.e3;
const double TeV = 1.e6;
const double invGeV = 1.e-3;
const double invTeV = 1.e-6;

//-----------------------------------------------------------------------------
// Standard ASG tool constructor
//-----------------------------------------------------------------------------
JERTool::JERTool(const std::string& name)
    : asg::AsgMetadataTool(name),
      m_etaAxis(NULL),
      m_jetAlgo(JETALGO_UNDEFINED),
      m_inputFile(NULL)
{
  declareProperty("PlotFileName",
                  m_fileName = "JetResolution/JERProviderPlots_2012.root");
  declareProperty("CollectionName", m_collectionName = "AntiKt4LCTopoJets");
  declareProperty("JERMethod", m_jerMethod = "Truth",
                  "Measurement method of the JER: {Truth}");
  // Auto-config is only applied in Athena
  declareProperty("UseAutoConfiguration", m_useAutoConfig = true,
                  "Configure BeamEnergy and SimulationType from Metadata");
  declareProperty("BeamEnergy", m_beamEnergy = "8TeV",
                  "Specify the sqrt(s) of the data/mc: {7TeV, 8TeV}");
  declareProperty("SimulationType", m_simulationType = "FullSim",
                  "Specify simulation type: {FullSim, AFII}");
  declareProperty("ErrorMethod", m_errorMethod = "Default",
                  "Specify error calculation: {Default, Alternate}");
}

//-----------------------------------------------------------------------------
JERTool::JERTool(const std::string& toolName, const std::string& plotFileName,
                 const std::string& jetCollection)
    : JERTool(toolName)
{
  setFileName(plotFileName);
  setJetCollection(jetCollection);
}

//-----------------------------------------------------------------------------
// Copy constructor - necessary for reflex in Athena.
// I'm not sure exactly what the use cases might be, so I'll just implement a
// shallow copy for now. I should come back to this later.
//-----------------------------------------------------------------------------
JERTool::JERTool(const JERTool& other)
    : asg::AsgMetadataTool(other.name() + "_copy"),
      m_etaAxis(other.m_etaAxis),
      m_useAutoConfig(other.m_useAutoConfig),
      m_fileName(other.m_fileName),
      m_collectionName(other.m_collectionName),
      m_jerMethod(other.m_jerMethod),
      m_beamEnergy(other.m_beamEnergy),
      m_simulationType(other.m_simulationType),
      m_errorMethod(other.m_errorMethod),
      m_is7TeV(other.m_is7TeV),
      m_isAFII(other.m_isAFII),
      m_useAltErr(other.m_useAltErr),
      m_jetAlgo(other.m_jetAlgo),
      m_inputFile(other.m_inputFile),
      m_jerFunc(other.m_jerFunc),
      m_jerFuncAFII(other.m_jerFuncAFII),
      m_jerData(other.m_jerData),
      m_errorData(other.m_errorData),
      m_jerMC(other.m_jerMC),
      m_errorMC(other.m_errorMC),
      m_diffDataMC(other.m_diffDataMC),
      m_errorDataMC(other.m_errorDataMC)
{}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
JERTool::~JERTool()
{
  if(m_etaAxis) delete m_etaAxis;
  // Clean up memory that was cloned from the input file
  for(auto mapPair : m_jerFunc)
    if(mapPair.second) delete mapPair.second;
  for(auto mapPair : m_jerFuncAFII)
    if(mapPair.second) delete mapPair.second;
  for(auto mapPair : m_jerMC)
    if(mapPair.second) delete mapPair.second;
  for(auto mapPair : m_errorMC)
    if(mapPair.second) delete mapPair.second;
  for(auto mapPair : m_diffDataMC)
    if(mapPair.second) delete mapPair.second;
  for(auto mapPair : m_errorDataMC)
    if(mapPair.second) delete mapPair.second;
}

//-----------------------------------------------------------------------------
// Initialize the tool
//-----------------------------------------------------------------------------
StatusCode JERTool::initialize()
{
  // Greet the user
  ATH_MSG_INFO("Initializing JER provider");

  // Auto-configuration currently only works in Athena
  #ifdef ASGTOOL_ATHENA
  if(m_useAutoConfig){
    ATH_CHECK( autoConfigure() );
  }
  #endif

  // Process the configuration flags
  ATH_CHECK( parseConfiguration() );

  // Print some information
  std::string description = m_collectionName;
  if     (m_jetAlgo == AKt4LC) description = "AntiKt4 LCW+JES";
  else if(m_jetAlgo == AKt4EM) description = "AntiKt4 EM+JES";
  else if(m_jetAlgo == AKt6LC) description = "AntiKt6 LCW+JES";
  else if(m_jetAlgo == AKt6EM) description = "AntiKt6 EM+JES";
  ATH_MSG_INFO("Retrieving JER for jets: " << description);

  // Load the inputs
  ATH_CHECK( loadJERInputs() );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Attempt auto-configuration of tool using Metadata. Athena only.
//-----------------------------------------------------------------------------
StatusCode JERTool::autoConfigure()
{
  // Identify sample as MC or data
  bool isData = false;
  std::string projectName;
  ATH_CHECK( retrieveMetadata("/TagInfo", "project_name", projectName) );
  ATH_MSG_DEBUG("Retrieved project name: " << projectName);
  if(projectName.find("data") != std::string::npos){
    ATH_MSG_DEBUG("Identified sample as data.");
    isData = true;
  }

  // Try to retrieve the beam energy
  float beamEnergy(0);
  ATH_CHECK( retrieveMetadata("/TagInfo", "beam_energy", beamEnergy) );
  ATH_MSG_DEBUG("Retrieved beam energy: " << beamEnergy*invTeV << " TeV");
  // Convert into known string
  int collisionEnergyTeV = int(beamEnergy*invTeV*2);
  if(collisionEnergyTeV == 8) m_beamEnergy = "8TeV";
  else if(collisionEnergyTeV == 7) m_beamEnergy = "7TeV";
  // 13 TeV placeholder
  else if(collisionEnergyTeV == 13){
    ATH_MSG_WARNING("No resolution measurements available yet for 13 TeV. " <<
                    "Auto-config will apply 8 TeV results instead.");
    m_beamEnergy = "8TeV";
  }
  else{
    ATH_MSG_ERROR("Unknown collision energy retrieved from Metadata: "
                  << collisionEnergyTeV << " TeV");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Overriding collision energy from Metadata: " << m_beamEnergy);

  // Try to retrieve the simulation type. Only works on MC
  if(!isData){
    std::string simulationFlavor;
    ATH_CHECK( retrieveMetadata("/Simulation/Parameters", "SimulationFlavour",
                                simulationFlavor) );
    ATH_MSG_DEBUG("Retrieved simulation flavor: " << simulationFlavor);
    // Convert into known string
    if(simulationFlavor == "default") m_simulationType = "FullSim";
    else if(simulationFlavor == "AtlfastII") m_simulationType = "AFII";
    else{
      ATH_MSG_ERROR("Unknown simulation type retrieved from Metadata: "
                    << simulationFlavor);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Overriding simulation type from Metadata: "
                 << m_simulationType);
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Parse configuration
//-----------------------------------------------------------------------------
StatusCode JERTool::parseConfiguration()
{
  // Beam energy
  if(m_beamEnergy == "8TeV")
    m_is7TeV = false;
  else if(m_beamEnergy == "7TeV")
    m_is7TeV = true;
  else{
    ATH_MSG_ERROR("No available configuration for beam energy "
                  << m_beamEnergy);
    return StatusCode::FAILURE;
  }

  // Simulation type
  if(m_simulationType == "FullSim")
    m_isAFII = false;
  else if(m_simulationType == "AFII")
    m_isAFII = true;
  else{
    ATH_MSG_ERROR("No available configuration for simulation type "
                  << m_simulationType);
    return StatusCode::FAILURE;
  }

  // Error method
  if(m_errorMethod == "Default")
    m_useAltErr = false;
  else if(m_errorMethod == "Alternate")
    m_useAltErr = true;
  else{
    ATH_MSG_ERROR("No available configuration for error method "
                  << m_errorMethod);
    return StatusCode::FAILURE;
  }

  // Determine the jet algorithm
  if(m_collectionName.find("AntiKt4LC") != std::string::npos)
    m_jetAlgo = AKt4LC;
  else if(m_collectionName.find("AntiKt4EM") != std::string::npos)
    m_jetAlgo = AKt4EM;
  else if(m_collectionName.find("AntiKt6LC") != std::string::npos)
    m_jetAlgo = AKt6LC;
  else if(m_collectionName.find("AntiKt6EM") != std::string::npos)
    m_jetAlgo = AKt6EM;
  else{
    ATH_MSG_FATAL("No support for JER of " << m_collectionName << " jets!");
    return StatusCode::FAILURE;
  }

  // Currently only Truth method is supported
  if(m_jerMethod != "Truth"){
    ATH_MSG_FATAL("JER method unsupported! Only Truth " <<
                  "is currently supported!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Load JER results from input file
//-----------------------------------------------------------------------------
StatusCode JERTool::loadJERInputs()
{
  // This hardcoded binning needs to be improved
  double etaBins[7] = {0, 0.8, 1.2, 2.1, 2.8, 3.6, 4.5};
  m_etaAxis = new TAxis(6, etaBins);
  std::string regions[m_nEtaBins] = {"_0","_1","_2","_3","_4","_5"};

  // Open input file using PathResolver
  ATH_MSG_DEBUG("Using JER file " << m_fileName);
  m_inputFile = new TFile(PathResolverFindCalibFile(m_fileName).c_str());
  if(!m_inputFile->IsOpen()){
    ATH_MSG_FATAL("JER input file " << m_fileName << " could not be found!");
    return StatusCode::FAILURE;
  }

  // Graph prefix name
  std::string algString;
  if     (m_jetAlgo == AKt4LC) algString = "AntiKt4LCTopoJES";
  else if(m_jetAlgo == AKt4EM) algString = "AntiKt4TopoJES";
  else if(m_jetAlgo == AKt6LC) algString = "AntiKt6LCTopoJES";
  else if(m_jetAlgo == AKt6EM) algString = "AntiKt6TopoJES";
  std::string graphPrefix = m_jerMethod + "_" + algString;

  // Convenience macro for checking status code below.
  // Pull object from file
  #define GETCHECK(type, dest, name)                  \
    do {                                              \
      if(!pullFromFile<type>(name, dest).isSuccess()) \
        return StatusCode::FAILURE;                   \
    } while(false)

  // Retrieve the 8 TeV JER
  if(!m_is7TeV){
    // Loop over the eta regions
    for(unsigned int i = 0; i < m_nEtaBins; ++i){
      std::string suffix = algString + regions[i];
      GETCHECK(TF1, m_jerFunc[i], graphPrefix + regions[i]);
      GETCHECK(TF1, m_jerFuncAFII[i], "TruthAF2_" + suffix);

      // (only up to 2.8 due to statistics) ---> m_nEtaBins-2
      // TODO: write this in a cleaner, safer way
      if ( i < m_nEtaBins - 2 ) {
        GETCHECK(TGraphErrors, m_jerMC[i], "BisectorFit_" + suffix);
        GETCHECK(TGraphErrors, m_jerData[i], "BisectorDataFit_" + suffix);
        GETCHECK(TGraph, m_errorMC[i], "BisectorFitUNCERT_" + suffix);
        GETCHECK(TGraph, m_errorData[i], "BisectorDataFitUNCERT_" + suffix);
      }
    }
  }

  // Retrieve the 7 TeV JER
  else{
    // Loop over the eta regions
    for(unsigned int i = 0; i < m_nEtaBins; ++i){
      std::string suffix = algString + regions[i];
      GETCHECK(TF1, m_jerFunc[i], graphPrefix + regions[i]);

      // (only up to 2.8 due to statistics) ---> m_nEtaBins-2
      // TODO: write this in a cleaner, safer way
      if ( i < m_nEtaBins - 2 ) {
        GETCHECK(TGraphErrors, m_diffDataMC[i], "DataMCBisector_" + suffix);
        GETCHECK(TGraph, m_errorDataMC[i], "DataMCBisectorUNCERT_" + suffix);
      }
    }
  }

  // Now close the input file
  m_inputFile->Close();
  delete m_inputFile;
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Extract an object from the input file
//-----------------------------------------------------------------------------
template<class T> StatusCode JERTool::pullFromFile(std::string name, T*& obj)
{
  if(m_inputFile == NULL || !m_inputFile->IsOpen())
    return StatusCode::FAILURE;
  T* inputObj = dynamic_cast<T*>(m_inputFile->Get(name.c_str()));
  if(inputObj == NULL){
    ATH_MSG_FATAL("Unable to retrieve " << T::Class()->GetName() <<
                  " with name " << name);
    return StatusCode::FAILURE;
  }
  // Clone the object
  obj = (T*) inputObj->Clone(name.c_str());
  // Remove TF1s from ROOT's control, to avoid ROOT
  // cleaning them up early in PyROOT.
  if(T::Class()->InheritsFrom("TF1"))
    gROOT->GetListOfFunctions()->Remove(obj);
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Extract an in situ measurement from one of the TGraph maps.
// All of these measurements use truncations in eta and pt.
//-----------------------------------------------------------------------------
template<class T> double JERTool::getInsituMeasurement(double pt, double eta,
                                                       std::map<int, T*> graphMap)
{
  const double GeV = 1.e3;
  const double invGeV = 1.e-3;
  // Truncate eta bin due to lacking stats in data/mc
  int etaBin = std::min(getEtaBin(eta), 3);
  pt = std::min(1000.*GeV, pt);
  if(fabs(eta) > 2.1 && pt > 300.*GeV) pt = 300.*GeV;
  return graphMap[etaBin]->Eval(pt*invGeV);
}

//-----------------------------------------------------------------------------
// Resolution retrieval methods
// With the new EDM, we shouldn't need sanity checks for the input units
//-----------------------------------------------------------------------------

// Get eta bin corresponding for this jet.
// Indexing convention of eta regions is off-by-one
// with respect to the TAxis (etaBin = TAxisBin - 1)
int JERTool::getEtaBin(const xAOD::Jet* jet)
{ return getEtaBin(jet->eta()); }

int JERTool::getEtaBin(double eta)
{
  int bin = m_etaAxis->FindBin(fabs(eta));
  bin = std::min((int) m_nEtaBins, bin); // include overflow
  return bin - 1;
}

// Get the offset from data/MC
double JERTool::getOffset(const xAOD::Jet* jet)
{ return getOffset(jet->pt(), jet->eta()); }

double JERTool::getOffset(double pt, double eta)
{
  return getInsituRelResolutionData(pt, eta) -
         getInsituRelResolutionMC(pt, eta);
}

// Read the resolution from the MC
double JERTool::getRelResolutionMC(const xAOD::Jet* jet)
{ return getRelResolutionMC(jet->pt(), jet->eta(), m_isAFII); }

double JERTool::getRelResolutionMC(double pt, double eta)
{ return getRelResolutionMC(pt, eta, m_isAFII); }

double JERTool::getRelResolutionMC(const xAOD::Jet* jet, bool isAFII)
{ return getRelResolutionMC(jet->pt(), jet->eta(), isAFII); }

double JERTool::getRelResolutionMC(double pt, double eta, bool isAFII)
{
  // Truncate pt to [10-2000] GeV
  pt = std::min(2.*TeV, std::max(10.*GeV, pt));
  std::map<int, TF1*>& jerFunc = isAFII ? m_jerFuncAFII : m_jerFunc;
  return jerFunc.at(getEtaBin(eta))->Eval(pt*invGeV);
}

// Read the "data truth" resolution, given by
// reso(MC FS truth) + reso(data in-situ) - reso(MC FS in-situ)
double JERTool::getRelResolutionData(const xAOD::Jet* jet)
{ return getRelResolutionData(jet->pt(), jet->eta()); }
double JERTool::getRelResolutionData(double pt, double eta)
{
  if(m_is7TeV){
    double sigmaMC = getRelResolutionMC(pt, eta);
    // Not sure why the factor of 0.01.
    // I guess the method returns value in percent..?
    double relOffset = getInsituDiffDataMC(pt, eta) * 0.01;
    return sigmaMC + relOffset * sigmaMC;
  }
  // Insitu measurements are Fullsim, so use the Fullsim
  // MC resolution when calculating the data resolution.
  const bool isAFII = false;
  return getRelResolutionMC(pt, eta, isAFII) + getOffset(pt, eta);
}

//-----------------------------------------------------------------------------
// Access to the in-situ measurements and their errors
// TODO: add 7TeV safety checks
//-----------------------------------------------------------------------------

// In-situ resolution in MC
double JERTool::getInsituRelResolutionMC(const xAOD::Jet* jet)
{ return getInsituRelResolutionMC(jet->pt(), jet->eta()); }
double JERTool::getInsituRelResolutionMC(double pt, double eta)
{ return getInsituMeasurement<TGraphErrors>(pt, eta, m_jerMC); }

// In-situ resolution in data
double JERTool::getInsituRelResolutionData(const xAOD::Jet* jet)
{ return getInsituRelResolutionData(jet->pt(), jet->eta()); }
double JERTool::getInsituRelResolutionData(double pt, double eta)
{ return getInsituMeasurement<TGraphErrors>(pt, eta, m_jerData); }

// In-situ uncertainty in MC
double JERTool::getInsituUncertMC(const xAOD::Jet* jet)
{ return getInsituUncertMC(jet->pt(), jet->eta()); }
double JERTool::getInsituUncertMC(double pt, double eta)
{ return getInsituMeasurement<TGraph>(pt, eta, m_errorMC); }

// In-situ uncertainty in data
double JERTool::getInsituUncertData(const xAOD::Jet* jet)
{ return getInsituUncertData(jet->pt(), jet->eta()); }
double JERTool::getInsituUncertData(double pt, double eta)
{ return getInsituMeasurement<TGraph>(pt, eta, m_errorData); }

// In-situ uncertainty in data and MC summed in quadrature
double JERTool::getInsituUncert(const xAOD::Jet* jet)
{ return getInsituUncert(jet->pt(), jet->eta()); }
double JERTool::getInsituUncert(double pt, double eta)
{
  return sqrt( pow(getInsituUncertMC(pt, eta), 2) +
               pow(getInsituUncertData(pt, eta), 2) );
}

//-----------------------------------------------------------------------------
// Calculate the resolution uncertainty
//-----------------------------------------------------------------------------
double JERTool::getUncertainty(const xAOD::Jet* jet)
{ return getUncertainty(jet, m_isAFII, m_useAltErr); }
//-----------------------------------------------------------------------------
double JERTool::getUncertainty(const xAOD::Jet* jet, bool isAFII, bool altErr)
{
  if(m_is7TeV){
    if(altErr || isAFII) ATH_MSG_ERROR("Alt/AFII option not available for 7 TeV");
    return getUncertainty_7TeV(jet);
  }

  // Low pt extrapolation issue in AFII
  double pt = jet->pt();
  double eta = jet->eta();
  if(isAFII && m_jetAlgo == AKt4LC && getEtaBin(eta) == 0 && pt < 25.*GeV){
    pt = 25.*GeV;
  }

  double insituFitErr = getInsituUncert(pt, eta);
  double jerData = getRelResolutionData(pt, eta);
  double jerMC = getRelResolutionMC(pt, eta, isAFII);

  // Is this the same as the getOffset method?
  double offset = jerData - jerMC;

  // AltErr method is a more optimistic uncertainty
  if(altErr){
    if(jerData < jerMC) offset = 0;
    return std::max(fabs(offset), insituFitErr);
  }

  // Default, conservative uncert
  return sqrt(insituFitErr*insituFitErr + offset*offset);
}

//-----------------------------------------------------------------------------
// The uncertainty, 2011
double JERTool::getUncertainty_7TeV(const xAOD::Jet* jet)
{
  int etaBin = getEtaBin(jet);
  // Setting boundaries due to lack of statistics
  float ptMin = 10.*GeV, ptMax = 1.*TeV, ptPiv = 2.*TeV;
  if(etaBin == 1){
    ptMax = 800.*GeV;
    ptPiv = 1.6*TeV;
  }
  if(etaBin > 2){
    ptMax = 500.*GeV;
    ptPiv = 1.*TeV;
  }

  // Get error in data-mc diff
  double dataMCError = getInsituDiffDataMCError(jet);
  // Get systematics
  double syst = getSystematics_7TeV(etaBin);

  // Above the validated range (no data available), we
  // double the uncertainty at ptPiv to be conservative
  double pt = jet->pt();
  float factor = 0;
  if(pt >= ptMin && pt <= ptMax) factor = 0; // redundant!
  else if(pt > ptMax && pt <= ptPiv) factor = (pt - ptMax)/(ptPiv - ptMax);
  else if(pt > ptPiv) factor = 1;

  double uncertainty = (1. + factor) *
                       sqrt(dataMCError * dataMCError + syst * syst) *
                       getRelResolutionData(jet);
  return uncertainty;
}

//-----------------------------------------------------------------------------
// 7 TeV systematic uncertainty
double JERTool::getSystematics_7TeV(int etaBin)
{
  double syst = 0;
  // Return the requested parametrization
  // Hardcoded values... Error prone!!
  if (etaBin == 0) {
    if     (m_jetAlgo == AKt6EM) syst = 0.09;
    else if(m_jetAlgo == AKt6LC) syst = 0.11;
    else if(m_jetAlgo == AKt4EM) syst = 0.17;
    else if(m_jetAlgo == AKt4LC) syst = 0.15;
  }
  else if (etaBin == 1) {
    if     (m_jetAlgo == AKt6EM) syst = 0.1;
    else if(m_jetAlgo == AKt6LC) syst = 0.10;
    else if(m_jetAlgo == AKt4EM) syst = 0.17;
    else if(m_jetAlgo == AKt4LC) syst = 0.15;
  }
  else if (etaBin == 2) {
    if     (m_jetAlgo == AKt6EM) syst = 0.12;
    else if(m_jetAlgo == AKt6LC) syst = 0.23;
    else if(m_jetAlgo == AKt4EM) syst = 0.19;
    else if(m_jetAlgo == AKt4LC) syst = 0.15;
  }
  else if (etaBin == 3) {
    if     (m_jetAlgo == AKt6EM) syst = 0.12;
    else if(m_jetAlgo == AKt6LC) syst = 0.23;
    else if(m_jetAlgo == AKt4EM) syst = 0.19;
    else if(m_jetAlgo == AKt4LC) syst = 0.19;
  }
  else if (etaBin == 4) {
    if     (m_jetAlgo == AKt6EM) syst = 0.13;
    else if(m_jetAlgo == AKt6LC) syst = 0.23;
    else if(m_jetAlgo == AKt4EM) syst = 0.19;
    else if(m_jetAlgo == AKt4LC) syst = 0.19;
  }
  else if (etaBin == 5) {
    if     (m_jetAlgo == AKt6EM) syst = 0.13;
    else if(m_jetAlgo == AKt6LC) syst = 0.23;
    else if(m_jetAlgo == AKt4EM) syst = 0.19;
    else if(m_jetAlgo == AKt4LC) syst = 0.19;
  }
  return syst;
}

// In-situ data-MC difference
double JERTool::getInsituDiffDataMC(const xAOD::Jet* jet)
{ return getInsituDiffDataMC(jet->pt(), jet->eta()); }
double JERTool::getInsituDiffDataMC(double pt, double eta)
{ return getInsituMeasurement<TGraphErrors>(pt, eta, m_diffDataMC); }

// In-situ uncertainty on the data-MC difference
double JERTool::getInsituDiffDataMCError(const xAOD::Jet* jet)
{ return getInsituDiffDataMCError(jet->pt(), jet->eta()); }
double JERTool::getInsituDiffDataMCError(double pt, double eta)
{
  // Special cases for high eta bins
  int etaBin = getEtaBin(eta);
  if(etaBin > 3){
    double diffError = 0;
    if(etaBin == 4) {
      // Hardcoded values... Error prone!!
      if      (m_jetAlgo == AKt6EM) diffError = 0.05;
      else if (m_jetAlgo == AKt6LC) diffError = 0.10;
      else if (m_jetAlgo == AKt4EM) diffError = 0.10;
      else if (m_jetAlgo == AKt4LC) diffError = 0.08;
    }
    else if(etaBin == 5) {
      // Hardcoded values... Error prone!!
      if      (m_jetAlgo == AKt6EM) diffError = 0.05;
      else if (m_jetAlgo == AKt6LC) diffError = 0.10;
      else if (m_jetAlgo == AKt4EM) diffError = 0.10;
      else if (m_jetAlgo == AKt4LC) diffError = 0.08;
    }
    return diffError;
  }
  // No explanation why the 0.01 factor in this case!
  return getInsituMeasurement<TGraph>(pt, eta, m_errorDataMC) * 0.01;
}
