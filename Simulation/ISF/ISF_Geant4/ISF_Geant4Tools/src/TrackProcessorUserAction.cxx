/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackProcessorUserAction.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "TrackProcessorUserAction.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ITruthBinding.h"
#include "ISF_Event/EntryLayer.h"

#include "ISF_HepMC_Event/HepMC_TruthBinding.h"

#include "ISF_Interfaces/IParticleBroker.h"

// Athena includes
#include "AtlasDetDescr/AtlasRegion.h"

#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

#include "MCTruth/EventInformation.h"
#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/VTrackInformation.h"

#include "SimHelpers/StepHelper.h"
#include "StoreGate/StoreGateSvc.h"

// Geant4 includes
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
#include "G4LogicalVolumeStore.hh"
//#include "G4VPhysicalVolume.hh"

#include <iostream>

iGeant4::TrackProcessorUserAction::TrackProcessorUserAction(const std::string& type,
                                                            const std::string& name,
                                                            const IInterface* parent)
  : ITrackProcessorUserAction(name),
    AthAlgTool(type,name,parent),
    m_particleBroker("ISF::ParticleBroker/ISF_ParticleStackService",name),
    m_geoIDSvc("ISF::GeoIDSvc/ISF_GeoIDSvc",name),
    m_geoIDSvcQuick(0),
    m_idR(0.), m_idZ(0.),
    m_caloRmean(0.),  m_caloZmean(0.),
    m_muonRmean(0.), m_muonZmean(0.),
    m_cavernRmean(0.),  m_cavernZmean(0.),
    m_volumeOffset(1),
    m_minHistoryDepth(0),
    m_hasCavern(true),
    m_passBackEkinThreshold(0.05), //TODO: should we add units here (MeV)?
    m_killBoundaryParticlesBelowThreshold(false)
{

  ATH_MSG_DEBUG("create TrackProcessorUserAction name: "<<name);

  declareInterface<ITrackProcessorUserAction>(this);

  declareProperty("ParticleBroker", m_particleBroker, "ISF Particle Broker Svc");
  declareProperty("GeoIDSvc"      , m_geoIDSvc      , "ISF GeoID Svc"          );
  declareProperty("Geant4OnlyMode", m_geant4OnlyMode=false);
  declareProperty("TruthVolumeLevel", m_truthVolLevel=1, "Level in geo hierarchy for the truth volumes");

  declareProperty("PassBackEkinThreshold",
          m_passBackEkinThreshold=0.05, //TODO: should we add units here (MeV)?
          "Ekin cut-off for particles returned to ISF");
  declareProperty("KillBoundaryParticlesBelowThreshold",
          m_killBoundaryParticlesBelowThreshold=false,
          "Kill particles at boundary which are below Ekin cut-off rather than continue their simulation in G4");
}

