/* emacs: this is -*- c++ -*- */
/**
 **     @file    Analysis_Fake.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:38 CET 
 **
 **     Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisExample_Analysis_Fake_H
#define TrigInDetAnalysisExample_Analysis_Fake_H


#include <iostream>

#include "TH1.h"

#include "TrigInDetAnalysis/TrackAnalysis.h"


class Analysis_Fake : public TrackAnalysis { 

 public:

  using TrackAnalysis::execute;
  
 public:
  
  Analysis_Fake(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut);
  
  virtual void initialise();
  
  virtual void execute(const std::vector<TIDA::Track*>& referenceTracks,
		       const std::vector<TIDA::Track*>& testTracks,
		       TrackAssociator* associator);
  
  virtual void finalise();
  
 private:

  double phi(double p);

 private:
  
  // Integral efficiency histograms

  TH1* m_h_integralD;
  TH1* m_h_integralN;
  TH1* m_h_integral;
    
  // Differential efficiency histograms
  TH1* m_h_etaD;
  TH1* m_h_phiD;
  TH1* m_h_z0D;
  TH1* m_h_d0D;
  TH1* m_h_pTD;
  TH1* m_h_etaN;
  TH1* m_h_phiN;
  TH1* m_h_z0N;
  TH1* m_h_d0N;
  TH1* m_h_pTN;
  TH1* m_h_eta;
  TH1* m_h_phi;
  TH1* m_h_z0;
  TH1* m_h_d0;
  TH1* m_h_pT;

  // Efficiency plateau cuts
  double m_pTCut;
  double m_etaCut;
  double m_d0Cut;
  double m_z0Cut;

};


#endif // TrigInDetAnalysisExample_Analysis_Fake_H
