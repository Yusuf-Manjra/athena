/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArMonitoring/LArHVMapTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

#include "LArHV/EMBHVElectrodeConstLink.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArHV/HECHVSubgapConstLink.h"
#include "LArHV/HECHVSubgap.h"
#include "LArHV/FCALHVLineConstLink.h"
#include "LArHV/FCALHVLine.h"
#include "LArReadoutGeometry/EMECCell.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArReadoutGeometry/EMBCell.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArReadoutGeometry/EMBCellConstLink.h"
#include "LArReadoutGeometry/EMECCellConstLink.h"
#include "LArReadoutGeometry/HECCellConstLink.h"
#include "LArReadoutGeometry/FCALTubeConstLink.h"

#include "LArHV/LArHVManager.h"



LArHVMapTool::LArHVMapTool(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent)
     {
     declareInterface<LArHVMapTool>( this );
     }

StatusCode LArHVMapTool::initialize(){

  CHECK(detStore()->retrieve(m_caloIdMgr));

  m_larem_id   = m_caloIdMgr->getEM_ID();
  m_larhec_id   = m_caloIdMgr->getHEC_ID();
  m_larfcal_id   = m_caloIdMgr->getFCAL_ID();

   CHECK(detStore()->retrieve(m_calodetdescrmgr));

   return StatusCode::SUCCESS;
}


std::vector<int> LArHVMapTool::GetHVLines(const Identifier& id) {
  std::set<int> hv;

  // LAr EMB
  if (m_larem_id->is_lar_em(id) && m_larem_id->sampling(id)>0) {
    if (abs(m_larem_id->barrel_ec(id))==1) {
      const EMBDetectorElement* embElement = dynamic_cast<EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (embElement) {
	const EMBCellConstLink cell = embElement->getEMBCell();
	unsigned int nelec = cell->getNumElectrodes();
	for (unsigned int i=0;i<nelec;i++) {
	  const EMBHVElectrodeConstLink electrode = cell->getElectrode(i);
	  for (unsigned int igap=0;igap<2;igap++) {
	    hv.insert(electrode->hvLineNo(igap));
	  }
	}
      }
      else {
	msg(MSG::ERROR) << "Failed d'cast to EMBDetectorElement" << std::endl;
      }
    } else { // LAr EMEC
      const EMECDetectorElement* emecElement = dynamic_cast<EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (emecElement) {
	const EMECCellConstLink cell = emecElement->getEMECCell();
	unsigned int nelec = cell->getNumElectrodes();
	for (unsigned int i=0;i<nelec;i++) {
	  const EMECHVElectrodeConstLink electrode = cell->getElectrode(i);
	  for (unsigned int igap=0;igap<2;igap++) hv.insert(electrode->hvLineNo(igap));
	}
      }
      else {
	msg(MSG::ERROR) << "Failed d'cast to EMECDetectorElement" << std::endl;
      }
    } 
  } else if (m_larhec_id->is_lar_hec(id)) { // LAr HEC
    const HECDetectorElement* hecElement = dynamic_cast<HECDetectorElement*>(m_calodetdescrmgr->get_element(id));
    if (hecElement) {
      const HECCellConstLink cell = hecElement->getHECCell();
      unsigned int nsubgaps = cell->getNumSubgaps();
      for (unsigned int igap=0;igap<nsubgaps;igap++) {
	const HECHVSubgapConstLink subgap = cell->getSubgap(igap);
	hv.insert(subgap->hvLineNo());
      }
    }
    else {
      msg(MSG::ERROR) << "Failed d'cast to HECDetectorElement" << std::endl;
    }
  } else if (m_larfcal_id->is_lar_fcal(id)) { // LAr FCAL
    const FCALDetectorElement* fcalElement = dynamic_cast<FCALDetectorElement*>(m_calodetdescrmgr->get_element(id));
    if (fcalElement) {
      const FCALTile* tile = fcalElement->getFCALTile();
      unsigned int nlines = tile->getNumHVLines();
      unsigned int nlines_found=0;
      for (unsigned int i=0;i<nlines;i++) {
	const FCALHVLineConstLink line = tile->getHVLine(i);
	if (line) nlines_found++;
      }
      if ( nlines_found>0 ) {
	for (unsigned int i=0;i<nlines;i++) {
	  const FCALHVLineConstLink line = tile->getHVLine(i);
	  if (!line) continue;
	  hv.insert(line->hvLineNo());
	}
      }
    }
    else {
      msg(MSG::ERROR) << "Failed d'cast to FCALDetectorElement" << std::endl;
    }
 
  } else if (m_larem_id->is_lar_em(id) && m_larem_id->sampling(id)==0) { // Presamplers
    if (abs(m_larem_id->barrel_ec(id))==1) {
      const EMBDetectorElement* embElement = dynamic_cast<EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (embElement) {
	const EMBCellConstLink cell = embElement->getEMBCell();
	const EMBPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule();
	for (unsigned int igap=0;igap<2;igap++) hv.insert(hvmodule->hvLineNo(igap));
      }
      else {
	msg(MSG::ERROR) << "Failed d'cast to EMBDetectorElement (for presampler)" << std::endl;
      }
    } else {
      const EMECDetectorElement* emecElement = dynamic_cast<EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (emecElement) {
	const EMECCellConstLink cell = emecElement->getEMECCell();
	const EMECPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule ();
	for (unsigned int igap=0;igap<2;igap++) hv.insert(hvmodule->hvLineNo(igap));
      }
      else {
	msg(MSG::ERROR) << "Failed d'cast to EMECDetectorElement (for presampler)" << std::endl;
      }
    }
  }

  std::vector<int> hvlines;
  for (std::set<int>::iterator i=hv.begin();i!=hv.end();++i) hvlines.push_back(*i);
  return hvlines;
}


