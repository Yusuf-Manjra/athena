/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SLEPTONS_G4STau2Minus_H
#define SLEPTONS_G4STau2Minus_H

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

#include "CxxUtils/checker_macros.h"

class G4STau2Minus: public G4ParticleDefinition
{
  // singleton implementation

private:

  static G4STau2Minus* theInstance ATLAS_THREAD_SAFE; // used in single-threaded Geant4 initialization
  G4STau2Minus(){}
  ~G4STau2Minus(){}

public:

  static G4STau2Minus* Definition(G4double mass=-1, G4double width=-1, G4double charge=-1, G4double PDG=-1, G4bool stable=true, G4double lifetime=-1, G4bool shortlived=false);

};

#endif // SLEPTONS_G4STau2Minus_H
