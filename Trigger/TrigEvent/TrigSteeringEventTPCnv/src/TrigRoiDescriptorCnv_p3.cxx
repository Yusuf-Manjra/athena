/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#undef private
#undef protected

#include <iostream>
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p3.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p3.h"


void TrigRoiDescriptorCnv_p3::persToTrans(const TrigRoiDescriptor_p3* persObj, 
					  TrigRoiDescriptor* transObj, 
					  MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p3::persToTrans called " << endreq;

#if 0
   transObj->m_phi       = persObj->geom[phi]        ;                 
   transObj->m_eta       = persObj->geom[eta]        ;                 
   transObj->m_zed       = persObj->geom[zed]        ;                 
   transObj->m_phiPlus   = persObj->geom[phiplus]    ;
   transObj->m_phiMinus  = persObj->geom[phiminus]   ;
   transObj->m_etaPlus   = persObj->geom[etaplus]    ;
   transObj->m_etaMinus  = persObj->geom[etaminus]   ;
   transObj->m_zedPlus   = persObj->geom[zedplus]    ;
   transObj->m_zedMinus  = persObj->geom[zedminus]   ;
   
   transObj->m_roiId     = persObj->ids[roiid]       ;         
   transObj->m_roiWord   = persObj->ids[roiword]     ;   
    
   transObj->m_fullscan  = persObj->fullscan   ;
   transObj->m_version   = 3;
#endif

   double _phi       = persObj->geom[phi]        ;                 
   double _eta       = persObj->geom[eta]        ;                 
   double _zed       = persObj->geom[zed]        ;                 
   double _phiPlus   = persObj->geom[phiplus]    ;
   double _phiMinus  = persObj->geom[phiminus]   ;
   double _etaPlus   = persObj->geom[etaplus]    ;
   double _etaMinus  = persObj->geom[etaminus]   ;
   double _zedPlus   = persObj->geom[zedplus]    ;
   double _zedMinus  = persObj->geom[zedminus]   ;
   
   transObj->m_roiId     = persObj->ids[roiid]       ;         
   transObj->m_roiWord   = persObj->ids[roiword]     ;   
    
   transObj->m_fullscan  = persObj->fullscan   ;

   /// now set up the variables
   transObj->construct( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, _zed, _zedMinus, _zedPlus ); 
   
   if ( persObj->rois.size()>0 ) {
     
     transObj->m_manageConstituents = true;
     
     for ( unsigned i=0 ; i<persObj->rois.size() ; i++ ) { 

       const std::vector<float>& _roi = persObj->rois[i];

       transObj->push_back( new TrigRoiDescriptor( _roi[eta], _roi[etaminus], _roi[etaplus],
						   _roi[phi], _roi[phiminus], _roi[phiplus],
						   _roi[zed], _roi[zedminus], _roi[zedplus] ) );
     } 
   } 

}


void TrigRoiDescriptorCnv_p3::transToPers(const TrigRoiDescriptor* transObj, 
				       TrigRoiDescriptor_p3* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigRoiDescriptorCnv_p3::transToPers called " << endreq;

   persObj->geom[phi]      = transObj->m_phi        ;
   persObj->geom[eta]      = transObj->m_eta        ;
   persObj->geom[zed]      = transObj->m_zed        ;
   persObj->geom[phiplus]  = transObj->m_phiPlus    ;
   persObj->geom[phiminus] = transObj->m_phiMinus   ;
   persObj->geom[etaplus]  = transObj->m_etaPlus    ;
   persObj->geom[etaminus] = transObj->m_etaMinus   ;
   persObj->geom[zedplus]  = transObj->m_zedPlus    ;
   persObj->geom[zedminus] = transObj->m_zedMinus   ;
         
   persObj->ids[roiid]     = transObj->m_roiId      ;
   persObj->ids[roiword]   = transObj->m_roiWord    ;

   persObj->fullscan       = transObj->m_fullscan   ;
   
   

   if ( transObj->m_roiDescriptors.size()>0 ) {

     persObj->rois.reserve(transObj->m_roiDescriptors.size());

     for ( unsigned i=0 ; i<transObj->m_roiDescriptors.size() ; i++ ) {
 
       std::vector<float> _roi(9);

       const IRoiDescriptor* _iroi = transObj->m_roiDescriptors[i]; 
 
       _roi[phi] = _iroi->phi();
       _roi[eta] = _iroi->eta();
       _roi[zed] = _iroi->zed();

       _roi[phiminus] = _iroi->phiMinus();
       _roi[phiplus]  = _iroi->phiPlus();

       _roi[etaminus] = _iroi->etaMinus();
       _roi[etaplus]  = _iroi->etaPlus();

       _roi[zedminus] = _iroi->zedMinus();
       _roi[zedplus]  = _iroi->zedPlus();

       persObj->rois.push_back( _roi );
     } 
   } 

}

