/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawCalibDataReadingAlg.h"
#include "LArIdentifier/LArOnlineID.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "LArRawEvent/LArCalibDigitContainer.h"
#include "LArRawEvent/LArAccumulatedDigitContainer.h"
#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "eformat/Version.h"
#include "eformat/index.h"

#include "LArByteStream/LArRodBlockStructure.h"
#include "LArByteStream/LArRodBlockAccumulatedV3.h"
#include "LArByteStream/LArRodBlockCalibrationV3.h"
#include "LArByteStream/LArRodBlockTransparentV0.h"

LArRawCalibDataReadingAlg::LArRawCalibDataReadingAlg(const std::string& name, ISvcLocator* pSvcLocator) :  
  AthReentrantAlgorithm(name, pSvcLocator) {}

  StatusCode LArRawCalibDataReadingAlg::initialize() {

  if (m_calibDigitKey.key().size()>0) {
    ATH_CHECK(m_calibDigitKey.initialize());
    m_doCalibDigits=true;
  }
  else {
    m_doCalibDigits=false;
  }

  if (m_accDigitKey.key().size()>0) {
    ATH_CHECK(m_accDigitKey.initialize());
    m_doAccDigits=true;
  }
  else {
    m_doAccDigits=false;
  }

  if (m_accCalibDigitKey.key().size()>0) {
    ATH_CHECK(m_accCalibDigitKey.initialize());
    m_doAccCalibDigits=true;
  }
  else {
    m_doAccCalibDigits=false;
  }

  if (m_febHeaderKey.key().size()>0) {
    ATH_CHECK(m_febHeaderKey.initialize());
    m_doFebHeaders=true;
  }
  else {
    m_doFebHeaders=false;
  }
  
  if(!(m_doCalibDigits || m_doAccDigits || m_doAccCalibDigits)) {
     ATH_MSG_FATAL("Needs ether CalibDigits or AccDigits  or AccCalibDigit Key");
     return StatusCode::FAILURE;
  }

  if(m_doCalibDigits && m_doAccCalibDigits) {
     ATH_MSG_FATAL("Could not have both CalibDigits, AccCalibDigits Key");
     return StatusCode::FAILURE;
  }

  if(m_doAccDigits && (m_doCalibDigits || m_doAccCalibDigits)) {
     ATH_MSG_FATAL("Could not have AccDigits with Calib Key");
     return StatusCode::FAILURE;
  }

  ATH_CHECK(m_robDataProviderSvc.retrieve());
  ATH_CHECK(detStore()->retrieve(m_onlineId,"LArOnlineID"));  
  return StatusCode::SUCCESS;
}     
  