StatusCode iGeant4::TrackProcessorUserAction::initialize()
{
  ATH_MSG_DEBUG("Initializing TrackProcessorUserAction");

  if (m_particleBroker.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve ISF Particle Broker: " << m_particleBroker);
    return StatusCode::FAILURE;
  }

  if (m_geoIDSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve ISF GeoID Svc: " << m_geoIDSvc);
    return StatusCode::FAILURE;
  }
  m_geoIDSvcQuick = &(*m_geoIDSvc);


  m_entryLayerMap["CALO::CALO"] = m_truthVolLevel+1;
  m_entryLayerMap["MUONQ02::MUONQ02"] = m_truthVolLevel+1;
  m_entryLayerMap["IDET::IDET"] = m_truthVolLevel+1;

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::TrackProcessorUserAction::finalize()
{
  return StatusCode::SUCCESS;
}


void iGeant4::TrackProcessorUserAction::BeginOfRunAction(const G4Run*)
{
  // get the geometry manager and check how many layers are present.
  G4TransportationManager *transportationManager(G4TransportationManager::GetTransportationManager());
  G4LogicalVolume *world((*(transportationManager->GetWorldsIterator()))->GetLogicalVolume());
  ATH_MSG_VERBOSE("World G4LogicalVolume Name: " << world->GetName() << " has " << world->GetNoDaughters() << " daughters.");
  if ("World::World"==world->GetName())
    {
      ATH_MSG_INFO("Atlas::Atlas is not the world volume, so assume we are in a cosmics job.");
      //Cosmics-specific configuration.
      m_volumeOffset=2;
      m_minHistoryDepth=1;
    }
}

void iGeant4::TrackProcessorUserAction::EndOfRunAction(const G4Run*)
{

}

void iGeant4::TrackProcessorUserAction::BeginOfEventAction(const G4Event*)
{
  //std::cout<<"clearing ISFParticle map"<<std::endl;
  m_parentISPmap.clear();
  m_curISP     =    0;
  m_curTrackID = -999;
}
void iGeant4::TrackProcessorUserAction::EndOfEventAction(const G4Event*)
{

}


void iGeant4::TrackProcessorUserAction::SteppingAction(const G4Step* aStep)
{

  G4Track*           aTrack   = aStep->GetTrack();
  int               aTrackID  = aTrack->GetTrackID();
  G4TrackStatus aTrackStatus  = aTrack->GetTrackStatus();

  const G4StepPoint *preStep  = aStep->GetPreStepPoint();
  const G4StepPoint *postStep = aStep->GetPostStepPoint();

  // update m_curISP if TrackID has changed or current ISP is unset
  if ( aTrackID!=m_curTrackID || !m_curISP ){
    m_curTrackID = aTrackID;

    std::map<int, ISF::ISFParticle*>::iterator ispIt = m_parentISPmap.find(m_curTrackID);
    m_curISP = ispIt!=m_parentISPmap.end() ? ispIt->second : 0;

    if ( !m_curISP ) {
      ATH_MSG_ERROR("No corresponding ISFParticle link found for current G4Track (TrackID=="<< m_curTrackID << ") !");
      G4ExceptionDescription description;
      description << G4String("SteppingAction: ") + "NULL ISFParticle pointer for current G4Step (trackID "
                  << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                  << ", parentID " << aTrack->GetParentID() << ")";
      G4Exception("iGeant4::TrackProcessorUserAction", "NoISFParticle", FatalException, description);
      return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }
  }

  // get geoID from parent
  AtlasDetDescr::AtlasRegion curGeoID = m_curISP->nextGeoID();
  ATH_MSG_DEBUG( "Currently simulating TrackID = " << m_curTrackID <<
                 " inside geoID = " << curGeoID );

  //std::cout<<"retrieved isp "<<m_curISP<<" for trackID "<<m_curTrackID<<std::endl;

  //
  // Geant4-only mode
  //
  if (m_geant4OnlyMode) {
    AtlasDetDescr::AtlasRegion nextG4GeoID = nextGeoId(aStep);
    if ( m_curISP->nextGeoID()!=nextG4GeoID ) {
      m_curISP->setNextGeoID( nextG4GeoID );
    }

    // check if dead track
    if ( aTrackStatus==fStopAndKill ) {
      ATH_MSG_DEBUG("Stepping dead G4Track, returning. TrackLength="<<aTrack->GetTrackLength()<<
                    " TrackEkin="<<aTrack->GetKineticEnergy()<<" TrackID="<<m_curTrackID);
      return;
    }

    const G4VPhysicalVolume *preVol  = preStep->GetPhysicalVolume();
    const G4VPhysicalVolume *postVol = postStep->GetPhysicalVolume();

    // check if particle left detector volume
    if ( postVol==0 ) {
      ATH_MSG_DEBUG("G4Step not in physical volume, returning. TrackLength="<<
                    aTrack->GetTrackLength()<<" TrackEkin="<<aTrack->GetKineticEnergy()<<
                    " TrackID="<<m_curTrackID);
      // left world
      return;
    }

    // check if particle is within same physical volume
    if ( preVol==postVol ) {
      ATH_MSG_DEBUG("G4Track stays inside geoID = "<<curGeoID);
      return;
    }

    //
    // this point is only reached if particle has crossed
    // a sub-det boundary in the Geant4-only mode
    //

    TrackHelper tHelp(aTrack);

    // only process particle at entry layer if primary or registered secondary

    if ( ! tHelp.IsSecondary() ) {

      // get entry layer
      ISF::EntryLayer layer = entryLayer(aStep);

      // update nextGeoID using Geant4 boundary definition
      AtlasDetDescr::AtlasRegion nextGeoID = nextGeoId(aStep);

      if (layer!=ISF::fUnsetEntryLayer) {

        // easier to create a new ISF particle than update existing one
        m_curISP = newISFParticle(aTrack, m_curISP, nextGeoID);
        m_particleBroker->registerParticle(m_curISP, layer, true);

      }
      m_curISP->setNextGeoID( AtlasDetDescr::AtlasRegion(nextGeoID) );
      const G4ThreeVector& particlePosition = aStep->GetPostStepPoint()->GetPosition();//Eigen compatibility HACK
      m_curISP->updatePosition(Amg::Vector3D(particlePosition.x(), particlePosition.y(), particlePosition.z()));//Eigen compatibility HACK
      const G4ThreeVector& particleMomentum = aStep->GetPostStepPoint()->GetMomentum();//Eigen compatibility HACK
      m_curISP->updateMomentum(Amg::Vector3D(particleMomentum.x(), particleMomentum.y(), particlePosition.z()));//Eigen compatibility HACK
    }

  //
  // non-Geant4-only mode
  //
  } else {
    // check geoID of postStep
    const G4ThreeVector     &postPos  = postStep->GetPosition();
    //const G4ThreeVector     &postMom  = postStep->GetMomentum();
    //AtlasDetDescr::AtlasRegion  nextGeoID = m_geoIDSvcQuick->identifyNextGeoID( postPos.x(),
    //                                                                            postPos.y(),
    //                                                                            postPos.z(),
    //                                                                            postMom.x(),
    //                                                                            postMom.y(),
    //                                                                            postMom.z() );
    AtlasDetDescr::AtlasRegion  nextGeoID = m_geoIDSvcQuick->identifyGeoID( postPos.x(),
                                                                            postPos.y(),
                                                                            postPos.z() );

    ATH_MSG_DEBUG("PostStep point resolved to geoID = "<<nextGeoID);

    // return if particle did NOT cross boundary
    if ( nextGeoID==curGeoID ) {
      ATH_MSG_DEBUG(" -> G4Track stays inside geoID = "<<curGeoID);

      //
      // for debugging:
      //
      if ( msgLvl(MSG::DEBUG) ) {
        const G4ThreeVector &prePos = preStep->GetPosition();
        AtlasDetDescr::AtlasRegion  preStepGeoID  = m_geoIDSvcQuick->identifyGeoID( prePos.x(),
                                                                                    prePos.y(),
                                                                                    prePos.z() );
        AtlasDetDescr::AtlasRegion  postStepGeoID = m_geoIDSvcQuick->identifyGeoID( postPos.x(),
                                                                                    postPos.y(),
                                                                                    postPos.z() );
      
        if( preStepGeoID!=postStepGeoID ) {
          const G4VPhysicalVolume     *preVol  = preStep->GetPhysicalVolume();
          const G4VPhysicalVolume     *postVol = postStep->GetPhysicalVolume();
          const G4ThreeVector         &preMom  = preStep->GetMomentum();
          const G4ThreeVector        &postMom  = postStep->GetMomentum();
          const G4TrackVector *secondaryVector = aStep->GetSecondary();
          const G4ThreeVector&      aTrack_pos = aTrack->GetPosition();
          const G4ThreeVector&      aTrack_mom = aTrack->GetMomentum();
          int pdgID=aTrack->GetDefinition()->GetPDGEncoding();
          int bc=0;
          TrackInformation* trackInfo=dynamic_cast<TrackInformation*>(aTrack->GetUserInformation());
          if (trackInfo) {
            bc=trackInfo->GetParticleBarcode();
          }

          ATH_MSG_WARNING("pre "<<preVol->GetName()<<" x="<<prePos.x()<<" y="<<prePos.y()<<" z="<<prePos.z()<<" p="<<preMom.mag()<<" geoID="<<preStepGeoID<<"; post "<<postVol->GetName()<<" x="<<postPos.x()<<" y="<<postPos.y()<<" z="<<postPos.z()<<" p="<<postMom.mag()<<" geoID="<<nextGeoID<<"; length="<<aStep->GetStepLength()<<"; n2nd="<<secondaryVector->size()<<" track  x="<<aTrack_pos.x()<<" y="<<aTrack_pos.y()<<" z="<<aTrack_pos.z()<<" p="<<aTrack_mom.mag()<<" curgeoID="<<curGeoID<<" pdgid="<<pdgID<<" bc="<<bc<<" trackID="<<m_curTrackID<<" ISF="<<m_curISP<<"; ploss="<<(postMom-preMom).mag());
        }
      }

      return;
    }
  
  
    //
    // this point is only reached if particle has crossed
    // a sub-det boundary in the non-Geant4-only mode
    //
  
    if ( aTrack->GetKineticEnergy() < m_passBackEkinThreshold ) {
      // kinetic energy of primary particle below threshold
      ATH_MSG_DEBUG(" -> G4Track enters geoID = " << nextGeoID <<
                    " but is below Ekin threshold. Not returned to ISF.");
      if ( m_killBoundaryParticlesBelowThreshold ) {
          aTrack->SetTrackStatus( fStopAndKill );
      } else {
          // TODO: link G4Track to ISF particle with the new GeoID
      }
    } else if ( aTrackStatus!=fAlive ) {
      // particle is killed by G4 in this step
      // TODO: do we need to handle this case specifically?
      ATH_MSG_DEBUG(" -> G4Track enters geoID = " << nextGeoID <<
                    " but is destroyed in this step. Not returned to ISF.");

    } else {
      // particle is above kinetic energy threshold and alive after this step
      // -> push new ISFParticle back to ISF particle broker
      ATH_MSG_DEBUG(" -> G4Track enters geoID = " << nextGeoID <<
                    " and is returned to ISF.");

      ISF::ISFParticle *parent = m_curISP;
      returnParticleToISF(aTrack, parent, nextGeoID);
    }

    //
    // handle secondaries that were created in this G4Step
    //
    const std::vector<const G4Track*> *secondaryVector = aStep->GetSecondaryInCurrentStep();
    // loop over new secondaries
       for ( const G4Track* aConstTrack_2nd : *secondaryVector ) {
      // get a non-const G4Track for current secondary (nasty!)
      G4Track *aTrack_2nd = const_cast<G4Track*>( aConstTrack_2nd );

      // check if new secondary position is behind boundary
         const G4ThreeVector&             pos_2nd = aTrack_2nd->GetPosition();
      AtlasDetDescr::AtlasRegion nextGeoID_2nd = m_geoIDSvcQuick->identifyGeoID( pos_2nd.x(),
                                                                                 pos_2nd.y(),
                                                                                 pos_2nd.z() );
      if( nextGeoID_2nd!=curGeoID ) {
        // secondary was generated in this step and has
        // a different geoID than the currently tracked one

        if ( aTrack_2nd->GetKineticEnergy() < m_passBackEkinThreshold ) {
          // kinetic energy of secondary particle below threshold
          ATH_MSG_DEBUG(" -> Secondary particle generated in this G4Step does not pass Ekin cut." <<
                        " Not returned to ISF.");
          if ( m_killBoundaryParticlesBelowThreshold ) {
              // TODO: should we use fKillTrackAndSecondaries instead?
              aTrack_2nd->SetTrackStatus( fStopAndKill );
          } else {
              // TODO: link G4Track to ISF particle with the new GeoID
          }
        } else {
          // secondary particle is above kinetic energy threshold
          // -> return it to ISF
          ATH_MSG_DEBUG(" -> Secondary particle generated in this G4Step is returned to ISF.");
          ISF::ISFParticle *parent = m_curISP;
          returnParticleToISF(aTrack_2nd, parent, nextGeoID_2nd);
        }
      }

    } // <-- end loop over new secondaries

  } // <-- end if non-Geant4-only mode

  return;
}

void iGeant4::TrackProcessorUserAction::PostUserTrackingAction(const G4Track* /*aTrack*/)
{

  return;
}

void iGeant4::TrackProcessorUserAction::PreUserTrackingAction(const G4Track* aTrack)
{

  G4Track* inT = const_cast<G4Track*> (aTrack);
  TrackHelper trackHelper(inT);

  EventInformation* eventInfo = static_cast<EventInformation*>
    (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());

  // set associated ISF particle
  TrackHelper tHelp(aTrack);
  ISF::ISFParticle* isp = 0;
  VTrackInformation* trackInfo=
    dynamic_cast<VTrackInformation*>(aTrack->GetUserInformation());

  if (trackInfo && trackInfo->GetISFParticle()) {

    //std::cout<<"have trackInfo and ISFParticle"<<std::endl;

    isp = const_cast<ISF::ISFParticle*> (trackInfo->GetISFParticle());

    if (!isp) ATH_MSG_ERROR("no ISFParticle in trackInfo for trackID "<<aTrack->GetTrackID()<<"!");

    int trackID=aTrack->GetTrackID();
    if (m_parentISPmap.find(trackID)==m_parentISPmap.end()) {
      m_parentISPmap[trackID]=isp;
      //std::cout<<"setting ISFParticle to "<<isp<<" for trackID "<<trackID<<" (have trackInfo)"<<std::endl;
    }
  }
  else if (aTrack->GetParentID()) {
    //std::cout<<"no trackInfo or no ISFParticle"<<std::endl;

    int parentID=aTrack->GetParentID();
    std::map<int, ISF::ISFParticle*>::iterator ispIt = m_parentISPmap.find(parentID);
    if ( ispIt == m_parentISPmap.end()) {
      ATH_MSG_ERROR("no parent ISFParticle found with G4Track parentID "<<parentID<<"!");
      isp = 0;
    } else {
      isp = ispIt->second;
    }

    //std::cout<<"got isp from parentID "<<parentID<<std::endl;

    int trackID=aTrack->GetTrackID();
    if (m_parentISPmap.find(trackID)==m_parentISPmap.end()) {
      m_parentISPmap[trackID]=isp;
      //std::cout<<"setting ISFParticle to "<<isp<<" for trackID "<<trackID<<std::endl;
    }

  }

  if (trackInfo && isp)
    trackInfo->SetISFParticle(const_cast<const ISF::ISFParticle*>(isp));


  if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary()) {
    HepMC::GenParticle* part=const_cast<HepMC::GenParticle*>(trackHelper.GetTrackInformation()->
                                                             GetHepMCParticle());

    if (trackHelper.IsPrimary()) eventInfo->SetCurrentPrimary(part);

    eventInfo->SetCurrentlyTraced(part);
  }

  // see if primary particle
  int pID = aTrack->GetParentID();
  bool isPrimary = !pID;

  if (!isPrimary) return;

  //
  // Get PrimaryParticleInformation from G4PrimaryParticle (assigned by TransportTool::addPrimaryVertex)
  // The ISFParticle should always exist, and the HepMC::GenParticle should exist if a primary EvGen particle (secondaries passed from G4 back to ISF for subsequent processing with G4 will have null pointer for HepMC::GenParticle)
  //

  PrimaryParticleInformation* ppInfo = dynamic_cast <PrimaryParticleInformation*> (aTrack->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation());
  if (!ppInfo) {
      G4ExceptionDescription description;
      description << G4String("PreUserTrackingAction: ") + "NULL PrimaryParticleInformation pointer for current G4Step (trackID "
                  << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                  << ", parentID " << aTrack->GetParentID() << ")";
      G4Exception("iGeant4::TrackProcessorUserAction", "NoPPInfo", FatalException, description);
      return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }
  // get parent
  const ISF::ISFParticle *parent = ppInfo->GetISFParticle();

  if (!parent) {ATH_MSG_ERROR("no ISFParticle in PrimaryParticleInformation for "<<aTrack->GetTrackID()<<"!");}

  // Set the event info properly
  eventInfo->SetCurrentISFPrimary(parent);

  // for particles passed to G4 from other simulations
  if (!m_geant4OnlyMode && !isp) {
    int trackID=aTrack->GetTrackID();
    m_parentISPmap[trackID]=const_cast<ISF::ISFParticle*>(parent);
  }

  return;
}

