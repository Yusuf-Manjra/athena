/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigRingerNeuralHypo.cxx
 * @author Danilo Enoque Ferreira de Lima 
 * $Author: dferreir $
 * $Date: 2009-05-07 21:01:28 $
 *
 * Implements the ringer-output neural processor
 */

#include "TrigMultiVarHypo/TrigRingerNeuralHypo.h"
#include "TrigCaloEvent/TrigRNNOutput.h"
#include "TrigCaloEvent/TrigRNNOutputContainer.h"


TrigRingerNeuralHypo::TrigRingerNeuralHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{  
  declareProperty("AcceptAll", m_acceptAll = false);
  declareProperty("Threshold", m_cut = 0.0);
  declareProperty("EmEtCut", m_emEtCut = 0.0);
  
  declareProperty("HltFeature", m_hlt_feature = "TrigRingerNeuralFex");  
  declareMonitoredVariable("NeuralNetworkOutput", m_decision);
}


HLT::ErrorCode TrigRingerNeuralHypo::hltInitialize() {

  if ( msg().level() <= MSG::INFO ) msg() << MSG::INFO << "on hltInitialize()" << endreq;

  if ( msg().level() <= MSG::DEBUG ) msg() << MSG::DEBUG << "TrigRingerNeuralHypo initialization completed successfully." << endreq;
  return HLT::OK;
}

HLT::ErrorCode TrigRingerNeuralHypo::hltFinalize() {  
  return HLT::OK;
}


HLT::ErrorCode TrigRingerNeuralHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){
  m_decision = 999.;

  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll)
  {
    pass = true;
    if ( msgLvl() <= MSG::DEBUG )
    {
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"  << endreq;
      return HLT::OK;
    }
  }
  else
  {
    pass = false;
    if ( msgLvl() <= MSG::DEBUG )
    {
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" << endreq;
    }
  }

  const TrigRNNOutput *rnnOutput = 0;
  HLT::ErrorCode hltStatus = getFeature(outputTE, rnnOutput, m_hlt_feature);
  if ( (hltStatus != HLT::OK) || (rnnOutput == 0)) {
    msg() << MSG::ERROR << "Couldn't get TrigRNNOutput from the Navigation. Accepting by default!" << endreq;
    pass = true;
    return HLT::OK;
  }

  if (rnnOutput->size() > 1) {
    msg() << MSG::WARNING << "Expected only one neuron at the neural network output. I'm only using the first output!" << endreq;
  }

  if (rnnOutput->size() == 0) {
    msg() << MSG::ERROR << "Neural Network output vector is empty. I can't proceed without an output!" << endreq;
    return HLT::ERROR;
  }

  m_decision = rnnOutput->at(0);

  // now the real decision
  if (m_decision > m_cut) {
    pass = true; //accept
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Accepting event with output = " << m_decision << endreq;
  }
  
  if (m_emEtCut > 0.0) {
    const TrigEMCluster *cluster = rnnOutput->cluster();
    if (cluster) {
      if (cluster->et() < m_emEtCut) {
        pass = false;
        if(msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Rejecting event with Et = " << cluster->et() << endreq;
      }
    }
  }
  
  if(msgLvl() <= MSG::DEBUG)
    if (!pass)
      msg() << MSG::DEBUG << "Rejecting event with output = " << m_decision << endreq;

  return HLT::OK;
}


