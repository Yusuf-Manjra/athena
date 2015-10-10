/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : TrigSteer
 *
 * @brief  TopAlgorithm for HLT Steering
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: TrigSteer.cxx,v 1.112 2009-05-05 20:29:14 tbold Exp $
 **********************************************************************************/

#include <algorithm>
#include <string>
#include <iostream>
#include <limits>

#include "TrigSteering/TrigSteer.h"

#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/AlgorithmTimer.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/ICoreDumpSvc.h"

#include "TrigSteering/Sequence.h"
#include "TrigSteering/TopoSequence.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/LvlConverter.h"
#include "TrigSteering/ResultBuilder.h"
#include "TrigSteering/ScalerSvc.h"
#include "TrigSteering/PeriodicScaler.h"
#include "TrigSteering/RandomScaler.h"
#include "TrigSteering/EventInfoAccessTool.h"

#include "TrigSteeringEvent/RobRequestInfo.h"


#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigInterfaces/Incidents.h"

#include "TrigNavigation/Navigation.h"


#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTUtils.h"
#include "TrigConfHLTData/HLTSignature.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "AthenaMonitoring/MonitorToolBase.h"
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"

#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoEvent/TopoInputEvent.h"
#include "L1TopoEvent/JetTOB.h"
#include "L1TopoCommon/Exception.h"
 
#include "TrigSteering/LvlTopoConverter.h"
#include "TrigConfInterfaces/IL1TopoConfigSvc.h"


using namespace HLT;
using namespace std;

