/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUHISTUTILS_HAD3PRONGPLOTS_H
#define TAUHISTUTILS_HAD3PRONGPLOTS_H

#include "GeneralTauPlots.h"
#include "xAODTau/TauJet.h"

namespace Tau{

class Had3ProngPlots: public PlotBase {
  public:
    Had3ProngPlots(PlotBase *pParent, std::string sDir, std::string sTauJetContainerName);

    void fill(const xAOD::TauJet& tau);

    Tau::GeneralTauPlots m_oGeneralTauPlots;
	  TH1* m_tauCoreFrac;
	  TH1* m_tauEoverPTrk;
	  TH1* m_tauTrkAvgDist;
	  TH1* m_tauDRMax;
	  TH1* m_tauSflight;
	  TH1* m_tauMtrks;
		
  private:
    void initializePlots();
    std::string m_sTauJetContainerName;
};

}

#endif