//_______________________________________________________________________
HepMC::GenParticle* iGeant4::TrackProcessorUserAction::findMatchingDaughter(HepMC::GenParticle* parent, bool verbose=false) const {
  // Add all necessary daughter particles
  if(NULL==parent->end_vertex()) {
    if(verbose) ATH_MSG_INFO ( "Number of daughters of "<<parent->barcode()<<": 0" );
    return parent;
  }
  const int pdgID(parent->pdg_id());
  for (HepMC::GenVertex::particles_out_const_iterator iter=parent->end_vertex()->particles_out_const_begin();
       iter!=parent->end_vertex()->particles_out_const_end(); ++iter){
    if (verbose) ATH_MSG_INFO ( "Daughter Particle of "<<parent->barcode()<<": " << **iter );
    if(NULL==(*iter)->end_vertex()) {
      if(verbose) ATH_MSG_INFO ( "Number of daughters of "<<(*iter)->barcode()<<": 0 (NULL)." );
    }
    else {
      if(verbose) ATH_MSG_INFO ("Number of daughters of "<<(*iter)->barcode()<<": " << (*iter)->end_vertex()->particles_out_size() );
    }
    if (pdgID == (*iter)->pdg_id()) {
      if (verbose) ATH_MSG_INFO ( "Look for daughters of Particle: " << (*iter)->barcode() );
      return this->findMatchingDaughter(*iter, verbose);
    }
  }
  if(!verbose) (void) this->findMatchingDaughter(parent, true);
  else {  ATH_MSG_ERROR ( "No matching Daughter Particles." ); }
  return parent;
}

