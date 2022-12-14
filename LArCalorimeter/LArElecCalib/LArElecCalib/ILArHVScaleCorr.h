/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARHVSCALRCORR_H
#define LARCONDITIONS_ILARHVSCALRCORR_H

#include "AthenaKernel/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"

class ILArHVScaleCorr {
/** This class defines the interface for accessing 
    semi-stable HV scale correction factors 
   * @stereotype Interface
   * @author W. Lampl
   */
  
 public: 

  virtual ~ILArHVScaleCorr() {};

  // online ID
  virtual const float& HVScaleCorr(const HWIdentifier& id )  const = 0 ;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILArHVScaleCorr,93397263,1) 

#include "AthenaKernel/CondCont.h"
CONDCONT_MIXED_DEF( ILArHVScaleCorr , 182821539 );

#endif 
