/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWDATACONTAINERREADOUT_H
#define AFP_RAWDATACONTAINERREADOUT_H


#include "AFP_ReadOut.h"


class AFP_RawDataContainerReadOut : public AFP_ReadOut {

private:
  uint32_t m_wordMarker;
  uint16_t m_subdetId;
  uint16_t m_mrodId;
  uint32_t m_lvl1Id;
  uint16_t m_ecrId;
  uint16_t m_bcId;
  uint32_t m_runNum;
  uint16_t m_runType;
  uint16_t m_triggerTypeId;
  uint32_t m_DetEventType;
  uint32_t m_timestamp;
  uint32_t m_BCId;
  static const uint32_t FULLEVMARKER = 0xaa1234aa;
  static const uint32_t ROBMARKER = 0xdd1234dd;
  static const uint32_t RODMARKER = 0xee1234ee;
  static const uint32_t RODHEADERSIZE = 0x00000009;
  static const uint32_t RODVERSION = 0x03010000;

public:
  AFP_RawDataContainerReadOut();
  ~AFP_RawDataContainerReadOut();
  bool is_FullEVmarker() { return m_wordMarker == FULLEVMARKER; };
  bool is_ROBmarker() { return m_wordMarker == ROBMARKER; };
  bool is_RODmarker() { return m_wordMarker == RODMARKER; };




  uint16_t subdetId() { return m_subdetId; }
  uint16_t mrodId() { return m_mrodId; }
  uint32_t runNum() { return m_runNum; }
  uint16_t runType() { return m_runType; }
  uint32_t lvl1Id() { return m_lvl1Id; }
  uint16_t ecrId() { return m_ecrId; }
  uint16_t bcId() { return m_bcId; }
  uint16_t trigtypeId() { return m_triggerTypeId; }
  uint32_t DetEventType() { return m_DetEventType; }
  uint32_t TimeStamp() { return m_timestamp; }
  uint32_t BCId() { return m_BCId; }

private:
  void setZero();
  void setZeroFull();
};

#endif
