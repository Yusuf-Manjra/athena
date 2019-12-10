/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_Hid2RESrcID.h" 
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "eformat/SourceIdentifier.h" 

#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"

using eformat::helper::SourceIdentifier; 

// implementation of RpcPad to RESrcID conversion. 
// Ketevi A. Assamagan
// Jan-16-2003, BNL
// 
// adapted for MdtCms by Michela Biglietti <biglietti@na.infn.it>


MDT_Hid2RESrcID::MDT_Hid2RESrcID() :
  m_muonIdHelperTool(nullptr), m_specialROBNumber(0), m_readKey("MuonMDT_CablingMap")
{

}

StatusCode MDT_Hid2RESrcID::set(const Muon::MuonIdHelperTool* muonIdHelperTool) {
  // Initialize the cabling Service
  m_muonIdHelperTool = muonIdHelperTool;
  ATH_CHECK( m_readKey.initialize() ); 
  return StatusCode::SUCCESS; 
}

uint32_t MDT_Hid2RESrcID::getRodID(const Identifier& offlineId) {
    
  // this method returns a RESrcID for the ROD, for a 
  // given MDT ID offline ID

  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
  if (sc==StatusCode::FAILURE)  
      throw std::runtime_error( "Unable to get MessageSvc");
  
 MsgStream log(msgSvc, "MDT_Hid2RESrcID::getRodID");  

  uint8_t  SubsystemId = 0;
  uint8_t  MrodId = 0;
  uint8_t  LinkId = 0;
  uint8_t  TdcId = 0;
  uint8_t  ChannelId = 0;

  log << MSG::DEBUG << "Getting RODId of the Station " << MSG::hex << offlineId << MSG::dec
      << endmsg;

  bool online;
  int station_name = m_muonIdHelperTool->mdtIdHelper().stationName(offlineId);
  int station_eta = m_muonIdHelperTool->mdtIdHelper().stationEta(offlineId);
  int station_phi = m_muonIdHelperTool->mdtIdHelper().stationPhi(offlineId);
  int multilayer = m_muonIdHelperTool->mdtIdHelper().multilayer(offlineId);
  int tubelayer = m_muonIdHelperTool->mdtIdHelper().tubeLayer(offlineId);
  int tube = m_muonIdHelperTool->mdtIdHelper().tube(offlineId);

  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
  const MuonMDT_CablingMap* readCdo{*readHandle};
  if(readCdo==nullptr){
    //ATH_MSG_ERROR("Null pointer to the read conditions object");
    log << MSG::ERROR << "Null pointer to the read conditions object" << endmsg;
    return 0;
  }
  online = readCdo->getOnlineId(station_name, station_eta,
				station_phi, multilayer,tubelayer,
				tube, 
				SubsystemId,
				MrodId,
				LinkId,
				TdcId,
				ChannelId);
  
  if (!online) {
    log << MSG::DEBUG << "ROD Id of the Station " << MSG::hex << "0x" << offlineId
	<< " not found" << endmsg;
    MrodId=0xff;
  }
  else {
    log << MSG::DEBUG << "The ROD ID of the station " 
	<< "is " << MSG::hex << "0x" << MrodId << MSG::dec << endmsg;
  }
  
  eformat::SubDetector detid = (eformat::SubDetector) SubsystemId;
  
  //if (SubsystemId == eformat::MUON_MDT_BARREL_A_SIDE) {
  //  detid = eformat::MUON_MDT_BARREL_A_SIDE;
  //}
  //else if (SubsystemId == eformat::MUON_MDT_BARREL_C_SIDE) {
  //  detid = eformat::MUON_MDT_BARREL_C_SIDE;
  //}
  //else if (SubsystemId == eformat::MUON_MDT_ENDCAP_A_SIDE) {
  //  detid = eformat::MUON_MDT_ENDCAP_A_SIDE;
  //}
  //else if (SubsystemId == eformat::MUON_MDT_ENDCAP_C_SIDE) {
  //  detid = eformat::MUON_MDT_ENDCAP_C_SIDE;
  //}
  
  // uint8_t m = (uint8_t) MrodId;
  SourceIdentifier sid(detid,MrodId); 
  
  uint32_t rod_id =  sid.code();

  return rod_id;
}

/** mapping SrcID from ROD to ROB
 */ 
uint32_t MDT_Hid2RESrcID::getRobID( uint32_t rod_id) {

  SourceIdentifier id = SourceIdentifier(rod_id);
  SourceIdentifier id2 = SourceIdentifier(id.subdetector_id(),id.module_id());
  //return id2.code();

  // FIXME ! the rob ID in the first data taken is wrong !
  //return 0x610003;

  // After data will be fixed the correct version should be:
  return rod_id;
}

/** mapping SrcID from ROB to ROS
 */ 
uint32_t MDT_Hid2RESrcID::getRosID( uint32_t rob_id) {
  //  Change Module Type to ROS, moduleid = 0  
  
  SourceIdentifier id (rob_id);
  SourceIdentifier id2(id.subdetector_id(),0);
  return id2.code(); 
}

  /** mapping SrcID from ROS to Det
   */ 
uint32_t MDT_Hid2RESrcID::getDetID  ( uint32_t ros_id) {
  //  ROS to DET
  
  SourceIdentifier id (ros_id);
  SourceIdentifier id2(id.subdetector_id(),0);
  return id2.code(); 
}




