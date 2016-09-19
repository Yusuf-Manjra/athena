/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4TrackCounter.h"
#include "MCTruth/TrackHelper.h"
#include <iostream>

//static G4TrackCounter ts1("G4TrackCounter");

void G4TrackCounter::BeginOfEvent(const G4Event*)
{
	nevts++;
	ATH_MSG_DEBUG("increasing nevts to "<<nevts);
}


void G4TrackCounter::EndOfRun(const G4Run*)
{
  if (nevts>0){
    avtracks=ntracks/(double)nevts;
    avtracks_en=ntracks_en/(double)nevts;
    avtracks_sec=ntracks_sec/(double)nevts;
  } else {
    avtracks=0;
    avtracks_en=0;
    avtracks_sec=0;
  }

  ATH_MSG_INFO("*****************************************************");
  ATH_MSG_INFO("number of G4Events has been "<<nevts<<", total ntracks: "<<ntracks_tot);
  ATH_MSG_INFO("average number of primary tracks per event has been "<<avtracks);
  ATH_MSG_INFO("average number of registered secondary tracks per event has been "<<avtracks_sec);
  ATH_MSG_INFO("average number of tracks with E>50MeV has been "<<avtracks_en);
  ATH_MSG_INFO("*****************************************************");
}


void G4TrackCounter::PreTracking(const G4Track* aTrack)
{

  ntracks_tot++;

  TrackHelper theHelper(aTrack);

  if(theHelper.IsPrimary()||
     theHelper.IsRegeneratedPrimary()) ntracks++;

  if(theHelper.IsRegisteredSecondary()) ntracks_sec++;

  if(aTrack->GetKineticEnergy()>50) ntracks_en++;

}

StatusCode G4TrackCounter::initialize()
{
	return StatusCode::SUCCESS;
}


StatusCode G4TrackCounter::queryInterface(const InterfaceID& riid, void** ppvInterface)
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

//=============================================================================
// New design of G4TrackCounter for multithreading
//=============================================================================

#include "G4ios.hh"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // merge results
  //---------------------------------------------------------------------------

  void G4TrackCounter::Report::merge(const G4TrackCounter::Report& rep){

    nEvents+=rep.nEvents;
    nTotalTracks+=rep.nTotalTracks;
    nPrimaryTracks+=rep.nPrimaryTracks;
    nSecondaryTracks+=rep.nSecondaryTracks;
    n50MeVTracks+=rep.n50MeVTracks;
  }

  //---------------------------------------------------------------------------
  // Increment event counter
  //---------------------------------------------------------------------------
  void G4TrackCounter::beginOfEvent(const G4Event* /*event*/)
  {
    //G4cout << "G4TrackCounter@" << this << "::beginOfEvent" << G4endl;
    m_report.nEvents++;
  }

  //---------------------------------------------------------------------------
  // Increment track counters
  //---------------------------------------------------------------------------
  void G4TrackCounter::preTracking(const G4Track* track)
  {
    //G4cout << "G4TrackCounter@" << this << "::preTracking" << G4endl;
    m_report.nTotalTracks++;
    TrackHelper helper(track);

    // Primary tracks
    if(helper.IsPrimary() || helper.IsRegeneratedPrimary())
      m_report.nPrimaryTracks++;

    // Secondary tracks
    if(helper.IsRegisteredSecondary())
      m_report.nSecondaryTracks++;

    // 50 MeV tracks
    const double minE = 50.;
    if(track->GetKineticEnergy() > minE)
      m_report.n50MeVTracks++;
  }

} // namespace G4UA
