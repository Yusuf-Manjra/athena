//  Ratio2.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__Ratio2__
#define __TopoCore__Ratio2__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class Ratio2 : public DecisionAlg {
   public:
      Ratio2(const std::string & name);
      virtual ~Ratio2();

      virtual StatusCode initialize();

      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:
      parType_t      p_MinET1 = { 0 };
      parType_t      p_MinET2 = { 0 };
      parType_t      p_EtaMin = { 0 };
      parType_t      p_EtaMax = { 49 };
      parType_t      p_HT = { 0 };
      parType_t      p_Ratio2[2] = { 0 , 0 };

   };
   
}

#endif
