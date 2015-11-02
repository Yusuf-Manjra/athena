
 //***************************************************************************
 //                           CMXCPTobCnv_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     copyright            : (C) 2008 by Alan Watson
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
 //***************************************************************************
 //  *                                                                         *
 //  *   This program is free software; you can redistribute it and/or modify  *
  // *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
 //  *   (at your option) any later version.                                   *
 //  *                                                                         *
 //  ***************************************************************************/
  
#ifndef CMXCPTobCnv_p1_H
#define CMXCPTobCnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXCPTob_p1.h"

using namespace LVL1;

/** T/P convertor for TrigT1Calo CMXCPTob class */

class CMXCPTobCnv_p1 : public T_AthenaPoolTPCnvBase< CMXCPTob, CMXCPTob_p1 > {

public:
  CMXCPTobCnv_p1() : m_CMXCPTobCnv(0) {}

  virtual void persToTrans( const CMXCPTob_p1* persObj, CMXCPTob* transObj, MsgStream &log );
  virtual void transToPers( const CMXCPTob* transObj, CMXCPTob_p1* persObj, MsgStream &log );

protected:
  CMXCPTobCnv_p1 *m_CMXCPTobCnv ;

}; // class TriggerTowerCnv_p1

#endif
