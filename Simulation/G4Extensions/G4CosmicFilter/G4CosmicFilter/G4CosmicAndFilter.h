/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// an action to query the SD responsible for the storing of the
// TrackRecords *at the entrance of the ID* if no track it aborts
// the event - jamie boyd 15 nov 06


#ifndef G4CosmicAndFilter_H
#define G4CosmicAndFilter_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

class G4CosmicAndFilter: public FADS::ActionsBase , public FADS::UserAction {
private:

  int ntracks;
  
  StoreGateSvc*   m_storeGate;
  
  int m_ntot,m_npass;
  
  std::string m_collectionName;
  std::string m_collectionName2;
	  
public:
        G4CosmicAndFilter(std::string s):FADS::ActionsBase(s),FADS::UserAction(s),ntracks(0),m_storeGate(0),m_ntot(0),m_npass(0){};
	void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);
        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction(const G4Run*);
        void SteppingAction(const G4Step*);

};


#endif
