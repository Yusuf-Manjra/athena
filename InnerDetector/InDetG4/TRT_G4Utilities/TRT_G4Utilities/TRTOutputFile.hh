/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTOutputFile_hh
#define TRTOutputFile_hh

#include "globals.hh"
#include <fstream>
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class TRTOutputFile
{
public:
 
  static TRTOutputFile* GetPointer();
  
  std::ofstream& GetReference()
  {return m_output;}
 
  MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

private:
  TRTOutputFile();
  ~TRTOutputFile();
  
  std::ofstream m_output;
  Athena::MsgStreamMember m_msg;
};

#endif
