/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTDigitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/MdtDigit.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MuonDigitContainer/MdtDigitCollection.h"

#include "TTree.h"
/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode MdtDigitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillMdtDigitVariables()");
  CHECK( this->clearVariables() );

  const MdtDigitContainer* MdtDigitContainer = nullptr;
  CHECK( m_evtStore->retrieve(MdtDigitContainer, m_ContainerName.c_str()) );

  ATH_MSG_DEBUG("retrieved MDT Digit Container with size "<<MdtDigitContainer->digit_size());

  if(MdtDigitContainer->size()==0) ATH_MSG_DEBUG(" MDT Digit Continer empty ");
  for(const MdtDigitCollection* coll : *MdtDigitContainer) {
    ATH_MSG_DEBUG( "processing collection with size " << coll->size() );
    for (unsigned int digitNum=0; digitNum<coll->size(); digitNum++) {
      const MdtDigit* digit = coll->at(digitNum);
      Identifier Id = digit->identify();

      std::string stName   = m_MdtIdHelper->stationNameString(m_MdtIdHelper->stationName(Id));
      int stationEta       = m_MdtIdHelper->stationEta(Id);
      int stationPhi       = m_MdtIdHelper->stationPhi(Id);
      int tube          = m_MdtIdHelper->tube(Id);
      int tubeLayer          = m_MdtIdHelper->tubeLayer(Id);
      int multilayer       = m_MdtIdHelper->multilayer(Id);
      int channel          = m_MdtIdHelper->channel(Id);
      int NofMultilayers   = m_MdtIdHelper->numberOfMultilayers(Id);

      ATH_MSG_DEBUG(     "MDT Digit Offline id:  Station Name [" << stName << " ]"
                         << " Station Eta ["  << stationEta      << "]"
                         << " Station Phi ["  << stationPhi      << "]"
                         << " Multilayer  ["   << multilayer       << "]"
                         << " ChNr ["         << channel         << "]");

      const MuonGM::MdtReadoutElement* rdoEl = MuonDetMgr->getMdtReadoutElement(Id);
      if (!rdoEl) return StatusCode::FAILURE;

      Amg::Vector3D gpos(0.,0.,0.);
      Amg::Vector2D lpos(0.,0.);

      rdoEl->surface(Id).localToGlobal(lpos, gpos,gpos);
      
      m_MDT_dig_localPosX.push_back( lpos.x() );
      m_MDT_dig_localPosY.push_back( lpos.y() );
      m_MDT_dig_globalPosX.push_back( gpos.x() );
      m_MDT_dig_globalPosY.push_back( gpos.y() );
      m_MDT_dig_globalPosZ.push_back( gpos.z() );
      
      m_MDT_dig_time.push_back(digit->tdc());
      m_MDT_dig_charge.push_back(digit->adc());
      m_MDT_dig_stationName.push_back(stName);
      m_MDT_dig_stationEta.push_back(stationEta);
      m_MDT_dig_stationPhi.push_back(stationPhi);
      m_MDT_dig_tube.push_back(tube);
      m_MDT_dig_tubeLayer.push_back(tubeLayer);
      m_MDT_dig_multilayer.push_back(multilayer);
      m_MDT_dig_channel.push_back(channel);
      m_MDT_dig_numberOfMultilayers.push_back(NofMultilayers);
      
      m_MDT_nDigits++;
    }
  }
  ATH_MSG_DEBUG(" finished fillMdtDigitVariables()");
  return StatusCode::SUCCESS;
}

/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode MdtDigitVariables::clearVariables()
{
  m_MDT_nDigits = 0;
  m_MDT_dig_time.clear();
  m_MDT_dig_charge.clear();
  m_MDT_dig_stationName.clear();
  m_MDT_dig_stationEta.clear();
  m_MDT_dig_stationPhi.clear();
  m_MDT_dig_tube.clear();
  m_MDT_dig_tubeLayer.clear();
  m_MDT_dig_multilayer.clear();
  m_MDT_dig_channel.clear();
  m_MDT_dig_numberOfMultilayers.clear();
  m_MDT_dig_localPosX.clear();
  m_MDT_dig_localPosY.clear();
  m_MDT_dig_globalPosX.clear();
  m_MDT_dig_globalPosY.clear();
  m_MDT_dig_globalPosZ.clear();

  return StatusCode::SUCCESS;
}

/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode MdtDigitVariables::initializeVariables()
{

  if(m_tree) {
    ATH_MSG_DEBUG("MDT digit:  init m_tree ");
    m_tree->Branch("Digits_MDT",              &m_MDT_nDigits, "Digits_MDT_n/i");
    m_tree->Branch("Digits_MDT_time", &m_MDT_dig_time); 
    m_tree->Branch("Digits_MDT_charge", &m_MDT_dig_charge); 
    m_tree->Branch("Digits_MDT_stationName", &m_MDT_dig_stationName);
    m_tree->Branch("Digits_MDT_stationEta",  &m_MDT_dig_stationEta);
    m_tree->Branch("Digits_MDT_stationPhi",  &m_MDT_dig_stationPhi);
    m_tree->Branch("Digits_MDT_numberOfMultilayers",  &m_MDT_dig_numberOfMultilayers);
    m_tree->Branch("Digits_MDT_tube",  &m_MDT_dig_tube);
    m_tree->Branch("Digits_MDT_tubeLayer",  &m_MDT_dig_tubeLayer);
    m_tree->Branch("Digits_MDT_multilayer",  &m_MDT_dig_multilayer);
    m_tree->Branch("Digits_MDT_localPosX", &m_MDT_dig_localPosX);
    m_tree->Branch("Digits_MDT_localPosY", &m_MDT_dig_localPosY);
    m_tree->Branch("Digits_MDT_globalPosX", &m_MDT_dig_globalPosX);
    m_tree->Branch("Digits_MDT_globalPosY",  &m_MDT_dig_globalPosY);
    m_tree->Branch("Digits_MDT_globalPosZ",  &m_MDT_dig_globalPosZ);
  }

  return StatusCode::SUCCESS;
}
