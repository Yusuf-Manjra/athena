/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <map>
#include "MuonCondData/MdtDeadTubeStatusContainer.h"
#include "MuonCondData/MdtDeadTubeStatus.h"
#include "MuonCondInterface/IMDT_DeadTubeConditionsTool.h" 
#include "MuonCondTool/MDT_DeadTubeConditionsTool.h"
#include "MuonCondTool/MDT_MapConversion.h"
#include "MuonCondSvc/MdtStringUtils.h"

//#include "RDBAccessSvc/IRDBAccessSvc.h"
//#include "RDBAccessSvc/IRDBRecord.h"
//#include "RDBAccessSvc/IRDBRecordset.h"
//#include "GeoModelSvc/IGeoModelSvc.h"
//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT DeadTube Info from COOL DB
//* retrieving of tables from DB 
//*********************************************************


MDT_DeadTubeConditionsTool::MDT_DeadTubeConditionsTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_condMapTool("MDT_MapConversion"), 
    log( msgSvc(), name ),
    m_debug(false),
    m_verbose(false)  
{
  
  declareInterface< IMDT_DeadTubeConditionsTool >(this);
  
  //  m_tubeStatusDataLocation="TubeStatusKey";
  
  declareProperty("DSCAnalysis",     m_dcsAnalysis=true);
  declareProperty("DeadTubeFolder",     m_deadtubeFolder="/MDT/TUBE_STATUS/DEAD_TUBE");
}


//StatusCode MDT_DeadTubeConditionsTool::updateAddress(SG::TransientAddress* /*tad*/)
StatusCode MDT_DeadTubeConditionsTool::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* /*tad*/)
{
  MsgStream log(msgSvc(), name());
  // CLID clid        = tad->clID();
  //std::string key  = tad->name();
  //if ( 228145 == clid && m_tubeStatusDataLocation == key)
  //  {
  //    log << MSG::DEBUG << "OK Tube Status" << endreq;
  //    return StatusCode::SUCCESS;
  //  }
  return StatusCode::FAILURE;
}
	


StatusCode MDT_DeadTubeConditionsTool::initialize()
{
  log.setLevel(outputLevel());
  m_debug = log.level() <= MSG::DEBUG;
  m_verbose = log.level() <= MSG::VERBOSE;

  log << MSG::INFO << "Initializing - folders names are: ChamberDropped "<<m_deadtubeFolder << endreq;
   
  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
     if( m_debug ) log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;
  }else{
    log << MSG::ERROR << "Failed to retrieve DetectorStore" << endreq;
    return sc;
  }
  
  
  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  sc = service( "IOVSvc", m_IOVSvc, CREATEIF );
  if ( sc.isFailure() )
    {
      log << MSG::ERROR << "Unable to get the IOVSvc" << endreq;
      return StatusCode::FAILURE;
    }
  
  if(sc.isFailure()) return StatusCode::FAILURE;
  
  
  
  // initialize the chrono service
  sc = service("ChronoStatSvc",m_chronoSvc);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Could not find the ChronoSvc" << endreq;
    return sc;
  }
	
    	 
  if(sc.isFailure()) return StatusCode::FAILURE;

  sc = m_detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (sc.isFailure())
    {
      log << MSG::FATAL << " Cannot retrieve MdtIdHelper " << endreq;
      return sc;
    }
  
  sc = m_condMapTool.retrieve();
  if ( sc.isFailure() )
    {
      log << MSG::ERROR << "Could not retrieve MDT_MapConversion" << endreq;
      return sc;
    }
  else
    {
      log<<MSG::INFO<<"MDT_MapConversion retrieved with statusCode = "<<sc<<" pointer = "<<m_condMapTool<<endreq;
    
    }
     
  return StatusCode::SUCCESS;
}

StatusCode MDT_DeadTubeConditionsTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  log.setLevel(outputLevel());
  m_debug = log.level() <= MSG::DEBUG;
  m_verbose = log.level() <= MSG::VERBOSE;
	 
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    log << MSG::INFO <<"LoadParameters "<< *itr << " I="<<I<<" "<<endreq;
    if(*itr==m_deadtubeFolder) {
      StatusCode sc = loadDeadTube(I,keys);
      if (sc.isFailure())
	{
	  return sc;
	}
    }

  }
  
  return StatusCode::SUCCESS;
}

 
	
StatusCode MDT_DeadTubeConditionsTool::loadDeadTube(IOVSVC_CALLBACK_ARGS_P(I,keys))
{


  log.setLevel(outputLevel());
  m_debug = log.level() <= MSG::DEBUG;
  m_verbose = log.level() <= MSG::VERBOSE;

  StatusCode sc=StatusCode::SUCCESS;
  log << MSG::INFO << "Load Dropped tube from  Chamber from DB" << endreq;
  
  // Print out callback information
   if( m_debug ) log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt)  if( m_debug ) log << MSG::DEBUG << *keyIt << " ";
   if( m_debug ) log << MSG::DEBUG << endreq;
 
  
	
  const CondAttrListCollection * atrc;
  log << MSG::INFO << "Try to read from folder <"<<m_deadtubeFolder<<">"<<endreq;
  
   sc=m_detStore->retrieve(atrc,m_deadtubeFolder);
   if(sc.isFailure())  {
     log << MSG::ERROR
 	<< "could not retreive the CondAttrListCollection from DB folder " 
 	<< m_deadtubeFolder << endreq;
     return sc;
 	  }

   else
     log<<MSG::INFO<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endreq;
  
 
  CondAttrListCollection::const_iterator itr;
 
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
 
    const coral::AttributeList& atr=itr->second;
    
    std::string chamber_name;
    std::string dead_tube;
    std::string tube_list;
    dead_tube=*(static_cast<const std::string*>((atr["DeadTube_List"]).addressOfData()));
    chamber_name=*(static_cast<const std::string*>((atr["Chamber_Name"]).addressOfData()));
    
    if( m_verbose ) log << MSG::VERBOSE << "Sequence load is \n" << chamber_name <<" tubes dead :  " <<dead_tube<<endreq;

    std::string mylist;
    std::vector<std::string> tokens;
    std::string delimiter = " ";
    MuonCalib::MdtStringUtils::tokenize(dead_tube,tokens,delimiter);
    Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
    for (unsigned int i=0; i<tokens.size(); i++) {
 
      int ml = atoi(const_cast<char*>((tokens[i].substr(0,1)).c_str()));
      int layer= atoi(const_cast<char*>((tokens[i].substr(1,2)).c_str()));
      int tube= atoi(const_cast<char*>((tokens[i].substr(2)).c_str()));
      mylist+=chamber_name+"_"+tokens[i];
      tube_list = tokens[i]+".";
      Identifier ChannelId =m_mdtIdHelper->channelID(ChamberId,ml,layer,tube); 
      m_cachedDeadTubesId.push_back(ChannelId);
    }
    
    Tube_Map.insert(std::make_pair(ChamberId,tube_list));
    if( m_verbose ) log << MSG::VERBOSE << "Sequence load is \n" << mylist<< endreq;
    m_Chamber_with_deadTube.push_back(ChamberId);
    m_cachedDeadTubes.push_back(mylist);
  }
  std::sort(m_cachedDeadTubesId.begin(),m_cachedDeadTubesId.end());   
//  for(unsigned int k=0;k<m_cachedDeadTubes.size();k++){
//     std::string Id = (m_cachedDeadTubes[k]);
//     log<< MSG::INFO << "writing from Tool SERVICE TUBES \n" << Id<<endreq;
//   }
  
  return StatusCode::SUCCESS;
}

