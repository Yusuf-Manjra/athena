///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthReentrantAlgorithm.cxx 
// Implementation file for class AthReentrantAlgorithm
// Author: C. Leggett
/////////////////////////////////////////////////////////////////// 

// AthenaBaseComps includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthAlgorithmDHUpdate.h"

// Framework includes
#include "GaudiKernel/Property.h"
#ifndef REENTRANT_GAUDI
#include "GaudiKernel/ThreadLocalContext.h"
#endif


#include "./VHKASupport.h"
/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthReentrantAlgorithm::AthReentrantAlgorithm( const std::string& name, 
			    ISvcLocator* pSvcLocator,
			    const std::string& version ) : 
  ::ReEntAlgorithm   ( name, pSvcLocator, version ),
  m_evtStore    ( "StoreGateSvc/StoreGateSvc",  name ),
  m_detStore    ( "StoreGateSvc/DetectorStore", name ),
  m_varHandleArraysDeclared (false)
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
  setUpMessaging();
  auto props = getProperties();
  for( Property* prop : props ) {
    if( prop->name() == "OutputLevel" ) {
      // prop->declareUpdateHandler
      //   (&AthReentrantAlgorithm::msg_update_handler, this);
    } else if (prop->name() == "ExtraOutputs" || prop->name() == "ExtraInputs") {
      prop->declareUpdateHandler
        (&AthReentrantAlgorithm::extraDeps_update_handler, this);
    }
  }

  declareProperty( "EvtStore",
                   m_evtStore = StoreGateSvc_t ("StoreGateSvc", name),
                   "Handle to a StoreGateSvc instance: it will be used to "
                   "retrieve data during the course of the job" );

  declareProperty( "DetStore",
                   m_detStore = StoreGateSvc_t ("StoreGateSvc/DetectorStore", name),
                   "Handle to a StoreGateSvc/DetectorStore instance: it will be used to "
                   "retrieve data during the course of the job" );

  // Set up to run AthAlgorithmDHUpdate in sysInitialize before
  // merging depedency lists.  This extends the output dependency
  // list with any symlinks implied by inheritance relations.
  m_updateDataHandles =
    std::make_unique<AthenaBaseComps::AthAlgorithmDHUpdate>
    (m_extendedExtraObjects,
     std::move (m_updateDataHandles));
}

