/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_VIEWHELPER_HH
#define ATHVIEWS_VIEWHELPER_HH

#include "CxxUtils/make_unique.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthViews/View.h"
#include "AthViews/GraphExecutionTask.h"
#include "AthContainers/DataVector.h"

#include "tbb/task.h"

namespace ViewHelper
{
	//Function to create a vector of views, each populated with one data object
	template< typename T >
	inline StatusCode MakeAndPopulate( std::string const& ViewNameRoot, std::vector< SG::View* > & ViewVector,
			SG::WriteHandle< T > & PopulateHandle, std::vector< T > const& InputData )
	{
		//Loop over all input data
		unsigned int const viewNumber = InputData.size();
		for ( unsigned int viewIndex = 0; viewIndex < viewNumber; ++viewIndex )
		{
			//Create view
			std::string viewName = ViewNameRoot + std::to_string( viewIndex );
			SG::View * outputView = new SG::View( viewName );
			ViewVector.push_back( outputView );

			//Attach the handle to the view
			StatusCode sc = PopulateHandle.setProxyDict( outputView );
			if ( !sc.isSuccess() )
			{
				ViewVector.clear();
				return sc;
			}

			//Populate the view
			sc = PopulateHandle.record( CxxUtils::make_unique< T >( InputData[ viewIndex ] ) );
			if ( !sc.isSuccess() )
			{
				ViewVector.clear();
				return sc;
			}
		}

		return StatusCode::SUCCESS;
	}

        //Function to add data to existing views
        template< typename T >
        inline StatusCode Populate( std::vector< SG::View* > & ViewVector,
                        SG::WriteHandle< T > & PopulateHandle, std::vector< T > const& InputData )
        {
		//Vector length check
                unsigned int const viewNumber = InputData.size();
		if ( viewNumber != ViewVector.size() ) return StatusCode::FAILURE;

                //Loop over all input data
                for ( unsigned int viewIndex = 0; viewIndex < viewNumber; ++viewIndex )
                {
                        //Attach the handle to the view
                        StatusCode sc = PopulateHandle.setProxyDict( ViewVector.at( viewIndex ) );
                        if ( !sc.isSuccess() )
                        {
                                return sc;
                        }

                        //Populate the view
                        sc = PopulateHandle.record( CxxUtils::make_unique< T >( InputData.at( viewIndex ) ) );
                        if ( !sc.isSuccess() )
                        {
                                return sc;
                        }
                }

                return StatusCode::SUCCESS;
        }

	//Function to run a set of views with the named algorithms
	inline StatusCode RunViews( std::vector< SG::View* > const& ViewVector, std::vector< std::string > const& AlgorithmNames,
			EventContext const& InputContext, SmartIF< IService > & AlgPool )
	{
		//Check there is work to do
		if ( !ViewVector.size() || !AlgorithmNames.size() )
		{
			return StatusCode::FAILURE;
		}

		//Create a tbb task for each view
		tbb::task_list allTasks;
		for ( SG::View* inputView : ViewVector )
		{
			//Make a context with the view attached
			EventContext * viewContext = new EventContext( InputContext );
			viewContext->setProxy( inputView );

			//Make the task
			tbb::task * viewTask = new( tbb::task::allocate_root() )GraphExecutionTask( AlgorithmNames, viewContext, AlgPool );
			allTasks.push_back( *viewTask );
		}
		
		//Run the tasks
		tbb::task::spawn_root_and_wait( allTasks );

		return StatusCode::SUCCESS;
	}

	//Function merging view data into a single collection
	template< typename T >
	inline StatusCode MergeViewCollection( std::vector< SG::View* > const& ViewVector, SG::ReadHandle< T > & QueryHandle, T & OutputData )
	{
		//Loop over all views
		for ( SG::View* inputView : ViewVector )
		{
			//Attach the handle to the view
			StatusCode sc = QueryHandle.setProxyDict( inputView );
			if ( !sc.isSuccess() )
			{
				OutputData.clear();
				return sc;
			}

			//Merge the data
			T inputData = *QueryHandle;
			OutputData.insert( OutputData.end(), inputData.begin(), inputData.end() );
		}

		return StatusCode::SUCCESS;
	}

	//Function merging view data into a single collection - overload for datavectors because aux stores are annoying
	template< typename T >
	inline StatusCode MergeViewCollection( std::vector< SG::View* > const& ViewVector, SG::ReadHandle< DataVector< T > > & QueryHandle, DataVector< T > & OutputData )
	{
		//Loop over all views
		for ( SG::View* inputView : ViewVector )
		{
			//Attach the handle to the view
			StatusCode sc = QueryHandle.setProxyDict( inputView );
			if ( !sc.isSuccess() )
			{
				OutputData.clear();
				return sc;
			}

			//Merge the data
			for ( const auto inputObject : *QueryHandle.cptr() )
			{
				//Relies on non-existant assignment operator in TrigComposite
				T * outputObject = new T();
				OutputData.push_back( outputObject );
				*outputObject = *inputObject;
			}

			//Tomasz version
			//const size_t sizeSoFar = OutputData.size();
			//OutputData.resize( sizeSoFar + QueryHandle->size() );
			//std::swap_ranges( QueryHandle.ptr()->begin(), QueryHandle.ptr()->end(), OutputData.begin() + sizeSoFar );
		}

		return StatusCode::SUCCESS;
	}
  /**
   * @arg unique_index - gets appended to the view name if >= 0
   */
  inline SG::View* makeView( const std::string& common_name, int unique_index=-1) {
	  return  (( unique_index == -1 ) ?
		   new SG::View( common_name ) :
		   new SG::View( common_name+ " "+std::to_string(unique_index) ) );
	  
  }
  
  /**
   * @brief - records object in a view
   */  
  template< typename T >
  StatusCode addToView( SG::View* view, const std::string& key, T* ptr ) {
    
    SG::WriteHandle< T > handle( key );
    
    if( handle.setProxyDict( view ).isFailure() ) {
      return StatusCode::FAILURE;
    }
    
    if ( handle.record( std::unique_ptr<T>( ptr ) ).isFailure() ) {
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }

  template< typename T >
  StatusCode addViewCollectionToView(SG::View* view, const std::string& key, const T* src ) {
    auto * viewColl = new ConstDataVector<T>();    
    viewColl->clear( SG::VIEW_ELEMENTS ); //make it a view
    viewColl->insert( viewColl->end(), src->begin(), src->end() ); // copy content

    return  addToView( view, key, viewColl );    
  }

  
  /**
   * @return nullptr if object of type T is missing in the view
   */
  template< typename T >
  const T* getFromView( SG::View* view, const std::string& key) {
    
    SG::ReadHandle<T> handle(key);
    
    if ( handle.setProxyDict(view).isFailure() )  {
      return nullptr;
    }
    
    return handle.cptr();
  }

  

  
} // EOF namspace ViewHelper

#endif
