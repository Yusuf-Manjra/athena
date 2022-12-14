/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCNV_P1_H
#define RECTPCNV_MISSINGETCNV_P1_H

/********************************************************************

NAME:     MissingEtCnv_p1.h
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for MissingET class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingET_p1.h"
#include "MissingETEvent/MissingET.h"

class MsgStream;

class MissingETCnv_p1 : public T_AthenaPoolTPCnvConstBase<MissingET, MissingET_p1>
{
    public:
        using base_class::transToPers;
        using base_class::persToTrans;


        MissingETCnv_p1() {};
        virtual void persToTrans( const MissingET_p1    *persObj,
                                  MissingET             *transObj,
                                  MsgStream             &msg ) const override;

        virtual void transToPers( const MissingET       *transObj,
                                  MissingET_p1          *persObj,
                                  MsgStream             &msg ) const override;
};

#endif // RECTPCNV_MISSINGETCNV_P1_H
