/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSiliconConditionsTestAlg.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

PixelSiliconConditionsTestAlg::PixelSiliconConditionsTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
}

StatusCode PixelSiliconConditionsTestAlg::initialize()
{  
  ATH_MSG_INFO("Calling initialize");

  ATH_CHECK(m_moduleDataKey.initialize());
  ATH_CHECK(m_readKeyTemp.initialize());
  ATH_CHECK(m_readKeyHV.initialize());
  ATH_CHECK(m_lorentzAngleTool.retrieve());
  ATH_CHECK(m_chargeDataKey.initialize());
  ATH_CHECK(m_distortionKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelSiliconConditionsTestAlg::execute(){

  SG::ReadCondHandle<PixelDCSHVData> hv(m_readKeyHV);
  SG::ReadCondHandle<PixelDCSTempData> temp(m_readKeyTemp);
  SG::ReadCondHandle<PixelModuleData> deadmap(m_moduleDataKey);
  SG::ReadCondHandle<PixelChargeCalibCondData> calib(m_chargeDataKey);
  SG::ReadCondHandle<PixelDistortionData> distortion(m_distortionKey);

  for (int i=0; i<2048; i++) { 
    ATH_MSG_DEBUG("Module " << i << " " 
                            << hv->getBiasVoltage(i) << " " 
                            << temp->getTemperature(i) << " " 
                            << deadmap->getModuleStatus(i) << " " 
                            << m_lorentzAngleTool->getLorentzShift(IdentifierHash(i)));
    for (int j=0; j<16; j++) {
      ATH_MSG_DEBUG("FE " << j << " "
                          << calib->getAnalogThreshold(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getAnalogThresholdSigma(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getAnalogThresholdNoise(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getInTimeThreshold(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getAnalogThreshold(i,j,PixelChargeCalibCondData::LONG) << " " 
                          << calib->getAnalogThresholdSigma(i,j,PixelChargeCalibCondData::LONG) << " " 
                          << calib->getAnalogThresholdNoise(i,j,PixelChargeCalibCondData::LONG) << " " 
                          << calib->getInTimeThreshold(i,j,PixelChargeCalibCondData::LONG) << " " 
                          << calib->getAnalogThreshold(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getAnalogThresholdSigma(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getAnalogThresholdNoise(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getInTimeThreshold(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getQ2TotA(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getQ2TotE(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getQ2TotC(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getQ2TotA(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getQ2TotE(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getQ2TotC(i,j,PixelChargeCalibCondData::GANGED));
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelSiliconConditionsTestAlg::finalize(){
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
