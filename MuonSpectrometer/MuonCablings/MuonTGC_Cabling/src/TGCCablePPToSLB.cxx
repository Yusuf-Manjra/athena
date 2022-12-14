/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCCablePPToSLB.h"

#include "MuonTGC_Cabling/TGCDatabasePPToSL.h" 
#include "MuonTGC_Cabling/TGCChannelPPOut.h"
#include "MuonTGC_Cabling/TGCChannelSLBIn.h"
#include "MuonTGC_Cabling/TGCModulePP.h"
#include "MuonTGC_Cabling/TGCModuleSLB.h"

namespace MuonTGC_Cabling {

// Constructor & Destructor
TGCCablePPToSLB::TGCCablePPToSLB(const std::string& filename)
  : TGCCable(TGCCable::PPToSLB)
{
  m_database[TGCIdBase::Endcap][TGCIdBase::WT] = 
    new TGCDatabasePPToSL(filename,"PP EWT");
  m_database[TGCIdBase::Endcap][TGCIdBase::WD] =
    new TGCDatabasePPToSL(filename,"PP EWD");
  m_database[TGCIdBase::Endcap][TGCIdBase::ST] = 
    new TGCDatabasePPToSL(filename,"PP EST");
  m_database[TGCIdBase::Endcap][TGCIdBase::SD] = 
    new TGCDatabasePPToSL(filename,"PP ESD");
  m_database[TGCIdBase::Endcap][TGCIdBase::WI] = 
    new TGCDatabasePPToSL(filename,"PP EWI");
  m_database[TGCIdBase::Endcap][TGCIdBase::SI] = 
    new TGCDatabasePPToSL(filename,"PP ESI");
  m_database[TGCIdBase::Forward][TGCIdBase::WT] =
    new TGCDatabasePPToSL(filename,"PP FWT");
  m_database[TGCIdBase::Forward][TGCIdBase::WD] = 
    new TGCDatabasePPToSL(filename,"PP FWD");
  m_database[TGCIdBase::Forward][TGCIdBase::ST] = 
    new TGCDatabasePPToSL(filename,"PP FST");
  m_database[TGCIdBase::Forward][TGCIdBase::SD] = 
    new TGCDatabasePPToSL(filename,"PP FSD");
  m_database[TGCIdBase::Forward][TGCIdBase::WI] = 
    new TGCDatabasePPToSL(filename,"PP FWI");
  m_database[TGCIdBase::Forward][TGCIdBase::SI] = 
    new TGCDatabasePPToSL(filename,"PP FSI");
}

TGCCablePPToSLB::~TGCCablePPToSLB(void)
  {
    delete m_database[TGCIdBase::Endcap][TGCIdBase::WT];
    delete m_database[TGCIdBase::Endcap][TGCIdBase::WD];
    delete m_database[TGCIdBase::Endcap][TGCIdBase::ST];
    delete m_database[TGCIdBase::Endcap][TGCIdBase::SD];
    delete m_database[TGCIdBase::Endcap][TGCIdBase::WI];
    delete m_database[TGCIdBase::Endcap][TGCIdBase::SI];
    delete m_database[TGCIdBase::Forward][TGCIdBase::WT];
    delete m_database[TGCIdBase::Forward][TGCIdBase::WD];
    delete m_database[TGCIdBase::Forward][TGCIdBase::ST];
    delete m_database[TGCIdBase::Forward][TGCIdBase::SD];
    delete m_database[TGCIdBase::Forward][TGCIdBase::WI];
    delete m_database[TGCIdBase::Forward][TGCIdBase::SI];
  }
  

TGCChannelId* TGCCablePPToSLB::getChannel(const TGCChannelId* channelId,
					  bool orChannel) const {
  if(channelId){
    if(channelId->getChannelIdType()==TGCIdBase::PPOut)
      return getChannelOut(channelId,orChannel);
    if(channelId->getChannelIdType()==TGCIdBase::SLBIn)
      return getChannelIn(channelId,orChannel);
  }
  return nullptr;
}

TGCModuleMap* TGCCablePPToSLB::getModule(const TGCModuleId* moduleId) const {
  if(moduleId){
    if(moduleId->getModuleIdType()==TGCModuleId::PP)
      return getModuleOut(moduleId);
    if(moduleId->getModuleIdType()==TGCModuleId::SLB)
      return getModuleIn(moduleId);
  }
  return nullptr;
}

TGCChannelId* TGCCablePPToSLB::getChannelIn(const TGCChannelId* slbin,
					    bool orChannel) const {
  if(orChannel) return nullptr;
  if(slbin->isValid()==false) return nullptr;
  TGCChannelPPOut* ppout = nullptr;

  // SLB channel
  const TGCChannelSLBIn* slbIn = nullptr;
  if(slbin->getChannelIdType()==TGCIdBase::SLBIn){
    slbIn = dynamic_cast<const TGCChannelSLBIn*>(slbin);
  } 
  if(!slbIn) {
    return nullptr;
  }

  // SLB module
  TGCModuleId* slb = slbIn->getModule();
  if(!slb) return nullptr;

  // SLB -> PP module connection
  TGCModuleMap* mapId = getModule(slb);
  delete slb;
  if(!mapId) return nullptr;

  // PP module
  int port = mapId->connector(0);
  TGCModuleId* pp = mapId->popModuleId(0);
  delete mapId;
  if(!pp) return nullptr;

  // SLB ->PP channel connection
  int block = -1;
  int channel = -1;
  TGCChannelSLBIn::CellType cellType = slbIn->getCellType();
  if(cellType==TGCChannelSLBIn::NoCellType){
    delete pp;
    return nullptr;
  }

  if(cellType==TGCChannelSLBIn::CellA) {
    block = 1+2*port;
    int lengthOfSLB = TGCChannelSLBIn::getLengthOfSLB(slbIn->getModuleType(),
						      TGCChannelSLBIn::CellB);
    channel = lengthOfSLB+slbIn->getChannelInSLB();
  }
  if(cellType==TGCChannelSLBIn::CellB) {
    block = 1+2*port;
    channel = slbIn->getChannelInSLB();
  }
  if(cellType==TGCChannelSLBIn::CellC) {
    block = 0+2*port;
    int lengthOfSLB = TGCChannelSLBIn::getLengthOfSLB(slbIn->getModuleType(),
						      TGCChannelSLBIn::CellD);
    channel = lengthOfSLB+slbIn->getChannelInSLB();
  }
  if(cellType==TGCChannelSLBIn::CellD) {
    block = 0+2*port;
    channel = slbIn->getChannelInSLB();
  }
  ppout = new TGCChannelPPOut(pp->getSideType(),
			      pp->getModuleType(),
			      pp->getRegionType(),
			      pp->getSector(),
			      pp->getId(),
			      block,
			      channel);
  
  delete pp;
  return ppout;
}


TGCChannelId* TGCCablePPToSLB::getChannelOut(const TGCChannelId* ppout,
					     bool orChannel) const {
  if(orChannel) return nullptr;
  if(ppout->isValid()==false) return nullptr;

  const int ppoutBlock = ppout->getBlock();

  TGCChannelSLBIn* slbin = nullptr;
  
  // PP module
  TGCModuleId* pp = ppout->getModule();
  if(!pp) return nullptr;

  // PP -> SLB module connection
  TGCModuleMap* mapId = getModule(pp);
  delete pp;
  if(!mapId) return nullptr;
  
  // SLB module
  TGCModuleId* slb = nullptr;
  const int size = mapId->size();
  for(int i=0;i<size;i++){
    if(mapId->connector(i)==ppoutBlock/2){
      slb = mapId->popModuleId(i);
      break;
    }
  }
  delete mapId;
  if(!slb) return nullptr;

  // PP ->SLB channel connection
  TGCChannelSLBIn::CellType cellType = TGCChannelSLBIn::NoCellType;
  int channelInSLB = -1;
  if(ppoutBlock%2==0){//D,C
    int lengthOfSLB = TGCChannelSLBIn::getLengthOfSLB(slb->getModuleType(),
						      TGCChannelSLBIn::CellD);
    if(ppout->getChannel()<lengthOfSLB){
      cellType = TGCChannelSLBIn::CellD;
      channelInSLB = ppout->getChannel();
    } else {
      cellType = TGCChannelSLBIn::CellC;
      channelInSLB = ppout->getChannel()-lengthOfSLB;
    }
  } else {//B,A
    int lengthOfSLB = TGCChannelSLBIn::getLengthOfSLB(slb->getModuleType(),
						      TGCChannelSLBIn::CellB);
    if(ppout->getChannel()<lengthOfSLB){
      cellType = TGCChannelSLBIn::CellB;
      channelInSLB = ppout->getChannel();
    } else {
      cellType = TGCChannelSLBIn::CellA;
      channelInSLB = ppout->getChannel()-lengthOfSLB;
    }
  }
  
  int channel = TGCChannelSLBIn::convertChannel(slb->getModuleType(),
						cellType,channelInSLB);
  slbin = new TGCChannelSLBIn(slb->getSideType(),
			      slb->getModuleType(),
			      slb->getRegionType(),
			      slb->getSector(),
			      slb->getId(),
			      channel);
  delete slb;

  return slbin;
}
  
TGCModuleMap* TGCCablePPToSLB::getModuleIn(const TGCModuleId* slb) const {
  if(slb->isValid()==false) return nullptr;

  const int slbId = slb->getId();
 
  TGCDatabase* databaseP =m_database[slb->getRegionType()][slb->getModuleType()];
  TGCModuleMap* mapId = nullptr;
  const int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(databaseP->getEntry(i,1)==slbId){
      int id = databaseP->getEntry(i,0);
      TGCModulePP* pp = new TGCModulePP(slb->getSideType(),
					slb->getModuleType(),
					slb->getRegionType(),
					slb->getSector(),
					id);

      mapId = new TGCModuleMap();
      mapId->insert(0,pp);
      break;
    } 
    if(databaseP->getEntry(i,2)==slbId) {
      int id = databaseP->getEntry(i,0);
      TGCModulePP* pp = new TGCModulePP(slb->getSideType(),
					slb->getModuleType(),
					slb->getRegionType(),
					slb->getSector(),
					id);
      
      mapId = new TGCModuleMap();
      mapId->insert(1,pp);
      break;
    } 
  }
  
  return mapId;
}
  
TGCModuleMap* TGCCablePPToSLB::getModuleOut(const TGCModuleId* pp) const {
  if(pp->isValid()==false) return nullptr;

  const int ppId = pp->getId();

  TGCDatabase* databaseP = m_database[pp->getRegionType()][pp->getModuleType()];
  TGCModuleMap* mapId = nullptr;
  const int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(databaseP->getEntry(i,0)==ppId)
      {
	int id=-1;
	TGCModuleSLB* slb=nullptr;
	mapId = new TGCModuleMap();

	id = databaseP->getEntry(i,1);
	if(id!=-1){
	  slb = new TGCModuleSLB(pp->getSideType(),
				 pp->getModuleType(),
				 pp->getRegionType(),
				 pp->getSector(),
				 id);
	  
	  mapId->insert(0,slb);
	}

	id = databaseP->getEntry(i,2);
	if(id!=-1){
	  slb = new TGCModuleSLB(pp->getSideType(),
				 pp->getModuleType(),
				 pp->getRegionType(),
				 pp->getSector(),
				 id);
	  
	  mapId->insert(1,slb);
	}
	break;
      } 
  }
  
  return mapId;
}

} //end of namespace
