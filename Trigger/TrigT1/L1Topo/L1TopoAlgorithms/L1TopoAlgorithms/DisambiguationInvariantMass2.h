//  DisambiguationInvariantMass2.h
//  TopoCore
//  Created by Davide Gerbaudo on 2016-10-10.
//  Copyright (c) 2016 Davide Gerbaudo. All rights reserved.

#ifndef __TopoCore__DisambiguationInvariantMass2__
#define __TopoCore__DisambiguationInvariantMass2__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {

  class DisambiguationInvariantMass2 : public DecisionAlg {
  public:
    DisambiguationInvariantMass2(const std::string & name);
    virtual ~DisambiguationInvariantMass2();
    virtual StatusCode initialize();
    virtual StatusCode processBitCorrect(const std::vector<TCS::TOBArray const *> & input,
                                         const std::vector<TCS::TOBArray *> & output,
                                         Decision & decison);
    virtual StatusCode process(const std::vector<TCS::TOBArray const *> & input,
                               const std::vector<TCS::TOBArray *> & output,
                               Decision & decison);
  private:
    parType_t  p_NumberLeading1 = { 0 };
    parType_t  p_NumberLeading2 = { 0 };
    parType_t  p_MinET1    [2] = { 0, 0 };
    parType_t  p_MinET2    [2] = { 0, 0 };
    parType_t  p_InvMassMin[2] = { 0, 0 };
    parType_t  p_InvMassMax[2] = { 0, 0 };
  };
}

#endif
