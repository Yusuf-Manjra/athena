/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_Geant4Tools/G4AtlasRunManager.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

#include "G4GeometryManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParallelWorldScoringProcess.hh"
#include "G4Run.hh"
#include "G4RegionStore.hh"
#include "G4ScoringManager.hh"
#include "G4StateManager.hh"
#include "G4TransportationManager.hh"
#include "G4UImanager.hh"
#include "G4UserRunAction.hh"
#include "G4Version.hh"

//________________________________________________________________________
iGeant4::G4AtlasRunManager::G4AtlasRunManager()
  : G4RunManager()
  , m_msg("G4AtlasRunManager")
  , m_releaseGeo(false)
  , m_recordFlux(false)
  , m_senDetTool("SensitiveDetectorMasterTool")
  , m_fastSimTool("FastSimulationMasterTool")
  , m_physListTool("PhysicsListToolBase")
  , m_userActionSvc("", "G4AtlasRunManager")
{
}

//________________________________________________________________________
iGeant4::G4AtlasRunManager* iGeant4::G4AtlasRunManager::GetG4AtlasRunManager()
{
  static G4AtlasRunManager* thisManager=0;
  if (!thisManager) thisManager=new G4AtlasRunManager;
  return thisManager;
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::SetUserActionSvc(const std::string& typeAndName)
{
  m_userActionSvc.setTypeAndName(typeAndName);
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::Initialize()
{
  // Call the base class Initialize method. This will call
  // InitializeGeometry and InitializePhysics.
  G4RunManager::Initialize();
  // Now that the G4 geometry is available, setup the user actions.
  if( !m_userActionSvc.name().empty() ) {
    ATH_MSG_INFO("Creating user actions now");
    if(m_userActionSvc.retrieve().isFailure()) {
      throw GaudiException("Could not retrieve UserActionSvc",
                           "CouldNotRetrieveUASvc", StatusCode::FAILURE);
    }
    if(m_userActionSvc->initializeActions().isFailure()) {
      throw GaudiException("Failed to initialize actions",
                           "UserActionInitError", StatusCode::FAILURE);
    }
  }
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::InitializeGeometry()
{
  ATH_MSG_INFO( "iGeant4::G4AtlasRunManager::InitializeGeometry()" );
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  IDetectorGeometrySvc *detGeoSvc;
  if (svcLocator->service("DetectorGeometrySvc",detGeoSvc,true).isFailure())
    {
      ATH_MSG_ERROR ( "Could not retrieve the DetectorGeometrySvc" );
      G4ExceptionDescription description;
      description << "InitializeGeometry: Failed to retrieve IDetectorGeometrySvc.";
      G4Exception("G4AtlasRunManager", "CouldNotRetrieveDetGeoSvc", FatalException, description);
      abort(); // to keep Coverity happy
    }
  G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
  ATH_MSG_INFO( "found lvs-size()==" << lvs->size() );
  G4LogicalVolumeStore::iterator volIt    = lvs->begin();
  G4LogicalVolumeStore::iterator volItEnd = lvs->end();
  for ( ; volIt!=volItEnd; ++volIt)
    {
      const G4String &volName = (*volIt)->GetName();

      if ( volName == "Muon::MuonSys" )
        {
          (*volIt)->SetSmartless( 0.1 );
          ATH_MSG_INFO( "Set smartlessness for Muon::MuonSys to 0.1" );
        }
      else if ( volName == "LArMgr::LAr::EMB::STAC")
        {
          (*volIt)->SetSmartless( 0.5 );
          ATH_MSG_INFO( "Set smartlessness for LArMgr::LAr::EMB::STAC to 0.5" );
        }
    }
  G4RunManager::SetUserInitialization(detGeoSvc->GetDetectorConstruction());
  if (userDetector)
    {
      G4RunManager::InitializeGeometry();
    }
  else
    {
      ATH_MSG_WARNING( " User Detector not set!!! Geometry NOT initialized!!!" );
    }

  // Geometry has been initialized.  Now get services to add some stuff to the geometry.
  if (m_senDetTool.retrieve().isFailure())
    {
      ATH_MSG_ERROR ( "Could not retrieve the SD master tool" );
      G4ExceptionDescription description;
      description << "InitializeGeometry: Failed to retrieve ISensitiveDetectorMasterTool.";
      G4Exception("G4AtlasRunManager", "CouldNotRetrieveSDMaster", FatalException, description);
      abort(); // to keep Coverity happy
    }
  if(m_senDetTool->initializeSDs().isFailure())
    {
      G4ExceptionDescription description;
      description << "InitializeGeometry: Call to ISensitiveDetectorMasterTool::initializeSDs failed.";
      G4Exception("G4AtlasRunManager", "FailedToInitializeSDs", FatalException, description);
      abort(); // to keep Coverity happy
    }
  return;
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::EndEvent()
{
  ATH_MSG_DEBUG( "G4AtlasRunManager::EndEvent" );
  // ZLM Note 1.12.2016: This function is not called
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::InitializePhysics()
{
  ATH_MSG_INFO( "iGeant4::G4AtlasRunManager::InitializePhysics()" );
  kernel->InitializePhysics();
  physicsInitialized = true;

  // Grab the physics list tool and set the extra options
  if (m_physListTool.retrieve().isFailure())
    {
      ATH_MSG_ERROR ( "Could not retrieve the physics list tool" );
      G4ExceptionDescription description;
      description << "InitializePhysics: Failed to retrieve IPhysicsListTool.";
      G4Exception("G4AtlasRunManager", "CouldNotRetrievePLTool", FatalException, description);
      abort(); // to keep Coverity happy
    }
  m_physListTool->SetPhysicsOptions();

  // Fast simulations last
  if (m_fastSimTool.retrieve().isFailure())
    {
      ATH_MSG_ERROR ( "Could not retrieve the FastSim master tool" );
      G4ExceptionDescription description;
      description << "InitializePhysics: Failed to retrieve IFastSimulationMasterTool.";
      G4Exception("G4AtlasRunManager", "CouldNotRetrieveFastSimMaster", FatalException, description);
      abort(); // to keep Coverity happy
    }
  if(m_fastSimTool->initializeFastSims().isFailure())
    {
      G4ExceptionDescription description;
      description << "InitializePhysics: Call to IFastSimulationMasterTool::initializeFastSims failed.";
      G4Exception("G4AtlasRunManager", "FailedToInitializeFastSims", FatalException, description);
      abort(); // to keep Coverity happy
    }

  if (m_recordFlux){
    G4UImanager *ui = G4UImanager::GetUIpointer();
    ui->ApplyCommand("/run/setCutForAGivenParticle proton 0 mm");

    G4ScoringManager* ScM = G4ScoringManager::GetScoringManagerIfExist();

    if(!ScM) return;

    //G4UImanager *ui=G4UImanager::GetUIpointer();

    ui->ApplyCommand("/score/create/cylinderMesh cylMesh_1");
    //                        R  Z(-24 to 24)
    ui->ApplyCommand("/score/mesh/cylinderSize 12. 24. m");
    //                iR iZ
    //ui->ApplyCommand("/score/mesh/nBin 1 1 1");
    ui->ApplyCommand("/score/mesh/nBin 120 480 1");

    ui->ApplyCommand("/score/quantity/energyDeposit eDep");

    ui->ApplyCommand("/score/quantity/cellFlux CF_photon");
    ui->ApplyCommand("/score/filter/particle photonFilter gamma");
    // above 2 line crete tally for cell flux for gamma

    ui->ApplyCommand("/score/quantity/cellFlux CF_neutron");
    ui->ApplyCommand("/score/filter/particle neutronFilter neutron");

    ui->ApplyCommand("/score/quantity/cellFlux CF_HEneutron");
    ui->ApplyCommand("/score/filter/particleWithKineticEnergy HEneutronFilter 20 7000000 MeV neutron");

    ui->ApplyCommand("/score/quantity/doseDeposit dose");

    ui->ApplyCommand("/score/close");
    ui->ApplyCommand("/score/list");

    G4int nPar = ScM->GetNumberOfMesh();

    if(nPar<1) return;

    G4ParticleTable::G4PTblDicIterator* particleIterator
     = G4ParticleTable::GetParticleTable()->GetIterator();

    for(G4int iw=0;iw<nPar;iw++)
    {
      G4VScoringMesh* mesh = ScM->GetMesh(iw);
      G4VPhysicalVolume* pWorld
         = G4TransportationManager::GetTransportationManager()
           ->IsWorldExisting(ScM->GetWorldName(iw));
      if(!pWorld)
      {
        pWorld = G4TransportationManager::GetTransportationManager()
           ->GetParallelWorld(ScM->GetWorldName(iw));
        pWorld->SetName(ScM->GetWorldName(iw));

        G4ParallelWorldScoringProcess* theParallelWorldScoringProcess
          = new G4ParallelWorldScoringProcess(ScM->GetWorldName(iw));
        theParallelWorldScoringProcess->SetParallelWorld(ScM->GetWorldName(iw));

        particleIterator->reset();
        while( (*particleIterator)() ){
          G4ParticleDefinition* particle = particleIterator->value();
          G4ProcessManager* pmanager = particle->GetProcessManager();
          if(pmanager)
          {
            pmanager->AddProcess(theParallelWorldScoringProcess);
            pmanager->SetProcessOrderingToLast(theParallelWorldScoringProcess, idxAtRest);
            pmanager->SetProcessOrderingToSecond(theParallelWorldScoringProcess, idxAlongStep);
            pmanager->SetProcessOrderingToLast(theParallelWorldScoringProcess, idxPostStep);
          }
        }
      }

      mesh->Construct(pWorld);
    }
  } // Do flux recording

  //  kernel->RunInitialization();
  RunInitialization();
  //std::cout<<"*AS* run init <<< "<<std::endl;
  return;
}

//________________________________________________________________________
bool iGeant4::G4AtlasRunManager::ProcessEvent(G4Event* event)
{

  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_GeomClosed);

  currentEvent = event;

  eventManager->ProcessOneEvent(currentEvent);
  if (currentEvent->IsAborted())
    {
      ATH_MSG_WARNING( "G4AtlasRunManager::ProcessEvent: Event Aborted at Detector Simulation level" );
      currentEvent=0;
      return true;
    }

  // *AS* AnalyzeEvent(currentEvent);
  if (currentEvent->IsAborted())
    {
      ATH_MSG_WARNING( "G4AtlasRunManager::ProcessEvent: Event Aborted at Analysis level" );
      currentEvent=0;
      return true;
    }

  if (m_recordFlux){
    G4ScoringManager* ScM = G4ScoringManager::GetScoringManagerIfExist();
    if(ScM){
      G4int nPar = ScM->GetNumberOfMesh();
      G4HCofThisEvent* HCE = currentEvent->GetHCofThisEvent();
      if(HCE && nPar>0){;
        G4int nColl = HCE->GetCapacity();
        for(G4int i=0;i<nColl;i++)
          {
            G4VHitsCollection* HC = HCE->GetHC(i);
            if(HC) ScM->Accumulate(HC);
          }
      }
    }
  }

  // stateManager->SetNewState(G4State_GeomClosed);
  StackPreviousEvent(currentEvent);
  bool abort=currentEvent->IsAborted();
  currentEvent = 0;

  // std::cout<<" ProcessEvent : done simulating one event "<<std::endl;
  return abort;
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::RunTermination()
{
  if (m_recordFlux){
    G4UImanager *ui=G4UImanager::GetUIpointer();
    ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 eDep edep.txt");
    ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 CF_neutron neutron.txt");
    ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 CF_HEneutron HEneutron.txt");
    ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 CF_photon photon.txt");
    ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 dose dose.txt");
  }

  // std::cout<<" this is G4AtlasRunManager::RunTermination() "<<std::endl;
#if G4VERSION_NUMBER < 1010
  for (size_t itr=0;itr<previousEvents->size();itr++) { delete (*previousEvents)[itr]; }
#else
  this->CleanUpPreviousEvents();
#endif
  previousEvents->clear();

  if (userRunAction) { userRunAction->EndOfRunAction(currentRun); }

  delete currentRun;
  currentRun = 0;
  runIDCounter++;

  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_Idle);

  G4GeometryManager::GetInstance()->OpenGeometry();

  kernel->RunTermination();

  userRunAction=0;
  userEventAction=0;
  userSteppingAction=0;
  userStackingAction=0;
  userTrackingAction=0;
  userDetector=0;
  userPrimaryGeneratorAction=0;


}
