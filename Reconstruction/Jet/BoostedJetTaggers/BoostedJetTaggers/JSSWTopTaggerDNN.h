/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JSSWTOPTAGGERDNN_H_
#define JSSWTOPTAGGERDNN_H_

#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"
#include "ParticleJetTools/JetTruthLabelingTool.h"

#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/parse_json.hh"

#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

#include <TFile.h>
#include <TF1.h>
#include <TH2.h>

#include <fstream>

class JSSWTopTaggerDNN:  public JSSTaggerBase {
  ASG_TOOL_CLASS0(JSSWTopTaggerDNN)
  
  public:
  
  //Default - so root can load based on a name
  JSSWTopTaggerDNN(const std::string& name);
  
  // Default - so we can clean up
  ~JSSWTopTaggerDNN();
  JSSWTopTaggerDNN& operator=(const JSSWTopTaggerDNN& rhs);
  
  // Run once at the start of the job to setup everything
  StatusCode initialize();
  
  // IJSSTagger interface
  virtual Root::TAccept& tag(const xAOD::Jet& jet) const;
  
  // Retrieve score for a given DNN type (top/W)
  double getScore(const xAOD::Jet& jet) const;
  
  // Get scale factor
  double getWeight(const xAOD::Jet& jet) const;
  
  // Write the decoration to the jet
  void decorateJet(const xAOD::Jet& jet, float mcutH, float mcutL, float scoreCut, float scoreValue, float weightValue) const;
  
  // Update the jet substructure variables for each jet to use in DNN
  std::map<std::string,double> getJetProperties(const xAOD::Jet& jet) const;
  StatusCode finalize();
  
private:
  std::string m_name;
  std::string m_APP_NAME;
  
  // for the tagging type
    enum TAGCLASS{Unknown, WBoson, TopQuark};

    // DNN tools
    std::unique_ptr<lwt::LightweightNeuralNetwork> m_lwnn;
    std::map<std::string, double> m_DNN_inputValues;   // variables for DNN

    std::string m_weightConfigPath;
  
    // for internal usage
    mutable TAGCLASS m_TagClass;

    // bool to check whether variables are corrupt
    mutable bool m_undefInput;

    // parameters to store specific cut values
    std::string m_strMassCutLow;
    std::string m_strMassCutHigh;
    std::string m_strScoreCut;

    // functions that are configurable for specific cut values
    TF1* m_funcMassCutLow;
    TF1* m_funcMassCutHigh;
    TF1* m_funcScoreCut;

    // histograms for scale factors
    std::unique_ptr<TFile> m_weightConfig;
    std::map<std::string, std::unique_ptr<TH2D>> m_weightHistograms;
  
    // truth labeling tool
    asg::AnaToolHandle<JetTruthLabelingTool> m_JetTruthLabelingTool; //!

    // string for scale factors
    std::string m_weightdecorationName;
    std::string m_weightFileName;
    std::string m_weightHistogramName;
    std::string m_weightFlavors;

    // decorators
    SG::AuxElement::Decorator<float> m_dec_mcutL;
    SG::AuxElement::Decorator<float> m_dec_mcutH;
    SG::AuxElement::Decorator<float> m_dec_scoreCut;
    SG::AuxElement::Decorator<float> m_dec_scoreValue;
    SG::AuxElement::Decorator<float> m_dec_weight;
};

#endif
