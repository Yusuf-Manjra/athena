/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileFCS_StepInfoSD
// Sensitive detector for TileCal G4 simulations using TileGeoModel
//
// Author: Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
//
// Major updates: July, 2013 (Sergey)
//
//************************************************************

#ifndef ISF_FASTCALOSIMSD_TILEFCS_STEPINFOSD_H
#define ISF_FASTCALOSIMSD_TILEFCS_STEPINFOSD_H

// Base class header
#include "FCS_StepInfoSD.h"

class G4Step;
class G4TouchableHistory;
class ITileCalculator;

class TileFCS_StepInfoSD: public FCS_StepInfoSD {
public:
  TileFCS_StepInfoSD(G4String name, const FCS_Param::Config& config);
  ~TileFCS_StepInfoSD();

  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;

  // Don't leave copy constructors or assignment operators around
  TileFCS_StepInfoSD(const TileFCS_StepInfoSD&) = delete;
  TileFCS_StepInfoSD& operator=(const TileFCS_StepInfoSD&) = delete;

private:
 ITileCalculator* m_calculator;
};

#endif // ISF_FASTCALOSIMSD_TILEFCS_STEPINFOSD_H

