/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_JSSTAGGERBASE_H
#define BOOSTEDJETSTAGGERS_JSSTAGGERBASE_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "JetInterface/IJetSelector.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "ParticleJetTools/LargeRJetLabelEnum.h"
#include "BoostedJetTaggers/TagResultEnum.h"

#include "PATCore/TAccept.h"

class JSSTaggerBase :   public asg::AsgTool ,
  virtual public IJetSelector,
  virtual public IJetSelectorTool
{
  ASG_TOOL_CLASS2(JSSTaggerBase, IJetSelector, IJetSelectorTool )

  protected:

    // the object where you store the contents of what the jet has passed
    mutable Root::TAccept m_accept;

    // Configurable members

    // Configuration file name
    std::string m_configFile;

    // Location where config files live on cvmfs
    std::string m_calibArea;

    // Keras configurations for ML taggers
    std::string m_kerasConfigFileName;
    std::string m_kerasConfigFilePath;
    std::string m_kerasConfigOutputName;
    std::string m_kerasCalibArea;

    // TMVA configurations for BDT taggers
    std::string m_tmvaConfigFileName;
    std::string m_tmvaConfigFilePath;
    std::string m_tmvaCalibArea;

    // Tagger information
    std::string m_wkpt;
    std::string m_tagType;

    // the kinematic bounds for the jet - these are in MeV (not GeV!)
    float m_jetPtMin;
    float m_jetPtMax;
    float m_jetEtaMax;

    // Bool controlling whether jet will be decorated
    bool m_decorate;

    // Decoration name
    std::string m_decorationName;

    // flag to calculate scale factor
    bool m_calcSF;
    bool m_IsMC;

    // Truth label options
    bool m_truthLabelUseTRUTH3;
    std::string m_truthParticleContainerName;
    std::string m_truthBosonContainerName;
    std::string m_truthTopQuarkContainerName;
    std::string m_truthLabelName;

    // Truth label accessor
    std::unique_ptr< SG::AuxElement::ConstAccessor<int> > m_acc_truthLabel;

    // default constructor - to be used in all derived classes
    JSSTaggerBase(const std::string &name);

    // provide a default implementation for IJetSelector::keep :
    virtual int keep(const xAOD::Jet& jet) const {
      return tag(jet);
    }

    // show the user what cuts are configured
    void showCuts() const;
};

#endif
