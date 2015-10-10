/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigCaloCellMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   P.A. Delsart
//           This is an Hlt algorithm that creates a cell container
//           with calorimeter cells within an RoI. Based on Cibran 
//           Santamarina original TrigCaloRec
//
// ********************************************************************
//

#include <sstream>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"
//
#include "CaloEvent/CaloCellContainer.h"
//
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
#include "CaloUtils/CaloCollectionHelper.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigCaloRec/TrigCaloCellMaker.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"
#include "TrigCaloRec/TrigCaloQuality.h"
#include "CaloInterface/ICaloCellMakerTool.h"

//
class ISvcLocator;
// EM mid layer cell size.
const double TrigCaloCellMaker::m_detas2 = 0.025;
const double TrigCaloCellMaker::m_dphis2 = 0.0245436926;


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

TrigCaloCellMaker::TrigCaloCellMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator)
  ,  m_tcrAlgTools(this), 
    m_fullScanEnabled(false),
    m_trustRoiLimits(false),
    m_createRoiForID(false),
    m_etaWidthForID(0),
    m_phiWidthForID(0),
    m_counter(0),
    pCaloCellContainer(NULL),
    pTrigCaloQuality(NULL)
 {

   // Name of Tools to check and finalize the cell container...
    declareProperty("ContainerTools",m_containerToolNames);

   // Eta and Phi size of the RoI window...
    declareProperty("eta_size",m_neta);
    declareProperty("phi_size",m_nphi);

    /// Trust the limits encoded in the RoI already
    declareProperty("TrustRoiLimits", m_trustRoiLimits=false);

    /// create an RoI for the ID trigger
    declareProperty("CreateRoiForID", m_createRoiForID=false);
    declareProperty("EtaWidthForID",  m_etaWidthForID=0);
    declareProperty("PhiWidthForID",  m_phiWidthForID=0);

    declareProperty ("TCRTools", m_tcrAlgTools);
    //declareProperty ("ContainerTools", m_containerTools);
    
    // prescale for persistency of CaloCell
    declareProperty ("PersistencyPrescaleFactor", m_persistencyPSF=0);
    declareProperty ("PersistencyKeyName"       , m_persistencyKey="TrigCaloCellMaker");


    declareMonitoredVariable("CellContainerSize", m_CellContainerSize);
    declareMonitoredVariable("EtaL2", m_EtaL2);
    declareMonitoredVariable("PhiL2", m_PhiL2);
    declareMonitoredVariable("ConversionErrorInLArEM",   m_conversionError[IAlgToolEFCalo::EFTTEM]);
    declareMonitoredVariable("ConversionErrorInLArHad",  m_conversionError[IAlgToolEFCalo::EFTTHEC]);
    declareMonitoredVariable("ConversionErrorInTile",    m_conversionError[IAlgToolEFCalo::EFTILE]);
    declareMonitoredVariable("ConversionErrorInFCalEm",  m_conversionError[IAlgToolEFCalo::EFFCALEM]);
    declareMonitoredVariable("ConversionErrorInFCalHad", m_conversionError[IAlgToolEFCalo::EFFCALHAD]);
    declareMonitoredVariable("ConversionErrorInFullCalo",m_conversionError[IAlgToolEFCalo::EFFULLCALO]);
     
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigCaloCellMaker::~TrigCaloCellMaker()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NSSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

HLT::ErrorCode TrigCaloCellMaker::hltInitialize()
{
  msg() << MSG::INFO << "in initialize()" << endreq;

  m_counter = 0;
  
  // Cache pointer to ToolSvc
  IToolSvc* toolSvc = 0;// Pointer to Tool Service
  if (service("ToolSvc", toolSvc).isFailure()) {
    msg() << MSG::FATAL << " Tool Service not found " << endreq;
    return HLT::TOOL_FAILURE;
  }

  if (timerSvc()) { // Global timers
    m_timer.push_back(addTimer("TCC_TimerTot"));
    m_timer.push_back(addTimer("TCC_RoICellContainer"));
  }


  /*if ( m_tcrAlgTools.retrieve().isFailure() ) {
    msg() << MSG::ERROR << "Failed to retrieve helper tools: " << m_tcrAlgTools << endreq;
    return HLT::BAD_JOB_SETUP;
   } else {
    msg() << MSG::INFO << "Retrieved " << m_tcrAlgTools << endreq;
  }*/

  std::vector<std::string>::iterator itrName;
  std::vector<std::string>::iterator endName;
  m_fullScanEnabled=false;

  ToolHandleArray<IAlgToolEFCalo>::iterator itrtcr = m_tcrAlgTools.begin();
  ToolHandleArray<IAlgToolEFCalo>::iterator endtcr = m_tcrAlgTools.end();

  for (; itrtcr!=endtcr; ++itrtcr) {

    if( (itrtcr->retrieve()).isFailure() ) {
        msg() << MSG::ERROR << "Unable to find tool named " << (*itrtcr).name() << endreq;
        return HLT::BAD_JOB_SETUP;
    } else {
      msg() << MSG::INFO << " successfully retrieved " << (*itrtcr).name() << endreq;
      std::vector<TrigTimer*> tooltimers = (*itrtcr)->getTimers();
      std::vector<TrigTimer*>::iterator timer = tooltimers.begin();
      for (; timer != tooltimers.end(); timer++){
          declareMonitoredObject((*timer)->name(),*(*timer), &TrigTimer::lastElapsed);
          //declareMonitoredObject((*timer)->name(),*(*timer), &TrigTimer::elapsed);
          if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found timer. Added to monitoring " << endreq;
      }
    }    
    if((*itrtcr).name() == "FullCaloCellContMaker")
        m_fullScanEnabled=true;
  }
  // access CellContainer tools and store them
  itrName = m_containerToolNames.begin() ;
  endName = m_containerToolNames.end() ;

  for (; itrName!=endName; ++itrName) {
     
    ListItem theItem(*itrName);
    IAlgTool* algtool;
     
    if( toolSvc->retrieveTool(theItem.type(), theItem.name(), algtool,this).isFailure() ) {
      msg() << MSG::FATAL << "Unable to find tool for " << (*itrName) << endreq;
      return HLT::TOOL_FAILURE;
    } else {
      msg() << MSG::INFO << (*itrName) << " successfully retrieved" << endreq;
 // Save the cell container tools (only for checking purposes) in m_containerTools
      m_containerTools.push_back(dynamic_cast<ICaloCellMakerTool*>(algtool) );
      if(timerSvc() ) m_timer.push_back(addTimer("TCC_"+theItem.name())); // One timer per tool
    }
  }
 // end of helpers...
 
 if (msgLvl() <= MSG::DEBUG)
   msg() << MSG::DEBUG
         << "Initialization of TrigCaloCellMaker completed successfully"
         << endreq;
 
 return HLT::OK;
}


HLT::ErrorCode TrigCaloCellMaker::hltFinalize()
{
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in finalize()" << endreq;

  return HLT::OK;
}

HLT::ErrorCode TrigCaloCellMaker::hltExecute(const HLT::TriggerElement* inputTE, 
					     HLT::TriggerElement* outputTE)
{
  
  // Time total TrigCaloCellMaker execution time.
  if (timerSvc()) m_timer[0]->start();
  for(int id=0; id<IAlgToolEFCalo::MAXNEFCALO; id++) m_conversionError[id]=0;
  
#ifndef NDEBUG
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in execute()" << endreq;
#endif
  
  // Monitoring initialization...
   m_CellContainerSize = 0.;

  // Some debug output:
#ifndef NDEBUG
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "outputTE->getId(): " << outputTE->getId() << endreq;
    msg() << MSG::DEBUG << "inputTE->getId(): " << inputTE->getId() << endreq;
  }
#endif
   
   // Get RoiDescriptor
  //  bool createdRoI = false;
  const TrigRoiDescriptor* tmpRoi   = 0;
  HLT::ErrorCode sc = getFeature(inputTE, tmpRoi, ""); 
  if (sc != HLT::OK || tmpRoi==0 ) return sc;
  
  const IRoiDescriptor* roiDescriptor = tmpRoi;

  if ( !roiDescriptor->composite() )  { 
    if ( !m_trustRoiLimits ){ 
      
      double   eta0 = roiDescriptor->eta();
      /// don't really need to wrap phi, as the RoI does it anyway, so phi
      /// from the RoI should always be wrapped anyway
      double   phi0 = HLT::wrapPhi(roiDescriptor->phi());
      
      // Set Phi in [0, 2Pi]
      //while (phi0 > 2.*M_PI) phi0 -= 2. * M_PI;
      //while (phi0 < 0. )     phi0 += 2. * M_PI;
      /// not needed 
      //   while (phi0 > M_PI) phi0 -= 2. * M_PI;
      //   while (phi0 <-M_PI) phi0 += 2. * M_PI;
      
      // set up the sampling windows (only for samp2??):
      
      //double etamin = std::max(-2.5,eta0-(m_detas2*(float)m_neta)/2.);
      //double etamax = std::min( 2.5,eta0+(m_detas2*(float)m_neta)/2.);
      double etamin = eta0-(m_detas2*(float)m_neta)/2.;
      double etamax = eta0+(m_detas2*(float)m_neta)/2.;
      // What if phimin < 0 or phimax > 2Pi ??  question
      double phimin = HLT::wrapPhi(phi0 - (m_dphis2*(float)m_nphi)/2.);
      double phimax = HLT::wrapPhi(phi0 + (m_dphis2*(float)m_nphi)/2.);
      
      /// need to map the limits as well !!!  - NB: HLT::wrapPhi does this now
      //  while (phimin > M_PI) phimin -= 2. * M_PI;
      //  while (phimin <-M_PI) phimin += 2. * M_PI;
      
      //  while (phimax > M_PI) phimax -= 2. * M_PI;
      //  while (phimax <-M_PI) phimax += 2. * M_PI;
      
      
#ifndef NDEBUG
      if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << " eta0 = "<< eta0 << endreq;
	msg() << MSG::DEBUG << " phi0 = "<< phi0 << endreq;
	msg() << MSG::DEBUG << " etamin = "<< etamin << endreq;
	msg() << MSG::DEBUG << " etamax = "<< etamax << endreq;
	msg() << MSG::DEBUG << " phimin = "<< phimin << endreq;
	msg() << MSG::DEBUG << " phimax = "<< phimax << endreq;
      }
#endif
      
      TrigRoiDescriptor* _roi = new TrigRoiDescriptor( roiDescriptor->roiWord(), roiDescriptor->l1Id(), roiDescriptor->roiId(),
						       eta0, etamin, etamax, phi0, phimin, phimax );
      attachFeature( outputTE, _roi, "" );
      roiDescriptor = _roi; 
    }
  }
  
  m_PhiL2 = roiDescriptor->phi();
  m_EtaL2 = roiDescriptor->eta();
  
  
  
