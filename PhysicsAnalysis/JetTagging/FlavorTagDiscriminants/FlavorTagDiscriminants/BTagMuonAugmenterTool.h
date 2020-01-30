// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_MUON_AUGMENTER_TOOL_H
#define BTAG_MUON_AUGMENTER_TOOL_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/ISingleJetModifier.h"

namespace FlavorTagDiscriminants {

  class BTagMuonAugmenter;

  class BTagMuonAugmenterTool : public asg::AsgTool, virtual public ISingleJetModifier
  {
    ASG_TOOL_CLASS(BTagMuonAugmenterTool, ISingleJetModifier )
  public:
    BTagMuonAugmenterTool(const std::string& name);
    ~BTagMuonAugmenterTool();

    StatusCode initialize() override;

    // returns 0 for success
    int modifyJet(xAOD::Jet& jet) const override;
  private:
    std::unique_ptr<BTagMuonAugmenter> m_aug;
    std::string m_muonAssociationName;
    float m_muonMinDR;
    float m_muonMinpT;
    std::string m_flipTagConfig;
    // You'll probably have to add some accessors here
  };

}
#endif
