/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackStateOnSurfaceCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p3.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"

void TrackStateOnSurfaceCnv_p3::
persToTrans( const Trk::TrackStateOnSurface_p3 *persObj, Trk::TrackStateOnSurface *transObj, MsgStream &log )
{
  ITPConverterFor<Trk::MeasurementBase>	*measureCnv = nullptr;
  const Trk::MeasurementBase* meas = createTransFromPStore( &measureCnv, persObj->m_measurementOnTrack, log );

  ITPConverter* dummy = topConverter ()->converterForType( typeid(Trk::TrackParameters));    
  if (!m_parametersCnv)  m_parametersCnv = dynamic_cast<TrackParametersCnv_p2*>(dummy); // FIXME - only in init?
  const Trk::TrackParameters* trackParameters = dynamic_cast<const Trk::TrackParameters*>(createTransFromPStore( &m_parametersCnv, persObj->m_trackParameters, log ));

  std::unique_ptr<const Trk::FitQuality>  fitQ(createTransFromPStore( &m_fitQCnv, persObj->m_fitQualityOnSurface, log));
  auto fitQos = fitQ ? Trk::FitQualityOnSurface(*fitQ) : Trk::FitQualityOnSurface{};

  ITPConverterFor<Trk::MaterialEffectsBase> *matBaseCnv = nullptr;  
  const Trk::MaterialEffectsBase* materialEffects = createTransFromPStore( &matBaseCnv, persObj->m_materialEffects, log );

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> types;
  std::bitset<Trk::TrackStateOnSurface::NumberOfPersistencyHints> hints;
  Trk::TrackStateOnSurface::splitToBitsets(persObj->m_typeFlags, types, hints);
  // There were some tracks saved that violate the isSane test in
  // TrackStateOnSurface.  If we were to pass meas or materialEffects
  // to this ctor then we would trip that assertion.  However,
  // we want to preserve the previous behavior of the TP converters,
  // which did allow reading  such tracks.  So defer setting these pointers
  // until after the checks,
  *transObj = Trk::TrackStateOnSurface(
    fitQos,
    nullptr,
    std::unique_ptr<const Trk::TrackParameters>(trackParameters),
    nullptr,
    types,
    hints);
  transObj->m_measurementOnTrack.reset(meas);
  transObj->m_materialEffectsOnTrack.reset(materialEffects);
}


void TrackStateOnSurfaceCnv_p3::
transToPers( const Trk::TrackStateOnSurface *transObj, Trk::TrackStateOnSurface_p3 *persObj, MsgStream &log ) 
{
  //--- Parameters
  ITPConverter* dummy = topConverter ()->converterForType( typeid(Trk::TrackParameters));    
  if (!m_parametersCnv)  m_parametersCnv = dynamic_cast<TrackParametersCnv_p2*>(dummy); // FIXME - only in init?
  
  std::bitset<Trk::TrackStateOnSurface::NumberOfPersistencyHints> persHints = transObj->hints();
  bool persistify_all = !(persHints.test(Trk::TrackStateOnSurface::PartialPersistification));
 
  persObj->m_trackParameters = toPersistent( &m_parametersCnv,
                                             ( (persistify_all || persHints.test(Trk::TrackStateOnSurface::PersistifyTrackParameters) ) 
                                               ? transObj->trackParameters()
                                               : nullptr),
                                             log );

  auto fitQos =
    persistify_all
      ? std::make_unique<Trk::FitQuality>(transObj->fitQualityOnSurface())
      : nullptr;

  persObj->m_fitQualityOnSurface = toPersistent(&m_fitQCnv, fitQos.get(), log);

  ITPConverterFor<Trk::MeasurementBase>  *measureCnv = nullptr;
  persObj->m_measurementOnTrack = toPersistent( &measureCnv,
                                                ((persistify_all || persHints.test(Trk::TrackStateOnSurface::PersistifyMeasurement) )
                                                 ? transObj->measurementOnTrack()
                                                 : nullptr),
                                                log );

  ITPConverterFor<Trk::MaterialEffectsBase> *matBaseCnv = nullptr;
  // @TODO create slimmed material effects on track object
  persObj->m_materialEffects = toPersistent( &matBaseCnv,
                                             ((persistify_all ||  persHints.test(Trk::TrackStateOnSurface::PersistifySlimCaloDeposit))
                                              ? transObj->materialEffectsOnTrack()
                                              : nullptr), log );
  if (persistify_all) {
    persObj->m_typeFlags =
      Trk::TrackStateOnSurface::joinBitsets(transObj->types(), persHints);
  } else {
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>
      typePattern;
    if ((persistify_all ||
         persHints.test(Trk::TrackStateOnSurface::PersistifyTrackParameters)) &&
        transObj->type(Trk::TrackStateOnSurface::Perigee)) {
      typePattern.set(Trk::TrackStateOnSurface::Perigee);
    }
    if (persistify_all ||
        persHints.test(Trk::TrackStateOnSurface::PersistifyMeasurement)) {
      if (transObj->type(Trk::TrackStateOnSurface::Measurement)) {
        typePattern.set(Trk::TrackStateOnSurface::Measurement);
      }
      if (transObj->type(Trk::TrackStateOnSurface::Outlier)) {
        typePattern.set(Trk::TrackStateOnSurface::Outlier);
      }
    }
    persObj->m_typeFlags = typePattern.to_ulong();
  }
}

void
MultiComponentStateOnSurfaceCnv_p1::persToTrans(
  const Trk::TrackStateOnSurface_p3* persObj,
  Trk::MultiComponentStateOnSurface* transObj,
  MsgStream& log)
{
  ITPConverter* dummy =
    topConverter()->converterForType(typeid(Trk::TrackStateOnSurface));
  if (!m_trackStateOnSurfaceCnv){
    m_trackStateOnSurfaceCnv = dynamic_cast<TrackStateOnSurfaceCnv_p3*>(dummy);
  }
  m_trackStateOnSurfaceCnv->persToTrans(persObj, transObj, log);
}
void
MultiComponentStateOnSurfaceCnv_p1::transToPers(
  const Trk::MultiComponentStateOnSurface* transObj,
  Trk::TrackStateOnSurface_p3* persObj,
  MsgStream& log)
{
  ITPConverter* dummy =
    topConverter()->converterForType(typeid(Trk::TrackStateOnSurface));
  if (!m_trackStateOnSurfaceCnv){
    m_trackStateOnSurfaceCnv = dynamic_cast<TrackStateOnSurfaceCnv_p3*>(dummy);
  }
  m_trackStateOnSurfaceCnv->transToPers(transObj, persObj, log);
}

