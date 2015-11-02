/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MYSQL.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"
#include <sstream>
#include <iostream>
#include <cassert>

#include "MuonGeoModel/Technology.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"


namespace MuonGM {

MYSQL* MYSQL::thePointer=0;

MYSQL::MYSQL() : m_includeCutoutsBog(0), m_includeCtbBis(0), m_controlAlines(0)
{
    m_geometry_version = "unknown";
    m_layout_name      = "unknown";
    m_amdb_version = 0;
    m_nova_version = 0;
    m_amdb_from_rdb = false;
    IMessageSvc* msgSvc = Athena::getMessageSvc();
    m_MsgStream = new MsgStream(msgSvc ,"MuonGeoModel_MYSQL");
    for(unsigned int i=0; i<NTgcReadouts; i++) tgcReadout[i]=NULL;

}
    
MYSQL::~MYSQL()
{
    //delete stations 
  std::map<std::string,Station*>::const_iterator it;
    for (it=stations.begin();it!=stations.end();it++)
    {
        delete (*it).second;
    }
    //delete technologies
    std::map<std::string,Technology*>::const_iterator it1;
    for (it1=technologies.begin();it1!=technologies.end();it1++)
    {
        delete (*it1).second;
    }
    delete m_MsgStream;
    m_MsgStream = 0;
    // reset the pointer so that at next initialize the MYSQL object will be re-created
    thePointer = 0;
}

MYSQL* MYSQL::GetPointer()
{
	if (!thePointer) {
            thePointer=new MYSQL;
	    //            std::cout<<MSG::INFO<<"MYSQL singleton created from scratch at location <"<<thePointer<<std::endl;
        }
	return thePointer;
}

Station* MYSQL::GetStation(std::string name)
{
  if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<" looking for station "<<name<<endreq;
  std::map<std::string, Station* >::const_iterator it= stations.find(name);
  if (it!=stations.end())
    {
      if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<"found the station"<<endreq;
      //      std::cout<<" station with old method "<<stations[name]->GetName()<<" new method "<<(it->second)->GetName()<<std::endl;
      return it->second;

    }
  else return 0;
}

Position MYSQL::GetStationPosition(std::string nameType, int fi, int zi)
{
    Position p;
    if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<" MYSQL::GetStationPosition for "<<nameType<<" fi/zi "<<fi<<" "<<zi<<endreq;
    int subtype = allocPosFindSubtype(nameType, fi, zi);
    std::string stname = nameType+MuonGM::buildString(subtype, 0);
    Station* st = GetStation(stname);
    if ( st != NULL ) {
        if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<" found in Station "<<st->GetName();
        p =  (*(st->FindPosition(zi, fi))).second;
        if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<" at p.fi,zi "<<p.phiindex<<" "<<p.zindex<<" shift/z "<<p.shift<<" "<<p.z<<endreq;
    }
    else
    {
      reLog()<<MSG::WARNING<<"::GetStationPosition nothing found for "
	 <<nameType<<" at fi/zi "<<fi<<" "<<zi<<endreq;
    }
    return p;
}


TgcReadoutParams* MYSQL::GetTgcRPars(std::string name)
{
   if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<"MYSQL::GetTgcRPars looking for a TgcRPars named <"<<name<<">"<<endreq;

   std::map<std::string, TgcReadoutParams* >::const_iterator it =tgcReadouts.find(name);
    if (it!=tgcReadouts.end())
    {
        return it->second;
    }
    else return NULL;
}

TgcReadoutParams* MYSQL::GetTgcRPars(int jsta)
{
  if (jsta-1<0 || jsta>=NTgcReadouts) {
    reLog()<<MSG::ERROR<<"MYSQL::GetTgcRPars jsta = "<<jsta<<" out of range (0,"<<NTgcReadouts-1<<")"<<endreq;
    return NULL;
  }  
  return tgcReadout[jsta-1];
}

Technology* MYSQL::GetTechnology(std::string name)
{
  std::map<std::string,Technology* >::const_iterator it =  technologies.find(name);
	if (it!=technologies.end())
	{
	  if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<"found the station technology name "<<name<<endreq;
	  return it->second;
	}
	else 
	{
	  if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<"MYSQL:: Technology "<<name<<"+++++++++ not found!"<<endreq;
	  return 0;
	}
}

void MYSQL::StoreTechnology(Technology* t)
{
  if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<"MYSQL::StoreTechnology /// techn. named "<<t->GetName()<<endreq;
  std::map<std::string,Technology* >::const_iterator it = technologies.find(t->GetName());
  if (it!=technologies.end())
    {
      reLog()<<MSG::ERROR
	 <<"MYSQL::StoreTechnology ERROR /// This place is already taken !!! for "
	 <<t->GetName()<<endreq;
        assert(0);
    }
    else technologies[t->GetName()]=t;
}

void MYSQL::StoreStation(Station* s)
{
  if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<"name "<<s->GetName()<<endreq;
  stations[s->GetName()]=s;
}
void MYSQL::StoreTgcRPars(TgcReadoutParams* s)
{
  if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<"MYSQL::StoreTgcRPars named "<<s->GetName()<<" located @ "<<s<<" jsta = "<<s->chamberType()<<endreq;
  if ( s->chamberType() >= NTgcReadouts)
  {
    reLog()<<MSG::ERROR<<"MYSQL::StoreTgcRPars ChamberType(JSTA) "<<s->chamberType() <<" > NTgcReadouts="<<NTgcReadouts<<endreq;
    return;
  }
  tgcReadout[s->chamberType()-1]=s;
}


void MYSQL::PrintAllStations()
{
	std::map<std::string,Station*>::const_iterator it;
	for (it=stations.begin();it!=stations.end();it++)
	{
		std::string key=(*it).first;
		//Station *s=(*it).second;
                std::cout<<"---> Station  "<<key<<std::endl;
	}
}

void MYSQL::PrintTechnologies()
{
	std::map<std::string,Technology* >::const_iterator it;
	for (it=technologies.begin();it!=technologies.end();it++)
	{
		std::string key=(*it).first;
                std::cout<<"---> Technology "<<key<<std::endl;
	}
}
Technology* MYSQL::GetATechnology(std::string name)
{
  std::map<std::string,Technology* >::const_iterator it = technologies.find(name);
    if (it!=technologies.end())
    {
      if (reLog().level()<=MSG::VERBOSE)  reLog()<<MSG::VERBOSE<<"found the station technology name "<<name<<endreq;
      return it->second;
    }
    else 
    {
        if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<"MYSQL:: Technology "<<name<<"+++++++++ not found!"<<endreq;
        for (unsigned int i=1; i<=20; i++)
        {
            char chindex[3];
            sprintf(chindex,"%i",i);
            //std::string newname = name.substr(0,3)+chindex;
            std::string newname = name.substr(0,3)+MuonGM::buildString(i,2);
	    it = technologies.find(newname);
            if (it!=technologies.end()) {
	      if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<" Selecting a technology called <"<<newname
		 		<<">"<<endreq;
                return it->second;
            }
        }
        return 0;
    }
}

std::string MYSQL::allocPosBuildKey(std::string statType, int fi, int zi)
{
    std::ostringstream mystream;
    mystream << statType << "fi" << MuonGM::buildString(fi, 1) << "zi" << MuonGM::buildString(zi, -1);
    if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<" from "<<statType <<" fi "<<  fi <<" zi "<< zi <<" we get as key "<<mystream.str()<<std::endl;
    return mystream.str();
}

allocPosIterator MYSQL::allocPosFind(std::string statType, int fi, int zi)
{
    std::string key = allocPosBuildKey(statType, fi, zi);
    return allocPosFind(key);
}

int MYSQL::allocPosFindSubtype(std::string statType, int fi, int zi)
{
    std::string key = allocPosBuildKey(statType, fi, zi);
    return allocPosFindSubtype(key);
}

int MYSQL::allocPosFindCutout(std::string statType, int fi, int zi)
{
    std::string key = allocPosBuildKey(statType, fi, zi);
    return allocPosFindCutout(key);
}

void MYSQL::addallocPos(std::string statType, int fi, int zi, int subtyp, int cutout)
{
    std::string key = allocPosBuildKey(statType, fi, zi);
    addallocPos(key, subtyp, cutout);
}

} // namespace MuonGM
