// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEammaHypo/TrigL2CaloLayersHypo.cxx
 **
 **   Description: Hypothesis algorithms to be run at Level 2 after T2Calo 
 **
 **   Authour: Denis Oliveira Damazio 
 **
 **   Created:      Sep 06 2010
 **
 **************************************************************************/ 

#include "TrigEgammaHypo/TrigL2CaloLayersHypo.h"

#include<cmath>
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"

class ISvcLocator;


TrigL2CaloLayersHypo::TrigL2CaloLayersHypo(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::HypoAlgo(name, pSvcLocator)
{
  // Read cuts - should probably get these from an xml file
  declareProperty("AcceptAll",      m_acceptAll=true);
  declareProperty("EnergyFractionCut",          m_EnergyFracCut);
  declareProperty("AbsoluteEnergyCut",         m_EnergyAbsCut);

  // declare monitoring histograms for all cut variables
  declareMonitoredVariable("Eta", monEta);
  declareMonitoredVariable("Phi", monPhi);
  declareMonitoredVariable("Energy",m_Energy);
  declareMonitoredVariable("PreSampler_Energy",m_preSamp);
  declareMonitoredVariable("PreSampler_fracEnergy",m_preSampFrac);
  declareMonitoredVariable("CutCounter", PassedCuts);
  m_EnergyAbsCut.clear();
  m_EnergyFracCut.clear();
  for(int i=0;i<4;i++){
  	m_EnergyAbsCut.push_back(999999.0);
	m_EnergyFracCut.push_back(1.0);
  }
}


TrigL2CaloLayersHypo::~TrigL2CaloLayersHypo()
{ }


HLT::ErrorCode TrigL2CaloLayersHypo::hltInitialize()
{
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initialization completed successfully"   << endreq;
    msg() << MSG::DEBUG << "AcceptAll           = "
	<< (m_acceptAll==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "EnergyFractionCut per layer = " 
	<< m_EnergyFracCut << endreq;  
    msg() << MSG::DEBUG << "AbsoluteEnergyCut per layer = " 
	<< m_EnergyAbsCut << endreq;  
  }

  if ( m_EnergyFracCut.size() != 4 ) {
    msg() << MSG::ERROR << " EnergyFracCut size is " <<  m_EnergyFracCut.size() << " but needs 4" << endreq;
    return StatusCode::FAILURE;
  }
  
  if ( m_EnergyAbsCut.size() != 4 ) {
    msg() << MSG::ERROR << " EnergyAbsCut size is " <<  m_EnergyAbsCut.size() << " but needs 4" << endreq;
    return StatusCode::FAILURE;
  }
  
  return HLT::OK;
}

HLT::ErrorCode TrigL2CaloLayersHypo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "In TrigL2CaloLayersHypo::finalize()" << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigL2CaloLayersHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    pass = true;
    if ( msgLvl() <= MSG::DEBUG ) 
	msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	    << endreq;
    return HLT::OK;
  } else {
    pass = false;
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	  << endreq;
    }
  }

  ///////////// get RoI descriptor ///////////////////////////////////////////////////////
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode stat = getFeature(outputTE, roiDescriptor) ;

  if (stat != HLT::OK) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }    
    return stat;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG 
	<< "Using outputTE("<< outputTE <<")->getId(): " << outputTE->getId()
	<< "; RoI ID = "   << roiDescriptor->roiId()
	<< ": Eta = "      << roiDescriptor->eta()
	<< ", Phi = "      << roiDescriptor->phi()
	<< endreq;
  }

  // fill local variables for RoI reference position
  
  // retrieve TrigEMCluster from the TE: must retrieve vector first
  std::vector< const xAOD::TrigEMCluster* > vectorOfClusters;  
  stat = getFeatures( outputTE, vectorOfClusters );

  if ( stat != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Failed to get TrigEMClusters" << endreq;
 
    return HLT::OK;
  }

  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Found vector with " << vectorOfClusters.size() 
	<< " TrigEMClusters" << endreq;
  }
  
  // should be only 1 cluster, normally!
  if (vectorOfClusters.size() != 1) {
    msg() << MSG::DEBUG << "Size of vector of TrigEMClusters is not 1!" 
	<< endreq;
    return HLT::OK;
  }

  // get cluster
  const xAOD::TrigEMCluster* pClus = vectorOfClusters.front();
  m_preSampFrac=m_preSamp=monEta=monPhi=m_Energy=-9999.0;

  if ( !pClus && (pClus->energy()>0.1) && (fabsf(pClus->eta())<2.1) ) {
    msg() << MSG::WARNING << "No EM cluster in RoI" << endreq;
    return HLT::OK;
  }
  monEta = pClus->eta();
  monPhi = pClus->phi();

  // increment event counter 
  PassedCuts++; //// the ROI at least contais the cluster

  std::vector<double> fracs;
  for(int i=0;i<4;i++){
	fracs.push_back(0);
	fracs[i] = (pClus->energy( ((CaloSampling::CaloSample)i) )+pClus->energy( ((CaloSampling::CaloSample)(i+4)) ))/pClus->energy();
  }
  m_Energy = pClus->energy();

  if ( fracs[0] > m_EnergyFracCut[0] ) return HLT::OK;
  PassedCuts++; //// 
  m_preSampFrac = fracs[0];

  if ( (pClus->energy( ((CaloSampling::CaloSample)0) ) + pClus->energy( ((CaloSampling::CaloSample)4) ) ) > m_EnergyAbsCut[0] ) return HLT::OK;
  PassedCuts++; //// 
  m_preSamp = (fracs[0])*pClus->energy();

  // got this far => passed!
  pass = true;

  // Reach this point successfully  
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "pass = " << pass << endreq;

  return HLT::OK;
}

