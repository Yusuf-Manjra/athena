/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMHitsCnvAlg.cxx 576328 2013-12-19 16:09:32Z morrisj $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigT1CaloEvent/CPMHitsCollection.h"
#include "xAODTrigL1Calo/CPMHitsContainer.h"
#include "xAODTrigL1Calo/CPMHitsAuxContainer.h"

// Local include(s):
#include "CPMHitsCnvAlg.h"

namespace xAODMaker {

   CPMHitsCnvAlg::CPMHitsCnvAlg( const std::string& name,
                                 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::CPMHitsCnvTool/CPMHitsCnvTool", this ) {

      declareProperty( "ESDKey" , m_esdKey  = "CPMHits" );
      declareProperty( "xAODKey", m_xaodKey = "CPMHits" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode CPMHitsCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " ESD Key: " << m_esdKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode CPMHitsCnvAlg::execute() {
      
      // Retrieve the ESD container:
      const CPMHitsCollection* esd = 0;
      CHECK( evtStore()->retrieve( esd, m_esdKey ) );
      
      // Create the xAOD container and its auxiliary store:
      xAOD::CPMHitsAuxContainer* aux = new xAOD::CPMHitsAuxContainer();
      xAOD::CPMHitsContainer* xaod = new xAOD::CPMHitsContainer();
      xaod->setStore( aux );
      
      // Fill the xAOD container:
      CHECK( m_cnvTool->convert( esd, xaod ) );     
      
      // Record the xAOD containers:
      CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      CHECK( evtStore()->record( xaod, m_xaodKey ) );

      // Return gracefully:
      return StatusCode::SUCCESS;     
   }

} // namespace xAODMaker
