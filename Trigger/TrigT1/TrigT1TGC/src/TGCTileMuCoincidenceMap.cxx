/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include "TrigT1TGC/TGCTileMuCoincidenceMap.hh"
#include "TrigT1TGC/TGCDatabaseManager.hh"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"


namespace LVL1TGCTrigger {

TGCTileMuCoincidenceMap::TGCTileMuCoincidenceMap(TGCArguments* tgcargs,
						 const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                                                 const std::string& version)
  :m_verName(version),
   m_tgcArgs(tgcargs),
   m_readCondKey(readCondKey)
{
  // intialize map
  for (size_t side=0; side< N_Side; side++){
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	for (size_t input=0; input< N_Input_TileMuModule; input++){
	  m_map[input][ssc][sec][side] = TM_NA;
	}
	m_flagPT[0][ssc][sec][side]  =0; //pt1     
	m_flagPT[1][ssc][sec][side]  =0; //pt2     
	m_flagPT[2][ssc][sec][side]  =0; //pt3     
	m_flagPT[3][ssc][sec][side]  =0; //pt4     
	m_flagPT[4][ssc][sec][side]  =1; //pt5     
	m_flagPT[5][ssc][sec][side]  =1; //pt6     
	for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	  m_flagROI[pos][ssc][sec][side]  = 1;
	}
      }
    }
  }
   
  if (!tgcArgs()->TILE_MU()) return;
  if (tgcArgs()->USE_CONDDB()) return;

  //////////////////////////////
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return;
  }
  MsgStream log(msgSvc, "TGCTileMuCoincidenceMap::TGCTileMuCoincidenceMap");

  // read Inner Coincidence Map 
  if (this->readMap()) {
    log << MSG::INFO 
      << " TGC TileMu CW version of " << m_verName << " is selected " << endmsg;
  } else {
    log << MSG::INFO  
	<< " NOT use TileMu " << endmsg;
    tgcArgs()->set_TILE_MU( false );
    for (size_t side=0; side< N_Side; side++){
      for (size_t sec=0; sec< N_EndcapSector; sec++){
	for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	  m_flagPT[0][ssc][sec][side]  =0; //pt1     
	  m_flagPT[1][ssc][sec][side]  =0; //pt2     
	  m_flagPT[2][ssc][sec][side]  =0; //pt3     
	  m_flagPT[3][ssc][sec][side]  =0; //pt4     
	  m_flagPT[4][ssc][sec][side]  =0; //pt5     
	  m_flagPT[5][ssc][sec][side]  =0; //pt6     
	}
      }    
    }
  }

  // for debug  
  //dumpMap();
}


TGCTileMuCoincidenceMap::~TGCTileMuCoincidenceMap()
{
}

TGCTileMuCoincidenceMap::TGCTileMuCoincidenceMap(const TGCTileMuCoincidenceMap& right)
  :m_verName(right.m_verName),
   m_readCondKey(right.m_readCondKey)
{
  for (size_t side=0; side< N_Side; side++){
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	for (size_t input=0; input< N_Input_TileMuModule; input++){
	  m_map[input][ssc][sec][side] = right.m_map[input][ssc][sec][side];
	}
	m_flagPT[0][ssc][sec][side] = right.m_flagPT[0][ssc][sec][side];
	m_flagPT[1][ssc][sec][side] = right.m_flagPT[1][ssc][sec][side];
	m_flagPT[2][ssc][sec][side] = right.m_flagPT[2][ssc][sec][side];
	m_flagPT[3][ssc][sec][side] = right.m_flagPT[3][ssc][sec][side];
	m_flagPT[4][ssc][sec][side] = right.m_flagPT[4][ssc][sec][side];
	m_flagPT[5][ssc][sec][side] = right.m_flagPT[5][ssc][sec][side];
	for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	  m_flagROI[pos][ssc][sec][side] = right.m_flagROI[pos][ssc][sec][side];
	}
      }
    }
  }
}