#ifndef NDEBUG
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " REGTEST: RoI id " << *roiDescriptor << roiDescriptor->eta() << endreq;
#endif
  

  //if (timerSvc()) m_timer[1]->start(); // Measures the time to retrieve the cells in the RoI

  // The CellContainer is not the cell owner (SG::VIEW_ELEMENTS)...
  //CaloCellContainer* pCaloCellContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);
  pCaloCellContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);
  pCaloCellContainer->reserve(190000);
  pTrigCaloQuality   = new TrigCaloQuality();

  bool isPersistent = (m_persistencyPSF!=0 && (m_counter%m_persistencyPSF)==0) ? 1 : 0;
  std::string persKey = isPersistent ? m_persistencyKey : "TrigCaloCellMaker";
#ifndef NDEBUG
  if ( msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "CaloCellContainer is stored with key = " << persKey << endreq;
  }
#endif

  std::string cellCollKey;
  sc = getUniqueKey( pCaloCellContainer, cellCollKey, persKey );
  if (sc != HLT::OK) { 
    msg() << MSG::DEBUG << "Could not retrieve the cell collection key" << endreq;
    return sc; 
  }     
  if ( store()->record(pCaloCellContainer, cellCollKey).isFailure() ) {
    msg() << MSG::ERROR << "Could not record a cell container in the RoI with key " << cellCollKey << endreq;
  }
  
  // Creating the cells containers...
  ToolHandleArray<IAlgToolEFCalo>::iterator itrtcr = m_tcrAlgTools.begin();
  ToolHandleArray<IAlgToolEFCalo>::iterator endtcr = m_tcrAlgTools.end();
  // Use specific Tools  to fill the container :
  int index=0;
  uint32_t error = 0;
  
  if (timerSvc()) m_timer[1]->start();

  for (; itrtcr!=endtcr; ++itrtcr) {
    
    //if (timerSvc()) m_timer[2+index]->start();
    StatusCode sc;

    if(m_fullScanEnabled) {
      sc= (*itrtcr)->execute(*pCaloCellContainer);
    } else {
      //      sc = (*itrtcr)->execute(*pCaloCellContainer, new TrigRoiDescriptor( eta0, etamin, etamax, phi0, phimin, phimax ) );
      sc = (*itrtcr)->execute(*pCaloCellContainer, *roiDescriptor );
    }
    
    if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Error executing TCR tool " << (*itrtcr).name() << endreq;
      return HLT::TOOL_FAILURE;
    } else {  
      uint32_t in_error = (*itrtcr)->report_error();
      unsigned int idet = (in_error>>28);
      if (0x0FFFFFFF & in_error) ++m_conversionError[idet];
      error|=(in_error&0x0FFFFFFF);
#     ifndef NDEBUG
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executed TCR tool " << (*itrtcr).name() << endreq;
#     endif
    }
    //if (timerSvc()) m_timer[2+index]->stop();
    
    ++index;
  }

  if (timerSvc()) m_timer[1]->stop();

  /// delete the Roi descriptor if we needed to create another (should really 
  /// add it to the navigation with the appropriate IDs !!! 
  //  if ( !m_trustRoiLimits && !m_useSuperRoi ) delete roiDescriptor;
  // if ( createdRoi ) delete roiDescriptor;

  if (pCaloCellContainer->size() < 1) return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);

  // Check conversion status
  pTrigCaloQuality->setPersistencyFlag(isPersistent);
  pTrigCaloQuality->setError(error);
  if (store()->record(pTrigCaloQuality, cellCollKey).isFailure()) {
    msg() << MSG::WARNING << "TrigCaloQuality cannot recorded with key=" 
	  << cellCollKey << endreq;
  } 
