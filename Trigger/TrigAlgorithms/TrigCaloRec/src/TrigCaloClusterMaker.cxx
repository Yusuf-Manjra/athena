/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigCaloClusterMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   C. Santamarina 
//           This is an Hlt algorithm that creates a cell container
//           with calorimeter cells within an RoI. Afterwards a cluster
//           container with the clusters made with those cells is
//           created with the standard offline clustering algorithms.
//
// ********************************************************************
//
#include <sstream>

//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "CaloInterface/ISetCaloCellContainerName.h"
//
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//
#include "CaloEvent/CaloCellContainer.h"
//
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
#include "CaloUtils/CaloCollectionHelper.h"
//
#include "CaloUtils/CaloClusterStoreHelper.h"
//
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloRec/CaloClusterProcessor.h"

#include "CaloEvent/CaloTowerContainer.h"
//
#include "TrigCaloRec/TrigCaloClusterMaker.h"
#include "TrigCaloRec/TrigCaloQuality.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "xAODCaloEvent/CaloClusterAuxContainer.h"
//#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

//
class ISvcLocator;


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigCaloClusterMaker::TrigCaloClusterMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
    m_pCaloClusterContainer(NULL),
    pTrigCaloQuality(NULL)
{

  // Eta and Phi size of the RoI window...

  // Name(s) of Cluster Maker Tools
  declareProperty("ClusterMakerTools",m_clusterMakerNames);

  // Name(s) of Cluster Correction Tools
  declareProperty("ClusterCorrectionTools",m_clusterCorrectionNames);


  // not needed since the key2key
  declareProperty("ClustersOutputName",m_clustersOutputName ="TriggerClusters");

  declareMonitoredVariable("ClusterContainerSize", m_ClusterContainerSize);

  declareMonitoredVariable("L2Eta",       m_L2Eta);
  declareMonitoredVariable("L2Phi",       m_L2Phi);
  
  // for leading cluster
  declareMonitoredVariable("containBadCells",       m_containBadCells);
  declareMonitoredVariable("eneFracMax",     m_eneFracMax);
  declareMonitoredVariable("clusterTime",    m_clusterTime);
  
  declareMonitoredStdContainer("Eta", m_Eta);
  declareMonitoredStdContainer("Phi", m_Phi);
  declareMonitoredStdContainer("Et",  m_Et);

  declareMonitoredStdContainer("EtaEFvsL2",   m_EtaEFvsL2);
  declareMonitoredStdContainer("PhiEFvsL2",   m_PhiEFvsL2);

}

/////////////////////////////////////////////////////////////////////
    // DESTRUCTOR:
    /////////////////////////////////////////////////////////////////////
    //
    TrigCaloClusterMaker::~TrigCaloClusterMaker()
{  }

