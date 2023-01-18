/* emacs: this is -*- c++ -*- */
/**
 **     @file    SigAnalysis.h
 **
 **     @author  mark sutton
 **     @date    Wed 28 Oct 2009 02:41:51 CET 
 **
 **     Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAEXAMPLE_SIGANALYSIS_H
#define TIDAEXAMPLE_SIGANALYSIS_H

#include <iostream>
#include <vector>
#include <map>

#include "TrigInDetAnalysis/TrackAnalysis.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/Efficiency.h"
#include "TrigInDetAnalysis/TIDDirectory.h"

#include "TH1F.h"
#include "TH2F.h"


class SigAnalysis : public TrackAnalysis { 
   
public:

  using TrackAnalysis::execute;

public:
  
  SigAnalysis( const std::string& name ) : TrackAnalysis( name ), m_Nreco(0), m_print(true), m_dir(0) {
    std::cout << "SigAnalysis::SigAnalysis() " << TrackAnalysis::name() << std::endl;
  }  

  ~SigAnalysis() {
    std::cout << "SigAnalysis::~SigAnalysis() " << name() << std::endl;
    std::map<std::string, TH1F*>::iterator hitr=m_histos.begin();
    std::map<std::string, TH1F*>::iterator hend=m_histos.end();
    for ( ; hitr!=hend ; hitr++ ) delete hitr->second;     

    Efficiency* heff[4]    = {    m_eff_pt,    m_eff_eta,    m_eff_phi,    m_eff_z0 };
    Efficiency* hpurity[4] = { m_purity_pt, m_purity_eta, m_purity_phi, m_purity_z0 };

    for ( int i=4 ; i-- ; ) { delete heff[i]; delete hpurity[i]; } 

    delete m_eff_ptp;
    delete m_eff_ptm;

  }  
  
  

  virtual void initialise();

  virtual void execute(const std::vector<TIDA::Track*>& reftracks,
                       const std::vector<TIDA::Track*>& testtracks,
                       TrackAssociator* matcher );

  virtual void finalise();
  
  void setprint(bool p) { m_print=p; }

private:
  
  void addHistogram( TH1F* h ) { 
      std::string name = h->GetName();
      m_histos.insert( std::map<std::string, TH1F*>::value_type( name, h) );
  }
  
  TH1F* find( const std::string& n ) { 
      std::map<std::string, TH1F*>::iterator hmitr = m_histos.find(n);
      if ( hmitr!=m_histos.end() ) return hmitr->second;
      else                         return 0; 
  }

private:

  std::map<std::string, TH1F*> m_histos;
 
  Efficiency* m_eff_pt;
  Efficiency* m_eff_ptp;
  Efficiency* m_eff_ptm;

  Efficiency* m_eff_eta;
  Efficiency* m_eff_phi;
  Efficiency* m_eff_z0;
  Efficiency* m_eff_d0;
  Efficiency* m_eff_a0;

  Efficiency* m_purity_pt;
  Efficiency* m_purity_eta;
  Efficiency* m_purity_phi;
  Efficiency* m_purity_z0;
  Efficiency* m_purity_d0;
  Efficiency* m_purity_a0;


  TH2F* m_h2;
  TH2F* m_h2m;
  TH2F* m_h2r;

  /// number of reconstructed tracks 
  int m_Nreco;

  /// flag to print out the matched tracks etc
  bool m_print;

  TIDDirectory* m_dir;

};



inline std::ostream& operator<<( std::ostream& s, const SigAnalysis& /*_s*/ ) { 
  return s;
}





#endif  // TIDAEXAMPLE_SIGANALYSIS_H 










