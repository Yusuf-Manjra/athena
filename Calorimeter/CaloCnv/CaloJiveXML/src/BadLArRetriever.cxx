/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/BadLArRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "EventContainers/SelectAllObject.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "Identifier/HWIdentifier.h"
#include "LArTools/LArCablingService.h"

using CLHEP::GeV;

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  BadLArRetriever::BadLArRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    typeName("BadLAr"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    m_calo_id_man  = CaloIdManager::instance();
    m_calocell_id  = m_calo_id_man->getCaloCell_ID();
    m_sgKey = "AllCalo"; 

    declareInterface<IDataRetriever>(this);
    declareProperty("StoreGateKey" , m_sgKey);
    declareProperty("LArlCellThreshold", m_cellThreshold = 50.);
    declareProperty("RetrieveLAr" , m_lar = true);
    declareProperty("DoBadLAr",     m_doBadLAr = false);
    declareProperty("CellConditionCut", m_cellConditionCut = false);

    declareProperty("CellEnergyPrec", m_cellEnergyPrec = 3);
  }

  /**
   * Initialise the ToolSvc
   */

  StatusCode BadLArRetriever::initialize() {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialising Tool" << endreq;

    if ( !service("ToolSvc", m_toolSvc) )
      return StatusCode::FAILURE;
    
    return StatusCode::SUCCESS;	
  }
  
  /**
   * LAr data retrieval from default collection
   */
  StatusCode BadLArRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieve()" << endreq;

    const CaloCellContainer* cellContainer;
    if ( !evtStore()->retrieve(cellContainer,m_sgKey))
      {
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Could not retrieve Calorimeter Cells " << endreq;
	return false;
      }

    if(m_lar){
      DataMap data = getBadLArData(cellContainer);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data).isFailure()){
        return false;
      } else {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Bad cell retrieved" << endreq;
      }
    }
    //LAr cells retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve LAr bad cell location and details
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap BadLArRetriever::getBadLArData(const CaloCellContainer* cellContainer) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "getBadLArData()" << endreq;

    DataMap m_DataMap;

    DataVect phi; phi.reserve(cellContainer->size());
    DataVect eta; eta.reserve(cellContainer->size());
    DataVect energy; energy.reserve(cellContainer->size());
    DataVect idVec; idVec.reserve(cellContainer->size());
    DataVect channel; channel.reserve(cellContainer->size());
    DataVect feedThrough; feedThrough.reserve(cellContainer->size());
    DataVect slot; slot.reserve(cellContainer->size());

    char rndStr[30]; // for rounding (3 digit precision)

    CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(CaloCell_ID::LAREM);
    CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(CaloCell_ID::LAREM);

    
    StatusCode scTool=m_toolSvc->retrieveTool("LArCablingService", m_larCablingSvc);
    if(scTool.isFailure()){
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not retrieve LArCablingService" << endreq;
    }
      
    const LArOnlineID* m_onlineId;
    if ( detStore()->retrieve(m_onlineId, "LArOnlineID").isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getBadLArData(),Could not get LArOnlineID!" << endreq;
    }

    if (m_doBadLAr==true) {

      double energyGeV;

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Start iterator loop over cells" << endreq;
      
      for(;it1!=it2;it1++){

	if( !(*it1)->badcell() ) continue;
	//if( (*it1)->energy() < m_cellThreshold) continue;
	
	if ((((*it1)->provenance()&0xFF)!=0xA5)&&m_cellConditionCut) continue; // check full conditions for LAr
	//Identifier cellid = (*it1)->ID(); 
	
	HWIdentifier LArhwid = m_larCablingSvc->createSignalChannelIDFromHash((*it1)->caloDDE()->calo_hash());
	
	energyGeV = (*it1)->energy()*(1./GeV);
        if (energyGeV == 0) energyGeV = 0.001; // 1 MeV due to LegoCut > 0.0 (couldn't be >= 0.0) 
	energy.push_back(DataType( gcvt( energyGeV, m_cellEnergyPrec, rndStr) ));
	
	idVec.push_back(DataType((Identifier::value_type)(*it1)->ID().get_compact() ));
	phi.push_back(DataType((*it1)->phi()));
	eta.push_back(DataType((*it1)->eta()));
	channel.push_back(DataType(m_onlineId->channel(LArhwid))); 
	feedThrough.push_back(DataType(m_onlineId->feedthrough(LArhwid))); 
	slot.push_back(DataType(m_onlineId->slot(LArhwid))); 

      } // end cell iterator

    } // doBadLAr

    // write values into DataMap
    m_DataMap["phi"] = phi;
    m_DataMap["eta"] = eta;
    m_DataMap["energy"] = energy;
    m_DataMap["id"] = idVec;
    m_DataMap["channel"] = channel;
    m_DataMap["feedThrough"] = feedThrough;
    m_DataMap["slot"] = slot;
    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " , collection: " << dataTypeName();
      msg(MSG::DEBUG) << " retrieved with " << phi.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // getBadLArData

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