/////////////////////////////////////////////////////////////////////
    // INITIALIZE:
    // The initialize method will create all the required algorithm objects
    // Note that it is NOT NECESSARY to run the initialize of individual
    // sub-algorithms.  The framework takes care of it.
    /////////////////////////////////////////////////////////////////////
    //

    HLT::ErrorCode TrigCaloClusterMaker::hltInitialize()
{
  msg() << MSG::DEBUG << "in initialize()" << endreq;
  
  // Global timers...
  if (timerSvc()) {
    m_timer.push_back(addTimer("TCCl_TimerTot"));
    m_timer.push_back(addTimer("TCCl_TimeClustMaker"));
    m_timer.push_back(addTimer("TCCl_TimeClustCorr"));
  }
  
  // Cache pointer to ToolSvc
  IToolSvc* toolSvc = 0;// Pointer to Tool Service
  if (service("ToolSvc", toolSvc).isFailure()) {
    msg() << MSG::FATAL << " Tool Service not found " << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  std::vector<std::string>::iterator itrName;
  std::vector<std::string>::iterator endName;

  for (int iC=1;iC<3;++iC){ 
    if (iC==1) {
      itrName = m_clusterMakerNames.begin();
      endName = m_clusterMakerNames.end();
    } else if (iC==2) {
      itrName = m_clusterCorrectionNames.begin();
      endName = m_clusterCorrectionNames.end();
    }

    for (; itrName!=endName; ++itrName) {

      ListItem theItem(*itrName);
      IAlgTool* algtool;

      if( toolSvc->retrieveTool(theItem.type(), theItem.name(), algtool,this).isFailure() ) {
	msg() << MSG::FATAL << "Unable to find tool for " << (*itrName) << endreq;
	return HLT::BAD_JOB_SETUP;
      } else {
	msg() << MSG::DEBUG << (*itrName) << " successfully retrieved" << endreq;
	if(iC==1) {
	  m_clusterMakerPointers.push_back(dynamic_cast<CaloClusterCollectionProcessor*>(algtool) );
	} else if (iC==2) {
	  m_clusterCorrectionPointers.push_back(dynamic_cast<CaloClusterProcessor*>(algtool) );
	}
        if(timerSvc() ) m_timer.push_back(addTimer("TCCl_"+theItem.name())); // One timer per tool
      }
    }
  }
 
  // end of helpers...
 
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG
	  << "Initialization of TrigCaloClusterMaker completed successfully"
	  << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigCaloClusterMaker::hltFinalize()
{
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in finalize()" << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigCaloClusterMaker::hltExecute(const HLT::TriggerElement* inputTE,
						HLT::TriggerElement* outputTE)
{
  // Time total TrigCaloClusterMaker execution time.
  if (timerSvc()) m_timer[0]->start();

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in execute()" << endreq;

  // Monitoring initialization...
  m_ClusterContainerSize = 0.;
  bool isSW=false;

  m_L2Eta=0.;
  m_L2Phi=0.;
  m_containBadCells=0;
  m_clusterTime=0;
  m_eneFracMax=0;
  m_Eta.clear();
  m_Phi.clear();
  m_Et.clear();

  m_EtaEFvsL2.clear();
  m_PhiEFvsL2.clear();


  // Some debug output:
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "outputTE->getId(): " << outputTE->getId() << endreq;
    msg() << MSG::DEBUG << "inputTE->getId(): " << inputTE->getId() << endreq;
  }
  //

  // Get RoiDescriptor

  const IRoiDescriptor*    roiDescriptor = 0;
  const TrigRoiDescriptor* tmproi = 0;

  HLT::ErrorCode sc = getFeature(inputTE, tmproi, ""); 
  if (sc != HLT::OK || tmproi==0 ) return sc;
  roiDescriptor = tmproi;
  
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "roi " << *roiDescriptor << endreq;

#if 0
  if (msgLvl() <= MSG::DEBUG) {
    double   eta0 = roiDescriptor->eta();
    double   phi0 = roiDescriptor->phi();
    while (phi0 > 2.*M_PI) phi0 -= 2. * M_PI;
    while (phi0 < 0. )     phi0 += 2. * M_PI;
    msg() << MSG::DEBUG << " eta0 = "<< eta0 << endreq;
    msg() << MSG::DEBUG << " phi0 = "<< phi0 << endreq;
  }
#endif

  m_L2Eta = roiDescriptor->eta();
  m_L2Phi = roiDescriptor->phi();

  // We now take care of the Cluster Making... 

  m_pCaloClusterContainer  = new xAOD::CaloClusterContainer();
  std::string clusterCollKey = "";

  pTrigCaloQuality = 0;
  std::string persKey = "";
  std::string persKeyLink = "";
  std::string cells_name = retrieveCellContName (outputTE);
  if (store()->retrieve( pTrigCaloQuality, cells_name ).isFailure()) {
    pTrigCaloQuality=0;
    msg() << MSG::WARNING << "cannot retireve TrigCaloQuality with key=" << cells_name << endreq;
  } else {
    persKey     = (pTrigCaloQuality->getPersistencyFlag() ? name() : "TrigCaloClusterMaker");
    persKeyLink = persKey + "_Link";
  }
  msg() << MSG::DEBUG << "CaloClusterContainer is stored with key  = " << persKey << endreq;
  msg() << MSG::DEBUG << "CaloCellLinkContainer is stored with key = " << persKeyLink << endreq;

  //  msg() << MSG::DEBUG << store()->dump() << endreq;
  sc = getUniqueKey( m_pCaloClusterContainer, clusterCollKey, persKey );
  if (sc != HLT::OK) {
    msg() << MSG::DEBUG << "Could not retrieve the cluster collection key" << endreq;
    return sc;
  }

  if (store()->record (m_pCaloClusterContainer, clusterCollKey).isFailure()) {
    msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << clusterCollKey << "> failed" << endreq;
    delete m_pCaloClusterContainer;
    return HLT::TOOL_FAILURE;
  }

  xAOD::CaloClusterAuxContainer aux;
  m_pCaloClusterContainer->setStore (&aux);
  


  // Looping over cluster maker tools... 
  
  if (timerSvc()) m_timer[1]->start(); // cluster makers time.

  int index=0;
  for (CaloClusterCollectionProcessor* clproc : m_clusterMakerPointers) {
    // We need to set the properties of the offline tools. this way of doing is ugly...
    // Abusing of harcoding?? Yes...
    if(m_clusterMakerNames[index].find("CaloTopoClusterMaker") != std::string::npos){
      AlgTool* algtool = dynamic_cast<AlgTool*> (clproc);
      
      if(!algtool || algtool->setProperty( StringProperty("CellsName",retrieveCellContName(outputTE)) ).isFailure()) {
        msg() << MSG::ERROR << "ERROR setting the CaloCellContainer name in the offline tool" << endreq;
        return HLT::TOOL_FAILURE;
      }

      //} else if(m_clusterMakerNames[index]=="CaloClusterBuilderSW/SWCluster"){
    } else if(m_clusterMakerNames[index].find("trigslw") != std::string::npos){
      AlgTool* algtool = dynamic_cast<AlgTool*> (clproc);
      if(!algtool || algtool->setProperty( StringProperty("CaloCellContainer",retrieveCellContName(outputTE)) ).isFailure()) { 
	msg() << MSG::ERROR << "ERROR setting the CaloCellContainer name in the offline tool" << endreq; 
        return HLT::TOOL_FAILURE; 
      } 
      if(!algtool || algtool->setProperty( StringProperty("TowerContainer",retrieveTowerContName(outputTE))).isFailure()) {
        msg() << MSG::ERROR << "ERROR setting the CaloCellContainer name in the offline tool" << endreq;
        return HLT::TOOL_FAILURE;
      }
    }
    

    if (timerSvc()) m_timer[3+index]->start();
    if ( (clproc->name()).find("trigslw") != std::string::npos ) isSW=true;
    if ( clproc->execute(m_pCaloClusterContainer).isFailure() ) {
      msg() << MSG::ERROR << "Error executing tool " << m_clusterMakerNames[index] << endreq;
    } else {
      msg() << MSG::DEBUG << "Executed tool " << m_clusterMakerNames[index] << endreq;
    }
    if (timerSvc()) m_timer[3+index]->stop();

    ++index;
  }
  if (timerSvc()) m_timer[1]->stop(); // cluster makers time.


//save raw state (uncalibrated)
       for (xAOD::CaloCluster* cl : *m_pCaloClusterContainer)
         {
           ATH_MSG_VERBOSE("found cluster with state "
               << cl->signalState());
	   cl->setRawE(cl->calE());
	   cl->setRawEta(cl->calEta());
	   cl->setRawPhi(cl->calPhi());
	   cl->setRawM(cl->calM());
         }
  
  

// Looping over cluster correction tools... 
  
  if (timerSvc()) m_timer[2]->start(); // cluster corrections time.
  std::vector<CaloClusterProcessor*>::const_iterator itrcct = m_clusterCorrectionPointers.begin();
  std::vector<CaloClusterProcessor*>::const_iterator endcct = m_clusterCorrectionPointers.end();
    
  index=0;
  for (; itrcct!=endcct; ++itrcct) {

    ISetCaloCellContainerName* setter =
      dynamic_cast<ISetCaloCellContainerName*> (*itrcct);
    if (setter) {
      std::string cells_name = retrieveCellContName (outputTE);
      if(setter->setCaloCellContainerName(cells_name) .isFailure()) {
        msg() << MSG::ERROR << "ERROR setting the CaloCellContainer name in the offline tool" << endreq;
        return HLT::BAD_JOB_SETUP;
      }
    }

    if (timerSvc()) m_timer[3+index+m_clusterMakerPointers.size()]->start(); // cluster corrections time.
    for (xAOD::CaloCluster* cl : *m_pCaloClusterContainer) {
      bool exec = false;
      if      ( (fabsf(cl->eta0())<1.45)  && ((*itrcct)->name().find("37") != std::string::npos ) ) exec=true;
      else if ( (fabsf(cl->eta0())>=1.45) && ((*itrcct)->name().find("55") != std::string::npos ) ) exec=true;
      else exec = false;
      if (!isSW) exec=true;
      if ( exec ) {
      if ( (*itrcct)->execute(cl).isFailure() ) {
        msg() << MSG::ERROR << "Error executing correction tool " <<  m_clusterCorrectionNames[index] << endreq;
        return HLT::TOOL_FAILURE;
      } else {
	msg() << MSG::DEBUG << "Executed correction tool " << m_clusterCorrectionNames[index] << endreq;
      }
      } // Check conditions
    }
    if (timerSvc()) m_timer[3+index+m_clusterMakerPointers.size()]->stop();
    ++index;
    
  }
  if (timerSvc()) m_timer[2]->stop(); // cluster corrections time.

  // quality flag for clusters
#if 0
  if (pTrigCaloQuality &&  pTrigCaloQuality->getError()) { // conversion errors in this RoI
    for (xAOD::CaloCluster* cl : *m_pCaloClusterContainer) {
      CaloClusterBadChannelData data(-999.,-999.,CaloSampling::Unknown,CaloBadChannel(4)); // unphysical data
      cl->addBadChannel(data);
    }
  }
#endif

  for (xAOD::CaloCluster* cl : *m_pCaloClusterContainer) {
    m_Et.push_back(cl->et());
    m_Eta.push_back(cl->eta());
    m_Phi.push_back(cl->phi());
    m_EtaEFvsL2.push_back(cl->eta()-m_L2Eta);
    m_PhiEFvsL2.push_back(cl->phi()-m_L2Phi);    
  }
  
  if( !m_pCaloClusterContainer->empty()) {
    xAOD::CaloCluster* cl = m_pCaloClusterContainer->front();
    double tmp = 0;
    cl->retrieveMoment (xAOD::CaloCluster::N_BAD_CELLS, tmp);
    m_containBadCells = tmp > 0;
    m_clusterTime     = cl->time();
    tmp = 0;
    cl->retrieveMoment (xAOD::CaloCluster::ENG_FRAC_MAX, tmp);
    m_eneFracMax = tmp;
  }
  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " REGTEST: Produced a Cluster Container of Size= " << m_pCaloClusterContainer->size() << endreq;
    if(!m_pCaloClusterContainer->empty()) {
      msg() << MSG::DEBUG << " REGTEST: Last Cluster Et  = " << (m_pCaloClusterContainer->back())->et() << endreq;
      msg() << MSG::DEBUG << " REGTEST: Last Cluster eta = " << (m_pCaloClusterContainer->back())->eta() << endreq;
      msg() << MSG::DEBUG << " REGTEST: Last Cluster phi = " << (m_pCaloClusterContainer->back())->phi() << endreq;
    }
  }
  

#if 0  
  // set the CaloClusterContainer property with the instance name and the RoI id...
  std::stringstream strm; strm << roiDescriptor->roiId();
  m_pCaloClusterContainer->setROIAuthor(m_clustersOutputName + "_" + strm.str());
#endif

  // record and lock the Clusters Container with the new EDM helper... 
  bool status = CaloClusterStoreHelper::finalizeClusters( store(), m_pCaloClusterContainer,
                                                        clusterCollKey, msg());

  if ( !status ) {  
    msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << clusterCollKey << "> failed" << endreq;
    return HLT::TOOL_FAILURE;
  } else {
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " REGTEST: Recorded the cluster container in the RoI " << endreq;
  }
    

  // Build the "uses" relation for the outputTE to the cell container
  std::string aliasKey = "";
  status = reAttachFeature(outputTE, m_pCaloClusterContainer, aliasKey, persKey );

  if (status != (bool)HLT::OK) {
    msg() << MSG::ERROR
	  << "Write of RoI Cluster Container into outputTE failed"
	  << endreq;
    return HLT::NAV_ERROR;
  }

  m_ClusterContainerSize = (float)m_pCaloClusterContainer->size();
  
  // get a pointer to caloclusterLink
  const CaloClusterCellLinkContainer* pCaloCellLinkContainer = 0;
  if (store()->retrieve( pCaloCellLinkContainer, clusterCollKey+"_links").isFailure()) {
    msg() << MSG::WARNING << "cannot get CaloClusterCellLinkContainer (not return FAILURE) " << endreq;
  }
  else {
    status = reAttachFeature(outputTE, pCaloCellLinkContainer, aliasKey, persKeyLink ); 
    if (status != (bool)HLT::OK) {
      msg() << MSG::ERROR
	    << "Write of RoI CellLink Container into outputTE failed"
	    << endreq;
    }
  }
  
  // Time total TrigCaloClusterMaker execution time.
  if (timerSvc()){
    m_timer[0]->stop();
  }

  return HLT::OK; 
}