//________________________________________________________________________
ISF::ISFParticle*
iGeant4::TrackProcessorUserAction::newISFParticle(G4Track* aTrack,
                                                  const ISF::ISFParticle* parent,
                                                  AtlasDetDescr::AtlasRegion  nextGeoID)
{

  G4ParticleDefinition* particleDefinition = aTrack->GetDefinition();
  //      const G4DynamicParticle* dynamicParticle = aTrack->GetDynamicParticle();

  const G4ThreeVector g4pos = aTrack->GetPosition();
  const double gTime = aTrack->GetGlobalTime();
  const HepGeom::Point3D<double> position(g4pos.x(),g4pos.y(),g4pos.z());

  G4ThreeVector g4mom = aTrack->GetMomentum();
  const HepGeom::Vector3D<double> momentum(g4mom.x(),g4mom.y(),g4mom.z());

  double mass    = particleDefinition->GetPDGMass();
  double charge  = particleDefinition->GetPDGCharge();
  int    pdgID   = particleDefinition->GetPDGEncoding();


  //  *AS* check if TrackInformation is available
  ISF::ITruthBinding* tBinding=0;
  Barcode::ParticleBarcode barcode=Barcode::fUndefinedBarcode;
  VTrackInformation * trackInfo= static_cast<VTrackInformation*>(aTrack->GetUserInformation());
  if (trackInfo && (trackInfo->GetHepMCParticle() ||
                    dynamic_cast<TrackBarcodeInfo*>(trackInfo))) {
    barcode=trackInfo->GetParticleBarcode();
    HepMC::GenParticle* genpart = const_cast<HepMC::GenParticle*>(trackInfo->GetHepMCParticle());
    if (genpart) {
      if (!m_geant4OnlyMode) {
        //find the last particle of this type in the decay chain - this is the one that we should pass back to ISF
        genpart = this->findMatchingDaughter(genpart);
      }
      tBinding=new ISF::HepMC_TruthBinding(*genpart);
      // particle should be already known to McTruth Tree
      tBinding->setPersistency( true);
    }
  }


  ISF::ISFParticle* isp= new ISF::ISFParticle( position,
                                               momentum,
                                               mass,
                                               charge,
                                               pdgID,
                                               gTime,
                                               *parent,
                                               barcode,
                                               tBinding );

  if (nextGeoID!=AtlasDetDescr::fUndefinedAtlasRegion) {
    isp->setNextGeoID( AtlasDetDescr::AtlasRegion(nextGeoID) );
    isp->setNextSimID( parent->nextSimID() );
  }

  //if (m_geant4OnlyMode) {
  //  if (trackInfo) {
  //    trackInfo->SetISFParticle(isp);
  //  }
  //}
  //// store new ISF particle in m_parentISPmap
  //if (aTrack->GetTrackStatus()==fAlive) {
  //  int trackID = aTrack->GetTrackID();
  //  if(trackID) {
  //    ATH_MSG_VERBOSE("Setting ISFParticle to "<<isp<<" for trackID "<<trackID<<" (from new ISFParticle)");
  //    m_parentISPmap[trackID] = isp;
  //  }
  //}  
  //FIXME: The two commented out if blocks above are correct and are meant
  //       to replace the if-else statement below. However, the above is not
  //       used now because of consistency with the already existing ~200M MC15 G4 Hits.
  //       Should be changed for MC15b and following!
  int trackID = aTrack->GetTrackID();
  if (m_geant4OnlyMode) {
    if (trackInfo && trackInfo->GetISFParticle()) {
     trackInfo->SetISFParticle(isp);
    }
    m_parentISPmap[trackID] = isp;
  } else {
    // store new ISF particle in m_parentISPmap
    if (aTrack->GetTrackStatus()==fAlive) {
      if(trackID) {
        ATH_MSG_VERBOSE("Setting ISFParticle to "<<isp<<" for trackID "<<trackID<<" (from new ISFParticle)");
        m_parentISPmap[trackID] = isp;
      }
    }
  }

  return isp;
}


