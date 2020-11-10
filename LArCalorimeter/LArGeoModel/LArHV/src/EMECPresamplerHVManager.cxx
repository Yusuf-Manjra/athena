/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

#include "EMECPresamplerHVPayload.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArCabling/LArHVCablingTool.h"

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
#include "LArRecConditions/LArHVIdMapping.h"
#endif

#include "Identifier/HWIdentifier.h"
#include "GeoModelKernel/CellBinning.h"

#include <atomic>


namespace {


struct ATLAS_NOT_THREAD_SAFE LegacyIdFunc
{
  LegacyIdFunc();
  std::vector<HWIdentifier> operator()(HWIdentifier id)
  {
    return m_cablingTool->getLArElectrodeIDvec (id);
  }
  LArHVCablingTool* m_cablingTool;
};


LegacyIdFunc::LegacyIdFunc()
{
  ToolHandle<LArHVCablingTool> tool ("LArHVCablingTool");
  if (!tool.retrieve().isSuccess()) {
    std::abort();
  }
  m_cablingTool = tool.get();
}


} // Anonymous namespace


class EMECPresamplerHVManager::Clockwork {
public:
  Clockwork(const EMECPresamplerHVManager* manager) {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iPhi=0; iPhi<64; ++iPhi) {
	moduleArray[iSide][iPhi] = std::make_unique<EMECPresamplerHVModule>(manager, iSide, iPhi);
      }
    }
    ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "HECHVManager");
    if (StatusCode::SUCCESS!=detStore->retrieve(elecId, "LArElectrodeID")) {
      throw std::runtime_error("EMECPresamplerHVManager failed to retrieve LArElectrodeID");
    }

    if (StatusCode::SUCCESS!=detStore->retrieve(hvId,"LArHVLineID")) {
      throw std::runtime_error("EMECPresamplerHVManager failed to retrieve LArHVLineID");
    }
  }
  ~Clockwork() {
  }
  CellBinning                   phiBinning{0.0, 2*M_PI, 64};
  std::unique_ptr<const EMECPresamplerHVModule> moduleArray[2][64];  // not dense
  const LArElectrodeID* elecId;
  const LArHVLineID* hvId;
};


class EMECPresamplerHVManager::EMECPresamplerHVData::Payload
{
public:
  std::vector<EMECPresamplerHVPayload> m_payloadArray;
};


EMECPresamplerHVManager::EMECPresamplerHVData::EMECPresamplerHVData()
{
}
  

EMECPresamplerHVManager::EMECPresamplerHVData::EMECPresamplerHVData
  (std::unique_ptr<Payload> payload)
  : m_payload (std::move (payload))
{
}
  

EMECPresamplerHVManager::EMECPresamplerHVData&
EMECPresamplerHVManager::EMECPresamplerHVData::operator= (EMECPresamplerHVData&& other)
{
  if (this != &other) {
    m_payload = std::move (other.m_payload);
  }
  return *this;
}
  

EMECPresamplerHVManager::EMECPresamplerHVData::~EMECPresamplerHVData()
{
}
  

bool EMECPresamplerHVManager::EMECPresamplerHVData::hvOn
  (const EMECPresamplerHVModule& module, const int& iGap) const
{
  return voltage (module, iGap) > INVALID;
}


double EMECPresamplerHVManager::EMECPresamplerHVData::voltage
  (const EMECPresamplerHVModule& module, const int& /*iGap*/) const
{
  return m_payload->m_payloadArray[index(module)].voltage;
}


double EMECPresamplerHVManager::EMECPresamplerHVData::current
  (const EMECPresamplerHVModule& module, const int& /*iGap*/) const
{
  return m_payload->m_payloadArray[index(module)].current;
}


int EMECPresamplerHVManager::EMECPresamplerHVData::hvLineNo
  (const EMECPresamplerHVModule& module, const int& /*iGap*/) const
{
  return m_payload->m_payloadArray[index(module)].hvLineNo;
}


int EMECPresamplerHVManager::EMECPresamplerHVData::index
  (const EMECPresamplerHVModule& module) const
{
  unsigned int phiIndex          = module.getPhiIndex();
  unsigned int sideIndex         = module.getSideIndex();
  unsigned int index = 64*sideIndex+phiIndex;
  return index;
}


EMECPresamplerHVManager::EMECPresamplerHVManager()
  : m_c (std::make_unique<Clockwork> (this))
{
}

EMECPresamplerHVManager::~EMECPresamplerHVManager()
{
}

const CellBinning *EMECPresamplerHVManager::getPhiBinning() const
{
  return &(m_c->phiBinning);
}

unsigned int EMECPresamplerHVManager::beginPhiIndex() const
{
  return m_c->phiBinning.getFirstDivisionNumber();
}

unsigned int EMECPresamplerHVManager::endPhiIndex() const
{
  return m_c->phiBinning.getFirstDivisionNumber() + m_c->phiBinning.getNumDivisions();
}

const EMECPresamplerHVModule& EMECPresamplerHVManager::getHVModule(unsigned int iSide, unsigned int iPhi) const
{
  return *(m_c->moduleArray[iSide][iPhi]);
}

unsigned int EMECPresamplerHVManager::beginSideIndex() const
{
  return 0;
}

unsigned int EMECPresamplerHVManager::endSideIndex() const
{
  return 2;
}

