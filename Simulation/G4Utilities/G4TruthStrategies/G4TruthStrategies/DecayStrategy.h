/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DecayStrategy_H
#define DecayStrategy_H

#include "MCTruthBase/TruthStrategy.h"
class DecayStrategy: public TruthStrategy {
public:
	DecayStrategy(const std::string);
	bool AnalyzeVertex(const G4Step*);
};

#endif
