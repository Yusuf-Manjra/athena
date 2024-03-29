/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"

namespace Muon
{

  // Default constructor:
  MMClusterOnTrack::MMClusterOnTrack():
    MuonClusterOnTrack(), // call base class ctor
    m_rio(),
    m_detEl(nullptr),
    m_stripDriftDists(std::vector<float>(0)),
    m_stripDriftDistErrors(std::vector<Amg::MatrixX>(0))
  {}

  // copy constructor:
  MMClusterOnTrack::MMClusterOnTrack( const MMClusterOnTrack& rot) = default;

  // Constructor with parameters
  MMClusterOnTrack::MMClusterOnTrack(
           const MMPrepData* RIO,
           const Trk::LocalParameters& locpos,
           const Amg::MatrixX& locerr,
           double positionAlongStrip,
           const std::vector<float>& stripDriftDists,
           const std::vector<Amg::MatrixX>& stripDriftDistErrors) :
    MuonClusterOnTrack(locpos, locerr, RIO->identify(), positionAlongStrip), //call base class constructor
    m_detEl( RIO->detectorElement() ),
    m_stripDriftDists(stripDriftDists),
    m_stripDriftDistErrors(stripDriftDistErrors)
  {
    //Set EL
    // m_rio = ElementLinkToIDC_TGC_Container("TGC_Measurements", RIO->getHashAndIndex().hashAndIndex(), RIO);
    m_rio.setElement(RIO);
  }

  MMClusterOnTrack::MMClusterOnTrack(
           const ElementLinkToIDC_MM_Container& RIO,
           const Trk::LocalParameters& locpos,
           const Amg::MatrixX& locerr,
           const Identifier& id,
           const MuonGM::MMReadoutElement* detEl,
           double positionAlongStrip,
           const std::vector<float>& stripDriftDists,
           const std::vector<Amg::MatrixX>& stripDriftDistErrors) :
    MuonClusterOnTrack(locpos, locerr, id, positionAlongStrip),  // call base class constructor
    m_rio( RIO ),
    m_detEl( detEl ),
    m_stripDriftDists(stripDriftDists),
    m_stripDriftDistErrors(stripDriftDistErrors)
  {
  }

  // Destructor:
  MMClusterOnTrack::~MMClusterOnTrack()
  {
    // we don't own the m_rio object (it belongs to SG), so don't delete it.
  }

  // assignment operator:
  MMClusterOnTrack& MMClusterOnTrack::operator=( const MMClusterOnTrack& rot)
  {
    if ( &rot != this)
      {
        MuonClusterOnTrack::operator=(rot);//base class ass. op.
        m_rio = rot.m_rio;
        m_detEl = rot.m_detEl;
        m_stripDriftDists = rot.m_stripDriftDists;
        m_stripDriftDistErrors = rot.m_stripDriftDistErrors;

      }
    return *this;
  }

  MsgStream& MMClusterOnTrack::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO<<"MMClusterOnTrack {"<<std::endl;

    MuonClusterOnTrack::dump(stream); 

    stream<<"}"<<endmsg;
    return stream;
  }

  std::ostream& MMClusterOnTrack::dump( std::ostream&    stream) const
  {
    stream << "MMClusterOnTrack {"<<std::endl;

    MuonClusterOnTrack::dump(stream); 

    stream<<"}"<<std::endl;
    return stream;
  }


}


