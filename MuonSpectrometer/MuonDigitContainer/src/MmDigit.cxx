/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MmDigit.cxx

#include "MuonDigitContainer/MmDigit.h"

//**********************************************************************
// Local definitions
//**********************************************************************
namespace {

  // Range of time.
  //const int timemin = 0;
  //const int timemax = 4096;

}
//**********************************************************************
// Member functions.
//**********************************************************************

// Default constructor.

MmDigit::MmDigit()
  : MuonDigit(), m_stripResponseTime(0), m_stripResponseCharge(0), m_stripResponsePosition(0), m_chipResponseTime(0), m_chipResponseCharge(0), m_chipResponsePosition(0), m_stripForTrigger(0), m_stripTimeForTrigger(0){ }
 
//**********************************************************************/
// Full constructor from Identifier.
MmDigit::MmDigit(const Identifier& id)
  : MuonDigit(id),  m_stripResponseTime(0), m_stripResponseCharge(0), m_stripResponsePosition(0), m_chipResponseTime(0), m_chipResponseCharge(0), m_chipResponsePosition(0), m_stripForTrigger(0), m_stripTimeForTrigger(0){ }

//**********************************************************************/
  
// Full constructor, with trigger Info

MmDigit::MmDigit(const Identifier& id, std::vector<float> stripResponseTime, std::vector<int> stripResponsePosition, std::vector<float> stripResponseCharge, 
std::vector<float> chipResponseTime, std::vector<int> chipResponsePosition, std::vector<float> chipResponseCharge, int stripForTrigger, float stripTimeForTrigger)
  : MuonDigit(id), m_stripResponseTime(stripResponseTime), m_stripResponseCharge(stripResponseCharge), m_stripResponsePosition(stripResponsePosition), 
  m_chipResponseTime(chipResponseTime), m_chipResponseCharge(chipResponseCharge), m_chipResponsePosition(chipResponsePosition), 
  m_stripForTrigger(stripForTrigger), m_stripTimeForTrigger(stripTimeForTrigger) { }
//**********************************************************************/
 
// Validity check.
bool MmDigit::is_valid(const MmIdHelper * /*mmHelper*/) const {
  return true; /*( (m_time>=timemin && m_time<=timemax) && (mmHelper->valid(m_muonId)));*/
}

