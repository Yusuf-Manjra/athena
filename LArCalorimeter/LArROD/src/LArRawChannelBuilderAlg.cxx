/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArRawChannelBuilderAlg.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArIdentifier/LArOnlineID.h"
#include <cmath>

LArRawChannelBuilderAlg::LArRawChannelBuilderAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm(name, pSvcLocator) {}
  
StatusCode LArRawChannelBuilderAlg::initialize() {
  ATH_CHECK(m_digitKey.initialize());	 
  ATH_CHECK(m_rawChannelKey.initialize());
  ATH_CHECK(m_pedestalKey.initialize());	 
  ATH_CHECK(m_adc2MeVKey.initialize());	 
  ATH_CHECK(m_ofcKey.initialize());	 
  ATH_CHECK(m_shapeKey.initialize());

  ATH_CHECK(detStore()->retrieve(m_onlineId,"LArOnlineID"));

  return StatusCode::SUCCESS;
}     

StatusCode LArRawChannelBuilderAlg::finalize() {
  return StatusCode::SUCCESS;
} 

StatusCode LArRawChannelBuilderAlg::execute_r(const EventContext& ctx) const {

  //Get event inputs from read handles:
  SG::ReadHandle<LArDigitContainer> inputContainer(m_digitKey,ctx);

  //Write output via write handle
  SG::WriteHandle<LArRawChannelContainer>outputContainer(m_rawChannelKey,ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<LArRawChannelContainer>()));
	    
  //Get Conditions input
  SG::ReadCondHandle<ILArPedestal> pedHdl(m_pedestalKey,ctx);
  const ILArPedestal* peds=*pedHdl;

  SG::ReadCondHandle<LArADC2MeV> adc2mevHdl(m_adc2MeVKey,ctx);
  const LArADC2MeV* adc2MeVs=*adc2mevHdl;

  SG::ReadCondHandle<ILArOFC> ofcHdl(m_ofcKey,ctx);
  const ILArOFC* ofcs=*ofcHdl;

  SG::ReadCondHandle<ILArShape> shapeHdl(m_shapeKey,ctx);
  const ILArShape* shapes=*shapeHdl;


  //Loop over digits:
  for (const LArDigit* digit : *inputContainer) {
    const std::vector<short>& samples=digit->samples();
    const size_t nSamples=samples.size();
    const HWIdentifier id=digit->hardwareID();
    const int gain=digit->gain();

    const float p=peds->pedestal(id,gain);
    //The following autos will resolve either into vectors or vector-proxies
    const auto& ofca=ofcs->OFC_a(id,gain);
    const auto& ofcb=ofcs->OFC_b(id,gain);
    const auto& adc2mev=adc2MeVs->ADC2MEV(id,gain);
    
    //Sanity check on input conditions data:
    if(ATH_UNLIKELY(ofca.size()!=nSamples)) {
      ATH_MSG_ERROR("Number of OFC a's doesn't mach number of samples for channel " << m_onlineId->channel_name(id) 
		    << " gain " << gain << ". Got " << ofca.size() << ", expected " << nSamples);
      return StatusCode::FAILURE;
    }
    
    if (ATH_UNLIKELY(p==ILArPedestal::ERRORCODE)) {
      ATH_MSG_ERROR("No valid pedestal for channel " << m_onlineId->channel_name(id) 
		    << " gain " << gain);
      return StatusCode::FAILURE;
    }


    //Apply OFCs to get amplitude and time
    float A=0;
    float At=0;
    bool saturated=false;
    for (size_t i=0;i<nSamples;++i) {
      A+=(samples[i]-p)*ofca[i];
      At+=(samples[i]-p)*ofcb[i];
      if (samples[i]==4096 || samples[i]==0) saturated=true;
    }
    
    const float tau=At/A;
    
    //Apply Ramp
    const float E=adc2mev[0]+A*adc2mev[1];
    
    uint16_t iquaShort=0;

    uint16_t prov=0xa5; //Means all constants from DB
    if (saturated) prov|=0x0400;

    if (std::fabs(E)>m_eCutForQ) {
      prov|=0x2000;
      //Get Q-factor
      const auto& shape=shapes->Shape(id,gain);
      if (ATH_UNLIKELY(shape.size()!=nSamples)) {
	ATH_MSG_ERROR("No valid shape for channel " <<  m_onlineId->channel_name(id) 
		      << " gain " << gain);
	return StatusCode::FAILURE;
      }

      float q=0;
      if (m_useShapeDer) {
	const auto& shapeDer=shapes->ShapeDer(id,gain);
	if (ATH_UNLIKELY(shapeDer.size()!=nSamples)) {
	  ATH_MSG_ERROR("No valid shape derivative for channel " <<  m_onlineId->channel_name(id) 
			<< " gain " << gain);
	  return StatusCode::FAILURE;
	}
	for (size_t i=0;i<nSamples;++i) {
	  q += std::pow((A*(shape[i]-tau*shapeDer[i])-(samples[i]-p)),2);
	}
      }//end if useShapeDer
      else {
	//Q-factor w/o shape derivative
	for (size_t i=0;i<nSamples;++i) {
	  q += std::pow((A*shape[i]-(samples[i]-p)),2);
	}
      }



      int iqua = static_cast<int>(q);
      if (iqua > 0xFFFF) iqua=0xFFFF;
      iquaShort = static_cast<uint16_t>(iqua & 0xFFFF);
    }
   

    const float timeOffset = ofcs->timeOffset(id,gain);
    
    const float time=(tau-timeOffset)*(Gaudi::Units::nanosecond/
				       Gaudi::Units::picosecond); //Convert time to ps

    outputContainer->emplace_back(id,(int)(floor(E+0.5)),(int)floor(time+0.5),iquaShort,prov,(CaloGain::CaloGain)gain);

  }
  return StatusCode::SUCCESS;
}


