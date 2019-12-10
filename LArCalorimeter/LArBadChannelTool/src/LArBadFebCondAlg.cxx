/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadFebCondAlg.h"
#include "LArBadChannelTool/LArBadChanBlobUtils.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArBadChannelTool/LArBadChannelDecoder.h"


LArBadFebCondAlg::LArBadFebCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_BCInputKey("/LAR/BadFebsOfl/BadFebs"),
  m_BCOutputKey("LArBadFeb","LArBadFeb"),
  m_condSvc("CondSvc",name)
{
  declareProperty("ReadKey",m_BCInputKey);
  declareProperty("WriteKey",m_BCOutputKey);
  declareProperty("InputFileName",m_inputFileName="");
}

LArBadFebCondAlg::~LArBadFebCondAlg() {}


StatusCode LArBadFebCondAlg::initialize() {

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );
  // Read Handles
  if(!m_BCInputKey.key().empty()) ATH_CHECK( m_BCInputKey.initialize() );
  ATH_CHECK( m_BCOutputKey.initialize() );

  // Register write handle
  if (m_condSvc->regHandle(this, m_BCOutputKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_BCOutputKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode LArBadFebCondAlg::execute() {
    
  SG::WriteCondHandle<LArBadFebCont> writeHandle{m_BCOutputKey};
  
  if (writeHandle.isValid()) {
    msg(MSG::DEBUG) << "Found valid write handle" << endmsg;
    return StatusCode::SUCCESS;
  }  

  std::unique_ptr<LArBadFebCont> badFebCont(new LArBadFebCont());
  EventIDRange rangeW;

  if(!m_BCInputKey.key().empty()) {

    SG::ReadCondHandle<AthenaAttributeList> readHandle{m_BCInputKey};
    const AthenaAttributeList* attrList{*readHandle};

    if (attrList==nullptr) {
      msg(MSG::ERROR) << "Failed to retrieve CondAttributeListCollection with key " << m_BCInputKey.key() << endmsg;
      return StatusCode::FAILURE;
    }
 
    
    const coral::Blob& blob = (*attrList)["Blob"].data<coral::Blob>();
    unsigned int chanSize = (*attrList)["ChannelSize"].data<unsigned int>();
    unsigned int stateSize = (*attrList)["StatusWordSize"].data<unsigned int>();
    unsigned int endian = (*attrList)["Endianness"].data<unsigned int>();
    unsigned int version = (*attrList)["Version"].data<unsigned int>();
      
    std::vector<std::pair<HWIdentifier,LArBadFeb> > bcVec = 
      LArBadChanBlobUtils::decodeBlob<LArBadFeb>( &blob, chanSize, stateSize, endian,
          					version, msg());
     
    for (auto& idBC : bcVec) {
      badFebCont->add(idBC.first,idBC.second);
    }
    //
    // Define validity of the output cond object and record it
    if(!readHandle.range(rangeW)) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }
  } else {

    EventIDBase start(0, 0);
    EventIDBase stop(std::numeric_limits<unsigned int>::max()-1,0);
    start.set_lumi_block(0);
    stop.set_lumi_block(std::numeric_limits<unsigned int>::max()-1);
    rangeW=EventIDRange( start, stop );

  }

  if (m_inputFileName.size()) {//Read supplemental data from ASCII file (if required)
     
     const LArOnlineID* onlineID;
     ATH_CHECK(detStore()->retrieve(onlineID,"LArOnlineID"));	       
     LArBadChannelDecoder decoder(&(*onlineID), msg());
     std::vector<std::pair<HWIdentifier,LArBadFeb> > bcVec = decoder.readFebASCII(m_inputFileName);
     for (auto& idBC : bcVec) {
       badFebCont->add(idBC.first,idBC.second);
     }
   } //end if have ASCII filename
 
 
 
   size_t nChanBeforeMege=badFebCont->size();
   badFebCont->sort(); //Sorts vector of bad febs and merges duplicate entries
   
   ATH_MSG_INFO("Read a total of " << badFebCont->size() << " problematic febs from database");
   if (nChanBeforeMege!=badFebCont->size()) {
     ATH_MSG_INFO("Merged " << nChanBeforeMege-badFebCont->size() << " duplicate entries");
   }
 
  if(writeHandle.record(rangeW,badFebCont.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record LArBadFebCont object with " 
		  << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
