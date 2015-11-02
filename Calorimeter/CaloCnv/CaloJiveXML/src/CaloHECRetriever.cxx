/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/CaloHECRetriever.h"

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
  CaloHECRetriever::CaloHECRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    typeName("HEC"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    m_calo_id_man  = CaloIdManager::instance();
    m_calocell_id  = m_calo_id_man->getCaloCell_ID();
    m_sgKey = "AllCalo"; 

    declareInterface<IDataRetriever>(this);
    declareProperty("StoreGateKey" , m_sgKey);
    declareProperty("HEClCellThreshold", m_cellThreshold = 50.);
    declareProperty("RetrieveHEC" , m_hec = true);
    declareProperty("DoBadHEC",     m_doBadHEC = false);
    declareProperty("DoHECCellDetails",  m_doHECCellDetails = false); 
    declareProperty("CellConditionCut", m_cellConditionCut = false);
    declareProperty("LArChannelsToIgnoreM5",  m_LArChannelsToIgnoreM5);
    declareProperty("DoMaskLArChannelsM5", m_doMaskLArChannelsM5 = false);

    declareProperty("CellEnergyPrec", m_cellEnergyPrec = 3);
    declareProperty("CellTimePrec", m_cellTimePrec = 3);
  }

  /**
   * Initialise the ToolSvc
   */

  StatusCode CaloHECRetriever::initialize() {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialising Tool" << endreq;

    if ( !service("ToolSvc", m_toolSvc) )
      return StatusCode::FAILURE;
    
    return StatusCode::SUCCESS;	
  }
   
  /**
   * HEC data retrieval from default collection
   */
  StatusCode CaloHECRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieve()" << endreq;

    const CaloCellContainer* cellContainer;
    if ( !evtStore()->retrieve(cellContainer,m_sgKey))
      {
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Could not retrieve Calorimeter Cells " << endreq;
	return false;
      }

   if(m_hec){
      DataMap data = getHECData(cellContainer);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data).isFailure()){
        return false;
      } else {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "HEC retrieved" << endreq;
      }
    }
    //HEC cells retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve HEC cell location and details
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap CaloHECRetriever::getHECData(const CaloCellContainer* cellContainer) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "getHECData()" << endreq;

    DataMap m_DataMap;

    DataVect phi; phi.reserve(cellContainer->size());
    DataVect eta; eta.reserve(cellContainer->size());
    DataVect energy; energy.reserve(cellContainer->size());
    DataVect idVec; idVec.reserve(cellContainer->size());
    DataVect channel; channel.reserve(cellContainer->size());
    DataVect feedThrough; feedThrough.reserve(cellContainer->size());
    DataVect slot; slot.reserve(cellContainer->size());

    DataVect cellTimeVec; cellTimeVec.reserve(cellContainer->size());
    DataVect cellGain; cellGain.reserve(cellContainer->size());
    DataVect cellPedestal; cellPedestal.reserve(cellContainer->size());
    DataVect adc2Mev; adc2Mev.reserve(cellContainer->size());
    DataVect BadCell; BadCell.reserve(cellContainer->size());

    char rndStr[30]; // for rounding (3 digit precision)

    CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(CaloCell_ID::LARHEC);
    CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(CaloCell_ID::LARHEC);

    
   StatusCode scTool=m_toolSvc->retrieveTool("LArCablingService", m_larCablingSvc);
   if(scTool.isFailure()){
	if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not retrieve LArCablingService" << endreq;
      }

    const ILArPedestal* larPedestal = nullptr;
    if(m_doHECCellDetails){
	if( detStore()->retrieve(larPedestal).isFailure() ){
	  if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getHECData(), Could not retrieve LAr Pedestal" << endreq;
	}
      }
      
    const LArOnlineID* onlineId = nullptr;
    if ( detStore()->retrieve(onlineId, "LArOnlineID").isFailure()) {
     if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getHECData(),Could not get LArOnlineID!" << endreq;
     }

      IAlgTool* algtool;
      ILArADC2MeVTool* adc2mevTool=0;
      if(m_doHECCellDetails){
	if( m_toolSvc->retrieveTool("LArADC2MeVTool", algtool).isFailure()){
	  if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getHECData(), Could not retrieve LAr ADC2MeV Tool" <<endreq;
	} else {
	  adc2mevTool=dynamic_cast<ILArADC2MeVTool*>(algtool);
	}
      }

      double energyGeV,cellTime;	
      double energyAllLArHEC = 0.;      

      for(;it1!=it2;it1++){
        if ((*it1)->energy() < m_cellThreshold) continue; // skip to next cell if threshold not passed

	if((*it1)->badcell()){ BadCell.push_back(1); }
	else{ BadCell.push_back(-1); }

	  if ((((*it1)->provenance()&0xFF)!=0xA5)&&m_cellConditionCut) continue; // check full conditions for HEC
	  Identifier cellid = (*it1)->ID(); 

          HWIdentifier LArhwid = m_larCablingSvc->createSignalChannelIDFromHash((*it1)->caloDDE()->calo_hash());
	  
	  //ignore HEC cells that are to be masked
	  if (m_doMaskLArChannelsM5){
	    bool maskChannel = false;
	    for (size_t i = 0; i < m_LArChannelsToIgnoreM5.size(); i++){
              if (cellid == m_LArChannelsToIgnoreM5[i]){
		maskChannel = true; 
		break;  // exit loop over bad channels
	      }	      
	    }
	    if (maskChannel) continue;  // continue loop over all channels
	  }

	  energyGeV = (*it1)->energy()*(1./GeV);
	  energy.push_back(DataType( gcvt( energyGeV, m_cellEnergyPrec, rndStr) ));
    	  energyAllLArHEC += energyGeV;

          idVec.push_back(DataType((Identifier::value_type)(*it1)->ID().get_compact() ));
          phi.push_back(DataType((*it1)->phi()));
          eta.push_back(DataType((*it1)->eta()));
          channel.push_back(DataType(onlineId->channel(LArhwid))); 
          feedThrough.push_back(DataType(onlineId->feedthrough(LArhwid))); 
       	  slot.push_back(DataType(onlineId->slot(LArhwid))); 

	  if ( m_doHECCellDetails){
	    cellTime = (*it1)->time();
	    cellTimeVec.push_back(DataType( gcvt( cellTime, m_cellTimePrec, rndStr) ) );
	    cellGain.push_back(DataType( (*it1)->gain() ) ); 
      	
	    int hecgain = (*it1)->gain();
	    float pedestal=larPedestal->pedestal(cellid,hecgain);
	    float pedvalue=0;
	    if (pedestal >= (1.0+LArElecCalib::ERRORCODE)) pedvalue = pedestal;
	    else pedvalue = 0;
	    cellPedestal.push_back(DataType(pedvalue));

            if (!adc2mevTool)
              adc2Mev.push_back(DataType(-1));
            else {
              const std::vector<float>* polynom_adc2mev = &(adc2mevTool->ADC2MEV(cellid,hecgain));
              if (polynom_adc2mev->size()==0){ adc2Mev.push_back(DataType(-1)); }
              else{ adc2Mev.push_back(DataType((*polynom_adc2mev)[1])); }
            }
	  }
      }

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Total energy in HEC (LAr) in GeV : " <<  energyAllLArHEC << endreq;

    // write values into DataMap
    m_DataMap["phi"] = phi;
    m_DataMap["eta"] = eta;
    m_DataMap["energy"] = energy;
    m_DataMap["id"] = idVec;
    m_DataMap["channel"] = channel;
    m_DataMap["feedThrough"] = feedThrough;
    m_DataMap["slot"] = slot;
    //Bad Cells
    if (m_doBadHEC==true) {
      m_DataMap["BadCell"] = BadCell;
    }
    // adc counts
    if ( m_doHECCellDetails){
       m_DataMap["cellTime"] = cellTimeVec;
       m_DataMap["cellGain"] = cellGain;
       m_DataMap["cellPedestal"] = cellPedestal;
       m_DataMap["adc2Mev"] = adc2Mev;
    }
    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " , collection: " << dataTypeName();
      msg(MSG::DEBUG) << " retrieved with " << phi.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // getHECData

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
