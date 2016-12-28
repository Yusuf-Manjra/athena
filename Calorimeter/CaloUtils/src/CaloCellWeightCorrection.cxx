/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCellWeightCorrection.h
PACKAGE:  offline/Calorimeter/CaloUtils

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  February, 2005

PURPOSE: Loops over list of ICellWeightTools and applies weight to cell
       
********************************************************************/

// Calo Header files:

#include "CaloUtils/CaloCellWeightCorrection.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"

// For Gaudi
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"

// CONSTRUCTOR:

CaloCellWeightCorrection::CaloCellWeightCorrection(const std::string& type, 
				     const std::string& name, 
				     const IInterface* parent) 
  : CaloCellCorrection(type, name, parent)
{

  declareProperty( "CellWeightToolNames", m_cellWeightToolNames);

 }

// DESTRUCTOR:

CaloCellWeightCorrection::~CaloCellWeightCorrection()
{ }

//////////////////////////////////////////////////////////////
// Gaudi INITIALIZE method
//////////////////////////////////////////////////////////////

StatusCode CaloCellWeightCorrection::initialize() {

//---- retrieve the noisetool ----------------
  IToolSvc* toolSvc = 0;// Pointer to Tool Service
  ATH_CHECK( service("ToolSvc", toolSvc) );

// access tools  and store them

 std::vector<std::string>::const_iterator itrName=m_cellWeightToolNames.begin();
 std::vector<std::string>::const_iterator endName= m_cellWeightToolNames.end();

 IAlgTool* algtool;    
 for (; itrName!=endName;++itrName){

   ListItem theItem(*itrName);


   ATH_MSG_DEBUG( "Retrieving " << *itrName  );
   StatusCode sc = toolSvc->retrieveTool(theItem.type(), theItem.name(), algtool);

   if (sc.isFailure()) {
     ATH_MSG_INFO( "Unable to find tool for " <<(*itrName) );
   }
   else 
     { 
       ATH_MSG_INFO( (*itrName) << " successfully retrieved" );

       m_cellWeightTools.push_back( dynamic_cast<ICellWeightTool*>(algtool) );
     }
 }


  // Return status code.
  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////
// EXECUTE method: Correct cells in input cell container
//////////////////////////////////////////////////////////////

StatusCode CaloCellWeightCorrection::execute(CaloCellContainer* cellCollection) 
{
  ATH_MSG_DEBUG( "Executing CaloCellWeightCorrection"  );

  if (!cellCollection) 
  {
    ATH_MSG_DEBUG( " Cell Correction tool receives invalid cell Collection " );
    return StatusCode::FAILURE;
  }

  // Loop over all the CaloCell Objects and call Make Correction.
  // Note that this is the base class of all the concrete correction
  // classes which implement a Make Correction method.

  CaloCellContainer::iterator cellIter = cellCollection->begin();
  CaloCellContainer::iterator cellIterEnd = cellCollection->end();

  for (; cellIter != cellIterEnd; ++cellIter)
  { 
    MakeCorrection( *cellIter );
  }

  // Done, Return success

  return StatusCode::SUCCESS;

}

void CaloCellWeightCorrection::MakeCorrection( CaloCell* theCell ) 
{
  

  double weight = 1.0;
  
  std::vector<ICellWeightTool*>::iterator toolIter=m_cellWeightTools.begin();
  std::vector<ICellWeightTool*>::iterator toolIterEnd=m_cellWeightTools.end();

  ICellWeightTool* theTool;
  
  for (; toolIter != toolIterEnd; toolIter++){
    theTool = *toolIter;
    
    // need to be able to initialize tool (i.e. set container)    
    weight *= theTool->wtCell( theCell );    
  }

  theCell->scaleEnergy( weight );
  
}


