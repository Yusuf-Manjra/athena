/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowBuilder.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  8th November, 2001

PURPOSE:  Loop over tracks and clusters to build calo objects, then 
          loop over tracks and clusters within calo objects to build
          eflow objects.

********************************************************************/

// INCLUDE HEADER FILES:

#include "eflowRec/eflowBuilder.h"

#include "eflowRec/eflowBaseAlg.h"

// INCLUDE GAUDI HEADER FILES:
 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ListItem.h"

#include <algorithm> 
#include <math.h>

//  END OF HEADER FILES INCLUDE

//  CONSTRUCTOR:
    
eflowBuilder::eflowBuilder(const std::string& name, 
		       ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator), 
  m_eflowCaloObjectBuilderAlgName("eflowCaloObjectBuilder/caloObjectBuilderAlg"),
  m_eflowObjectBuilderAlgName("eflowObjectBuilder/objectBuilderAlg"),
  m_eflowQuantitiesAlgName("eflowQuantities/quantAlg")
{

// The following properties can be specified at run-time
// (declared in jobOptions file)
                            
  // Names of various sub-algorithms:
  declareProperty("eflowCaloObjectBuilderAlgName", 
		  m_eflowCaloObjectBuilderAlgName);
  declareProperty("eflowObjectBuilderAlgName", m_eflowObjectBuilderAlgName);
  declareProperty("eflowQuantitiesAlgName", m_eflowQuantitiesAlgName);
  m_eflowCaloObjectBuilderAlg = 0;
  m_eflowObjectBuilderAlg = 0;
  m_eflowQuantitiesAlg = 0;
}

eflowBuilder::~eflowBuilder()
{ }

StatusCode eflowBuilder::initialize()
{

  Algorithm* pAlg;
  StatusCode sc;
  MsgStream log( messageService(), name() );  

  // Get pointer to StoreGateSvc and cache:

  //////////////////////////////////////////////////////////////////
  // Create the eflowCaloObjectBuilder Algorithm:

  std::string type, name;

  ListItem caloObjectBuilderAlg(m_eflowCaloObjectBuilderAlgName);
  sc = createSubAlgorithm(caloObjectBuilderAlg.type(), 
			  caloObjectBuilderAlg.name(), pAlg);

  if( sc.isFailure() ) 
  {
    log << MSG::DEBUG
	<< "Unable to create " << m_eflowCaloObjectBuilderAlgName
	<< endreq;
    m_eflowCaloObjectBuilderAlg = 0;
  } 
  else
  {
    m_eflowCaloObjectBuilderAlg = dynamic_cast<eflowBaseAlg*>(pAlg);
  }
  
  //////////////////////////////////////////////////////////////////
  // Create the eflowObjectBuilder sub-algorithm:

  ListItem objectBuilderAlg(m_eflowObjectBuilderAlgName);
  sc = createSubAlgorithm(objectBuilderAlg.type(), 
			  objectBuilderAlg.name(), pAlg);
  if( sc.isFailure() ) 
  {
    log << MSG::DEBUG
	<< "Unable to create " << m_eflowObjectBuilderAlgName
	<< endreq;
    m_eflowObjectBuilderAlg = 0;
  } 
  else
  {
    m_eflowObjectBuilderAlg = dynamic_cast<eflowBaseAlg*>(pAlg);
  }

  //////////////////////////////////////////////////////////////////
  // Create the eflowQuantities sub-algorithm:

  ListItem quantAlg(m_eflowQuantitiesAlgName);
  sc = createSubAlgorithm(quantAlg.type(), quantAlg.name(), pAlg);
  if( sc.isFailure() ) 
  {
    log << MSG::DEBUG
	<< "Unable to create " << m_eflowQuantitiesAlgName
	<< endreq;
    m_eflowQuantitiesAlg = 0;
  } 
  else
  {
    m_eflowQuantitiesAlg = dynamic_cast<eflowBaseAlg*>(pAlg);
  }

/////////////////////////////////////////////////////////////////

    log << MSG::INFO << "Using the Algorithms:" << endreq;
    log << MSG::INFO <<  m_eflowCaloObjectBuilderAlgName << endreq;
    log << MSG::INFO << m_eflowObjectBuilderAlgName << endreq;
    log << MSG::INFO << m_eflowQuantitiesAlgName << endreq;


  return sc;
}


/////////////////////////////////////////////////////////////////

StatusCode eflowBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////

// ATHENA EXECUTE METHOD:
   
StatusCode eflowBuilder::execute()
{  

  StatusCode sc;

  MsgStream log( messageService(), name() );
  log << MSG::DEBUG 
      << "Executing eflowBuilder" 
      << endreq;

  // Build Calorimeter Objects

  if (m_eflowCaloObjectBuilderAlg)
    {
      sc = m_eflowCaloObjectBuilderAlg->execute();
    }

  // Build Energy Flow Objects

  if (sc.isSuccess() && m_eflowObjectBuilderAlg)
    {
      sc = m_eflowObjectBuilderAlg->execute();
    }

  // Calculate parameters

  if (sc.isSuccess() && m_eflowQuantitiesAlg)
    {
      sc = m_eflowQuantitiesAlg->execute();
    }

  return StatusCode::SUCCESS;

}