//________________________________________________________________________
void
iGeant4::TrackProcessorUserAction::returnParticleToISF( G4Track *aTrack,
                                                        ISF::ISFParticle *parentISP,
                                                        AtlasDetDescr::AtlasRegion nextGeoID )
{
  // kill track inside G4
  aTrack->SetTrackStatus( fStopAndKill );

  // create new ISFParticle -> return value
  ISF::ISFParticle *newISP = newISFParticle(aTrack, parentISP, nextGeoID);

  // flag the track to let code downstream know that this track was returned to ISF
  VTrackInformation* trackInfo = dynamic_cast<VTrackInformation*>(aTrack->GetUserInformation());
  if (!trackInfo) {
    trackInfo = new TrackBarcodeInfo(Barcode::fUndefinedBarcode, newISP);
    aTrack->SetUserInformation(trackInfo);
  }
  trackInfo->SetReturnedToISF(true);


  // push the particle back to ISF
  m_particleBroker->push(newISP, parentISP);

  return;
}


//________________________________________________________________________
ISF::EntryLayer
iGeant4::TrackProcessorUserAction::entryLayer(const G4Step* aStep)
{
  //
  // this is the same prescription for getting the entry layer as in
  // MCTruth/MCTruthSteppingAction
  //

  const G4StepPoint *preStep =aStep->GetPreStepPoint();
  const G4StepPoint *postStep=aStep->GetPostStepPoint();

  G4TouchableHistory *preTHist=(G4TouchableHistory *)preStep->GetTouchable();
  G4TouchableHistory *postTHist=(G4TouchableHistory *)postStep->GetTouchable();
  int nLev1 = preTHist->GetHistoryDepth();
  int nLev2 = postTHist->GetHistoryDepth();

  std::map<std::string, int, std::less<std::string> >::const_iterator it;

  std::string vname1;
  bool pass=false;
  for (it=m_entryLayerMap.begin(); it!=m_entryLayerMap.end(); it++) {

    int il=(*it).second;

    if (il<=(nLev1+1)) {
      vname1=preTHist->GetVolume(nLev1-il+1)->GetName();

      if (vname1!=(*it).first) continue;

      if (il<=(nLev2+1)) {
        if (vname1==postTHist->GetVolume(nLev2-il+1)->GetName()) continue;
      }

      pass=true;
      break;
    }
  }


  ISF::EntryLayer layer=ISF::fUnsetEntryLayer;
  if (pass) {
    if      (vname1=="IDET::IDET")       layer=ISF::fAtlasCaloEntry;
    else if (vname1=="CALO::CALO")       layer=ISF::fAtlasMuonEntry;
    else if (vname1=="MUONQ02::MUONQ02") layer=ISF::fAtlasMuonExit;
  }

  return layer;
}


