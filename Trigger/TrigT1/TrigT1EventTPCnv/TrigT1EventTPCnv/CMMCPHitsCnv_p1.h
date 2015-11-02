
 //***************************************************************************
 //                           CMMCPHitsCnv_p1.h  -  description
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
  
#ifndef CMMCPHitsCnv_p1_H
#define CMMCPHitsCnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMMCPHits_p1.h"

using namespace LVL1;

/** T/P convertor for TrigT1Calo CMMCPHits class */

class CMMCPHitsCnv_p1 : public T_AthenaPoolTPCnvBase< CMMCPHits, CMMCPHits_p1 > {

public:
  CMMCPHitsCnv_p1() : m_CMMCPHitsCnv(0) {}

  virtual void persToTrans( const CMMCPHits_p1* persObj, CMMCPHits* transObj, MsgStream &log );
  virtual void transToPers( const CMMCPHits* transObj, CMMCPHits_p1* persObj, MsgStream &log );

protected:
  CMMCPHitsCnv_p1 *m_CMMCPHitsCnv ;

}; // class TriggerTowerCnv_p1

#endif