# ifndef NDEBUG
  else {
    msg() << MSG::DEBUG << "successfully record TrigCaloQuality with key=" << cellCollKey << " " << error << endreq;
  }
# endif

  // Looping over cell container tools... this could be used for calibrations...
  
  std::vector<ICaloCellMakerTool*> ::const_iterator itrcont=m_containerTools.begin();
  std::vector<ICaloCellMakerTool*> ::const_iterator endcont=m_containerTools.end();
  
  index=0;
  for (; itrcont!=endcont; ++itrcont) {
    if (timerSvc()) m_timer[2+index]->start();
    if ( (*itrcont)->process(pCaloCellContainer).isFailure() ) {
      //msg() << MSG::ERROR << "Error executing tool " << m_containerToolNames[index] << endreq;
      msg() << MSG::ERROR << "Error executing container tool " << endreq;
    } 
#ifndef NDEBUG
else {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executed container tool " << endreq;
    }
#endif
    if (timerSvc()) m_timer[2+index]->stop();
    ++index;
  }
  if ( m_fullScanEnabled) pCaloCellContainer->updateCaloIterators();
  (evtStore()->setConst( pCaloCellContainer ) ).ignore();
//if (timerSvc()) m_timer[1]->stop();

 /* // Protection against types used in methods API (uint16_t). Ask Tomasz.
  if (pCaloCellContainer->size()>= 65535) {
    msg() << MSG::WARNING << "CellContainer Size larger than 65535, Chain Aborted " << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
  }
*/

  cellCollKey = "";
  try {
    sc = reAttachFeature(outputTE, pCaloCellContainer, cellCollKey, persKey );
    if (sc != HLT::OK) {
      msg() << MSG::WARNING << "Could not record a cell container in the RoI with key " << cellCollKey << " " << name() << endreq;
    } 
#   ifndef NDEBUG
    else {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << " REGTEST: Recorded the cell container in the RoI " << endreq;
    }
#   endif
  } catch (std::exception& e) {
    msg() << MSG::WARNING << "HLTNavigation reached it's limit: " << e.what() << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
  }