TGCTileMuCoincidenceMap& TGCTileMuCoincidenceMap::operator=(const TGCTileMuCoincidenceMap& right)
{
  if (this != &right) {
    for (size_t side=0; side< N_Side; side++){
      for (size_t sec=0; sec< N_EndcapSector; sec++){
	for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	  for (size_t input=0; input< N_Input_TileMuModule; input++){
	    m_map[input][ssc][sec][side] = right.m_map[input][ssc][sec][side];
	  }
	  m_flagPT[0][ssc][sec][side] = right.m_flagPT[0][ssc][sec][side];
	  m_flagPT[1][ssc][sec][side] = right.m_flagPT[1][ssc][sec][side];
	  m_flagPT[2][ssc][sec][side] = right.m_flagPT[2][ssc][sec][side];
	  m_flagPT[3][ssc][sec][side] = right.m_flagPT[3][ssc][sec][side];
	  m_flagPT[4][ssc][sec][side] = right.m_flagPT[4][ssc][sec][side];
	  m_flagPT[5][ssc][sec][side] = right.m_flagPT[5][ssc][sec][side];
	  for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	    m_flagROI[pos][ssc][sec][side] = right.m_flagROI[pos][ssc][sec][side];
	  }
	}
      }
    }
    m_verName = right.m_verName;
  }
  return  *this;
}

bool TGCTileMuCoincidenceMap::readMap() 
{
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return false;
  }
  MsgStream log(msgSvc, "TGCTileMuCoincidenceMap::TGCTileMuCoincidenceMap");

  // select right database according to a set of thresholds
  std::string dbname="";
  dbname = "TileMuCoincidenceMap." + m_verName + "._12.db";

  //----- 
  
  std::string fullName;
  fullName = PathResolver::find_file( dbname.c_str(), "DATAPATH" );
  bool isFound =( fullName.length() > 0 );
  if( !isFound) {
    log << MSG::WARNING 
	<< " Could not found " << dbname << endmsg;
    return false ;  
  } 

  std::ifstream file(fullName.c_str(),std::ios::in);    

  enum{BufferSize=1024};
  char buf[BufferSize];
  std::string tag;

  while(file.getline(buf,BufferSize)){
    int sideId = -1;
    int sectorId = -1;
    int sscId    = -1;
    int use[N_PT_THRESH] = {0, 0, 0, 0, 0, 0};
    int roi[N_ROI_IN_SSC] = {1, 1, 1, 1, 1, 1, 1, 1};
    std::istringstream header(buf); 
    header >> tag;
    if(tag=="#"){ // read header part.     
      header >> sideId >> sectorId >> sscId 
	     >> use[0] >> use[1] >> use[2] 
	     >> use[3] >> use[4] >> use[5] 
	     >> roi[0] >> roi[1] >> roi[2] >> roi[3]
	     >> roi[4] >> roi[5] >> roi[6] >> roi[7];
    }
    // check Id
    if( sideId<0   || sideId>=N_Side ||
        sectorId<0 || sectorId>=N_EndcapSector ||
	sscId<0    || sscId>=N_Endcap_SSC ) {
      log << MSG::WARNING 
	  << " illegal parameter in database header : " << header.str()
	  << " in file " << dbname
	  << endmsg;
      file.close();
      return false;
    }
    for (size_t pt=0; pt<N_PT_THRESH; pt++){
      m_flagPT[pt][sscId][sectorId][sideId]   = use[pt];
    }
    for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
      m_flagROI[pos][sscId][sectorId][sideId] = roi[pos];
    }

    // get trigger word
    file.getline(buf,BufferSize);
    std::istringstream cont(buf);
    unsigned int word;
    for(size_t pos=0; pos<N_Input_TileMuModule; pos++){
      cont >> word;
      m_map[pos][sscId][sectorId][sideId] = word;
    }
  }
  file.close();	  

  return true;
}


