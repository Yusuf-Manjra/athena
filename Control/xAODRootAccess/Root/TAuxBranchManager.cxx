// Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

// ROOT include(s):
#include <TBranch.h>

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"

// Local include(s):
#include "xAODRootAccess/tools/TAuxBranchManager.h"
#include "xAODRootAccess/tools/THolder.h"

namespace xAOD {

   TAuxBranchManager::TAuxBranchManager( auxid_t auxid,
                                         ::TBranch* br, THolder* holder )
      : m_branch( br ), m_holder( holder ), m_entry( -1 ),
        m_isSet( kTRUE ), m_auxId( auxid ), m_vector( nullptr ) {

   }

   TAuxBranchManager::TAuxBranchManager( const TAuxBranchManager& parent )
      : TVirtualManager(), m_branch( parent.m_branch ), m_holder( nullptr ),
        m_entry( parent.m_entry ), m_isSet( parent.m_isSet ),
        m_auxId( parent.m_auxId ), m_vector( nullptr ) {

      if( parent.m_holder ) {
         m_holder = new THolder( *parent.m_holder );
      }
      if( parent.m_vector ) {
         m_vector =
            SG::AuxTypeRegistry::instance().makeVector( m_auxId,
                                                        ( size_t ) 0,
                                                        ( size_t ) 0 ).release();
      }
   }

   TAuxBranchManager::~TAuxBranchManager() {

      if( m_holder ) {
         delete m_holder;
      }
      if( m_vector ) {
         delete m_vector;
      }
   }

   TAuxBranchManager&
   TAuxBranchManager::operator=( const TAuxBranchManager& rhs ) {

      // Check if anything needs to be done:
      if( this == &rhs ) {
         return *this;
      }

      m_branch = rhs.m_branch;
      if( m_holder ) delete m_holder;
      if( rhs.m_holder ) {
         m_holder = new THolder( *rhs.m_holder );
      } else {
         m_holder = 0;
      }
      m_entry = rhs.m_entry;
      m_isSet = rhs.m_isSet;
      m_auxId = rhs.m_auxId;
      if( m_vector ) delete m_vector;
      if( rhs.m_vector ) {
         m_vector =
            SG::AuxTypeRegistry::instance().makeVector( m_auxId,
                                                        ( size_t ) 0,
                                                        ( size_t ) 0 ).release();
      } else {
         m_vector = 0;
      }

      // Return this same object:
      return *this;
   }

   ::TBranch* TAuxBranchManager::branch() {

      return m_branch;
   }

   ::TBranch** TAuxBranchManager::branchPtr() {

      return &m_branch;
   }

   const THolder* TAuxBranchManager::holder() const {

      return m_holder;
   }

   THolder* TAuxBranchManager::holder() {

      return m_holder;
   }

   ::Int_t TAuxBranchManager::getEntry( ::Long64_t entry, ::Int_t getall ) {

      // Check if anything needs to be done:
      if( entry == m_entry ) return 0;

      // Remember which entry we're loading at the moment:
      m_entry = entry;

      // Load the entry:
      return m_branch->GetEntry( entry, getall );
   }

   const void* TAuxBranchManager::object() const {

      return std::as_const(*m_holder).get();
   }

   void* TAuxBranchManager::object() {

      return m_holder->get();
   }

   void TAuxBranchManager::setObject( void* obj ) {

      m_holder->setOwner( kFALSE );
      m_holder->set( obj );
      m_isSet = kTRUE;
      return;
   }

   ::Bool_t TAuxBranchManager::create() {

      // If we already have it set, let's stop here:
      if( m_isSet ) return kTRUE;

      // Otherwise let's create a default object:
      m_isSet = kTRUE;
      if( ! m_vector ) {
         m_vector =
            SG::AuxTypeRegistry::instance().makeVector( m_auxId,
                                                        ( size_t ) 0,
                                                        ( size_t ) 0 ).release();
      }
      // ...and use it to fill the current event:
      m_holder->setOwner( kFALSE );
      m_holder->set( m_vector->toVector() );

      // We are now "set":
      return kTRUE;
   }

   ::Bool_t TAuxBranchManager::isSet() const {
      return m_isSet;
   }

   void TAuxBranchManager::reset() {

      m_isSet = kFALSE;
      return;
   }

} // namespace xAOD
