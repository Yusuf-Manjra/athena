/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class ElectronPhotonVariableCorrectionBase
   @brief Tool to correct electron and photon MC variables.

   @author Nils Gillwald (DESY) nils.gillwald@desy.de
   @date   February 2020
**/

#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionBase.h"
#include "EgammaAnalysisHelpers/AsgElectronPhotonIsEMSelectorConfigHelper.h"
#include "xAODEventShape/EventShape.h"

// EDM includes
#include "xAODEgamma/EgammaEnums.h"
#include "TEnv.h"

//Root includes
#include "TF1.h"
#include "TGraph.h"
#include "TFile.h"

//allow advanced math for the TF1
#include "TMath.h"

#include "PathResolver/PathResolver.h"

// ===========================================================================
// Standard Constructor
// ===========================================================================
ElectronPhotonVariableCorrectionBase::ElectronPhotonVariableCorrectionBase(const std::string& myname) :
    AsgTool(myname)
{
    //declare the needed properties
    declareProperty("ConfigFile",m_configFile="","The configuration file to use");
}

// ===========================================================================
// Initialize
// ===========================================================================
StatusCode ElectronPhotonVariableCorrectionBase::initialize()
{
    // Locate configuration file, abort if not found
    std::string configFile;
    if (!m_configFile.empty())
    {
        configFile = PathResolverFindCalibFile(m_configFile);
        if (configFile == "")
        {
            ATH_MSG_ERROR("Could not locate configuration file " << m_configFile);
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG("Use configuration file " << m_configFile);
    }
    else
    {
        ATH_MSG_ERROR("Config file string is empty. Please provide a config file to the tool.");
        return StatusCode::FAILURE;
    }

    // retrieve properties from configuration file, using TEnv class
    TEnv env(configFile.c_str());
    // Send warning if duplicates found in conf file
    env.IgnoreDuplicates(false);

    //retrieve variable to correct
    if (env.Lookup("Variable"))
    {
        m_correctionVariable = env.GetValue("Variable","");
    }
    else
    {
        ATH_MSG_ERROR("Correction variable is empty or not in configuration file.");
        return StatusCode::FAILURE;
    }

    // initialize the variable aux element accessors
    // variable to be corrected
    m_variableToCorrect = std::make_unique<SG::AuxElement::Accessor<float>>(m_correctionVariable);
    // save original value under different name
    m_originalVariable = std::make_unique<SG::AuxElement::Accessor<float>>(m_correctionVariable + "_original");

    // Get the function used to correct the variable
    if (env.Lookup("Function"))
    {
        m_correctionFunctionString = env.GetValue("Function","failure");
        // initialize the actual correction function TF1
        m_correctionFunctionTF1 = std::make_unique<TF1>(TF1("m_correctionFunctionTF1",m_correctionFunctionString.c_str()));
    }
    else
    {
        ATH_MSG_ERROR("Correction function is empty or not in configuration file.");
        return StatusCode::FAILURE;
    }

    // Get the number of parameters used in the correction function
    if (env.Lookup("nFunctionParameters"))
    {
        m_numberOfFunctionParameters = env.GetValue("nFunctionParameters",-1);
    }
    else
    {
        ATH_MSG_ERROR("You did not specify the number of parameters in the correction function.");
        return StatusCode::FAILURE;
    }

    // resize all vectors used for getting parameters to m_numberOfFunctionParameters
    m_ParameterTypeVector.resize(m_numberOfFunctionParameters);
    m_binValues.resize(m_numberOfFunctionParameters);
    m_graphCopies.resize(m_numberOfFunctionParameters);

    // Save the type of all parameters in the correction function (assuming m_numberOfFunctionParameters parameters)
    for (int parameter_itr = 0; parameter_itr < m_numberOfFunctionParameters; parameter_itr++)
    {
        // if the parameter #parameter_itr is in the conf file, save its type
        TString parameterType = TString::Format("Parameter%dType",parameter_itr);
        if (env.Lookup(parameterType))
        {
            // convert string to ParameterType, fail if non-existing type
            ElectronPhotonVariableCorrectionBase::parameterType type = stringToParameterType(env.GetValue(parameterType.Data(),""));
            if( type == ElectronPhotonVariableCorrectionBase::parameterType::Failure )
            {
                ATH_MSG_ERROR("Parameter " << parameter_itr << " read-in failed, not an allowed parameter type.");
                return StatusCode::FAILURE;
            }
            // save type, get according type information and save it
            m_ParameterTypeVector.at(parameter_itr) = type;
            ATH_CHECK(getParameterInformationFromConf(env, parameter_itr, type));
        }
        // else fail
        else
        {
            ATH_MSG_ERROR("Did not find Parameter" << parameter_itr << ", although you specified there were " << m_numberOfFunctionParameters << " parameters for the correction function.");
            return StatusCode::FAILURE;
        }
    } // end loop over all function parameters

    // check to which EGamma object the conf file should be applied to, if flag is set
    if (env.Lookup("ApplyTo"))
    {
        std::string applyToObjectsFlag = env.GetValue("ApplyTo","Failure");
        m_applyToObjects = stringToEGammaObject(applyToObjectsFlag);
        // fail if not passed a proper type
        if (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::Failure)
        {
            ATH_MSG_ERROR("You did not correctly specify the object type in the ApplyTo flag.");
            return StatusCode::FAILURE;
        }
    }
    // else fail
    else
    {
        ATH_MSG_ERROR("You did not specify to which objects this conf file should be applied to (ApplyTo).");
        return StatusCode::FAILURE;
    }

    //everything worked out, so
    return StatusCode::SUCCESS;
}
 
// ===========================================================================
// Application of correction
// ===========================================================================
const CP::CorrectionCode ElectronPhotonVariableCorrectionBase::applyCorrection(xAOD::Photon& photon ) const
{
    // check if we should only deal with converted / unconverted photons
    if (!passedCorrectPhotonType(photon))
    {
        ATH_MSG_ERROR("You specified in the conf file that the tool should only be used for (un-)converted photons, but passed the other conversion type.");
        return CP::CorrectionCode::Error;
    }
    
    // From the object, get the variable value according to the variable from the conf file
    // if variable not found, fail
    float original_variable = 0.;
    if( m_variableToCorrect->isAvailable(photon) )
    {
        original_variable = (*m_variableToCorrect)(photon);
        //Save the original value to the photon under different name
        (*m_originalVariable)(photon) = original_variable;
    }
    else
    {
        ATH_MSG_ERROR("The correction variable \"" << m_correctionVariable << "\" provided in the conf file is not available.");
        return CP::CorrectionCode::Error;
    }
    
    //declare objects needed to retrieve photon properties
    std::vector<float> properties; //safe value of function parameter i at place i
    properties.resize(m_numberOfFunctionParameters);
    float absEta; //safe absolute value of eta of event
    float pt; //safe pt of event

    //Get all the properties from the photon and fill them to properties, eta, pt
    if (getKinematicProperties(photon, pt, absEta) != StatusCode::SUCCESS)
    {
        ATH_MSG_ERROR("Could not retrieve kinematic properties of this photon object.");
        return CP::CorrectionCode::Error;
    }
    if (getCorrectionParameters(properties, pt, absEta) != StatusCode::SUCCESS)
    {
        ATH_MSG_ERROR("Could not get the correction parameters for this photon object.");
        return CP::CorrectionCode::Error;
    }
    
    // Apply the correction, write to the corrected AuxElement
    correct((*m_variableToCorrect)(photon),original_variable, properties).ignore(); // ignore as will always return SUCCESS

    // everything worked out, so
    return CP::CorrectionCode::Ok;
}

const CP::CorrectionCode ElectronPhotonVariableCorrectionBase::applyCorrection(xAOD::Electron& electron ) const
{   
    if (!(m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allElectrons || m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allEGammaObjects))
    {
        ATH_MSG_ERROR("You want to correct electrons, but passed a conf file with ApplyTo flag not set for electrons. Are you using the correct conf file?");
        return CP::CorrectionCode::Error;
    }

    // From the object, get the variable value according to the variable from the conf file
    // if variable not found, fail
    float original_variable = 0.;
    if (m_variableToCorrect->isAvailable(electron))
    {
        original_variable = (*m_variableToCorrect)(electron);
        //Save the original value to the photon under different name
        (*m_originalVariable)(electron) = original_variable;
    }
    else
    {
        ATH_MSG_ERROR("The correction variable \"" << m_correctionVariable << "\" provided in the conf file is not available.");
        return CP::CorrectionCode::Error;
    }
    
    //declare objects needed to retrieve electron properties
    std::vector<float> properties; //safe value of function parameter i at place i
    properties.resize(m_numberOfFunctionParameters);
    float absEta; //safe absolute value of eta of event
    float pt; //safe pt of event

    //Get all the properties from the electron and fill them to properties, eta, pt
    if (getKinematicProperties(electron, pt, absEta) != StatusCode::SUCCESS)
    {
        ATH_MSG_ERROR("Could not retrieve kinematic properties of this electron object.");
        return CP::CorrectionCode::Error;
    }
    if (getCorrectionParameters(properties, pt, absEta) != StatusCode::SUCCESS)
    {
        ATH_MSG_ERROR("Could not get the correction parameters for this electron object.");
        return CP::CorrectionCode::Error;
    }
    
    // Apply the correction, write to the corrected AuxElement
    correct((*m_variableToCorrect)(electron),original_variable, properties).ignore(); // ignore as will always return SUCCESS

    // everything worked out, so
    return CP::CorrectionCode::Ok;
}

const StatusCode ElectronPhotonVariableCorrectionBase::correct(float& return_corrected_variable, const float &original_variable, std::vector<float>& properties) const
{   
    // set the parameters of the correction function
    for (unsigned int parameter_itr = 0; parameter_itr < properties.size(); parameter_itr++)
    {
        m_correctionFunctionTF1->SetParameter(parameter_itr,properties.at(parameter_itr));
    }

    // Calculate corrected value
    return_corrected_variable = m_correctionFunctionTF1->Eval(original_variable);

    // everything worked out, so
    return StatusCode::SUCCESS;
}

// ===========================================================================
// Corrected Copies
// ===========================================================================
const CP::CorrectionCode ElectronPhotonVariableCorrectionBase::correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon ) const
{
    out_photon = new xAOD::Photon(in_photon);
    return applyCorrection(*out_photon);
}

