/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"

namespace Muon
{

  // Default constructor:
  sTgcClusterOnTrack::sTgcClusterOnTrack():
    MuonClusterOnTrack(), // call base class ctor
    m_rio(),
    m_detEl(0)
  {}

  // copy constructor:
  sTgcClusterOnTrack::sTgcClusterOnTrack( const sTgcClusterOnTrack& rot):
    MuonClusterOnTrack(rot), // base clas ctor
    m_rio(rot.m_rio),
    m_detEl(rot.m_detEl)
  {}

  // Constructor with parameters
  sTgcClusterOnTrack::sTgcClusterOnTrack(
					 const sTgcPrepData* RIO,
					 const Trk::LocalParameters& locpos,
					 const Amg::MatrixX& locerr,
					 double positionAlongStrip) :
    MuonClusterOnTrack(locpos, locerr, RIO->identify(), positionAlongStrip), //call base class constructor
    m_detEl( RIO->detectorElement() )
  {
    //Set EL
    // m_rio = ElementLinkToIDC_TGC_Container("TGC_Measurements", RIO->getHashAndIndex().hashAndIndex(), RIO);
    m_rio.setElement(RIO);
  }

  // Destructor:
  sTgcClusterOnTrack::~sTgcClusterOnTrack()
  {
    // we don't own the m_rio object (it belongs to SG), so don't delete it.
  }

  // assignment operator:
  sTgcClusterOnTrack& sTgcClusterOnTrack::operator=( const sTgcClusterOnTrack& rot)
  {
    if ( &rot != this)
      {
        MuonClusterOnTrack::operator=(rot);//base class ass. op.
        m_rio = rot.m_rio;
        m_detEl = rot.m_detEl;
      }
    return *this;
  }

  MsgStream& sTgcClusterOnTrack::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO<<"sTgcClusterOnTrack {"<<std::endl;

    MuonClusterOnTrack::dump(stream); 

    stream<<"}"<<endreq;
    return stream;
  }

  std::ostream& sTgcClusterOnTrack::dump( std::ostream&    stream) const
  {
    stream << "sTgcClusterOnTrack {"<<std::endl;

    MuonClusterOnTrack::dump(stream); 

    stream<<"}"<<std::endl;
    return stream;
  }


}