//________________________________________________________________________
AtlasDetDescr::AtlasRegion
iGeant4::TrackProcessorUserAction::nextGeoId(const G4Step* aStep)
{
  // Static so that it will keep the value from the previous step
  static AtlasDetDescr::AtlasRegion nextGeoID = m_truthVolLevel>1?AtlasDetDescr::fAtlasCavern:AtlasDetDescr::fUndefinedAtlasRegion;
  static const G4Track* aTrack = 0;
  StepHelper step(aStep);

  // Protect against being in a mother volume, then reset it to undefined if we don't know where we are
  //  Since we start particles in the beampipe (for the most part) this will rarely result in returning
  //  an undefined region answer; since G4 works on a stack rather than a queue, this is safe-ish
  if (step.PostStepBranchDepth()<m_truthVolLevel){
    if (m_truthVolLevel>1 &&
        step.PostStepBranchDepth()>m_truthVolLevel-1 && m_hasCavern &&
        step.GetPostStepLogicalVolumeName(m_truthVolLevel-1).find("CavernInfra") ) nextGeoID = AtlasDetDescr::fAtlasCavern;
    return nextGeoID;
  }
  if (aTrack != aStep->GetTrack()){
    // First step with this track!
    nextGeoID = AtlasDetDescr::fUndefinedAtlasRegion;
    aTrack = aStep->GetTrack();
  } // Otherwise use the cached value via the static

  static G4LogicalVolume * BPholder=0 , * IDholder=0 , * CALOholder=0 , * MUholder=0 , * TTRholder=0 ;
  if (BPholder==0){ // Initialize
    m_hasCavern=false;
    G4LogicalVolumeStore * lvs = G4LogicalVolumeStore::GetInstance();
    for (size_t i=0;i<lvs->size();++i){
      if ( !(*lvs)[i] ) continue;
      if ( (*lvs)[i]->GetName() == "BeamPipe::BeamPipe" ) BPholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "IDET::IDET" ) IDholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "CALO::CALO" ) CALOholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "MUONQ02::MUONQ02" ) MUholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "TTR_BARREL::TTR_BARREL" ) TTRholder = (*lvs)[i];
    }
    checkVolumeDepth( G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume() , m_truthVolLevel );
  }

  // Ordering inside out (most truth in the ID anyway...)
  if ( IDholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasID;
  } else if ( CALOholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasCalo;
  } else if ( MUholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasMS;
  } else if ( BPholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = (step.PostStepBranchDepth()>m_truthVolLevel && step.GetPostStepLogicalVolumeName(m_truthVolLevel+1)=="BeamPipe::BeamPipeCentral")?AtlasDetDescr::fAtlasID:AtlasDetDescr::fAtlasForward;
  } else if ( TTRholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasCavern;
  } else if (m_hasCavern && step.GetPostStepLogicalVolumeName(m_truthVolLevel-1).find("CavernInfra")!=std::string::npos) {
    nextGeoID = AtlasDetDescr::fAtlasCavern;
  } else {
    // We are in trouble
    //ATH_MSG_ERROR("vol1: "<<step.GetPostStepLogicalVolumeName(1)<<", vol2: "<<step.GetPostStepLogicalVolumeName(2)<<", postname="<<postname<<", returning undefined geoID");
    G4ThreeVector myPos = aStep->GetPostStepPoint()->GetPosition();
    ATH_MSG_ERROR("Returning undefined geoID from " << step.GetPostStepLogicalVolume() << " requesting " << step.GetPostStepLogicalVolume(m_truthVolLevel) << " at " << myPos.x() << " " << myPos.y() << " " << myPos.z() );
  }

  return nextGeoID;
}

