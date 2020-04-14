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
  AthAlgTool(t,n,p),
  m_magFieldSvc("AtlasFieldSvc",n)
{
  declareInterface<INSWCalibTool>(this);

  declareProperty("MagFieldSvc",   m_magFieldSvc, "Magnetic Field Service");
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
  ATH_CHECK(m_magFieldSvc.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());

  if ( !(m_idHelperSvc->hasMM() && m_idHelperSvc->hasSTgc() ) ) {
    ATH_MSG_ERROR("MuonIdHelperTool not properly configured, missing MM or STGC");
    return StatusCode::FAILURE;
  }
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

  Amg::Vector3D magneticField;

  m_magFieldSvc->getField(&globalPos,&magneticField);
  
  double lorentzAngle = m_lorentzAngleFunction->Eval((magneticField.y() > 0. ? 1. : -1.) * std::fabs (magneticField.y()) ) * toRad; // in radians;

  vDriftCorrected = m_vDrift * std::cos(lorentzAngle);
  calibStrip.distDrift = vDriftCorrected * calibStrip.time;
  calibStrip.resTransDistDrift = pitchErr + std::pow(m_transDiff * calibStrip.distDrift, 2); 
  calibStrip.resLongDistDrift = std::pow(m_ionUncertainty * vDriftCorrected, 2) + std::pow(m_longDiff * calibStrip.distDrift, 2);
  calibStrip.dx = std::sin(lorentzAngle) * calibStrip.time * m_vDrift;

  return StatusCode::SUCCESS;
}


StatusCode Muon::NSWCalibTool::finalize()
{
  ATH_MSG_DEBUG("In finalize()");
  m_lorentzAngleFunction->Delete();
  return StatusCode::SUCCESS;
}
