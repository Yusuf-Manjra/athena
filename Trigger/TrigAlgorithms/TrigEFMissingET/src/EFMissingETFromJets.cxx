/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromJets.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Doug Schaefer, Justin Chiu
CREATED:  May 20, 2014

PURPOSE:  Updates TrigMissingETHelper using info from jets

 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromJets.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CxxUtils/sincosf.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
//#include "FourMomUtils/P4DescendingSorters.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

#include <cmath>
#include <string>
using namespace std;

EFMissingETFromJets::EFMissingETFromJets(const std::string& type, 
    const std::string& name, 
    const IInterface* parent) :
  EFMissingETBaseTool(type, name, parent)
{
  // declareProperty("SaveUncalibrated", m_saveuncalibrated = false ,"save uncalibrated topo. clusters");
  // declare configurables
  
  _fextype = FexType::JET;
  
  m_methelperposition = 4;
}


EFMissingETFromJets::~EFMissingETFromJets()
{
}


StatusCode EFMissingETFromJets::initialize()
{
  if(msgLvl(MSG::DEBUG)) 
    msg(MSG::DEBUG) << "called EFMissingETFromJets::initialize()" << endreq;
  
  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) 
    msg(MSG::WARNING) << name() << ": Unable to locate TrigTimer Service" << endreq;
  
  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromJets::execute()
{

  msg(MSG::DEBUG) << name() << ": Executing Jet algorithm for ETMiss" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromJets::finalize()
{
  if(msgLvl(MSG::DEBUG)) 
    msg(MSG::DEBUG) << "called EFMissingETFromJets::finalize()" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromJets::execute(xAOD::TrigMissingET *, 
					TrigEFMissingEtHelper *metHelper, 
					const xAOD::CaloClusterContainer * /* caloCluster */,
					const xAOD::JetContainer *MHTJetContainer)
{
  if(msgLvl(MSG::DEBUG)) 
    msg(MSG::DEBUG) << "called EFMissingETFromJets::execute()" << endreq; // EFMissingET_Fex_Jets 

  if(m_timersvc)
    m_glob_timer->start(); // total time

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "started MET jet CPU timer" << endreq;
    
  TrigEFMissingEtComponent* metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Jet component

  if (metComp==0) {  msg(MSG::ERROR) << "cannot fetch Topo. cluster component!" << endreq;  return StatusCode::FAILURE; }  
  if(string(metComp->m_name).substr(0,3)!="JET"){ msg(MSG::ERROR) << "fetched " << metComp->m_name << " instead of the Jet component!" << endreq; return StatusCode::FAILURE; }

  std::vector<const xAOD::Jet*> MHTJetsVec(MHTJetContainer->begin(), MHTJetContainer->end());

  msg(MSG::DEBUG) << "num of jets: " << MHTJetsVec.size() << endreq; 
    
  for (const xAOD::Jet* aJet : MHTJetsVec) {
    metComp->m_ex -= aJet->px();
    metComp->m_ey -= aJet->py();
    metComp->m_ez -= aJet->pz();
    metComp->m_sumEt += aJet->pt();
    metComp->m_sumE  += aJet->e(); 
    metComp->m_usedChannels += 1;
    metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,aJet->pt())+0.5));              	
    // move from "processing" to "processed" state
    metComp->m_status ^= m_maskProcessing; // switch off bit
    metComp->m_status |= m_maskProcessed;  // switch on bit
   }  

  msg(MSG::DEBUG) << " calculated MET: " << sqrt((metComp->m_ex)*(metComp->m_ex)+(metComp->m_ey)*(metComp->m_ey)) << endreq;    


  if(m_timersvc)
    m_glob_timer->stop(); // total time

  return StatusCode::SUCCESS;
}

