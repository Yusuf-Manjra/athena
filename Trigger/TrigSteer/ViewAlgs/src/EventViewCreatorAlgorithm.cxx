/*
  General-purpose view creation algorithm <bwynne@cern.ch>
  
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EventViewCreatorAlgorithm.h"
#include "AthLinks/ElementLink.h"
#include "AthViews/ViewHelper.h"
#include "AthViews/View.h"
#include "DecisionHandling/TrigCompositeUtils.h"

using namespace TrigCompositeUtils;

EventViewCreatorAlgorithm::EventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
: InputMakerBase( name, pSvcLocator ) {}

EventViewCreatorAlgorithm::~EventViewCreatorAlgorithm(){}

StatusCode EventViewCreatorAlgorithm::initialize() {
  ATH_MSG_DEBUG("Will produce views=" << m_viewsKey << " roIs=" << m_inViewRoIs );
  ATH_CHECK( m_viewsKey.initialize() );
  ATH_CHECK( m_inViewRoIs.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode EventViewCreatorAlgorithm::execute( const EventContext& context ) const {

   if (m_mergeOutputs)
    return executeMerged(context);

  // create the output decisions, similar to inputs (copy basic links)
  std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> > outputHandles;
  ATH_CHECK (decisionInputToOutput(context, outputHandles));

 
  // make the views
  auto viewsHandle = SG::makeHandle( m_viewsKey, context );
  auto viewVector1 = std::make_unique< ViewContainer >();
  ATH_CHECK( viewsHandle.record(  std::move( viewVector1 ) ) );
  auto viewVector = viewsHandle.ptr();
  unsigned int viewCounter = 0;

  //map all RoIs that are stored
  ElementLinkVector<TrigRoiDescriptorCollection> RoIsFromDecision;

  // loop over decisions
  for (auto outputHandle: outputHandles) {
    if( not outputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Got no decisions from output "<< outputHandle.key() << " because handle not valid");
      continue;
    }

    if( outputHandle->size() == 0){ // input filtered out
      ATH_MSG_DEBUG( "Got no decisions from output "<< outputHandle.key()<<": handle is valid but container is empty.");
      continue;
    }
    ATH_MSG_DEBUG( "Got output "<< outputHandle.key()<<" with " << outputHandle->size() << " elements" );
    // loop over output decisions in container of outputHandle, follow link to inputDecision
    for ( auto outputDecision : *outputHandle){ 
      ElementLinkVector<DecisionContainer> inputLinks = getLinkToPrevious(outputDecision);
      ATH_MSG_DEBUG( "Got inputLinks with " << inputLinks.size() << " elements" );
      // loop over input links as predecessors
      for (auto input: inputLinks){
        const Decision* inputDecision = *input;
        // find the RoI
        const auto roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( inputDecision, m_roisLink.value() );
        const auto roiEL = roiELInfo.link;
        ATH_CHECK( roiEL.isValid() );
        // check if already found
        auto roiIt=find(RoIsFromDecision.begin(), RoIsFromDecision.end(), roiEL);
        if ( roiIt == RoIsFromDecision.end() ){
          RoIsFromDecision.push_back(roiEL); // just to keep track of which we have used
          ATH_MSG_DEBUG("Found RoI:" <<**roiEL<<" FS="<<(*roiEL)->isFullscan());
          ATH_MSG_DEBUG("Positive decisions on RoI, making view" );
	  // make the view
          auto newView = ViewHelper::makeView( name()+"_view", viewCounter++, m_viewFallThrough ); //pointer to the view

          // Use a fall-through filter if one is provided
          if ( m_viewFallFilter.size() ) {
            newView->setFilter( m_viewFallFilter );
          }

          viewVector->push_back( newView );
          
          // link decision to this view
          outputDecision->setObjectLink( TrigCompositeUtils::viewString(), ElementLink< ViewContainer >(m_viewsKey.key(), viewVector->size()-1 ));//adding view to TC
          ATH_MSG_DEBUG( "Adding new view to new decision; storing view in viewVector component " << viewVector->size()-1 );
          ATH_CHECK( linkViewToParent( inputDecision, viewVector->back() ) );
          ATH_CHECK( placeRoIInView( roiEL, viewVector->back(), context ) );
        }
        else {
          int iview = roiIt - RoIsFromDecision.begin();
          auto theview = viewVector->at(iview);
	  // check that this view is not already linked to this decision (this is true if the mergerd output is enabled)
	  const auto existView = TrigCompositeUtils::findLink<ViewContainer> (outputDecision, TrigCompositeUtils::viewString());
	  const auto existViewLink= existView.link;
	  if (existViewLink.isValid() && existViewLink == ElementLink< ViewContainer >(m_viewsKey.key(), iview )){
	    ATH_MSG_DEBUG( "View  already linked, doing nothing");
	  } else if (existViewLink.isValid()){
	    ATH_MSG_ERROR( "View already linked, but it is the wrong view! Configuration problem. Got " << existViewLink.index() << ", expected " << iview);
            return StatusCode::FAILURE;
          } else {
	    outputDecision->setObjectLink( TrigCompositeUtils::viewString(), ElementLink< ViewContainer >(m_viewsKey.key(), iview ) ); //adding view to TC
	    ATH_MSG_DEBUG( "Adding already mapped view " << iview << " in ViewVector , to new decision");
	    ATH_CHECK( linkViewToParent( inputDecision, theview ) );
	  }
        }
      }// loop over previous inputs
    } // loop over decisions
  }// loop over output keys

  // launch view execution
  ATH_MSG_DEBUG( "Launching execution in " << viewVector->size() << " views" );
  ATH_CHECK( ViewHelper::ScheduleViews( viewVector,           // Vector containing views
					m_viewNodeName,             // CF node to attach views to
					context,                    // Source context
					getScheduler(),
					m_reverseViews ) );
  
  // report number of views, stored already when container was created
  // auto viewsHandle = SG::makeHandle( m_viewsKey );
  // ATH_CHECK( viewsHandle.record(  std::move( viewVector ) ) );
  ATH_MSG_DEBUG( "Store "<< viewsHandle->size() <<" Views");

  if (msgLvl(MSG::DEBUG)) debugPrintOut(context, outputHandles);
  return StatusCode::SUCCESS;
}



StatusCode EventViewCreatorAlgorithm::executeMerged( const EventContext& context ) const {

   // create the output decisions, similar to inputs (copy basic links)
  std::vector< SG::WriteHandle<TrigCompositeUtils::DecisionContainer> > outputHandles;
  ATH_CHECK (decisionInputToOutput(context, outputHandles));

  // make the views
  auto viewsHandle = SG::makeHandle( m_viewsKey, context ); 
  auto viewVector1 = std::make_unique< ViewContainer >();
  ATH_CHECK( viewsHandle.record(  std::move( viewVector1 ) ) );
  auto viewVector = viewsHandle.ptr();
  unsigned int viewCounter = 0;

  //map all RoIs that are stored
  ElementLinkVector<TrigRoiDescriptorCollection> RoIsFromDecision;

  // loop over decisions
  for (auto outputHandle: outputHandles) {
    if( not outputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Got no decisions from output "<< outputHandle.key() << " because handle not valid");
      continue;
    }

    if( outputHandle->size() == 0){ // input filtered out
      ATH_MSG_DEBUG( "Got no decisions from output "<< outputHandle.key()<<": handle is valid but container is empty.");
      continue;
    }
    ATH_MSG_DEBUG( "Got output "<< outputHandle.key()<<" with " << outputHandle->size() << " elements" );
    // loop over output decisions in container of outputHandle, follow link to inputDecision
    for ( auto outputDecision : *outputHandle){ 
      const auto roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( outputDecision, m_roisLink.value() );
      const auto roiEL = roiELInfo.link;
      ATH_CHECK( roiEL.isValid() );
      // check if already found
      auto roiIt=find(RoIsFromDecision.begin(), RoIsFromDecision.end(), roiEL);
      if ( roiIt == RoIsFromDecision.end() ){
	RoIsFromDecision.push_back(roiEL); // just to keep track of which we have used 
	ATH_MSG_DEBUG("Found RoI:" <<**roiEL<<" FS="<<(*roiEL)->isFullscan());
	ATH_MSG_DEBUG("Positive decisions on RoI, making view" );
	// make the view
	auto newView = ViewHelper::makeView( name()+"_view", viewCounter++, m_viewFallThrough ); //pointer to the view

        // Use a fall-through filter if one is provided
        if ( m_viewFallFilter.size() ) {
          newView->setFilter( m_viewFallFilter );
        }

	viewVector->push_back( newView );
	
	// link decision to this view
	outputDecision->setObjectLink( TrigCompositeUtils::viewString(), ElementLink< ViewContainer >(m_viewsKey.key(), viewVector->size()-1 ));//adding view to TC
	ATH_MSG_DEBUG( "Adding new view to new decision; storing view in viewVector component " << viewVector->size()-1 );
	ATH_CHECK( placeRoIInView( roiEL, viewVector->back(), context ) );
	ElementLinkVector<DecisionContainer> inputLinks = getLinkToPrevious(outputDecision);
	ATH_MSG_DEBUG( "Got inputLinks with " << inputLinks.size() << " elements" );
	for (auto input: inputLinks){
	  const Decision* inputDecision = *input;
	  ATH_CHECK( linkViewToParent( inputDecision, viewVector->back() ) );
	}
      }
      else {          
	ATH_MSG_DEBUG( "View  already linked, doing nothing");	
      }
    } // loop over decisions   
  }// loop over output keys

  // launch view execution
  ATH_MSG_DEBUG( "Launching execution in " << viewVector->size() << " views" );
  ATH_CHECK( ViewHelper::ScheduleViews( viewVector,           // Vector containing views
					m_viewNodeName,             // CF node to attach views to
					context,                    // Source context
					getScheduler(), 
					m_reverseViews ) );
  
  if (msgLvl(MSG::DEBUG)) debugPrintOut(context, outputHandles);
  return StatusCode::SUCCESS;
}



StatusCode EventViewCreatorAlgorithm::linkViewToParent( const TrigCompositeUtils::Decision* inputDecision, SG::View* newView ) const {
  if ( m_requireParentView ) {
    // see if there is a view linked to the decision object, if so link it to the view that is just made
    LinkInfo<ViewContainer> parentViewLinkInfo = findLink<ViewContainer>(inputDecision, viewString(), /*suppressMultipleLinksWarning*/ true );
    if ( parentViewLinkInfo.isValid() ) {
      ATH_CHECK( parentViewLinkInfo.link.isValid() );
      auto parentView = *parentViewLinkInfo.link;
      newView->linkParent( parentView );
      ATH_MSG_DEBUG( "Parent view linked" );
    } else {
      ATH_MSG_ERROR( "Parent view not linked because it could not be found" );
      ATH_MSG_ERROR( TrigCompositeUtils::dump( inputDecision, [](const xAOD::TrigComposite* tc){
        return "TC " + tc->name() + ( tc->hasObjectLink("view") ? " has view " : " has no view " );
      } ) );
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_DEBUG( "Parent view linking not required" );
  }
  return StatusCode::SUCCESS;
}

StatusCode EventViewCreatorAlgorithm::placeRoIInView( const ElementLink<TrigRoiDescriptorCollection>& roiEL, SG::View* view, const EventContext& context ) const {
  // fill the RoI output collection
  auto oneRoIColl = std::make_unique< ConstDataVector<TrigRoiDescriptorCollection> >();
  oneRoIColl->clear( SG::VIEW_ELEMENTS ); //Don't delete the RoIs
  oneRoIColl->push_back( *roiEL );

  view->setROI(roiEL);

  //store the RoI in the view
  auto handle = SG::makeHandle( m_inViewRoIs, context );
  ATH_CHECK( handle.setProxyDict( view ) );
  ATH_CHECK( handle.record( std::move( oneRoIColl ) ) );
  return StatusCode::SUCCESS;
}
