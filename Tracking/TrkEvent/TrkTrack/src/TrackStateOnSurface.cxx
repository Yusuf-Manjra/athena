/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <stdexcept>
#include "TrkTrack/AlignmentEffectsOnTrack.h"

namespace Trk {
TrackStateOnSurface::TrackStateOnSurface()
    :
    m_fitQualityOnSurface(0),
    m_trackParameters(0),
    m_measurementOnTrack(0),
    m_materialEffectsOnTrack(0),
    m_alignmentEffectsOnTrack(0)
{
}

TrackStateOnSurface::TrackStateOnSurface(
    const MeasurementBase          *meas,
    const TrackParameters          *trackParameter,
    const FitQualityOnSurface      *fitQoS,
    const MaterialEffectsBase      *materialEffects,
    const AlignmentEffectsOnTrack       *alignmentEffectsOnTrack
    ):
    m_fitQualityOnSurface(fitQoS),
    m_trackParameters(trackParameter),
    m_measurementOnTrack(meas),
    m_materialEffectsOnTrack( materialEffects ),
    m_alignmentEffectsOnTrack(alignmentEffectsOnTrack)
{
    assert(isSane());
    setFlags();
}

TrackStateOnSurface::TrackStateOnSurface(
    const MeasurementBase* /**meas*/,
    const TrackParameters* /**trackParameter*/,
    const FitQualityOnSurface* /**fitQoS*/,
    const MaterialEffectsBase* /**materialEffects*/,
    const TrackStateOnSurface::TrackStateOnSurfaceType /**type*/
    )
{
    throw "Using deprecated ctor";
}

TrackStateOnSurface::TrackStateOnSurface(
    const MeasurementBase          *meas,
    const TrackParameters          *trackParameter,
    const FitQualityOnSurface      *fitQoS,
    const MaterialEffectsBase      *materialEffects,
    const std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern,
    const AlignmentEffectsOnTrack       *alignmentEffectsOnTrack
    ):
    m_fitQualityOnSurface(fitQoS),
    m_trackParameters(trackParameter),
    m_measurementOnTrack(meas),
    m_materialEffectsOnTrack( materialEffects ),
    m_alignmentEffectsOnTrack( alignmentEffectsOnTrack ),
    m_typeFlags(typePattern)
{
  assert(isSane());
  //setFlags();
}
 
  
TrackStateOnSurface::TrackStateOnSurface
    (
    const MeasurementBase* meas,
    const TrackParameters* trackParameter
    ):
    m_fitQualityOnSurface(0),
    m_trackParameters(trackParameter),
    m_measurementOnTrack(meas),
    m_materialEffectsOnTrack(0),
    m_alignmentEffectsOnTrack(0)
{
    assert(isSane());
    setFlags();
}

TrackStateOnSurface::TrackStateOnSurface(
    const TrackStateOnSurface& rhs
    ):
    m_fitQualityOnSurface(rhs.m_fitQualityOnSurface 
        ? new FitQualityOnSurface(*rhs.m_fitQualityOnSurface) : 0 ),
    m_trackParameters(rhs.m_trackParameters 
        ? rhs.m_trackParameters->clone() : 0),
    m_measurementOnTrack(rhs.m_measurementOnTrack 
        ? rhs.m_measurementOnTrack->clone() : 0),
    m_materialEffectsOnTrack(rhs.m_materialEffectsOnTrack
        ? rhs.m_materialEffectsOnTrack->clone() : 0),
    m_alignmentEffectsOnTrack(rhs.m_alignmentEffectsOnTrack
        ? rhs.m_alignmentEffectsOnTrack : 0),
    m_typeFlags(rhs.m_typeFlags)
{
}

TrackStateOnSurface::~TrackStateOnSurface(){
    delete m_fitQualityOnSurface;
    delete m_trackParameters;
    delete m_measurementOnTrack;
    delete m_materialEffectsOnTrack;
    delete m_alignmentEffectsOnTrack;
}

TrackStateOnSurface& TrackStateOnSurface::operator=(const TrackStateOnSurface& rhs)
{
    if (this!=&rhs){
        delete m_fitQualityOnSurface;
        delete m_trackParameters;
        delete m_measurementOnTrack;
        delete m_materialEffectsOnTrack;
        delete m_alignmentEffectsOnTrack;
        m_fitQualityOnSurface = rhs.m_fitQualityOnSurface 
            ? new FitQualityOnSurface(*rhs.m_fitQualityOnSurface) : 0 ;
        m_trackParameters = rhs.m_trackParameters 
            ? rhs.m_trackParameters->clone() : 0;
        m_measurementOnTrack = rhs.m_measurementOnTrack 
            ? rhs.m_measurementOnTrack->clone() : 0 ; 
        m_materialEffectsOnTrack = rhs.m_materialEffectsOnTrack
            ? rhs.m_materialEffectsOnTrack->clone() : 0;
        m_alignmentEffectsOnTrack = rhs.m_alignmentEffectsOnTrack
            ? rhs.m_alignmentEffectsOnTrack : 0;
        m_typeFlags = rhs.m_typeFlags;
        assert(isSane());
    }
    return *this;
}

std::string TrackStateOnSurface::dumpType() const{
    std::string type;
    if (m_typeFlags.test(TrackStateOnSurface::Measurement))
      type+="Measurement ";
    if (m_typeFlags.test(TrackStateOnSurface::InertMaterial))
      type+="InertMaterial ";
    if (m_typeFlags.test(TrackStateOnSurface::BremPoint))
      type+="BremPoint ";
    if (m_typeFlags.test(TrackStateOnSurface::Scatterer))
      type+="Scatterer ";
    if (m_typeFlags.test(TrackStateOnSurface::Perigee))
      type+="Perigee ";
    if (m_typeFlags.test(TrackStateOnSurface::Outlier))
      type+="Outlier ";    
    if (m_typeFlags.test(TrackStateOnSurface::Hole))
      type+="Hole ";
    if (m_typeFlags.test(TrackStateOnSurface::CaloDeposit))
      type+="CaloDeposit ";
    if (m_typeFlags.test(TrackStateOnSurface::Parameter))
      type+="Parameter ";
    if (m_typeFlags.test(TrackStateOnSurface::FitQuality))
      type+="FitQuality ";
    if (m_typeFlags.test(TrackStateOnSurface::Alignment))
      type+="Alignment ";
    return type;
}

const Surface& 
TrackStateOnSurface::surface() const {
  if (m_trackParameters) return m_trackParameters->associatedSurface();
  if (m_measurementOnTrack) return m_measurementOnTrack->associatedSurface();
  if (m_materialEffectsOnTrack) return m_materialEffectsOnTrack->associatedSurface();
  if (m_alignmentEffectsOnTrack) return m_alignmentEffectsOnTrack->associatedSurface();
  throw std::runtime_error("TrackStateOnSurface without Surface!");
  // const Surface* dummy=0;
  // return *dummy;
}

bool 
TrackStateOnSurface::isSane() const {
  std::vector<const Surface* > surfaces;

  if (m_trackParameters) surfaces.push_back(&(m_trackParameters->associatedSurface()));
  if (m_measurementOnTrack) surfaces.push_back(&(m_measurementOnTrack->associatedSurface()));
  if (m_materialEffectsOnTrack) surfaces.push_back(&(m_materialEffectsOnTrack->associatedSurface()));
  if (m_alignmentEffectsOnTrack) surfaces.push_back(&(m_alignmentEffectsOnTrack->associatedSurface()));

  auto surfaceIt = surfaces.begin();
  bool surfacesDiffer=false;
  while (surfaceIt!=surfaces.end()){
    if (**surfaceIt!=*surfaces[0]) {
      surfacesDiffer=true;
      break;
    }
    surfaceIt++;
  }
  
  if (surfacesDiffer){
    std::cerr<<"TrackStateOnSurface::isSane. Surfaces differ! "<<std::endl;;
    if (m_trackParameters) std::cerr<<"ParamSurf: ["<<&(m_trackParameters->associatedSurface())<<"] "<<m_trackParameters->associatedSurface()<<std::endl;
    if (m_measurementOnTrack) std::cerr<<"measSurf: ["<<&(m_measurementOnTrack->associatedSurface())<<"] "<<m_measurementOnTrack->associatedSurface()<<std::endl;
    if (m_materialEffectsOnTrack) std::cerr<<"matSurf: ["<<&(m_materialEffectsOnTrack->associatedSurface())<<"] "<<m_materialEffectsOnTrack->associatedSurface()<<std::endl;
    if (m_alignmentEffectsOnTrack) std::cerr<<"alignSurf: ["<<&(m_alignmentEffectsOnTrack->associatedSurface())<<"] "<<m_alignmentEffectsOnTrack->associatedSurface()<<std::endl;
    return false;
  }
 
  return true;
}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const TrackStateOnSurface& tsos)
{ 
    std::string name("TrackStateOnSurface: ");
    sl <<name<<"\t of type : "<<tsos.dumpType()<<endreq;
    //write out "type" of object
    
    if (sl.level()<MSG::INFO) 
    {
        sl<<name<<"Detailed dump of contained objects follows:"<<endreq;
        if ( tsos.fitQualityOnSurface()!=0) 
            sl << *(tsos.fitQualityOnSurface() )<<endreq<<" (end of FitQualityOnSurface dump)"<<endreq;
    
        if ( tsos.trackParameters() !=0) 
            sl << *(tsos.trackParameters() )<<endreq<<" (end of TrackParameters dump)"<<endreq;
    
        if ( tsos.measurementOnTrack()!=0) 
            sl << *(tsos.measurementOnTrack() )<<endreq<<" (end of MeasurementBase dump"<<endreq;
    
        if (tsos.materialEffectsOnTrack()!=0) 
            sl << *(tsos.materialEffectsOnTrack() )<<endreq<<" (end of MaterialEffectsBase dump)"<<endreq;
        
        if (tsos.alignmentEffectsOnTrack()!=0) 
            sl << *(tsos.alignmentEffectsOnTrack() )<<endreq<<" (end of AlignmentEffectsOnTrack dump)"<<endreq;
    }
    return sl; 
}

