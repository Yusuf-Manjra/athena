/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_Track_CaloEntr_H
#define MB_Track_CaloEntr_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MB_Track_MboyCyl.h"

  /**
   @class MB_Track_CaloEntr

   This class is the class to manage track data at Calo entrance

  @author samusog@cern.ch
  
  */

class MB_Track_CaloEntr: public MB_Track_MboyCyl {
public:
   MB_Track_CaloEntr();
  ~MB_Track_CaloEntr();

   void SetTrackCalo(
      double ThetaOfCrossingPoint,
       double PhiOfCrossingPoint,
        double ThetaOfUnitVector,
	 double PhiOfUnitVector,
	  double InverseMomentum,
      double Cov11, double Cov21, double Cov22,
      double Cov31, double Cov32, double Cov33,
      double Cov41, double Cov42, double Cov43,
      double Cov44, double Cov51, double Cov52,
      double Cov53, double Cov54, double Cov55,
      double XCrossingPoint ,double YCrossingPoint ,double ZCrossingPoint,
      double UXUnitVector ,double UYUnitVector ,double UZUnitVector,
      double Momentum,
      double ZHalfLengthOfCylinder,double RadiusOfCylinder
   );

};
#endif
