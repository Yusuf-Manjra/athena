/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCatchAllStrategy_H
#define MuonCatchAllStrategy_H

#include "MCTruthBase/TruthStrategy.h"
class MuonCatchAllStrategy: public TruthStrategy {
public:
	MuonCatchAllStrategy(const std::string);
	bool AnalyzeVertex(const G4Step*);
};

#endif
