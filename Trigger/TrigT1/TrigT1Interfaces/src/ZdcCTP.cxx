/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////// -*- C++ -*- /////////////////////////////
// ZdcCTP.cxx
// Implementation file for class ZdcCTP
// Author: Matthew Hoppesch <mch6@illinois.edu>
///////////////////////////////////////////////////////////////////

// TrigT1Interfaces includes
#include "TrigT1Interfaces/ZdcCTP.h"


LVL1::ZdcCTP::ZdcCTP( unsigned int word0 ) :
    m_cableWord0(word0)
{}

/** set the data that is sent on cable 0 */
void
LVL1::ZdcCTP::setCableWord0(uint32_t data) {
  m_cableWord0 = data;
}

/** return the data that is sent on cable 0 */
unsigned int
LVL1::ZdcCTP::cableWord0() const {
   return m_cableWord0;
}
