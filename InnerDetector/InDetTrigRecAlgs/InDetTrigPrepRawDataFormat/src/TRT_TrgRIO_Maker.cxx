/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// 
// Implementation for TRT_TrgRIO_Maker.
//  (see header file)
//****************************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "AthenaKernel/Timeout.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetTrigPrepRawDataFormat/TRT_TrgRIO_Maker.h"
#include "TRT_DriftCircleTool/ITRT_DriftCircleTool.h"
#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"


//Trigger
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "InDetTrigToolInterfaces/ITrigRawDataProviderTool.h"


namespace InDet{

  // /////////////////////////////////////////////////////////////////
  // Constructor
  // /////////////////////////////////////////////////////////////////
  TRT_TrgRIO_Maker::TRT_TrgRIO_Maker 
  (const std::string& name,ISvcLocator* pSvcLocator) : 
    HLT::FexAlgo(name,pSvcLocator),
    m_trt_rdo_location("TRT_RDOs"),
    m_trt_rio_location("TRT_TrigDriftCircles"),
    m_rawDataProvider("InDet::TrigTRTRawDataProvider"),
    m_driftcircle_tool("InDet::TRT_DriftCircleTool"), 
    m_riocontainer(0),
    m_mode_rio_production(0),
    m_regionSelector("RegSelSvc", name),
    m_bsErrorSvc("TRT_ByteStream_ConditionsSvc",name),
    m_doFullScan(false),
    m_etaHalfWidth (0.1),
    m_phiHalfWidth (0.1),
    m_doTimeOutChecks(true)
  {
    
    // Read TRT_TrgRIO_Maker steering parameters
    declareProperty("TRT_RIOContainerName", m_trt_rio_location);
    declareProperty("TRT_RDOContainerName", m_trt_rdo_location);
    declareProperty("ModeRIOProduction",    m_mode_rio_production);
    declareProperty("doFullScan",           m_doFullScan);
    declareProperty("EtaHalfWidth",         m_etaHalfWidth);
    declareProperty("PhiHalfWidth",         m_phiHalfWidth);
    declareProperty("RawDataProvider",      m_rawDataProvider);
    declareProperty("doTimeOutChecks",      m_doTimeOutChecks);

    //selection of the TRT bad channels(true/false)
    declareProperty("TRT_DriftCircleTool",    m_driftcircle_tool);

    declareMonitoredVariable("numTrtDriftCircles", m_numTrtDriftCircles   );
    declareMonitoredVariable("numTrtIds", m_numTrtIds    );
    declareMonitoredStdContainer("TrtHashId", m_ClusHashId);
    declareMonitoredStdContainer("TrtBsErr", m_TrtBSErr);
  
    m_timerRegSel=m_timerSeed=m_timerCluster=0;
    m_timerSGate=0;  

  }
  
  //----------------------------------  
  //          beginRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TRT_TrgRIO_Maker::hltBeginRun(){

    // Get the messaging service, print where you are
    msg() << MSG::INFO << "TRT_TrgRIO_Maker::hltBeginRun()";
    if (!m_doFullScan){
      msg() << MSG::INFO << "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth;
    } else {
      msg() << MSG::INFO << "FullScan mode";
    }
    msg() << MSG::INFO << endreq;


