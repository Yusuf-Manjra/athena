/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCNV_P3_H
#define RECTPCNV_MISSINGETCNV_P3_H

/********************************************************************

NAME:     MissingEtCnv_p3.h
PACKAGE:  offline/Reconstruction/RecTPCnv
PURPOSE:  Transient/Persistent converter for MissingET class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingET_p3.h"
#include "MissingETEvent/MissingET.h"

class MsgStream;

class MissingETCnv_p3 : public T_AthenaPoolTPCnvConstBase<MissingET, MissingET_p3>
{
    public:
       using base_class::transToPers;
       using base_class::persToTrans;


        MissingETCnv_p3() {};
        virtual void persToTrans( const MissingET_p3    *persObj,
                                  MissingET             *transObj,
                                  MsgStream             &msg ) const override;

        virtual void transToPers( const MissingET       *transObj,
                                  MissingET_p3          *persObj,
                                  MsgStream             &msg ) const override;

	void persToTrans( MissingET* trans, std::vector<float>::const_iterator i) const;
	void transToPers( const MissingET* trans,  std::vector<float> &all) const;
};


#endif
