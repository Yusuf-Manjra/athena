// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_SMOOTHEDWZTAGGER_H_
#define BOOSTEDJETSTAGGERS_SMOOTHEDWZTAGGER_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"

#include "PATCore/TAccept.h"

#include <TFile.h>
#include <TH2.h>

class TF1;

class SmoothedWZTagger : public  JSSTaggerBase {
  ASG_TOOL_CLASS0(SmoothedWZTagger)
  public:

  //Default - so root can load based on a name
  SmoothedWZTagger(const std::string& name);

  // Default - so we can clean up
  ~SmoothedWZTagger();

  // Run once at the start of the job to setup everything
  StatusCode initialize();

  // Run once at the end of the job to clean up
  StatusCode finalize();
  
  // Implement IJSSTagger interface
  virtual Root::TAccept& tag(const xAOD::Jet& jet) const;
  
  // get scale factor
  double getWeight(const xAOD::Jet& jet) const;

  private:

    // need to set in initialization
    std::string m_wkpt;
    std::string m_tagType;
    std::string m_configFile;
    std::string m_weightConfigPath;

    // parameters to store specific cut values
    std::string m_strMassCutLow;
    std::string m_strMassCutHigh;
    std::string m_strD2Cut;
    std::string m_strNtrkCut;

    // functions that are configurable for specific cut values
    TF1* m_funcMassCutLow;
    TF1* m_funcMassCutHigh;
    TF1* m_funcD2Cut;
    TF1* m_funcNtrkCut;

    // string for decorating jets with DNN output
    std::string m_decorationName;
 
    // string for scale factors
    std::string m_weightdecorationName;
    std::string m_weightFileName;
    std::string m_weightHistogramName;
    std::string m_weightFlavors;

    // histograms for scale factors
    std::unique_ptr<TFile> m_weightConfig;
    std::map<std::string, std::unique_ptr<TH2D>> m_weightHistograms;
  
    // decorators
    SG::AuxElement::Decorator<float>    m_dec_mcutL;
    SG::AuxElement::Decorator<float>    m_dec_mcutH;
    SG::AuxElement::Decorator<float>    m_dec_d2cut;
    SG::AuxElement::Decorator<float>    m_dec_ntrkcut;
    SG::AuxElement::Decorator<float>    m_dec_weight;
    SG::AuxElement::Decorator<int>      m_dec_accept;

    // accessor to truth label
    SG::AuxElement::ConstAccessor<int> m_acc_truthLabel;
};

#endif