bool iGeant4::TrackProcessorUserAction::checkVolumeDepth( G4LogicalVolume * lv , int volLevel , int d ) {
  if (lv==0) return false;
  bool Cavern = false;

  // Check the volumes rather explicitly
  if ( lv->GetName() == "BeamPipe::BeamPipe" ||
       lv->GetName() == "IDET::IDET" ||
       lv->GetName() == "CALO::CALO" ||
       lv->GetName() == "MUONQ02::MUONQ02" ||
       lv->GetName() == "TTR_BARREL::TTR_BARREL" ){
    if (d==volLevel){
      ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << d);
    } else {
      ATH_MSG_ERROR("Volume " << lv->GetName() << " at depth " << d << " instead of depth " << volLevel);
      throw "WrongDepth";
    } // Check of volume level
  } else if ( lv->GetName()=="BeamPipe::BeamPipeCentral" ){ // Things that are supposed to be one deeper
    if (d==volLevel+1){
      ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << d);
    } else {
      ATH_MSG_ERROR("Volume " << lv->GetName() << " at depth " << d << " instead of depth " << volLevel+1);
      throw "WrongDepth";
    } // Check of volume level
  } else if ( lv->GetName().find("CavernInfra")!=std::string::npos ){ // Things that are supposed to be one shallower
    m_hasCavern=true;
    if (d==volLevel-1){
      Cavern=true;
      ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << d);
      // Note: a number of volumes exist with "CavernInfra" in the name at the wrong depth, so we just need to
      //   check that there's at least one at the right depth
    } // Check of volume level
  } // Check of volume name

  // Going through the volume depth
  for (int i=0; i<lv->GetNoDaughters(); ++i){
    Cavern = Cavern || checkVolumeDepth( lv->GetDaughter(i)->GetLogicalVolume() , volLevel , d+1 );
  }
  if (d==0 && !Cavern && volLevel>1){
    ATH_MSG_ERROR("No CavernInfra volume registered at depth " << volLevel-1);
    throw "WrongDepth";
  }
  return Cavern;
}