// Destructor
///////////////
AthReentrantAlgorithm::~AthReentrantAlgorithm()
{ 
  ATH_MSG_DEBUG ("Calling destructor");
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void 
AthReentrantAlgorithm::msg_update_handler( Property& outputLevel ) 
{
   // We can't just rely on the return value of msgLevel() here. Since it's
   // not well defined whether the base class gets updated with the new
   // output level first, or this class. So by default just use the property
   // itself. The fallback is only there in case Gaudi changes its property
   // type at one point, to be able to fall back on something.
   IntegerProperty* iprop = dynamic_cast< IntegerProperty* >( &outputLevel );
   if( iprop ) {
     msgStream().setLevel( static_cast<MSG::Level> (iprop->value()) );
   } else {
     msgStream().setLevel( msgLevel() );
   }
}

/**
 * @brief Add StoreName to extra input/output deps as needed
 *
 * use the logic of the VarHandleKey to parse the DataObjID keys
 * supplied via the ExtraInputs and ExtraOuputs Properties to add
 * the StoreName if it's not explicitly given
 */
void 
AthReentrantAlgorithm::extraDeps_update_handler( Property& ExtraDeps ) 
{  
  DataObjIDColl newColl;
  Gaudi::Property<DataObjIDColl> *prop = dynamic_cast<Gaudi::Property<DataObjIDColl>*> (&ExtraDeps);
  if ( prop ) {
    for (auto id : prop->value()) {
      SG::VarHandleKey vhk(id.clid(), id.key(), Gaudi::DataHandle::Reader);
      id.updateKey( vhk.objKey() );
      newColl.emplace( id );
    }
    if (newColl.size() != 0) prop->setValue( newColl );
  } else {
    ATH_MSG_ERROR("unable to dcast ExtraInput/Output Property");
  }
}

#ifndef REENTRANT_GAUDI
/**
 * @brief Standard Gaudi execute method.
 *
 * Find the event context and call @c execute_r.
 */
StatusCode AthReentrantAlgorithm::execute()
{
  return execute_r (Gaudi::Hive::currentContext());
}
#endif


/** Specify if the algorithm is clonable
 */
bool AthReentrantAlgorithm::isClonable() const
{
  // Reentrant algorithms are clonable.
  return true;
}

/** Cardinality (Maximum number of clones that can exist)
 *  special value 0 means that algorithm is reentrant
 *
 * Override this to return 0 for reentrant algorithms.
 */
unsigned int AthReentrantAlgorithm::cardinality() const
{
  return 0;
}


/**
 * @brief Perform system initialization for an algorithm.
 *
 * We override this to declare all the elements of handle key arrays
 * at the end of initialization.
 * See comments on updateVHKA.
 */
StatusCode AthReentrantAlgorithm::sysInitialize()
{
  ATH_CHECK( ReEntAlgorithm::sysInitialize() );

  for ( SG::VarHandleKeyArray* a: m_vhka ) {
    a->declare( this );
  }
  m_varHandleArraysDeclared = true;

  return StatusCode::SUCCESS;
}


/**
 * @brief Handle START transition.
 *
 * We override this in order to make sure that conditions handle keys
 * can cache a pointer to the conditions container.
 */
StatusCode AthReentrantAlgorithm::sysStart()
{
  ATH_CHECK( ReEntAlgorithm::sysStart() );

  // Call start() on all input handles.
  // This allows CondHandleKeys to cache pointers to their conditions containers.
  // (CondInputLoader makes the containers that it creates during start(),
  // so initialize() is too early for this.)
  for (Gaudi::DataHandle* h : inputHandles()) {
    if (h->isCondition()) {
      if (SG::VarHandleKey* k = dynamic_cast<SG::VarHandleKey*> (h)) {
        ATH_CHECK( k->start() );
      }
    }
  }
  
  return StatusCode::SUCCESS;
}


void AthReentrantAlgorithm::renounceArray( SG::VarHandleKeyArray& vh ) {
  vh.renounce();
}

/**
 * @brief Return this algorithm's input handles.
 *
 * We override this to include handle instances from key arrays
 * if they have not yet been declared.
 * See comments on updateVHKA.
 */
std::vector<Gaudi::DataHandle*> AthReentrantAlgorithm::inputHandles() const
{
  std::vector<Gaudi::DataHandle*> v = ReEntAlgorithm::inputHandles();
  if (!m_varHandleArraysDeclared) {
    VHKASupport::insertInput( m_vhka, v );
  }  
  return v;
}


/**
 * @brief Return this algorithm's output handles.
 *
 * We override this to include handle instances from key arrays
 * if they have not yet been declared.
 * See comments on updateVHKA.
 */
std::vector<Gaudi::DataHandle*> AthReentrantAlgorithm::outputHandles() const
{
  std::vector<Gaudi::DataHandle*> v = ReEntAlgorithm::outputHandles();
  if (!m_varHandleArraysDeclared) {
    VHKASupport::insertOutput( m_vhka, v );
  }  

  return v;
}


/**
 * @brief Return the list of extra output dependencies.
 *
 * This list is extended to include symlinks implied by inheritance
 * relations.
 */
const DataObjIDColl& AthReentrantAlgorithm::extraOutputDeps() const
{
  // If we didn't find any symlinks to add, just return the collection
  // from the base class.  Otherwise, return the extended collection.
  if (!m_extendedExtraObjects.empty()) {
    return m_extendedExtraObjects;
  }
  return Algorithm::extraOutputDeps();
}