StatusCode LArRawCalibDataReadingAlg::execute(const EventContext& ctx) const {
  LArCalibDigitContainer* cdigits=nullptr;
  LArAccumulatedDigitContainer* accdigits=nullptr;
  LArAccumulatedCalibDigitContainer* caccdigits=nullptr;
  LArFebHeaderContainer* febHeaders=nullptr;

  if (m_doCalibDigits) {
    SG::WriteHandle<LArCalibDigitContainer> cdigitsHdl(m_calibDigitKey,ctx);
    ATH_CHECK(cdigitsHdl.record(std::make_unique<LArCalibDigitContainer>()));
    cdigits=cdigitsHdl.ptr();
    cdigits->reserve(200000); //Enough space for the full calo
  }

  if (m_doAccDigits) {
    SG::WriteHandle<LArAccumulatedDigitContainer> accdigitsHdl(m_accDigitKey,ctx);
    ATH_CHECK(accdigitsHdl.record(std::make_unique<LArAccumulatedDigitContainer>()));
    accdigits=accdigitsHdl.ptr();
    accdigits->reserve(200000); //Enough space for the full calo
  }

  if (m_doAccCalibDigits) {
    SG::WriteHandle<LArAccumulatedCalibDigitContainer> caccdigitsHdl(m_accCalibDigitKey,ctx);
    ATH_CHECK(caccdigitsHdl.record(std::make_unique<LArAccumulatedCalibDigitContainer>()));
    caccdigits=caccdigitsHdl.ptr();
    caccdigits->reserve(200000); //Enough space for the full calo
  }

  if (m_doFebHeaders) {
    SG::WriteHandle<LArFebHeaderContainer> febHeadersHdl(m_febHeaderKey,ctx);
    ATH_CHECK(febHeadersHdl.record(std::make_unique<LArFebHeaderContainer>()));
    febHeaders=febHeadersHdl.ptr();
    febHeaders->reserve(1524); //Total number of LAr Front End Boards
  }

  //Get full events and filter out LAr ROBs
  const RawEvent* fullEvent=m_robDataProviderSvc->getEvent(ctx);
  std::map<eformat::SubDetectorGroup, std::vector<const uint32_t*> > rawEventTOC;
  eformat::helper::build_toc(*fullEvent, rawEventTOC);
  auto larRobs=rawEventTOC.find(eformat::LAR);
  if (larRobs==rawEventTOC.end()) {
     ATH_MSG_DEBUG("No LAr data found in this event.");
     return StatusCode::SUCCESS;
  } 
  
  
  std::unique_ptr<LArRodBlockStructure> rodBlock;
  uint16_t rodMinorVersion=0x0;
  uint32_t rodBlockType=0x0;


  for (const uint32_t* robPtr : larRobs->second) {
    OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment rob(robPtr);
    ATH_MSG_VERBOSE("Decoding ROB fragment 0x" << std::hex << rob.rob_source_id () << " with " << std::dec << rob.rod_fragment_size_word() << "ROB words");

    if (rob.rod_fragment_size_word() <3) {
      ATH_MSG_ERROR("Encountered corrupt ROD fragment, less than 3 words!");
      if (m_failOnCorruption) {
	return StatusCode::FAILURE;
      }else 
	continue;
    }
    
     eformat::helper::Version ver(rob.rod_version());
    //(re-)init rodBlock only once per event or if (very unlikly or even impossible) some FEBs have a differnt firmware
    if (rodBlock==nullptr || rodMinorVersion !=ver.minor_version() || rodBlockType!=(rob.rod_detev_type()&0xff)) {
      rodMinorVersion=ver.minor_version();
      rodBlockType=rob.rod_detev_type()&0xff;
      ATH_MSG_VERBOSE("Found version " << rodMinorVersion <<  " of Rod Block Type  " <<  rodBlockType);
      if (rodBlockType==10) { // Accumulated calib. digits
	  rodBlock.reset(new LArRodBlockAccumulatedV3);
      }//end of rodBlockType ==10
      else if (rodBlockType==7 || rodBlockType==2) { // Calib. digits
         if(rodMinorVersion>=6) {
            rodBlock.reset(new LArRodBlockCalibrationV3);
         } else {
            ATH_MSG_ERROR("Found unsupported ROD Block version " << rodMinorVersion
                        << " of ROD block type " << rodBlockType);
            return m_failOnCorruption ? StatusCode::FAILURE : StatusCode::SUCCESS;
         }
      } else {
	ATH_MSG_ERROR("Found unsupported Rod block type " << rodBlockType);
	return m_failOnCorruption ? StatusCode::FAILURE : StatusCode::SUCCESS;
      }
    }//End if need to re-init RodBlock

    const uint32_t* pData=rob.rod_data();
    const uint32_t  nData=rob.rod_ndata();
    if (!rodBlock->setFragment(pData,nData)) {
      ATH_MSG_ERROR("Failed to assign fragment pointer to LArRodBlockStructure");
      return StatusCode::FAILURE;
    }

    if(m_verifyChecksum) {
      const uint32_t onsum  = rodBlock->onlineCheckSum();
      const uint32_t offsum = rodBlock->offlineCheckSum();
      if(onsum!=offsum) {
	ATH_MSG_ERROR("Checksum error:");
	ATH_MSG_ERROR("online checksum  = 0x" << MSG::hex << onsum);
	ATH_MSG_ERROR("offline checksum = 0x" << MSG::hex << offsum << MSG::dec);
	if (m_failOnCorruption) 
	  return StatusCode::FAILURE;
	else
	  continue; //Jump to the next ROD-block
      }
    }

    //Loop over FEBs in ROD:
    do {
      HWIdentifier fId(Identifier32(rodBlock->getFEBID()));
      if (!m_onlineId->isValidId(fId)) {
	ATH_MSG_ERROR("Invalid FEB identifer 0x" << std::hex << fId.get_identifier32().get_compact()); 
	if (m_failOnCorruption) 
	  return StatusCode::FAILURE;
	else
	  continue;
      }
      const int NthisFebChannel=m_onlineId->channelInSlotMax(fId);

      //Decode LArCalibDigits (if requested)
      if (m_doCalibDigits) {
	uint32_t gain;
        uint16_t dac;
        uint16_t delay;
        bool ispulsed;
	int fcNb;
	std::vector<short> samples;
	while (rodBlock->getNextRawData(fcNb,samples,gain)) {
	  if (fcNb>=NthisFebChannel)
	    continue;
	  if (samples.size()==0) continue; // Ignore missing cells
          dac = rodBlock->getDAC();
          delay = rodBlock->getDelay();
          ispulsed = rodBlock->getPulsed(fcNb);
	  HWIdentifier cId = m_onlineId->channel_Id(fId,fcNb);
	  cdigits->emplace_back(new LArCalibDigit(cId, (CaloGain::CaloGain)gain, std::move(samples), dac, delay, ispulsed));
	  samples.clear();
	}//end getNextRawData loop
      }//end if m_doCalibDigits

      //Decode LArAccumulatedDigits (if requested)
      if (m_doAccDigits && rodBlockType==10) {
	uint32_t gain;
	int fcNb;
	std::vector<uint32_t> samplesSum;
	std::vector<uint32_t> samples2Sum;
        uint32_t nTrigger;
	while (rodBlock->getNextAccumulatedDigit(fcNb,samplesSum,samples2Sum,gain)) {
	  if (fcNb>=NthisFebChannel)
	    continue;
	  if (samplesSum.size()==0 || samples2Sum.size()==0) continue; // Ignore missing cells
          nTrigger = rodBlock->getNTrigger();
	  HWIdentifier cId = m_onlineId->channel_Id(fId,fcNb);
	  accdigits->emplace_back(new LArAccumulatedDigit(cId, (CaloGain::CaloGain)gain, std::move(samplesSum), std::move(samples2Sum), nTrigger));
	  samplesSum.clear();
	  samples2Sum.clear();
	}//end getNext loop
      }//end if m_doAccDigits

      //Decode LArAccumulatedCalibDigits (if requested)
      if (m_doAccCalibDigits) {
	uint32_t gain;
	uint32_t itmp;
        uint16_t dac;
        uint16_t delay;
        uint16_t nstep;
        uint16_t istep;
        bool ispulsed;
	int fcNb;
	std::vector<uint32_t> samplesSum;
	std::vector<uint32_t> samples2Sum;
        uint32_t nTrigger;
	while (rodBlock->getNextAccumulatedCalibDigit(fcNb,samplesSum,samples2Sum,itmp,gain)) {
	  if (fcNb>=NthisFebChannel)
	    continue;
	  if (samplesSum.size()==0 || samples2Sum.size()==0) continue; // Ignore missing cells
          dac = rodBlock->getDAC();
          delay = rodBlock->getDelay();
          ispulsed = rodBlock->getPulsed(fcNb);
          nTrigger = rodBlock->getNTrigger();
          nstep = rodBlock->getNStep();
          istep = rodBlock->getStepIndex();
	  HWIdentifier cId = m_onlineId->channel_Id(fId,fcNb);
	  caccdigits->emplace_back(new LArAccumulatedCalibDigit(cId, (CaloGain::CaloGain)gain, std::move(samplesSum), std::move(samples2Sum), nTrigger, dac, delay, ispulsed, nstep, istep));
	  samplesSum.clear();
	  samples2Sum.clear();
	}//end getNext loop
      }//end if m_doAccDigits

      //Decode FebHeaders (if requested)
      if (m_doFebHeaders) {
	std::unique_ptr<LArFebHeader> larFebHeader(new LArFebHeader(fId));
	larFebHeader->SetFormatVersion(rob.rod_version());
	larFebHeader->SetSourceId(rob.rod_source_id());
	larFebHeader->SetRunNumber(rob.rod_run_no());
	larFebHeader->SetELVL1Id(rob.rod_lvl1_id());
	larFebHeader->SetBCId(rob.rod_bc_id());
	larFebHeader->SetLVL1TigType(rob.rod_lvl1_trigger_type());
	larFebHeader->SetDetEventType(rob.rod_detev_type());
  
	//set DSP data
	const unsigned nsample=rodBlock->getNumberOfSamples();
	larFebHeader->SetRodStatus(rodBlock->getStatus());
	larFebHeader->SetDspCodeVersion(rodBlock->getDspCodeVersion()); 
	larFebHeader->SetDspEventCounter(rodBlock->getDspEventCounter()); 
	larFebHeader->SetRodResults1Size(rodBlock->getResults1Size()); 
	larFebHeader->SetRodResults2Size(rodBlock->getResults2Size()); 
	larFebHeader->SetRodRawDataSize(rodBlock->getRawDataSize()); 
	larFebHeader->SetNbSweetCells1(rodBlock->getNbSweetCells1()); 
	larFebHeader->SetNbSweetCells2(rodBlock->getNbSweetCells2()); 
	larFebHeader->SetNbSamples(nsample); 
	larFebHeader->SetOnlineChecksum(rodBlock->onlineCheckSum());
	larFebHeader->SetOfflineChecksum(rodBlock->offlineCheckSum());

	if(!rodBlock->hasControlWords()) {
	  larFebHeader->SetFebELVL1Id(rob.rod_lvl1_id());
	  larFebHeader->SetFebBCId(rob.rod_bc_id());
	} else {
	  const uint16_t evtid = rodBlock->getCtrl1(0) & 0x1f;
	  const uint16_t bcid  = rodBlock->getCtrl2(0) & 0x1fff;
	  larFebHeader->SetFebELVL1Id(evtid);
	  larFebHeader->SetFebBCId(bcid);
	  for(int iadc=0;iadc<16;iadc++) {
	    larFebHeader->SetFebCtrl1(rodBlock->getCtrl1(iadc));
	    larFebHeader->SetFebCtrl2(rodBlock->getCtrl2(iadc));
	    larFebHeader->SetFebCtrl3(rodBlock->getCtrl3(iadc));
	  }
	  for(unsigned int i = 0; i<nsample; i++ ) {
	    larFebHeader->SetFebSCA(rodBlock->getRadd(0,i) & 0xff);
	  }
	}//end else no control words
	febHeaders->push_back(std::move(larFebHeader));
      }//end if m_doFebHeaders

    }while (rodBlock->nextFEB()); //Get NextFeb
  } //end loop over ROBs
  return StatusCode::SUCCESS;
}
