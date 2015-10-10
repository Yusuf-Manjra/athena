/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_NULLCLEANER_H
#define TRIGJETHYPO_NULLCLEANER_H


// ********************************************************************
//
// NAME:     NullCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "xAODJet/Jet.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h" // ICleaner
#include "./FlowNetwork.h"

namespace AOD{
  class Jet;
}

class NullCleaner: public ICleaner{
  /* apply no cleaning cuts */
public:
  bool operator()(const xAOD::Jet*) const override;
};


#endif
