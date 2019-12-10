/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSLateralShapeParametrizationFluctChain_h
#define TFCSLateralShapeParametrizationFluctChain_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitChain.h"

class TFCSLateralShapeParametrizationFluctChain:public TFCSLateralShapeParametrizationHitChain {
public:
  TFCSLateralShapeParametrizationFluctChain(const char* name=nullptr, const char* title=nullptr, float RMS=1.0);
  TFCSLateralShapeParametrizationFluctChain(TFCSLateralShapeParametrizationHitBase* hitsim);

  // set and get the amount of hit energy fluctation around E/n for n hits
  void set_hit_RMS(float RMS);
  float get_hit_RMS() const {return m_RMS;};

  virtual FCSReturnCode simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const override;

  void Print(Option_t *option) const override;
private:
  float m_RMS;
  
  ClassDefOverride(TFCSLateralShapeParametrizationFluctChain,1)  //TFCSLateralShapeParametrizationFluctChain
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSLateralShapeParametrizationFluctChain+;
#endif

#endif
