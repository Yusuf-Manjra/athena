﻿/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCDigitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"

#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "MuonDigitContainer/sTgcDigitCollection.h"

#include "TTree.h"

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode sTGCDigitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillNSWsTGCDigitVariables()");
  CHECK( this->clearVariables() );

  const sTgcDigitContainer* nsw_sTgcDigitContainer = nullptr;
  CHECK( m_evtStore->retrieve(nsw_sTgcDigitContainer, m_ContainerName.c_str()) );

  sTgcDigitContainer::const_iterator it   = nsw_sTgcDigitContainer->begin();
  sTgcDigitContainer::const_iterator it_e = nsw_sTgcDigitContainer->end();
  ATH_MSG_DEBUG("retrieved sTGC Digit Container with size "<<nsw_sTgcDigitContainer->digit_size());

  if(nsw_sTgcDigitContainer->size()==0) ATH_MSG_WARNING(" sTGC Digit Continer empty ");
  for(; it!=it_e; ++it) {
    const sTgcDigitCollection* coll = *it;
    ATH_MSG_DEBUG( "processing collection with size " << coll->size() );
    for (unsigned int digitNum=0; digitNum<coll->size(); digitNum++) {
      const sTgcDigit* digit = coll->at(digitNum);
      Identifier Id = digit->identify();

      std::string stName   = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(Id));
      int stationEta       = m_sTgcIdHelper->stationEta(Id);
      int stationPhi       = m_sTgcIdHelper->stationPhi(Id);
      int multiplet        = m_sTgcIdHelper->multilayer(Id);
      int gas_gap          = m_sTgcIdHelper->gasGap(Id);
      int channel          = m_sTgcIdHelper->channel(Id);
      int stationEtaMin    = m_sTgcIdHelper->stationEtaMin(Id);
      int stationEtaMax    = m_sTgcIdHelper->stationEtaMax(Id);
      int stationPhiMin    = m_sTgcIdHelper->stationPhiMin(Id);
      int stationPhiMax    = m_sTgcIdHelper->stationPhiMax(Id);
      int gas_gapMin       = m_sTgcIdHelper->gasGapMin(Id);
      int gas_gapMax       = m_sTgcIdHelper->gasGapMax(Id);
      int padEta           = m_sTgcIdHelper->padEta(Id);
      int padPhi           = m_sTgcIdHelper->padPhi(Id);
      int NofMultilayers   = m_sTgcIdHelper->numberOfMultilayers(Id);
      int multilayerMin    = m_sTgcIdHelper->multilayerMin(Id);
      int multilayerMax    = m_sTgcIdHelper->multilayerMax(Id);
      int channelTypeMin   = m_sTgcIdHelper->channelTypeMin(Id);
      int channelTypeMax   = m_sTgcIdHelper->channelTypeMax(Id);
      int channelMin       = m_sTgcIdHelper->channelMin(Id);
      int channelMax       = m_sTgcIdHelper->channelMax(Id);
      int channelType      = m_sTgcIdHelper->channelType(Id);

      ATH_MSG_DEBUG(     "sTGC Digit Offline id:  Station Name [" << stName << " ]"
                         << " Station Eta ["  << stationEta      << "]"
                         << " Station Phi ["  << stationPhi      << "]"
                         << " Multiplet  ["   << multiplet       << "]"
                         << " GasGap ["       << gas_gap         << "]"
                         << " ChNr ["         << channel         << "]"
                         << " Station EtaMin ["  << stationEtaMin      << "]"
                         << " Station EtaMax ["  << stationEtaMax      << "]"
                         << " Station PhiMin ["  << stationPhiMin      << "]"
                         << " Station PhiMax ["  << stationPhiMax      << "]");

      int isSmall = stName[2] == 'S';
      const MuonGM::sTgcReadoutElement* rdoEl = MuonDetMgr->getsTgcRElement_fromIdFields(isSmall, stationEta, stationPhi, multiplet );

      int channelNumber = 0;
      const Identifier phiId, etaId;
      Amg::Vector3D gpos(0.,0.,0.);
      Amg::Vector2D lpos(0.,0.);

      rdoEl->stripPosition(Id,lpos);
      rdoEl->surface(Id).localToGlobal(lpos, gpos,gpos);

      std::vector<Amg::Vector2D> local_pad_corners;
      rdoEl->padCorners(Id,local_pad_corners);
      std::vector<Amg::Vector3D> global_pad_corners;

      for(auto& local_corner : local_pad_corners) {
          Amg::Vector3D global_corner;
          rdoEl->surface(Id).localToGlobal(local_corner, global_corner, global_corner);
          global_pad_corners.push_back(global_corner);
      }


      m_NSWsTGC_dig_globalPosX->push_back( gpos.x() );
      m_NSWsTGC_dig_globalPosY->push_back( gpos.y() );
      m_NSWsTGC_dig_globalPosZ->push_back( gpos.z() );
      m_NSWsTGC_dig_localPosX->push_back( lpos.x() );
      m_NSWsTGC_dig_localPosY->push_back( lpos.y() );
      for(auto corner : global_pad_corners) {
          if(channelType ==0 ) {
              m_NSWsTGC_dig_PadglobalCornerPosX->push_back(corner.x());
              m_NSWsTGC_dig_PadglobalCornerPosY->push_back(corner.y());
              m_NSWsTGC_dig_PadglobalCornerPosZ->push_back(corner.z());
          }
      }
      channelNumber  = rdoEl->stripNumber(lpos,Id);
      m_NSWsTGC_dig_channelPosX->push_back( lpos.x() );
      m_NSWsTGC_dig_channelPosY->push_back( lpos.y() );

      m_NSWsTGC_dig_channel_type->push_back(channelType);
      m_NSWsTGC_dig_stationName->push_back(stName);
      m_NSWsTGC_dig_stationEta->push_back(stationEta);
      m_NSWsTGC_dig_stationPhi->push_back(stationPhi);
      m_NSWsTGC_dig_multiplet->push_back(multiplet);
      m_NSWsTGC_dig_gas_gap->push_back(gas_gap);
      m_NSWsTGC_dig_channel->push_back(channel);
      m_NSWsTGC_dig_stationEtaMin->push_back(stationEtaMin);
      m_NSWsTGC_dig_stationEtaMax->push_back(stationEtaMax);
      m_NSWsTGC_dig_stationPhiMin->push_back(stationPhiMin);
      m_NSWsTGC_dig_stationPhiMax->push_back(stationPhiMax);
      m_NSWsTGC_dig_gas_gapMin->push_back(gas_gapMin);
      m_NSWsTGC_dig_gas_gapMax->push_back(gas_gapMax);
      m_NSWsTGC_dig_padEta->push_back(padEta);
      m_NSWsTGC_dig_padPhi->push_back(padPhi);
      m_NSWsTGC_dig_numberOfMultilayers->push_back(NofMultilayers);
      m_NSWsTGC_dig_multilayerMin->push_back(multilayerMin);
      m_NSWsTGC_dig_multilayerMax->push_back(multilayerMax);
      m_NSWsTGC_dig_channelTypeMin->push_back(channelTypeMin);
      m_NSWsTGC_dig_channelTypeMax->push_back(channelTypeMax);
      m_NSWsTGC_dig_channelMin->push_back(channelMin);
      m_NSWsTGC_dig_channelMax->push_back(channelMax);
      m_NSWsTGC_dig_channelNumber->push_back(channelNumber);

      m_NSWsTGC_dig_bctag->push_back(digit->bcTag());
      m_NSWsTGC_dig_time->push_back(digit->time());
      m_NSWsTGC_dig_charge->push_back(digit->charge());
      m_NSWsTGC_dig_isDead->push_back(digit->isDead());
      m_NSWsTGC_dig_isPileup->push_back(digit->isPileup());

      if(channelType == 0) m_NSWsTGC_nPadDigits++;
      m_NSWsTGC_nDigits++;
    }
  }
  ATH_MSG_INFO(" finished fillNSWsTGCDigitVariables()");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode sTGCDigitVariables::clearVariables()
{

  m_NSWsTGC_nDigits = 0;
  m_NSWsTGC_nPadDigits = 0;
  m_NSWsTGC_dig_time->clear();
  m_NSWsTGC_dig_bctag->clear();
  m_NSWsTGC_dig_charge->clear();
  m_NSWsTGC_dig_isDead->clear();
  m_NSWsTGC_dig_isPileup->clear();
  m_NSWsTGC_dig_stationName->clear();
  m_NSWsTGC_dig_stationEta->clear();
  m_NSWsTGC_dig_stationPhi->clear();
  m_NSWsTGC_dig_multiplet->clear();
  m_NSWsTGC_dig_gas_gap->clear();
  m_NSWsTGC_dig_channel_type->clear();
  m_NSWsTGC_dig_channel->clear();
  m_NSWsTGC_dig_stationEtaMin->clear();
  m_NSWsTGC_dig_stationEtaMax->clear();
  m_NSWsTGC_dig_stationPhiMin->clear();
  m_NSWsTGC_dig_stationPhiMax->clear();
  m_NSWsTGC_dig_gas_gapMin->clear();
  m_NSWsTGC_dig_gas_gapMax->clear();
  m_NSWsTGC_dig_padEta->clear();
  m_NSWsTGC_dig_padPhi->clear();
  m_NSWsTGC_dig_numberOfMultilayers->clear();
  m_NSWsTGC_dig_multilayerMin->clear();
  m_NSWsTGC_dig_multilayerMax->clear();
  m_NSWsTGC_dig_channelTypeMin->clear();
  m_NSWsTGC_dig_channelTypeMax->clear();
  m_NSWsTGC_dig_channelMin->clear();
  m_NSWsTGC_dig_channelMax->clear();
  m_NSWsTGC_dig_channelNumber->clear();
  m_NSWsTGC_dig_channelPosX->clear();
  m_NSWsTGC_dig_channelPosY->clear();
  m_NSWsTGC_dig_localPosX->clear();
  m_NSWsTGC_dig_localPosY->clear();
  m_NSWsTGC_dig_globalPosX->clear();
  m_NSWsTGC_dig_globalPosY->clear();
  m_NSWsTGC_dig_globalPosZ->clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode sTGCDigitVariables::initializeVariables()
{

  m_NSWsTGC_nDigits           = 0;
  m_NSWsTGC_nPadDigits        = 0;
  m_NSWsTGC_dig_time          = new std::vector<double>();
  m_NSWsTGC_dig_bctag         = new std::vector<int>();
  m_NSWsTGC_dig_charge        = new std::vector<double>();
  m_NSWsTGC_dig_isDead        = new std::vector<bool>();
  m_NSWsTGC_dig_isPileup      = new std::vector<bool>();
  
  m_NSWsTGC_dig_stationName   = new std::vector<std::string>();
  m_NSWsTGC_dig_stationEta    = new std::vector<int>();
  m_NSWsTGC_dig_stationPhi    = new std::vector<int>();
  m_NSWsTGC_dig_multiplet     = new std::vector<int>();
  m_NSWsTGC_dig_gas_gap       = new std::vector<int>();
  m_NSWsTGC_dig_channel_type  = new std::vector<int>();
  m_NSWsTGC_dig_channel       = new std::vector<int>();

  m_NSWsTGC_dig_stationEtaMin = new std::vector<int>();
  m_NSWsTGC_dig_stationEtaMax = new std::vector<int>();
  m_NSWsTGC_dig_stationPhiMin = new std::vector<int>();
  m_NSWsTGC_dig_stationPhiMax = new std::vector<int>();
  m_NSWsTGC_dig_gas_gapMin    = new std::vector<int>();
  m_NSWsTGC_dig_gas_gapMax    = new std::vector<int>();
  m_NSWsTGC_dig_padEta        = new std::vector<int>();
  m_NSWsTGC_dig_padPhi        = new std::vector<int>();

  m_NSWsTGC_dig_numberOfMultilayers = new std::vector<int>();
  m_NSWsTGC_dig_multilayerMin       = new std::vector<int>();
  m_NSWsTGC_dig_multilayerMax       = new std::vector<int>();
  m_NSWsTGC_dig_channelTypeMin      = new std::vector<int>();
  m_NSWsTGC_dig_channelTypeMax      = new std::vector<int>();
  m_NSWsTGC_dig_channelMin          = new std::vector<int>();
  m_NSWsTGC_dig_channelMax          = new std::vector<int>();
  m_NSWsTGC_dig_channelNumber       = new std::vector<int>();

  m_NSWsTGC_dig_channelPosX = new std::vector<double>();
  m_NSWsTGC_dig_channelPosY = new std::vector<double>();
  m_NSWsTGC_dig_localPosX = new std::vector<double>();
  m_NSWsTGC_dig_localPosY = new std::vector<double>();
  m_NSWsTGC_dig_globalPosX = new std::vector<double>();
  m_NSWsTGC_dig_globalPosY = new std::vector<double>();
  m_NSWsTGC_dig_globalPosZ = new std::vector<double>();

  if(m_tree) {
    ATH_MSG_DEBUG("sTGC digit:  init m_tree ");
    m_tree->Branch("Digits_sTGC",              &m_NSWsTGC_nDigits, "Digits_sTGC_n/i");
    m_tree->Branch("Digits_sTGC_Pad_Digits",             &m_NSWsTGC_nPadDigits, "Digits_sTGC_Pad_Digits_n/i");
    m_tree->Branch("Digits_sTGC_time",       "std::vector< double >", &m_NSWsTGC_dig_time);
    m_tree->Branch("Digits_sTGC_bctag",       "std::vector< int >", &m_NSWsTGC_dig_bctag);
    m_tree->Branch("Digits_sTGC_charge",     "std::vector< double >", &m_NSWsTGC_dig_charge);
    m_tree->Branch("Digits_sTGC_isDead",     "std::vector< bool >",   &m_NSWsTGC_dig_isDead);
    m_tree->Branch("Digits_sTGC_isPileup",   "std::vector< bool >",  &m_NSWsTGC_dig_isPileup);
    m_tree->Branch("Digits_sTGC_stationName", &m_NSWsTGC_dig_stationName);
    m_tree->Branch("Digits_sTGC_stationEta",  &m_NSWsTGC_dig_stationEta);
    m_tree->Branch("Digits_sTGC_stationPhi",  &m_NSWsTGC_dig_stationPhi);
    m_tree->Branch("Digits_sTGC_multiplet",   &m_NSWsTGC_dig_multiplet);
    m_tree->Branch("Digits_sTGC_gas_gap",     &m_NSWsTGC_dig_gas_gap);
    m_tree->Branch("Digits_sTGC_channel_type",&m_NSWsTGC_dig_channel_type);
    m_tree->Branch("Digits_sTGC_channel",     &m_NSWsTGC_dig_channel);
    m_tree->Branch("Digits_sTGC_stationEtaMin",  &m_NSWsTGC_dig_stationEtaMin);
    m_tree->Branch("Digits_sTGC_stationEtaMax",  &m_NSWsTGC_dig_stationEtaMax);
    m_tree->Branch("Digits_sTGC_stationPhiMin",  &m_NSWsTGC_dig_stationPhiMin);
    m_tree->Branch("Digits_sTGC_stationPhiMax",  &m_NSWsTGC_dig_stationPhiMax);
    m_tree->Branch("Digits_sTGC_gas_gapMin",  &m_NSWsTGC_dig_gas_gapMin);
    m_tree->Branch("Digits_sTGC_gas_gapMax",  &m_NSWsTGC_dig_gas_gapMax);
    m_tree->Branch("Digits_sTGC_padEta",  &m_NSWsTGC_dig_padEta);
    m_tree->Branch("Digits_sTGC_padPhi",  &m_NSWsTGC_dig_padPhi);
    m_tree->Branch("Digits_sTGC_numberOfMultilayers",  &m_NSWsTGC_dig_numberOfMultilayers);
    m_tree->Branch("Digits_sTGC_multilayerMin",  &m_NSWsTGC_dig_multilayerMin);
    m_tree->Branch("Digits_sTGC_multilayerMax",  &m_NSWsTGC_dig_multilayerMax);
    m_tree->Branch("Digits_sTGC_channelTypeMin",  &m_NSWsTGC_dig_channelTypeMin);
    m_tree->Branch("Digits_sTGC_channelTypeMax",  &m_NSWsTGC_dig_channelTypeMax);
    m_tree->Branch("Digits_sTGC_channelMin",  &m_NSWsTGC_dig_channelMin);
    m_tree->Branch("Digits_sTGC_channelMax",  &m_NSWsTGC_dig_channelMax);
    m_tree->Branch("Digits_sTGC_channelNumber",  &m_NSWsTGC_dig_channelNumber);

    m_tree->Branch("Digits_sTGC_channelPosX", "std::vector< double >", &m_NSWsTGC_dig_channelPosX);
    m_tree->Branch("Digits_sTGC_channelPosY", "std::vector< double >", &m_NSWsTGC_dig_channelPosY);
    m_tree->Branch("Digits_sTGC_localPosX", "std::vector< double >", &m_NSWsTGC_dig_localPosX);
    m_tree->Branch("Digits_sTGC_localPosY", "std::vector< double >", &m_NSWsTGC_dig_localPosY);
    m_tree->Branch("Digits_sTGC_globalPosX", "std::vector< double >", &m_NSWsTGC_dig_globalPosX);
    m_tree->Branch("Digits_sTGC_globalPosY", "std::vector< double >", &m_NSWsTGC_dig_globalPosY);
    m_tree->Branch("Digits_sTGC_globalPosZ", "std::vector< double >", &m_NSWsTGC_dig_globalPosZ);
    m_tree->Branch("Digits_sTGC_PadglobalCornerPosX", "std::vector< double >", &m_NSWsTGC_dig_PadglobalCornerPosX);
    m_tree->Branch("Digits_sTGC_PadglobalCornerPosY", "std::vector< double >", &m_NSWsTGC_dig_PadglobalCornerPosY);
    m_tree->Branch("Digits_sTGC_PadglobalCornerPosZ", "std::vector< double >", &m_NSWsTGC_dig_PadglobalCornerPosZ);
  }

  return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called on finalize level of main alg */
void sTGCDigitVariables::deleteVariables()
{

  delete m_NSWsTGC_dig_stationName;
  delete m_NSWsTGC_dig_stationEta;
  delete m_NSWsTGC_dig_stationPhi;
  delete m_NSWsTGC_dig_multiplet;
  delete m_NSWsTGC_dig_gas_gap;
  delete m_NSWsTGC_dig_channel_type;
  delete m_NSWsTGC_dig_channel;
  delete m_NSWsTGC_dig_stationEtaMin;
  delete m_NSWsTGC_dig_stationEtaMax;
  delete m_NSWsTGC_dig_stationPhiMin;
  delete m_NSWsTGC_dig_stationPhiMax;
  delete m_NSWsTGC_dig_gas_gapMin;
  delete m_NSWsTGC_dig_gas_gapMax;
  delete m_NSWsTGC_dig_padEta;
  delete m_NSWsTGC_dig_padPhi;
  delete m_NSWsTGC_dig_numberOfMultilayers;
  delete m_NSWsTGC_dig_multilayerMin;
  delete m_NSWsTGC_dig_multilayerMax;
  delete m_NSWsTGC_dig_channelTypeMin;
  delete m_NSWsTGC_dig_channelTypeMax;
  delete m_NSWsTGC_dig_channelMin;
  delete m_NSWsTGC_dig_channelMax;
  delete m_NSWsTGC_dig_channelNumber;
  delete m_NSWsTGC_dig_channelPosX;
  delete m_NSWsTGC_dig_channelPosY;
  delete m_NSWsTGC_dig_localPosX;
  delete m_NSWsTGC_dig_localPosY;
  delete m_NSWsTGC_dig_globalPosX;
  delete m_NSWsTGC_dig_globalPosY;
  delete m_NSWsTGC_dig_globalPosZ;

  delete m_NSWsTGC_dig_time;
  delete m_NSWsTGC_dig_bctag;
  delete m_NSWsTGC_dig_charge;
  delete m_NSWsTGC_dig_isDead;
  delete m_NSWsTGC_dig_isPileup;


  m_NSWsTGC_nDigits      = 0;
  m_NSWsTGC_nPadDigits      = 0;
  m_NSWsTGC_dig_time     = nullptr;
  m_NSWsTGC_dig_bctag    = nullptr;
  m_NSWsTGC_dig_charge   = nullptr;
  m_NSWsTGC_dig_isDead   = nullptr;
  m_NSWsTGC_dig_isPileup = nullptr;
  m_NSWsTGC_dig_stationName   = nullptr;
  m_NSWsTGC_dig_stationEta    = nullptr;
  m_NSWsTGC_dig_stationPhi    = nullptr;
  m_NSWsTGC_dig_multiplet     = nullptr;
  m_NSWsTGC_dig_gas_gap       = nullptr;
  m_NSWsTGC_dig_channel_type  = nullptr;
  m_NSWsTGC_dig_channel       = nullptr;
  m_NSWsTGC_dig_stationEtaMin = nullptr;
  m_NSWsTGC_dig_stationEtaMax = nullptr;
  m_NSWsTGC_dig_stationPhiMin = nullptr;
  m_NSWsTGC_dig_stationPhiMax = nullptr;
  m_NSWsTGC_dig_gas_gapMin    = nullptr;
  m_NSWsTGC_dig_gas_gapMax    = nullptr;
  m_NSWsTGC_dig_padEta        = nullptr;
  m_NSWsTGC_dig_padPhi        = nullptr;
  m_NSWsTGC_dig_numberOfMultilayers = nullptr;
  m_NSWsTGC_dig_multilayerMin       = nullptr;
  m_NSWsTGC_dig_multilayerMax       = nullptr;
  m_NSWsTGC_dig_channelTypeMin      = nullptr;
  m_NSWsTGC_dig_channelTypeMax      = nullptr;
  m_NSWsTGC_dig_channelMin          = nullptr;
  m_NSWsTGC_dig_channelMax          = nullptr;
  m_NSWsTGC_dig_channelNumber       = nullptr;
  m_NSWsTGC_dig_channelPosX = nullptr;
  m_NSWsTGC_dig_channelPosY = nullptr;
  m_NSWsTGC_dig_localPosX   = nullptr;
  m_NSWsTGC_dig_localPosY   = nullptr;
  m_NSWsTGC_dig_globalPosX  = nullptr;
  m_NSWsTGC_dig_globalPosY  = nullptr;
  m_NSWsTGC_dig_globalPosZ  = nullptr;

  return;
}
