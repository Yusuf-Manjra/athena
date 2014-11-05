/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleBrokerDynamicOnReadIn.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Framework
#include "GaudiKernel/ITHistSvc.h"
// C include
#include <assert.h>
// DetectorDescription
#include "AtlasDetDescr/AtlasRegionHelper.h"
// ISF_Services include
#include "ISF_Services/ParticleBrokerDynamicOnReadIn.h"
// ISF_Event
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleContainer.h"
#include "ISF_Event/ISFBenchmarkHelper.h"
// ISF_Interfaces include
#include "ISF_Interfaces/IStackFiller.h"
#include "ISF_Interfaces/IEntryLayerTool.h"
#include "ISF_Interfaces/IGeoIDSvc.h"
#include "ISF_Interfaces/SimulationFlavor.h"
// barcode utils
#include "BarcodeServices/BitCalculator.h"
// Benchmarking
#include "PmbCxxUtils/CustomBenchmark.h"
// ROOT includes
#include "TTree.h"
// AtlasDetDescr
#include "AtlasDetDescr/AtlasRegionHelper.h"

/** Constructor **/
ISF::ParticleBrokerDynamicOnReadIn::ParticleBrokerDynamicOnReadIn(const std::string& name,ISvcLocator* svc) :
  AthService(name,svc),
  m_particleStackFiller("ISF::ISF_GenEventStackFiller/StackFiller"),
  m_entryLayerTool("iGeant4::EntryLayerTool/ISF_EntryLayerTool"),
  m_entryLayerToolQuick(0),
  m_orderingTool(""),
  m_orderingToolQuick(0),
  m_geoIDSvc("", name),
  m_geoIDSvcQuick(0),
  m_forceGeoIDSvc(false),
  m_validateGeoID(false),
  m_particles(),
  m_popParticles(),
  m_simSelector(),
  m_simSelectorSet(),
  m_screenOutputPrefix("isf >> "),
  m_screenEmptyPrefix(),
  m_barcodeSvc("", name),
  m_doSelectorCPUMon(false),
  m_benchPDGCode(0),
  m_benchGeoID(0),
  m_validationOutput(false),
  m_thistSvc("THistSvc",name),
  m_validationStream("ParticleBroker"),
  m_t_pushPosition(0),
  m_t_entryLayerPos(),
  m_val_pdg(0),
  m_val_p(0.),
  m_val_x(0.),
  m_val_y(0.),
  m_val_z(0.),
  m_simflavor(ISF::UndefinedSim)
{
    // the particle stack filler tool
    declareProperty("StackFiller"                , m_particleStackFiller  );
    // the entry layer tool to write TrackRecordCollections
    declareProperty("EntryLayerTool"             , m_entryLayerTool       );
    // particle ing tool
    declareProperty("ParticleOrderingTool"       , m_orderingTool         );
    // the geo service and selectors needed
    declareProperty("GeoIDSvc"                   , m_geoIDSvc             );
    declareProperty("AlwaysUseGeoIDSvc"          , m_forceGeoIDSvc        );
    declareProperty("ValidateGeoIDs"             , m_validateGeoID        );
    // collect and print cpu monitoring information
    declareProperty("SimSelectorCPUMonitoring"   , m_doSelectorCPUMon     );
    // refine the screen output for debugging
    declareProperty("ScreenOutputPrefix"         , m_screenOutputPrefix   );
    // The Particle/Vertex BarcodeService used in ISF to store barcode info
    declareProperty("BarcodeService"             , m_barcodeSvc,  
		    "The Particle/Vertex BarcodeService used in ISF" );
    
    // write out validation info
    declareProperty( "ValidationOutput",
                     m_validationOutput = false,
                     "If turned on, write out a ROOT tree.");
    declareProperty("ValidationStreamName",
                     m_validationStream = "ParticleBroker",
                     "Name of the output stream" );
    declareProperty("THistService",
                     m_thistSvc,
                     "The THistSvc" );
}


