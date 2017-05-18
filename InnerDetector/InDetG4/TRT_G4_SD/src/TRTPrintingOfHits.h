/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTPrintingOfHits_hh
#define TRT_G4_SD_TRTPrintingOfHits_hh

#include "AthenaKernel/MsgStreamMember.h"

class TRTUncompressedHit;
class TRTOutputFile;

class TRTPrintingOfHits
{
  public:
    TRTPrintingOfHits();
    ~TRTPrintingOfHits();

    void PrintUncompressedHit(TRTUncompressedHit*);

    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

  private:
    int m_printMessages;

    int m_mask;
    int m_shift;

    TRTOutputFile* m_pOutputFile;

    mutable Athena::MsgStreamMember m_msg;

};

#endif //TRT_G4_SD_TRTPrintingOfHits_hh