const CP::CorrectionCode ElectronPhotonVariableCorrectionBase::correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron) const
{
    out_electron = new xAOD::Electron(in_electron);
    return applyCorrection(*out_electron);
}

// ===========================================================================
// Helper Functions
// ===========================================================================

const StatusCode ElectronPhotonVariableCorrectionBase::getKinematicProperties(const xAOD::Egamma& egamma_object, float& pt, float& absEta) const
{
    // just reteriving eta and pt is probably less expensive then checking if I need it and
    // then retrieve it only if I actually need it

    // protection against bad clusters
    const xAOD::CaloCluster* cluster  = egamma_object.caloCluster();
    if ( cluster == nullptr )
    {
        ATH_MSG_ERROR("EGamma object calorimeter cluster is NULL: Cluster " << cluster);
        return StatusCode::FAILURE;
    }

    // Fill variables
    // eta position in second sampling
    absEta   = fabsf(cluster->etaBE(2));
    // transverse energy in calorimeter (using eta position in second sampling)
    const double energy =  cluster->e();
    double et = 0.;
    if (absEta<999.) {
      const double cosheta = cosh(absEta);
      et = (cosheta != 0.) ? energy/cosheta : 0.;
    }
    pt = et;

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::getParameterInformationFromConf(TEnv& env, const int& parameter_number, const ElectronPhotonVariableCorrectionBase::parameterType& type)
{
    // don't want to write the same code multiple times, so set flags when to retrieve eta/pt bins
    bool getEtaBins = false;
    bool getPtBins = false;
    // form strings according to which parameter to retrieve
    TString filePathKey = TString::Format("Parameter%dFile",parameter_number);
    TString graphNameKey = TString::Format("Parameter%dGraphName",parameter_number);
    TString binValues = TString::Format("Parameter%dValues",parameter_number);
    // helpers
    TString filePath = "";
    TString graphName = "";

    // according to the parameter type, retrieve the information from conf
    if (type == ElectronPhotonVariableCorrectionBase::parameterType::EtaDependentTGraph || type == ElectronPhotonVariableCorrectionBase::parameterType::PtDependentTGraph)
    {
        // check if necessary information is in conf, else fail
        if (env.Lookup(filePathKey))
        {
            //get the path to the root file where the graph is saved
            filePath = PathResolverFindCalibFile(env.GetValue(filePathKey.Data(),""));
            // fail if file not found
            if (filePath == "")
            {
                ATH_MSG_ERROR("Could not locate Parameter" << parameter_number << " TGraph file.");
                return StatusCode::FAILURE;
            }
        }
        else
        {
            ATH_MSG_ERROR("Could not retrieve Parameter" << parameter_number << " file path.");
            return StatusCode::FAILURE;
        }
        // check if necessary information is in conf, else fail
        if (env.Lookup(graphNameKey))
        {
            //get the name of the TGraph
            graphName = env.GetValue(graphNameKey.Data(),"");
        }
        else
        {
            ATH_MSG_ERROR("Could not retrieve Parameter" << parameter_number << " graph name.");
            return StatusCode::FAILURE;
        }
        // open file, if it works, try to find graph, get graph, store a copy, else warning + fail
        std::unique_ptr<TFile> file (new TFile(filePath.Data(),"READ"));
        // check if file is open - if open, get graph, if not, fail
        if (file->IsOpen())
        {
            // if graph exists, get it, else fail
            if (file->Get(graphName))
            {
                std::unique_ptr<TGraph> graph ((TGraph*)file->Get(graphName.Data()));
                m_graphCopies.at(parameter_number) = (TGraph*)graph->Clone(); //Or use copy constructor?
                file->Close();
            }
            else
            {
                ATH_MSG_ERROR("Could not find TGraph " << graphName << " in file " << filePath);
                return StatusCode::FAILURE;
            }
        }
        else
        {
            ATH_MSG_ERROR("Could not open Parameter" << parameter_number << " TGraph file " << filePath.Data());
            return StatusCode::FAILURE;
        }
    }
    else if (type == ElectronPhotonVariableCorrectionBase::parameterType::EtaBinned )
    {
        //get eta binning later
        getEtaBins = true;
    }
    else if (type == ElectronPhotonVariableCorrectionBase::parameterType::PtBinned )
    {
        //get pt binning later
        getPtBins = true;
    }
    else if (type == ElectronPhotonVariableCorrectionBase::parameterType::EtaTimesPtBinned )
    {
        //get eta and pt binning later
        getEtaBins = true;
        getPtBins = true;
    }
    else if (type == ElectronPhotonVariableCorrectionBase::parameterType::EventDensity )
    {
        // nothing has to be retrieved, no additional parameters for EventDensity currently
        return StatusCode::SUCCESS;
    }

    // if needed and not already retrieved, get eta binning
    if (getEtaBins && !m_retrievedEtaBinning)
    {
        // check if necessary information is in conf, else fail
        if (env.Lookup("EtaBins"))
        {
            //get the eta binning (global!)
            m_etaBins = AsgConfigHelper::HelperFloat("EtaBins",env);
            //force that the low bin edges are given by the conf file, starting with 0
            if (m_etaBins.at(0) != 0.)
            {
                ATH_MSG_ERROR("Lowest bin edge given for parameter " << parameter_number << " is not 0. Please provide the lower bin edges of your correction binning in the conf file, starting with 0.");
                return StatusCode::FAILURE;
            }
            // don't want to retrieve the same thing twice from conf
            m_retrievedEtaBinning = true;
        }
        else
        {
            ATH_MSG_ERROR("Could not retrieve eta binning.");
            return StatusCode::FAILURE;
        }
    }
    // if needed and not already retrieved, get pt binning
    if (getPtBins && !m_retrievedPtBinning)
    {
        // check if necessary information is in conf, else fail
        if (env.Lookup("PtBins"))
        {
            //get the pt binning (global!)
            m_ptBins = AsgConfigHelper::HelperFloat("PtBins",env);
            //force that the low bin edges are given by the conf file, starting with 0
            if (m_ptBins.at(0) != 0.)
            {
                ATH_MSG_ERROR("Lowest bin edge given for parameter " << parameter_number << " is not 0. Please provide the lower bin edges of your correction binning in the conf file, starting with 0.");
                return StatusCode::FAILURE;
            }
            // don't want to retrieve the same thing twice from conf
            m_retrievedPtBinning = true;
        }
        else
        {
            ATH_MSG_ERROR("Could not retrieve pt binning.");
            return StatusCode::FAILURE;
        }
    }
    if ( getEtaBins || getPtBins)
    {
        // check if necessary information is in conf, else fail
        if (env.Lookup(binValues))
        {
            //get the binned values of the eta/pt dependent parameter
            m_binValues.at(parameter_number) = AsgConfigHelper::HelperFloat(binValues.Data(),env);
        }
        else
        {
            ATH_MSG_ERROR("Could not retrieve binned values.");
            return StatusCode::FAILURE;
        }
    }

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::getCorrectionParameters(std::vector<float>& properties, const float& pt, const float& absEta) const
{
    // according to the parameter type, get the actual parameter going to the correction function
    // for this, loop over the parameter type vector
    for (unsigned int parameter_itr = 0; parameter_itr < m_ParameterTypeVector.size(); parameter_itr++)
    {
        ElectronPhotonVariableCorrectionBase::parameterType type = m_ParameterTypeVector.at(parameter_itr);
        if (type == ElectronPhotonVariableCorrectionBase::parameterType::EtaDependentTGraph)
        {
            // evaluate TGraph at abs(eta)
            properties.at(parameter_itr) = m_graphCopies.at(parameter_itr)->Eval(absEta);
        }
        else if (type == ElectronPhotonVariableCorrectionBase::parameterType::PtDependentTGraph)
        {
            // evaluate TGraph at pt
            properties.at(parameter_itr) = m_graphCopies.at(parameter_itr)->Eval(pt);
        }
        else if (type == ElectronPhotonVariableCorrectionBase::parameterType::EtaBinned)
        {
            // get value of correct eta bin
            ATH_CHECK(get1DBinnedParameter(properties.at(parameter_itr),absEta,m_etaBins,parameter_itr));
        }
        else if (type == ElectronPhotonVariableCorrectionBase::parameterType::PtBinned)
        {
            // get value of correct pt bin
            ATH_CHECK(get1DBinnedParameter(properties.at(parameter_itr),pt,m_ptBins,parameter_itr));
        }
        else if (type == ElectronPhotonVariableCorrectionBase::parameterType::EtaTimesPtBinned)
        {
            // get value of correct eta x pt bin
            ATH_CHECK(get2DBinnedParameter(properties.at(parameter_itr),absEta,pt,parameter_itr));
        }
        else if (type == ElectronPhotonVariableCorrectionBase::parameterType::EventDensity)
        {
            // get event density
            ATH_CHECK(getDensity(properties.at(parameter_itr), "TopoClusterIsoCentralEventShape"));
        }
    }

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::get1DBinnedParameter(float& return_parameter_value, const float& evalPoint, const std::vector<float>& binning, const int& parameter_number) const
{
    ANA_MSG_VERBOSE("Get 1DBinnedParameters...");
    ANA_MSG_VERBOSE("EvalPoint: " << evalPoint);
    // check in which bin the evalPoint is
    // if the evalPoint is < 0, something is very wrong
    if (evalPoint < binning.at(0))
    {
        ATH_MSG_ERROR("Abs(Eta) or pT of object is smaller than 0.");
        return StatusCode::FAILURE;
    }
    // loop over bin boundaries
    //run only up to binning.size()-1, as running to binning.size() will get a seg fault for the boundary check
    for (unsigned int bin_itr = 0; bin_itr < binning.size()-1; bin_itr++)
    {
        // if the evaluation point is within the checked bins boundaries, this is the value we want
        if (evalPoint > binning.at(bin_itr) && evalPoint < binning.at(bin_itr + 1))
        {
            // we found the according bin, so return the according value
            return_parameter_value = m_binValues.at(parameter_number).at(bin_itr);
            return StatusCode::SUCCESS;
        }
    }
    //if this point is ever reached, the evaluation point is larger then the largest lowest bin edge
    //if that's the case, return the value for the last bin
    //the -1 is because the parameter numbering in a vector starts at 0
    return_parameter_value = m_binValues.at(parameter_number).at(m_binValues.size()-1);

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::get2DBinnedParameter(float& return_parameter_value, const float& etaEvalPoint, const float& ptEvalPoint, const int& parameter_number) const
{
    //need to find eta bin, and need to find pt bin
    //from this, calculate which parameter of the list is needed to be returned.
    int etaBin = -1;
    int ptBin = -1;

    // get eta bin
    //run only up to binning.size()-1, as running to binning.size() will get a seg fault for the boundary check
    for (unsigned int etaBin_itr = 0; etaBin_itr < m_etaBins.size()-1; etaBin_itr++)
    {
        // if the evaluation point is within the checked bins boundaries, this is the value we want
        if (etaEvalPoint > m_etaBins.at(etaBin_itr) && etaEvalPoint < m_etaBins.at(etaBin_itr + 1))
        {
            // we found the according bin, so set to the according value
            etaBin = etaBin_itr;
        }
    }
    //if etaBin was not set yet, it's larger then the largest lower bin edge
    //thus, need to set it to the last bin
    if (etaBin == -1)
    {
        //need to correct for vector numbering starting at 0
        etaBin = m_etaBins.size()-1;
    }

    // get pt bin
    //run only up to binning.size()-1, as running to binning.size() will get a seg fault for the boundary check
    for (unsigned int ptBin_itr = 0; ptBin_itr < m_ptBins.size()-1; ptBin_itr++)
    {
        // if the evaluation point is within the checked bins boundaries, this is the value we want
        if (ptEvalPoint > m_ptBins.at(ptBin_itr) && ptEvalPoint < m_ptBins.at(ptBin_itr + 1))
        {
            // we found the according bin, so set to the according value
            ptBin = ptBin_itr;
        }
    }
    //if ptBin was not set yet, it's larger then the largest lower bin edge
    //thus, need to set it to the last bin
    if (ptBin == -1)
    {
        //need to correct for vector numbering starting at 0
        ptBin = m_ptBins.size()-1;
    }

    // return the value corresponding to the pt x eta bin found
    /* Note: Assuming that the values are binned in pt x eta in the conf file:
     *           eta bin 0 | eta bin 1 | eta bin 2 | eta bin 3 | eta bin 4 | etc.
     * pt bin 0      0           1           2           3           4
     * pt bin 1      5           6           7           8           9
     * pt bin 2     10          11          12          13          14
     * pt bin 3     15          16          17          18          19
     * pt bin 4     20          21          22          23          24
     * etc.
     * the correct parameter is saved in the vector at 4*ptBinNumber + etaBinNumber
     * */
    return_parameter_value = m_binValues.at(parameter_number).at(m_etaBins.size() * ptBin + etaBin);

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::getDensity(float& value, const std::string& eventShapeContainer) const
{
    // retrieve the event shape container
    const xAOD::EventShape* evtShape = nullptr;
    if(evtStore()->retrieve(evtShape, eventShapeContainer).isFailure()){
        ATH_MSG_ERROR("Cannot retrieve density container " << eventShapeContainer);
        return StatusCode::FAILURE;
    }
    // get the density from the container
    value = evtShape->getDensity(xAOD::EventShape::Density);
    return StatusCode::SUCCESS;
}

ElectronPhotonVariableCorrectionBase::parameterType ElectronPhotonVariableCorrectionBase::stringToParameterType( const std::string& input ) const
{
    // return parameter type according to string given in conf file
    if( input == "EtaDependentTGraph") return ElectronPhotonVariableCorrectionBase::parameterType::EtaDependentTGraph;
    else if( input == "PtDependentTGraph") return ElectronPhotonVariableCorrectionBase::parameterType::PtDependentTGraph;
    else if( input == "EtaBinned") return ElectronPhotonVariableCorrectionBase::parameterType::EtaBinned;
    else if( input == "PtBinned") return ElectronPhotonVariableCorrectionBase::parameterType::PtBinned;
    else if( input == "EtaTimesPtBinned") return ElectronPhotonVariableCorrectionBase::parameterType::EtaTimesPtBinned;
    else if( input == "EventDensity") return ElectronPhotonVariableCorrectionBase::parameterType::EventDensity;
    else
    {
        // if not a proper type, return failure type - check and fail on this!
        ATH_MSG_ERROR(input.c_str() << " is not an allowed parameter type.");
        return ElectronPhotonVariableCorrectionBase::parameterType::Failure;
    }
}

ElectronPhotonVariableCorrectionBase::EGammaObjects ElectronPhotonVariableCorrectionBase::stringToEGammaObject( const std::string& input ) const
{
    // return object type which correction should be applied to
    if( input == "unconvertedPhotons" ) return ElectronPhotonVariableCorrectionBase::EGammaObjects::unconvertedPhotons;
    else if( input == "convertedPhotons" ) return ElectronPhotonVariableCorrectionBase::EGammaObjects::convertedPhotons;
    else if( input == "allPhotons" ) return ElectronPhotonVariableCorrectionBase::EGammaObjects::allPhotons;
    else if( input == "allElectrons" ) return ElectronPhotonVariableCorrectionBase::EGammaObjects::allElectrons;
    else if( input == "allEGammaObjects" ) return ElectronPhotonVariableCorrectionBase::EGammaObjects::allEGammaObjects;
    else 
    {
        // if not a proper object type, return failure type - check and fail on this!
        ATH_MSG_ERROR(input.c_str() << " is not an allowed EGamma object type to apply corrections to.");
        return ElectronPhotonVariableCorrectionBase::EGammaObjects::Failure;
    }
}

bool ElectronPhotonVariableCorrectionBase::passedCorrectPhotonType(const xAOD::Photon& photon) const
{
    // retrieve if photon is converted or unconverted
    bool isConvertedPhoton = xAOD::EgammaHelpers::isConvertedPhoton(&photon);
    bool isUnconvertedPhoton = !isConvertedPhoton;

    // check if conf file ApplyTo flag matches photon conversion type
    return ((applyToConvertedPhotons() && isConvertedPhoton) || (applyToUnconvertedPhotons() && isUnconvertedPhoton));
}

bool ElectronPhotonVariableCorrectionBase::applyToConvertedPhotons() const
{
    bool applyToAllEGamma = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allEGammaObjects);
    bool applyToAllPhotons = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allPhotons);
    bool applyToConvertedPhotons = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::convertedPhotons);
    return (applyToAllEGamma || applyToAllPhotons || applyToConvertedPhotons);
}

bool ElectronPhotonVariableCorrectionBase::applyToUnconvertedPhotons() const
{
    bool applyToAllEGamma = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allEGammaObjects);
    bool applyToAllPhotons = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allPhotons);
    bool applyToUnconvertedPhotons = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::unconvertedPhotons);
    return (applyToAllEGamma || applyToAllPhotons || applyToUnconvertedPhotons);
}

bool ElectronPhotonVariableCorrectionBase::applyToElectrons() const
{
    bool applyToAllEGamma = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allEGammaObjects);
    bool applyToAllElectrons = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allElectrons);
    return (applyToAllEGamma || applyToAllElectrons);
}