ISF::ParticleBrokerDynamicOnReadIn::~ParticleBrokerDynamicOnReadIn() 
{}


/** framework methods */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::initialize()
{

    // Screen output
    for (size_t prl = 0; prl < m_screenOutputPrefix.size(); ++prl) m_screenEmptyPrefix += " ";
    ATH_MSG_DEBUG ( m_screenOutputPrefix << "--------------------------------------------------------");
    
    // retrieve the particle stack filler tool
    if ( m_particleStackFiller.retrieve().isFailure() ){
        ATH_MSG_FATAL( m_screenOutputPrefix <<  "Could not retrieve ParticleStack Filler. Abort.");
        return StatusCode::FAILURE;
    } else
        ATH_MSG_INFO( m_screenEmptyPrefix <<  "- StackFiller      : " << m_particleStackFiller.typeAndName() );

    // retrieve the entry layer tool
    if ( m_entryLayerTool.retrieve().isFailure() ){
        ATH_MSG_FATAL( m_screenOutputPrefix <<  "Could not retrieve EntryLayer Tool. Abort.");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO( m_screenEmptyPrefix <<  "- EntryLayerTool   : " << m_entryLayerTool.typeAndName() );
        // store a quick-access-pointer (removes GaudiOverhead)
        m_entryLayerToolQuick = &(*m_entryLayerTool);
    }

    // retrieve the particle ing tool if given
    if ( ! m_orderingTool.empty()) {
      if ( m_orderingTool.retrieve().isFailure() ){
          ATH_MSG_FATAL( m_screenOutputPrefix <<  "Could not retrieve ParticleOrderingTool. Abort.");
          return StatusCode::FAILURE;
      } else {
          ATH_MSG_INFO( m_screenEmptyPrefix <<  "- Particel OrderingTool   : " << m_orderingTool.typeAndName() );
          // store a quick-access-pointer (removes GaudiOverhead)
          m_orderingToolQuick = &(*m_orderingTool);
      }
    }

    // retrieve the geo identification decision tool
    if ( m_geoIDSvc.retrieve().isFailure()){
        ATH_MSG_FATAL( m_screenOutputPrefix << "Could not retrieve GeometryIdentifier Service. Abort.");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO( m_screenEmptyPrefix <<  "- GeoIDSvc         : " 
                      << (m_geoIDSvc.empty() ? "<not configured>" : m_geoIDSvc.typeAndName()) );
        // store a quick-access-pointer (removes GaudiOverhead)
        m_geoIDSvcQuick = &(*m_geoIDSvc);
    }

    // retrieve the barcode service
    if ( m_barcodeSvc.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_barcodeSvc.propertyName() << ": Failed to retrieve service " << m_barcodeSvc.type());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( m_barcodeSvc.propertyName()  << ": Retrieved service " << m_barcodeSvc.type());
    }

    // setup CPU Benchmarks
    if (m_doSelectorCPUMon) {
      if (!m_benchPDGCode)
        m_benchPDGCode = new PMonUtils::CustomBenchmark(ISF::fMaxBenchmarkPDGCode );
      if (!m_benchGeoID)
        m_benchGeoID   = new PMonUtils::CustomBenchmark(AtlasDetDescr::fNumAtlasRegions      );
    }

    // setup for validation mode
    if ( m_validationOutput) {

      // retrieve the histogram service
      if ( m_thistSvc.retrieve().isSuccess() ) {
        ATH_CHECK( registerPosValTree( "push_position",
                                       "push() particle positions",
                                       m_t_pushPosition) );
        ATH_CHECK( registerPosValTree( "caloEntry_pos",
                                       "CaloEntryLayer positions",
                                       m_t_entryLayerPos[ISF::fAtlasCaloEntry] ) );
        ATH_CHECK( registerPosValTree( "muonEntry_pos",
                                       "MuonEntryLayer positions",
                                       m_t_entryLayerPos[ISF::fAtlasMuonEntry] ) );
        ATH_CHECK( registerPosValTree( "muonExit_pos",
                                       "MuonExitLayer positions",
                                       m_t_entryLayerPos[ISF::fAtlasMuonExit]  ) );
      }

      // error when trying to retrieve the THistSvc
      else {
        // -> turn off validation output
        ATH_MSG_ERROR( m_screenOutputPrefix << "Validation mode turned on but unable to retrieve THistService. Will not write out ROOT histograms/Trees.");
        m_validationOutput = false;
      }

    }

    // initialization was successful
    return StatusCode::SUCCESS;
}


