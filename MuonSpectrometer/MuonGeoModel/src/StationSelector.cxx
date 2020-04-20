/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "MuonGeoModel/StationSelector.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/MYSQL.h"
#include <fstream>
#include <sstream>
#include "AthenaKernel/getMessageSvc.h"

namespace MuonGM {

std::atomic<int> StationSelector::m_selectType = 0;


StationSelector::StationSelector(std::string filename)
{
  std::ifstream from;
  from.open(filename.c_str());
  char buffer[200];
  while (from.getline(buffer,200)) {
    std::istringstream line(buffer);
    std::string key;
    line>>key;
    m_selector.push_back(key);
  }

  MYSQL *mysql = MYSQL::GetPointer();
  StationIterator it;
  for (it=mysql->StationBegin(); it!=mysql->StationEnd(); it++) {
    if (select(it)) {
      m_theMap[(*it).first] = (*it).second;
    }
  }
}


StationSelector::StationSelector(std::vector<std::string> s):m_selector(s) {
  MYSQL *mysql = MYSQL::GetPointer();
  StationIterator it;
  for (it=mysql->StationBegin(); it!=mysql->StationEnd(); it++) {
    if (select(it)) m_theMap[(*it).first]=(*it).second;
  }
}


StationIterator StationSelector::begin()
{
  return m_theMap.begin();
}


StationIterator StationSelector::end()
{
  return m_theMap.end();
}


bool StationSelector::select(StationIterator it)
{
  MsgStream log(Athena::getMessageSvc(), "MuonGeoModel");
  std::string name = (*it).second->GetName();
  int selFlag = m_selectType;
  if (m_selector[0]=="*") {
    selFlag = 1;  // override JO choice for general configuration
  }

  if (selFlag==1) {
    for (unsigned int k=0; k<m_selector.size(); k++) {
      std::string sel = m_selector[k];

      if (sel.size()<=name.size()) {
        bool myflag = true;
        for (unsigned int i=0; i<sel.size(); i++) {
          if (sel[i]=='*') continue;
          if (sel[i]!=name[i]) {
            myflag = false;break;
          }
        }

        if (myflag) return myflag;
      }
    }
    return false;
  } else if (selFlag==2) {
    for (unsigned int k=0; k<m_selector.size(); k++) {
      std::string sel = m_selector[k];

      if (sel.size()!=name.size()) continue;   // require exact match

      bool myflag = true;
      for (unsigned int i=0;i<sel.size();i++) {
        if (sel[i]=='*') continue;
        if (sel[i]!=name[i]) {
          myflag = false;break;
        }
      }

      if (myflag) {
        log << MSG::INFO
            << " StationSelector: removing station " << name
            << endmsg;
        return false;
      }

    }

    return true;
  }

  return true;
}

void StationSelector::SetSelectionType(int t)
{
  m_selectType = t;
}

} // namespace MuonGM
