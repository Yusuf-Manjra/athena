/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CosmicPerigeeAction_H
#define CosmicPerigeeAction_H

#include "FadsActions/UserAction.h"
#include "FadsActions/ActionsBase.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

// Can't be forward declared - it's a type def
#include "TrackRecord/TrackRecordCollection.h"

#include <string>

class TrackRecorderSD;

class CosmicPerigeeAction: public FADS::ActionsBase , public FADS::UserAction {

  public:
   CosmicPerigeeAction(std::string s): FADS::ActionsBase(s),FADS::UserAction(s),perigeeRecord(0),trackRecordCollection(0),m_idZ(0),m_idR(0) {}
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void BeginOfRunAction(const G4Run*);
   void EndOfRunAction(const G4Run*);
   void SteppingAction(const G4Step*);

  private:
   TrackRecorderSD *perigeeRecord;
   TrackRecordCollection *trackRecordCollection;
   AthenaHitsCollectionHelper theHelper;
   double m_idZ, m_idR;

};

#endif
