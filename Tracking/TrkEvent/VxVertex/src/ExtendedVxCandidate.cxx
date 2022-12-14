/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"

namespace Trk
{

//default constructor for persistensy
  ExtendedVxCandidate::ExtendedVxCandidate() : VxCandidate(), m_fullCovariance(nullptr)
  {}
   
   
//real working constructor   
  ExtendedVxCandidate::ExtendedVxCandidate(const Trk::RecVertex& recVertex,
                                           const std::vector<Trk::VxTrackAtVertex *>& vxTrackAtVertex,
                                           const Amg::MatrixX * fullCov) :
                                           VxCandidate(recVertex, vxTrackAtVertex),
					   m_fullCovariance(fullCov) {}

  ExtendedVxCandidate::~ExtendedVxCandidate()
  {
   if(m_fullCovariance != nullptr) { delete m_fullCovariance;
}
  }

  ExtendedVxCandidate::ExtendedVxCandidate(const ExtendedVxCandidate& rhs):
                                           VxCandidate(rhs),
                                           m_fullCovariance(rhs.m_fullCovariance ? new Amg::MatrixX(*rhs.m_fullCovariance) : nullptr)
  {}
  
  ExtendedVxCandidate & ExtendedVxCandidate::operator= (const ExtendedVxCandidate & rhs)
  {
   if (this!=&rhs)
   {
     this->VxCandidate::operator=(rhs);
     delete m_fullCovariance;
     m_fullCovariance = rhs.m_fullCovariance ? new Amg::MatrixX(*rhs.m_fullCovariance) : nullptr;
   }
   return *this;
  }//end of assignement operator


}//end of namespace definitions
