/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "FastCaloSimV2Tool.h"


// FastCaloSim includes
#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include "ISF_FastCaloSimParametrization/CaloGeometryFromCaloDDM.h"

//!
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictParser/IdDictParser.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/TileID.h"
//!

// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"

#include "PathResolver/PathResolver.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TFile.h"
#include <fstream>

using std::abs;
using std::atan2;

/** Constructor **/
ISF::FastCaloSimV2Tool::FastCaloSimV2Tool( const std::string& type, const std::string& name,  const IInterface* parent)
  : BaseSimulatorTool(type, name, parent)
{
}

/** framework methods */
StatusCode ISF::FastCaloSimV2Tool::initialize()
{
  ATH_CHECK( BaseSimulatorTool::initialize() );

  ATH_CHECK( m_caloCellMakerToolsSetup.retrieve() );
  ATH_MSG_DEBUG( "Successfully retrieve CaloCellMakerTools: " << m_caloCellMakerToolsSetup );
  ATH_CHECK( m_caloCellMakerToolsRelease.retrieve() );

  ATH_CHECK(m_rndGenSvc.retrieve());
  m_randomEngine = m_rndGenSvc->GetEngine( m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_ERROR("Could not get random number engine from RandomNumberService. Abort.");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_paramSvc.retrieve());

  // Get FastCaloSimCaloExtrapolation
  ATH_CHECK(m_FastCaloSimCaloExtrapolation.retrieve());

  // Output data handle
  ATH_CHECK( m_caloCellKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimV2Tool::setupEventST()
{
  ATH_MSG_DEBUG ("setupEventST");

  m_theContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);

  ATH_CHECK(evtStore()->record(m_theContainer, m_caloCellsOutputName));

  return this->commonSetup();
}

StatusCode ISF::FastCaloSimV2Tool::setupEvent()
{
  ATH_MSG_DEBUG ("setupEvent");

  m_theContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);

  return this->commonSetup();
}

StatusCode ISF::FastCaloSimV2Tool::commonSetup()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  for (const ToolHandle<ICaloCellMakerTool>& tool : m_caloCellMakerToolsSetup)
    {
      std::string chronoName=this->name()+"_"+ tool.name();
      if (m_chrono) m_chrono->chronoStart(chronoName);
      StatusCode sc = tool->process(m_theContainer, ctx);
      if (m_chrono) {
        m_chrono->chronoStop(chronoName);
        ATH_MSG_DEBUG( "Chrono stop : delta " << m_chrono->chronoDelta (chronoName,IChronoStatSvc::USER) * CLHEP::microsecond / CLHEP::second << " second " );
      }

      if (sc.isFailure())
        {
          ATH_MSG_ERROR( "Error executing tool " << tool.name() );
          return StatusCode::FAILURE;
        }
    }

  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimV2Tool::releaseEvent()
{
  ATH_MSG_VERBOSE( "FastCaloSimV2Tool " << name() << " releaseEvent() " );
  // Run the version of releaseEvent that returns the output collection
  // Run the normal method
  ATH_CHECK(this->releaseEventST());
  if ( m_theContainer ) {

    // Record with WriteHandle
    SG::WriteHandle< CaloCellContainer > caloCellHandle( m_caloCellKey, Gaudi::Hive::currentContext() );
    ATH_CHECK( caloCellHandle.record( std::make_unique< CaloCellContainer >( *m_theContainer ) ) );
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode ISF::FastCaloSimV2Tool::releaseEventST()
{
  ATH_MSG_VERBOSE("release Event");
  const EventContext& ctx = Gaudi::Hive::currentContext();

  //run release tools in a loop
  for (const ToolHandle<ICaloCellMakerTool>& tool : m_caloCellMakerToolsRelease)
    {
      ATH_MSG_VERBOSE( "Calling tool " << tool.name() );

      ATH_CHECK(tool->process(m_theContainer, ctx));
    }

  return StatusCode::SUCCESS;

}

/** Simulation Call */
StatusCode ISF::FastCaloSimV2Tool::simulate(const ISF::ISFParticle& isfp, ISFParticleContainer&, McEventCollection*) const
{

  ATH_MSG_VERBOSE("NEW PARTICLE! FastCaloSimV2Tool called with ISFParticle: " << isfp);

  Amg::Vector3D particle_position =  isfp.position();
  Amg::Vector3D particle_direction(isfp.momentum().x(),isfp.momentum().y(),isfp.momentum().z());

  //int barcode=isfp.barcode(); // isfp barcode, eta and phi: in case we need them
  // float eta_isfp = particle_position.eta();
  // float phi_isfp = particle_position.phi();

  //Don't simulate particles with total energy below 10 MeV
  if(isfp.ekin() < 10) {
    ATH_MSG_VERBOSE("Skipping particle with Ekin: " << isfp.ekin() <<" MeV. Below the 10 MeV threshold.");
    return StatusCode::SUCCESS;
  }


  /// for anti protons and anti neutrons the kinetic energy should be
  /// calculated as Ekin = E() + M() instead of E() - M()
  /// this is achieved by constructing the TFCSTruthState with negative mass
  float isfp_mass = isfp.mass();
  if(isfp.pdgCode() == -2212 or isfp.pdgCode() == -2112) {
    isfp_mass = - isfp_mass;
    ATH_MSG_VERBOSE("Found anti-proton/neutron, negative mass is used for TFCSTruthState ");
  }


  TFCSTruthState truth;
  truth.SetPtEtaPhiM(particle_direction.perp(), particle_direction.eta(), particle_direction.phi(), isfp_mass);
  truth.set_pdgid(isfp.pdgCode());
  truth.set_vertex(particle_position[Amg::x], particle_position[Amg::y], particle_position[Amg::z]);

  TFCSExtrapolationState extrapol;
  m_FastCaloSimCaloExtrapolation->extrapolate(extrapol,&truth);
  TFCSSimulationState simulstate(m_randomEngine);

  ATH_MSG_DEBUG(" particle: " << isfp.pdgCode() << " Ekin: " << isfp.ekin() << " position eta: " << particle_position.eta() << " direction eta: " << particle_direction.eta() << " position phi: " << particle_position.phi() << " direction phi: " << particle_direction.phi());

  ATH_CHECK(m_paramSvc->simulate(simulstate, &truth, &extrapol));

  ATH_MSG_DEBUG("Energy returned: " << simulstate.E());
  ATH_MSG_VERBOSE("Energy fraction for layer: ");
  for (int s = 0; s < CaloCell_ID_FCS::MaxSample; s++)
    ATH_MSG_VERBOSE(" Sampling " << s << " energy " << simulstate.E(s));

  //Now deposit all cell energies into the CaloCellContainer
  for(const auto& iter : simulstate.cells()) {
    CaloCell* theCell = (CaloCell*)m_theContainer->findCell(iter.first->calo_hash());
    theCell->addEnergy(iter.second);
  }

  return StatusCode::SUCCESS;
}
