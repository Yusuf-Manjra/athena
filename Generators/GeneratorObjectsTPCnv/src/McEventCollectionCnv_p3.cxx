///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McEventCollectionCnv_p3.cxx 
// Implementation file for class McEventCollectionCnv_p3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <utility>
#include <cmath>

// GeneratorObjectsTPCnv includes
#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p3.h"
#include "HepMcDataPool.h"

namespace {
// helper method to compute the number of particles and vertices in a
// whole McEventCollection
std::pair<unsigned int,unsigned int> 
nbrParticlesAndVertices( const McEventCollection* mcEvents ) {
  unsigned int nParts = 0;
  unsigned int nVerts = 0;
  const McEventCollection::const_iterator itrEnd = mcEvents->end();
  for ( McEventCollection::const_iterator itr = mcEvents->begin();
	itr != itrEnd;
	++itr ) {
    nParts += (*itr)->particles_size();
    nVerts += (*itr)->vertices_size();
  }

  return std::make_pair( nParts, nVerts );
}
}

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

McEventCollectionCnv_p3::McEventCollectionCnv_p3() :
  Base_t( ),
  m_pool( new HepMC::DataPool )
{}

McEventCollectionCnv_p3::McEventCollectionCnv_p3( const McEventCollectionCnv_p3& rhs ) :
  Base_t( rhs ),
  m_pool( new HepMC::DataPool )
{}

McEventCollectionCnv_p3& 
McEventCollectionCnv_p3::operator=( const McEventCollectionCnv_p3& rhs )
{
  if ( this != &rhs ) {
    Base_t::operator=( rhs );
    m_pool = new HepMC::DataPool;
  } 
  return *this;
}

// Destructor
///////////////