    /*
    StatusCode sc = m_rawDataProvider->initContainer();
    if (sc.isFailure())
      msg() << MSG::WARNING << "RDO container cannot be registered" << endreq;
    */
    return HLT::OK;
  }


  //----------------------------------  
  //          Initialize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TRT_TrgRIO_Maker::hltInitialize() {
    //----------------------------------------------------------------------------

      
    // Get TRT_DriftCircle tool
    if ( m_driftcircle_tool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << m_driftcircle_tool.propertyName() << ": Failed to retrieve tool " << m_driftcircle_tool << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else {
      msg() << MSG::INFO << m_driftcircle_tool.propertyName() << ": Retrieved tool " << m_driftcircle_tool << endreq;
    }
  
    // Get DetectorStore service
    //
    StoreGateSvc* detStore;
    StatusCode s=service("DetectorStore",detStore);
    if (s.isFailure())  {
      msg()<<MSG::FATAL<<"DetectorStore service not found"<<endreq; 
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 


    const TRT_ID * IdHelper(0);
    if (detStore->retrieve(IdHelper, "TRT_ID").isFailure()) {
      msg() << MSG::FATAL << "Could not get TRT ID helper" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }


    if(!store()->transientContains<InDet::TRT_DriftCircleContainer>(m_trt_rio_location)) {

      // Create TRT RIO container
      m_riocontainer = new InDet::TRT_DriftCircleContainer(IdHelper->straw_layer_hash_max());
      if(!m_riocontainer) {
	msg()<< MSG::FATAL <<"Could not creat TRT_DriftCircleContainer "
	     << m_trt_rio_location << endreq; 
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }

      if (store()->record(m_riocontainer, m_trt_rio_location).isFailure()) {
	msg() << MSG::WARNING << " Container " << m_trt_rio_location
	      << " could not be recorded in StoreGate !" 
	      << endreq;
      } 
      else {
	msg() << MSG::INFO << "Container " << m_trt_rio_location 
	      << " successfully registered  in StoreGate" << endreq;  
      }
    }
    else {    
      s = store()->retrieve(m_riocontainer, m_trt_rio_location);
    
      if (s.isFailure()) {
	msg() << MSG::ERROR << "Failed to get Cluster Container" << endreq;
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
      else { 
	msg() << MSG::INFO << "Got Cluster Container from TDS: " << m_trt_rio_location << endreq;
      }
    }
    m_riocontainer->addRef();

    if(!m_doFullScan){
      // Retrieving Region Selector Tool:
      if ( m_regionSelector.retrieve().isFailure() ) {
	msg() << MSG::FATAL
	      << m_regionSelector.propertyName()
	      << " : Unable to retrieve RegionSelector tool "
	      << m_regionSelector.type() << endreq;
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
    }
    else{
      msg() << MSG::INFO
	    << "RegionSelector tool not needed for FullScan"<< endreq;
    }

    if (m_bsErrorSvc.retrieve().isFailure()){
      msg() << MSG::FATAL << "Could not retrieve " << m_bsErrorSvc << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    //decoding tool
    if (m_rawDataProvider.retrieve().isFailure()){
      msg() << MSG::ERROR 
	    << "Raw data provider not available"  << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    // Timers:
    m_timerRegSel  = addTimer("InDetEFTClRegSel"); // 1
    m_timerSeed    = addTimer("InDetEFTClSeed");   // 2
    m_timerCluster = addTimer("InDetEFTCluster");  // 3
    m_timerSGate   = addTimer("InDetEFTClSGate");  // 4

    return HLT::OK;
  }
  //----------------------------------------------------------------------------
  TRT_TrgRIO_Maker::~TRT_TrgRIO_Maker() {}  
  //----------------------------------------------------------------------------
  //         Execute method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TRT_TrgRIO_Maker::hltExecute(const HLT::TriggerElement*,
					      HLT::TriggerElement* outputTE){
    //----------------------------------------------------------------------------
    
    //initialisation of monitored quantities
    m_numTrtIds = 0;
    m_numTrtDriftCircles = 0;
    m_listOfTrtIds.clear();
    m_ClusHashId.clear();
    m_TrtBSErr.clear();

    // For the moment, the output will be stored in StoreGate:
    // ------------------------------------------------------
    // TRT_DriftCircle container registration
  
    if(doTiming()) m_timerSGate->resume();

    if(!store()->transientContains<InDet::TRT_DriftCircleContainer>(m_trt_rio_location)) {
      m_riocontainer->cleanup();
      
      if (store()->record(m_riocontainer,
			  m_trt_rio_location,false).isFailure()) {
	msg() << MSG::WARNING << "Container " << m_trt_rio_location
	      << " could not be recorded in StoreGate !" 
	      << endreq;
      } 
      else{
	if(msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "REGTEST: Container '" << m_trt_rio_location
		<< " recorded in StoreGate " << endreq;
      }
    }
    else {
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Container '" << m_trt_rio_location
	      << "' recorded in StoreGate" << endreq;
    }
  
    if(doTiming()) m_timerSGate->pause();

    //---------------------------
    // Trigger specific part:
    //---------------------------
    //   Get from the Trigger Element the RoI
    //   Ask the Region Selector for the list of hash indexes for this ROI.
    //   Transform those indexes into a RDO collection.
    //-------------------------------------------------------------------------

    //reset BS error service
    m_bsErrorSvc->resetCounts();
    StatusCode scdec = StatusCode::SUCCESS;

    if(!m_doFullScan){ //this is not needed for FullScan
  
      if(doTiming()) m_timerSeed->start();
    
      // Get RoiDescriptor
      const TrigRoiDescriptor* roi;
      if ( HLT::OK !=  getFeature(outputTE, roi) ) {
	msg() << MSG::WARNING << "Can't get RoI" << endreq;
	return HLT::NAV_ERROR;
      }
    
      if (!roi){
	msg() << MSG::WARNING << "Received NULL RoI" << endreq;
	return HLT::NAV_ERROR;
      }

      double RoiEtaWidth = roi->etaPlus() - roi->etaMinus();
      double RoiPhiWidth = roi->phiPlus() - roi->phiMinus();
      const double M_2PI = 2*M_PI;
      const float  M_PIF = float(M_PI);
      while ( RoiPhiWidth> M_PIF ) RoiPhiWidth -= M_2PI;
      while ( RoiPhiWidth<-M_PIF ) RoiPhiWidth += M_2PI;
      if ( RoiPhiWidth> M_PI ) RoiPhiWidth -= 1e-7;
      if ( RoiPhiWidth<-M_PI ) RoiPhiWidth += 1e-7;

      if (!roi->isFullscan()){
	if( fabs(RoiEtaWidth/2. - m_etaHalfWidth) > 0.02) {
	  if(msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << "ROI range is different from configuration: " << endreq;
	    msg() << MSG::DEBUG << "eta width = " << RoiEtaWidth << "; with etaPlus = " << roi->etaPlus() << "; etaMinus = " << roi->etaMinus() << endreq;
	    msg() << MSG::DEBUG << "etaHalfWidth from config: " << m_etaHalfWidth << endreq;
	  }
	}
	
      }
      else {
	if (m_etaHalfWidth<2.5 || m_phiHalfWidth<3.) {
	  msg() << MSG::WARNING << "FullScan RoI and etaHalfWidth from config: " << m_etaHalfWidth << endreq;
	  msg() << MSG::WARNING << "FullScan RoI and phiHalfWidth from config: " << m_phiHalfWidth << endreq;
	}
      }


      if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "REGTEST:" << *roi << endreq;
      }

      //NaN. this is for the check on the case in which the call of phi() and eta() do not return a well-defined numerical value
      if (roi->phi() != roi->phi() || roi->eta() !=roi->eta()){
	msg() << MSG::WARNING << "Received bad RoI " << *roi << endreq; 
	return HLT::NAV_ERROR;
      }

      scdec = m_rawDataProvider->decode(roi);

  
      if(doTiming()) m_timerSeed->stop();

      //   Get the TRT RDO's:
      //     - First get the TRT ID's using the RegionSelector
      //     - Retrieve from SG the RDOContainer: 
      //       Identifiable Container that contains pointers to all the RDO 
      //        collections (one collection per detector)
      //     - Retrieve from StoreGate the RDO collections.
      //       (the ByteStreamConvertors are called here).
      //std::vector<IdentifierHash> listOfTrtIds; 
    
      if(doTiming()) m_timerRegSel->start();
  
      m_regionSelector->DetHashIDList( TRT, *roi, m_listOfTrtIds);
      m_numTrtIds = m_listOfTrtIds.size();

      if(doTiming()) m_timerRegSel->stop();

      if (msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "REGTEST: TRT : Roi contains " 
	      << m_numTrtIds << " det. Elements" << endreq;
    }
    else {   //fullscan
      scdec = m_rawDataProvider->decode(0);    //NULL RoI means decode everything
    }

    if (scdec.isSuccess()){
      //check for recoverable errors
      int n_err_total = 0;
       
      int bsErrors[EFID_MAXNUM_TRT_BS_ERRORS];
      
      for (size_t idx = 0; idx<size_t(EFID_MAXNUM_TRT_BS_ERRORS); idx++){
	int n_errors = m_bsErrorSvc->getNumberOfErrors(TRTByteStreamErrors::errorTypes(idx));
	n_err_total += n_errors;
	bsErrors[idx] = n_errors;
      }

      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "decoding errors: " << n_err_total;

      if (n_err_total){
	for (size_t idx = 0; idx<size_t(EFID_MAXNUM_TRT_BS_ERRORS); idx++){
	  //	  m_TrtBSErr.push_back(bsErrors[idx]);
	  if (bsErrors[idx])
	    m_TrtBSErr.push_back(idx);

	  msg() << MSG::DEBUG << " " << bsErrors[idx];
	}
      }

      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << endreq;

    } else {
      msg() << MSG::DEBUG << " m_rawDataProvider->decode failed" << endreq;
    }

  

    if(doTiming()) m_timerSGate->start();

    const TRT_RDO_Container* p_trtRDOContainer;
    StatusCode sc = store()->retrieve(p_trtRDOContainer, m_trt_rdo_location);

    if (sc.isFailure() ) {
      msg() << MSG::FATAL << "Could not find the TRT_RDO_Container " 
	    << m_trt_rdo_location << endreq;

      // Activate the TriggerElement anyhow.
      // (FEX algorithms should not cut and it could be that in the Pixel/SCT 
      // tracks can anyhow be reconstructed) 
      return HLT::ERROR;
    } 
    else{
      if (msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "REGTEST: Found the TRT_RDO_Container "
	      << m_trt_rdo_location << endreq;
    }
    if(doTiming()) m_timerSGate->pause();

    if(!m_doFullScan){
    
      if(doTiming()) m_timerCluster->resume();

      for (unsigned int i=0; i<m_listOfTrtIds.size(); i++) {

	if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	  msg() << MSG::WARNING << "Timeout reached. Aborting sequence." << endreq;
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}

	TRT_RDO_Container::const_iterator 
	  RDO_collection_iter = p_trtRDOContainer->indexFind(m_listOfTrtIds[i]); 

	if (RDO_collection_iter == p_trtRDOContainer->end()) continue;
    
	const InDetRawDataCollection<TRT_RDORawData>* RDO_Collection(*RDO_collection_iter);
      
	if (!RDO_Collection) continue;
      
	if (msgLvl() <= MSG::VERBOSE)
	  msg() << MSG::VERBOSE << "RDO collection size="
		<< RDO_Collection->size()
		<< ", ID=" << RDO_Collection->identify() << endreq;
      
	if (RDO_Collection->size() != 0){

	  const InDet::TRT_DriftCircleCollection* p_rio = 
	    m_driftcircle_tool->convert (m_mode_rio_production, RDO_Collection, true);
      
	  m_numTrtDriftCircles += p_rio->size();
	  // -me- fix test
	  if(p_rio->size() != 0) {
	  
	    // Record in StoreGate the drift circle container:
	    sc = m_riocontainer->addCollection(p_rio,p_rio->identifyHash());

	    m_ClusHashId.push_back(p_rio->identifyHash());

	    if(sc.isFailure()) {
	      if (msgLvl() <= MSG::VERBOSE)
		msg() << MSG::VERBOSE
		      << "Failed registering TRT_DriftCircle collection : "
		      <<  p_rio->identify()  <<endreq;
	      delete p_rio;
	    }
	  } else {
	    // -me- cleanup memory
	    delete p_rio;
	  }
	}
      }
      if(doTiming()) m_timerCluster->pause();
    }
    else{   //fullscan
    
      //      p_trtRDOContainer->clID();
      TRT_RDO_Container::const_iterator rdoCollections    = p_trtRDOContainer->begin();
      TRT_RDO_Container::const_iterator rdoCollectionsEnd = p_trtRDOContainer->end();
    
      if(doTiming()) m_timerCluster->resume();
    
      for(; rdoCollections!=rdoCollectionsEnd; ++rdoCollections) {
      
	if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	  msg() << MSG::WARNING << "Timeout reached. Aborting sequence." << endreq;
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}

	const InDetRawDataCollection<TRT_RDORawData>* currentCollection(*rdoCollections);
	
        const InDet::TRT_DriftCircleCollection* p_rio=
          m_driftcircle_tool->convert(m_mode_rio_production, currentCollection , true);
        
	if(p_rio) {
	  if (p_rio->size() != 0) {
	    m_numTrtDriftCircles += p_rio->size();
	    // -me- new IDC does no longer register in Storegate if hash is used !
	    //s = p_riocontainer->addCollection(p_rio, p_rio->identify()); 

	    sc = m_riocontainer->addCollection(p_rio, p_rio->identifyHash()); 
	    //s = m_store_gate->record(p_rio,p_riocontainer->key(p_rio->identify()));
	    if(sc.isFailure()) {
	      if (msgLvl() <= MSG::VERBOSE)
		msg() << MSG::VERBOSE << "Error while registering TRT_DriftCircle collection"<<endreq; 
	      //return HLT::ERROR;
	      delete p_rio;
	    }
	  } 
	  else {
	    if (msgLvl() <= MSG::DEBUG)
	      msg() << MSG::DEBUG << "Don't write empty collections" << endreq;
	    // -me- cleanup memory
	    delete p_rio;
          }
	}
      }
      if(doTiming()) m_timerCluster->pause();
    }


    if(doTiming()){
      m_timerCluster->stop();
      m_timerSGate->stop();
    }

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: Reconstructed " << m_numTrtDriftCircles 
	    << " drift circles in the RoI " << endreq;
  
    //igb fixme
    //sc = store()->setConst(m_riocontainer);
    //if(sc.isFailure()) {
    //msg() << MSG::ERROR
    //<< " ??? " <<endreq;
    //return sc;
    //}

    return HLT::OK;
  }
  //-----------------------------------
  //          Finalize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TRT_TrgRIO_Maker::hltFinalize() {
    //----------------------------------------------------------------------------
    m_riocontainer->cleanup();

    //delete m_riocontainer;
    //igb -fix me- it crashes
    m_riocontainer->release();

    return HLT::OK;
  }

  //----------------------------------  
  //          endRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TRT_TrgRIO_Maker::hltEndRun()
  {

    msg() << MSG::INFO << "TRT_TrgRIO_Maker::endRun()" << endreq;

    return HLT::OK;
  }

  //---------------------------------------------------------------------------
}