std::string TrigCaloClusterMaker::retrieveCellContName( HLT::TriggerElement* outputTE )
{
  // We retrieve the CellContainer from the Trigger Element...
  std::vector<const CaloCellContainer*> vectorOfCellContainers;

  if( getFeatures(outputTE, vectorOfCellContainers, "") != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get TrigCells" << endreq;    
    return "";
  }
  
  msg() << MSG::DEBUG << "Got vector with " << vectorOfCellContainers.size() << " CellContainers" << endreq;
  
  // if no containers were found, just leave the vector empty and leave
  if ( vectorOfCellContainers.size() < 1) {
    msg() << MSG::ERROR << "No cells to analyse, leaving!" << endreq;
    return "";
  }

  // get last ccontainer to be put in vector (should also be the only one)
  const CaloCellContainer* theCellCont = vectorOfCellContainers.back();

  // All this only to retrieve the key : 
  std::string cellCollKey;
  if ( getStoreGateKey( theCellCont, cellCollKey) != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get key for TrigCells" << endreq;    
    return "";
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG  << " REGTEST: Retrieved the cell container in the RoI " << endreq;
    msg() << MSG::DEBUG << " REGTEST: Retrieved a Cell Container of Size= " << theCellCont->size() << endreq;
  }

  return cellCollKey;
}


