/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasTools/PhysicsListToolBase.h"

#include "G4VUserPhysicsList.hh"
#include "G4RunManager.hh"
#include "G4EmProcessOptions.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"

#include "CLHEP/Units/PhysicalConstants.h"

PhysicsListToolBase::PhysicsListToolBase(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent)
  , m_phys_option(this)
  , m_phys_decay(this)
  , m_physicsList(nullptr)
{
  declareInterface<IPhysicsListTool>(this);
  declareProperty("PhysicsList"     , m_physicsListName = "FTFP_BERT", "Name for physics list");
  declareProperty("NeutronTimeCut"  , m_neutronTimeCut = 0,            "Time cut for neutron killer");
  declareProperty("NeutronEnergyCut", m_neutronEnergyCut = 0,          "Energy cut for neutron killer");
  declareProperty("GeneralCut"      , m_generalCut = 0,                "General cut");
  declareProperty("EMMaxEnergy"     , m_emMaxEnergy = 7*CLHEP::TeV,    "Maximum energy for EM tables");
  declareProperty("EMMinEnergy"     , m_emMinEnergy = 100*CLHEP::eV,   "Minimum energy for EM tables");
  declareProperty("EMDEDXBinning"   , m_emDEDXBinning = 77,            "Binning for EM dE/dX tables");
  declareProperty("EMLambdaBinning" , m_emLambdaBinning = 77,          "Binning for EM Lambda tables");
  declareProperty("ApplyEMCuts"     , m_applyEMCuts = true,            "Apply cuts EM flag in Geant4");
  declareProperty("PhysOption"      , m_phys_option ,                  "Tool handle array of physics options" );
  declareProperty("PhysicsDecay"    , m_phys_decay ,                   "Tool handle array of physics decays" );
}


StatusCode PhysicsListToolBase::initialize( )
{
  if (m_phys_option.size())
    {
      ATH_MSG_INFO( "Initializing list of " <<  m_phys_option.size() << " physics options"  );
      CHECK( m_phys_option.retrieve() );
    }

  if (m_phys_decay.size())
    {
      ATH_MSG_INFO( "Initializing list of " <<  m_phys_decay.size() << " Decays "  );
      CHECK( m_phys_decay.retrieve() );
    }

  return StatusCode::SUCCESS;
}


void PhysicsListToolBase::CreatePhysicsList()
{
  G4PhysListFactory factory;
  if (m_physicsListName != "" && factory.IsReferencePhysList(m_physicsListName))
    {
      m_physicsList = factory.GetReferencePhysList(m_physicsListName);
    }
  if (!m_physicsList)
    {
      ATH_MSG_ERROR("Unable to initialize physics List: " << m_physicsList);
      throw "PhysicsListInitializationError";
    }
  // Call these as functions.  As this could be used as a base class, having
  //   these as separate functions lets someone who is inheriting from this use
  //   them...

  //Register physics options to the G4VModularPhysicsList
  for (auto itr: m_phys_option)
    {
      m_physicsList->RegisterPhysics(itr->GetPhysicsOption());
    }
  //Register decays to the G4VModularPhysicsList
  for (auto itr: m_phys_decay)
    {
      m_physicsList->RegisterPhysics(itr->GetPhysicsOption());
    }

  //ConstructProcess();
}

G4VUserPhysicsList* PhysicsListToolBase::GetPhysicsList()
{
  if (!m_physicsList)
    {
      CreatePhysicsList();
    }
  return m_physicsList;
}

void PhysicsListToolBase::SetPhysicsList()
{
  CreatePhysicsList();
  G4RunManager::GetRunManager()->SetUserInitialization(m_physicsList);
}

void PhysicsListToolBase::SetPhysicsOptions()
{
  if (!m_physicsList)
    {
      ATH_MSG_WARNING("Physics list not initialized before calling ConstructProcess()");
      return;
    }

  if (m_neutronTimeCut)
    {
      std::ostringstream oss;
      oss<<"/physics_engine/neutron/timeLimit "<<m_neutronTimeCut<<" ns";
      G4UImanager* UImanager = G4UImanager::GetUIpointer();
      UImanager->ApplyCommand(oss.str());
    }

  if (m_neutronEnergyCut > 0)
    {
      std::ostringstream oss;
      oss<<"/physics_engine/neutron/energyLimit "<<m_neutronEnergyCut<<" MeV";
      G4UImanager* UImanager = G4UImanager::GetUIpointer();
      UImanager->ApplyCommand(oss.str());
    }

  G4EmProcessOptions emo;
  emo.SetMaxEnergy(m_emMaxEnergy);
  emo.SetDEDXBinning(m_emDEDXBinning);
  emo.SetLambdaBinning(m_emLambdaBinning);
  emo.SetMinEnergy(m_emMinEnergy);
  if (m_applyEMCuts)
    {
      emo.SetApplyCuts(true);
    }
  return;
}
