// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventFormatMetaDataTool.cxx 651874 2015-03-05 14:16:19Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "EventFormatMetaDataTool.h"

namespace xAODMaker {

   /// Name of the incident sent around for metadata writing
   //static const char* METADATASTOP = "MetaDataStop";

   EventFormatMetaDataTool::EventFormatMetaDataTool( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_inputMetaStore( "StoreGateSvc/InputMetaDataStore", name ),
        m_outputMetaStore( "StoreGateSvc/MetaDataStore", name ),
        m_format() {

      declareProperty( "InputMetaStore", m_inputMetaStore );
      declareProperty( "OutputMetaStore", m_outputMetaStore );

      declareProperty( "InputKey", m_inputKey = "EventFormat" );
      declareProperty( "OutputKey", m_outputKey = "EventFormat" );

      declareInterface< ::IMetaDataTool >( this );
   }

   StatusCode EventFormatMetaDataTool::initialize() {

      // Greet the user:
      ATH_MSG_DEBUG( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "  InputMetaStore  = " << m_inputMetaStore );
      ATH_MSG_DEBUG( "  OutputMetaStore = " << m_outputMetaStore );
      ATH_MSG_DEBUG( "  InputKey  = " << m_inputKey );
      ATH_MSG_DEBUG( "  OutputKey = " << m_outputKey );

      // Connect to the metadata stores:
      ATH_CHECK( m_inputMetaStore.retrieve() );
      ATH_CHECK( m_outputMetaStore.retrieve() );

      // Reset the internal object if it exists:
      m_format.reset();

      // Return gracefully:
      return StatusCode::SUCCESS;
   }


   StatusCode EventFormatMetaDataTool::beginInputFile()
   {
      StatusCode sc = collectMetaData();
      if( sc.isFailure() ) {
         REPORT_ERROR( sc )
            << "Failed to collect metadata from the input file";
         throw std::runtime_error( "Failed to collect trigger configuration "
                                   "metadata from the input" );
      } 
      return StatusCode::SUCCESS;
   }


   StatusCode EventFormatMetaDataTool::endInputFile()
   {
      return StatusCode::SUCCESS;
   }


   StatusCode EventFormatMetaDataTool::metaDataStop()
   {
      StatusCode sc = writeMetaData();
      if( sc.isFailure() ) {
         REPORT_ERROR( sc )
            << "Failed to write metadata to the output store";
         throw std::runtime_error( "Failed write xAOD::EventFormat "
                                   "to the output" );
      }
      return StatusCode::SUCCESS;
   }

   StatusCode EventFormatMetaDataTool::collectMetaData() {

      // If the input file doesn't have any event format metadata,
      // then finish right away:
      if( ! m_inputMetaStore->contains< xAOD::EventFormat >( m_inputKey ) ) {
         return StatusCode::SUCCESS;
      }

      // Retrieve the input container:
      const xAOD::EventFormat* input = 0;
      ATH_CHECK( m_inputMetaStore->retrieve( input, m_inputKey ) );

      // Create an output container if it doesn't exist yet:
      if( ! m_format.get() ) {
         ATH_MSG_DEBUG( "Creating output container" );
         m_format.reset( new xAOD::EventFormat() );
      }

      // Merge the new object into the output one:
      for( auto itr = input->begin(); itr != input->end(); ++itr ) {
         if( ! m_format->exists( itr->second.hash() ) ) {
            m_format->add( itr->second );
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EventFormatMetaDataTool::writeMetaData() {

      // The output may already have such an object, coming from some
      // other source. Like the event format service.
      if( m_outputMetaStore->contains< xAOD::EventFormat >( m_outputKey ) ) {
         ATH_MSG_DEBUG( "xAOD::EventFormat object already in the output" );
         return StatusCode::SUCCESS;
      }

      // Record the event format metadata, if any was found in the
      // processed input files.
      if( m_format.get() ) {
         ATH_MSG_DEBUG( "Recording event format metadata" );
         ATH_CHECK( m_outputMetaStore->record( std::move( m_format ),
                                               m_outputKey ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
