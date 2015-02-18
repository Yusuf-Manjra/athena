// emacs: this is -*- c++ -*-
//
//   @file    TrigJetSplitterAllTE.h        
//
//                   
//  
//   Copyright (C) 2014 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrigJetSplitterAllTE.h, v0.0   Tue 17 Jun 2014 03:26:44 CEST sutt $


#ifndef  TRIGJETSPLITTERALLTE_H
#define  TRIGJETSPLITTERALLTE_H

#include <iostream>

#include "TrigInterfaces/AllTEAlgo.h"


class TrigJetSplitterAllTE : public HLT::AllTEAlgo {

 public:
  TrigJetSplitterAllTE(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigJetSplitterAllTE();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  HLT::ErrorCode hltFinalize();

 private:

  std::string m_jetInputKey;
  std::string m_jetOutputKey;

  double m_etaHalfWidth;
  double m_phiHalfWidth;
  double m_zHalfWidth;

  double m_minJetEt; // is this needed still? 
                     // (at least allows 1-to-1 tests with TrigEFBjetSequenceAllTE while we keep it at 15)
                     // ((not really a 1-1 comparison though, because of differences in the way the chains are configured))

  double m_maxJetEta;

};
 

#endif


