/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTPCnv/SegmentCollectionCnv_tlp2.h"

SegmentCollectionCnv_tlp2::SegmentCollectionCnv_tlp2( )
{
   // Add all converters defined in this top level converter:
   // never change the order of adding converters!

   addMainTPConverter();
   
   addTPConverter( &m_trackSegmentCnv );
   addTPConverter( &m_segmentCnv );

   addTPConverter( &m_planeSurfacesCnv );         
   addTPConverter( &m_discSurfacesCnv );         
   addTPConverter( &m_cylinderSurfacesCnv );         
   addTPConverter( &m_straightLineSurfacesCnv );         
   addTPConverter( &m_surfacesCnv );

   addTPConverter( &m_cylinderBoundsCnv );        
   addTPConverter( &m_diamondBoundsCnv );         
   addTPConverter( &m_discBoundsCnv );            
   addTPConverter( &m_rectangleBoundsCnv );       
   addTPConverter( &m_trapesoidBoundsCnv );       

   addTPConverter( &m_fitQualitiesCnv );
   addTPConverter( &m_errorMatricesCnv );   
   addTPConverter( &m_covarianceMatricesCnv );     
   addTPConverter( &m_localParametersCnv );            

   // addTPConverter( &m_RIOsCnv );
   addTPConverter( &m_pseudoMeasurementOnTrackCnv );
   addTPConverter( &m_crotCnv );   

   // Added with Tag TrkEventTPCnv-00-16-00
   addTPConverter( &m_rotatedTrapesoidBoundsCnv );           
   addTPConverter( &m_detElSurfCnv );   
   
   //Added with TrkEventTPCnv-00-20-15          
   addTPConverter( &m_ellipseBoundsCnv );
}

SegmentCollectionCnv_tlp2::~SegmentCollectionCnv_tlp2()
= default;

void SegmentCollectionCnv_tlp2::setPStorage( Trk::SegmentCollection_tlp2 *storage )
{
   // for implicit TrackCollection_p1 from the base class:
     setMainCnvPStorage( &storage->m_segmentCollections );

     // for all converters defined in this top level converter
     m_segmentCnv.                  setPStorage( &storage->m_segments );                
     m_trackSegmentCnv.             setPStorage( &storage->m_tracksegments );            

     m_discSurfacesCnv.             setPStorage( &storage->m_boundSurfaces );         
     m_planeSurfacesCnv.            setPStorage( &storage->m_boundSurfaces );         
     m_cylinderSurfacesCnv.         setPStorage( &storage->m_boundSurfaces );         
     m_straightLineSurfacesCnv.     setPStorage( &storage->m_boundSurfaces );         
     m_surfacesCnv.                 setPStorage( &storage->m_surfaces );        // 8      
                                    
     m_cylinderBoundsCnv.           setPStorage( &storage->m_cylinderBounds );        
     m_diamondBoundsCnv.            setPStorage( &storage->m_diamondBounds );         
     m_discBoundsCnv.               setPStorage( &storage->m_discBounds );            
     m_rectangleBoundsCnv.          setPStorage( &storage->m_rectangleBounds );       
     m_trapesoidBoundsCnv.          setPStorage( &storage->m_trapesoidBounds );       

     m_fitQualitiesCnv.             setPStorage( &storage->m_fitQualities );          
     m_covarianceMatricesCnv.       setPStorage( &storage->m_hepSymMatrices );        
     m_errorMatricesCnv.            setPStorage( &storage->m_hepSymMatrices );        
     m_localParametersCnv.          setPStorage( &storage->m_localParameters );       

     m_pseudoMeasurementOnTrackCnv. setPStorage( &storage->m_pseudoMeasurementOnTrack );  
     m_crotCnv.                     setPStorage( &storage->m_competingRotsOnTrack );     
     m_detElSurfCnv.                setPStorage( &storage->m_detElementSurfaces );      // 21
     m_rotatedTrapesoidBoundsCnv.   setPStorage( &storage->m_rotatedTrapesoidBounds);
     m_ellipseBoundsCnv.        setPStorage( &storage->m_ellipseBounds); 
     
}   
 