TrigSteer::TrigSteer(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_config(0),
    m_log(0), 
    m_logLvl(0),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_l1topoConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_robDataProvider("ROBDataProviderSvc/ROBDataProviderSvc",name),
    m_navigation("HLT::Navigation/Navigation", this),
    m_scalerSvc("HLT::ScalerSvc/ScalerSvc", name),
    m_opiScaler("HLT::RandomScaler/OPIScaler", this),
    m_lvlCnvTool("HLT::Lvl1Converter/Lvl1Converter",this),
    m_lvlTopoCnvTool("HLT::LvlTopoConverter/LvlTopoConverter",this),
    m_resultBuilder("HLT::ResultBuilder/ResultBuilder", this),
    m_monTools( this ),
    m_opiTools( this ),
    m_incSvc("IncidentSvc", name),
    m_storeGate("StoreGateSvc", name),
    m_coreDumpSvc("CoreDumpSvc", name),
    m_executionOrderStrategy("HLT__OptimalExecutionOrderStrategy", this),
    m_EventInfoTool("HLT::EventInfoAccessTool/EventInfoAccessTool", this),
    m_timerSvc("TrigTimerSvc/TrigTimerSvc", name),    
    m_timerTotal(0),m_timerTotalAccepted(0), m_timerTotalRejected(0),
    m_timerLvlConverter(0), m_timerChains(0), m_timerResultBuilder(0), m_timerMonitoring(0), m_timerMonitoringSave(0),
    m_timerExec(0), m_timerCallEB(0), 
    m_abortTimer(0),
    m_incidentTimer(0, HLT::Incidents::EventTimeout::type())
{
   declareProperty("doTiming", m_doTiming = true, "switch for doing timing monitoring");
   declareProperty("doHypo", m_doHypo = true, "switch to run HYPO algorithms only");
   declareProperty("doFex", m_doFex = true, "switch to run FEX algorithms only");
   declareProperty("LvlConverterTool", m_lvlCnvTool, "tool for the trigger level conversion");
   declareProperty("LvlTopoConverter", m_lvlTopoCnvTool, "tool for the topological trigger conversion");
   declareProperty("HLTLevel", m_hltLevel = "L2", "Which trigger level: ('L2', 'EF', 'HLT')");
   declareProperty("ConfigSvc", m_configSvc, "HLT config svc -> to retrieve trigger configuration from (XML/DB/..?)");
   declareProperty("L1TopoTrigConfigSvc", m_l1topoConfigSvc, "Service to provide the L1Topo menu)");
   declareProperty("ROBDataProviderSvc", m_robDataProvider, "RobDataProvider for data fetching based on ROD ID");
   declareProperty("Navigation", m_navigation, "Trigger Navigation: takes care of all TriggerElements and the features");
   declareProperty("ScalerSvc", m_scalerSvc, "tool for pre-scalers & pass-through-scalers");
   declareProperty("ResultBuilder", m_resultBuilder, "tool that creates the trigger level result object");
   declareProperty("MonTools",  m_monTools, "List of private monitoring tools" );
   declareProperty("OPITools",  m_opiTools, "List of private OPI monitoring tools. They are different from MonTools because they run before result is built and can modify it" );
   declareProperty("OPIScaler", m_opiScaler, "Scaler used for the OPITools");
   declareProperty("softEventTimeout", m_softEventTimeout=-1, "Soft event timeout in nanoseconds.");
   declareProperty("hardEventTimeout", m_hardEventTimeout=-1, "Hard event timeout in nanoseconds.");  
   declareProperty("doOperationalInfo", m_doOperationalInfo=0, "Level of operational information collected by steering, by default =0 - do nothing.");
   declareProperty("sortChains", m_sortChains = 1, "If != 0 sorts chains by counter value before execution (and prescales application). The value <0 means ascending order while >0 disceding. ");
   declareProperty("ExecutionOrderStrategy", m_executionOrderStrategy, "Strategy tool defining which chains should run first in each step. Default is as orderd by sortChain directive.");
   declareProperty("cachingMode", m_cachingMode = HLT::RoICacheHelper::HistoryBased,
                   "Caching mode: 0 - off, 1 - RoI based, 2 - RoI +features, 3 - as 2 +check only used features, default is 3(most advanced)");
   declareProperty("enableCoherentPrescaling", m_enableCoherentPrescaling=false, "Enables coherent prescaling (so chains from one prescaling group fire in the same event)");
   declareProperty("enableRobRequestPreparation", m_enableRobRequestPreparation=false, "Enables ROB-request-preparation step");
   declareProperty("enableRerun", m_enableRerun=true, "Enables rerun of prescaled chains");
   declareProperty("EBstrategy", m_strategyEB = 0, "EB strategy: 0 = call by chains, 1 = call at the end of chains ");
   declareProperty("doL1TopoSimulation",m_doL1TopoSimulation=true,"Turns on L1Topo Sim");
   declareProperty("TopoOutputLevel", m_topoOutputLevel, "OutputLevel for L1Topo algorithms" );
   declareProperty("EventInfoAccessTool", m_EventInfoTool,"Tool to update the EventInfo at the end of the execution");
}


TrigSteer::~TrigSteer()
{
  if(m_config) {
    delete m_config->robRequestInfo();
    // delete m_config; -> up to Tomasz
  }
}


StatusCode TrigSteer::initialize()
{
   m_log = new MsgStream( msgSvc(), name());
   m_logLvl = m_log->level();

   ATH_MSG_DEBUG("start initialize");


   CHECK( m_storeGate.retrieve());


   // Get ROBDataProvider for data pre-fetch
   CHECK( m_robDataProvider.retrieve());

  
   // Setup the HLT ROB Data Provider Service when configured
   if ( &*m_robDataProvider ) {
#ifdef ATLAS_GAUDI_V21
      m_trigROBDataProvider = SmartIF<ITrigROBDataProviderSvc>( &*m_robDataProvider );
#else
      m_trigROBDataProvider = SmartIF<ITrigROBDataProviderSvc>( IID_ITrigROBDataProviderSvc, &*m_robDataProvider );
#endif
      if (m_trigROBDataProvider.isValid()) {
         (*m_log)  << MSG::INFO << " A ROBDataProviderSvc implementing the HLT interface ITrigROBDataProviderSvc was found."
                   << endreq;
      } else {
         (*m_log)  << MSG::INFO << " No ROBDataProviderSvc implementing the HLT interface ITrigROBDataProviderSvc was found."
                   << endreq;
      }
   }

   // get the navigation tool
   CHECK(m_navigation.retrieve());
   ATH_MSG_DEBUG("successfully retrieved navigation tool: " << m_navigation);


   // Create & set the config object (hlt runtime config), holding all common variables
   m_config = new HLT::AlgoConfig();
   m_config->setMsgStream(m_log);
   m_config->setMsgLvl(m_logLvl);
   m_config->setUseTiming(m_doTiming);
   m_config->setNavigation(&*m_navigation);
   m_config->setStoreGate(&*m_storeGate);
   m_config->setSteeringOPILevel(m_doOperationalInfo);
   m_config->setRobRequestInfo(new RobRequestInfo());

   // set the trigger level of this instance
   if (m_hltLevel == "L2")          m_config->setHLTLevel(HLT::L2);
   else if (m_hltLevel == "EF")     m_config->setHLTLevel(HLT::EF);
   else if (m_hltLevel == "HLT")    m_config->setHLTLevel(HLT::HLT);
   else                             m_config->setHLTLevel(HLT::UNKNOWN);
  
   // Get CoreDumpSvc to inject chain information
   CHECK(m_coreDumpSvc.retrieve());
   ATH_MSG_DEBUG("successfully retrieved the core dump service");
    
   // Get the TrigConfiguration Service
   CHECK(m_configSvc.retrieve());
   ATH_MSG_DEBUG("successfully retrieved config service: " << m_configSvc);

   //Get the L1TopoConfiguration Service
   ATH_MSG_INFO("L1Topo emulation activated: " << (m_doL1TopoSimulation ? "yes" : "no") );

   if(m_doL1TopoSimulation){
      CHECK(m_l1topoConfigSvc.retrieve());
      ATH_MSG_DEBUG("successfully retrieved L1Topo config service: " << m_l1topoConfigSvc);
   }

   // Get the ScalerSvc for Prescale/passthrough
   CHECK(m_scalerSvc.retrieve());
   ATH_MSG_DEBUG("successfully retrieved ScalerSvc: " << m_scalerSvc);


   if ( ScalerSvc::registerType( "HLT::RandomScaler" ) &&
        ScalerSvc::registerType( "HLT::PeriodicScaler", new ScalerFactory<PeriodicScaler>() ) && 
        ScalerSvc::registerType( "HLT::PeriodicScalerTake1st", new ScalerFactory<PeriodicScalerTake1st>() ) ) {
      (*m_log) << MSG::DEBUG << "Successfully configured ScalerSvc: " << m_scalerSvc << endreq;
   }

   if ( m_opiScaler.retrieve().isFailure() ) {
      (*m_log) << MSG::FATAL << "Failed to retrieve OPIScaler: " << m_opiScaler
               << endreq;
      return StatusCode::FAILURE;   
   }


   // find out which TE types should be produced at which level

   if (m_hltLevel == "HLT"){
      m_producedTEsHLT =  TrigConf::HLTUtils::allTEsProducedInLevel("HLT", *(m_configSvc->chainList()), *(m_configSvc->sequenceList()));
      for ( unsigned t = 0; t < m_producedTEsHLT.size(); t++ ) {
         (*m_log) << MSG::DEBUG << "Will produce at HLT TE of id: " << m_producedTEsHLT[t] << endreq;
      }
   } else {
      m_producedTEsL2 =  TrigConf::HLTUtils::allTEsProducedInLevel("L2", *(m_configSvc->chainList()), *(m_configSvc->sequenceList()));
      for ( unsigned t = 0; t < m_producedTEsL2.size(); t++ ) {
         (*m_log) << MSG::VERBOSE << "Will produce at L2 TE of id: " << m_producedTEsL2[t] << endreq;
      }
    
      m_producedTEsEF =  TrigConf::HLTUtils::allTEsProducedInLevel("EF", *(m_configSvc->chainList()), *(m_configSvc->sequenceList()));
      for ( unsigned t = 0; t < m_producedTEsEF.size(); t++ ) {
         (*m_log) << MSG::VERBOSE << "Will produce at EF TE of id: " << m_producedTEsEF[t] << endreq;
      }
   }
 
 
   //Get HLTChains from the ConfigSvc
   const TrigConf::HLTChainList* chainList =  m_configSvc->chainList();

   // create the TopoSteering instance and configure from the menu
   if(m_doL1TopoSimulation && (m_hltLevel == "L2" || m_hltLevel == "HLT") ) {

      //Loading the topo menu
      const TXC::L1TopoMenu *topomenu = m_l1topoConfigSvc->menu();

      if(topomenu == nullptr) {
         ATH_MSG_FATAL("No L1 Topo menu from " << m_l1topoConfigSvc->name());
         return StatusCode::FAILURE;
      }

      //create the L1Topo steering and configure it
      m_topoSteer = new TCS::TopoSteering();
      m_topoSteer->setMsgLevel( TrigConf::MSGTC::Level(m_topoOutputLevel) );
      m_topoSteer->setAlgMsgLevel( TrigConf::MSGTC::Level(m_topoOutputLevel) );

      try {
         m_topoSteer->setupFromConfiguration(*topomenu);
      }
      catch(TCS::Exception e) {
         ATH_MSG_FATAL("Caught std::exception when configuring steering from menu: " << e.what() );
      }
   }


   m_chains.reserve( chainList->size() );


   // create our own HLT::Chains
   for(TrigConf::HLTChain* confchain : *chainList) {
      // only if chain is part of this level:
      if (confchain->level() == m_hltLevel) {
         HLT::SteeringChain* chain = new HLT::SteeringChain( confchain, this, m_config, m_scalerSvc.operator->() );
         ATH_MSG_DEBUG(*confchain);
         m_chains.push_back(chain);
         if (!chain->isValid()) {
            ATH_MSG_ERROR("Error creating chain " << confchain->chain_name() << ", will now abort the job.");
            return StatusCode::FAILURE;
         }
      } else {
         ATH_MSG_DEBUG("ignore chain: " << confchain->chain_name() << " which is not configured as " << m_hltLevel);
      }
   }
   (*m_log) << MSG::INFO << "# Configured chain(s) at level " << m_hltLevel << ": " << m_chains.size() << endreq;



   if ( m_doHypo == false && m_doFex == false ) {
      (*m_log) << MSG::FATAL << "Setup inconsistent, doFex and doHypo set both to false" << endreq;
      (*m_log) << MSG::FATAL << "In normal conditions both should be true. While reruning doHypo=true, doFex=false" << endreq;
      (*m_log) << MSG::FATAL << "In NULL selection setup doHypo=false, doFex=true" << endreq;
      return StatusCode::FAILURE;
   }

   // Activate/deactivate HYPO/FEX execution
   HLT::Algo::m_isReRunning = !m_doFex;     // if doFex is true we do not work in "reruning" mode
   HLT::Algo::m_isNoSelection = !m_doHypo;  // is doHypo is true we do not work in "no selection" mode
   HLT::RoICacheHelper::s_cachingMode = (HLT::RoICacheHelper::CachingMode)m_cachingMode;

   // Get & Set LvlConverter:
   if (m_lvlCnvTool.empty()) {
      (*m_log) << MSG::FATAL << "no LvlConverter tool set in the jobOptions-> abort" << endreq;
      return StatusCode::FAILURE;
   }
   CHECK(m_lvlCnvTool.retrieve());
   ATH_MSG_DEBUG("Retrieved " << m_lvlCnvTool);
   m_lvlCnvTool->setConfig(m_config);
   m_lvlCnvTool->setConfiguredChains(m_chains);
   m_lvlCnvTool->setConfigurationKeys(m_configSvc->masterKey(), m_configSvc->hltPrescaleKey());



   // Get & Set LvlTopoConverter and topo Steering:
   if(m_doL1TopoSimulation) {
     
      ATH_MSG_INFO("Initializing LvlTopoConverter");
      CHECK(m_lvlTopoCnvTool.retrieve());

      m_lvlTopoCnvTool->setConfig(m_config);
      m_lvlTopoCnvTool->setConfiguredChains(m_chains);
      m_lvlTopoCnvTool->setConfigurationKeys(m_configSvc->masterKey(), m_configSvc->hltPrescaleKey());

   }


   // Create the ResultBuilder
   if (m_resultBuilder.empty()) {
      (*m_log) << MSG::FATAL << "no ResultBuilder tool set in the jobOptions-> abort" << endreq;
      return StatusCode::FAILURE;
   }
   CHECK( m_resultBuilder.retrieve() );
   ATH_MSG_DEBUG("Retrieved ResultBuilder" << m_resultBuilder);
   m_resultBuilder->setConfig(m_config);
   m_resultBuilder->setConfiguredChains(m_chains);
   m_resultBuilder->setConfigurationKeys(m_configSvc->masterKey(), m_configSvc->hltPrescaleKey());      


   if (m_doTiming) {
      // create timers
      if ( m_timerSvc.retrieve().isFailure() ) {
         (*m_log) << MSG::ERROR << "Requested timing measurements but can't retrieve TrigTimerSvc" << endreq;
         return  StatusCode::FAILURE;
      }
      (*m_log) << MSG::DEBUG << "Retrieved TrigTimerSvc" << endreq;
      m_timerTotal          = m_timerSvc->addItem(name()+":TotalTime");
      m_timerTotalAccepted  = m_timerSvc->addItem(name()+":TotalTimeAccepted");
      m_timerTotalRejected  = m_timerSvc->addItem(name()+":TotalTimeRejected");
      m_timerLvlConverter   = m_timerSvc->addItem(name()+":LvlConverterTime");
      m_timerChains         = m_timerSvc->addItem(name()+":ChainsTime");
      m_timerTotalRerun     = m_timerSvc->addItem(name()+":ChainsRerunTime");
      m_timerResultBuilder  = m_timerSvc->addItem(name()+":ResultBuilderTime");
      m_timerMonitoring     = m_timerSvc->addItem(name()+":MonitoringTime");
      m_timerMonitoringSave = m_timerSvc->addItem("DONOTUSE:"+name()+":MonitoringTimeSave");
      m_timerExec           = m_timerSvc->addItem(name()+":ExecTime");
      m_timerCallEB         = m_timerSvc->addItem(name()+":CallEBTime");
   }


   // Create OPI tools
   CHECK(m_opiTools.retrieve());
   ATH_MSG_DEBUG("Retrieved " << m_opiTools);


   // Create monitoring tools
   CHECK(m_monTools.retrieve());
   ATH_MSG_DEBUG("Retrieved " << m_monTools);
  
   //Create EventInfoAccess Tool
   CHECK( m_EventInfoTool.retrieve() );
   ATH_MSG_DEBUG("Retrieved " << m_EventInfoTool);


   // Add Incident listeners
   CHECK( m_incSvc.retrieve() );
   ATH_MSG_DEBUG("Adding listener for event timeout");
   m_incSvc->addListener(this, "EventTimeout");

  
   // reset all configured chains, so that in any case
   // the first event sees exactly the same chains as all other events!
   if (!resetChains(m_chains) ) {
      (*m_log) << MSG::ERROR << "Failed to reset vector of all configured HLT::Chain objects!" << endreq;
   }

   //  (*m_log) << MSG::INFO << "m_prescaleBeforeExecution = " << m_prescaleBeforeExecution << endreq;
   //  (*m_log) << MSG::INFO << "m_calculatePrescaledChains = " << m_calculatePrescaledChains << endreq;
  
   if ( m_hardEventTimeout > 0 )
      (*m_log) << MSG::WARNING << "Hard event timeout set (" << m_hardEventTimeout/1e6 << " ms)" << endreq;
   if ( m_softEventTimeout > 0 )
      (*m_log) << MSG::WARNING << "Soft event timeout set (" << m_softEventTimeout/1e6 << " ms)" << endreq;

   if (m_enableCoherentPrescaling) 
      configureCoherentPrescaling();


   if( m_executionOrderStrategy.retrieve().isFailure() ) {
      (*m_log) << MSG::WARNING << "Can not retrieve chains ordering tool: " << m_executionOrderStrategy << " will continue w/o it" << endreq;    
   } else {

      CHECK( m_executionOrderStrategy->prepare(m_chains) );
      //    (*m_log) << MSG::WARNING << "Can not prepare chains ordering tool: " << m_executionOrderStrategy << " will continue w/o it" << endreq;    
   }




 
   ATH_MSG_DEBUG("initializing done");

   return StatusCode::SUCCESS;
}




StatusCode TrigSteer::finalize()
{
   ATH_MSG_DEBUG("finalizing");

   // delete all chains
   for(HLT::SteeringChain* chain : m_chains) delete chain;
   m_chains.clear();
   m_activeChains.clear();
   ATH_MSG_DEBUG("finalized chains");

   // delete all sequences
   for(HLT::Sequence * seq : m_sequences) delete seq;
   m_sequences.clear();
   m_algos.clear();
   ATH_MSG_DEBUG("finalized sequences");

   delete m_log; m_log=0;
   delete m_config; m_config=0;

   return StatusCode::SUCCESS;
}


class OPISteerHelper {
public:

  OPISteerHelper(TrigTimer* timer, TrigOperationalInfo* opi) 
    : m_timer(timer), m_opi(opi) {}
  
  ~OPISteerHelper() {
    if(m_timer) { 
      m_timer -> stop();
      if(m_opi) m_opi -> set("Texec", m_timer->elapsed());
    }
  }
  
private:

  OPISteerHelper();
  OPISteerHelper(const OPISteerHelper &);
  OPISteerHelper& operator=(const OPISteerHelper &);

  TrigTimer *m_timer;
  TrigOperationalInfo *m_opi;
};




StatusCode TrigSteer::execute()
{  
   if (m_doTiming) {
      m_timerTotal->start();
      m_timerTotalAccepted->start();
      m_timerTotalRejected->start();
      m_timerExec->start();
      m_timerCallEB->start();
   }

   // some debug output
   ATH_MSG_DEBUG("Executing TrigSteer for new event");
   ATH_MSG_DEBUG("//////////////////////////////////////////////////\n");
   ATH_MSG_DEBUG("/////////  Start of HLT Processing in " << m_config->getInstance().substr(1,3) << " /////////");
   ATH_MSG_DEBUG("//////////////////////////////////////////////////");

   // reset states
   m_config->reset();

   // reset Navigation
   m_navigation->reset();

   // clear vector of chains
   m_activeChains.clear();

   //reset EB status (for merged system)
   m_stepForEB = 0;

   if ( setEvent() != HLT::OK ) {
      return StatusCode::FAILURE;
   }

   // Create OPI fater setEvent() - in case it fails
   TrigOperationalInfo* operationalInfo = new TrigOperationalInfo();
  

   // Execute LvlConverter, this contains:
   // - activate chains according to the active chains of the previous Trigger Level,
   // - create TriggerElements either from Level 1 RoIs or from previous TriggerElements
   if (m_doTiming) m_timerLvlConverter->start();

   HLT::ErrorCode ec = m_lvlCnvTool->hltExecute(m_activeChains);
   sortChains(m_activeChains);

   if(m_doL1TopoSimulation) {
      m_topoSteer->reset();

      // fill the input event
      //HLT::ErrorCode topoEC = 
      if( m_lvlTopoCnvTool->hltExecute(m_activeChains, m_topoSteer->inputEvent() ) != HLT::OK) {
         return StatusCode::FAILURE;
      }

      // forward the mapping ( roiWord -> TE ) to the TopoSequences
      const map<uint32_t, TriggerElement*> * roiWord2TEMap = m_lvlCnvTool->roIWordToTE();
      for( HLT::Sequence * seq : m_sequences) {
         TopoSequence * topoSeq = dynamic_cast<TopoSequence*>(seq);
         if(topoSeq)
            topoSeq->setRoi2TEMap( roiWord2TEMap );
      }
   }

   // reorder chains order so they run optimally
   m_executionOrderStrategy->apply(m_activeChains);
  
   m_config->setLvlConverterStatus(ec);
   m_config->setHltStatus(ec);
   if (m_doTiming) {
      m_timerLvlConverter->stop();
      operationalInfo->set("Tconv", m_timerLvlConverter->elapsed() );
   }
   // prepare navigation (preregistration of objects)
   m_navigation->prepare();    

   operationalInfo->set("Nch", m_activeChains.size()); // this is here to make it easy to plot correlation nchains.vs.time
   operationalInfo->set("Nroi", m_navigation->countAllOfType(0, false)-1.0); // because the RoI nodes and initial node have id 0 (so all nodes of id 0 -1 give # of RoIs)

   // DEBUG output: print all activated HLT Chains
   if (m_config->getMsgLvl() <=MSG::DEBUG) {
      (*m_log) << MSG::DEBUG << "Working with "<< m_activeChains.size() <<" active chains in this level: "
               << endreq;
      for (std::vector<HLT::SteeringChain*>::iterator iterChain = m_activeChains.begin();
           iterChain != m_activeChains.end(); ++iterChain) {
         (*m_log) << MSG::DEBUG << "Active: " << *(*iterChain) << " lower_chain: " << (*iterChain)->getLowerChainName() <<" EB after step: "<<(*iterChain)->getEBAfterStep() << endreq;

      }
   }

   /*
    * Create the timers for the hard/soft event timeout.
    * Both disable themselves once the ScopeAlarm object goes out
    * of scope, i.e. at the end of the event.
    */
   Athena::ScopedTimer hardTimeout(static_cast<unsigned int>(m_hardEventTimeout/1e6));
   Athena::ScopedTimer softTimeout(static_cast<unsigned int>(m_softEventTimeout/1e6));
   if (m_hardEventTimeout > 0) m_abortTimer.start(hardTimeout);
   if (m_softEventTimeout > 0) m_incidentTimer.start(softTimeout);
  
   if (m_doTiming) m_timerChains->start();

   if (m_doOperationalInfo > 0  )
      m_config->setSteeringOPILevel(m_opiScaler->decision(m_doOperationalInfo));

   // at the begin of the event reset all algorithms (ROB list preparation is also cached)
   resetChainsROBRequestPreparation(m_activeChains);
  
   if ( canContinueEvent(ec) ) {
      //=========== here the trigger decision is calculated =============
      // run on all non-prescaled chains      
      runChains(false);    
   } else {
      (*m_log) << MSG::DEBUG << "LvlConverter returned: " << strErrorCode(ec) << endreq;
   }

   // figure out if event is passed on physics stream

   bool eventPassed = false;  
   bool isPhysicsAccept = false;

   for (std::vector<HLT::SteeringChain*>::iterator iterChain = m_activeChains.begin();
        iterChain != m_activeChains.end(); ++iterChain) {
     // check whether the event is accepted
     eventPassed = (*iterChain)->chainPassed() || eventPassed;
     if ( (*iterChain)->chainPassed()){
       for (auto chain_stream : (*iterChain)->getStreamTags()){
	  if ( chain_stream.getType() == "physics" ){
	    isPhysicsAccept=true;
	    (*m_log) << MSG::DEBUG << "FPP chain "<< (*iterChain)->getChainName()  <<" gives Physics Accepts" << endreq;
	    break;
	  }
	}
     }
     if (isPhysicsAccept) break;
   }

   // run on the prescaled chains, but only if trigger decision was positive
   if (m_doTiming) m_timerTotalRerun->reset();

   if (m_enableRerun){
      if (eventPassed && isPhysicsAccept && canContinueEvent(ec) ) {
	(*m_log) << MSG::DEBUG << "FPP Start rerun " << endreq;
         // calculate trigger decision for prescaled chains
         if (m_doTiming) m_timerTotalRerun->start();
         runChains(true);
         if (m_doTiming) m_timerTotalRerun->stop();
      } 
   }

   if (m_doTiming) {
      m_timerChains->stop();
      operationalInfo->set("Tproc", m_timerChains->elapsed() );
   }

   if (m_doFex) { 
      // if the flag doFex is set to false that means we are re-running in this case no operation on navigation like this can be done
      std::string key;  
      m_navigation->attachFeature(m_navigation->getInitialNode(), operationalInfo, 
                                  HLT::Navigation::ObjectCreatedByNew, key, "OPI"+m_config->getInstance() );
   }
   else {
      delete operationalInfo;
      operationalInfo = 0;
   }

   // Create scoped timer after OPI ownership is settled
   OPISteerHelper execTimer(m_timerExec, operationalInfo);

   // Produce the result, even if chains were not executed
   if (m_doTiming) m_timerResultBuilder->start();

   HLT::ErrorCode ec2 = m_resultBuilder->fillTriggerInfo(m_activeChains);
   m_config->setHltStatus(std::max(  m_config->getHltStatus(), ec2));

   if ( ec2 != HLT::OK ) {
      (*m_log) << MSG::ERROR << "failed to fill trigger info: " << strErrorCode(ec2) << endreq;
   }

   //called after the TriggerInfo is filled
   if ( m_EventInfoTool->updateStreamTag(m_activeChains) != StatusCode::SUCCESS){
     (*m_log) << MSG::FATAL << "Failed to update the EventInfo" << endreq;
     return StatusCode::FAILURE;
   }  

   ToolHandleArray< IMonitorToolBase >::iterator itOPI = m_opiTools.begin();
   ToolHandleArray< IMonitorToolBase >::iterator itOPIEnd = m_opiTools.end();
   for (  ; itOPI != itOPIEnd; ++itOPI ) {
      if ( (*itOPI)->fillHists().isFailure() ) {
         (*m_log) << MSG::WARNING << "tool " << (*itOPI).typeAndName() << " failed it's task: fillHists()" << endreq;
      }
   }

   ec2 = m_resultBuilder->hltExecute(m_activeChains);
   m_config->setHltStatus(std::max(  m_config->getHltStatus(), ec2));

   if (m_doTiming) m_timerResultBuilder->stop();

   if ( ec2 != OK ) {
      (*m_log) << MSG::ERROR << "ResultBuilder algorithm failed with: " << strErrorCode(ec2) << endreq;
   }

  

   if (m_doTiming) {
      if ( eventPassed )
         m_timerTotalAccepted->stop();
      else
         m_timerTotalRejected->stop();
      m_timerTotal->stop();
   }

   // execute all monitoring tools
   if (m_doTiming) {
      *m_timerMonitoring = *m_timerMonitoringSave;
      m_timerMonitoringSave->start();
   }

   ToolHandleArray< IMonitorToolBase >::iterator itMon = m_monTools.begin();
   ToolHandleArray< IMonitorToolBase >::iterator itMonEnd = m_monTools.end();
   for (  ; itMon != itMonEnd; ++itMon ) {
      if ( (*itMon)->fillHists().isFailure() ) {
         (*m_log) << MSG::WARNING << "tool " << (*itMon).typeAndName() << " failed it's task: fillHists()" << endreq;
      }
   }

   if (m_doTiming) { 
      m_timerMonitoringSave->stop();
      if(operationalInfo) {
         operationalInfo->set("Tres", m_timerResultBuilder->elapsed() );
         operationalInfo->set("Tmon", m_timerMonitoringSave->elapsed() );
      }
   }

   // reset all used (activated) chains for new event:
   if (!resetChains(m_chains)) {
      (*m_log) << MSG::FATAL << "Failed to reset vector of active HLT::Chain objects!" << endreq;
      return StatusCode::FAILURE;
   }

   // some debug output
   if (m_logLvl <= MSG::DEBUG) {
      (*m_log) << MSG::DEBUG << "\n ///////////////////////////////////////////////\n";
      (*m_log) << MSG::DEBUG << " /////////  Event processing finished  ///////// \n";
      (*m_log) << MSG::DEBUG << " ///////////////////////////////////////////////" << endreq;
      (*m_log) << MSG::DEBUG << "Most severe error: " <<  strErrorCode(m_config->getHltStatus()) << endreq;
   }

   if ( !canContinueJob() ) {
      (*m_log) << MSG::FATAL << "Errors were too severe in this event will abort the job ..." << endreq;
      return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;
}


void  TrigSteer::doPrefetching(bool &secondPass, bool& noError){
  (*m_log) << MSG::DEBUG << "TrigSteer::doPrefetching starts " << m_stepForEB <<endreq;

  // skip if EB already called in the steering
  if( m_stepForEB != 0) return;// should be useless


  //  skip if the event is already in cache
  if (m_trigROBDataProvider.isValid()) {
    if (m_trigROBDataProvider->isEventComplete()){ // this is return always treu for offline running
      (*m_log) << MSG::DEBUG <<"FPP Event is complete" <<endreq;
      if ( m_strategyEB == 0){ // return for online, do the prefetching for testing EBstrategy=1
	(*m_log) << MSG::DEBUG <<"FPP Event is complete; do not pre-fetch data for online running" <<endreq;
	return;
      }
    }
  }

  // Prepare ROB requests at this step:
    
  // preparation of ROB requests is first thing to do in each step
  // (prior to chain execution): the ROB IDs needed by each
  // algorithm are collected

  if (m_config->getMsgLvl() <=MSG::DEBUG)
    (*m_log) << MSG::DEBUG << "\n ///\n /// Start collecting ROB requests\n ///\n" << endreq;


  // clear the list of ROB IDs at the beginning of each step
  getAlgoConfig()->robRequestInfo()->clearRequestScheduledRobIDs();
     
  for (std::vector<HLT::SteeringChain*>::iterator iterChain = m_activeChains.begin();
       iterChain != m_activeChains.end(); ++iterChain) {
    //    (*m_log) << MSG::DEBUG <<(*(*iterChain))<<endreq;

    // first perform same chain selection as done before execution (see below)
    
    // first pass (do not run prescaled chains)
    if ( ! secondPass && ! (*iterChain)->runInFirstPass() ) continue;
    
    // second pass when we want to run all chains which were to be resurrected and were prescaled before
    if ( secondPass  && ! (*iterChain)->runInSecondPass() ) continue;
    
    if ((*iterChain)->isMerged() && (*iterChain)->nextStepAfterEB()!=0) continue; //skip merged chains waiting for EB
    
    HLT::ErrorCode chainCode = (*iterChain)->prepareStepRobRequests();
    
    
    // evaluate the error code returned by the ROB request preparation
    if ( chainCode.action() > HLT::Action::CONTINUE )
      chainCode = HLT::ErrorCode(chainCode.action(), chainCode.reason(), 
				 HLT::SteeringInternalReason::ALGO_ERROR);
    m_config->setHltStatus( chainCode > m_config->getHltStatus() ? chainCode : m_config->getHltStatus() ); // keep ErrorCode in m_config always up to date
    
    if (m_config->getMsgLvl() <=MSG::DEBUG) 
      (*m_log) << MSG::DEBUG << "Executed ROB request from chain: " << **iterChain << " in " << (secondPass ? "second pass" : "first pass")  << endreq; 
    
    if ( !canContinueEvent(chainCode) ) {
      (*m_log) << MSG::WARNING << "Chain " << (*iterChain)->getChainName()
	       << " returned HLT::ErrorCode = " << strErrorCode(chainCode) << " :"
	       << " any further execution in this event is stopped!" << endreq;
      noError = false;
      break;
    }
  }
  

  // need to request ROBs from data provider
  issueRobRequests();

  if (m_config->getMsgLvl() <=MSG::DEBUG)
    (*m_log) << MSG::DEBUG << "\n ///\n /// Done collecting and issuing ROB requests\n ///\n" << endreq;

}



void TrigSteer::runChains(bool secondPass) {
  if (m_config->getMsgLvl() <=MSG::DEBUG)
    (*m_log) << MSG::DEBUG <<"runChains(): m_activeChains="<<m_activeChains.size() << endreq;

  bool chainsStillActive =  m_activeChains.size()>0;
  bool noError = true;
  bool eventPassed = false;
  int  step = 0; //just for debug: follow numeration of signature counters: starts from 1 (but currentStep in chains starts from 0)
  bool doEBbyChain = false;

  while ( chainsStillActive && noError ) {
    step++;
    if (m_config->getMsgLvl() <=MSG::DEBUG) {
      (*m_log) << MSG::DEBUG << "\n /////////////////////////////////////////////////\n";
      (*m_log) << MSG::DEBUG << " /////////  Executing next Step " << step << " (" <<  (secondPass ? "second pass" : "first pass") << ")\n";
      (*m_log) << MSG::DEBUG << " /////////////////////////////////////////////////" << endreq;
    }

    chainsStillActive = false;

    // Prepare ROB requests:
    if (m_enableRobRequestPreparation)  doPrefetching(secondPass, noError);
   
    
    for (std::vector<HLT::SteeringChain*>::iterator iterChain = m_activeChains.begin();
         iterChain != m_activeChains.end(); ++iterChain) {

      // select (non-)prescaled chains, depending on function argument
      if ( secondPass == false ) { 
        // first pass (do not run prescaleed chains)
        if ( ! (*iterChain)->runInFirstPass()) continue;
      }
      if ( secondPass == true ) {
        //second pass when we want to run all chains which were to be Resurrected and were prescalled before
        if ( ! (*iterChain)->runInSecondPass()) continue;
      }

 
      // In case we crash, CoreDumpSvc will print this info
      m_coreDumpSvc->setCoreDumpInfo("Current trigger chain",(*iterChain)->getChainName());

      // execute step
      HLT::ErrorCode chainCode = (*iterChain)->executeStep();
      if ( chainCode.action() > HLT::Action::CONTINUE )
        chainCode = HLT::ErrorCode(chainCode.action(), chainCode.reason(), 
                                   HLT::SteeringInternalReason::ALGO_ERROR);
      m_config->setHltStatus( chainCode > m_config->getHltStatus() ? chainCode : m_config->getHltStatus() ); // keep ErrorCode in m_config always up to date
      
      /* std::cout <<"FPP: TrigSteer::runChains():  step "<<step<<" chain "<< (*iterChain)->getChainName() 
         << " EBAfterStep="<<(*iterChain)->getEBAfterStep()  
         << " isActive="<< (*iterChain)->isActive() 
         << " passed="<<(*iterChain)->chainPassed()
         << " passedRaw="<< (*iterChain)->chainPassedRaw() 
         << " isPrescaled= " << (*iterChain)->isPrescaled()
         << " isResurrected="  << (*iterChain)->isResurrected()
         << " isPassedThrough="<< (*iterChain)->isPassedThrough()
         <<" ->requestScheduledRobIDs().size()="<<getAlgoConfig()->robRequestInfo()->requestScheduledRobIDs().size()
         <<" HLT status="<<m_config->getHltStatus() << " chaincode="<<chainCode <<std::endl; */

      if (m_config->getMsgLvl() <=MSG::DEBUG) 
        (*m_log) << MSG::DEBUG << "Executed chain: " << **iterChain << " in " << (secondPass ? "second pass" : "first pass")  << endreq; 
      
      if ( !canContinueEvent(chainCode) ) {
        (*m_log) << MSG::WARNING << "Chain " << (*iterChain)->getChainName()
                 << " returned HLT::ErrorCode = " << strErrorCode(chainCode) << " :"
                 << " any further execution in this event is stopped!" << endreq;
        noError = false;
        break;
      }

      eventPassed = (*iterChain)->chainPassed() || eventPassed;
      //EB strategy for merged HLT:
      // strategy = 0: with final menu, call the EB when
      //   - one chain is completed and accepted the event
      //   - one chain requires the EB
      // strategy = 1: call EB at the end of processing (L2-like)

      if (( m_hltLevel == "HLT" ) && m_strategyEB == 0 ){ //call EB if requested by the chain
        // temporary add getEBAfterStep()>0. since  nextStepAfterEB() is buggy and cannot be included with a new TrigSteeringEvent tag (TrigSteeringEvent-01-03-01 is ok)
        doEBbyChain =  (*iterChain)->isActive() && (*iterChain)->nextStepAfterEB() && ((*iterChain)->getEBAfterStep()>0.) ;
        if( doEBbyChain || eventPassed){
          if ( doEBbyChain )
            (*m_log) << MSG::DEBUG << "Call EB at step " << step << " because chain needs it" << endreq;
          else 
            (*m_log) << MSG::DEBUG << "Call EB at step " << step << " because the event is accepted" << endreq;
	  
          issueEventBuildingRequest(step);
        }
      }
      
      chainsStillActive = (*iterChain)->isActive() || chainsStillActive;
    }   
  }


  // final call of EB at the end of steps (done here if strategyEB == 1)
  if (eventPassed){
    (*m_log) << MSG::DEBUG << "Call EB at step " << step << " at the end of steps processing because not done previously" << endreq;
    issueEventBuildingRequest(step);
  }

  m_coreDumpSvc->setCoreDumpInfo("Current trigger chain","<NONE>");
}



void TrigSteer::issueEventBuildingRequest(int step) {
  // issue the EB request through the TrigROBDataProviderSvc
  // only if not already requested 
  // for offline running, just set the EB step, doing nothing else

  // check if complete event has been already requested (by DCM or Steering)
  if (m_trigROBDataProvider.isValid() && m_trigROBDataProvider->isEventComplete()) {
    if ( m_stepForEB == 0 ){ // record this step as negative, in case EB is not called by the steering
      m_stepForEB = (-1.) * step;
      if (m_config->getMsgLvl() <=MSG::DEBUG) 
        (*m_log) << MSG::DEBUG << "Request of Event Building already issued, not by Steering! " << endreq; 
    } else {
      if (m_config->getMsgLvl() <=MSG::DEBUG) 
        (*m_log) << MSG::DEBUG << "Request of Event Building already issued by Steering! " << endreq; 
    }
    return;
  }

   
  // set the EB step now
  if (m_doTiming) m_timerCallEB->stop();
  m_stepForEB = step;

  // return if running is offline 
  if (!m_trigROBDataProvider.isValid()) {
    if (m_config->getMsgLvl() <=MSG::DEBUG) 
      (*m_log) << MSG::DEBUG << "Request of Event Building is issued for offline running at step " << m_stepForEB << endreq; 
    return;
  }

  // issue the EB
  if (m_config->getMsgLvl() <=MSG::DEBUG) 
    (*m_log) << MSG::DEBUG << "Going to issue Event Building at step " << m_stepForEB << endreq; 
  m_trigROBDataProvider->collectCompleteEventData(name());

  return;
}

void TrigSteer::issueRobRequests() {

  if (m_config->getMsgLvl() <=MSG::DEBUG) 
    (*m_log) << MSG::DEBUG << "Going to issue ROB requests (#ROB IDs = " << getAlgoConfig()->robRequestInfo()->requestScheduledRobIDs().size() << ")" << endreq; 

  m_robDataProvider->addROBData(getAlgoConfig()->robRequestInfo()->requestScheduledRobIDs());
}

bool TrigSteer::resetChains(std::vector<HLT::SteeringChain*>& chains)
{
  for (std::vector<HLT::SteeringChain*>::iterator iterChain = chains.begin();
       iterChain != chains.end(); ++iterChain) {
    if ( !(*iterChain)->resetChain() ) {
      (*m_log) << MSG::WARNING << "Failed to reset chain" << (*iterChain)->getConfigChain()->chain_name() << endreq; 
      return false;
    }
  }
  (*m_log) << MSG::DEBUG << "Chains reset went ok"  << endreq; 
  return true;
}


bool TrigSteer::resetChainsROBRequestPreparation(std::vector<HLT::SteeringChain*>& chains)
{
  for (std::vector<HLT::SteeringChain*>::iterator iterChain = chains.begin();
       iterChain != chains.end(); ++iterChain) {
    if ( !(*iterChain)->resetROBRequestPreparation() ) return false;
  }
  return true;
}

// Helper method, to get substring of defined Algorithm name
void findAlgTypeName(const std::string& property, std::string& type_name,
		     std::string& instance_name)
{
  int slash_pos = property.find_first_of("/");
  type_name = property.substr( 0, slash_pos );
  instance_name = (slash_pos > 0) ? property.substr( slash_pos + 1) : type_name;
  replace(instance_name.begin(), instance_name.end(), '/', '_');
  replace(instance_name.begin(), instance_name.end(), '/', '_');
}



HLT::Algo* TrigSteer::getAlgo(std::string name)
{
  HLT::Algo* algo = 0;

  std::string subAlg_type, subAlg_name;

  findAlgTypeName(name, subAlg_type, subAlg_name);
  //  findAlgTypeName(subAlg_NameParam, algNameFromConfig, subAlg_param);
  //  subAlg_name = subAlg_Name;// + "_" + subAlg_param + m_config->getInstance();

  std::map<std::string, HLT::Algo*>::const_iterator iter = m_algos.find(subAlg_name);
  if (iter != m_algos.end())
    algo = iter->second;
  else {
    Algorithm* algPointer;
    if (createSubAlgorithm(subAlg_type, subAlg_name, algPointer).isSuccess()) {
      algo = static_cast<HLT::Algo*>(algPointer);
      algo->setConfig(m_config);
      m_algos[subAlg_name] = algo;
      (*m_log) << MSG::DEBUG << "createSubAlgorithm: created " << subAlg_name << " of type "
               << subAlg_type << " (" << name << ")" << endreq;
    } else {
      (*m_log) << MSG::WARNING << "createSubAlgorithm: '"
               << subAlg_type << "/" << subAlg_name << "' FAILED" << endreq;
      algo=0;
    }
  }

  // tune algorithms settings
  // if the steering is configured to run either hypo only or fex only we do not need to fex or hypo algorithms respectively
  if ( dynamic_cast<HLT::HypoAlgo*>(algo) ) {
    BooleanProperty enabled("Enable", m_doHypo);
    algo->setProperty(enabled).ignore();
    algo->setProperties().ignore();
    (*m_log) << MSG::DEBUG <<( m_doHypo ? "enabling " : "disabling ") << subAlg_name << " because it is Hypo" << endreq;
  }
  if ( dynamic_cast<HLT::TECreateAlgo*>(algo) ) {
    BooleanProperty enabled("Enable", m_doFex);
    algo->setProperty(enabled).ignore();
    algo->setProperties().ignore();
    ATH_MSG_DEBUG((m_doFex ? "enabling " : "disabling ") << subAlg_name << " because it is Fex");
  }

  return algo;
}


std::string TrigSteer::producedFirstAtLevel(unsigned int id) {
  if ( std::find(m_producedTEsL2.begin(), m_producedTEsL2.end(), id) != m_producedTEsL2.end() )    return "L2";
  if ( std::find(m_producedTEsEF.begin(), m_producedTEsEF.end(), id) != m_producedTEsEF.end() )    return "EF";
  if ( std::find(m_producedTEsHLT.begin(), m_producedTEsHLT.end(), id) != m_producedTEsHLT.end() ) return "HLT";
  return "NONE";
}






HLT::Sequence* TrigSteer::findSeqForOutputTeType(const unsigned int teType)
{

   if (m_config->getMsgLvl() <= MSG::DEBUG) {
      std::string teName;
      TrigConf::HLTTriggerElement::getLabel(teType, teName);
      (*m_log) << MSG::DEBUG << "in findSeqForOutputTeType " << teName << " [" << teType << "]" << endl;
   }

   // look if sequence already exists
   for (HLT::Sequence* seq : m_sequences)
      if ( seq->outputTEType() == teType) return seq;


   // If we reach this point, no sequence producing the given teType was found.
   // So we search for this sequence among all configured sequences...

   // loop over all configured TrigConf::HLTSequences and look for the one
   // producing the given teType ... if we can find it
   // - create HLT::Algorithm(s) (save a pointer in the global list)
   // - create our own HLT::Sequences (save a pointer in the global list)

   for (TrigConf::HLTSequence* confseq : *m_configSvc->sequenceList()) {
      // is this HLTSequence producing the given teType ?
      if (confseq->outputTE()->id() != teType) continue;
      std::vector<std::string> seqAlgs = confseq->algorithms();
      ATH_MSG_DEBUG("Calling createSequence");
   
      return createSequence(*confseq,teType);
   }

   std::string teName;
   TrigConf::HLTTriggerElement::getLabel(teType, teName);
   if(m_l1topoConfigSvc->menu()->getL1TopoConfigOutputList().hasTrigger(teName)) {
      if(m_doL1TopoSimulation==false)
         ATH_MSG_WARNING("Found TopoSeq, but TopoSimulation is disabled");

      ATH_MSG_DEBUG("Calling createTopoSequence");
      return createTopoSequence(teType);
   }

   return nullptr;
}



HLT::Sequence* TrigSteer::createSequence(const TrigConf::HLTSequence& seq, const unsigned int teType) {

   // get pointer to all Algorithms for this Sequence:
   const std::vector<std::string>& hltAlgoVec = seq.algorithms();
   std::vector<HLT::Algo*> algos;
 
   // create (if not done before) this algorithm
   for (unsigned int j = 0; j < hltAlgoVec.size(); ++j) {
      // if algo name is empty we should print a fatal message
      if(hltAlgoVec[j]=="") {
         std::string temp;
         TrigConf::HLTTriggerElement::getLabel(teType, temp);
         ATH_MSG_FATAL("Name of algorithm number " << j << " in sequence '" << temp << "' is empty -> abort.");
      }
      HLT::Algo* tmpAlgo = getAlgo(hltAlgoVec[j]);
      // do not store NULL pointers:
      if (tmpAlgo)
         algos.push_back(tmpAlgo);
   }
 
   // get input TE types and sequences
   std::vector<unsigned int> inputTypes;
   std::vector< Sequence* > inputSequences;
   for( TrigConf::HLTTriggerElement* inputTE : seq.inputTEs() ) {

      unsigned int inputId = inputTE->id();
      inputTypes.push_back( inputId );
      // make sure we have the sequences producing these inputTEs:
      if ( producedFirstAtLevel(inputId) == m_hltLevel ) {

         Sequence* newseq = findSeqForOutputTeType( inputId );
         // if seq not NULL and not in vector already, save it:
         if (newseq && find(inputSequences.begin(), inputSequences.end(), newseq) == inputSequences.end() ) {
            inputSequences.push_back(newseq);
            (*m_log) << MSG::DEBUG << "Sequences recursion: one producing " << inputId << " added for recursive calls of the one producing: " << teType << endreq;
         }
      }
   }


   // get the output TE type
   unsigned int outputType = seq.outputTE()->id();

   // get topo_start_from TE types
   const TrigConf::HLTTriggerElement* hltTopoTE = seq.topoStartTE();
   std::vector<unsigned int> topoStartTypes;
   Sequence * topoStartFromSequence = 0;
   // only add an entry if pointer not NULL
   if (hltTopoTE) {
      topoStartTypes.push_back( hltTopoTE->id() );
      topoStartFromSequence = findSeqForOutputTeType( hltTopoTE->id() );
   }


   if (inputTypes.size() == 0)
      ATH_MSG_DEBUG("creating a sequence without input types! Output is: " << outputType);

   if (algos.size() == 0) {
      std::string temp;
      TrigConf::HLTTriggerElement::getLabel(outputType, temp);
      ATH_MSG_ERROR("failed to create sequence: sequence has no algorithms. Output type: " << temp);
      return 0;
   }

   // create new HLT::Sequence
   
   HLT::Algo* firstAlgo = algos[0];
   std::vector<HLT::Algo*> tailAlgos;
   std::copy(++algos.begin(), algos.end(),
             std::back_insert_iterator< std::vector<HLT::Algo*> >(tailAlgos));

   HLT::Sequence* newseq = new HLT::Sequence(inputTypes, outputType,
                                             firstAlgo, tailAlgos, m_config, topoStartTypes);

   for (std::vector< Sequence* >::const_iterator it = inputSequences.begin();
        it != inputSequences.end(); ++it) {
      newseq->m_previousSequences.push_back( *it );
   }

   newseq->m_topoStartFromSequence = topoStartFromSequence;

   m_sequences.push_back(newseq);

   return newseq;

}


HLT::Sequence* TrigSteer::createTopoSequence(const unsigned int teType){

  HLT::Sequence* toposeq = new HLT::TopoSequence(m_config,teType,m_topoSteer);

  m_sequences.push_back(toposeq);

  return toposeq;

}


// Exectued once per offline job and for every new run online
StatusCode TrigSteer::start()
{  
  // OPI tools : bookHists()
  ToolHandleArray< IMonitorToolBase >::iterator itOPI    = m_opiTools.begin();
  ToolHandleArray< IMonitorToolBase >::iterator itOPIEnd = m_opiTools.end();
  for (  ; itOPI != itOPIEnd; ++itOPI ) {
    if ( (*itOPI)->bookHists().isFailure() ) {
      (*m_log) << MSG::ERROR << "tool " << (*itOPI).typeAndName() << " failed it's task - bookHists() - ... stop!" << endreq;
      return StatusCode::FAILURE;
    }
  }

  //monitoring tools : bookHists()
  ToolHandleArray< IMonitorToolBase >::iterator itMon    = m_monTools.begin();
  ToolHandleArray< IMonitorToolBase >::iterator itMonEnd = m_monTools.end();
  for (  ; itMon != itMonEnd; ++itMon ) {
    if ( (*itMon)->bookHists().isFailure() ) {
      (*m_log) << MSG::ERROR << "tool " << (*itMon).typeAndName() << " failed it's task - bookHists() - ... stop!" << endreq;
      return StatusCode::FAILURE;
    }
  }

  (*m_log) << MSG::DEBUG << "monitoring tools bookHists done " << m_monTools << " " << m_opiTools << endreq;

  return StatusCode::SUCCESS;
}


// Exectued at every run-number change both online and offline
StatusCode TrigSteer::endRun()
{
  // reset Navigation
  m_navigation->reset();
  (*m_log) << MSG::DEBUG << "Navigation reset for the last time" << endreq;

  if (!resetChains(m_chains) ) {
    (*m_log) << MSG::ERROR << "Failed to reset vector of all configured HLT::Chain objects!" << endreq;
  }
  
  return StatusCode::SUCCESS;
}

// Exectued once per offline job and for every new run online
StatusCode TrigSteer::stop()
{
   // OPI tools : finalHists()
   ToolHandleArray< IMonitorToolBase >::iterator itOPI    = m_opiTools.begin();
   ToolHandleArray< IMonitorToolBase >::iterator itOPIEnd = m_opiTools.end();
   for (  ; itOPI != itOPIEnd; ++itOPI ) {
      if ( (*itOPI)->finalHists().isFailure() ) {
         (*m_log) << MSG::ERROR << "tool " << (*itOPI).typeAndName() << " failed it's task - finalHists() - ... stop!" << endreq;
         return StatusCode::FAILURE;
      }
   }

   // monitoring tools : finalHists()
   ToolHandleArray< IMonitorToolBase >::iterator itMon    = m_monTools.begin();
   ToolHandleArray< IMonitorToolBase >::iterator itMonEnd = m_monTools.end();
   for (  ; itMon != itMonEnd; ++itMon ) {
      if ( (*itMon)->finalHists().isFailure() ) {
         (*m_log) << MSG::ERROR << "tool " << (*itMon).typeAndName() << " failed it's task - finalHists() - ... stop!" << endreq;
         return StatusCode::FAILURE;
      }
   }

   (*m_log) << MSG::DEBUG << "monitoring tools finalHists done " << m_monTools << " " << m_opiTools << endreq;

   return StatusCode::SUCCESS;
}




const std::vector<const HLT::SteeringChain*> TrigSteer::getActiveChains() const
{
  std::vector<const HLT::SteeringChain*> constVec;

  for (std::vector<HLT::SteeringChain*>::const_iterator it = m_activeChains.begin();
       it != m_activeChains.end(); ++it) {
    constVec.push_back(*it);
  }
  return constVec;
}


// we want to histogram the chains by name
namespace {
  int compareByName(const HLT::SteeringChain* ch1, const HLT::SteeringChain* ch2) {
    return ch1->getChainName() < ch2->getChainName();  
  }
}

const std::vector<const HLT::SteeringChain*> TrigSteer::getConfiguredChains() const {

   std::vector<const HLT::SteeringChain*> sortedChains;

   sortedChains.assign(m_chains.begin(),m_chains.end());

   sort(sortedChains.begin(), sortedChains.end(), compareByName);

   return sortedChains;
}


bool TrigSteer::canContinueEvent(HLT::ErrorCode ec) {
  ErrorCode mostSevere = std::max(  m_config->getHltStatus(), ec);
  m_config->setHltStatus(mostSevere);
  if ( mostSevere.action() > HLT::Action::ABORT_CHAIN ) {
    if (m_config->getMsgLvl() <= MSG::DEBUG)
      (*m_log) << MSG::DEBUG << "event processing aborted due to: "  << strErrorCode(mostSevere) << endreq;
    return false;
  }
  return true;
}

bool TrigSteer::canContinueJob() {
  if ( m_config->getHltStatus().action() > HLT::Action::ABORT_EVENT ) {
    (*m_log) << MSG::WARNING << "job processing aborted due to: "  << strErrorCode(m_config->getHltStatus()) << endreq;
    return false;
  }
  return true;
}


void TrigSteer::handle(const Incident& inc) {
   if ( inc.type()==HLT::Incidents::EventTimeout::type() ) {
      // The timeout singleton is reset by AthenaEvenLoopMgr for each event
      setTimeout(Athena::Timeout::instance());
   }
}


HLT::ErrorCode TrigSteer::setEvent() {

  const EventInfo* einfo(0);
  StatusCode sc =  m_config->getStoreGate()->retrieve(einfo);
  if(sc.isFailure()){
    (*m_log) << MSG::FATAL << "Can't get EventInfo object for update event information"  << endreq;
    return HLT::ErrorCode(Action::ABORT_EVENT, Reason::USERDEF_1, SteeringInternalReason::UNKNOWN);
  } else {
    if ( einfo->event_ID() ) {
      m_config->setLumiBlockNumber( einfo->event_ID()->lumi_block() );
      
      ITrigLBNHist::set_lbn( m_config->getLumiBlockNumber() ); // this is setting LBN for all trigger monitoring tools

      m_config->setLvl1Id( einfo->event_ID()->event_number() );

      // request the config service to set the correct prescales for the lumiblock
      StatusCode sc = m_configSvc->assignPrescalesToChains( einfo->event_ID()->lumi_block() );
      if (sc.isFailure()) {
        (*m_log) << MSG::FATAL << "ConfigSvc failed to assign HLT prescales to chains." << endreq;
        return HLT::ErrorCode(Action::ABORT_JOB, Reason::USERDEF_1, SteeringInternalReason::BAD_JOB_SETUP);        
      }

      if (m_enableCoherentPrescaling) 
	configureCoherentPrescaling();

      //load new keys to ResultBuilder (fore recording) & LvlConverter (for checking)
      m_resultBuilder->setConfigurationKeys(m_configSvc->masterKey(), m_configSvc->hltPrescaleKey());
      m_lvlCnvTool->setConfigurationKeys(m_configSvc->masterKey(), m_configSvc->hltPrescaleKey());
      
    } else {
      if (m_logLvl <= MSG::ERROR)(*m_log) << MSG::ERROR << "EventNumber&LBN not possible because missing event_ID" << endreq;
      return HLT::ErrorCode(Action::ABORT_EVENT, Reason::USERDEF_2, SteeringInternalReason::UNKNOWN);
    }
  }

  return HLT::OK;
}




/**
 * coherent prescaling
 *
 */

int compareLT(const HLT::SteeringChain* ch1, const HLT::SteeringChain* ch2) {
  return ch1->getChainCounter() < ch2->getChainCounter();  
}

int compareGT(const HLT::SteeringChain* ch1, const HLT::SteeringChain* ch2) {
  return ch1->getChainCounter() > ch2->getChainCounter();  
}

void HLT::TrigSteer::sortChains(std::vector<HLT::SteeringChain*>& chains) {
  if ( m_sortChains < 0 ) {
    sort(chains.begin(), chains.end(), compareLT);
  } else {
    sort(chains.begin(), chains.end(), compareGT);
  }
  // print order
  return;
}

struct comparePS {
  
  bool operator() (const HLT::SteeringChain* ch1, const HLT::SteeringChain* ch2) {

    // all what is below 1 means infinity (n omater if it is 0 or -1
    float ps1 = ch1->prescaleFactor() < 1. ? std::numeric_limits<float>::infinity() : ch1->prescaleFactor();
    float ps2 = ch2->prescaleFactor() < 1. ? std::numeric_limits<float>::infinity() : ch2->prescaleFactor();
    
    // if PS factors are equal the order does not matter in fact ... but we need to stably 
    // order them (otherwise sorting will fail)
    if ( ps1 == ps2 )
      return compareLT(ch1, ch2);
    
    return ps1 < ps2;
  }
};

void
TrigSteer::configureCoherentPrescaling() {
   // group chains by prescale group
   typedef std::map<std::string, std::set<HLT::SteeringChain*, comparePS> > PrescalingGroup;
   PrescalingGroup prescaling_group;
  
   for(HLT::SteeringChain* ch : m_chains ) {
      ch->setPrescaleMaster(0); // clear all previous prescaling ordering
      if ( ch->prescaleGroup() != "" ) {
         prescaling_group[ch->prescaleGroup()].insert(ch);
      }
   }
  

   // now order chains in each prescaling group by value of prescale
   for( PrescalingGroup::value_type g : prescaling_group) {
      HLT::SteeringChain* previous = 0;
      for(HLT::SteeringChain* ch : g.second ) {
         ATH_MSG_DEBUG("Prescaling group: " << ch->prescaleGroup()
                       << " chain: " << ch->getChainName() 
                       << " PS: " << ch->prescaleFactor() 
                       << " seeded of: " << ch->getLowerChainName() );
         ch->setPrescaleMaster(previous);
         previous = ch;
      }
   }
}