/** framework methods */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::finalize()
{
    ATH_MSG_INFO ( m_screenOutputPrefix << "finalize() successful");
    return StatusCode::SUCCESS;
}


/** Register an array of SimulationSelectors */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::registerSimSelector( SimSelectorToolArray &simSelectorTools, AtlasDetDescr::AtlasRegion geoID)
{
  // (1.) retrieve all SimulationSelector tools in the array
  if ( simSelectorTools.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_screenOutputPrefix <<  "Could not retrieve SimulatorSelector Tool Array. Abort.");
      return StatusCode::FAILURE;
  }

  // reserve memory in STL vector
  m_simSelector[geoID].reserve ( simSelectorTools.size());

  // (2.) loop over SimulationSelector tool array and store
  //      its contents in m_simSelector[]
  SimSelectorToolArray::iterator fSimSelIter    = simSelectorTools.begin();
  SimSelectorToolArray::iterator fSimSelIterEnd = simSelectorTools.end();
  for ( ; fSimSelIter != fSimSelIterEnd; ++fSimSelIter ) {
    ISimulationSelector *curSelector = &**fSimSelIter;
    // initialize the SimulationSelector
    curSelector->initializeSelector();
    // register current SimulationSelector to the m_simSelector vector
    m_simSelector[geoID].push_back( curSelector);
  }

  return StatusCode::SUCCESS;
}


/** setup the ROOT TTrees for THistSvc in validation mode */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::registerPosValTree( const char *treeName,
                                                                   const char *treeDescr,
                                                                   TTree *&tree ) {
  // Create the prefix of histogram names for the THistSvc
  std::string prefix = "/" + m_validationStream + "/";

  tree = new TTree( treeName, treeDescr );
  tree->Branch("x"  , &m_val_x  , "x/F");
  tree->Branch("y"  , &m_val_y  , "y/F");
  tree->Branch("z"  , &m_val_z  , "z/F");
  tree->Branch("p"  , &m_val_p  , "p/F");
  tree->Branch("pdg", &m_val_pdg, "pdg/I");

  // register the Tree to the THistSvc and return it's StatusCode
  return (m_thistSvc->regTree( prefix+treeName, tree) );
}


/** fill the TTree with the given (x,y,z) values */
void ISF::ParticleBrokerDynamicOnReadIn::fillPosValTree( TTree *tree,
                                                         const ISFParticle &p ) {
  // the particle position
  const Amg::Vector3D &pos = p.position();
  // fill the member variables
  m_val_x = pos.x();
  m_val_y = pos.y();
  m_val_z = pos.z();
  m_val_p   = p.momentum().mag();
  m_val_pdg = p.pdgCode();
  // fill the ROOT TTree
  tree->Fill();

  return;
}


/** update all SimulationSelectors in the routing chain with the given particle */
void ISF::ParticleBrokerDynamicOnReadIn::updateAllSelectors(const ISFParticle &particle) {

  // update all simSelectors with the given (new) particle

  // iterators used to loop over all sim Selectors
  SimSelectorSet::iterator  simSelectorIter    = m_simSelectorSet.begin();
  SimSelectorSet::iterator  simSelectorIterEnd = m_simSelectorSet.end();

  for ( ; simSelectorIter != simSelectorIterEnd; ++simSelectorIter)
      (*simSelectorIter)->update( particle);
}


/** go through the chain of SimulationSelectors and let the first one which selects the particle
    decide which simulation the particle will be assigned to */
