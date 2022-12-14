/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelDCSStatusData.h"

void PixelDCSStatusData::setModuleStatus(const int chanNum, const int value) {
  m_moduleStatus[chanNum] = value;
}

int PixelDCSStatusData::getModuleStatus(const int chanNum) const {
  auto itr = m_moduleStatus.find(chanNum);
  if (itr!=m_moduleStatus.end()) { return itr->second; }
  return 0;
}

