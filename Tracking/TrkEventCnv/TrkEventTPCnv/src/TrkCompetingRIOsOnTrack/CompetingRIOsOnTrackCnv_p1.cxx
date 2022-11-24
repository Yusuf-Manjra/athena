/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"
#include <vector>
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void CompetingRIOsOnTrackCnv_p1::persToTrans( const Trk::CompetingRIOsOnTrack_p1 *persObj, Trk::CompetingRIOsOnTrack *transObj, MsgStream &log )
{
  fillTransFromPStore( &m_localParCnv, persObj->m_localParameters, &transObj->m_localParams, log );
   // fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrorMatrix, &transObj->m_localErrMat, log );

  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrorMatrix, &dummy, log );

  EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "CompetingRIOsOnTrackCnv_p1");

  #ifdef UseFloatsIn_CompetingRIOsOnTrack
  auto tmpVector = std::vector<double>();
  tmpVector.reserve(persObj->m_assignProb.size());
  for (std::vector<float>::const_iterator itr  = persObj->m_assignProb.begin() ; 
  itr != persObj->m_assignProb.end()   ; ++itr)
  {
    tmpVector.push_back((double)(*itr));
  }
  transObj->m_assignProb = std::move(tmpVector);
  #else
  transObj->m_assignProb = std::vector<double>(persObj->m_assignProb);
  #endif

}

void CompetingRIOsOnTrackCnv_p1::transToPers( const Trk::CompetingRIOsOnTrack * transObj, Trk::CompetingRIOsOnTrack_p1 * persObj, MsgStream & log )
{
    persObj->m_localParameters = toPersistent( &m_localParCnv, &transObj->m_localParams, log );
    
    Trk::ErrorMatrix pMat;
    EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_localCovariance, "CompetingRIOsOnTrackCnv_p1");
    persObj->m_localErrorMatrix = toPersistent( &m_errorMxCnv, &pMat, log ); 

    persObj->m_assignProb.assign (transObj->m_assignProb.begin(),
                                  transObj->m_assignProb.end());
}
