/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//###############################################
//   BLM Sensitive Detector class
//   Bostjan Macek 14.february.2008
//###############################################

#ifndef BLM_G4_SD_BLMSENSORSD_H
#define BLM_G4_SD_BLMSENSORSD_H

// Base class header
#include "G4VSensitiveDetector.hh"

// Athena headers
#include "InDetSimEvent/SiHitCollection.h"
#include "StoreGate/WriteHandle.h"
#include <gtest/gtest_prod.h>

// G4 needed classes
class G4Step;

class BLMSensorSD : public G4VSensitiveDetector
{
 FRIEND_TEST( BLMSensorSDtest, Initialize );
 FRIEND_TEST( BLMSensorSDtest, ProcessHits );
 FRIEND_TEST( BLMSensorSDtest, AddHit );
 public:
  // Constructor
  BLMSensorSD(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  ~BLMSensorSD() {}

  // Process the hits from G4
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;

  // Initialize from G4
  void Initialize(G4HCofThisEvent *) override final;

  /** Templated method to stuff a single hit into the sensitive detector class.  This
   could get rather tricky, but the idea is to allow fast simulations to use the very
   same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

 private:
  // The hits collection
  SG::WriteHandle<SiHitCollection> m_HitColl;
};

#endif //BLM_G4_SD_BLMSENSORSD_H
