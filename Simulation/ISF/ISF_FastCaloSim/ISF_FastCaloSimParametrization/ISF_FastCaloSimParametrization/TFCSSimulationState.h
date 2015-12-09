/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSSimulationState_h
#define TFCSSimulationState_h

#include <TObject.h>
#include "ISF_FastCaloSimParametrization/FastCaloSim_CaloCell_ID.h"

class TFCSSimulationState:public TObject {
  public:
    TFCSSimulationState();
    
    bool   is_valid() const {return m_Ebin>=0;};
    double E() const {return m_Etot;};
    double E(int sample) const {return m_E[sample];};
    int    Ebin() const {return m_Ebin;};
    
    void set_Ebin(int bin) {m_Ebin=bin;};
    void add_E(int sample,double Esample) {m_E[sample]+=Esample;m_Etot+=Esample;};
    
    //empty function so far
    //not sure if we should keep the function here or rather write a deposit_cell function or similar
    virtual void deposit_HIT(int sample,double hit_eta,double hit_phi,double hit_weight);

    void clear();
  private:
    int    m_Ebin;
    double m_Etot;
    double m_E[CaloCell_ID_FCS::MaxSample];

  ClassDef(TFCSSimulationState,1)  //TFCSSimulationState
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSSimulationState;
#endif

#endif