void ISF::ParticleBrokerDynamicOnReadIn::selectAndStore( ISF::ISFParticle* p)
{
  // consult the routing chain to find the simulator ID corresponding to this
  // particle
  SimSvcID selectedSimID = identifySimID( p);

  // in case a simulator was determined for this particle
  //   -> register this simulator to the particle
  //   -> push particle onto container
  if ( selectedSimID != ISF::fUndefinedSimID) {
    ATH_MSG_VERBOSE( m_screenOutputPrefix << "Assigning " << *p
                     << " to simulator with ID=" << selectedSimID << ".");
    // register the SimulatorID to the particle
    p->setNextSimID( selectedSimID);

    // record the simulation flavor in the particle's extra barcode
    storeSimulationFlavor( p );

    // set particle order
    if (m_orderingToolQuick) m_orderingToolQuick->setOrder( *p);
    
    // push the particle onto the particle container
    m_particles.push( p);

  // no simulator could be found
  //   -> drop particle
  } else {
    AtlasDetDescr::AtlasRegion geoID = p->nextGeoID();
    // different error message for empty simulation Selector chain:
    if ( !m_simSelector[geoID].size()) {
      ATH_MSG_INFO( m_screenOutputPrefix << "No SimulationSelectors registered for GeoID="
                    << AtlasDetDescr::AtlasRegionHelper::getName(geoID) << ". Will not assign this particle to any simulator, dropping it.");
    }
    else {
      ATH_MSG_INFO( m_screenOutputPrefix << "Current particle not selected by any SimulationSelector with GeoID="
                    << AtlasDetDescr::AtlasRegionHelper::getName(geoID) << ". Will not assign this particle to any simulator, dropping it.");
    }
    delete p;
  }
}


/** store the simulation flavor of SimulationSelector that has selected the particle */
void ISF::ParticleBrokerDynamicOnReadIn::storeSimulationFlavor( ISF::ISFParticle* p )
{
  // m_simflavor has been identified in identifySimID()  
  if ( m_barcodeSvc->hasBitCalculator() ) {
    Barcode::ParticleBarcode extrabc = p->getExtraBC();
    m_barcodeSvc->getBitCalculator()->SetSimulator(extrabc,m_simflavor);
    p->setExtraBC( extrabc );
  }
}


/** go through the chain of SimulationSelectors and return the SimulatoID of the first
    SimulationSelector that selects the particle */
ISF::SimSvcID ISF::ParticleBrokerDynamicOnReadIn::identifySimID( const ISF::ISFParticle* p) {
  // implicit assumption that the geoID is set properly at this point (it is a private method)!
  AtlasDetDescr::AtlasRegion geoID = p->nextGeoID();

  // iterators used to loop over all simulation Selectors
  SimSelectorArray::iterator selectorIt     = m_simSelector[geoID].begin();
  SimSelectorArray::iterator selectorItEnd  = m_simSelector[geoID].end();

  // will store whether the particle was selected by a simulation filter
  bool selected = false;

  // block defines scope for Benchmarks
  //  -> benchmarks will be stared/stopped automatically via the CustomBenchmarkGuard
  //     constructor and destructor, respectively
  {
    // prepare the pdgCode for the benchmark
    unsigned int pdgCode = ISFBenchmarkHelper::getBenchReadyPdgCode( *p);

    // setup sim svc benchmarks
    PMonUtils::CustomBenchmarkGuard benchPDG  ( m_benchPDGCode, pdgCode );
    PMonUtils::CustomBenchmarkGuard benchGeoID( m_benchGeoID  , geoID   );

    // go through the chain of selectors and find the first
    // selector which selects out the particle
    for ( ; !selected && (selectorIt != selectorItEnd) ; ++selectorIt) {
      // check if the current selector selects it
      selected = (*selectorIt)->selfSelect( *p);
    }

  } // stop benchmarks

  // determine the simulator ID
  ISF::SimSvcID simID = selected ? (*--selectorIt)->simSvcID() : ISF::SimSvcID(ISF::fUndefinedSimID);

  // reset simulation flavor -> stored in setSimulationFlavor();
  m_simflavor = ISF::UndefinedSim;

  if (selected) {
    std::string simulatorType("none");
    ServiceHandle<ISimulationSvc>* hsimulator = (*selectorIt)->simulator();
    if ( hsimulator!=0 ) {
      //if ( simulator.retrieve().isSuccess() ) { 
      //if ( (*(*--selectorIt)->simulator()) != 0 ) {
      simulatorType = (*hsimulator).type(); 
	//}
    } 
    std::transform(simulatorType.begin(), simulatorType.end(), simulatorType.begin(), ::tolower);

    if      (simulatorType.find("fatras")!=std::string::npos)   { m_simflavor = ISF::FatrasSim; }
    else if (simulatorType.find("g4")!=std::string::npos)       { m_simflavor = ISF::Geant4Sim; }
    else if (simulatorType.find("geant")!=std::string::npos)    { m_simflavor = ISF::Geant4Sim; }
    else if (simulatorType.find("full")!=std::string::npos)     { m_simflavor = ISF::Geant4Sim; }
    else if (simulatorType.find("fastcalo")!=std::string::npos) { m_simflavor = ISF::FastCaloSim; }
    else    { m_simflavor = ISF::UndefinedSim; } // = 0

    ATH_MSG_DEBUG( "simulation flavor : " << m_simflavor << " " << simulatorType );

  }

  return simID;
}


