/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/CosmicPerigeeAction.h"
#include "MCTruth/TrackHelper.h"
#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include <iostream>

//static CosmicPerigeeAction cpa("CosmicPerigeeAction");

CosmicPerigeeAction::CosmicPerigeeAction(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),trackRecordCollection(0),m_idZ(0),m_idR(0),m_allowMods(0) {


  declareProperty("AllowMods",m_allowMods);

}

void CosmicPerigeeAction::BeginOfEvent(const G4Event*)
{
  //trackRecordCollection = new TrackRecordCollection("CosmicPerigee");
  trackRecordCollection = theHelper.RetrieveNonconstCollection<TrackRecordCollection>("CosmicPerigee");
  EnvelopeGeometryManager *gm=EnvelopeGeometryManager::GetGeometryManager();
  m_idR = gm->IdetOuterRadius();
  m_idZ = gm->IdetMaxZ();
}
void CosmicPerigeeAction::EndOfEvent(const G4Event*)
{

  if (!m_allowMods)
    theHelper.SetConstCollection(trackRecordCollection);

}

void CosmicPerigeeAction::BeginOfRun(const G4Run*)
{
}

void CosmicPerigeeAction::Step(const G4Step* aStep)
{
  static double pMinPrimary=100*CLHEP::MeV;
  static bool hasBeenSaved = false;

  // See if this is a new track
  if (aStep->GetPreStepPoint()->GetStepStatus() == fUndefined) hasBeenSaved = false;

  // See if we've already saved it
  if (hasBeenSaved) return;

  // Only save muons or tracks in the ID
  if (aStep->GetTrack()->GetDefinition() != G4MuonPlus::Definition() &&
      aStep->GetTrack()->GetDefinition() != G4MuonMinus::Definition() &&
      ( aStep->GetPostStepPoint()->GetPosition().rho() > m_idR ||
        aStep->GetPostStepPoint()->GetPosition().z() > m_idZ   ||
        aStep->GetPostStepPoint()->GetPosition().z() < -m_idZ   ) ){
    // Both not a muon and not in the ID
    return;
  }

  // Check momentum
  if (aStep->GetTrack()->GetMomentum().mag() < pMinPrimary) return;

  // First order approximation of the answer to "is this the perigee"
  // if it is now moving away from the perigee and has not been saved, save it
  // That is true if mom dot pos > 0
  if ( aStep->GetTrack()->GetMomentum().x() * aStep->GetPostStepPoint()->GetPosition().x() +
       aStep->GetTrack()->GetMomentum().y() * aStep->GetPostStepPoint()->GetPosition().y() < 0) return;

  // Save the vertex...
  hasBeenSaved = true;

  // Decide whether to save the prestep or poststep point - which one is closer to Perigee?
  G4StepPoint *theStep = aStep->GetPreStepPoint();
  if ( fabs( aStep->GetPreStepPoint()->GetMomentumDirection().x() * aStep->GetPreStepPoint()->GetPosition().x() +
             aStep->GetPreStepPoint()->GetMomentumDirection().y() * aStep->GetPreStepPoint()->GetPosition().y() ) >
       fabs( aStep->GetPostStepPoint()->GetMomentumDirection().x() * aStep->GetPostStepPoint()->GetPosition().x() +
             aStep->GetPostStepPoint()->GetMomentumDirection().y() * aStep->GetPostStepPoint()->GetPosition().y() ) ){
    // Using the post step point
    theStep = aStep->GetPostStepPoint();
  }

  int pdgcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  double ener = theStep->GetTotalEnergy();
  G4ThreeVector pos = theStep->GetPosition();
  G4ThreeVector mom = theStep->GetMomentum();
  double time       = theStep->GetGlobalTime();
  G4VPhysicalVolume *preVol = theStep->GetPhysicalVolume();

  if (aStep->GetTrack()->GetDefinition() == G4Geantino::Definition() ) pdgcode=999;
  if (aStep->GetTrack()->GetDefinition() == G4ChargedGeantino::Definition() ) pdgcode=998;

  //create the TimedTrackRecord
  TrackHelper trHelp(aStep->GetTrack());
  int barcode = trHelp.GetBarcode();
  trackRecordCollection->Emplace(pdgcode,ener,mom,pos,time,barcode,preVol->GetName());
}

StatusCode CosmicPerigeeAction::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode CosmicPerigeeAction::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
