/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "NSWCalibTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

namespace {
  static constexpr double const& toRad = M_PI/180;
  static constexpr double const& pitchErr = 0.425 * 0.425 / 12;
  static constexpr double const& reciprocalSpeedOfLight = 1. / Gaudi::Units::c_light; // mm/ns
}

Muon::NSWCalibTool::NSWCalibTool(const std::string& t,
				  const std::string& n, 
				  const IInterface* p ) :
  AthAlgTool(t,n,p)
{
  declareInterface<INSWCalibTool>(this);

  declareProperty("DriftVelocity", m_vDrift = 0.047, "Drift Velocity");
  declareProperty("TimeResolution", m_timeRes = 25., "Time Resolution");
  declareProperty("longDiff",m_longDiff=0.019); //mm/mm
  declareProperty("transDiff",m_transDiff=0.036); //mm/mm
  declareProperty("ionUncertainty",m_ionUncertainty=4.0); //ns
  declareProperty("timeOffset", m_timeOffset = -100); //ns                       
}


StatusCode Muon::NSWCalibTool::initialize()
{

  ATH_MSG_DEBUG("In initialize()");
  ATH_CHECK(m_idHelperSvc.retrieve());
  if ( !(m_idHelperSvc->hasMM() && m_idHelperSvc->hasSTgc() ) ) {
    ATH_MSG_ERROR("MM or STGC not part of initialized detector layout");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_fieldCondObjInputKey.initialize());
  m_lorentzAngleFunction = new TF1("lorentzAngleFunction","[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x",0,2);
  m_lorentzAngleFunction->SetParameters(0,58.87, -2.983, -10.62, 2.818);
  return StatusCode::SUCCESS;

}

StatusCode Muon::NSWCalibTool::calibrate( const Muon::MM_RawData* mmRawData, const Amg::Vector3D& globalPos, NSWCalib::CalibratedStrip& calibStrip) const
{

  double  vDriftCorrected;
  calibStrip.charge = mmRawData->charge();
  calibStrip.time = mmRawData->time() - globalPos.norm() * reciprocalSpeedOfLight + m_timeOffset;

  calibStrip.identifier = mmRawData->identify();

  /// magnetic field
  MagField::AtlasFieldCache fieldCache;
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, Gaudi::Hive::currentContext()};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (!fieldCondObj) {
    ATH_MSG_ERROR("doDigitization: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
    return StatusCode::FAILURE;
  }
  fieldCondObj->getInitializedCache(fieldCache);

  Amg::Vector3D magneticField;
  fieldCache.getField(globalPos.data(), magneticField.data());

  /// get the component parallel to to the eta strips (same used in digitization)
  double phi    = globalPos.phi();
  double bfield = (magneticField.x()*std::sin(phi)-magneticField.y()*std::cos(phi))*1000.;

  /// swap sign depending on the readout side
  int gasGap = m_idHelperSvc->mmIdHelper().gasGap(mmRawData->identify());
  bool changeSign = ( globalPos.z() > 0. ? (gasGap==1 || gasGap==3) : (gasGap==2 || gasGap==4) );
  if (changeSign) bfield = -bfield;

  /// sign of the lorentz angle matches digitization - angle is in radians
  double lorentzAngle = (bfield>0. ? 1. : -1.)*m_lorentzAngleFunction->Eval(std::abs(bfield)) * toRad;

  vDriftCorrected = m_vDrift * std::cos(lorentzAngle);
  calibStrip.distDrift = vDriftCorrected * calibStrip.time;

  /// transversal and longitudinal components of the resolution
  calibStrip.resTransDistDrift = pitchErr + std::pow(m_transDiff * calibStrip.distDrift, 2); 
  calibStrip.resLongDistDrift = std::pow(m_ionUncertainty * vDriftCorrected, 2) 
    + std::pow(m_longDiff * calibStrip.distDrift, 2);
  
  calibStrip.dx = std::sin(lorentzAngle) * calibStrip.time * m_vDrift;

  return StatusCode::SUCCESS;
}


StatusCode Muon::NSWCalibTool::finalize()
{
  ATH_MSG_DEBUG("In finalize()");
  m_lorentzAngleFunction->Delete();
  return StatusCode::SUCCESS;
}
