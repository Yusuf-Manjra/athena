/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RoiCollectionToViews.h"
#include "AthViews/ViewHelper.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "AthContainers/ConstDataVector.h"

namespace AthViews {

///////////////////////////////////////////////////////////////////
// Public methods: 
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
RoiCollectionToViews::RoiCollectionToViews( const std::string& name,
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
}

// Destructor
///////////////
RoiCollectionToViews::~RoiCollectionToViews()
{
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode RoiCollectionToViews::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  CHECK( m_trigRoIs.initialize() );
  CHECK( m_viewRoIs.initialize() );
  CHECK( m_w_views.initialize() );
  CHECK( m_scheduler.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode RoiCollectionToViews::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const EventContext& ctx = getContext();

  //Load the collection of RoI descriptors
  SG::ReadHandle< TrigRoiDescriptorCollection > inputRoIs( m_trigRoIs, ctx );
  ATH_CHECK( inputRoIs.isValid() );
 
  //Skip if there's nothing to do
  //if ( inputRoIs->empty() ) return StatusCode::SUCCESS;
  //NB: don't do this any more. Calling the helper with an empty collection is now correct
  //as this will deactivate the node in the scheduler

  std::vector< ConstDataVector<TrigRoiDescriptorCollection> > outputRoICollectionVector;
  for ( auto roi: *inputRoIs )
  {
    ATH_MSG_DEBUG( "RoI Eta: " << roi->eta() << " Phi: " << roi->phi() << " RoIWord: " << roi->roiWord() );
    
    ConstDataVector<TrigRoiDescriptorCollection> oneRoIColl (SG::VIEW_ELEMENTS);
    oneRoIColl.push_back( roi );
    outputRoICollectionVector.push_back( std::move(oneRoIColl) );
  }

  //Create the views and populate them
  auto viewVector =  std::make_unique<ViewContainer>();
  CHECK( ViewHelper::MakeAndPopulate( m_viewBaseName, //Base name for all views to use
				      viewVector.get(),                                 //Vector to store views
				      m_viewRoIs,                                 //A writehandlekey to use to access the views
				      ctx,                                        //The context of this algorithm
				      outputRoICollectionVector,                  //Data to initialise each view - one view will be made per entry
				      m_viewFallThrough ) );                      //Allow fall through from view to storegate

  //Run the algorithms in views
  CHECK( ViewHelper::ScheduleViews( viewVector.get(), //View vector
				    m_viewNodeName,                       //CF node to attach views to
				    ctx,                                  //Context to attach the views to
				    m_scheduler.get() ) );                //Scheduler

  //Store the collection of views
  SG::WriteHandle< ViewContainer > outputViewHandle( m_w_views, ctx );
  outputViewHandle.record( std::move( viewVector ) );

  return StatusCode::SUCCESS;
}

} //> end namespace AthViews
