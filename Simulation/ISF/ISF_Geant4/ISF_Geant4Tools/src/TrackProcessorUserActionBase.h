/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H
#define ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H

#include "G4AtlasTools/UserActionBase.h"

//#include "ISF_Geant4Interfaces/ITrackProcessorUserAction.h"

#include <string>

// forward declarations
class EventInformation;

namespace HepMC {
  class GenParticle;
}

namespace ISF {
  class ISFParticle;
}

namespace iGeant4 {

  class TrackProcessorUserActionBase: public UserActionBase {

  public:
    TrackProcessorUserActionBase(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TrackProcessorUserActionBase() {}

    virtual StatusCode queryInterface(const InterfaceID&, void**) override;

    virtual void BeginOfEvent(const G4Event*) override final;
    virtual void EndOfEvent(const G4Event*) override final;

    virtual void PreTracking(const G4Track* aTrack) override final;
    virtual void PostTracking(const G4Track*) override final;

    virtual void Step(const G4Step*) override final;

  protected:
    EventInformation                   *m_eventInfo;   //!< event-global G4 UserInformation

  private:
    /** This method is called by TrackProcessorUserActionBase after the
     *  G4Track->ISFParticle association has been established for the current G4Track */
    virtual void ISFSteppingAction(const G4Step*, ISF::ISFParticle *curISP) = 0;

    /** for keeping track of the currently processed G4Track and its corresponding ISFParticle */
    ISF::ISFParticle                   *m_curBaseISP;  //!< the corresponding ISFParticle to this G4Track
  };

}

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H
