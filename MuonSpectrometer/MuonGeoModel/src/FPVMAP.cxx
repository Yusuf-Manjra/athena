/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/FPVMAP.h"
#include <iostream>

#include "MuonGeoModel/Technology.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

namespace MuonGM {


FPVMAP::FPVMAP()
{
  m_nreused=0;
}

FPVMAP::~FPVMAP()
{
}

GeoVPhysVol* FPVMAP::GetDetector(std::string name)
{
  if (m_Detectors.find(name)!=m_Detectors.end()) {
    m_nreused++;
    // std::cout<<"FPVMAP:: the pointer to "<<name
    //          <<" is already stored; saving memory "<<m_nreused<<std::endl;
    return m_Detectors[name];
  }
  else return 0;
}

void FPVMAP::StoreDetector(GeoVPhysVol* s, std::string name)
{
  // std::cout<<"FPVMAP:: store the pointer to "<<name<<std::endl;
  m_Detectors[name]=s;
}

void FPVMAP::PrintAllDetectors()
{
  MsgStream log(Athena::getMessageSvc(), "MuonGM::FPVMAP");

  for (DetectorIterator it=m_Detectors.begin();it!=m_Detectors.end();it++) {
    std::string key=(*it).first;
    log << MSG::INFO << "---> A PhysVol corresponds to  " << key << endmsg;
  }
}

} // namespace MuonGM