std::ostream& operator << ( std::ostream& sl, const TrackStateOnSurface& tsos)
{
    std::string name("TrackStateOnSurface: ");
    sl <<name<<"\t of type : "<<tsos.dumpType()<<std::endl;
    
    if ( tsos.fitQualityOnSurface()!=0) 
    {
        sl <<"\t HAS FitQualityOnSurface(s)."<<std::endl;
        sl <<"\t \t"<< *(tsos.fitQualityOnSurface() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO FitQualityOnSurfaces."<<std::endl;
    }

    if ( tsos.trackParameters()!=0) 
    {
        sl <<"\t HAS TrackParameter(s)."<<std::endl;
        sl <<"\t \t"<< *(tsos.trackParameters() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO TrackParameters."<<std::endl;
    }

    if (tsos.measurementOnTrack()!=0) 
    {
        sl <<"\t HAS MeasurementBase(s)."<<std::endl;
        sl <<"\t \t"<< *(tsos.measurementOnTrack() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO MeasurementBase."<<std::endl;
    }

    if (tsos.materialEffectsOnTrack()!=0) 
    {
        sl <<"\t HAS MaterialEffectsBase."<<std::endl;
        sl <<"\t \t"<< *(tsos.materialEffectsOnTrack() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO MaterialEffects."<<std::endl;
    }   /**return sl; don't return here, the next code becomes dead**/
    
    if (tsos.alignmentEffectsOnTrack()!=0) 
    {
        sl <<"\t HAS AlignmentEffectsOnTrack."<<std::endl;
        sl <<"\t \t"<< *(tsos.alignmentEffectsOnTrack() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO AlignmentEffectsOnTrack."<<std::endl;
    }   return sl;
}
}//namespace Trk