/** Initialize the stackSvc and the truthSvc */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::initializeEvent()
{ 

  ATH_MSG_DEBUG( m_screenOutputPrefix << "Initializing particle stack");

  // fill set of simulation selectors once per job
  //  -> the std::set allows us to address each selector only once
  //     (the same selector might appear in multiple geoIDs)
  if ( !m_simSelectorSet.size() )  {
    for ( int curGeoID = AtlasDetDescr::fFirstAtlasRegion; curGeoID < AtlasDetDescr::fNumAtlasRegions; ++curGeoID) { 
      // fill the set with the selectors from the current geoID
      m_simSelectorSet.insert( m_simSelector[curGeoID].begin(), m_simSelector[curGeoID].end() );
    }

    ATH_MSG_DEBUG("Number of unique SimulationSelctors registered: "
                  << m_simSelectorSet.size() );
  }

  // call beginEvent() for all selectors registerd to the ParticleBroker:
  //   iterators used to loop over all sim Selectors
  SimSelectorSet::iterator  simSelectorIter    = m_simSelectorSet.begin();
  SimSelectorSet::iterator  simSelectorIterEnd = m_simSelectorSet.end();
  for ( ; simSelectorIter != simSelectorIterEnd; ++simSelectorIter)
      (*simSelectorIter)->beginEvent(); // call beginEvent() on current selector

  // read particles from EvGen
  ISFParticleContainer initContainer;
  if ( m_particleStackFiller->fillStack( initContainer).isFailure() ){
     ATH_MSG_FATAL (  m_screenOutputPrefix << "Could not fill initial particle container. Abort." );
     return StatusCode::FAILURE;
  } else
   ATH_MSG_VERBOSE ( m_screenOutputPrefix << "Initial particle container filled, initial size: " << initContainer.size() );

  // update the routing chain selectors with the particles in the initial stack
  ISFParticleContainer::iterator particleIter    = initContainer.begin();
  ISFParticleContainer::iterator particleIterEnd = initContainer.end();
  for ( ; particleIter != particleIterEnd; ++particleIter) {

    // identify the geoID of the particle
    m_geoIDSvcQuick->identifyAndRegNextGeoID(**particleIter);
    // the geoID at this point better makes sense :)
    assertAtlasRegion( (**particleIter).nextGeoID() ); 

    // update all registered selectors (in all geoIDs) with this particle
    updateAllSelectors( **particleIter);

    // inform the entry layer tool about this particle
    ISF::EntryLayer layer = m_entryLayerToolQuick->registerParticle( **particleIter);

    // if validation mode: fill the corresponding entry layer ROOT tree
    if ( m_validationOutput && validEntryLayer(layer) ) {
      fillPosValTree( m_t_entryLayerPos[layer], **particleIter);
    }

  }

  // move the particles from the initial stack to the different geoID stacks
  particleIter    = initContainer.begin();
  particleIterEnd = initContainer.end();
  int iparticle=initContainer.size();
  for ( ; particleIter != particleIterEnd; ++particleIter,iparticle--) {
    //       - if a Selector selects a particle -> it is pushed onto the active stack
    //       - if it is not selected -> pushed onto the hold stack
    selectAndStore( *particleIter);
    if (!m_orderingToolQuick) {
      (*particleIter)->setOrder(iparticle);
    }
  }

  // empty initial stack
  //  (don't delete the pointers, since they are now used in the local
  //   particle container: m_particles)
  initContainer.clear();

  return StatusCode::SUCCESS;
}


