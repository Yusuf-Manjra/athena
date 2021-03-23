/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCRDOVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonSimData/MuonSimDataCollection.h"

#include "MuonRDO/CscRawDataContainer.h"

#include "MuonReadoutGeometry/CscReadoutElement.h"

#include "TTree.h"


using namespace Muon;

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode CSCRDOVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG("do fillCSCRDOVariables()");

  // clear variables
  CHECK( this->clearVariables() );

  const CscRawDataContainer* rdo_container = nullptr;
  CHECK( m_evtStore->retrieve(rdo_container, m_ContainerName.c_str()) );

  if(rdo_container->size()==0) ATH_MSG_WARNING(" CSC RDO Container empty ");

  for(const CscRawDataCollection* coll : *rdo_container) {
  
    for (auto rdo: *coll) {
   
      Identifier Id;
      Id = rdo->identify();

      std::string stName   = m_CscIdHelper->stationNameString(m_CscIdHelper->stationName(Id));
      int stationEta       = m_CscIdHelper->stationEta(Id);
      int stationPhi       = m_CscIdHelper->stationPhi(Id);
      int channel          = m_CscIdHelper->channel(Id);
      int chamberLayer     = m_CscIdHelper->chamberLayer(Id);
      int wireLayer        = m_CscIdHelper->wireLayer(Id);
      int strip            = m_CscIdHelper->strip(Id);
      bool measuresPhi       = m_CscIdHelper->measuresPhi(Id);


      ATH_MSG_DEBUG(     "MicroMegas RDO Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " channel ["      << channel         << "]"
                      << " chamberLayer [" << chamberLayer    << "]"
                      << " wireLayer ["    << wireLayer       << "]"
                      << " strip ["        << strip           << "]"
                      << " measuresPhi ["  << measuresPhi      << "]" );

      // to be stored in the ntuple
      m_Csc_rdo_stationName.push_back(stName);
      m_Csc_rdo_stationEta.push_back(stationEta);
      m_Csc_rdo_stationPhi.push_back(stationPhi);
      m_Csc_rdo_channel.push_back(channel);
      m_Csc_rdo_chamberLayer.push_back(chamberLayer);
      m_Csc_rdo_wireLayer.push_back(wireLayer);
      m_Csc_rdo_strip.push_back(strip);
      m_Csc_rdo_measuresPhi.push_back(measuresPhi);
      m_Csc_rdo_time.push_back(rdo->time());

      if (!MuonDetMgr->getCscReadoutElement(Id)) {
        ATH_MSG_ERROR("CSCRDOVariables::fillVariables() - Failed to retrieve CscReadoutElement for" << __FILE__ << __LINE__ << m_CscIdHelper->print_to_string(Id).c_str());
        return StatusCode::FAILURE;
      }

      // rdo counter for the ntuple
      m_Csc_nrdo++;
    }
  }

  ATH_MSG_DEBUG("processed " << m_Csc_nrdo << " MicroMegas rdo");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode CSCRDOVariables::clearVariables()
{
  m_Csc_nrdo = 0;

  m_Csc_rdo_stationName.clear();
  m_Csc_rdo_stationEta.clear();
  m_Csc_rdo_stationPhi.clear();
  m_Csc_rdo_channel.clear();
  m_Csc_rdo_chamberLayer.clear();
  m_Csc_rdo_wireLayer.clear();
  m_Csc_rdo_strip.clear();
  m_Csc_rdo_measuresPhi.clear();
  m_Csc_rdo_time.clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode CSCRDOVariables::initializeVariables()
{

  if(m_tree) {
    m_tree->Branch("RDO_CSC_n",             &m_Csc_nrdo);
    m_tree->Branch("RDO_CSC_stationName",   &m_Csc_rdo_stationName);
    m_tree->Branch("RDO_CSC_stationEta",    &m_Csc_rdo_stationEta);
    m_tree->Branch("RDO_CSC_stationPhi",    &m_Csc_rdo_stationPhi);
    m_tree->Branch("RDO_CSC_channel",       &m_Csc_rdo_channel);
    m_tree->Branch("RDO_CSC_chamberLayer",  &m_Csc_rdo_chamberLayer);
    m_tree->Branch("RDO_CSC_wireLayer",     &m_Csc_rdo_wireLayer);
    m_tree->Branch("RDO_CSC_strip",         &m_Csc_rdo_strip);
    m_tree->Branch("RDO_CSC_measuresPhi",   &m_Csc_rdo_measuresPhi);    
    m_tree->Branch("RDO_CSC_time",          &m_Csc_rdo_time);

  }
  return StatusCode::SUCCESS;
}

void CSCRDOVariables::deleteVariables()
{
  return;
}