void TGCTileMuCoincidenceMap::dumpMap() const
{
  // select right database according to a set of thresholds
  std::string fullName="TileMuCoincidenceMap."+m_verName+"._12.out";

  std::ofstream file(fullName.c_str());    

  for (size_t side=0; side< N_Side; side++){
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	file << "# " << side << " " << sec << " " << ssc << " "
	     << m_flagPT[0][ssc][sec][side] << " "
	     << m_flagPT[1][ssc][sec][side]  << " "
	     << m_flagPT[2][ssc][sec][side]  << " "
	     << m_flagPT[3][ssc][sec][side]  << " "
	     << m_flagPT[4][ssc][sec][side]  << " "
	     << m_flagPT[5][ssc][sec][side]  << " "
	     << m_flagROI[0][ssc][sec][side]  << " "
	     << m_flagROI[1][ssc][sec][side]  << " "
	     << m_flagROI[2][ssc][sec][side]  << " "
	     << m_flagROI[3][ssc][sec][side]  << " "
	     << m_flagROI[4][ssc][sec][side]  << " "
	     << m_flagROI[5][ssc][sec][side]  << " "
	     << m_flagROI[6][ssc][sec][side]  << " "
	     << m_flagROI[7][ssc][sec][side]  << " "
	     << std::endl;
	file << m_map[0][ssc][sec][side]  << " "
	     << m_map[1][ssc][sec][side]  << " "
	     << m_map[2][ssc][sec][side]  << " "
	     << m_map[3][ssc][sec][side]  << " "
	     << std::endl;
      }
    }
  }
  file.close();	  
}

int  TGCTileMuCoincidenceMap::getMask(const int module,
                                       const int ssc,
                                       const int sec,
                                       const int side ) const
{
  if ((module<0)||(module>=TGCTriggerData::N_TILE_INPUT)) return TM_NA;
  if ((ssc<0)||(ssc>=TGCTriggerData::N_ENDCAP_SSC)) return TM_NA;
  if ((sec<0)||(sec>=TGCTriggerData::N_ENDCAP_SECTOR)) return TM_NA;
  if ((side<0)||(side>=TGCTriggerData::N_SIDE)) return TM_NA;

  if  (tgcArgs()->USE_CONDDB()) {
    SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
    const TGCTriggerData* readCdo{*readHandle};
    return readCdo->getTrigMaskTile(module, ssc, sec, side);
  } else {
    return  m_map[module][ssc][sec][side];
  }
}

int  TGCTileMuCoincidenceMap::getFlagPT(const int pt,
                                         const int ssc,
                                         const int sec,
                                         const int side)  const
{
  if ((pt<=0)||(pt>N_PT_THRESH)) return -1;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return -1;
  if ((sec<0)||(sec>=N_EndcapSector)) return -1;
  if ((side<0)||(side>=N_Side)) return -1;

  if  (tgcArgs()->USE_CONDDB()) {
    SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
    const TGCTriggerData* readCdo{*readHandle};
    return readCdo->getFlagPtTile(pt, ssc, sec, side);
  } else {
    return  m_flagPT[pt-1][ssc][sec][side];
  }
}

int  TGCTileMuCoincidenceMap::getFlagROI(const int roi,
                                          const int ssc,
                                          const int sec,
                                          const int side)  const
{
  if ((roi<0)||(roi>=N_ROI_IN_SSC)) return -1;
  if ((ssc<0)||(ssc>=N_Endcap_SSC)) return 0;
  if ((sec<0)||(sec>=N_EndcapSector)) return -1;
  if ((side<0)||(side>=N_Side)) return -1;

  if  (tgcArgs()->USE_CONDDB()) {
    SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
    const TGCTriggerData* readCdo{*readHandle};
    return readCdo->getFlagRoiTile(roi, ssc, sec, side);
  } else {
    return  m_flagROI[roi][ssc][sec][side];
  }
}

} //end of namespace bracket
