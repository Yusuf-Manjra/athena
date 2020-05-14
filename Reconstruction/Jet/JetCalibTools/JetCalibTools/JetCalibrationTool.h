///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetCalibrationTool.h 
// Header file for class JetCalibrationTool
// Author: Joe Taenzer <joseph.taenzer@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETCALIBTOOLS_APPLYJETCALIBRATION_H
#define JETCALIBTOOLS_APPLYJETCALIBRATION_H 1

#include <string.h>

#include <TString.h>
#include <TEnv.h>

#include "AsgTools/AsgTool.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventShape/EventShape.h"
#include "StoreGate/ReadDecorHandleKey.h"

// JetCalibTools includes
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"
#include "JetCalibTools/CalibrationMethods/JetPileupCorrection.h"
#include "JetCalibTools/CalibrationMethods/ResidualOffsetCorrection.h"
#include "JetCalibTools/CalibrationMethods/EtaJESCorrection.h"
#include "JetCalibTools/CalibrationMethods/GlobalSequentialCorrection.h"
#include "JetCalibTools/CalibrationMethods/InsituDataCorrection.h"
#include "JetCalibTools/CalibrationMethods/JMSCorrection.h"
#include "JetCalibTools/CalibrationMethods/JetSmearingCorrection.h"

class JetPileupCorrection;
class ResidualOffsetCorrection;
class EtaJESCorrection;
class GlobalSequentialCorrection;
class InsituDataCorrection;
class JMSCorrection;
class JetSmearingCorrection;

class JetCalibrationTool
  : public ::JetCalibrationToolBase {

  ASG_TOOL_CLASS2(JetCalibrationTool, IJetCalibrationTool, IJetModifier)

public:
  /// Constructor with parameters: 
  JetCalibrationTool(const std::string& name = "JetCalibrationTool");

  /// Destructor: 
  virtual ~JetCalibrationTool(); 

  enum jetScale { EM, LC, PFLOW };

  // Initialize the tool (default, assumes private members were set in the constructor)
  virtual StatusCode initializeTool(const std::string& name);

  // PATInterfaces inherited method
  StatusCode initialize();
 
  //PATInterfaces inherited method
  StatusCode finalize();

  virtual StatusCode applyCalibration(xAOD::JetContainer&) const;

  // Retrieve pTmax from in situ corrections
  virtual VecD retrieveEtaIntercalPtMax(){return m_relInsituPtMax;}
  virtual VecD retrieveAbsoluteInsituPtMax(){return m_absInsituPtMax;}

  // Get the nominal resolution
  virtual StatusCode getNominalResolutionData(const xAOD::Jet& jet, double& resolution) const;
  virtual StatusCode getNominalResolutionMC(  const xAOD::Jet& jet, double& resolution) const;
  
protected:
  /// This is where the actual calibration code goes.
  virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const;

//Private methods
private:
  //Set event by event info like rho, mu, NPV
  virtual StatusCode initializeEvent(JetEventInfo& jetEventInfo) const;
  //Create the vector of calibration classes
  StatusCode getCalibClass(const std::string& name, TString calibration);

//Private members
private:
  // ReadHandleKey(s)
  SG::ReadHandleKey<xAOD::EventInfo>        m_rhkEvtInfo;
  SG::ReadHandleKey<xAOD::EventShape>       m_rhkRhoKey;
  SG::ReadHandleKey<xAOD::VertexContainer>  m_rhkPV;
  SG::ReadDecorHandleKey<xAOD::EventInfo>        m_rdhkEvtInfo {this
      ,"AveIntPerXKey"
      ,"EventInfo.AveIntPerXDecor"
      ,"Decoration for Average Interaction Per Crossing"};

  //Variables for configuration
  std::string m_jetAlgo;
  std::string m_config;
  std::string m_calibSeq;
  std::string m_calibAreaTag;
  std::string m_originScale;
  bool m_devMode;
  bool m_isData;
  bool m_timeDependentCalib;
  bool m_originCorrectedClusters;
  std::string m_rhoKey;
  std::string m_dir;
  std::string m_eInfoName;
  std::vector<TString> m_timeDependentInsituConfigs;
  std::vector<double>  m_runBins;
  bool m_doSetDetectorEta;

  //TEnv to hold the global text config
  TEnv * m_globalConfig;
  std::vector<TEnv*> m_globalTimeDependentConfigs;

  //Bools/enums to avoid string comparisons at run time
  jetScale m_jetScale;
  bool m_doJetArea;
  bool m_doResidual;
  bool m_doOrigin;
  bool m_doGSC;

  std::string m_gscDepth; // if not set then use the one defined in the config

  // vector with pTmax of each in situ correction
  VecD m_relInsituPtMax, m_absInsituPtMax;

  //Class objects for each calibration step
  std::vector<JetCalibrationToolBase*> m_calibClasses;
  JetPileupCorrection * m_jetPileupCorr;
  EtaJESCorrection * m_etaJESCorr;
  GlobalSequentialCorrection * m_globalSequentialCorr;
  InsituDataCorrection * m_insituDataCorr;
  std::vector<JetCalibrationToolBase*> m_insituTimeDependentCorr;
  JMSCorrection * m_jetMassCorr;
  JetSmearingCorrection* m_jetSmearCorr;

}; 

#endif //> !JETCALIBTOOLS_APPLYJETCALIBRATION_H
