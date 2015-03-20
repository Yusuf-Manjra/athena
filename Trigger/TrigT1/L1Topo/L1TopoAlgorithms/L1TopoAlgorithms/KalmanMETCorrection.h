//  KalmanMETCorrection.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__KalmanMETCorrection__
#define __TopoCore__KalmanMETCorrection__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class KalmanMETCorrection : public DecisionAlg {
   public:
      KalmanMETCorrection(const std::string & name);
      virtual ~KalmanMETCorrection();

      virtual StatusCode initialize();

      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_NumberLeading2 = { 0 };
      parType_t      p_XE[6] = {0, 0, 0,0,0,0};
      parType_t      p_MinEt[6] = { 0 ,0,0,0,0,0};

   };
   
}

#endif
