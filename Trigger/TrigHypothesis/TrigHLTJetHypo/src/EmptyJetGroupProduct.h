/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_EMPTYJETGROUPPRODUCT_H
#define TRIGHLTJETHYPO_EMPTYJETGROUPPRODUCT_H

#include "./IJetGroupProduct.h"
#include <vector>

class EmptyJetGroupProduct: public IJetGroupProduct{
  /*
   * Return an empty vector if jet indices
   */
public:
  EmptyJetGroupProduct(){}
  
  virtual std::vector<std::size_t> next(const Collector&collector) override {
    if(collector){
      collector->collect("EmptyJetGroup::next()",
                         "end of iteration ");
    }
    return std::vector<std::size_t>();
  }
  virtual bool valid() const override {return true;}
  
};

#endif