# ifndef NDEBUG
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " REGTEST: Produced a Cell Container of Size= " << pCaloCellContainer->size() << endreq;
# endif

  //monitoring
  m_CellContainerSize = (float)pCaloCellContainer->size();

  /// create and attache an RoI for the ID Trigger
  if ( m_createRoiForID ) {

    if ( roiDescriptor->composite() ) { 
      msg() << MSG::WARNING << " Request to build an RoI for the ID From a composite RoiDescriptor" << *roiDescriptor  << endreq;
    }
    

    TrigRoiDescriptor* _roi = new TrigRoiDescriptor( roiDescriptor->roiWord(), 
						     roiDescriptor->l1Id(), 
						     roiDescriptor->roiId(),
						     roiDescriptor->eta(),
						     roiDescriptor->eta()-m_etaWidthForID, 
						     roiDescriptor->eta()+m_etaWidthForID, 
						     roiDescriptor->phi(), 
						     HLT::wrapPhi(roiDescriptor->phi()-m_phiWidthForID), 
						     HLT::wrapPhi(roiDescriptor->phi()+m_phiWidthForID) ); 

    if ( m_etaWidthForID==0 || m_phiWidthForID==0 )  {
      msg() << MSG::WARNING << "ZERO width RoI requested for the ID: " <<  *_roi << endreq;
    }

    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: attach RoI for ID " << *_roi << endreq;
    attachFeature( outputTE, _roi, "forID" );
  }


  // Time total TrigCaloCellMaker execution time.
  if (timerSvc()) {
     m_timer[0]->stop();
  }

  if (error) return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::USERDEF_2);

  ++m_counter;
  
  return HLT::OK; 
}