/** Finalize the event in the stackSvc */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::finalizeEvent() {

  // go throught all SimSelectors and call the endEvent() on them
  SimSelectorSet::iterator fSimSelIter    = m_simSelectorSet.begin();
  SimSelectorSet::iterator fSimSelIterEnd = m_simSelectorSet.end();
  for ( ; fSimSelIter != fSimSelIterEnd; ++fSimSelIter )
    (*fSimSelIter)->endEvent();

  for (size_t i=0;i<m_extraParticles.size();i++) {delete m_extraParticles[i];}
  m_extraParticles.clear();

  return StatusCode::SUCCESS;
}


/** add a new particle to the stack and link it to its parent */
void ISF::ParticleBrokerDynamicOnReadIn::push( ISFParticle *particle, const ISFParticle* /*parent*/) {
  // this call does not make much sense with no given particle
  assert(particle);

  registerParticle(particle);

  // get the particle's next geoID
  AtlasDetDescr::AtlasRegion geoID = particle->nextGeoID();

  // only process particles with well defined geoID
  if ( !validAtlasRegion( geoID) ) {
    ATH_MSG_ERROR( m_screenOutputPrefix << "Trying to push particle onto the stack with unknown geoID=" << geoID
                   << ". Dropping this particle.");
    delete particle;
    return;
  }

  // (*) let the Selectors select the particle
  //       - if a Selector selects a particle -> it is pushed onto the active stack
  //       - if it is not selected -> particle is dropped (deleted)
  selectAndStore( particle);
}

/** Get vectors of ISF particles from the broker */
const ISF::ConstISFParticleVector& ISF::ParticleBrokerDynamicOnReadIn::popVector(size_t maxVectorSize)
{
  // delete particles from previous return vector and empty the vector
  ISF::ConstISFParticleVector::const_iterator partIt  = m_popParticles.begin();
  ISF::ConstISFParticleVector::const_iterator partEnd = m_popParticles.end();
  for ( ; partIt != partEnd; ++partIt) {
    ISF::ISFParticle *curPart = const_cast<ISF::ISFParticle*>(*partIt);
    delete curPart;
  }
  m_popParticles.clear();

  // if there are particles in the queue
  if ( m_particles.size()) {

    ParticleOrder returnOrder = m_particles.top()->getOrder();
    
    // loop as long as we have particles in the m_particles queue
    do {
      // get the next particle from the ordered queue
      const ISFParticle *curParticle = m_particles.top();
      ParticleOrder      curOrder    = curParticle->getOrder();
      
      // if this particle has a different order or the maximum size of the return vector is reached
      //   -> don't add any more particles to the m_popParticles std::vector
      if (  m_orderingToolQuick && ((curOrder != returnOrder) || (m_popParticles.size()>=maxVectorSize) ) ) break;
      
      // add this particle to the, later returned, m_popParticles std::vector
      m_popParticles.push_back( curParticle);
      // remove this particle from the ordered queue
      m_particles.pop();
    } while ( m_particles.size() ) ;
    
  }
  // return the popParticles vector
  return m_popParticles;
}