std::string TrigCaloClusterMaker::retrieveTowerContName(HLT::TriggerElement* outputTE)
{
  // We retrieve the TowerContainer from the Trigger Element...
  std::vector<const CaloTowerContainer*> vectorOfTowerContainers;

  if ( getFeatures(outputTE, vectorOfTowerContainers, "") != HLT::OK ) {
    msg() << MSG::ERROR << "Failed to get TrigTowers" << endreq;
    return "";
  }
  
  msg() << MSG::DEBUG << "Got vector with " << vectorOfTowerContainers.size() << " TowerContainers" << endreq;
  
  // if no containers were found, just leave the vector empty and leave
  if ( vectorOfTowerContainers.size() < 1) {
    msg() << MSG::ERROR << "No towers to analyse, leaving!" << endreq;
    return "";
  }

  // get last ccontainer to be put in vector (should also be the only one)
  const CaloTowerContainer* theCont = vectorOfTowerContainers.back();

  // All this only to rebuild the key : 
  std::string towerCollKey;
  if ( getStoreGateKey( theCont, towerCollKey) != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get key for TrigTowers" << endreq;    
    return "";
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG  << " REGTEST: Retrieved the Tower container in the RoI " << endreq;
    msg() << MSG::DEBUG << " REGTEST: Retrieved a Tower Container of Size= " << theCont->size() << endreq;
  }

  //std::cout << "@@ Got key " << towerCollKey << std::endl;
  return towerCollKey;
}
