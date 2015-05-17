/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//      2015-02-15 Author: Bertrand Laforge (LPNHE Paris)
//
/////////////////////////////////////////////////////////////

#ifndef photonMonTool_H
#define photonMonTool_H

#include "egammaPerformance/egammaMonToolBase.h"

class photonMonTool : public egammaMonToolBase
{
 public:
  
  photonMonTool(const std::string& type, const std::string& name, const IInterface* parent); 
  
  virtual ~photonMonTool();
  
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
     
 protected:

  // Properties
  std::string m_PhotonContainer;   // Container name for photons

  // Global panel histograms
  TH1 * m_hN;       // Histogram for number of photons
  TH1 * m_hEt;      // Histogram for photon transverse energies
  TH1 * m_hEta;     // Histogram for photon eta
  TH1 * m_hPhi;     // Histogram for photon phi
  TH2 * m_hEtaPhi;  // Histogram for photon eta,phi
  TH1 * m_hTopoEtCone40; // Histogram for calo based isolation energy
  TH1 * m_hPtCone20; // Histogram for track based isolation energy
  TH1 * m_hTime;         // Histogram for photon cluster time

  // Tight photon histograms
  TH1 * m_hTightN;       // Histogram for number of photons
  TH1 * m_hTightEt;      // Histogram for photon transverse energies
  TH1 * m_hTightEta;     // Histogram for photon eta
  TH1 * m_hTightPhi;     // Histogram for photon phi
  TH2 * m_hTightEtaPhi;  // Histogram for photon eta,phi
  TH1 * m_hTightTopoEtCone40; // Histogram for calo based isolation energy
  TH1 * m_hTightPtCone20; // Histogram for track based isolation energy

  std::vector<TH1*> m_hvTightNOfTRTHits;    
  std::vector<TH1*> m_hvTightConvType;
  std::vector<TH1*> m_hvTopoEtCone40;  // Histograms for photon calo-based isolation transverse energies
  std::vector<TH1*> m_hvPtCone20;  // Histograms for photon track-based isolation transverse energies
  std::vector<TH1*> m_hvTime;  // Histograms for photon times

  // 1D Histograms Loose photons
  std::vector<TH1*> m_hvN;   // Histograms for number of photons
  std::vector<TH1*> m_hvEt;  // Histograms for photon transverse energies
  std::vector<TH1*> m_hvEta; // Histograms for photon transverse eta
  std::vector<TH1*> m_hvPhi; // Histograms for photon transverse phi

  // photon ID histograms
  std::vector<TH1*> m_hvEhad1;         
  std::vector<TH1*> m_hvCoreEM;         
  std::vector<TH1*> m_hvF1;
  std::vector<TH1*> m_hvF2;
  std::vector<TH1*> m_hvF3;
  std::vector<TH1*> m_hvRe233e237;
  std::vector<TH1*> m_hvRe237e277;
  std::vector<TH1*> m_hvNOfTRTHits;    
  std::vector<TH1*> m_hvConvType;
  std::vector<TH1*> m_hvConvTrkMatch1;
  std::vector<TH1*> m_hvConvTrkMatch2;

  // 1D Histograms Tight photons
  std::vector<TH1*> m_hvTightN;   // Histograms for number of photons
  std::vector<TH1*> m_hvTightEt;  // Histograms for photon transverse energies
  std::vector<TH1*> m_hvTightEta; // Histograms for photon transverse eta
  std::vector<TH1*> m_hvTightPhi; // Histograms for photon transverse phi

  std::vector<TH1*> m_hvTightConvTrkMatch1;
  std::vector<TH1*> m_hvTightConvTrkMatch2;
};

#endif