/* Dump the stack to the screen */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::dump() const
{
  ATH_MSG_INFO( m_screenOutputPrefix << "==== ISF Particle Stack Dump ====");
  ATH_MSG_INFO( m_screenOutputPrefix << " 'active' particles -> ready to be simulated");
  ATH_MSG_INFO( m_screenOutputPrefix << " 'onHold' particles -> no sim Selector decision yet, waiting in routing chain");
  ATH_MSG_INFO( m_screenOutputPrefix << " --- ");
  ATH_MSG_INFO( m_screenOutputPrefix << " Number of 'active' particles: " <<  m_particles.size());
  for ( int geoID = AtlasDetDescr::fFirstAtlasRegion; geoID < AtlasDetDescr::fNumAtlasRegions; ++geoID) { 
    ATH_MSG_INFO( m_screenOutputPrefix << " --- SimGeoID=" << geoID
                  << " (" << AtlasDetDescr::AtlasRegionHelper::getName(geoID) << ") ---");
    ATH_MSG_INFO( m_screenOutputPrefix << "   Routing Chain has length "
                  << m_simSelector[geoID].size() );
  }
  ATH_MSG_INFO( m_screenOutputPrefix << "=================================");

  return StatusCode::SUCCESS;
}


/** Query the interfaces. */
StatusCode ISF::ParticleBrokerDynamicOnReadIn::queryInterface(const InterfaceID& riid, void** ppvInterface) {

 if ( IID_IParticleBroker == riid ) 
    *ppvInterface = (IParticleBroker*)this;
 else  {
   // Interface is not directly available: try out a base class
   return Service::queryInterface(riid, ppvInterface);
 }
 addRef();
 return StatusCode::SUCCESS;
}


/** Register the particle */
void ISF::ParticleBrokerDynamicOnReadIn::registerParticle( ISFParticle* particle, ISF::EntryLayer layerInput, bool takeOwnership )
{
  
  // get the particle's next geoID
  AtlasDetDescr::AtlasRegion geoID = particle->nextGeoID();

  // if GeoID not set (e.g. ISF::fUndefinedGeoID) or a flag is set to always use the GeoIDSvc
  // -> let the geoIDSvc find the next geoID
  if ( !validAtlasRegion(geoID) || m_forceGeoIDSvc) {
    geoID = m_geoIDSvcQuick->identifyAndRegNextGeoID(*particle);
  }
  // inform the entry layer tool about this particle
  ISF::EntryLayer layer = m_entryLayerToolQuick->registerParticle( *particle, layerInput);

  // ---> if validation mode: fill the corresponding entry layer ROOT tree
  if ( m_validationOutput) {
    // fill the push() position TTree
    fillPosValTree( m_t_pushPosition, *particle);
    // in case particle was added to an entry layer, add it to the corresponding TTree
    if ( validEntryLayer(layer) )
      fillPosValTree( m_t_entryLayerPos[layer], *particle);
  }
  // <--- end validation output

  // validation mode: check whether the particle position corresponds to the GeoID given
  // by the particle itself
  if ( m_validateGeoID) {
     AtlasDetDescr::AtlasRegion identifiedGeoID = m_geoIDSvcQuick->identifyNextGeoID(*particle);
    if ( (geoID!=AtlasDetDescr::fUndefinedAtlasRegion) && (geoID!=identifiedGeoID) ) {
      ATH_MSG_WARNING("Validating GeoID: GeoIDSvc resolves a particle's position to a different GeoID than stored in the particle:");
      ATH_MSG_WARNING("     assigned=" << geoID << "  GeoIDSvc=" << identifiedGeoID);
      ATH_MSG_WARNING("     Particle: " << *particle);
    }
  }

  if (takeOwnership) m_extraParticles.push_back(particle);

  return;
}
