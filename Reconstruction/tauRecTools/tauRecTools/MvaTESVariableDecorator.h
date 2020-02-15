/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H
#define TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H

// tauRecTools include(s)
#include "tauRecTools/TauRecToolBase.h"

// xAOD include(s)
#include "xAODEventInfo/EventInfo.h"

class MvaTESVariableDecorator
: public TauRecToolBase
{
 public:
  
  ASG_TOOL_CLASS2( MvaTESVariableDecorator, TauRecToolBase, ITauToolBase )
    
  MvaTESVariableDecorator(const std::string& name="MvaTESVariableDecorator");
  virtual ~MvaTESVariableDecorator();
    
  StatusCode initialize() override;
  StatusCode execute(xAOD::TauJet& xTau) override;
  StatusCode finalize() override;

 private:

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"Key_eventInfo", "EventInfo", "EventInfo key"};
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexInputContainer{this,"Key_vertexInputContainer", "PrimaryVertices", "input vertex container key"};
};


#endif // TAURECTOOLSDEV_MVATESVARIABLEDECORATOR_H