McEventCollectionCnv_p3::~McEventCollectionCnv_p3()
{
  delete m_pool; m_pool = 0;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void McEventCollectionCnv_p3::persToTrans( const McEventCollection_p3* persObj,
					   McEventCollection* transObj, 
					   MsgStream& msg ) 
{
  msg << MSG::DEBUG << "Loading McEventCollection from persistent state..."
      << endreq;

  // elements are managed by DataPool
  transObj->clear(SG::VIEW_ELEMENTS);
  
  static bool first = true;
  if ( first ) {
    first = false;
    m_pool->evt.reserve (   20 );
    m_pool->vtx.reserve ( 1000 );
    m_pool->part.reserve( 1000 );
  }

  {
    const unsigned int nVertices = persObj->m_genVertices.size();
    if ( m_pool->vtx.capacity() - m_pool->vtx.allocated() < nVertices ) {
      m_pool->vtx.reserve( m_pool->vtx.allocated() + nVertices );
    }
    const unsigned int nParts = persObj->m_genParticles.size();
    if ( m_pool->part.capacity() - m_pool->part.allocated() < nParts ) {
      m_pool->part.reserve( m_pool->part.allocated() + nParts );
    }
  }

  const unsigned int nEvts = persObj->m_genEvents.size();
  if ( m_pool->evt.capacity() - m_pool->evt.allocated() < nEvts ) {
    m_pool->evt.reserve( m_pool->evt.allocated() + nEvts );
  }
  transObj->reserve( nEvts );
  for ( std::vector<GenEvent_p3>::const_iterator 
	  itr = persObj->m_genEvents.begin(),
	  itrEnd = persObj->m_genEvents.end();
	itr != itrEnd;
	++itr ) {
    const GenEvent_p3& persEvt = *itr;

    HepMC::GenEvent * genEvt        = m_pool->evt.nextElementPtr();
    genEvt->m_signal_process_id     = persEvt.m_signalProcessId;
    genEvt->m_event_number          = persEvt.m_eventNbr;
    genEvt->m_event_scale           = persEvt.m_eventScale;
    genEvt->m_alphaQCD              = persEvt.m_alphaQCD;
    genEvt->m_alphaQED              = persEvt.m_alphaQED;
    genEvt->m_signal_process_vertex = 0;
    genEvt->m_weights               = persEvt.m_weights;
    genEvt->m_random_states         = persEvt.m_randomStates;
    genEvt->m_vertex_barcodes.clear();
    genEvt->m_particle_barcodes.clear();
    genEvt->m_pdf_info = 0;         //> not available at that time...

    transObj->push_back( genEvt );
    
    // create a temporary map associating the barcode of an end-vtx to its 
    // particle.
    // As not all particles are stable (d'oh!) we take 50% of the number of
    // particles as an initial size of the hash-map (to prevent re-hash)
    ParticlesMap_t partToEndVtx( (persEvt.m_particlesEnd-
				  persEvt.m_particlesBegin)/2 );
    
    // create the vertices
    const unsigned int endVtx = persEvt.m_verticesEnd;
    for ( unsigned int iVtx= persEvt.m_verticesBegin; iVtx != endVtx; ++iVtx ) {
      genEvt->add_vertex( createGenVertex( *persObj, 
					   persObj->m_genVertices[iVtx], 
					   partToEndVtx ) );
    } //> end loop over vertices
    
    // set the signal process vertex
    const int sigProcVtx = persEvt.m_signalProcessVtx;
    if ( sigProcVtx != 0 ) {
      genEvt->set_signal_process_vertex( genEvt->barcode_to_vertex( sigProcVtx ) );
    }

    // connect particles to their end vertices
    for ( ParticlesMap_t::iterator 
	    p = partToEndVtx.begin(),
	    endItr = partToEndVtx.end();
	  p != endItr;
	  ++p ) {
      HepMC::GenVertex* decayVtx = genEvt->barcode_to_vertex( p->second );
      if ( decayVtx ) {
	decayVtx->add_particle_in( p->first );
      } else {
	msg << MSG::ERROR
	    << "GenParticle points to null end vertex !!"
	    << endreq;
      }
    }
  } //> end loop over m_genEvents

  msg << MSG::DEBUG << "Loaded McEventCollection from persistent state [OK]"
      << endreq;

  return;
}

void McEventCollectionCnv_p3::transToPers( const McEventCollection* /*transObj*/, 
					   McEventCollection_p3* /*persObj*/, 
					   MsgStream& msg ) 
{
  msg << MSG::ERROR
      << "This transient-to-persistent converter method has been RETIRED !!"
      << endreq
      << "You are not supposed to end-up here ! Go away !"
      << endreq;

  throw std::runtime_error( "Retired McEventCollectionCnv_p3::transToPers() !!" );
  return;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

HepMC::GenVertex* 
McEventCollectionCnv_p3::createGenVertex( const McEventCollection_p3& persEvt,
					  const GenVertex_p3& persVtx,
					  ParticlesMap_t& partToEndVtx ) const
{
  HepMC::GenVertex * vtx = m_pool->vtx.nextElementPtr();
  vtx->m_position.setX( persVtx.m_x );
  vtx->m_position.setY( persVtx.m_y );
  vtx->m_position.setZ( persVtx.m_z );
  vtx->m_position.setT( persVtx.m_t );
  vtx->m_particles_in.clear();
  vtx->m_particles_out.clear();
  vtx->m_id      = persVtx.m_id;
  vtx->m_weights.m_weights.reserve( persVtx.m_weights.size() );
  vtx->m_weights.m_weights.assign ( persVtx.m_weights.begin(),
				    persVtx.m_weights.end() );
  vtx->m_event   = 0;
  vtx->m_barcode = persVtx.m_barcode;
  
  // handle the in-going (orphans) particles
  const unsigned int nPartsIn = persVtx.m_particlesIn.size();
  for ( unsigned int i = 0; i != nPartsIn; ++i ) {
     createGenParticle( persEvt.m_genParticles[persVtx.m_particlesIn[i]], 
			partToEndVtx );
  }
  
  // now handle the out-going particles
  const unsigned int nPartsOut = persVtx.m_particlesOut.size();
  for ( unsigned int i = 0; i != nPartsOut; ++i ) {
     vtx->add_particle_out( createGenParticle( persEvt.m_genParticles[persVtx.m_particlesOut[i]],
					       partToEndVtx ) );
  }

  return vtx;
}

HepMC::GenParticle* 
McEventCollectionCnv_p3::createGenParticle( const GenParticle_p3& persPart,
					    ParticlesMap_t& partToEndVtx ) const
{
  using std::abs;
  using std::sqrt;
  using std::pow;

  HepMC::GenParticle* p    = m_pool->part.nextElementPtr();
  p->m_pdg_id              = persPart.m_pdgId;
  p->m_status              = persPart.m_status;
  p->m_polarization.m_theta= static_cast<double>(persPart.m_thetaPolarization);
  p->m_polarization.m_phi  = static_cast<double>(persPart.m_phiPolarization  );
  p->m_production_vertex   = 0;
  p->m_end_vertex          = 0;
  p->m_barcode             = persPart.m_barcode;

  // Note: do the E calculation in extended (long double) precision.
  // That happens implicitly on x86 with optimization on; saying it
  // explicitly ensures that we get the same results with and without
  // optimization.  (If this is a performance issue for platforms
  // other than x86, one could change to double for those platforms.)
  if ( 0 == persPart.m_recoMethod ) {
    p->m_momentum.setPx( persPart.m_px );
    p->m_momentum.setPy( persPart.m_py );
    p->m_momentum.setPz( persPart.m_pz );
    double temp_e = sqrt( (long double)(persPart.m_px)*persPart.m_px + 
                          (long double)(persPart.m_py)*persPart.m_py + 
                          (long double)(persPart.m_pz)*persPart.m_pz + 
                          (long double)(persPart.m_m) *persPart.m_m );
    p->m_momentum.setE( temp_e );
  } else {
    const int signM2 = ( persPart.m_m >= 0. ? 1 : -1 );
    const double persPart_ene =
      sqrt( abs((long double)(persPart.m_px)*persPart.m_px +
                (long double)(persPart.m_py)*persPart.m_py +
                (long double)(persPart.m_pz)*persPart.m_pz +
        signM2* (long double)(persPart.m_m)* persPart.m_m));
    const int signEne = ( persPart.m_recoMethod == 1 ? 1 : -1 );
    p->m_momentum.set( persPart.m_px,
		       persPart.m_py,
		       persPart.m_pz,
		       signEne * persPart_ene );
  }

  // setup flow
  const unsigned int nFlow = persPart.m_flow.size();
  for ( unsigned int iFlow= 0; iFlow != nFlow; ++iFlow ) {
    p->m_flow.set_icode( persPart.m_flow[iFlow].first, 
			 persPart.m_flow[iFlow].second );
  }

  if ( persPart.m_endVtx != 0 ) {
    partToEndVtx[p] = persPart.m_endVtx;
  }

  return p;
}