EMECPresamplerHVManager::EMECPresamplerHVData
EMECPresamplerHVManager::getData (idfunc_t idfunc,
                                  const std::vector<const CondAttrListCollection*>& attrLists) const
{
  auto payload = std::make_unique<EMECPresamplerHVData::Payload>();
  payload->m_payloadArray.reserve(2*64);
  for (unsigned int i=0;i<64;i++) {
    payload->m_payloadArray[i].voltage = EMECPresamplerHVData::INVALID;
  }
    
  for (const CondAttrListCollection* atrlistcol : attrLists) {

    for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {
	 
      // Construct HWIdentifier
      // 1. decode COOL Channel ID
      unsigned int chanID = (*citr).first;
      int cannode = chanID/1000;
      int line = chanID%1000;

      // 2. Construct the identifier
      HWIdentifier id = m_c->hvId->HVLineId(1,1,cannode,line);


      std::vector<HWIdentifier> electrodeIdVec = idfunc(id);

      for(size_t i=0;i<electrodeIdVec.size();i++) {
        HWIdentifier& elecHWID = electrodeIdVec[i];
        int detector = m_c->elecId->detector(elecHWID);
        // check we are in EMECPresampler
        if (detector==3) {


          unsigned int sideIndex=1-m_c->elecId->zside(elecHWID);
          unsigned int phiIndex=m_c->elecId->module(elecHWID);        // from 0 to 31
// rotation for C side
          if (sideIndex==0) {
            if (phiIndex<16) phiIndex=15-phiIndex;
            else phiIndex=47-phiIndex;
          }

// GU  January 2017  -   fix for HV EMEC PS distribution
// 0-31 in phi module
// each module has 2 cell in phi, in the mapping database this is referred by "gapIndex" 
//   0 is on the low phi side (in the ATLAS frame)
//   1 in on the  high phi side
// so in total 64 sectors in phi given by 2*phiIndex+gapIndex
// the two gap of these sectors are powered by the same line and have the same HV
    
          unsigned int gapIndex=m_c->elecId->gap(elecHWID);
 
          unsigned int index = 64*sideIndex+2*phiIndex+gapIndex;

	      
          float voltage = EMECPresamplerHVData::INVALID;
          if (!((*citr).second)["R_VMEAS"].isNull()) voltage = ((*citr).second)["R_VMEAS"].data<float>();
          float current = 0.;
          if (!((*citr).second)["R_IMEAS"].isNull()) current = ((*citr).second)["R_IMEAS"].data<float>();


          payload->m_payloadArray[index].voltage=voltage;
          payload->m_payloadArray[index].current=current;
          payload->m_payloadArray[index].hvLineNo=chanID;
        } // for (electrodeIdVec)
      } // is EMECPresampler
    } // for (atrlistcol)
  }

  return EMECPresamplerHVManager::EMECPresamplerHVData (std::move (payload));
}


EMECPresamplerHVManager::EMECPresamplerHVData
EMECPresamplerHVManager::getData ATLAS_NOT_THREAD_SAFE () const
{
  std::vector<const CondAttrListCollection*> attrLists;
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "EMBHVManager");
  const CondAttrListCollection* atrlistcol = nullptr;
  if (detStore->retrieve(atrlistcol, "/LAR/DCS/HV/BARREl/I16").isSuccess()) {
    attrLists.push_back (atrlistcol);
  }
  if (detStore->retrieve(atrlistcol, "/LAR/DCS/HV/BARREl/I8").isSuccess()) {
    attrLists.push_back (atrlistcol);
  }
  return getData (LegacyIdFunc(), attrLists);
}


#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
EMECPresamplerHVManager::EMECPresamplerHVData
EMECPresamplerHVManager::getData (const LArHVIdMapping& hvIdMapping,
                                  const std::vector<const CondAttrListCollection*>& attrLists) const
{
  auto idfunc = [&] (HWIdentifier id) { return hvIdMapping.getLArElectrodeIDvec(id); };
  return getData (idfunc, attrLists);
}


int EMECPresamplerHVManager::hvLineNo(const EMECPresamplerHVModule& module
				     , const LArHVIdMapping* hvIdMapping) const {
  int sideIndex = module.getSideIndex();
  int phiIndex  = module.getPhiIndex()/2;

  // ________________________ Construct ElectrodeID ________________________________
  int id_detector = 3;
  int id_zside = 1 - sideIndex;
  int id_module{0};
  if(sideIndex==1){
    id_module = phiIndex;
  }
  else {
    if(phiIndex<16) {
      id_module = 15 - phiIndex;
    }
    else {
      id_module = 47 - phiIndex;
    }
  }
  int id_hv_phi{0};
  int id_hv_eta{0};
  int id_gap = module.getPhiIndex()%2;
  int id_electrode{0};

  HWIdentifier elecHWID = m_c->elecId->ElectrodeId(id_detector
						   ,id_zside
						   ,id_module
						   ,id_hv_phi
						   ,id_hv_eta
						   ,id_gap
						   ,id_electrode);
  // ________________________  ________________________________

  // Get LArHVLineID corresponding to a given LArElectrodeId
  HWIdentifier id = hvIdMapping->getLArHVLineID(elecHWID);

  // Extract HV Line No
  return m_c->hvId->can_node(id)*1000 + m_c->hvId->hv_line(id);
}
#endif
