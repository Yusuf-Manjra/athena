/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileGeoG4SD
// Sensitive detector for TileCal G4 simulations using TileGeoModel
//
// Author: Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
//
// Major updates: July, 2013 (Sergey)
//
//************************************************************

#ifndef TILEGEOG4SD_TILEGEOG4SD_H
#define TILEGEOG4SD_TILEGEOG4SD_H

// Base class header
#include "G4VSensitiveDetector.hh"

// Member variables
#include "TileGeoG4SD/TileSDOptions.h"
#include "StoreGate/WriteHandle.h"
#include "TileSimEvent/TileHitVector.h"

class TileGeoG4SDCalc;
class TileGeoG4LookupBuilder;

class G4HCofThisEvent;
class G4Step;
class G4String;

class TileGeoG4SD : public G4VSensitiveDetector
{
public:
    TileGeoG4SD (G4String name, const std::string& hitCollectionName, const TileSDOptions opts);
   ~TileGeoG4SD ();

    void   Initialize  (G4HCofThisEvent*) override final;
    G4bool ProcessHits (G4Step*, G4TouchableHistory*) override final;
    void   EndOfAthenaEvent  ();

    TileGeoG4SDCalc* GetCalculator() { return m_calc; }

private:

    TileGeoG4SDCalc*          m_calc;
    TileGeoG4LookupBuilder*   m_lookup;
    const TileSDOptions       m_options;

    SG::WriteHandle<TileHitVector> m_HitColl;
};

#endif

