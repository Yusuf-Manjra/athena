/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsData/PixelDCSStateData.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel DCS module state in PixelDCSStateData.
 */

#ifndef PIXELDCSSTATEDATA_H
#define PIXELDCSSTATEDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <unordered_map>

#include "AthenaKernel/CondCont.h"

class PixelDCSStateData {
  public:
    void setModuleStatus(const int chanNum, const int value);
    int getModuleStatus(const int chanNum) const;

    enum DCSModuleState{READY,ON,UNKNOWN,TRANSITION,UNDEFINED,NOSTATE};

  private:
    typedef std::unordered_map<int, int> IntConditions;
    IntConditions  m_moduleStatus;
};

CLASS_DEF( PixelDCSStateData , 345932855 , 1 )

CONDCONT_DEF( PixelDCSStateData, 578988355 );

#endif
